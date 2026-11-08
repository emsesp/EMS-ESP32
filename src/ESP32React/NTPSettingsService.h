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

#ifndef FACTORY_NTP_THERMOSTAT_SYNC
#define FACTORY_NTP_THERMOSTAT_SYNC 1
#endif

#ifndef FACTORY_NTP_SERVER
#define FACTORY_NTP_SERVER "time.google.com"
#endif

#define NTP_SETTINGS_FILE "/config/ntpSettings.json"

#define NTP_SETTINGS_SERVICE_PATH "/rest/ntpSettings"
#define TIME_PATH "/rest/time"

// Fixed length for tz_format passed to setenv to avoid the realloc leak in #3184.
// Must be >= longest POSIX TZ string in tz_zones.h (currently 44 for Pacific/Chatham).
#ifndef NTP_TZ_FORMAT_LEN
#define NTP_TZ_FORMAT_LEN 48
#endif

class NTPSettings {
  public:
    bool    enabled         = FACTORY_NTP_ENABLED;
    String  tzLabel         = FACTORY_NTP_TIME_ZONE_LABEL;
    String  tzFormat        = FACTORY_NTP_TIME_ZONE_FORMAT;
    String  server          = FACTORY_NTP_SERVER;
    uint8_t thermostat_sync = FACTORY_NTP_THERMOSTAT_SYNC;
    String  tzLabelT        = FACTORY_NTP_TIME_ZONE_LABEL;
    String  tzFormatT       = FACTORY_NTP_TIME_ZONE_FORMAT;

    static void              read(NTPSettings & settings, JsonObject root);
    static StateUpdateResult update(JsonObject root, NTPSettings & settings);

    // Refresh tz_format / tz_format_t from the IANA label using tz_zones.h.
    // Returns true if either format string changed.
    static bool syncFormatsFromLabels(NTPSettings & settings);
};

class NTPSettingsService : public StatefulService<NTPSettings> {
  public:
    NTPSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager);

    void        begin();
    void        loop();
    static void ntp_received(struct timeval * tv);

  private:
    HttpEndpoint<NTPSettings>  _httpEndpoint;
    FSPersistence<NTPSettings> _fsPersistence;
    volatile bool              _connected;

    void configureNTP();
    void configureTime(AsyncWebServerRequest * request, JsonVariant json);
};

#endif
