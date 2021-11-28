#include <MqttSettingsService.h>

#include "../../src/emsesp_stub.hpp" // proddy added

using namespace std::placeholders; // for `_1` etc

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
    WiFi.onEvent(std::bind(&MqttSettingsService::WiFiEvent, this, _1, _2));
    _mqttClient.onConnect(std::bind(&MqttSettingsService::onMqttConnect, this, _1));
    _mqttClient.onDisconnect(std::bind(&MqttSettingsService::onMqttDisconnect, this, _1));
    addUpdateHandler([&](const String & originId) { onConfigUpdated(); }, false);
}

MqttSettingsService::~MqttSettingsService() {
}

void MqttSettingsService::begin() {
    _fsPersistence.readFromFS();
}

void MqttSettingsService::loop() {
    if (_reconfigureMqtt || (_disconnectedAt && (uint32_t)(uuid::get_uptime() - _disconnectedAt) >= MQTT_RECONNECTION_DELAY)) {
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
    // emsesp::EMSESP::logger().info(F("Connected to MQTT, %s"), (sessionPresent) ? F("with persistent session") : F("without persistent session"));
}

void MqttSettingsService::onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
    // emsesp::EMSESP::logger().info(F("Disconnected from MQTT reason: %d"), (uint8_t)reason);
    _disconnectReason = reason;
    _disconnectedAt   = uuid::get_uptime();
}

void MqttSettingsService::onConfigUpdated() {
    _reconfigureMqtt = true;
    _disconnectedAt  = 0;

    // added by proddy
    emsesp::EMSESP::mqtt_.start(); // reload EMS-ESP MQTT settings
}

void MqttSettingsService::WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
    switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
    case SYSTEM_EVENT_ETH_GOT_IP:
        emsesp::EMSESP::esp8266React.getNetworkSettingsService()->read([&](NetworkSettings & networkSettings) {
            if (!networkSettings.enableIPv6 && _state.enabled) {
                // emsesp::EMSESP::logger().info(F("IPv4 Network connection found, starting MQTT client"));
                onConfigUpdated();
            }
        });
        break;
    case SYSTEM_EVENT_GOT_IP6:
        if (_state.enabled) {
            // emsesp::EMSESP::logger().info(F("IPv6 Network connection found, starting MQTT client"));
            onConfigUpdated();
        }
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
    case SYSTEM_EVENT_ETH_DISCONNECTED:
        if (_state.enabled) {
            // emsesp::EMSESP::logger().info(F("Network connection dropped, stopping MQTT client"));
            _mqttClient.disconnect();
            // onConfigUpdated();
        }
        break;

    default:
        break;
    }
}

void MqttSettingsService::configureMqtt() {
    // disconnect if connected
    _mqttClient.disconnect();
    // only connect if WiFi is connected and MQTT is enabled
    if (_state.enabled && emsesp::EMSESP::system_.network_connected()) {
        // emsesp::EMSESP::logger().info(F("Configuring Mqtt client"));
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
        // } else {
        // emsesp::EMSESP::logger().info(F("Error configuring Mqtt client"));
    }
}

void MqttSettings::read(MqttSettings & settings, JsonObject & root) {
    root["enabled"]          = settings.enabled;
    root["host"]             = settings.host;
    root["port"]             = settings.port;
    root["base"]             = settings.base;
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
    root["mqtt_qos"]                = settings.mqtt_qos;
    root["mqtt_retain"]             = settings.mqtt_retain;
    root["ha_climate_format"]       = settings.ha_climate_format;
    root["ha_enabled"]              = settings.ha_enabled;
    root["nested_format"]           = settings.nested_format;
    root["send_response"]           = settings.send_response;
}

