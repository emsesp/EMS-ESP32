#include "UploadFileService.h"

#include <emsesp.h>

#include <esp_ota_ops.h>

UploadFileService::UploadFileService(AsyncWebServer * server, SecurityManager * securityManager)
    : _securityManager(securityManager)
    , _is_firmware(false)
    , _is_filesystem(false)
    , _is_recovery(false) {
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
        const std::size_t filesize = request->contentLength();

        _is_firmware   = false;
        _is_filesystem = false;
        _is_recovery   = false;

        switch (OtaUpdater::classify(filename, filesize, emsesp::System::MIN_FIRMWARE_SIZE)) {
        case OtaUpdater::FileType::FILESYSTEM:
            _is_filesystem = true;
            _ota.clearMd5(); // clear any stale md5 so Update.end() doesn't compare against it
            break;
        case OtaUpdater::FileType::FIRMWARE:
            _is_firmware = true;
            break;
        case OtaUpdater::FileType::JSON:
            _ota.clearMd5();
            break;
        case OtaUpdater::FileType::MD5:
            if (!_ota.setMd5(data, len)) {
                emsesp::EMSESP::logger().err("Invalid MD5 digest file");
                handleError(request, 406); // Not Acceptable
            } else {
                emsesp::EMSESP::logger().info("MD5 digest received (%s). Now upload the firmware BIN", _ota.md5());
            }
            return;
        default:
            _ota.clearMd5();
            emsesp::EMSESP::logger().err("Unsupported file type: %s, size: %u", filename.c_str(), filesize);
            handleError(request, 406); // Not Acceptable - unsupported file type
            return;
        }

        if (_is_firmware) {
            if (!OtaUpdater::isCompatibleFirmware(data, len)) {
                handleError(request, 503); // service unavailable
                return;
            }

            const esp_partition_t * target = nullptr;
#ifdef EMSESP_HAS_RECOVERY
            if (OtaUpdater::isRecoveryFirmware(data, len)) {
                target = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, nullptr);
                if (target == nullptr || target == esp_ota_get_running_partition()) {
                    emsesp::EMSESP::logger().err("The recovery firmware can't be installed on this board");
                    handleError(request, 406);
                    return;
                }
                _is_recovery = true;
                emsesp::EMSESP::logger().info("Recovery firmware uploading to the %s partition (size: %dKB). Please wait...", target->label, filesize / 1024);
            } else
#endif
            {
                emsesp::EMSESP::logger().info("Firmware uploading (file %s, size: %dKB). Please wait...", filename.c_str(), filesize / 1024);
            }

            // turn off UART to prevent interference with the upload
            emsesp::EMSuart::stop();

            // tell main loop we're uploading so services as paused (e.g. MQTT)
            emsesp::EMSESP::system_.systemStatus(emsesp::SYSTEM_STATUS::SYSTEM_STATUS_UPLOADING);

            if (_ota.beginFirmware(filesize, target, !_is_recovery)) {
                if (_ota.md5Applied()) {
                    emsesp::EMSESP::logger().info("Firmware MD5 check enabled (%s)", _ota.md5());
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

            if (_ota.beginFilesystem()) {
                request->onDisconnect([this] { handleDisconnect(); });
            } else {
                emsesp::EMSESP::logger().err("Update.begin(U_SPIFFS) failed: %s", _ota.errorString());
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
            if (!_ota.write(data, len)) {
                emsesp::EMSESP::logger().err("OTA update failed at offset %u (chunk %u): %s",
                                             static_cast<unsigned>(_ota.progress()),
                                             static_cast<unsigned>(len),
                                             _ota.errorString());
                _ota.abort();
                handleError(request, 500); // internal error, failed
                return;
            }
            if (final) {
                if (!_ota.end()) {
                    emsesp::EMSESP::logger().err("OTA update failed: %s", _ota.errorString());
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
#ifdef EMSESP_HAS_RECOVERY
    if (_is_recovery && !request->_tempObject) {
        emsesp::EMSESP::system_.recovery_installed();
        auto *     response = new emsesp::PsramAsyncJsonResponse(false);
        JsonObject root     = response->getRoot();
        root["recovery"]    = true;
        if (_ota.md5Applied()) {
            emsesp::EMSESP::logger().info("Firmware MD5 matches");
            root["md5_ok"] = true;
        }
        response->setLength();
        request->send(response);
        _ota.clearMd5();
        _is_recovery = false;
        emsesp::EMSESP::system_.systemStatus(emsesp::SYSTEM_STATUS::SYSTEM_STATUS_NORMAL); // no restart needed
        return;
    }
#endif

    if ((_is_firmware || _is_filesystem) && !request->_tempObject) {
        if (_is_firmware) {
            // set NVS to tell EMS-ESP this is a new fresh firmware on next restart
            emsesp::EMSESP::nvs_.putBool(emsesp::EMSESP_NVS_BOOT_NEW_FIRMWARE, true);
        }

        if (_is_firmware && _ota.md5Applied()) {
            emsesp::EMSESP::logger().info("Firmware MD5 matches");
            auto *     response = new emsesp::PsramAsyncJsonResponse(false);
            JsonObject root     = response->getRoot();
            root["md5_ok"]      = true;
            response->setLength();
            request->send(response);
            _ota.clearMd5();
        } else {
            AsyncWebServerResponse * response = request->beginResponse(200);
            request->send(response);
        }
        emsesp::EMSESP::system_.systemStatus(
            emsesp::SYSTEM_STATUS::SYSTEM_STATUS_PENDING_RESTART); // will be handled by the main loop. We use pending for the Web's SystemMonitor
        return;
    }

    // add MD5 to the response
    if (_ota.hasMd5()) {
        auto *     response = new emsesp::PsramAsyncJsonResponse(false);
        JsonObject root     = response->getRoot();
        root["md5"]         = _ota.md5();
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
        _is_recovery   = false;
        _ota.abort();
    }

    // if we aborted a filesystem upload, remount LittleFS so the device keeps working
    if (_is_filesystem) {
        LittleFS.begin();
    }
}

void UploadFileService::handleDisconnect() {
    emsesp::EMSESP::logger().info("Upload finished");
    emsesp::EMSESP::system_.uart_init(); // re-enable UART

#ifdef EMSESP_HAS_RECOVERY
    if (_is_recovery) {
        // the client hung up before the upload completed
        _ota.abort();
        emsesp::EMSESP::system_.systemStatus(emsesp::SYSTEM_STATUS::SYSTEM_STATUS_NORMAL);
    }
#endif

    _is_firmware   = false;
    _is_filesystem = false;
    _is_recovery   = false;
}
