#ifndef RestartService_h
#define RestartService_h

#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif

#include <ESPAsyncWebServer.h>
#include <SecurityManager.h>

#define RESTART_SERVICE_PATH "/rest/restart"

class RestartService {
 public:
  RestartService(AsyncWebServer* server, SecurityManager* securityManager);

  static void restartNow() {
    WiFi.disconnect(true);
    delay(500);
    ESP.restart();
  }

 private:
  void restart(AsyncWebServerRequest* request);
};

#endif  // end RestartService_h
