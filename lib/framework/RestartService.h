#ifndef RestartService_h
#define RestartService_h

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "SecurityManager.h"

#define RESTART_SERVICE_PATH "/rest/restart"
#define PARTITION_SERVICE_PATH "/rest/partition"
#define FACTORYPARTITION_SERVICE_PATH "/rest/factoryPartition"

class RestartService {
  public:
    RestartService(AsyncWebServer * server, SecurityManager * securityManager);

    static void restartNow();

  private:
    void restart(AsyncWebServerRequest * request);
    void partition(AsyncWebServerRequest * request);
    void factory(AsyncWebServerRequest * request);
};

#endif
