#ifndef WiFiScanner_h
#define WiFiScanner_h

#include <WiFi.h>
#include <AsyncTCP.h>

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <SecurityManager.h>

#define SCAN_NETWORKS_SERVICE_PATH "/rest/scanNetworks"
#define LIST_NETWORKS_SERVICE_PATH "/rest/listNetworks"

// #define MAX_WIFI_SCANNER_SIZE 1024

class WiFiScanner {
  public:
    WiFiScanner(AsyncWebServer * server, SecurityManager * securityManager);

  private:
    void scanNetworks(AsyncWebServerRequest * request);
    void listNetworks(AsyncWebServerRequest * request);
};

#endif
