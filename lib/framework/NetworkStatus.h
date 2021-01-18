#ifndef NetworkStatus_h
#define NetworkStatus_h

#include <WiFi.h>
#include <ETH.h>
#include <AsyncTCP.h>

#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>
#include <IPAddress.h>
#include <SecurityManager.h>

#define MAX_NETWORK_STATUS_SIZE 1024
#define NETWORK_STATUS_SERVICE_PATH "/rest/networkStatus"

class NetworkStatus {
  public:
    NetworkStatus(AsyncWebServer * server, SecurityManager * securityManager);

  private:
    void networkStatus(AsyncWebServerRequest * request);
};

#endif
