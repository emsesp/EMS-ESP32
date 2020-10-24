#ifndef MqttSettingsService_h
#define MqttSettingsService_h

#include <StatefulService.h>
#include <HttpEndpoint.h>
#include <FSPersistence.h>
#include <AsyncMqttClient.h>
#include <ESPUtils.h>
#include <uuid/common.h>

#include "../../src/system.h"
#include "../../src/mqtt.h"
#include "../../src/dallassensor.h"

#define MQTT_RECONNECTION_DELAY 1000

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

#ifndef FACTORY_MQTT_USERNAME
#define FACTORY_MQTT_USERNAME ""
#endif

#ifndef FACTORY_MQTT_PASSWORD
#define FACTORY_MQTT_PASSWORD ""
#endif

#ifndef FACTORY_MQTT_CLIENT_ID
#define FACTORY_MQTT_CLIENT_ID generateClientId()
static String generateClientId() {
#ifdef ESP32
    return ESPUtils::defaultDeviceValue("esp32-");
#elif defined(ESP8266)
    return ESPUtils::defaultDeviceValue("esp8266-");
#endif
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

#define EMSESP_DEFAULT_MQTT_FORMAT 2 // nested
#define EMSESP_DEFAULT_MQTT_QOS 0
#define EMSESP_DEFAULT_MQTT_RETAIN false
#define EMSESP_DEFAULT_PUBLISH_TIME 10

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
    uint16_t publish_time_boiler;
    uint16_t publish_time_thermostat;
    uint16_t publish_time_solar;
    uint16_t publish_time_mixer;
    uint16_t publish_time_other;
    uint16_t publish_time_sensor;
    uint8_t  mqtt_format; // 1=single, 2=nested, 3=ha, 4=custom
    uint8_t  mqtt_qos;
    bool     mqtt_retain;

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

#ifdef ESP32
    void onStationModeGotIP(WiFiEvent_t event, WiFiEventInfo_t info);
    void onStationModeDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
#elif defined(ESP8266)
    WiFiEventHandler _onStationModeDisconnectedHandler;
    WiFiEventHandler _onStationModeGotIPHandler;
    void             onStationModeGotIP(const WiFiEventStationModeGotIP & event);
    void             onStationModeDisconnected(const WiFiEventStationModeDisconnected & event);
#endif

    void onMqttConnect(bool sessionPresent);
    void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
    void configureMqtt();
};

#endif // end MqttSettingsService_h
