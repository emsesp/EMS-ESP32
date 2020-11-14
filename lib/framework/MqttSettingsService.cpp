#include <MqttSettingsService.h>

// forward declarators
namespace emsesp {
class EMSESP {
  public:
    static Mqtt         mqtt_;
    static DallasSensor dallassensor_;
};
} // namespace emsesp

/**
 * Retains a copy of the cstr provided in the pointer provided using dynamic allocation.
 *
 * Frees the pointer before allocation and leaves it as nullptr if cstr == nullptr.
 */
static char * retainCstr(const char * cstr, char ** ptr) {
    // free up previously retained value if exists
    free(*ptr);
    *ptr = nullptr;

    // dynamically allocate and copy cstr (if non null)
    if (cstr != nullptr) {
        *ptr = (char *)malloc(strlen(cstr) + 1);
        strcpy(*ptr, cstr);
    }

    // return reference to pointer for convenience
    return *ptr;
}

MqttSettingsService::MqttSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _httpEndpoint(MqttSettings::read, MqttSettings::update, this, server, MQTT_SETTINGS_SERVICE_PATH, securityManager)
    , _fsPersistence(MqttSettings::read, MqttSettings::update, this, fs, MQTT_SETTINGS_FILE)
    , _retainedHost(nullptr)
    , _retainedClientId(nullptr)
    , _retainedUsername(nullptr)
    , _retainedPassword(nullptr)
    , _reconfigureMqtt(false)
    , _disconnectedAt(0)
    , _disconnectReason(AsyncMqttClientDisconnectReason::TCP_DISCONNECTED)
    , _mqttClient() {
#ifdef ESP32
    WiFi.onEvent(std::bind(&MqttSettingsService::onStationModeDisconnected, this, std::placeholders::_1, std::placeholders::_2),
                 WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);
    WiFi.onEvent(std::bind(&MqttSettingsService::onStationModeGotIP, this, std::placeholders::_1, std::placeholders::_2), WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);
#elif defined(ESP8266)
    _onStationModeDisconnectedHandler = WiFi.onStationModeDisconnected(std::bind(&MqttSettingsService::onStationModeDisconnected, this, std::placeholders::_1));
    _onStationModeGotIPHandler        = WiFi.onStationModeGotIP(std::bind(&MqttSettingsService::onStationModeGotIP, this, std::placeholders::_1));
#endif
    _mqttClient.onConnect(std::bind(&MqttSettingsService::onMqttConnect, this, std::placeholders::_1));
    _mqttClient.onDisconnect(std::bind(&MqttSettingsService::onMqttDisconnect, this, std::placeholders::_1));
    addUpdateHandler([&](const String & originId) { onConfigUpdated(); }, false);
}

MqttSettingsService::~MqttSettingsService() {
}

void MqttSettingsService::begin() {
    _fsPersistence.readFromFS();
}

void MqttSettingsService::loop() {
    if (_reconfigureMqtt || (_disconnectedAt && (unsigned long)(uuid::get_uptime() - _disconnectedAt) >= MQTT_RECONNECTION_DELAY)) {
        // reconfigure MQTT client
        configureMqtt();

        // clear the reconnection flags
        _reconfigureMqtt = false;
        _disconnectedAt  = 0;
    }
}

bool MqttSettingsService::isEnabled() {
    return _state.enabled;
}

bool MqttSettingsService::isConnected() {
    return _mqttClient.connected();
}

const char * MqttSettingsService::getClientId() {
    return _mqttClient.getClientId();
}

AsyncMqttClientDisconnectReason MqttSettingsService::getDisconnectReason() {
    return _disconnectReason;
}

AsyncMqttClient * MqttSettingsService::getMqttClient() {
    return &_mqttClient;
}

void MqttSettingsService::onMqttConnect(bool sessionPresent) {
    //   Serial.print(F("Connected to MQTT, "));
    //   if (sessionPresent) {
    //     Serial.println(F("with persistent session"));
    //   } else {
    //     Serial.println(F("without persistent session"));
    //   }
}

