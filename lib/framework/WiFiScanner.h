#ifndef WiFiScanner_h
#define WiFiScanner_h

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
#include <SecurityManager.h>

#define SCAN_NETWORKS_SERVICE_PATH "/rest/scanNetworks"
#define LIST_NETWORKS_SERVICE_PATH "/rest/listNetworks"

#define MAX_WIFI_SCANNER_SIZE 1024

class WiFiScanner {
 public:
  WiFiScanner(AsyncWebServer* server, SecurityManager* securityManager);

 private:
  void scanNetworks(AsyncWebServerRequest* request);
  void listNetworks(AsyncWebServerRequest* request);

#ifdef ESP8266
  uint8_t convertEncryptionType(uint8_t encryptionType);
#endif
};

#endif  // end WiFiScanner_h
