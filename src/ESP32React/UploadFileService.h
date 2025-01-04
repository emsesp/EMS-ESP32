#ifndef UploadFileService_h
#define UploadFileService_h

#include "SecurityManager.h"

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <Update.h>
#include <WiFi.h>

#include <array>

#define UPLOAD_FILE_PATH "/rest/uploadFile"

#define TEMP_FILENAME_PATH "/pre_load.json" // for uploaded json files, handled by System::check_restore()

class UploadFileService {
  public:
    UploadFileService(AsyncWebServer * server, SecurityManager * securityManager);

  private:
    SecurityManager *    _securityManager;
    bool                 _is_firmware;
    std::array<char, 33> _md5;

    void handleUpload(AsyncWebServerRequest * request, const String & filename, size_t index, uint8_t * data, size_t len, bool final);
    void uploadComplete(AsyncWebServerRequest * request);
    void handleError(AsyncWebServerRequest * request, int code);

    void handleEarlyDisconnect();
};

#endif