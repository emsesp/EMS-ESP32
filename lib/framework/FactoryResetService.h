#ifndef FactoryResetService_h
#define FactoryResetService_h

#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif

#include <ESPAsyncWebServer.h>
#include <SecurityManager.h>
#include <RestartService.h>
#include <FS.h>

#define FS_CONFIG_DIRECTORY "/config"
#define FACTORY_RESET_SERVICE_PATH "/rest/factoryReset"

class FactoryResetService {
  FS* fs;

 public:
  FactoryResetService(AsyncWebServer* server, FS* fs, SecurityManager* securityManager);

  void factoryReset();

 private:
  void handleRequest(AsyncWebServerRequest* request);
};

#endif  // end FactoryResetService_h
