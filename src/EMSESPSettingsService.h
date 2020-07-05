#ifndef EMSESPSettingsConfig_h
#define EMSESPSettingsConfig_h

#include <HttpEndpoint.h>
#include <FSPersistence.h>

#define EMSESP_SETTINGS_FILE "/config/emsespSettings.json"
#define EMSESP_SETTINGS_SERVICE_PATH "/rest/emsespSettings"

#define EMSESP_DEFAULT_TX_MODE 1
#define EMSESP_DEFAULT_EMS_BUS_ID 0x0B // service key
#define EMSESP_DEFAULT_SYSTEM_HEARTBEAT true
#define EMSESP_DEFAULT_SYSLOG_LEVEL -1
#define EMSESP_DEFAULT_SYSLOG_MARK_INTERVAL 0
#define EMSESP_DEFAULT_SYSLOG_HOST ""
#define EMSESP_DEFAULT_MASTER_THERMOSTAT 0 // not set
#define EMSESP_DEFAULT_SHOWER_TIMER false
#define EMSESP_DEFAULT_SHOWER_ALERT false
#define EMSESP_DEFAULT_MQTT_FORMAT 2 // nested
#define EMSESP_DEFAULT_MQTT_QOS 0

#ifndef EMSESP_STANDALONE
#define EMSESP_DEFAULT_PUBLISH_TIME 10
#else
#define EMSESP_DEFAULT_PUBLISH_TIME 0
#endif

namespace emsesp {

enum MQTT_format : uint8_t { SINGLE = 1, NESTED, HA, CUSTOM };

class EMSESPSettings {
  public:
    uint8_t  tx_mode;
    uint8_t  ems_bus_id;
    bool     system_heartbeat;
    int8_t   syslog_level; // uuid::log::Level
    uint32_t syslog_mark_interval;
    String   syslog_host;
    uint8_t  master_thermostat;
    bool     shower_timer;
    bool     shower_alert;
    uint16_t publish_time; // seconds
    uint8_t  mqtt_format;  // 1=single, 2=nested, 3=ha, 4=custom
    uint8_t  mqtt_qos;

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