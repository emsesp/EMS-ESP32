#include <UploadFileService.h>
#include <esp_ota_ops.h>
#include <esp_app_format.h>

#include "../../src/emsesp_stub.hpp"

using namespace std::placeholders; // for `_1` etc

static char md5[33] = "\0";

static FileType fileType = ft_none;

UploadFileService::UploadFileService(PsychicHttpServer * server, SecurityManager * securityManager)
    : _server(server)
    , _securityManager(securityManager) {
}

void UploadFileService::registerURI() {
    _server->maxUploadSize = 2300000; // 2.3 MB

    PsychicUploadHandler * uploadHandler = new PsychicUploadHandler();

    uploadHandler->onUpload(std::bind(&UploadFileService::handleUpload, this, _1, _2, _3, _4, _5, _6));
    uploadHandler->onRequest(std::bind(&UploadFileService::uploadComplete, this, _1)); //gets called after upload has been handled
    _server->on(UPLOAD_FILE_PATH, HTTP_POST, uploadHandler);
}

esp_err_t UploadFileService::handleUpload(PsychicRequest * request, const String & filename, uint64_t index, uint8_t * data, size_t len, bool final) {
    // quit if not authorized
    Authentication authentication = _securityManager->authenticateRequest(request);
    if (!AuthenticationPredicates::IS_ADMIN(authentication)) {
        return handleError(request, 403); // forbidden
    }

    File jsonFile;

    // at init
    if (!index) {
        // check details of the file, to see if its a valid bin or json file
        std::string fname(filename.c_str());
        auto        position  = fname.find_last_of(".");
        std::string extension = fname.substr(position + 1);
        size_t      fsize     = request->contentLength();

        fileType = ft_none;
        if ((extension == "bin") && (fsize > 1000000)) {
            fileType = ft_firmware;
        } else if (extension == "json") {
            fileType = ft_json;
            md5[0]   = '\0'; // clear md5
        } else if (extension == "md5") {
            fileType = ft_md5;
            if (len == 32) {
                memcpy(md5, data, 32);
                md5[32] = '\0';
            }
            return ESP_OK;
        } else {
            md5[0] = '\0';
            return handleError(request, 406); // Not Acceptable - unsupported file type
        }

        if (fileType == ft_firmware) {
            // Check firmware header, 0xE9 magic offset 0 indicates esp bin, chip offset 12: esp32:0, S2:2, C3:5
#if CONFIG_IDF_TARGET_ESP32 // ESP32/PICO-D4
            if (len > 12 && (data[0] != 0xE9 || data[12] != 0)) {
                return handleError(request, 503); // service unavailable
            }
#elif CONFIG_IDF_TARGET_ESP32S2
            if (len > 12 && (data[0] != 0xE9 || data[12] != 2)) {
                return handleError(request, 503); // service unavailable
            }
#elif CONFIG_IDF_TARGET_ESP32C3
            if (len > 12 && (data[0] != 0xE9 || data[12] != 5)) {
                return handleError(request, 503); // service unavailable
            }
#elif CONFIG_IDF_TARGET_ESP32S3
            if (len > 12 && (data[0] != 0xE9 || data[12] != 9)) {
                return handleError(request, 503); // service unavailable
            }
#endif
            // it's firmware - initialize the ArduinoOTA updater
            if (Update.begin(fsize - sizeof(esp_image_header_t))) {
                if (strlen(md5) == 32) {
                    Update.setMD5(md5);
                    md5[0] = '\0';
                }
            } else {
                return handleError(request, 507); // failed to begin, send an error response Insufficient Storage
            }
        } else {
            // its a normal file, open a new temp file to write the contents too
            jsonFile = LittleFS.open(TEMP_FILENAME_PATH, FILE_WRITE);
        }
    }

    if (fileType == ft_json) {
        if (len) {
            if (len != jsonFile.write(data, len)) { // stream the incoming chunk to the opened file
                return handleError(request, 507);   // failed to write chunk to file, send an error response Insufficient Storage
            }
        }
    } else {
        // if we haven't delt with an error, continue with the firmware update
        if (!request->_tempObject) {
            if (Update.write(data, len) != len) {
                handleError(request, 500);
            }
            if (final) {
                if (!Update.end(true)) {
                    handleError(request, 500);
                }
            }
        }
    }

    return ESP_OK;
}

esp_err_t UploadFileService::uploadComplete(PsychicRequest * request) {
    // did we complete uploading a json file? no need to close the file
    if (fileType == ft_md5) {
        if (strlen(md5) == 32) {
            PsychicJsonResponse response = PsychicJsonResponse(request, false, 256);
            JsonObject          root     = response.getRoot();
            root["md5"]                  = md5;
            return response.send();
        }
        return ESP_OK;
    }

    // store and restart regardless of whether it worked or not
    emsesp::EMSESP::system_.store_nvs_values();
    request->reply(200);
    RestartService::restartNow();
    return ESP_OK;
}

esp_err_t UploadFileService::handleError(PsychicRequest * request, int code) {
    // if we have had an error already, do nothing
    if (request->_tempObject) {
        return ESP_OK;
    }

    // check for invalid extension and immediately kill the connection, which will through an error
    // that is caught by the web code. Unfortunately the http error code is not sent to the client on fast network connections
    if (code == 406) {
        request->client()->close();
        fileType = ft_none;
        Update.abort();
    }

    return request->reply(code);
}
