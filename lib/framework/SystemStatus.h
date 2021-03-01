#ifndef SystemStatus_h
#define SystemStatus_h

#include <WiFi.h>
#include <AsyncTCP.h>
#include <FS.h>
#include <LITTLEFS.h>

#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>
#include <SecurityManager.h>

#include <uuid/log.h>         // proddy added

#define MAX_ESP_STATUS_SIZE 1024
#define SYSTEM_STATUS_SERVICE_PATH "/rest/systemStatus"

class SystemStatus {
  public:
    SystemStatus(AsyncWebServer * server, SecurityManager * securityManager);

  private:
    void systemStatus(AsyncWebServerRequest * request);
};

#endif // end SystemStatus_h
