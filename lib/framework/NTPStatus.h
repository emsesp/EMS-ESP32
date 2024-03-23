#ifndef NTPStatus_h
#define NTPStatus_h

#include <ctime>
#include <WiFi.h>
#include <esp_sntp.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

#include "SecurityManager.h"

#include <uuid/common.h>

#define NTP_STATUS_SERVICE_PATH "/rest/ntpStatus"

class NTPStatus {
  public:
    NTPStatus(AsyncWebServer * server, SecurityManager * securityManager);

  private:
    void ntpStatus(AsyncWebServerRequest * request);
};

#endif
