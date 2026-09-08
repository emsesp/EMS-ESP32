#include "UploadFileService.h"

#include <emsesp.h>

#include <esp_app_format.h>
#include <esp_ota_ops.h>

static String getFilenameExtension(const String & filename) {
    const auto pos = filename.lastIndexOf('.');
    if (pos != -1) {
        return filename.substring(static_cast<unsigned int>(pos) + 1);
    }
    return {};
}

// Accepts a raw 32-char hex digest, optionally with trailing newline or a GNU "hash  filename" suffix.
static bool parseMd5Digest(const uint8_t * data, size_t len, std::array<char, 33> & out) {
    size_t i = 0;
    while (i < len && (data[i] == ' ' || data[i] == '\t' || data[i] == '\r' || data[i] == '\n')) {
        ++i;
    }
    if (len - i < 32) {
        return false;
    }
    for (size_t n = 0; n < 32; n++) {
        const char c   = static_cast<char>(data[i + n]);
        const bool hex = (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
        if (!hex) {
            return false;
        }
        out[n] = (c >= 'A' && c <= 'F') ? static_cast<char>(c - 'A' + 'a') : c;
    }
    out[32] = '\0';
    return true;
}

UploadFileService::UploadFileService(AsyncWebServer * server, SecurityManager * securityManager)
    : _securityManager(securityManager)
    , _is_firmware(false)
    , _is_filesystem(false)
    , _md5_applied(false)
    , _md5() {
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

        _is_firmware   = false;
        _is_filesystem = false;

        if (extension == "bin" && filename.endsWith("littlefs.bin")) {
            // LittleFS filesystem image
            _is_filesystem = true;
            _md5[0]        = '\0'; // clear any stale md5 so Update.end() doesn't compare against it
            _md5_applied   = false;
        } else if ((extension == "bin") && (filesize > 1000000)) {
            _is_firmware = true;
        } else if (extension == "json") {
            _md5[0]      = '\0'; // clear md5
            _md5_applied = false;
        } else if (extension == "md5") {
            if (!parseMd5Digest(data, len, _md5)) {
                emsesp::EMSESP::logger().err("Invalid MD5 digest file");
                handleError(request, 406); // Not Acceptable
            } else {
                emsesp::EMSESP::logger().info("MD5 digest received (%s). Now upload the firmware BIN", _md5.data());
            }
            return;
        } else {
            _md5.front() = '\0';
            _md5_applied = false;
            emsesp::EMSESP::logger().err("Unsupported file type: %s, size: %u", filename.c_str(), filesize);
            handleError(request, 406); // Not Acceptable - unsupported file type
            return;
        }

        if (_is_firmware) {
            // Check firmware header, 0xE9 magic offset 0 indicates esp bin, chip offset 12: esp32:0, S2:2, C3:5
#if CONFIG_IDF_TARGET_ESP32 // ESP32/PICO-D4
            if (len > 12 && (data[0] != ESP_IMAGE_HEADER_MAGIC || data[12] != ESP_CHIP_ID_ESP32)) {
                handleError(request, 503); // service unavailable
                return;
            }
#elif CONFIG_IDF_TARGET_ESP32S2
            if (len > 12 && (data[0] != ESP_IMAGE_HEADER_MAGIC || data[12] != ESP_CHIP_ID_ESP32S2)) {
                handleError(request, 503); // service unavailable
                return;
            }
#elif CONFIG_IDF_TARGET_ESP32C3
            if (len > 12 && (data[0] != ESP_IMAGE_HEADER_MAGIC || data[12] != ESP_CHIP_ID_ESP32C3)) {
                handleError(request, 503); // service unavailable
                return;
            }
#elif CONFIG_IDF_TARGET_ESP32S3
            if (len > 12 && (data[0] != ESP_IMAGE_HEADER_MAGIC || data[12] != ESP_CHIP_ID_ESP32S3)) {
                handleError(request, 503); // service unavailable
                return;
            }
#elif CONFIG_IDF_TARGET_ESP32C6
            if (len > 12 && (data[0] != ESP_IMAGE_HEADER_MAGIC || data[12] != ESP_CHIP_ID_ESP32C6)) {
                handleError(request, 503); // service unavailable
                return;
            }
#endif
            // it's firmware - initialize the ArduinoOTA updater
            emsesp::EMSESP::logger().info("Firmware uploading (file %s, size: %dKB). Please wait...", filename.c_str(), filesize / 1024);

            // turn off UART to prevent interference with the upload
            emsesp::EMSuart::stop();

            // tell main loop we're uploading so services as paused (e.g. MQTT)
            emsesp::EMSESP::system_.systemStatus(emsesp::SYSTEM_STATUS::SYSTEM_STATUS_UPLOADING);

            if (Update.begin(filesize - sizeof(esp_image_header_t))) {
                _md5_applied = false;
                if (strlen(_md5.data()) == _md5.size() - 1) {
                    Update.setMD5(_md5.data());
                    _md5_applied = true;
                    emsesp::EMSESP::logger().info("Firmware MD5 check enabled (%s)", _md5.data());
                }
                request->onDisconnect([this] { handleDisconnect(); }); // success, let's make sure we end the update if the client hangs up
            } else {
                handleError(request, 507); // failed to begin, send an error response Insufficient Storage
                return;
            }
        } else if (_is_filesystem) {
            // LittleFS filesystem image - flash directly to the spiffs/littlefs partition
            emsesp::EMSESP::logger().info("Uploading filesystem file %s (size: %uKB). Please wait...", filename.c_str(), static_cast<unsigned>(filesize / 1024));
            emsesp::EMSuart::stop();
            LittleFS.end(); // unmount LittleFS before we overwrite the partition under it

            // request->contentLength() is the multipart HTTP body size, not the file size,
            // so it can exceed the partition by a few hundred bytes. Use UPDATE_SIZE_UNKNOWN
            // and let the Update library size against the whole partition.
            if (Update.begin(UPDATE_SIZE_UNKNOWN, U_SPIFFS)) {
                // emsesp::EMSESP::logger().info("Update.begin(U_SPIFFS) ok, partition size %u bytes", static_cast<unsigned>(Update.size()));
                request->onDisconnect([this] { handleDisconnect(); });
            } else {
                emsesp::EMSESP::logger().err("Update.begin(U_SPIFFS) failed: %s", Update.errorString());
                handleError(request, 507);
                return;
            }
        } else {
            // its a normal file, open a new temp file to write the contents too
            request->_tempFile = LittleFS.open(TEMP_FILENAME_PATH, "w");
        }
    }

    if (_is_firmware || _is_filesystem) {
        if (!request->_tempObject) {
            //continue with the OTA update
            if (Update.write(data, len) != len) {
                emsesp::EMSESP::logger().err("OTA update failed at offset %u (chunk %u): %s",
                                             static_cast<unsigned>(Update.progress()),
                                             static_cast<unsigned>(len),
                                             Update.errorString());
                handleError(request, 500); // internal error, failed
                return;
            }
            if (final) {
                if (!Update.end(true)) {
                    emsesp::EMSESP::logger().err("OTA update failed: %s", Update.errorString());
                    handleError(request, 500); // internal error, failed
                    return;
                }
            }
        }
    } else {
        // stream the incoming chunk to the opened file
        if (len && len != request->_tempFile.write(data, len)) {
            handleError(request, 507); // 507-Insufficient Storage
        }
    }
}

