#ifndef RestartService_h
#define RestartService_h

#include <WiFi.h>

#include <PsychicHttp.h>
#include <SecurityManager.h>

#define RESTART_SERVICE_PATH "/rest/restart"
#define PARTITION_SERVICE_PATH "/rest/partition"

class RestartService {
  public:
    RestartService(PsychicHttpServer * server, SecurityManager * securityManager);

    void registerURI();

    static void restartNow() {
        delay(500); // wait for async tcp to catch up
        ESP.restart();
    }

  private:
    SecurityManager *   _securityManager;
    PsychicHttpServer * _server;

    esp_err_t restart(PsychicRequest * request);
    esp_err_t partition(PsychicRequest * request);
};

#endif
