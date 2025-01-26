#include "MqttSettingsService.h"

#include <emsesp.h>

MqttSettingsService::MqttSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _httpEndpoint(MqttSettings::read, MqttSettings::update, this, server, MQTT_SETTINGS_SERVICE_PATH, securityManager)
    , _fsPersistence(MqttSettings::read, MqttSettings::update, this, fs, MQTT_SETTINGS_FILE)
    , _reconfigureMqtt(false)
    , _disconnectedAt(0)
    , _disconnectReason(espMqttClientTypes::DisconnectReason::TCP_DISCONNECTED)
    , _mqttClient(nullptr) {
    WiFi.onEvent([this](WiFiEvent_t event, WiFiEventInfo_t info) { WiFiEvent(event); });
    addUpdateHandler([this] { onConfigUpdated(); }, false);
}

static String generateClientId() {
#ifdef EMSESP_STANDALONE
    return "ems-esp";
#else
    return "esp32-" + String(static_cast<uint32_t>(ESP.getEfuseMac()), HEX);
#endif
}

MqttSettingsService::~MqttSettingsService() {
    delete _mqttClient;
}

void MqttSettingsService::begin() {
    _fsPersistence.readFromFS();
    startClient();
}

void MqttSettingsService::startClient() {
    static bool isSecure = false;
    if (_mqttClient != nullptr) {
        // do we need to change the client?
        if ((isSecure && _state.enableTLS) || (!isSecure && _state.enableTLS)) {
            return;
        }
        delete _mqttClient;
        _mqttClient = nullptr;
    }
#ifndef TASMOTA_SDK
    if (_state.enableTLS) {
        isSecure = true;
        if (emsesp::EMSESP::system_.PSram() > 0) {
            _mqttClient = new espMqttClientSecure(espMqttClientTypes::UseInternalTask::YES);
        } else {
            _mqttClient = new espMqttClientSecure(espMqttClientTypes::UseInternalTask::NO);
        }
        if (_state.rootCA == "insecure") {
            static_cast<espMqttClientSecure *>(_mqttClient)->setInsecure();
        } else {
            String certificate = "-----BEGIN CERTIFICATE-----\n" + _state.rootCA + "\n-----END CERTIFICATE-----\n";
            static_cast<espMqttClientSecure *>(_mqttClient)->setCACert(certificate.c_str());
        }
        static_cast<espMqttClientSecure *>(_mqttClient)->onConnect([this](bool sessionPresent) { onMqttConnect(sessionPresent); });
        static_cast<espMqttClientSecure *>(_mqttClient)->onDisconnect([this](espMqttClientTypes::DisconnectReason reason) { onMqttDisconnect(reason); });
        static_cast<espMqttClientSecure *>(_mqttClient)
            ->onMessage(
                [this](const espMqttClientTypes::MessageProperties & properties, const char * topic, const uint8_t * payload, size_t len, size_t index, size_t total) {
                    onMqttMessage(properties, topic, payload, len, index, total);
                });
        return;
    }
#endif
    isSecure = false;
    if (emsesp::EMSESP::system_.PSram() > 0) {
        _mqttClient = new espMqttClient(espMqttClientTypes::UseInternalTask::YES);
    } else {
        _mqttClient = new espMqttClient(espMqttClientTypes::UseInternalTask::NO);
    }
    static_cast<espMqttClient *>(_mqttClient)->onConnect([this](bool sessionPresent) { onMqttConnect(sessionPresent); });
    static_cast<espMqttClient *>(_mqttClient)->onDisconnect([this](espMqttClientTypes::DisconnectReason reason) { onMqttDisconnect(reason); });
    static_cast<espMqttClient *>(_mqttClient)
        ->onMessage(
            [this](const espMqttClientTypes::MessageProperties & properties, const char * topic, const uint8_t * payload, size_t len, size_t index, size_t total) {
                onMqttMessage(properties, topic, payload, len, index, total);
            });
}

