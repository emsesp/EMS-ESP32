#ifndef NTPSettingsService_h
#define NTPSettingsService_h

#include "HttpEndpoint.h"
#include "FSPersistence.h"

#include <ctime>
#include <esp_sntp.h>

#ifndef FACTORY_NTP_ENABLED
#define FACTORY_NTP_ENABLED true
#endif

#ifndef FACTORY_NTP_TIME_ZONE_LABEL
#define FACTORY_NTP_TIME_ZONE_LABEL "Europe/London"
#endif

#ifndef FACTORY_NTP_TIME_ZONE_FORMAT
#define FACTORY_NTP_TIME_ZONE_FORMAT "GMT0BST,M3.5.0/1,M10.5.0"
#endif

#ifndef FACTORY_NTP_SERVER
#define FACTORY_NTP_SERVER "time.google.com"
#endif

#define NTP_SETTINGS_FILE "/config/ntpSettings.json"

#define NTP_SETTINGS_SERVICE_PATH "/rest/ntpSettings"
#define TIME_PATH "/rest/time"

class NTPSettings {
  public:
    bool   enabled;
    String tzLabel;
    String tzFormat;
    String server;

    static void              read(NTPSettings & settings, JsonObject root);
    static StateUpdateResult update(JsonObject root, NTPSettings & settings);
};

class NTPSettingsService : public StatefulService<NTPSettings> {
  public:
    NTPSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager);

    void        begin();
    static void ntp_received(struct timeval * tv);

  private:
    HttpEndpoint<NTPSettings>  _httpEndpoint;
    FSPersistence<NTPSettings> _fsPersistence;
    bool                       _connected;

    void WiFiEvent(WiFiEvent_t event);
    void configureNTP();
    void configureTime(AsyncWebServerRequest * request, JsonVariant json);
};

#endif
