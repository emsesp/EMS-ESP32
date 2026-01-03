#ifndef MqttSettingsService_h
#define MqttSettingsService_h

#include "StatefulService.h"
#include "HttpEndpoint.h"
#include "FSPersistence.h"

#include <espMqttClient.h>

#include <uuid/common.h>
#include <default_settings.h>

#define MQTT_RECONNECTION_DELAY 2000 // 2 seconds

#define MQTT_SETTINGS_FILE "/config/mqttSettings.json"
#define MQTT_SETTINGS_SERVICE_PATH "/rest/mqttSettings"

#ifndef FACTORY_MQTT_ENABLED
#define FACTORY_MQTT_ENABLED false
#endif

#ifndef FACTORY_MQTT_HOST
#define FACTORY_MQTT_HOST "" // is blank
#endif

#ifndef FACTORY_MQTT_PORT
#define FACTORY_MQTT_PORT 1883
#endif

#ifndef FACTORY_MQTT_BASE
#define FACTORY_MQTT_BASE "ems-esp"
#endif

#ifndef FACTORY_MQTT_USERNAME
#define FACTORY_MQTT_USERNAME ""
#endif

#ifndef FACTORY_MQTT_PASSWORD
#define FACTORY_MQTT_PASSWORD ""
#endif

#ifndef FACTORY_MQTT_KEEP_ALIVE
#define FACTORY_MQTT_KEEP_ALIVE 60
#endif

#ifndef FACTORY_MQTT_CLEAN_SESSION
#define FACTORY_MQTT_CLEAN_SESSION false
#endif

#ifndef FACTORY_MQTT_MAX_TOPIC_LENGTH
#define FACTORY_MQTT_MAX_TOPIC_LENGTH 128
#endif

#ifndef EMSESP_MQTT_RUNNING_CORE
#define EMSESP_MQTT_RUNNING_CORE 1
#endif

#ifdef EMSESP_MQTT_STACKSIZE
#undef EMC_TASK_STACK_SIZE
#define EMC_TASK_STACK_SIZE EMSESP_MQTT_STACKSIZE
#endif

#ifndef EMSESP_MQTT_PRIORITY
#define EMSESP_MQTT_PRIORITY 1
#endif

class MqttSettings {
  public:
    bool     enabled = FACTORY_MQTT_ENABLED;
    String   host;
    uint16_t port = FACTORY_MQTT_PORT;
    String   rootCA;
    bool     enableTLS = false;
    String   username;
    String   password;
    String   clientId;
    uint16_t keepAlive    = FACTORY_MQTT_KEEP_ALIVE;
    bool     cleanSession = FACTORY_MQTT_CLEAN_SESSION;

    // EMS-ESP specific
    String   base;
    uint16_t publish_time_boiler     = EMSESP_DEFAULT_PUBLISH_TIME;
    uint16_t publish_time_thermostat = EMSESP_DEFAULT_PUBLISH_TIME;
    uint16_t publish_time_solar      = EMSESP_DEFAULT_PUBLISH_TIME;
    uint16_t publish_time_mixer      = EMSESP_DEFAULT_PUBLISH_TIME;
    uint16_t publish_time_water      = EMSESP_DEFAULT_PUBLISH_TIME;
    uint16_t publish_time_other      = EMSESP_DEFAULT_PUBLISH_TIME_OTHER;
    uint16_t publish_time_sensor     = EMSESP_DEFAULT_PUBLISH_TIME;
    uint16_t publish_time_heartbeat  = EMSESP_DEFAULT_PUBLISH_HEARTBEAT;
    uint8_t  mqtt_qos                = EMSESP_DEFAULT_MQTT_QOS;
    bool     mqtt_retain             = EMSESP_DEFAULT_MQTT_RETAIN;
    bool     ha_enabled              = EMSESP_DEFAULT_HA_ENABLED;
    uint8_t  nested_format           = EMSESP_DEFAULT_NESTED_FORMAT;
    String   discovery_prefix;
    uint8_t  discovery_type     = EMSESP_DEFAULT_DISCOVERY_TYPE;
    bool     publish_single     = EMSESP_DEFAULT_PUBLISH_SINGLE;
    bool     publish_single2cmd = EMSESP_DEFAULT_PUBLISH_SINGLE2CMD;
    bool     send_response      = EMSESP_DEFAULT_SEND_RESPONSE;
    uint8_t  entity_format      = EMSESP_DEFAULT_ENTITY_FORMAT;

    static void              read(MqttSettings & settings, JsonObject root);
    static StateUpdateResult update(JsonObject root, MqttSettings & settings);
};

class MqttSettingsService : public StatefulService<MqttSettings> {
  public:
    MqttSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager);
    ~MqttSettingsService();

    void                                 begin();
    void                                 startClient();
    void                                 loop();
    bool                                 isEnabled();
    bool                                 isConnected();
    const char *                         getClientId();
    espMqttClientTypes::DisconnectReason getDisconnectReason();
    MqttClient *                         getMqttClient();

  protected:
    void onConfigUpdated();

  private:
    HttpEndpoint<MqttSettings>  _httpEndpoint;
    FSPersistence<MqttSettings> _fsPersistence;

    // variable to help manage connection
    bool          _reconfigureMqtt;
    unsigned long _disconnectedAt;

    // connection status
    espMqttClientTypes::DisconnectReason _disconnectReason;

    // the MQTT client instance
    MqttClient * _mqttClient;

    void WiFiEvent(WiFiEvent_t event);
    void onMqttConnect(bool sessionPresent);
    void onMqttDisconnect(espMqttClientTypes::DisconnectReason reason);
    void
    onMqttMessage(const espMqttClientTypes::MessageProperties & properties, const char * topic, const uint8_t * payload, size_t len, size_t index, size_t total);

    bool configureMqtt();
};

#endif
