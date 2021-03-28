#ifndef RestartService_h
#define RestartService_h

#include <WiFi.h>
#include <AsyncTCP.h>

#include <ESPAsyncWebServer.h>
#include <SecurityManager.h>

#define RESTART_SERVICE_PATH "/rest/restart"

class RestartService {
  public:
    RestartService(AsyncWebServer * server, SecurityManager * securityManager);

    static void restartNow() {
        WiFi.disconnect(true);
        delay(500);
        ESP.restart();
    }

  private:
    void restart(AsyncWebServerRequest * request);
};

#endif // end RestartService_h