void MqttSettingsService::onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
    // Serial.print(F("Disconnected from MQTT reason: "));
    // Serial.println((uint8_t)reason);
    _disconnectReason = reason;
    _disconnectedAt   = uuid::get_uptime();
}

void MqttSettingsService::onConfigUpdated() {
    _reconfigureMqtt = true;
    _disconnectedAt  = 0;

    // added by proddy
    // reload EMS-ESP MQTT settings
    emsesp::EMSESP::mqtt_.start();
}

#ifdef ESP32
void MqttSettingsService::onStationModeGotIP(WiFiEvent_t event, WiFiEventInfo_t info) {
    if (_state.enabled) {
        // Serial.println(F("WiFi connection dropped, starting MQTT client."));
        onConfigUpdated();
    }
}

void MqttSettingsService::onStationModeDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
    if (_state.enabled) {
        // Serial.println(F("WiFi connection dropped, stopping MQTT client."));
        onConfigUpdated();
    }
}
#elif defined(ESP8266)
void MqttSettingsService::onStationModeGotIP(const WiFiEventStationModeGotIP & event) {
    if (_state.enabled) {
        // Serial.println(F("WiFi connection dropped, starting MQTT client."));
        onConfigUpdated();
    }
}

void MqttSettingsService::onStationModeDisconnected(const WiFiEventStationModeDisconnected & event) {
    if (_state.enabled) {
        // Serial.println(F("WiFi connection dropped, stopping MQTT client."));
        onConfigUpdated();
    }
}
#endif

void MqttSettingsService::configureMqtt() {
    // disconnect if currently connected
    _mqttClient.disconnect();

    // only connect if WiFi is connected and MQTT is enabled
    if (_state.enabled && WiFi.isConnected()) {
        // Serial.println(F("Connecting to MQTT..."));
        _mqttClient.setServer(retainCstr(_state.host.c_str(), &_retainedHost), _state.port);
        if (_state.username.length() > 0) {
            _mqttClient.setCredentials(retainCstr(_state.username.c_str(), &_retainedUsername),
                                       retainCstr(_state.password.length() > 0 ? _state.password.c_str() : nullptr, &_retainedPassword));
        } else {
            _mqttClient.setCredentials(retainCstr(nullptr, &_retainedUsername), retainCstr(nullptr, &_retainedPassword));
        }
        _mqttClient.setClientId(retainCstr(_state.clientId.c_str(), &_retainedClientId));
        _mqttClient.setKeepAlive(_state.keepAlive);
        _mqttClient.setCleanSession(_state.cleanSession);
        _mqttClient.setMaxTopicLength(_state.maxTopicLength);
        _mqttClient.connect();
    }

    emsesp::EMSESP::dallassensor_.reload();
}

void MqttSettings::read(MqttSettings & settings, JsonObject & root) {
    root["enabled"]          = settings.enabled;
    root["host"]             = settings.host;
    root["port"]             = settings.port;
    root["username"]         = settings.username;
    root["password"]         = settings.password;
    root["client_id"]        = settings.clientId;
    root["keep_alive"]       = settings.keepAlive;
    root["clean_session"]    = settings.cleanSession;
    root["max_topic_length"] = settings.maxTopicLength;

    // added by proddy for EMS-ESP
    root["publish_time_boiler"]     = settings.publish_time_boiler;
    root["publish_time_thermostat"] = settings.publish_time_thermostat;
    root["publish_time_solar"]      = settings.publish_time_solar;
    root["publish_time_mixer"]      = settings.publish_time_mixer;
    root["publish_time_other"]      = settings.publish_time_other;
    root["publish_time_sensor"]     = settings.publish_time_sensor;
    root["mqtt_format"]             = settings.mqtt_format;
    root["mqtt_qos"]                = settings.mqtt_qos;
    root["mqtt_retain"]             = settings.mqtt_retain;
}

