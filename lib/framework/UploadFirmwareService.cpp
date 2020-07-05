#include <UploadFirmwareService.h>

UploadFirmwareService::UploadFirmwareService(AsyncWebServer* server, SecurityManager* securityManager) :
    _securityManager(securityManager) {
  server->on(UPLOAD_FIRMWARE_PATH,
             HTTP_POST,
             std::bind(&UploadFirmwareService::uploadComplete, this, std::placeholders::_1),
             std::bind(&UploadFirmwareService::handleUpload,
                       this,
                       std::placeholders::_1,
                       std::placeholders::_2,
                       std::placeholders::_3,
                       std::placeholders::_4,
                       std::placeholders::_5,
                       std::placeholders::_6));
#ifdef ESP8266
  Update.runAsync(true);
#endif
}

void UploadFirmwareService::handleUpload(AsyncWebServerRequest* request,
                                         const String& filename,
                                         size_t index,
                                         uint8_t* data,
                                         size_t len,
                                         bool final) {
  if (!index) {
    Authentication authentication = _securityManager->authenticateRequest(request);
    if (AuthenticationPredicates::IS_ADMIN(authentication)) {
      if (Update.begin(request->contentLength())) {
        // success, let's make sure we end the update if the client hangs up
        request->onDisconnect(UploadFirmwareService::handleEarlyDisconnect);
      } else {
        // failed to begin, send an error response
        Update.printError(Serial);
        handleError(request, 500);
      }
    } else {
      // send the forbidden response
      handleError(request, 403);
    }
  }

  // if we haven't delt with an error, continue with the update
  if (!request->_tempObject) {
    if (Update.write(data, len) != len) {
      Update.printError(Serial);
      handleError(request, 500);
    }
    if (final) {
      if (!Update.end(true)) {
        Update.printError(Serial);
        handleError(request, 500);
      }
    }
  }
}

void UploadFirmwareService::uploadComplete(AsyncWebServerRequest* request) {
  // if no error, send the success response
  if (!request->_tempObject) {
    request->onDisconnect(RestartService::restartNow);
    AsyncWebServerResponse* response = request->beginResponse(200);
    request->send(response);
  }
}

void UploadFirmwareService::handleError(AsyncWebServerRequest* request, int code) {
  // if we have had an error already, do nothing
  if (request->_tempObject) {
    return;
  }
  // send the error code to the client and record the error code in the temp object
  request->_tempObject = new int(code);
  AsyncWebServerResponse* response = request->beginResponse(code);
  request->send(response);
}

void UploadFirmwareService::handleEarlyDisconnect() {
#ifdef ESP32
  Update.abort();
#elif defined(ESP8266)
  Update.end();
#endif
}
