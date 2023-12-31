#ifndef SystemStatus_h
#define SystemStatus_h

#include <WiFi.h>
#include <FS.h>
#include <LittleFS.h>

#include <ArduinoJson.h>
#include <PsychicHttp.h>
#include <SecurityManager.h>

#define MAX_ESP_STATUS_SIZE 1024
#define SYSTEM_STATUS_SERVICE_PATH "/rest/systemStatus"

class SystemStatus {
  public:
    SystemStatus(PsychicHttpServer * server, SecurityManager * securityManager);
    void registerURI();

  private:
    SecurityManager *   _securityManager;
    PsychicHttpServer * _server;

    esp_err_t systemStatus(PsychicRequest * request);
};

#endif
