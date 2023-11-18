#ifndef NTPStatus_h
#define NTPStatus_h

#include <time.h>

#include <WiFi.h>
#include <esp_sntp.h>

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <SecurityManager.h>
#include <uuid/common.h>

#define MAX_NTP_STATUS_SIZE 1024
#define NTP_STATUS_SERVICE_PATH "/rest/ntpStatus"

class NTPStatus {
  public:
    NTPStatus(AsyncWebServer * server, SecurityManager * securityManager);

  private:
    void ntpStatus(AsyncWebServerRequest * request);
};

#endif
