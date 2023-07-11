#include <MqttSettingsService.h>

#include "../../src/emsesp_stub.hpp" // proddy added

using namespace std::placeholders;   // for `_1` etc

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
    , _disconnectReason(espMqttClientTypes::DisconnectReason::TCP_DISCONNECTED)
    , _mqttClient(nullptr) {
    WiFi.onEvent(std::bind(&MqttSettingsService::WiFiEvent, this, _1, _2));
    addUpdateHandler([&](const String & originId) { onConfigUpdated(); }, false);
}

MqttSettingsService::~MqttSettingsService() {
}

void MqttSettingsService::begin() {
    _fsPersistence.readFromFS();
    startClient();
}

void MqttSettingsService::startClient() {
    static bool isSecure = false;
    if (_mqttClient != nullptr) {
        // do we need to change the client?
        if ((isSecure && _state.rootCA.length() > 0) || (!isSecure && _state.rootCA.length() == 0)) {
            return;
        }
        delete _mqttClient;
    }
#if CONFIG_IDF_TARGET_ESP32S3
    if (_state.rootCA.length() > 0) {
        isSecure    = true;
        _mqttClient = static_cast<MqttClient *>(new espMqttClientSecure(espMqttClientTypes::UseInternalTask::NO));
        if (_state.rootCA == "insecure") {
            static_cast<espMqttClientSecure *>(_mqttClient)->setInsecure();
        } else {
            String certificate = "-----BEGIN CERTIFICATE-----\n" + _state.rootCA + "\n-----END CERTIFICATE-----\n";
            static_cast<espMqttClientSecure *>(_mqttClient)->setCACert(retainCstr(certificate.c_str(), &_retainedRootCA));
        }
        static_cast<espMqttClientSecure *>(_mqttClient)->onConnect(std::bind(&MqttSettingsService::onMqttConnect, this, _1));
        static_cast<espMqttClientSecure *>(_mqttClient)->onDisconnect(std::bind(&MqttSettingsService::onMqttDisconnect, this, _1));
        return;
    }
#endif
    isSecure    = false;
    _mqttClient = static_cast<MqttClient *>(new espMqttClient(espMqttClientTypes::UseInternalTask::NO));
    static_cast<espMqttClient *>(_mqttClient)->onConnect(std::bind(&MqttSettingsService::onMqttConnect, this, _1));
    static_cast<espMqttClient *>(_mqttClient)->onDisconnect(std::bind(&MqttSettingsService::onMqttDisconnect, this, _1));
}

void MqttSettingsService::loop() {
    if (_reconfigureMqtt || (_disconnectedAt && (uint32_t)(uuid::get_uptime() - _disconnectedAt) >= MQTT_RECONNECTION_DELAY)) {
        // reconfigure MQTT client
        _disconnectedAt  = configureMqtt() ? 0 : uuid::get_uptime();
        _reconfigureMqtt = false;
    }
    _mqttClient->loop();
}

bool MqttSettingsService::isEnabled() {
    return _state.enabled;
}

bool MqttSettingsService::isConnected() {
    return _mqttClient->connected();
}

const char * MqttSettingsService::getClientId() {
    return _mqttClient->getClientId();
}

void MqttSettingsService::setWill(const char * topic) {
#if CONFIG_IDF_TARGET_ESP32S3
    if (_state.rootCA.length() > 0) {
        static_cast<espMqttClientSecure *>(_mqttClient)->setWill(topic, 1, true, "offline");
        return;
    }
#endif
    static_cast<espMqttClient *>(_mqttClient)->setWill(topic, 1, true, "offline");
}

void MqttSettingsService::onMessage(espMqttClientTypes::OnMessageCallback callback) {
#if CONFIG_IDF_TARGET_ESP32S3
    if (_state.rootCA.length() > 0) {
        static_cast<espMqttClientSecure *>(_mqttClient)->onMessage(callback);
        return;
    }
#endif
    static_cast<espMqttClient *>(_mqttClient)->onMessage(callback);
}