StateUpdateResult MqttSettings::update(JsonObject & root, MqttSettings & settings) {
    MqttSettings newSettings = {};
    bool         changed     = false;

    newSettings.enabled        = root["enabled"] | FACTORY_MQTT_ENABLED;
    newSettings.host           = root["host"] | FACTORY_MQTT_HOST;
    newSettings.port           = root["port"] | FACTORY_MQTT_PORT;
    newSettings.base           = root["base"] | FACTORY_MQTT_BASE;
    newSettings.username       = root["username"] | FACTORY_MQTT_USERNAME;
    newSettings.password       = root["password"] | FACTORY_MQTT_PASSWORD;
    newSettings.clientId       = root["client_id"] | FACTORY_MQTT_CLIENT_ID;
    newSettings.keepAlive      = root["keep_alive"] | FACTORY_MQTT_KEEP_ALIVE;
    newSettings.cleanSession   = root["clean_session"] | FACTORY_MQTT_CLEAN_SESSION;
    newSettings.maxTopicLength = root["max_topic_length"] | FACTORY_MQTT_MAX_TOPIC_LENGTH;
    newSettings.mqtt_qos       = root["mqtt_qos"] | EMSESP_DEFAULT_MQTT_QOS;
    newSettings.mqtt_retain    = root["mqtt_retain"] | EMSESP_DEFAULT_MQTT_RETAIN;

    newSettings.publish_time_boiler     = root["publish_time_boiler"] | EMSESP_DEFAULT_PUBLISH_TIME;
    newSettings.publish_time_thermostat = root["publish_time_thermostat"] | EMSESP_DEFAULT_PUBLISH_TIME;
    newSettings.publish_time_solar      = root["publish_time_solar"] | EMSESP_DEFAULT_PUBLISH_TIME;
    newSettings.publish_time_mixer      = root["publish_time_mixer"] | EMSESP_DEFAULT_PUBLISH_TIME;
    newSettings.publish_time_other      = root["publish_time_other"] | EMSESP_DEFAULT_PUBLISH_TIME;
    newSettings.publish_time_sensor     = root["publish_time_sensor"] | EMSESP_DEFAULT_PUBLISH_TIME;

    newSettings.ha_climate_format = root["ha_climate_format"] | EMSESP_DEFAULT_HA_CLIMATE_FORMAT;
    newSettings.ha_enabled        = root["ha_enabled"] | EMSESP_DEFAULT_HA_ENABLED;
    newSettings.nested_format     = root["nested_format"] | EMSESP_DEFAULT_NESTED_FORMAT;
    newSettings.send_response     = root["send_response"] | EMSESP_DEFAULT_SEND_RESPONSE;

    if (newSettings.enabled != settings.enabled) {
        changed = true;
    }

    if (newSettings.mqtt_qos != settings.mqtt_qos) {
        emsesp::EMSESP::mqtt_.set_qos(newSettings.mqtt_qos);
        changed = true;
    }

    if (newSettings.nested_format != settings.nested_format) {
        changed = true;
    }

    if (newSettings.send_response != settings.send_response) {
        changed = true;
    }

    if (newSettings.ha_climate_format != settings.ha_climate_format) {
        emsesp::EMSESP::mqtt_.ha_climate_format(newSettings.ha_climate_format);
        changed = true;
    }

    if (newSettings.ha_enabled != settings.ha_enabled) {
        emsesp::EMSESP::mqtt_.ha_enabled(newSettings.ha_enabled);
        changed = true;
    }

    if (newSettings.mqtt_retain != settings.mqtt_retain) {
        emsesp::EMSESP::mqtt_.set_retain(newSettings.mqtt_retain);
        changed = true;
    }

    if (newSettings.publish_time_boiler != settings.publish_time_boiler) {
        emsesp::EMSESP::mqtt_.set_publish_time_boiler(newSettings.publish_time_boiler);
        changed = true;
    }

    if (newSettings.publish_time_thermostat != settings.publish_time_thermostat) {
        emsesp::EMSESP::mqtt_.set_publish_time_thermostat(newSettings.publish_time_thermostat);
        changed = true;
    }

    if (newSettings.publish_time_solar != settings.publish_time_solar) {
        emsesp::EMSESP::mqtt_.set_publish_time_solar(newSettings.publish_time_solar);
        changed = true;
    }

    if (newSettings.publish_time_mixer != settings.publish_time_mixer) {
        emsesp::EMSESP::mqtt_.set_publish_time_mixer(newSettings.publish_time_mixer);
        changed = true;
    }

    if (newSettings.publish_time_other != settings.publish_time_other) {
        emsesp::EMSESP::mqtt_.set_publish_time_other(newSettings.publish_time_other);
        changed = true;
    }

    if (newSettings.publish_time_sensor != settings.publish_time_sensor) {
        emsesp::EMSESP::mqtt_.set_publish_time_sensor(newSettings.publish_time_sensor);
        changed = true;
    }

    if (changed) {
        emsesp::EMSESP::mqtt_.reset_mqtt();
    }

    settings = newSettings;

    return StateUpdateResult::CHANGED;
}
