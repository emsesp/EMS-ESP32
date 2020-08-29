#ifndef APStatus_h
#define APStatus_h

#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif

#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>
#include <IPAddress.h>
#include <SecurityManager.h>
#include <APSettingsService.h>

#define MAX_AP_STATUS_SIZE 1024
#define AP_STATUS_SERVICE_PATH "/rest/apStatus"

class APStatus {
 public:
  APStatus(AsyncWebServer* server, SecurityManager* securityManager, APSettingsService* apSettingsService);

 private:
  APSettingsService* _apSettingsService;
  void apStatus(AsyncWebServerRequest* request);
};

#endif  // end APStatus_h
