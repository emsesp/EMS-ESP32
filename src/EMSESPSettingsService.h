#ifndef EMSESPSettingsConfig_h
#define EMSESPSettingsConfig_h

#include <HttpEndpoint.h>
#include <FSPersistence.h>

#define EMSESP_SETTINGS_FILE "/config/emsespSettings.json"
#define EMSESP_SETTINGS_SERVICE_PATH "/rest/emsespSettings"

#define EMSESP_DEFAULT_TX_MODE 1
#define EMSESP_DEFAULT_EMS_BUS_ID 0x0B // service key

#define EMSESP_DEFAULT_SYSLOG_LEVEL -1
#define EMSESP_DEFAULT_SYSLOG_MARK_INTERVAL 0
#define EMSESP_DEFAULT_SYSLOG_HOST ""

#define EMSESP_DEFAULT_MASTER_THERMOSTAT 0 // not set
#define EMSESP_DEFAULT_SHOWER_TIMER false
#define EMSESP_DEFAULT_SHOWER_ALERT false

namespace emsesp {

enum MQTT_format : uint8_t { SINGLE = 1, NESTED, HA, CUSTOM };

class EMSESPSettings {
  public:
    uint8_t tx_mode;
    uint8_t ems_bus_id;
    uint8_t master_thermostat;
    bool    shower_timer;
    bool    shower_alert;

    // syslog
    int8_t   syslog_level; // uuid::log::Level
    uint32_t syslog_mark_interval;
    String   syslog_host;

    static void              read(EMSESPSettings & settings, JsonObject & root);
    static StateUpdateResult update(JsonObject & root, EMSESPSettings & settings);
};

class EMSESPSettingsService : public StatefulService<EMSESPSettings> {
  public:
    EMSESPSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager);

    void begin();

  private:
    HttpEndpoint<EMSESPSettings>  _httpEndpoint;
    FSPersistence<EMSESPSettings> _fsPersistence;
};

} // namespace emsesp

#endif