espMqttClientTypes::DisconnectReason MqttSettingsService::getDisconnectReason() {
    return _disconnectReason;
}

MqttClient * MqttSettingsService::getMqttClient() {
    return _mqttClient;
}

void MqttSettingsService::onMqttConnect(bool sessionPresent) {
    // _disconnectedAt = 0;
    emsesp::EMSESP::mqtt_.on_connect();
    // emsesp::EMSESP::logger().info("Connected to MQTT, %s", (sessionPresent) ? ("with persistent session") : ("without persistent session"));
}

void MqttSettingsService::onMqttDisconnect(espMqttClientTypes::DisconnectReason reason) {
    _disconnectReason = reason;
    if (!_disconnectedAt) {
        _disconnectedAt = uuid::get_uptime();
    }
    emsesp::EMSESP::mqtt_.on_disconnect(reason);
}

void MqttSettingsService::onConfigUpdated() {
    _reconfigureMqtt = true;
    _disconnectedAt  = 0;

    // added by proddy
    startClient();
    emsesp::EMSESP::mqtt_.start(); // reload EMS-ESP MQTT settings
}

void MqttSettingsService::WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
    switch (event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
    case ARDUINO_EVENT_ETH_GOT_IP:
    case ARDUINO_EVENT_ETH_GOT_IP6:
    case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
        if (_state.enabled) {
            onConfigUpdated();
        }
        break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
    case ARDUINO_EVENT_ETH_DISCONNECTED:
        if (_state.enabled) {
            _mqttClient->disconnect(true);
        }
        break;

    default:
        break;
    }
}

bool MqttSettingsService::configureMqtt() {
    // disconnect if connected
    if (_mqttClient->connected()) {
        emsesp::EMSESP::logger().info("Disconneting to configure");
        _mqttClient->disconnect(true);
    }
    // only connect if WiFi is connected and MQTT is enabled
    if (_state.enabled && emsesp::EMSESP::system_.network_connected() && !_state.host.isEmpty()) {
        // if (_state.enabled && !_state.host.isEmpty()) {
        _reconfigureMqtt = false;
#if CONFIG_IDF_TARGET_ESP32S3
        if (_state.rootCA.length() > 0) {
            // emsesp::EMSESP::logger().info("Start secure MQTT with rootCA");
            static_cast<espMqttClientSecure *>(_mqttClient)->setServer(retainCstr(_state.host.c_str(), &_retainedHost), _state.port);
            if (_state.username.length() > 0) {
                static_cast<espMqttClientSecure *>(_mqttClient)
                    ->setCredentials(retainCstr(_state.username.c_str(), &_retainedUsername),
                                     retainCstr(_state.password.length() > 0 ? _state.password.c_str() : nullptr, &_retainedPassword));
            } else {
                static_cast<espMqttClientSecure *>(_mqttClient)->setCredentials(retainCstr(nullptr, &_retainedUsername), retainCstr(nullptr, &_retainedPassword));
            }
            static_cast<espMqttClientSecure *>(_mqttClient)->setClientId(retainCstr(_state.clientId.c_str(), &_retainedClientId));
            static_cast<espMqttClientSecure *>(_mqttClient)->setKeepAlive(_state.keepAlive);
            static_cast<espMqttClientSecure *>(_mqttClient)->setCleanSession(_state.cleanSession);
            return _mqttClient->connect();
        }
#endif
        // emsesp::EMSESP::logger().info("Configuring MQTT client");
        static_cast<espMqttClient *>(_mqttClient)->setServer(retainCstr(_state.host.c_str(), &_retainedHost), _state.port);
        if (_state.username.length() > 0) {
            static_cast<espMqttClient *>(_mqttClient)
                ->setCredentials(retainCstr(_state.username.c_str(), &_retainedUsername),
                                 retainCstr(_state.password.length() > 0 ? _state.password.c_str() : nullptr, &_retainedPassword));
        } else {
            static_cast<espMqttClient *>(_mqttClient)->setCredentials(retainCstr(nullptr, &_retainedUsername), retainCstr(nullptr, &_retainedPassword));
        }
        static_cast<espMqttClient *>(_mqttClient)->setClientId(retainCstr(_state.clientId.c_str(), &_retainedClientId));
        static_cast<espMqttClient *>(_mqttClient)->setKeepAlive(_state.keepAlive);
        static_cast<espMqttClient *>(_mqttClient)->setCleanSession(_state.cleanSession);
        return _mqttClient->connect();
    }
    return false;
}

