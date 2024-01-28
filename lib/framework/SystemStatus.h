#ifndef SystemStatus_h
#define SystemStatus_h

#include <WiFi.h>
#include <AsyncTCP.h>
#include <FS.h>
#include <LittleFS.h>

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <SecurityManager.h>

// #define MAX_ESP_STATUS_SIZE 1024
#define SYSTEM_STATUS_SERVICE_PATH "/rest/systemStatus"

class SystemStatus {
  public:
    SystemStatus(AsyncWebServer * server, SecurityManager * securityManager);

  private:
    void systemStatus(AsyncWebServerRequest * request);
};

#endif
