#ifndef NTPStatus_h
#define NTPStatus_h

#include <time.h>

#include <WiFi.h>
#include <esp_sntp.h>

#include <ArduinoJson.h>
#include <PsychicHttp.h>
#include <SecurityManager.h>
#include <uuid/common.h>

#define MAX_NTP_STATUS_SIZE 1024
#define NTP_STATUS_SERVICE_PATH "/rest/ntpStatus"

class NTPStatus {
  public:
    NTPStatus(PsychicHttpServer * server, SecurityManager * securityManager);
    void registerURI();

  private:
    SecurityManager *   _securityManager;
    PsychicHttpServer * _server;

    esp_err_t ntpStatus(PsychicRequest * request);
};

#endif
