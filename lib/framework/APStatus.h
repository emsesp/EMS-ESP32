#ifndef APStatus_h
#define APStatus_h

#include <WiFi.h>
#include <AsyncTCP.h>

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <IPAddress.h>

#include "SecurityManager.h"
#include "APSettingsService.h"

#define AP_STATUS_SERVICE_PATH "/rest/apStatus"

class APStatus {
  public:
    APStatus(AsyncWebServer * server, SecurityManager * securityManager, APSettingsService * apSettingsService);

  private:
    APSettingsService * _apSettingsService;
    void                apStatus(AsyncWebServerRequest * request);
};

#endif