StateUpdateResult MqttSettings::update(JsonObject & root, MqttSettings & settings) {
    MqttSettings newSettings = {};

    newSettings.enabled        = root["enabled"] | FACTORY_MQTT_ENABLED;
    newSettings.host           = root["host"] | FACTORY_MQTT_HOST;
    newSettings.port           = root["port"] | FACTORY_MQTT_PORT;
    newSettings.username       = root["username"] | FACTORY_MQTT_USERNAME;
    newSettings.password       = root["password"] | FACTORY_MQTT_PASSWORD;
    newSettings.clientId       = root["client_id"] | FACTORY_MQTT_CLIENT_ID;
    newSettings.keepAlive      = root["keep_alive"] | FACTORY_MQTT_KEEP_ALIVE;
    newSettings.cleanSession   = root["clean_session"] | FACTORY_MQTT_CLEAN_SESSION;
    newSettings.maxTopicLength = root["max_topic_length"] | FACTORY_MQTT_MAX_TOPIC_LENGTH;

    newSettings.publish_time_boiler     = root["publish_time_boiler"] | EMSESP_DEFAULT_PUBLISH_TIME;
    newSettings.publish_time_thermostat = root["publish_time_thermostat"] | EMSESP_DEFAULT_PUBLISH_TIME;
    newSettings.publish_time_solar      = root["publish_time_solar"] | EMSESP_DEFAULT_PUBLISH_TIME;
    newSettings.publish_time_mixer      = root["publish_time_mixer"] | EMSESP_DEFAULT_PUBLISH_TIME;
    newSettings.publish_time_other      = root["publish_time_other"] | EMSESP_DEFAULT_PUBLISH_TIME;
    newSettings.publish_time_sensor     = root["publish_time_sensor"] | EMSESP_DEFAULT_PUBLISH_TIME;
    newSettings.mqtt_format             = root["mqtt_format"] | EMSESP_DEFAULT_MQTT_FORMAT;
    newSettings.mqtt_qos                = root["mqtt_qos"] | EMSESP_DEFAULT_MQTT_QOS;
    newSettings.mqtt_retain             = root["mqtt_retain"] | EMSESP_DEFAULT_MQTT_RETAIN;

    if (newSettings.mqtt_qos != settings.mqtt_qos) {
        emsesp::EMSESP::mqtt_.set_qos(newSettings.mqtt_qos);
    }

    if (newSettings.mqtt_format != settings.mqtt_format) {
        emsesp::EMSESP::mqtt_.set_format(newSettings.mqtt_format);
    }

    if (newSettings.mqtt_retain != settings.mqtt_retain) {
        emsesp::EMSESP::mqtt_.set_retain(newSettings.mqtt_retain);
    }

    if (newSettings.publish_time_boiler != settings.publish_time_boiler) {
        emsesp::EMSESP::mqtt_.set_publish_time_boiler(newSettings.publish_time_boiler);
    }
    if (newSettings.publish_time_thermostat != settings.publish_time_thermostat) {
        emsesp::EMSESP::mqtt_.set_publish_time_thermostat(newSettings.publish_time_thermostat);
    }
    if (newSettings.publish_time_solar != settings.publish_time_solar) {
        emsesp::EMSESP::mqtt_.set_publish_time_solar(newSettings.publish_time_solar);
    }
    if (newSettings.publish_time_mixer != settings.publish_time_mixer) {
        emsesp::EMSESP::mqtt_.set_publish_time_mixer(newSettings.publish_time_mixer);
    }
    if (newSettings.publish_time_other != settings.publish_time_other) {
        emsesp::EMSESP::mqtt_.set_publish_time_other(newSettings.publish_time_other);
    }
    if (newSettings.publish_time_sensor != settings.publish_time_sensor) {
        emsesp::EMSESP::mqtt_.set_publish_time_sensor(newSettings.publish_time_sensor);
    }

    emsesp::EMSESP::mqtt_.reset_publish_fails(); // reset fail counter back to 0

    settings = newSettings;

    return StateUpdateResult::CHANGED;
}