void MqttSettings::read(MqttSettings & settings, JsonObject & root) {
#if CONFIG_IDF_TARGET_ESP32S3
    root["rootCA"] = settings.rootCA;
#endif
    root["enabled"]       = settings.enabled;
    root["host"]          = settings.host;
    root["port"]          = settings.port;
    root["base"]          = settings.base;
    root["username"]      = settings.username;
    root["password"]      = settings.password;
    root["client_id"]     = settings.clientId;
    root["keep_alive"]    = settings.keepAlive;
    root["clean_session"] = settings.cleanSession;
    root["entity_format"] = settings.entity_format;

    // added by proddy for EMS-ESP
    root["publish_time_boiler"]     = settings.publish_time_boiler;
    root["publish_time_thermostat"] = settings.publish_time_thermostat;
    root["publish_time_solar"]      = settings.publish_time_solar;
    root["publish_time_mixer"]      = settings.publish_time_mixer;
    root["publish_time_other"]      = settings.publish_time_other;
    root["publish_time_sensor"]     = settings.publish_time_sensor;
    root["publish_time_heartbeat"]  = settings.publish_time_heartbeat;
    root["mqtt_qos"]                = settings.mqtt_qos;
    root["mqtt_retain"]             = settings.mqtt_retain;
    root["ha_enabled"]              = settings.ha_enabled;
    root["nested_format"]           = settings.nested_format;
    root["discovery_prefix"]        = settings.discovery_prefix;
    root["discovery_type"]          = settings.discovery_type;
    root["publish_single"]          = settings.publish_single;
    root["publish_single2cmd"]      = settings.publish_single2cmd;
    root["send_response"]           = settings.send_response;
}

