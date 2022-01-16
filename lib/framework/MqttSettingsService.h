#ifndef MqttSettingsService_h
#define MqttSettingsService_h

#include <StatefulService.h>
#include <HttpEndpoint.h>
#include <FSPersistence.h>
#include <AsyncMqttClient.h>
#include <ESPUtils.h>

#include <uuid/common.h>

#define MQTT_RECONNECTION_DELAY 2000 // 2 seconds

#define MQTT_SETTINGS_FILE "/config/mqttSettings.json"
#define MQTT_SETTINGS_SERVICE_PATH "/rest/mqttSettings"

#ifndef FACTORY_MQTT_ENABLED
#define FACTORY_MQTT_ENABLED false
#endif

#ifndef FACTORY_MQTT_HOST
#define FACTORY_MQTT_HOST "test.mosquitto.org"
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

#ifndef FACTORY_MQTT_CLIENT_ID
#define FACTORY_MQTT_CLIENT_ID generateClientId()
static String generateClientId() {
    return ESPUtils::defaultDeviceValue("esp32-");
}
#endif

#ifndef FACTORY_MQTT_KEEP_ALIVE
#define FACTORY_MQTT_KEEP_ALIVE 16
#endif

#ifndef FACTORY_MQTT_CLEAN_SESSION
#define FACTORY_MQTT_CLEAN_SESSION true
#endif

#ifndef FACTORY_MQTT_MAX_TOPIC_LENGTH
#define FACTORY_MQTT_MAX_TOPIC_LENGTH 128
#endif

class MqttSettings {
  public:
    // host and port - if enabled
    bool     enabled;
    String   host;
    uint16_t port;

    // username and password
    String username;
    String password;

    // client id settings
    String clientId;

    // connection settings
    uint16_t keepAlive;
    bool     cleanSession;
    uint16_t maxTopicLength;

    // proddy EMS-ESP specific
    String   base;
    uint16_t publish_time_boiler;
    uint16_t publish_time_thermostat;
    uint16_t publish_time_solar;
    uint16_t publish_time_mixer;
    uint16_t publish_time_other;
    uint16_t publish_time_sensor;
    uint8_t  mqtt_qos;
    bool     mqtt_retain;
    bool     ha_enabled;
    uint8_t  nested_format;
    String   discovery_prefix;
    bool     publish_single;
    bool     send_response;

    static void              read(MqttSettings & settings, JsonObject & root);
    static StateUpdateResult update(JsonObject & root, MqttSettings & settings);
};

class MqttSettingsService : public StatefulService<MqttSettings> {
  public:
    MqttSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager);
    ~MqttSettingsService();

    void                            begin();
    void                            loop();
    bool                            isEnabled();
    bool                            isConnected();
    const char *                    getClientId();
    AsyncMqttClientDisconnectReason getDisconnectReason();
    AsyncMqttClient *               getMqttClient();

  protected:
    void onConfigUpdated();

  private:
    HttpEndpoint<MqttSettings>  _httpEndpoint;
    FSPersistence<MqttSettings> _fsPersistence;

    // Pointers to hold retained copies of the mqtt client connection strings.
    // This is required as AsyncMqttClient holds references to the supplied connection strings.
    char * _retainedHost;
    char * _retainedClientId;
    char * _retainedUsername;
    char * _retainedPassword;

    // variable to help manage connection
    bool          _reconfigureMqtt;
    unsigned long _disconnectedAt;

    // connection status
    AsyncMqttClientDisconnectReason _disconnectReason;

    // the MQTT client instance
    AsyncMqttClient _mqttClient;

    void WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info);
    void onMqttConnect(bool sessionPresent);
    void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
    void configureMqtt();
};

#endif
