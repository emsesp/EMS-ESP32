#ifndef FeaturesService_h
#define FeaturesService_h

#include <Features.h>

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

#define MAX_FEATURES_SIZE 256
#define FEATURES_SERVICE_PATH "/rest/features"

class FeaturesService {
 public:
  FeaturesService(AsyncWebServer* server);

 private:
  void features(AsyncWebServerRequest* request);
};

#endif
