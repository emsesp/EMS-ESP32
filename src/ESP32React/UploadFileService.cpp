#include "UploadFileService.h"

#include <emsesp.h>

#include <esp_app_format.h>

static String getFilenameExtension(const String & filename) {
    const auto pos = filename.lastIndexOf('.');
    if (pos != -1) {
        return filename.substring(static_cast<unsigned int>(pos) + 1);
    }
    return {};
}

UploadFileService::UploadFileService(AsyncWebServer * server, SecurityManager * securityManager)
    : _securityManager(securityManager)
    , _is_firmware(false)
    , _md5() {
    // upload a file via a form
    server->on(
        UPLOAD_FILE_PATH,
        HTTP_POST,
        [this](AsyncWebServerRequest * request) { uploadComplete(request); },
        [this](AsyncWebServerRequest * request, const String & filename, size_t index, uint8_t * data, size_t len, bool final) {
            handleUpload(request, filename, index, data, len, final);
        });
}

void UploadFileService::handleUpload(AsyncWebServerRequest * request, const String & filename, size_t index, uint8_t * data, size_t len, bool final) {
    // quit if not authorized
    Authentication authentication = _securityManager->authenticateRequest(request);
    if (!AuthenticationPredicates::IS_ADMIN(authentication)) {
        handleError(request, 403); // send the forbidden response
        return;
    }

    // at init
    if (!index) {
        // check details of the file, to see if its a valid bin or json file
        const String      extension = getFilenameExtension(filename);
        const std::size_t filesize  = request->contentLength();

        _is_firmware = false;
        if ((extension == "bin") && (filesize > 1000000)) {
            _is_firmware = true;
        } else if (extension == "json") {
            _md5[0] = '\0'; // clear md5
        } else if (extension == "md5") {
            if (len == _md5.size() - 1) {
                std::memcpy(_md5.data(), data, _md5.size() - 1);
                _md5.back() = '\0';
            }
            return;
        } else {
            _md5.front() = '\0';
            handleError(request, 406); // Not Acceptable - unsupported file type
            return;
        }

        if (_is_firmware) {
            // Check firmware header, 0xE9 magic offset 0 indicates esp bin, chip offset 12: esp32:0, S2:2, C3:5
#if CONFIG_IDF_TARGET_ESP32 // ESP32/PICO-D4
            if (len > 12 && (data[0] != 0xE9 || data[12] != 0)) {
                handleError(request, 503); // service unavailable
                return;
            }
#elif CONFIG_IDF_TARGET_ESP32S2
            if (len > 12 && (data[0] != 0xE9 || data[12] != 2)) {
                handleError(request, 503); // service unavailable
                return;
            }
#elif CONFIG_IDF_TARGET_ESP32C3
            if (len > 12 && (data[0] != 0xE9 || data[12] != 5)) {
                handleError(request, 503); // service unavailable
                return;
            }
#elif CONFIG_IDF_TARGET_ESP32S3
            if (len > 12 && (data[0] != 0xE9 || data[12] != 9)) {
                handleError(request, 503); // service unavailable
                return;
            }
#endif
            // it's firmware - initialize the ArduinoOTA updater
            if (Update.begin(filesize - sizeof(esp_image_header_t))) {
                if (strlen(_md5.data()) == _md5.size() - 1) {
                    Update.setMD5(_md5.data());
                    _md5.front() = '\0';
                }
                request->onDisconnect([this] { handleEarlyDisconnect(); }); // success, let's make sure we end the update if the client hangs up
            } else {
                handleError(request, 507); // failed to begin, send an error response Insufficient Storage
                return;
            }
        } else {
            // its a normal file, open a new temp file to write the contents too
            request->_tempFile = LittleFS.open(TEMP_FILENAME_PATH, "w");
        }
    }

    if (!_is_firmware) {
        if (len && len != request->_tempFile.write(data, len)) { // stream the incoming chunk to the opened file
            handleError(request, 507);                           // 507-Insufficient Storage
        }
    } else if (!request->_tempObject) { // if we haven't delt with an error, continue with the firmware update
        if (Update.write(data, len) != len) {
            handleError(request, 500); // internal error, failed
            return;
        }
        if (final && !Update.end(true)) {
            handleError(request, 500); // internal error, failed
        }
    }
}

void UploadFileService::uploadComplete(AsyncWebServerRequest * request) {
    // did we just complete uploading a json file?
    if (request->_tempFile) {
        request->_tempFile.close(); // close the file handle as the upload is now done
        AsyncWebServerResponse * response = request->beginResponse(200);
        request->send(response);
        emsesp::EMSESP::system_.systemStatus(
            emsesp::SYSTEM_STATUS::SYSTEM_STATUS_PENDING_RESTART); // will be handled by the main loop. We use pending for the Web's SystemMonitor
        return;
    }

    // check if it was a firmware upgrade
    // if no error, send the success response as a JSON
    if (_is_firmware && !request->_tempObject) {
        AsyncWebServerResponse * response = request->beginResponse(200);
        request->send(response);
        emsesp::EMSESP::system_.systemStatus(
            emsesp::SYSTEM_STATUS::SYSTEM_STATUS_PENDING_RESTART); // will be handled by the main loop. We use pending for the Web's SystemMonitor
        return;
    }

    if (strlen(_md5.data()) == _md5.size() - 1) {
        auto *     response = new AsyncJsonResponse(false);
        JsonObject root     = response->getRoot();
        root["md5"]         = _md5.data();
        response->setLength();
        request->send(response);
        return;
    }

    handleError(request, 500);
}

void UploadFileService::handleError(AsyncWebServerRequest * request, int code) {
    // if we have had an error already, do nothing
    if (request->_tempObject) {
        return;
    }

    // send the error code to the client and record the error code in the temp object
    AsyncWebServerResponse * response = request->beginResponse(code);
    request->send(response);

    // check for invalid extension and immediately kill the connection, which will through an error
    // that is caught by the web code. Unfortunately the http error code is not sent to the client on fast network connections
    if (code == 406) {
        request->client()->close(true);
        handleEarlyDisconnect();
    }
}

void UploadFileService::handleEarlyDisconnect() {
    _is_firmware = false;
    Update.abort();
}