StateUpdateResult MqttSettings::update(JsonObject & root, MqttSettings & settings) {
    MqttSettings newSettings = {};
    bool         changed     = false;

#if CONFIG_IDF_TARGET_ESP32S3
    newSettings.rootCA = root["rootCA"] | "";
#endif
    newSettings.enabled      = root["enabled"] | FACTORY_MQTT_ENABLED;
    newSettings.host         = root["host"] | FACTORY_MQTT_HOST;
    newSettings.port         = root["port"] | FACTORY_MQTT_PORT;
    newSettings.base         = root["base"] | FACTORY_MQTT_BASE;
    newSettings.username     = root["username"] | FACTORY_MQTT_USERNAME;
    newSettings.password     = root["password"] | FACTORY_MQTT_PASSWORD;
    newSettings.clientId     = root["client_id"] | FACTORY_MQTT_CLIENT_ID;
    newSettings.keepAlive    = root["keep_alive"] | FACTORY_MQTT_KEEP_ALIVE;
    newSettings.cleanSession = root["clean_session"] | FACTORY_MQTT_CLEAN_SESSION;
    newSettings.mqtt_qos     = root["mqtt_qos"] | EMSESP_DEFAULT_MQTT_QOS;
    newSettings.mqtt_retain  = root["mqtt_retain"] | EMSESP_DEFAULT_MQTT_RETAIN;

    newSettings.publish_time_boiler     = root["publish_time_boiler"] | EMSESP_DEFAULT_PUBLISH_TIME;
    newSettings.publish_time_thermostat = root["publish_time_thermostat"] | EMSESP_DEFAULT_PUBLISH_TIME;
    newSettings.publish_time_solar      = root["publish_time_solar"] | EMSESP_DEFAULT_PUBLISH_TIME;
    newSettings.publish_time_mixer      = root["publish_time_mixer"] | EMSESP_DEFAULT_PUBLISH_TIME;
    newSettings.publish_time_other      = root["publish_time_other"] | EMSESP_DEFAULT_PUBLISH_TIME;
    newSettings.publish_time_sensor     = root["publish_time_sensor"] | EMSESP_DEFAULT_PUBLISH_TIME;
    newSettings.publish_time_heartbeat  = root["publish_time_heartbeat"] | EMSESP_DEFAULT_PUBLISH_HEARTBEAT;

    newSettings.ha_enabled         = root["ha_enabled"] | EMSESP_DEFAULT_HA_ENABLED;
    newSettings.nested_format      = root["nested_format"] | EMSESP_DEFAULT_NESTED_FORMAT;
    newSettings.discovery_prefix   = root["discovery_prefix"] | EMSESP_DEFAULT_DISCOVERY_PREFIX;
    newSettings.discovery_type     = root["discovery_type"] | EMSESP_DEFAULT_DISCOVERY_TYPE;
    newSettings.publish_single     = root["publish_single"] | EMSESP_DEFAULT_PUBLISH_SINGLE;
    newSettings.publish_single2cmd = root["publish_single2cmd"] | EMSESP_DEFAULT_PUBLISH_SINGLE2CMD;
    newSettings.send_response      = root["send_response"] | EMSESP_DEFAULT_SEND_RESPONSE;
    newSettings.entity_format      = root["entity_format"] | EMSESP_DEFAULT_ENTITY_FORMAT;

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

    if (newSettings.discovery_prefix != settings.discovery_prefix) {
        changed = true;
    }

    if (newSettings.discovery_type != settings.discovery_type) {
        changed = true;
    }

    if (newSettings.entity_format != settings.entity_format) {
        changed = true;
    }

    //  if both settings are stored from older version, HA has priority
    if (newSettings.ha_enabled && newSettings.publish_single) {
        newSettings.publish_single = false;
    }

    if (newSettings.publish_single != settings.publish_single) {
        if (newSettings.publish_single) {
            newSettings.ha_enabled = false;
        }
        changed = true;
    }

    if (newSettings.publish_single2cmd != settings.publish_single2cmd) {
        changed = true;
    }

    if (newSettings.send_response != settings.send_response) {
        changed = true;
    }

    if (newSettings.ha_enabled != settings.ha_enabled) {
        emsesp::EMSESP::mqtt_.ha_enabled(newSettings.ha_enabled);
        if (newSettings.ha_enabled) {
            newSettings.publish_single = false;
        }
        changed = true;
    }

    if (newSettings.mqtt_retain != settings.mqtt_retain) {
        emsesp::EMSESP::mqtt_.set_retain(newSettings.mqtt_retain);
        changed = true;
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

    if (newSettings.publish_time_heartbeat != settings.publish_time_heartbeat) {
        emsesp::EMSESP::mqtt_.set_publish_time_heartbeat(newSettings.publish_time_heartbeat);
    }

#if CONFIG_IDF_TARGET_ESP32S3
    // strip down to certificate only
    newSettings.rootCA.replace("\r", "");
    newSettings.rootCA.replace("\n", "");
    newSettings.rootCA.replace("-----BEGIN CERTIFICATE-----", "");
    newSettings.rootCA.replace("-----END CERTIFICATE-----", "");
    newSettings.rootCA.replace(" ", "");
    if (newSettings.rootCA.length() == 0 && newSettings.port > 8800) {
        newSettings.rootCA = "insecure";
    }
    if (newSettings.rootCA != settings.rootCA) {
        changed = true;
    }
#endif
    // save the new settings
    settings = newSettings;

    if (changed) {
        emsesp::EMSESP::mqtt_.reset_mqtt();
    }

    return StateUpdateResult::CHANGED;
}
