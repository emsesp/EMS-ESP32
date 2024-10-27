#ifndef NetworkStatus_h
#define NetworkStatus_h

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <IPAddress.h>

#include "IPUtils.h"
#include "SecurityManager.h"

#define NETWORK_STATUS_SERVICE_PATH "/rest/networkStatus"

class NetworkStatus {
  public:
    NetworkStatus(AsyncWebServer * server, SecurityManager * securityManager);

  private:
    void networkStatus(AsyncWebServerRequest * request);
};

#endif
