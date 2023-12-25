#ifndef NTPSettingsService_h
#define NTPSettingsService_h

#include <WiFi.h>

#include <HttpEndpoint.h>
#include <FSPersistence.h>

#include <time.h>
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

    static void read(NTPSettings & settings, JsonObject & root) {
        root["enabled"]   = settings.enabled;
        root["server"]    = settings.server;
        root["tz_label"]  = settings.tzLabel;
        root["tz_format"] = settings.tzFormat;
    }

    static StateUpdateResult update(JsonObject & root, NTPSettings & settings) {
        settings.enabled  = root["enabled"] | FACTORY_NTP_ENABLED;
        settings.server   = root["server"] | FACTORY_NTP_SERVER;
        settings.tzLabel  = root["tz_label"] | FACTORY_NTP_TIME_ZONE_LABEL;
        settings.tzFormat = root["tz_format"] | FACTORY_NTP_TIME_ZONE_FORMAT;
        return StateUpdateResult::CHANGED;
    }
};

class NTPSettingsService : public StatefulService<NTPSettings> {
  public:
    NTPSettingsService(PsychicHttpServer * server, FS * fs, SecurityManager * securityManager);

    void begin();
    void registerURI();

    static void ntp_received(struct timeval * tv);

  private:
    SecurityManager *   _securityManager;
    PsychicHttpServer * _server;

    HttpEndpoint<NTPSettings>  _httpEndpoint;
    FSPersistence<NTPSettings> _fsPersistence;

    bool connected_ = false;
    void WiFiEvent(WiFiEvent_t event);
    void configureNTP();

    // POST
    esp_err_t configureTime(PsychicRequest * request, JsonVariant & json);
};

#endif
