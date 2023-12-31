#ifndef WiFiScanner_h
#define WiFiScanner_h

#include <WiFi.h>

#include <ArduinoJson.h>
#include <PsychicHttp.h>
#include <SecurityManager.h>

#define SCAN_NETWORKS_SERVICE_PATH "/rest/scanNetworks"
#define LIST_NETWORKS_SERVICE_PATH "/rest/listNetworks"

#define MAX_WIFI_SCANNER_SIZE 1024

class WiFiScanner {
  public:
    WiFiScanner(PsychicHttpServer * server, SecurityManager * securityManager);

    void registerURI();

  private:
    SecurityManager *   _securityManager;
    PsychicHttpServer * _server;

    esp_err_t scanNetworks(PsychicRequest * request);
    esp_err_t listNetworks(PsychicRequest * request);
};

#endif