void MqttSettingsService::loop() {
    if (_reconfigureMqtt || (_disconnectedAt && static_cast<uint32_t>(uuid::get_uptime() - _disconnectedAt) >= MQTT_RECONNECTION_DELAY)) {
        // reconfigure MQTT client
        _disconnectedAt  = configureMqtt() ? 0 : uuid::get_uptime();
        _reconfigureMqtt = false;
    }
    if (emsesp::EMSESP::system_.PSram() == 0) {
        _mqttClient->loop();
    }
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

void MqttSettingsService::onMqttMessage(const espMqttClientTypes::MessageProperties & properties,
                                        const char *                                  topic,
                                        const uint8_t *                               payload,
                                        size_t                                        len,
                                        size_t                                        index,
                                        size_t                                        total) {
    (void)properties;
    (void)index;
    (void)total;
    emsesp::EMSESP::mqtt_.on_message(topic, payload, len);
}

espMqttClientTypes::DisconnectReason MqttSettingsService::getDisconnectReason() {
    return _disconnectReason;
}

MqttClient * MqttSettingsService::getMqttClient() {
    return _mqttClient;
}

void MqttSettingsService::onMqttConnect(bool sessionPresent) {
    (void)sessionPresent;
    emsesp::EMSESP::mqtt_.on_connect();
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

    startClient();
    emsesp::EMSESP::mqtt_.start(); // reload EMS-ESP MQTT settings
}

void MqttSettingsService::WiFiEvent(WiFiEvent_t event) {
    switch (event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
    case ARDUINO_EVENT_ETH_GOT_IP:
    case ARDUINO_EVENT_ETH_GOT_IP6:
    case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
        if (_state.enabled && !_mqttClient->connected()) {
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
    // disconnect if already connected
    if (_mqttClient->connected()) {
        emsesp::EMSESP::logger().info("Disconnecting to configure");
        _mqttClient->disconnect(true);
    }

    // only connect if WiFi is connected and MQTT is enabled
    if (_state.enabled && emsesp::EMSESP::system_.network_connected() && !_state.host.isEmpty()) {
        // create last will topic with the base prefixed. It has to be static because the client destroys the reference
        static char will_topic[FACTORY_MQTT_MAX_TOPIC_LENGTH];
        if (_state.base.isEmpty()) {
            snprintf(will_topic, sizeof(will_topic), "status");
        } else {
            snprintf(will_topic, sizeof(will_topic), "%s/status", _state.base.c_str());
        }

        _reconfigureMqtt = false;
#ifndef TASMOTA_SDK
        if (_state.enableTLS) {
#if defined(EMSESP_DEBUG)
            emsesp::EMSESP::logger().debug("Start secure MQTT with rootCA");
#endif
            static_cast<espMqttClientSecure *>(_mqttClient)->setServer(_state.host.c_str(), _state.port);
            if (_state.username.length() > 0) {
                static_cast<espMqttClientSecure *>(_mqttClient)
                    ->setCredentials(_state.username.c_str(), _state.password.length() > 0 ? _state.password.c_str() : nullptr);
            }
            static_cast<espMqttClientSecure *>(_mqttClient)->setClientId(_state.clientId.c_str());
            static_cast<espMqttClientSecure *>(_mqttClient)->setKeepAlive(_state.keepAlive);
            static_cast<espMqttClientSecure *>(_mqttClient)->setCleanSession(_state.cleanSession);
            static_cast<espMqttClientSecure *>(_mqttClient)->setWill(will_topic, 1, true, "offline"); // QOS 1, retain
            return _mqttClient->connect();
        }
#endif
        static_cast<espMqttClient *>(_mqttClient)->setServer(_state.host.c_str(), _state.port);
        if (_state.username.length() > 0) {
            static_cast<espMqttClient *>(_mqttClient)->setCredentials(_state.username.c_str(), _state.password.length() > 0 ? _state.password.c_str() : nullptr);
        }
        static_cast<espMqttClient *>(_mqttClient)->setClientId(_state.clientId.c_str());
        static_cast<espMqttClient *>(_mqttClient)->setKeepAlive(_state.keepAlive);
        static_cast<espMqttClient *>(_mqttClient)->setCleanSession(_state.cleanSession);
        static_cast<espMqttClient *>(_mqttClient)->setWill(will_topic, 1, true, "offline"); // QOS 1, retain
        return _mqttClient->connect();
    }

    return false;
}

void MqttSettings::read(MqttSettings & settings, JsonObject root) {
#ifndef TASMOTA_SDK
    root["enableTLS"] = settings.enableTLS;
    root["rootCA"]    = settings.rootCA;
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

    root["publish_time_boiler"]     = settings.publish_time_boiler;
    root["publish_time_thermostat"] = settings.publish_time_thermostat;
    root["publish_time_solar"]      = settings.publish_time_solar;
    root["publish_time_mixer"]      = settings.publish_time_mixer;
    root["publish_time_water"]      = settings.publish_time_water;
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

StateUpdateResult MqttSettings::update(JsonObject root, MqttSettings & settings) {
    MqttSettings newSettings = {};
    bool         changed     = false;

#ifndef TASMOTA_SDK
    newSettings.enableTLS = root["enableTLS"];
    newSettings.rootCA    = root["rootCA"] | "";
#else
    newSettings.enableTLS = false;
#endif
    newSettings.enabled      = root["enabled"] | FACTORY_MQTT_ENABLED;
    newSettings.host         = root["host"] | FACTORY_MQTT_HOST;
    newSettings.port         = static_cast<uint16_t>(root["port"] | FACTORY_MQTT_PORT);
    newSettings.base         = root["base"] | FACTORY_MQTT_BASE;
    newSettings.username     = root["username"] | FACTORY_MQTT_USERNAME;
    newSettings.password     = root["password"] | FACTORY_MQTT_PASSWORD;
    newSettings.clientId     = root["client_id"] | generateClientId();
    newSettings.keepAlive    = static_cast<uint16_t>(root["keep_alive"] | FACTORY_MQTT_KEEP_ALIVE);
    newSettings.cleanSession = root["clean_session"] | FACTORY_MQTT_CLEAN_SESSION;
    newSettings.mqtt_qos     = static_cast<uint8_t>(root["mqtt_qos"] | EMSESP_DEFAULT_MQTT_QOS);
    newSettings.mqtt_retain  = root["mqtt_retain"] | EMSESP_DEFAULT_MQTT_RETAIN;

    newSettings.publish_time_boiler     = static_cast<uint16_t>(root["publish_time_boiler"] | EMSESP_DEFAULT_PUBLISH_TIME);
    newSettings.publish_time_thermostat = static_cast<uint16_t>(root["publish_time_thermostat"] | EMSESP_DEFAULT_PUBLISH_TIME);
    newSettings.publish_time_solar      = static_cast<uint16_t>(root["publish_time_solar"] | EMSESP_DEFAULT_PUBLISH_TIME);
    newSettings.publish_time_mixer      = static_cast<uint16_t>(root["publish_time_mixer"] | EMSESP_DEFAULT_PUBLISH_TIME);
    newSettings.publish_time_water      = static_cast<uint16_t>(root["publish_time_water"] | EMSESP_DEFAULT_PUBLISH_TIME);
    newSettings.publish_time_other      = static_cast<uint16_t>(root["publish_time_other"] | EMSESP_DEFAULT_PUBLISH_TIME_OTHER);
    newSettings.publish_time_sensor     = static_cast<uint16_t>(root["publish_time_sensor"] | EMSESP_DEFAULT_PUBLISH_TIME);
    newSettings.publish_time_heartbeat  = static_cast<uint16_t>(root["publish_time_heartbeat"] | EMSESP_DEFAULT_PUBLISH_HEARTBEAT);

    newSettings.ha_enabled         = root["ha_enabled"] | EMSESP_DEFAULT_HA_ENABLED;
    newSettings.nested_format      = static_cast<uint8_t>(root["nested_format"] | EMSESP_DEFAULT_NESTED_FORMAT);
    newSettings.discovery_prefix   = root["discovery_prefix"] | EMSESP_DEFAULT_DISCOVERY_PREFIX;
    newSettings.discovery_type     = static_cast<uint8_t>(root["discovery_type"] | EMSESP_DEFAULT_DISCOVERY_TYPE);
    newSettings.publish_single     = root["publish_single"] | EMSESP_DEFAULT_PUBLISH_SINGLE;
    newSettings.publish_single2cmd = root["publish_single2cmd"] | EMSESP_DEFAULT_PUBLISH_SINGLE2CMD;
    newSettings.send_response      = root["send_response"] | EMSESP_DEFAULT_SEND_RESPONSE;
    newSettings.entity_format      = static_cast<uint8_t>(root["entity_format"] | EMSESP_DEFAULT_ENTITY_FORMAT);

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

    if (newSettings.publish_time_water != settings.publish_time_water) {
        emsesp::EMSESP::mqtt_.set_publish_time_water(newSettings.publish_time_water);
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

#ifndef TASMOTA_SDK
    // strip down to certificate only
    newSettings.rootCA.replace("\r", "");
    newSettings.rootCA.replace("\n", "");
    newSettings.rootCA.replace("-----BEGIN CERTIFICATE-----", "");
    newSettings.rootCA.replace("-----END CERTIFICATE-----", "");
    newSettings.rootCA.replace(" ", "");
    if (newSettings.rootCA.length() == 0 && newSettings.enableTLS) {
        newSettings.rootCA = "insecure";
    }
    if (newSettings.enableTLS != settings.enableTLS || newSettings.rootCA != settings.rootCA) {
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
