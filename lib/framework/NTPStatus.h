#ifndef NTPStatus_h
#define NTPStatus_h

#include <time.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#include <lwip/apps/sntp.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <sntp.h>
#endif

#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>
#include <SecurityManager.h>

#define MAX_NTP_STATUS_SIZE 1024
#define NTP_STATUS_SERVICE_PATH "/rest/ntpStatus"

class NTPStatus {
 public:
  NTPStatus(AsyncWebServer* server, SecurityManager* securityManager);

 private:
  void ntpStatus(AsyncWebServerRequest* request);
};

#endif  // end NTPStatus_h