void UploadFileService::uploadComplete(AsyncWebServerRequest * request) {
    emsesp::EMSESP::logger().info("Upload successful");

    // did we just complete uploading a json file?
    if (request->_tempFile) {
        request->_tempFile.close(); // close the file handle as the upload is now done
        AsyncWebServerResponse * response = request->beginResponse(200);
        request->send(response);
        emsesp::EMSESP::system_.systemStatus(
            emsesp::SYSTEM_STATUS::SYSTEM_STATUS_PENDING_RESTART); // will be handled by the main loop. We use pending for the Web's SystemMonitor
        return;
    }

    // check if it was a firmware or filesystem image upgrade
    // if no error, send the success response and request a restart
    if ((_is_firmware || _is_filesystem) && !request->_tempObject) {
        if (_is_firmware) {
            // set NVS to tell EMS-ESP this is a new fresh firmware on next restart
            emsesp::EMSESP::nvs_.putBool(emsesp::EMSESP_NVS_BOOT_NEW_FIRMWARE, true);
        }

        if (_is_firmware && _md5_applied) {
            emsesp::EMSESP::logger().info("Firmware MD5 matches");
            auto *     response = new emsesp::PsramAsyncJsonResponse(false);
            JsonObject root     = response->getRoot();
            root["md5_ok"]      = true;
            response->setLength();
            request->send(response);
            _md5.front() = '\0';
            _md5_applied = false;
        } else {
            AsyncWebServerResponse * response = request->beginResponse(200);
            request->send(response);
        }
        emsesp::EMSESP::system_.systemStatus(
            emsesp::SYSTEM_STATUS::SYSTEM_STATUS_PENDING_RESTART); // will be handled by the main loop. We use pending for the Web's SystemMonitor
        return;
    }

    // add MD5 to the response
    if (strlen(_md5.data()) == _md5.size() - 1) {
        auto *     response = new emsesp::PsramAsyncJsonResponse(false);
        JsonObject root     = response->getRoot();
        root["md5"]         = _md5.data();
        response->setLength();
        request->send(response);
        return;
    }

    handleError(request, 500);
}

void UploadFileService::handleError(AsyncWebServerRequest * request, int code) {
    emsesp::EMSESP::logger().info("Upload error: %d", code);
    emsesp::EMSESP::system_.uart_init(); // re-enable UART

    // if we have had an error already, do nothing
    if (request->_tempObject) {
        return;
    }

    // send the error code to the client and record the error code in the temp object
    AsyncWebServerResponse * response = request->beginResponse(code);
    request->send(response);

    // check for invalid extension and immediately kill the connection, which will throw an error
    // that is caught by the web code. Unfortunately the http error code is not sent to the client on fast network connections
    if (code == 406) {
        request->client()->close();
        _is_firmware   = false;
        _is_filesystem = false;
        Update.abort();
    }

    // if we aborted a filesystem upload, remount LittleFS so the device keeps working
    if (_is_filesystem) {
        LittleFS.begin();
    }
}

void UploadFileService::handleDisconnect() {
    emsesp::EMSESP::logger().info("Upload finished");
    emsesp::EMSESP::system_.uart_init(); // re-enable UART

    _is_firmware   = false;
    _is_filesystem = false;
}
