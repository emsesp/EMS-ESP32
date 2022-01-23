#ifndef FactoryResetService_h
#define FactoryResetService_h

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SecurityManager.h>
#include <RestartService.h>
#include <FS.h>

#define FS_CONFIG_DIRECTORY "/config"
#define FACTORY_RESET_SERVICE_PATH "/rest/factoryReset"

class FactoryResetService {
    FS * fs;

  public:
    FactoryResetService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager);

    void factoryReset();

  private:
    void handleRequest(AsyncWebServerRequest * request);
};

#endif
