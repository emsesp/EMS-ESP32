#ifndef FactoryResetService_h
#define FactoryResetService_h

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>

#include "SecurityManager.h"
#include "RestartService.h"

#define FS_CONFIG_DIRECTORY "/config"
#define FACTORY_RESET_SERVICE_PATH "/rest/factoryReset"

class FactoryResetService {
  public:
    FactoryResetService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager);

    void factoryReset();

  private:
    FS * fs;
    void handleRequest(AsyncWebServerRequest * request);
};

#endif
