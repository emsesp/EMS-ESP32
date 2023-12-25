#ifndef NetworkStatus_h
#define NetworkStatus_h

#include <WiFi.h>

#include <ETH.h>

#include <ArduinoJson.h>
#include <PsychicHttp.h>
#include <IPAddress.h>
#include <IPUtils.h>
#include <SecurityManager.h>

#define MAX_NETWORK_STATUS_SIZE 1024
#define NETWORK_STATUS_SERVICE_PATH "/rest/networkStatus"

class NetworkStatus {
  public:
    NetworkStatus(PsychicHttpServer * server, SecurityManager * securityManager);
    void registerURI();

  private:
    SecurityManager *   _securityManager;
    PsychicHttpServer * _server;

    esp_err_t networkStatus(PsychicRequest * request);
};

#endif
