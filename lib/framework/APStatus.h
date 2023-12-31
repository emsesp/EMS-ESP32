#ifndef APStatus_h
#define APStatus_h

#include <WiFi.h>

#include <ArduinoJson.h>
#include <PsychicHttp.h>
#include <IPAddress.h>
#include <SecurityManager.h>
#include <APSettingsService.h>

#define MAX_AP_STATUS_SIZE 1024
#define AP_STATUS_SERVICE_PATH "/rest/apStatus"

class APStatus {
  public:
    APStatus(PsychicHttpServer * server, SecurityManager * securityManager, APSettingsService * apSettingsService);
    void registerURI();

  private:
    SecurityManager *   _securityManager;
    PsychicHttpServer * _server;

    APSettingsService * _apSettingsService;

    esp_err_t apStatus(PsychicRequest * request);
};

#endif
