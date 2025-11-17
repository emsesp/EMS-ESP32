/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2024  emsesp.org - proddy, MichaelDvP
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mqtt.h"
#include "emsesp.h"
#include "emsdevice.h"

namespace emsesp {

MqttClient * Mqtt::mqttClient_;

// static parameters we make global
std::string Mqtt::mqtt_base_;
std::string Mqtt::mqtt_basename_; // base name for MQTT topics with / replaced with _. Used for uniq_id in HA
uint8_t     Mqtt::mqtt_qos_;
bool        Mqtt::mqtt_retain_;
uint32_t    Mqtt::publish_time_boiler_;
uint32_t    Mqtt::publish_time_thermostat_;
uint32_t    Mqtt::publish_time_solar_;
uint32_t    Mqtt::publish_time_mixer_;
uint32_t    Mqtt::publish_time_water_;
uint32_t    Mqtt::publish_time_sensor_;
uint32_t    Mqtt::publish_time_other_;
uint32_t    Mqtt::publish_time_heartbeat_;
bool        Mqtt::mqtt_enabled_;
uint8_t     Mqtt::entity_format_;
bool        Mqtt::ha_enabled_;
uint8_t     Mqtt::nested_format_;
std::string Mqtt::discovery_prefix_;
uint8_t     Mqtt::discovery_type_;
bool        Mqtt::send_response_;
bool        Mqtt::publish_single_;
bool        Mqtt::publish_single2cmd_;

std::vector<Mqtt::MQTTSubFunction> Mqtt::mqtt_subfunctions_;

uint32_t Mqtt::mqtt_publish_fails_ = 0;
bool     Mqtt::connecting_         = false;
bool     Mqtt::initialized_        = false;
bool     Mqtt::ha_climate_reset_   = false;
uint16_t Mqtt::queuecount_         = 0;
uint8_t  Mqtt::connectcount_       = 0;
uint32_t Mqtt::mqtt_message_id_    = 0;
char     will_topic_[Mqtt::MQTT_TOPIC_MAX_SIZE]; // because MQTT library keeps only char pointer

std::string Mqtt::lasttopic_    = "";
std::string Mqtt::lastpayload_  = "";
std::string Mqtt::lastresponse_ = "";

// Home Assistant specific
// icons from https://materialdesignicons.com used with the UOMs (unit of measurements)
MAKE_WORD(measurement)
MAKE_WORD(total_increasing)
// MAKE_WORD_CUSTOM(icondegrees, "mdi:coolant-temperature") // DeviceValueUOM::DEGREES
MAKE_WORD_CUSTOM(iconpercent, "mdi:percent-outline")  // DeviceValueUOM::PERCENT
MAKE_WORD_CUSTOM(iconkb, "mdi:memory")                // DeviceValueUOM::KB
MAKE_WORD_CUSTOM(iconlmin, "mdi:water-boiler")        // DeviceValueUOM::LMIN
MAKE_WORD_CUSTOM(iconua, "mdi:lightning-bolt-circle") // DeviceValueUOM::UA
MAKE_WORD_CUSTOM(iconnum, "mdi:counter")              // DeviceValueUOM::NONE

uuid::log::Logger Mqtt::logger_{F_(mqtt), uuid::log::Facility::DAEMON};

// subscribe to an MQTT topic, and store the associated callback function
// only if it already hasn't been added
// topics exclude the base
void Mqtt::subscribe(const uint8_t device_type, const std::string & topic, mqtt_sub_function_p cb) {
    // check if we already have the topic subscribed for this specific device type, if so don't add it again
    // add the function (in case its not there) and quit because it already exists
    if (!mqtt_subfunctions_.empty()) {
        for (auto & mqtt_subfunction : mqtt_subfunctions_) {
            if ((mqtt_subfunction.device_type_ == device_type) && (strcmp(mqtt_subfunction.topic_.c_str(), topic.c_str()) == 0)) {
                if (cb) {
                    mqtt_subfunction.mqtt_subfunction_ = cb;
                }
                return; // exit - don't add
            }
        }
    }

    // register in our libary with the callback function.
    // We store the original topic string without base
    // removed std::move(topic) in 3.7.0-dev.43
    mqtt_subfunctions_.emplace_back(device_type, topic, cb);

    if (!enabled() || !connected()) {
        return;
    }

    // add to MQTT queue as a subscribe operation
    queue_subscribe_message(topic);
}

// subscribe without storing to subfunctions
void Mqtt::subscribe(const std::string & topic) {
    // add to MQTT queue as a subscribe operation
    queue_subscribe_message(topic);
}

// resubscribe to all MQTT topics
// if it's already in the queue then just ignore it
void Mqtt::resubscribe() {
    if (mqtt_subfunctions_.empty()) {
        return;
    }

    for (const auto & mqtt_subfunction : mqtt_subfunctions_) {
        queue_subscribe_message(mqtt_subfunction.topic_);
    }
}

// Main MQTT loop - sends out top item on publish queue
void Mqtt::loop() {
    // exit if MQTT is not enabled or if there is no network connection
    if (!connected()) {
        return;
    }

    queuecount_ = mqttClient_->queueSize();

    uint32_t currentMillis = uuid::get_uptime();

    // send heartbeat
    if (currentMillis - last_publish_heartbeat_ > publish_time_heartbeat_) {
        last_publish_heartbeat_ = currentMillis;
        EMSESP::system_.send_heartbeat(); // send heartbeat
    }

    // temperature and analog sensor publish on change
    if (!publish_time_sensor_) {
        EMSESP::publish_sensor_values(false);
    }

    // wait for empty queue before sending scheduled device messages
    if (queuecount_ > 0) {
        return;
    }

    // create publish messages for each of the EMS device values, adding to queue, only one device per loop
    if (publish_time_boiler_ && (currentMillis - last_publish_boiler_ > publish_time_boiler_)) {
        last_publish_boiler_ = (currentMillis / publish_time_boiler_) * publish_time_boiler_;
        EMSESP::publish_device_values(EMSdevice::DeviceType::BOILER);
    } else

        if (publish_time_thermostat_ && (currentMillis - last_publish_thermostat_ > publish_time_thermostat_)) {
        last_publish_thermostat_ = (currentMillis / publish_time_thermostat_) * publish_time_thermostat_;
        EMSESP::publish_device_values(EMSdevice::DeviceType::THERMOSTAT);
    } else

        if (publish_time_solar_ && (currentMillis - last_publish_solar_ > publish_time_solar_)) {
        last_publish_solar_ = (currentMillis / publish_time_solar_) * publish_time_solar_;
        EMSESP::publish_device_values(EMSdevice::DeviceType::SOLAR);
    } else

        if (publish_time_mixer_ && (currentMillis - last_publish_mixer_ > publish_time_mixer_)) {
        last_publish_mixer_ = (currentMillis / publish_time_mixer_) * publish_time_mixer_;
        EMSESP::publish_device_values(EMSdevice::DeviceType::MIXER);
    } else

        if (publish_time_water_ && (currentMillis - last_publish_water_ > publish_time_water_)) {
        last_publish_water_ = (currentMillis / publish_time_water_) * publish_time_water_;
        EMSESP::publish_device_values(EMSdevice::DeviceType::WATER);
    } else

        if (publish_time_other_ && (currentMillis - last_publish_other_ > publish_time_other_)) {
        last_publish_other_ = (currentMillis / publish_time_other_) * publish_time_other_;
        EMSESP::publish_other_values(); // switch and heatpump
    } else

        if (publish_time_sensor_ && (currentMillis - last_publish_sensor_ > publish_time_sensor_)) {
        last_publish_sensor_ = (currentMillis / publish_time_sensor_) * publish_time_sensor_;
        EMSESP::publish_sensor_values(true);
    }
}

// print MQTT log and other stuff to console
void Mqtt::show_mqtt(uuid::console::Shell & shell) {
    shell.printfln("MQTT is %s", connected() ? F_(connected) : F_(disconnected));
    shell.printfln("MQTT Entity ID format is %d", entity_format_);

    shell.printfln("MQTT publish errors: %lu", mqtt_publish_fails_);
    shell.printfln("MQTT queue: %d", queuecount_);
    shell.println();

    // show subscriptions
    shell.printfln("MQTT topic subscriptions:");
    for (const auto & mqtt_subfunction : mqtt_subfunctions_) {
        shell.printfln(" %s/%s", Mqtt::base().c_str(), mqtt_subfunction.topic_.c_str());
    }

    shell.println();
    shell.println();
}

#if defined(EMSESP_TEST)
// simulate receiving a MQTT message, used only for testing
void Mqtt::incoming(const char * topic, const char * payload) {
    if (payload != nullptr) {
        on_message(topic, (const uint8_t *)payload, strlen(payload));
    }
}
#endif

// received an MQTT message that we subscribed too
// topic is the full path
// payload is json or a single string and converted to a json with key 'value'
void Mqtt::on_message(const char * topic, const uint8_t * payload, size_t len) {
    // the payload is not terminated
    // convert payload to a null-terminated char string
    // see https://www.emelis.net/espMqttClient/#code-samples
    char message[len + 1];
    memcpy(message, payload, len);
    message[len] = '\0';

#if defined(EMSESP_DEBUG)
    if (len) {
        LOG_DEBUG("Received topic %s => payload '%s' (length %d)", topic, message, len);
    } else {
        LOG_DEBUG("Received topic %s", topic);
    }
#endif
    // remove HA topics if we don't use discovery
    if (strncmp(topic, discovery_prefix().c_str(), discovery_prefix().size()) == 0) {
        if (!ha_enabled_ && len) { // don't ping pong the empty message
            queue_publish_message(topic, "", true);
            LOG_DEBUG("Remove topic %s", topic);
        }
        return;
    }

    // for misconfigured mqtt servers and publish2command ignore echos
    if (publish_single_ && publish_single2cmd_ && lasttopic_ == topic && lastpayload_ == message) {
        LOG_DEBUG("Received echo message %s: %s", topic, message);
        return;
    }

    // check first against any of our subscribed topics
    for (const auto & mf : mqtt_subfunctions_) {
        // add the base back
        char full_topic[MQTT_TOPIC_MAX_SIZE];
        snprintf(full_topic, sizeof(full_topic), "%s/%s", Mqtt::base().c_str(), mf.topic_.c_str());

        if ((!strcmp(topic, full_topic)) && (mf.mqtt_subfunction_)) {
            if (!(mf.mqtt_subfunction_)(message)) {
                LOG_ERROR("error: invalid payload %s for this topic %s", message, topic);
                Mqtt::queue_publish("response", "error: invalid data");
            }
            return;
        }
    }

    JsonDocument input_doc;
    JsonDocument output_doc;
    JsonObject   input;
    JsonObject   output;

    // convert payload into a json doc
    // if the payload doesn't not contain the key 'value' or 'data', treat the whole payload as the 'value'
    if (len != 0) {
        DeserializationError error = deserializeJson(input_doc, (const char *)message);
        if (((!input_doc["value"].is<JsonVariantConst>()) && (!input_doc["data"].is<JsonVariantConst>())) || error) {
            input_doc.clear();
            input_doc["value"] = (const char *)message; // always a string
        }
    }

    // parse and call the command
    input               = input_doc.as<JsonObject>();
    output              = output_doc.to<JsonObject>();
    uint8_t return_code = Command::process(topic, true, input, output); // mqtt is always authenticated

    if (return_code != CommandRet::OK) {
        char error[100];
        if (output.size()) {
            snprintf(error, sizeof(error), "MQTT command failed with error %s (%s)", (const char *)output["message"], Command::return_code_string(return_code));
        } else {
            snprintf(error, sizeof(error), "MQTT command failed with error %s", Command::return_code_string(return_code));
        }
        LOG_ERROR(error);
        Mqtt::queue_publish("response", error);
    } else {
        // all good, send back json output from call
        Mqtt::queue_publish("response", output);
    }
}

// print all the topics related to a specific device type
void Mqtt::show_topic_handlers(uuid::console::Shell & shell, const uint8_t device_type) {
    if (std::count_if(mqtt_subfunctions_.cbegin(),
                      mqtt_subfunctions_.cend(),
                      [=](MQTTSubFunction const & mqtt_subfunction) { return device_type == mqtt_subfunction.device_type_; })
        == 0) {
        return;
    }

    // shell.print(" Subscribed MQTT topics: ");
    // for (const auto & mqtt_subfunction : mqtt_subfunctions_) {
    //     if (mqtt_subfunction.device_type_ == device_type) {
    //         shell.printf("%s ", mqtt_subfunction.topic_.c_str());
    //     }
    // }
    shell.println();
}

// called when an MQTT Publish ACK is received
void Mqtt::on_publish(uint16_t packetId) const {
    LOG_DEBUG("Packet %d sent successful", packetId);
}

// called when MQTT settings have changed via the MQTT Settings or Application Settings Web pages
void Mqtt::reset_mqtt() {
    if (!enabled()) {
        return;
    }

    if (!mqttClient_) {
        return;
    }

    if (mqttClient_->connected()) {
        mqttClient_->disconnect(true); // force a disconnect
    }
}

// load the settings from service
void Mqtt::load_settings() {
    EMSESP::esp32React.getMqttSettingsService()->read([&](MqttSettings & mqttSettings) {
        mqtt_base_          = mqttSettings.base.c_str(); // Convert String to std::string
        mqtt_qos_           = mqttSettings.mqtt_qos;
        mqtt_retain_        = mqttSettings.mqtt_retain;
        mqtt_enabled_       = mqttSettings.enabled;
        ha_enabled_         = mqttSettings.ha_enabled;
        nested_format_      = mqttSettings.nested_format;
        publish_single_     = mqttSettings.publish_single;
        publish_single2cmd_ = mqttSettings.publish_single2cmd;
        send_response_      = mqttSettings.send_response;
        discovery_prefix_   = mqttSettings.discovery_prefix.c_str();
        entity_format_      = mqttSettings.entity_format;
        discovery_type_     = mqttSettings.discovery_type;

        // convert to milliseconds
        publish_time_boiler_     = mqttSettings.publish_time_boiler * 1000;
        publish_time_thermostat_ = mqttSettings.publish_time_thermostat * 1000;
        publish_time_solar_      = mqttSettings.publish_time_solar * 1000;
        publish_time_mixer_      = mqttSettings.publish_time_mixer * 1000;
        publish_time_water_      = mqttSettings.publish_time_water * 1000;
        publish_time_other_      = mqttSettings.publish_time_other * 1000;
        publish_time_sensor_     = mqttSettings.publish_time_sensor * 1000;
        publish_time_heartbeat_  = mqttSettings.publish_time_heartbeat * 1000;
    });

    // create unique ID from the mqtt base replacing all / with underscores, in case it's a path
    basename(mqtt_base_);
}

// start mqtt
void Mqtt::start() {
    mqttClient_ = EMSESP::esp32React.getMqttClient();

    load_settings(); // fetch MQTT settings

    if (!mqtt_enabled_) {
        return; // quit, not using MQTT
    }

    // if already initialized, don't do it again
    // also to prevent duplicated loading from MqttSettingsService::onConfigUpdated()
    if (initialized_) {
        return;
    }
    initialized_ = true;

    // add the 'publish' command ('call system publish' in console or via API)
    Command::add(EMSdevice::DeviceType::SYSTEM, F_(publish), System::command_publish, FL_(publish_cmd));

#if defined(EMSESP_STANDALONE)
    Mqtt::on_connect(); // simulate an MQTT connection
#endif
}

void Mqtt::set_publish_time_boiler(uint16_t publish_time) {
    publish_time_boiler_ = publish_time * 1000; // convert to milliseconds
}

void Mqtt::set_publish_time_thermostat(uint16_t publish_time) {
    publish_time_thermostat_ = publish_time * 1000; // convert to milliseconds
}

void Mqtt::set_publish_time_solar(uint16_t publish_time) {
    publish_time_solar_ = publish_time * 1000; // convert to milliseconds
}

void Mqtt::set_publish_time_mixer(uint16_t publish_time) {
    publish_time_mixer_ = publish_time * 1000; // convert to milliseconds
}

void Mqtt::set_publish_time_water(uint16_t publish_time) {
    publish_time_water_ = publish_time * 1000; // convert to milliseconds
}

void Mqtt::set_publish_time_other(uint16_t publish_time) {
    publish_time_other_ = publish_time * 1000; // convert to milliseconds
}

void Mqtt::set_publish_time_sensor(uint16_t publish_time) {
    publish_time_sensor_ = publish_time * 1000; // convert to milliseconds
}

void Mqtt::set_publish_time_heartbeat(uint16_t publish_time) {
    publish_time_heartbeat_ = publish_time * 1000; // convert to milliseconds
}

bool Mqtt::get_publish_onchange(uint8_t device_type) {
    if (publish_single_ && !ha_enabled_) {
        return false;
    }
    if (device_type == EMSdevice::DeviceType::BOILER) {
        if (!publish_time_boiler_) {
            return true;
        }
    } else if (device_type == EMSdevice::DeviceType::THERMOSTAT) {
        if (!publish_time_thermostat_) {
            return true;
        }
    } else if (device_type == EMSdevice::DeviceType::SOLAR) {
        if (!publish_time_solar_) {
            return true;
        }
    } else if (device_type == EMSdevice::DeviceType::MIXER) {
        if (!publish_time_mixer_) {
            return true;
        }
    } else if (device_type == EMSdevice::DeviceType::WATER) {
        if (!publish_time_water_) {
            return true;
        }
    } else if (!publish_time_other_) {
        return true;
    }
    return false;
}
void Mqtt::on_disconnect(espMqttClientTypes::DisconnectReason reason) {
    // only show the error once, not every 2 seconds
    if (!connecting_) {
        return;
    }
    connecting_ = false;
    connectcount_++; // count # reconnects

    if (reason == espMqttClientTypes::DisconnectReason::TCP_DISCONNECTED) {
        LOG_WARNING("MQTT disconnected: TCP");
    } else if (reason == espMqttClientTypes::DisconnectReason::MQTT_UNACCEPTABLE_PROTOCOL_VERSION) {
        LOG_WARNING("MQTT disconnected: Unacceptable protocol version");
    } else if (reason == espMqttClientTypes::DisconnectReason::MQTT_IDENTIFIER_REJECTED) {
        LOG_WARNING("MQTT disconnected: Identifier Rejected");
    } else if (reason == espMqttClientTypes::DisconnectReason::MQTT_SERVER_UNAVAILABLE) {
        LOG_WARNING("MQTT disconnected: Server unavailable");
    } else if (reason == espMqttClientTypes::DisconnectReason::MQTT_MALFORMED_CREDENTIALS) {
        LOG_WARNING("MQTT disconnected: Malformed credentials");
    } else if (reason == espMqttClientTypes::DisconnectReason::MQTT_NOT_AUTHORIZED) {
        LOG_WARNING("MQTT disconnected: Not authorized");
    } else if (reason == espMqttClientTypes::DisconnectReason::TLS_BAD_FINGERPRINT) {
        LOG_WARNING("MQTT disconnected: Server fingerprint invalid");
    } else {
        LOG_WARNING("MQTT disconnected: code %d", reason);
    }

    mqttClient_->clearQueue(true);
}

// MQTT on_connect - when an MQTT connect is established
void Mqtt::on_connect() {
    if (connecting_) {
        return; // prevent duplicated connections
    }

    LOG_INFO("MQTT connected");

    connecting_ = true;
    queuecount_ = mqttClient_->queueSize();

    load_settings(); // reload MQTT settings - in case they have changes

    if (ha_enabled_) {
        queue_unsubscribe_message(discovery_prefix_ + "/+/" + Mqtt::basename() + "/#");
        EMSESP::reset_mqtt_ha(); // re-create all HA devices if there are any
        ha_status();             // create the EMS-ESP device in HA, which is MQTT retained
        ha_climate_reset(true);
    } else {
        // with disabled HA we subscribe and the broker sends all stored HA-emsesp-configs.
        // Around line 272 they are removed (search for "// remove HA topics if we don't use discover")
        // If HA is enabled the subscriptions are removed.
        // As described in the doc (https://docs.emsesp.org/Troubleshooting?id=home-assistant):
        // disable HA, wait 5 minutes (to allow the broker to send all), than reenable HA again.
        queue_subscribe_message(discovery_prefix_ + "/+/" + Mqtt::basename() + "/#");
    }

    // send initial MQTT messages for some of our services
    EMSESP::system_.send_heartbeat(); // send heartbeat

    // re-subscribe to all custom registered MQTT topics
    resubscribe();

    // publish to the last will topic (see Mqtt::start() function) to say we're alive
    queue_publish_retain("status", "online"); // retain: https://github.com/emsesp/EMS-ESP32/discussions/2086
}

// Home Assistant Discovery - the main master Device called EMS-ESP
// e.g. homeassistant/sensor/ems-esp/status/config
// all the values from the heartbeat payload will be added as attributes to the entity state
void Mqtt::ha_status() {
    JsonDocument doc;

    char uniq[70];
    if (Mqtt::entity_format() == entityFormat::MULTI_SHORT) {
        snprintf(uniq, sizeof(uniq), "%s_system_status", Mqtt::basename().c_str());
    } else {
        strcpy(uniq, "system_status");
    }

    doc["uniq_id"]    = uniq;
    doc["def_ent_id"] = (std::string) "binary_sensor." + uniq;
    doc["stat_t"]     = Mqtt::base() + "/status";
    doc["name"]       = "System status";
    doc["pl_on"]      = "online";
    doc["pl_off"]     = "offline";
    doc["stat_cla"]   = "measurement";
    doc["dev_cla"]    = "connectivity";
    doc["ent_cat"]    = "diagnostic";

    // doc["avty_t"]      = "~/status"; // commented out, as it causes errors in HA sometimes
    // doc["json_attr_t"] = "~/heartbeat"; // store also as HA attributes

    JsonObject dev = doc["dev"].to<JsonObject>();
    dev["name"]    = Mqtt::basename();
    dev["sw"]      = "v" + std::string(EMSESP_APP_VERSION);
    dev["mf"]      = "EMS-ESP";
    dev["mdl"]     = "EMS-ESP";
#ifndef EMSESP_STANDALONE
    dev["cu"] = "http://" + (EMSESP::system_.ethernet_connected() ? ETH.localIP().toString() : WiFi.localIP().toString());
#endif
    JsonArray ids = dev["ids"].to<JsonArray>();
    ids.add(Mqtt::basename());

    char topic[MQTT_TOPIC_MAX_SIZE];
    snprintf(topic, sizeof(topic), "binary_sensor/%s/system_status/config", Mqtt::basename().c_str());
    Mqtt::queue_ha(topic, doc.as<JsonObject>()); // publish the config payload with retain flag

// create the HA sensors - must match the MQTT payload keys in the heartbeat topic
// Note we don't use camelCase as it would change the HA entity_id and impact historic data
#ifndef EMSESP_STANDALONE
    if (!EMSESP::system_.ethernet_connected() || WiFi.isConnected()) {
        publish_system_ha_sensor_config(DeviceValueType::INT8, "WiFi RSSI", "rssi", DeviceValueUOM::DBM);
        publish_system_ha_sensor_config(DeviceValueType::INT8, "WiFi strength", "wifistrength", DeviceValueUOM::PERCENT);
    }
#endif

    publish_system_ha_sensor_config(DeviceValueType::STRING, "EMS Bus", "bus_status", DeviceValueUOM::NONE);
    publish_system_ha_sensor_config(DeviceValueType::STRING, "Uptime", "uptime", DeviceValueUOM::NONE);
    publish_system_ha_sensor_config(DeviceValueType::INT8, "Uptime (sec)", "uptime_sec", DeviceValueUOM::SECONDS);
    publish_system_ha_sensor_config(DeviceValueType::INT8, "Free memory", "freemem", DeviceValueUOM::KB);
    publish_system_ha_sensor_config(DeviceValueType::INT8, "Max alloc", "max_alloc", DeviceValueUOM::KB);
    publish_system_ha_sensor_config(DeviceValueType::INT8, "MQTT fails", "mqttfails", DeviceValueUOM::NONE);
    publish_system_ha_sensor_config(DeviceValueType::INT8, "Rx received", "rxreceived", DeviceValueUOM::NONE);
    publish_system_ha_sensor_config(DeviceValueType::INT8, "Rx fails", "rxfails", DeviceValueUOM::NONE);
    publish_system_ha_sensor_config(DeviceValueType::INT8, "Tx reads", "txreads", DeviceValueUOM::NONE);
    publish_system_ha_sensor_config(DeviceValueType::INT8, "Tx writes", "txwrites", DeviceValueUOM::NONE);
    publish_system_ha_sensor_config(DeviceValueType::INT8, "Tx fails", "txfails", DeviceValueUOM::NONE);

#if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S2
    publish_system_ha_sensor_config(DeviceValueType::INT8, "CPU temperature", "temperature", DeviceValueUOM::DEGREES);
#endif

    if (!EMSESP::system_.ethernet_connected()) {
        publish_system_ha_sensor_config(DeviceValueType::INT16, "WiFi reconnects", "wifireconnects", DeviceValueUOM::NONE);
    }
    // This one comes from the info MQTT topic - and handled in the publish_ha_sensor_config function
    publish_system_ha_sensor_config(DeviceValueType::STRING, "Version", "version", DeviceValueUOM::NONE);
}

// add sub or pub task to the queue.
// the base is not included in the topic
bool Mqtt::queue_message(const uint8_t operation, const std::string & topic, const std::string & payload, const bool retain) {
    if (topic == "response" && operation == Operation::PUBLISH) {
        lastresponse_ = payload;
        if (!send_response_) {
            return true;
        }
    }
    if (!mqtt_enabled_ || topic.empty() || !connected()) {
        return false; // quit, not using MQTT
    }
// check free mem
#ifndef EMSESP_STANDALONE
    // if (ESP.getFreeHeap() < 60 * 1024 || ESP.getMaxAllocHeap() < 40 * 1024) {
    if (heap_caps_get_free_size(MALLOC_CAP_8BIT) < 60 * 1024) { // checks free Heap+PSRAM
        if (operation == Operation::PUBLISH) {
            mqtt_message_id_++;
            mqtt_publish_fails_++;
        }
        LOG_WARNING("%s failed: low memory", operation == Operation::PUBLISH ? "Publish" : operation == Operation::SUBSCRIBE ? "Subscribe" : "Unsubscribe");
        return false; // quit
    }
    if (queuecount_ >= MQTT_QUEUE_MAX_SIZE) {
        if (operation == Operation::PUBLISH) {
            mqtt_message_id_++;
            mqtt_publish_fails_++;
        }
        LOG_WARNING("%s failed: queue full", operation == Operation::PUBLISH ? "Publish" : operation == Operation::SUBSCRIBE ? "Subscribe" : "Unsubscribe");
        return false; // quit
    }
#endif

    uint16_t packet_id = 0;
    char     fulltopic[MQTT_TOPIC_MAX_SIZE];

    if (topic.find(discovery_prefix_) == 0) {
        strlcpy(fulltopic, topic.c_str(), sizeof(fulltopic)); // leave discovery topic as it is
    } else {
        // it's not a discovery topic, added the mqtt base to the topic path
        snprintf(fulltopic, sizeof(fulltopic), "%s/%s", Mqtt::base().c_str(), topic.c_str());
    }

    if (operation == Operation::PUBLISH) {
        packet_id = mqttClient_->publish(fulltopic, mqtt_qos_, retain, payload.c_str());
        mqtt_message_id_++;
        LOG_DEBUG("Publishing topic '%s', pid %d", fulltopic, packet_id);
    } else if (operation == Operation::SUBSCRIBE) {
        packet_id = mqttClient_->subscribe(fulltopic, mqtt_qos_);
        LOG_DEBUG("Subscribing to topic '%s', pid %d", fulltopic, packet_id);
    } else if (operation == Operation::UNSUBSCRIBE) {
        packet_id = mqttClient_->unsubscribe(fulltopic);
        LOG_DEBUG("Unsubscribing to topic '%s', pid %d", fulltopic, packet_id);
    }
#ifndef EMSESP_STANDALONE
    if (packet_id == 0) {
        LOG_WARNING("%s failed: %s", operation == Operation::PUBLISH ? "Publish" : operation == Operation::SUBSCRIBE ? "Subscribe" : "Unsubscribe", fulltopic);
        mqtt_publish_fails_++;
    }
#endif
    return (packet_id != 0);
}

// add MQTT subscribe message to queue
void Mqtt::queue_subscribe_message(const std::string & topic) {
    queue_message(Operation::SUBSCRIBE, topic, "", false); // no payload, no retain
}

// add MQTT unsubscribe message to queue
void Mqtt::queue_unsubscribe_message(const std::string & topic) {
    queue_message(Operation::UNSUBSCRIBE, topic, "", false); // no payload, no retain
}

// internal function to add MQTT message to queue, payload is a string
bool Mqtt::queue_publish_message(const std::string & topic, const std::string & payload, const bool retain) {
    return queue_message(Operation::PUBLISH, topic, payload, retain);
}

// MQTT Publish, using the user's retain flag
bool Mqtt::queue_publish(const std::string & topic, const std::string & payload) {
    return queue_publish_message(topic, payload, mqtt_retain_);
}

// MQTT Publish, using the user's retain flag - except for char * strings payload
bool Mqtt::queue_publish(const char * topic, const char * payload) {
    return queue_publish_message(topic, payload, mqtt_retain_);
}

// MQTT Publish, using the user's retain flag - std::string payload
bool Mqtt::queue_publish(const char * topic, const std::string & payload) {
    return queue_publish_message(topic, payload, mqtt_retain_);
}

bool Mqtt::queue_publish(const char * topic, const JsonObjectConst payload) {
    return queue_publish(topic, payload, mqtt_retain_);
}

// MQTT Publish, for json doc, only if its not empty - with retain flag set
bool Mqtt::queue_publish(const std::string & topic, const JsonObjectConst payload) {
    return queue_publish_retain(topic, payload);
}

// MQTT Publish, topic and payload are strings, forcing retain flag
bool Mqtt::queue_publish_retain(const char * topic, const std::string & payload) {
    return queue_publish_message(topic, payload, true);
}

// publish json doc, only if its not empty, forcing retain flag
bool Mqtt::queue_publish_retain(const std::string & topic, const JsonObjectConst payload) {
    return queue_publish(topic.c_str(), payload, true);
}

// publish json doc, only if its not empty, forcing retain flag
bool Mqtt::queue_publish_retain(const char * topic, const JsonObjectConst payload) {
    return queue_publish(topic, payload, true);
}

// MQTT Publish, for char * strings payload, forcing retain flag
bool Mqtt::queue_publish_retain(const char * topic, const char * payload) {
    return queue_publish_message(topic, payload, true);
}

// converts json payload to string, uses any retain flag
bool Mqtt::queue_publish(const char * topic, const JsonObjectConst payload, const bool retain) {
    if (payload.size()) {
        std::string payload_text;
        payload_text.reserve(measureJson(payload) + 1);
        serializeJson(payload, payload_text); // convert json to string
        return queue_publish_message(topic, payload_text, retain);
    }
    return false;
}

// publish empty payload to remove the topic
bool Mqtt::queue_remove_topic(const char * topic) {
    if (ha_enabled_) {
        return queue_publish_message(Mqtt::discovery_prefix() + topic, "", true); // publish with retain to remove from broker
    } else {
        return queue_publish_message(topic, "", true); // publish with retain to remove from broker
    }
}

// queue a Home Assistant config topic and payload, with retain flag set
bool Mqtt::queue_ha(const char * topic, const JsonObjectConst payload) {
    if (!enabled()) {
        return false;
    }

    std::string payload_text;
    payload_text.reserve(measureJson(payload) + 1);
    serializeJson(payload, payload_text); // convert json to string

    return queue_publish_message(Mqtt::discovery_prefix() + topic, payload_text, true); // with retain true
}

// create's a ha sensor config topic from a device value object (dev)
// adds ids, name, mf, mdl, via_device
// and also takes a flag (create_device_config) used to also create the main HA device config. This is only needed for one entity
bool Mqtt::publish_ha_sensor_config_dv(DeviceValue & dv,
                                       const char *  model,
                                       const char *  brand,
                                       const char *  version,
                                       const bool    remove,
                                       const bool    create_device_config) {
    // calculate the min and max
    int16_t  dv_set_min;
    uint32_t dv_set_max;
    (void)dv.get_min_max(dv_set_min, dv_set_max);

    // determine if we're creating the command topics which we use special HA configs
    // if the entity has been marked as read-only it'll be created as a binary_sensor or sensor
    // or we're dealing with Energy sensors that must have "diagnostic" as an entity category (e.g. negheat & nrgww)
    bool has_cmd = dv.has_cmd && !dv.has_state(DeviceValueState::DV_READONLY) && (dv.uom != DeviceValueUOM::KWH);

    return publish_ha_sensor_config(dv.type,
                                    dv.tag,
                                    dv.get_fullname().c_str(),
                                    (dv.fullname ? dv.fullname[0] : nullptr), // EN name
                                    dv.device_type,
                                    dv.short_name,
                                    dv.uom,
                                    remove,
                                    has_cmd,
                                    dv.options,
                                    dv.options_size,
                                    dv_set_min,
                                    dv_set_max,
                                    dv.numeric_operator,
                                    model,
                                    brand,
                                    version,
                                    create_device_config);
}

// publish HA sensor specific for System using the heartbeat tag
bool Mqtt::publish_system_ha_sensor_config(uint8_t type, const char * name, const char * entity, const uint8_t uom) {
    return publish_ha_sensor_config(type, DeviceValueTAG::TAG_DEVICE_DATA, name, name, EMSdevice::DeviceType::SYSTEM, entity, uom, false, false, nullptr, 0, 0, 0, 0);
}

// MQTT discovery configs
// entity must match the key/value pair in the *_data topic
// note: some extra string copying done here, it looks messy but does help with heap fragmentation issues
bool Mqtt::publish_ha_sensor_config(uint8_t               type,        // EMSdevice::DeviceValueType
                                    int8_t                tag,         // EMSdevice::DeviceValueTAG
                                    const char * const    fullname,    // fullname, already translated
                                    const char * const    en_name,     // original name in english
                                    const uint8_t         device_type, // EMSdevice::DeviceType
                                    const char * const    entity,      // same as shortname
                                    const uint8_t         uom,         // EMSdevice::DeviceValueUOM (0=NONE)
                                    const bool            remove,      // true if we want to remove this topic
                                    const bool            has_cmd,
                                    const char * const ** options,
                                    uint8_t               options_size,
                                    const int16_t         dv_set_min,
                                    const uint32_t        dv_set_max,
                                    const int8_t          num_op,
                                    const char * const    model,
                                    const char * const    brand,
                                    const char * const    version,
                                    const bool            create_device_config) {
    // ignore if name (fullname) is empty
    if (!fullname || !en_name) {
        return false;
    }

    // create the device name
    auto device_name = EMSdevice::device_type_2_device_name(device_type);

    bool has_tag = ((tag < DeviceValue::NUM_TAGS) && (tag != DeviceValue::DeviceValueTAG::TAG_NONE) && strlen(DeviceValue::DeviceValueTAG_s[tag][0]));

    // create entity by add the hc/dhw tag if present, separating with an _
    char entity_with_tag[50];
    if (tag >= DeviceValueTAG::TAG_HC1) {
        snprintf(entity_with_tag, sizeof(entity_with_tag), "%s_%s", EMSdevice::tag_to_mqtt(tag), entity);
    } else {
        snprintf(entity_with_tag, sizeof(entity_with_tag), "%s", entity);
    }

    // build unique identifier also used to build the default_entity_id which also becomes the Entity ID in HA
    char uniq_id[80];

    // list of boiler entities that need conversion for 3.6 compatibility, add ww suffix
    // used when entity_format is SINGLE_OLD or MULTI_OLD to keep with 3.6.5
    // see #1714 - https://github.com/emsesp/EMS-ESP32/issues/1714
    const char * dhw_old[] =
        {FL_(nrgWw)[0], FL_(upTimeCompWw)[0], FL_(nrgConsCompWw)[0], FL_(auxElecHeatNrgConsWw)[0], FL_(nrgSuppWw)[0], FL_(wwAltOpPrioWw)[0], FL_(hpCircPumpWw)[0]};
    uint8_t num_dhw_old = sizeof(dhw_old) / sizeof(dhw_old[0]);

    // create the uniq_d based on the entity format
    if (Mqtt::entity_format() == entityFormat::MULTI_SHORT) {
        // base name + shortname
        snprintf(uniq_id, sizeof(uniq_id), "%s_%s_%s", Mqtt::basename().c_str(), device_name, entity_with_tag);
    } else if (Mqtt::entity_format() == entityFormat::SINGLE_SHORT) {
        // shortname only (=default)
        snprintf(uniq_id, sizeof(uniq_id), "%s_%s", device_name, entity_with_tag);
    } else if (Mqtt::entity_format() == entityFormat::SINGLE_OLD) {
        // shortname, remap to 3.6
        if (has_tag && (device_type == EMSdevice::DeviceType::BOILER || device_type == EMSdevice::DeviceType::THERMOSTAT)
            && tag == DeviceValue::DeviceValueTAG::TAG_DHW1) {
            snprintf(entity_with_tag, sizeof(entity_with_tag), "ww%s", entity);
            for (uint8_t i = 0; i < num_dhw_old; i++) {
                if (strcmp(entity, dhw_old[i]) == 0) { // special case for tp1de #1714
                    snprintf(entity_with_tag, sizeof(entity_with_tag), "%sww", dhw_old[i]);
                }
            }
            snprintf(uniq_id, sizeof(uniq_id), "%s_%s", device_name, entity_with_tag);
        } else if (has_tag && device_type == EMSdevice::DeviceType::WATER && tag >= DeviceValue::DeviceValueTAG::TAG_DHW3) {
            snprintf(entity_with_tag, sizeof(entity_with_tag), "wwc%d_%s", tag - DeviceValue::DeviceValueTAG::TAG_DHW1 + 1, entity);
            snprintf(uniq_id, sizeof(uniq_id), "solar_%s", entity_with_tag);
        } else if (has_tag && device_type == EMSdevice::DeviceType::WATER && tag >= DeviceValue::DeviceValueTAG::TAG_DHW1) {
            snprintf(entity_with_tag, sizeof(entity_with_tag), "wwc%d_%s", tag - DeviceValue::DeviceValueTAG::TAG_DHW1 + 1, entity);
            snprintf(uniq_id, sizeof(uniq_id), "mixer_%s", entity_with_tag);
        } else {
            snprintf(uniq_id, sizeof(uniq_id), "%s_%s", device_name, entity_with_tag);
        }
    } else if (Mqtt::entity_format() == entityFormat::MULTI_OLD) {
        // base name + shortname, remap to 3.6
        if (has_tag && (device_type == EMSdevice::DeviceType::BOILER || device_type == EMSdevice::DeviceType::THERMOSTAT)
            && tag == DeviceValue::DeviceValueTAG::TAG_DHW1) {
            snprintf(entity_with_tag, sizeof(entity_with_tag), "ww%s", entity);
            for (uint8_t i = 0; i < num_dhw_old; i++) {
                if (strcmp(entity, dhw_old[i]) == 0) { // special case for tp1de #1714
                    snprintf(entity_with_tag, sizeof(entity_with_tag), "%sww", dhw_old[i]);
                }
            }
            snprintf(uniq_id, sizeof(uniq_id), "%s_%s_%s", Mqtt::basename().c_str(), device_name, entity_with_tag);
        } else if (has_tag && device_type == EMSdevice::DeviceType::WATER && tag >= DeviceValue::DeviceValueTAG::TAG_DHW3) {
            snprintf(entity_with_tag, sizeof(entity_with_tag), "wwc%d_%s", tag - DeviceValue::DeviceValueTAG::TAG_DHW1 + 1, entity);
            snprintf(uniq_id, sizeof(uniq_id), "%s_solar_%s", Mqtt::basename().c_str(), entity_with_tag);
        } else if (has_tag && device_type == EMSdevice::DeviceType::WATER && tag >= DeviceValue::DeviceValueTAG::TAG_DHW1) {
            snprintf(entity_with_tag, sizeof(entity_with_tag), "wwc%d_%s", tag - DeviceValue::DeviceValueTAG::TAG_DHW1 + 1, entity);
            snprintf(uniq_id, sizeof(uniq_id), "%s_mixer_%s", Mqtt::basename().c_str(), entity_with_tag);
        } else {
            snprintf(uniq_id, sizeof(uniq_id), "%s_%s_%s", Mqtt::basename().c_str(), device_name, entity_with_tag);
        }
    } else {
        // entity_format is 0, the old v3.4 style
        // take en_name and replace all spaces
        char uniq_s[60];
        strlcpy(uniq_s, en_name, sizeof(uniq_s));
        Helpers::replace_char(uniq_s, ' ', '_');
        Helpers::replace_char(uniq_s, '+', '2'); //changes 'eco+_switch_off' to 'eco2_switch_off' (HA ignores '+')
        if (has_tag && (device_type == EMSdevice::DeviceType::BOILER || device_type == EMSdevice::DeviceType::THERMOSTAT)
            && tag == DeviceValue::DeviceValueTAG::TAG_DHW1) {
            snprintf(entity_with_tag, sizeof(entity_with_tag), "ww%s", entity);
            snprintf(uniq_id, sizeof(uniq_id), "%s_dhw_%s", device_name, Helpers::toLower(uniq_s).c_str());
            for (uint8_t i = 0; i < num_dhw_old; i++) {
                if (strcmp(entity, dhw_old[i]) == 0) { // special case for tp1de #1714
                    snprintf(entity_with_tag, sizeof(entity_with_tag), "%sww", dhw_old[i]);
                }
            }
        } else if (has_tag && device_type == EMSdevice::DeviceType::WATER && tag >= DeviceValue::DeviceValueTAG::TAG_DHW3) {
            snprintf(entity_with_tag, sizeof(entity_with_tag), "wwc%d_%s", tag - DeviceValue::DeviceValueTAG::TAG_DHW1 + 1, entity);
            snprintf(uniq_id, sizeof(uniq_id), "solar_wwc%d_%s", tag - DeviceValue::DeviceValueTAG::TAG_DHW1 + 1, Helpers::toLower(uniq_s).c_str());
        } else if (has_tag && device_type == EMSdevice::DeviceType::WATER && tag >= DeviceValue::DeviceValueTAG::TAG_DHW1) {
            snprintf(entity_with_tag, sizeof(entity_with_tag), "wwc%d_%s", tag - DeviceValue::DeviceValueTAG::TAG_DHW1 + 1, entity);
            snprintf(uniq_id, sizeof(uniq_id), "mixer_wwc%d_%s", tag - DeviceValue::DeviceValueTAG::TAG_DHW1 + 1, Helpers::toLower(uniq_s).c_str());
        } else if (has_tag) {
            snprintf(uniq_id, sizeof(uniq_id), "%s_%s_%s", device_name, DeviceValue::DeviceValueTAG_s[tag][0], Helpers::toLower(uniq_s).c_str());
        } else {
            snprintf(uniq_id, sizeof(uniq_id), "%s_%s", device_name, Helpers::toLower(uniq_s).c_str());
        }
    }

    // build a config topic that will be prefix onto a HA type (e.g. number, switch)
    char config_topic[70];
    snprintf(config_topic, sizeof(config_topic), "%s/%s_%s/config", Mqtt::base().c_str(), device_name, entity_with_tag);

    // create the topic
    // depending on the type and whether the device entity is writable (i.e. a command)
    // https://developers.home-assistant.io/docs/core/entity
    char topic[MQTT_TOPIC_MAX_SIZE];
    topic[0] = '\0'; // nullify, making it empty
    if (has_cmd) {
        // if it's a command then we can use Number, Switch, Select or Text. Otherwise stick to Sensor
        switch (type) {
        case DeviceValueType::INT8:
        case DeviceValueType::UINT8:
        case DeviceValueType::INT16:
        case DeviceValueType::UINT16:
        case DeviceValueType::UINT24:
        case DeviceValueType::UINT32:
            // number - https://www.home-assistant.io/integrations/number.mqtt
            // older Domoticz does not support number, will default to Sensor
            if (discovery_type() == discoveryType::HOMEASSISTANT || discovery_type() == discoveryType::DOMOTICZ_LATEST) {
                snprintf(topic, sizeof(topic), "number/%s", config_topic);
            }
            break;
        case DeviceValueType::BOOL:
            // switch - https://www.home-assistant.io/integrations/switch.mqtt
            snprintf(topic, sizeof(topic), "switch/%s", config_topic);
            break;
        case DeviceValueType::ENUM:
            // select - https://www.home-assistant.io/integrations/select.mqtt
            snprintf(topic, sizeof(topic), "select/%s", config_topic);
            break;
        case DeviceValueType::CMD:
            if (uom == DeviceValueUOM::NONE) {
                snprintf(topic, sizeof(topic), "select/%s", config_topic); // hardcoded commands are always ENUMS
            } else if (discovery_type() == discoveryType::HOMEASSISTANT || discovery_type() == discoveryType::DOMOTICZ_LATEST) {
                snprintf(topic, sizeof(topic), "number/%s", config_topic);
            }
            break;
        case DeviceValueType::STRING:
            // text - https://www.home-assistant.io/integrations/text.mqtt
            // Domoticz does not support text, will default to Sensor
            if (discovery_type() == discoveryType::HOMEASSISTANT) {
                snprintf(topic, sizeof(topic), "text/%s", config_topic); // e.g. set_datetime, set_holiday, set_wwswitchtime
            }
            break;
        default:
            break;
        }
    }

    // if at this point we don't have a topic created yet, create a default sensor one. We always need a topic.
    bool is_sensor = false;
    if (!strnlen(topic, sizeof(topic))) {
        snprintf(topic, sizeof(topic), (type == DeviceValueType::BOOL) ? "binary_sensor/%s" : "sensor/%s", config_topic); // binary sensor (for booleans)
        is_sensor = true;
    }

    // if we're asking to remove this topic, send an empty payload and exit
    // https://github.com/emsesp/EMS-ESP32/issues/196
    if (remove) {
        LOG_DEBUG("Queuing removing topic %s", topic);
        return queue_remove_topic(topic);
    }

    // build the full payload
    JsonDocument doc;
    doc["uniq_id"] = uniq_id;

    // set the entity_id. This is breaking change in HA 2025.10.0 - see https://github.com/home-assistant/core/pull/151775
    // extract the string from topic up to the / using std::string
    std::string topic_str(topic);
    doc["def_ent_id"] = topic_str.substr(0, topic_str.find("/")) + "." + uniq_id;

    char sample_val[30] = "0"; // sample, correct(!) entity value, used only to prevent warning/error in HA if real value is not published yet

    // we add the command topic parameter for commands
    if (has_cmd) {
        // add category
        // doc["ent_cat"] = "config"; // for writeable entities, like switch, number, text, select

        char command_topic[MQTT_TOPIC_MAX_SIZE];
        // add command topic
        if (tag >= DeviceValueTAG::TAG_HC1) {
            snprintf(command_topic, sizeof(command_topic), "%s/%s/%s/%s", Mqtt::base().c_str(), device_name, EMSdevice::tag_to_mqtt(tag), entity);
        } else {
            snprintf(command_topic, sizeof(command_topic), "%s/%s/%s", Mqtt::base().c_str(), device_name, entity);
        }
        doc["cmd_t"] = command_topic;

        // extend for enums and also commands, add options
        if ((type == DeviceValueType::ENUM) || (type == DeviceValueType::CMD && uom == DeviceValueUOM::NONE)) {
            JsonArray option_list = doc["ops"].to<JsonArray>();
            if (EMSESP::system_.enum_format() == ENUM_FORMAT_INDEX) {
                // use index numbers
                for (uint8_t i = 0; i < options_size; i++) {
                    option_list.add(Helpers::itoa(i)); // as a string
                }
                snprintf(sample_val, sizeof(sample_val), "'0'");
            } else {
                // use strings
                for (uint8_t i = 0; i < options_size; i++) {
                    option_list.add(Helpers::translated_word(options[i]));
                }
                snprintf(sample_val, sizeof(sample_val), "'%s'", Helpers::translated_word(options[0]));
            }
        } else if (type != DeviceValueType::STRING && type != DeviceValueType::BOOL) {
            // For numeric's add the range
            doc["mode"] = "box"; // auto, slider or box
            if (num_op > 0) {
                doc["step"] = 1.0 / num_op;
            } else if (num_op < 0) {
                doc["step"] = -num_op;
            } else {
                doc["step"] = 1;
            }
        }

        // set min and max values, if we have a valid range
        if (dv_set_min != 0 || dv_set_max != 0) {
            doc["min"] = dv_set_min;
            doc["max"] = dv_set_max;
            snprintf(sample_val, sizeof(sample_val), "%i", dv_set_min);
        }
    }

    // friendly name = <tag> <name>
    char   ha_name[70];
    char * F_name = strdup(fullname);
    Helpers::CharToUpperUTF8(F_name); // capitalize first letter
    if (has_tag) {
        // exclude heartbeat tag
        snprintf(ha_name, sizeof(ha_name), "%s %s", EMSdevice::tag_to_string(tag), F_name);
    } else {
        snprintf(ha_name, sizeof(ha_name), "%s", F_name); // no tag
    }
    free(F_name); // very important!
    doc["name"] = ha_name;

    // add state_topic and it's value_template. This is not needed for commands, only sensors
    if (type != DeviceValueType::CMD || is_sensor) {
        // state topic, except for commands
        char stat_t[MQTT_TOPIC_MAX_SIZE];

        // This is where we determine which MQTT topic to pull the data from
        // There is one exception for DeviceType::SYSTEM, which uses the heartbeat topic, and when fetching the version we want to take this from the info topic instead
        if ((device_type == EMSdevice::DeviceType::SYSTEM) && (strncmp(entity, "version", 7) == 0)) {
            snprintf(stat_t, sizeof(stat_t), "%s/%s", Mqtt::base().c_str(), F_(info));
        } else {
            snprintf(stat_t, sizeof(stat_t), "%s/%s", Mqtt::base().c_str(), tag_to_topic(device_type, tag).c_str());
        }
        doc["stat_t"] = stat_t;

        // value template
        // if its nested mqtt format then use the appended entity name, otherwise take the original name
        char val_obj[100];
        char val_cond[200];
        if (is_nested() && tag >= DeviceValueTAG::TAG_HC1) {
            snprintf(val_obj, sizeof(val_obj), "value_json['%s']['%s']", EMSdevice::tag_to_mqtt(tag), entity);
            snprintf(val_cond, sizeof(val_cond), "value_json.%s is defined and %s is defined", EMSdevice::tag_to_mqtt(tag), val_obj);
        } else {
            snprintf(val_obj, sizeof(val_obj), "value_json['%s']", entity);
            snprintf(val_cond, sizeof(val_cond), "%s is defined", val_obj);
        }

        // special case to handle booleans
        // applies to both Binary Sensor (read only) and a Switch (for a command)
        // has no unit of measure or icon, and must be true/false (not on/off or 1/0)
        if (type == DeviceValueType::BOOL) {
            add_ha_bool(doc.as<JsonObject>());
            strlcpy(sample_val, "false", sizeof(sample_val)); // default is "false"
        }

        // don't bother with value template conditions if using Domoticz which doesn't fully support MQTT Discovery
        if (discovery_type() == discoveryType::HOMEASSISTANT) {
            doc["val_tpl"] = (std::string) "{{" + val_obj + " if " + val_cond + " else " + sample_val + "}}";

            // adds availability, dev, ids to the config section to HA Discovery config
            // except for commands
            add_ha_avail_section(doc.as<JsonObject>(), stat_t, false, val_cond);
        } else {
            // Domoticz doesn't support value templates, so we just use the value directly
            // Also omit the uom and other state classes
            doc["val_tpl"] = (std::string) "{{" + val_obj + "}}";
        }
    }

    // Add the state class, device class and an optional icon based on the uom
    // first set the catagory for System entities
    // https://github.com/emsesp/EMS-ESP32/discussions/1459#discussioncomment-7694873
    add_ha_classes(doc.as<JsonObject>(), device_type, type, uom, entity);

    // add dev section
    if (device_type == EMSdevice::DeviceType::SYSTEM) {
        add_ha_dev_section(doc.as<JsonObject>(), nullptr, nullptr, nullptr, nullptr, false);
    } else {
        add_ha_dev_section(doc.as<JsonObject>(), EMSdevice::device_type_2_device_name(device_type), model, brand, version, create_device_config);
    }

    return queue_ha(topic, doc.as<JsonObject>());
}

// Add the uom, state class, device class and an optional icon based on the uom
void Mqtt::add_ha_classes(JsonObject doc, const uint8_t device_type, const uint8_t type, const uint8_t uom, const char * entity, bool is_discovery) {
    if (device_type == EMSdevice::DeviceType::SYSTEM) {
        doc["ent_cat"] = "diagnostic"; // instead of 'config'
    }

    // for HA discovery we use different namings
    const char * dc_ha  = is_discovery ? "dev_cla" : "device_class"; // device class
    const char * sc_ha  = is_discovery ? "stat_cla" : "state_class"; // state class
    const char * uom_ha = is_discovery ? "unit_of_meas" : "uom";     // unit of measure

    // set uom, unless boolean
    // using HA uom specific codes from https://github.com/home-assistant/core/blob/dev/homeassistant/const.py
    if (type != DeviceValueType::BOOL) {
        if (uom == DeviceValueUOM::HOURS) {
            doc[uom_ha] = "h";
        } else if (uom == DeviceValueUOM::MINUTES) {
            doc[uom_ha] = "min";
        } else if (uom == DeviceValueUOM::SECONDS) {
            doc[uom_ha] = "s";
        } else if (uom != DeviceValueUOM::NONE) {
            doc[uom_ha] = EMSdevice::uom_to_string(uom); // use default
        } else if (discovery_type() != discoveryType::HOMEASSISTANT) {
            doc[uom_ha] = " "; // Domoticz uses " " for a no-uom
        }
    }

    // set state and device class
    // also icon, when there is no device class that sets one
    switch (uom) {
    case DeviceValueUOM::DEGREES:
    case DeviceValueUOM::DEGREES_R:
    case DeviceValueUOM::K:
        doc[sc_ha] = F_(measurement);
        doc[dc_ha] = "temperature";
        // override uom if fahrenheit
        doc[uom_ha] = EMSESP::system_.fahrenheit() ? DeviceValue::DeviceValueUOM_s[DeviceValueUOM::FAHRENHEIT] : DeviceValue::DeviceValueUOM_s[uom];
        break;
    case DeviceValueUOM::PERCENT:
        doc[sc_ha] = F_(measurement);
        doc[dc_ha] = "power_factor";
        if (is_discovery)
            doc["ic"] = F_(iconpercent); // icon
        break;
    case DeviceValueUOM::SECONDS:
    case DeviceValueUOM::MINUTES:
    case DeviceValueUOM::HOURS:
        if (type == DeviceValueType::TIME) {
            doc[sc_ha] = F_(total_increasing);
        } else {
            doc[sc_ha] = F_(measurement);
        }
        doc[dc_ha] = "duration"; // https://github.com/emsesp/EMS-ESP32/issues/822
        break;
    case DeviceValueUOM::KB:
        if (is_discovery)
            doc["ic"] = F_(iconkb);
        break;
    case DeviceValueUOM::LMIN:
    case DeviceValueUOM::LH:
        if (is_discovery)
            doc["ic"] = F_(iconlmin);
        doc[sc_ha] = F_(measurement);
        break;
    case DeviceValueUOM::WH:
        // https://github.com/emsesp/EMS-ESP32/issues/1796
        if (entity == FL_(energyToday)[0]) {
            doc[sc_ha] = F_(total_increasing);
        } else if (entity == FL_(energyLastHour)[0]) {
            doc[sc_ha] = "total";
        } else {
            doc[sc_ha] = F_(measurement);
        }
        doc[dc_ha] = "energy";
        break;
    case DeviceValueUOM::KWH:
        doc[sc_ha] = F_(total_increasing);
        doc[dc_ha] = "energy";
        break;
    case DeviceValueUOM::UA:
        if (is_discovery)
            doc["ic"] = F_(iconua);
        doc[sc_ha] = F_(measurement);
        break;
    case DeviceValueUOM::BAR:
        doc[sc_ha] = F_(measurement);
        doc[dc_ha] = "pressure";
        break;
    case DeviceValueUOM::W:
    case DeviceValueUOM::KW:
        doc[sc_ha] = F_(measurement);
        doc[dc_ha] = "power";
        break;
    case DeviceValueUOM::DBM:
        doc[sc_ha] = F_(measurement);
        doc[dc_ha] = "signal_strength";
        break;
    case DeviceValueUOM::CONNECTIVITY:
        doc[sc_ha] = F_(measurement);
        doc[dc_ha] = "connectivity";
        break;
    case DeviceValueUOM::NONE:
        // for device entities which have numerical values, with no UOM
        if ((type != DeviceValueType::STRING)
            && (type == DeviceValueType::INT8 || type == DeviceValueType::UINT8 || type == DeviceValueType::INT16 || type == DeviceValueType::UINT16
                || type == DeviceValueType::UINT24 || type == DeviceValueType::UINT32)) {
            if (is_discovery)
                doc["ic"] = F_(iconnum); // set icon
            // determine if its a measurement or total increasing
            // most of the values are measurement. for example Tx Reads will increment but can be reset to 0 after a restart
            // all the starts are increasing, and they are ULONGs
            if (type == DeviceValueType::UINT24 || type == DeviceValueType::UINT32) {
                doc[sc_ha] = F_(total_increasing);
            } else {
                doc[sc_ha] = F_(measurement); // default to measurement
            }
        }
        break;
    default:
        break;
    }
}

bool Mqtt::publish_ha_climate_config(const int8_t tag, const bool has_roomtemp, const bool remove, const int16_t min, const uint32_t max) {
    uint8_t       hc_num      = tag < DeviceValueTAG::TAG_SRC1 ? tag : tag - DeviceValueTAG::TAG_SRC1 + 1;
    const char *  tagname     = tag < DeviceValueTAG::TAG_SRC1 ? "hc" : "src";
    const uint8_t device_type = tag < DeviceValueTAG::TAG_SRC1 ? EMSdevice::DeviceType::THERMOSTAT : EMSdevice::DeviceType::CONNECT;
    const char *  devicename  = EMSdevice::device_type_2_device_name(device_type);

    char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
    char topic_t[Mqtt::MQTT_TOPIC_MAX_SIZE];
    char hc_mode_s[30];
    char seltemp_s[30];
    char currtemp_s[30];
    char currhum_s[30];
    char hc_mode_cond[80];
    char seltemp_cond[100];
    char currtemp_cond[100];
    char currhum_cond[100];
    char mode_str_tpl[400];
    char name_s[10];
    char uniq_id_s[60];
    char temp_cmd_s[30];
    char mode_cmd_s[30];
    char min_s[10];
    char max_s[10];

    snprintf(topic, sizeof(topic), "climate/%s/%s_%s%d/config", Mqtt::basename().c_str(), devicename, tagname, hc_num);
    if (remove) {
        return queue_remove_topic(topic); // publish empty payload with retain flag
    }

    if (Mqtt::is_nested()) {
        // nested format
        snprintf(hc_mode_s, sizeof(hc_mode_s), "value_json.%s%d.mode", tagname, hc_num);
        snprintf(hc_mode_cond, sizeof(hc_mode_cond), "value_json.%s%d is undefined or %s is undefined", tagname, hc_num, hc_mode_s);
        snprintf(seltemp_s, sizeof(seltemp_s), "value_json.%s%d.seltemp", tagname, hc_num);
        snprintf(seltemp_cond, sizeof(seltemp_cond), "value_json.%s%d is defined and %s is defined", tagname, hc_num, seltemp_s);

        if (has_roomtemp) {
            snprintf(currtemp_s, sizeof(currtemp_s), "value_json.%s%d.currtemp", tagname, hc_num);
            snprintf(currtemp_cond, sizeof(currtemp_cond), "value_json.%s%d is defined and %s is defined", tagname, hc_num, currtemp_s);
        }
        if (tag >= DeviceValueTAG::TAG_SRC1) {
            snprintf(currhum_s, sizeof(currhum_s), "value_json.%s%d.airhumidity", tagname, hc_num);
            snprintf(currhum_cond, sizeof(currhum_cond), "value_json.%s%d is defined and %s is defined", tagname, hc_num, currhum_s);
        }
        snprintf(topic_t, sizeof(topic_t), "~/%s", Mqtt::tag_to_topic(device_type, DeviceValueTAG::TAG_NONE).c_str());
    } else {
        // single format
        snprintf(hc_mode_s, sizeof(hc_mode_s), "value_json.mode");
        snprintf(hc_mode_cond, sizeof(hc_mode_cond), "%s is undefined", hc_mode_s);
        snprintf(seltemp_s, sizeof(seltemp_s), "value_json.seltemp");
        snprintf(seltemp_cond, sizeof(seltemp_cond), "%s is defined", seltemp_s);

        if (has_roomtemp) {
            snprintf(currtemp_s, sizeof(currtemp_s), "value_json.currtemp");
            snprintf(currtemp_cond, sizeof(currtemp_cond), "%s is defined", currtemp_s);
        }
        if (tag >= DeviceValueTAG::TAG_SRC1) {
            snprintf(currhum_s, sizeof(currhum_s), "value_json.airhumidity");
            snprintf(currhum_cond, sizeof(currhum_cond), "%s is defined", currhum_s);
        }
        snprintf(topic_t, sizeof(topic_t), "~/%s", Mqtt::tag_to_topic(device_type, tag).c_str());
    }

    snprintf(mode_str_tpl,
             sizeof(mode_str_tpl),
             "{%%if %s%%}off{%%elif %s=='%s'%%}heat{%%elif %s=='%s'%%}heat{%%elif %s=='%s'%%}off{%%elif %s=='%s'%%}off{%%else%%}auto{%%endif%%}",
             hc_mode_cond,
             hc_mode_s,
             Helpers::translated_word(FL_(manual)),
             hc_mode_s,
             Helpers::translated_word(FL_(day)),
             hc_mode_s,
             Helpers::translated_word(FL_(night)),
             hc_mode_s,
             Helpers::translated_word(FL_(off)));

    snprintf(name_s, sizeof(name_s), "%s%d", tagname, hc_num);

    if (Mqtt::entity_format() == entityFormat::MULTI_SHORT) {
        snprintf(uniq_id_s, sizeof(uniq_id_s), "%s_%s_%s%d", Mqtt::basename().c_str(), devicename, tagname, hc_num); // add basename
    } else {
        snprintf(uniq_id_s, sizeof(uniq_id_s), "%s_%s%d", devicename, tagname, hc_num); // backward compatible with v3.4
    }

    snprintf(temp_cmd_s, sizeof(temp_cmd_s), "~/%s/%s%d/seltemp", devicename, tagname, hc_num);
    snprintf(mode_cmd_s, sizeof(mode_cmd_s), "~/%s/%s%d/mode", devicename, tagname, hc_num);

    JsonDocument doc;

    doc["~"]             = Mqtt::base();
    doc["uniq_id"]       = uniq_id_s;
    doc["def_ent_id"]    = (std::string) "climate." + uniq_id_s;
    doc["name"]          = name_s;
    doc["mode_stat_t"]   = topic_t;
    doc["mode_stat_tpl"] = mode_str_tpl;
    doc["temp_cmd_t"]    = temp_cmd_s;
    doc["temp_stat_t"]   = topic_t;
    doc["temp_stat_tpl"] = (std::string) "{{" + seltemp_s + " if " + seltemp_cond + " else 0}}";

    if (has_roomtemp) {
        doc["curr_temp_t"]   = topic_t;
        doc["curr_temp_tpl"] = (std::string) "{{" + currtemp_s + " if " + currtemp_cond + " else 0}}";
    }
    if (tag >= DeviceValueTAG::TAG_SRC1) {
        doc["curr_hum_t"]   = topic_t;
        doc["curr_hum_tpl"] = (std::string) "{{" + currhum_s + " if " + currhum_cond + " else 0}}";
    }

    doc["min_temp"]   = Helpers::render_value(min_s, min, 0, EMSESP::system_.fahrenheit() ? 2 : 0);
    doc["max_temp"]   = Helpers::render_value(max_s, max, 0, EMSESP::system_.fahrenheit() ? 2 : 0);
    doc["temp_step"]  = "0.5";
    doc["mode_cmd_t"] = mode_cmd_s;

    // add hvac_action - https://github.com/emsesp/EMS-ESP32/discussions/2562
    doc["act_t"]   = "~/boiler_data";
    doc["act_tpl"] = "{% if value_json.hpactivity=='cooling'%}cooling{%elif value_json.heatingactive=='on'%}heating{%else%}idle{%endif%}";

    // the HA climate component only responds to auto, heat and off
    JsonArray modes = doc["modes"].to<JsonArray>();

    modes.add("auto");
    modes.add("heat");
    modes.add("off");

    add_ha_dev_section(doc.as<JsonObject>(), devicename, nullptr, nullptr, nullptr, false);                                         // add dev section
    add_ha_avail_section(doc.as<JsonObject>(), topic_t, false, seltemp_cond, has_roomtemp ? currtemp_cond : nullptr, hc_mode_cond); // add availability section

    return queue_ha(topic, doc.as<JsonObject>()); // publish the config payload with retain flag
}

// based on the device and tag, create the MQTT topic name (without the basename)
// differs based on whether MQTT nested is enabled
// tag = EMSdevice::DeviceValueTAG
std::string Mqtt::tag_to_topic(uint8_t device_type, int8_t tag) {
    // the system device is treated differently. The topic is 'heartbeat' and doesn't follow the usual convention
    if (device_type == EMSdevice::DeviceType::SYSTEM) {
        return F_(heartbeat);
    }

    std::string topic = EMSdevice::device_type_2_device_name(device_type);

    // if there is a tag add it
    if (!is_nested() && tag >= DeviceValueTAG::TAG_HC1) {
        return topic + "_data_" + EMSdevice::tag_to_mqtt(tag);
    } else {
        return topic + "_data";
    }
}

// add devs section to an existing doc, only for HA
// under devs node it will create ids and name and optional mf, mdl, via_device
// name could be EMSdevice::device_type_2_device_name(dv.device_type));
void Mqtt::add_ha_dev_section(JsonObject doc, const char * name, const char * model, const char * brand, const char * version, const bool create_model) {
    // only works for HA
    if (discovery_type() != discoveryType::HOMEASSISTANT) {
        return;
    }

    // create dev section
    JsonObject dev_json = doc["dev"].to<JsonObject>();

    // add ids and name - with capitalize first letter
    JsonArray ids = dev_json["ids"].to<JsonArray>(); // ids, it is an array with a single element
    if (name != nullptr) {
        // for ids, replace all spaces with -
        std::string lower_name_str(name);
        std::replace(lower_name_str.begin(), lower_name_str.end(), ' ', '-');
        ids.add(Mqtt::basename() + "-" + Helpers::toLower(lower_name_str));

        auto cap_name = strdup(name);
        Helpers::CharToUpperUTF8(cap_name); // capitalize first letter
        dev_json["name"] = Mqtt::basename() + " " + cap_name;
        free(cap_name);
    } else {
        ids.add(Mqtt::basename());
        dev_json["name"] = Mqtt::basename();
    }

    // this is used to only create it once when entities are dynamically added
    if (create_model) {
        dev_json["mf"] = brand != nullptr ? brand : "EMS-ESP";
        if (model != nullptr) {
            dev_json["mdl"] = model;
        }
        if (version != nullptr) {
            dev_json["sw"] = version;
        }
        dev_json["via_device"] = Mqtt::basename();
    }
}

// adds sections for HA Discovery to an existing JSON doc
//  adds dev section with ids, name, mf, mdl, via_device
//  adds optional availability section
void Mqtt::add_ha_avail_section(JsonObject doc, const char * state_t, const bool is_first, const char * cond1, const char * cond2, const char * negcond) {
    // only works for HA
    if (discovery_type() != discoveryType::HOMEASSISTANT) {
        return;
    }

    // skip availability section if no conditions set
    if (!cond1 && !cond2 && !negcond) {
        return;
    }

    // adds "availability" section to HA Discovery config
    JsonArray    avty = doc["avty"].to<JsonArray>();
    JsonDocument avty_json;

    // make local copy of state, as the pointer will get de-referenced
    char state[50];
    strlcpy(state, state_t, sizeof(state));

    char         tpl[150];
    const char * tpl_draft = "{{'online' if %s else 'offline'}}";

    // condition 1
    if (cond1 != nullptr) {
        avty_json.clear();
        avty_json["t"] = state;
        snprintf(tpl, sizeof(tpl), tpl_draft, cond1);
        avty_json["val_tpl"] = tpl;
        avty.add(avty_json); // returns 0 if no mem
    }

    // condition 2
    if (cond2 != nullptr) {
        avty_json.clear();
        avty_json["t"] = state;
        snprintf(tpl, sizeof(tpl), tpl_draft, cond2);
        avty_json["val_tpl"] = tpl;
        avty.add(avty_json); // returns 0 if no mem
    }

    // negative condition
    if (negcond != nullptr) {
        avty_json.clear();
        avty_json["t"] = state;
        snprintf(tpl, sizeof(tpl), "{{'offline' if %s else 'online'}}", negcond);
        avty_json["val_tpl"] = tpl;
        avty.add(avty_json); // returns 0 if no mem
    }

    doc["avty_mode"] = "all";
}

// adds the pl_on and pl_off sections to the doc
void Mqtt::add_ha_bool(JsonObject doc) {
    const char * pl_on  = "pl_on";
    const char * pl_off = "pl_off";
    if (EMSESP::system_.bool_format() == BOOL_FORMAT_TRUEFALSE) {
        doc[pl_on]  = true;
        doc[pl_off] = false;
    } else if (EMSESP::system_.bool_format() == BOOL_FORMAT_10) {
        doc[pl_on]  = 1;
        doc[pl_off] = 0;
    } else {
        char result[12];
        doc[pl_on]  = Helpers::render_boolean(result, true);
        doc[pl_off] = Helpers::render_boolean(result, false);
    }
}

} // namespace emsesp