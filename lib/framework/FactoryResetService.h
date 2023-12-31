#ifndef FactoryResetService_h
#define FactoryResetService_h

#include <WiFi.h>
#include <PsychicHttp.h>
#include <SecurityManager.h>
#include <RestartService.h>
#include <FS.h>

#define FS_CONFIG_DIRECTORY "/config"
#define FACTORY_RESET_SERVICE_PATH "/rest/factoryReset"

class FactoryResetService {
    FS * fs;

  public:
    FactoryResetService(PsychicHttpServer * server, FS * fs, SecurityManager * securityManager);

    void registerURI();
    void factoryReset();

  private:
    SecurityManager *   _securityManager;
    PsychicHttpServer * _server;

    esp_err_t handleRequest(PsychicRequest * request);
};

#endif
