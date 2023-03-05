/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2023  Paul Derbyshire
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
#include "version.h"
#include "emsdevice.h"

namespace emsesp {

AsyncMqttClient * Mqtt::mqttClient_;

// static parameters we make global
std::string Mqtt::mqtt_base_;
std::string Mqtt::mqtt_basename_;
uint8_t     Mqtt::mqtt_qos_;
bool        Mqtt::mqtt_retain_;
uint32_t    Mqtt::publish_time_boiler_;
uint32_t    Mqtt::publish_time_thermostat_;
uint32_t    Mqtt::publish_time_solar_;
uint32_t    Mqtt::publish_time_mixer_;
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

std::deque<Mqtt::QueuedMqttMessage> Mqtt::mqtt_messages_;
std::vector<Mqtt::MQTTSubFunction>  Mqtt::mqtt_subfunctions_;

uint32_t Mqtt::mqtt_publish_fails_ = 0;
bool     Mqtt::connecting_         = false;
bool     Mqtt::initialized_        = false;
bool     Mqtt::ha_climate_reset_   = false;
uint8_t  Mqtt::connectcount_       = 0;
uint32_t Mqtt::mqtt_message_id_    = 0;
char     will_topic_[Mqtt::MQTT_TOPIC_MAX_SIZE]; // because MQTT library keeps only char pointer

std::string Mqtt::lasttopic_   = "";
std::string Mqtt::lastpayload_ = "";

// Home Assistant specific
// icons from https://materialdesignicons.com used with the UOMs (unit of measurements)
MAKE_WORD(measurement)
MAKE_WORD(total_increasing)
MAKE_WORD_CUSTOM(icondegrees, "mdi:coolant-temperature") // DeviceValueUOM::DEGREES
MAKE_WORD_CUSTOM(iconpercent, "mdi:percent-outline")     // DeviceValueUOM::PERCENT
MAKE_WORD_CUSTOM(iconkb, "mdi:memory")                   // DeviceValueUOM::KB
MAKE_WORD_CUSTOM(iconlmin, "mdi:water-boiler")           // DeviceValueUOM::LMIN
MAKE_WORD_CUSTOM(iconua, "mdi:lightning-bolt-circle")    // DeviceValueUOM::UA
MAKE_WORD_CUSTOM(iconnum, "mdi:counter")                 // DeviceValueUOM::NONE

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
    mqtt_subfunctions_.emplace_back(device_type, std::move(topic), std::move(cb));

    if (!enabled()) {
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
        bool found = false;
        for (const auto & message : mqtt_messages_) {
            found |= ((message.content_->operation == Operation::SUBSCRIBE) && (mqtt_subfunction.topic_ == message.content_->topic));
        }

        if (!found) {
            queue_subscribe_message(mqtt_subfunction.topic_);
        }
    }
}

// Main MQTT loop - sends out top item on publish queue
void Mqtt::loop() {
    // exit if MQTT is not enabled or if there is no network connection
    if (!connected()) {
        return;
    }

    uint32_t currentMillis = uuid::get_uptime();

    // publish MQTT queue, but timed to avoid overloading the TCP pipe
    if ((uint32_t)(currentMillis - last_mqtt_poll_) > MQTT_PUBLISH_WAIT) {
        last_mqtt_poll_ = currentMillis;
        process_queue();
    }

    // send heartbeat
    if ((currentMillis - last_publish_heartbeat_ > publish_time_heartbeat_)) {
        last_publish_heartbeat_ = currentMillis;
        EMSESP::system_.send_heartbeat(); // send heartbeat
    }

    // dallas publish on change
    if (!publish_time_sensor_) {
        EMSESP::publish_sensor_values(false);
    }

    if (!mqtt_messages_.empty()) {
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

        if (publish_time_other_ && (currentMillis - last_publish_other_ > publish_time_other_)) {
        last_publish_other_ = (currentMillis / publish_time_other_) * publish_time_other_;
        EMSESP::publish_other_values(); // switch and heatpump
        EMSESP::webSchedulerService.publish();
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
    shell.println();

    // show subscriptions
    shell.printfln("MQTT topic subscriptions:");
    for (const auto & mqtt_subfunction : mqtt_subfunctions_) {
        shell.printfln(" %s/%s", mqtt_base_.c_str(), mqtt_subfunction.topic_.c_str());
    }
    shell.println();

    // show queues
    if (mqtt_messages_.empty()) {
        shell.printfln("MQTT queue is empty");
        shell.println();
        return;
    }

    shell.printfln("MQTT queue (%d):", mqtt_messages_.size());

    for (const auto & message : mqtt_messages_) {
        auto content = message.content_;
        char topic[MQTT_TOPIC_MAX_SIZE];

        // prefix base, only if it's not a discovery topic
        if (content->topic.compare(0, discovery_prefix().size(), discovery_prefix()) == 0) {
            snprintf(topic, sizeof(topic), "%s/%s", mqtt_base_.c_str(), content->topic.c_str());
        } else {
            snprintf(topic, sizeof(topic), "%s", content->topic.c_str());
        }

        if (content->operation == Operation::PUBLISH) {
            // Publish messages
            if (message.retry_count_ == 0) {
                if (message.packet_id_ == 0) {
                    shell.printfln(" [%02d] (Pub) topic=%s payload=%s", message.id_, topic, content->payload.c_str());
                } else {
                    shell.printfln(" [%02d] (Pub) topic=%s payload=%s (pid %d)", message.id_, topic, content->payload.c_str(), message.packet_id_);
                }
            } else {
                shell.printfln(" [%02d] (Pub) topic=%s payload=%s (pid %d, retry #%d)",
                               message.id_,
                               topic,
                               content->payload.c_str(),
                               message.packet_id_,
                               message.retry_count_);
            }
        } else {
            // Subscribe messages
            shell.printfln(" [%02d] (Sub) topic=%s", message.id_, topic);
        }
    }
    shell.println();
}

#if defined(EMSESP_TEST)
// simulate receiving a MQTT message, used only for testing
void Mqtt::incoming(const char * topic, const char * payload) {
    if (payload != nullptr) {
        on_message(topic, payload, strlen(payload));
    }
}
#endif

// received an MQTT message that we subscribed too
// topic is the full path
// payload is json or a single string and converted to a json with key 'value'
void Mqtt::on_message(const char * topic, const char * payload, size_t len) const {
    // sometimes the payload is not terminated correctly, so make a copy
    // convert payload to a null-terminated char string
    char message[len + 2];
    if (payload != nullptr) {
        strlcpy(message, payload, len + 1);
    }

#if defined(EMSESP_DEBUG)
    if (len) {
        LOG_DEBUG("Received topic `%s` => payload `%s` (length %d)", topic, message, len);
    } else {
        LOG_DEBUG("Received topic `%s`", topic);
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
        snprintf(full_topic, sizeof(full_topic), "%s/%s", mqtt_base_.c_str(), mf.topic_.c_str());

        if ((!strcmp(topic, full_topic)) && (mf.mqtt_subfunction_)) {
            if (!(mf.mqtt_subfunction_)(message)) {
                LOG_ERROR("error: invalid payload %s for this topic %s", message, topic);
                if (send_response_) {
                    Mqtt::queue_publish("response", "error: invalid data");
                }
            }
            return;
        }
    }

    StaticJsonDocument<EMSESP_JSON_SIZE_SMALL> input_doc;
    DynamicJsonDocument                        output_doc(EMSESP_JSON_SIZE_XLARGE);
    JsonObject                                 input, output;

    // convert payload into a json doc
    // if the payload doesn't not contain the key 'value' or 'data', treat the whole payload as the 'value'
    if (len != 0) {
        DeserializationError error = deserializeJson(input_doc, (const char *)message);
        if ((!input_doc.containsKey("value") && !input_doc.containsKey("data")) || error) {
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
            snprintf(error, sizeof(error), "Call failed with error: %s (%s)", (const char *)output["message"], Command::return_code_string(return_code).c_str());
        } else {
            snprintf(error, sizeof(error), "Call failed with error code (%s)", Command::return_code_string(return_code).c_str());
        }
        LOG_ERROR(error);
        if (send_response_) {
            Mqtt::queue_publish("response", error);
        }
    } else {
        // all good, send back json output from call
        if (send_response_) {
            Mqtt::queue_publish("response", output);
        }
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
// its a poor man's QOS we assume the ACK represents the last Publish sent
// check if ACK matches the last Publish we sent, if not report an error. Only if qos is 1 or 2
// and always remove from queue
void Mqtt::on_publish(uint16_t packetId) const {
    // find the MQTT message in the queue and remove it
    if (mqtt_messages_.empty()) {
        LOG_DEBUG("No message stored for ACK pid %d", packetId);
        return;
    }

    auto mqtt_message = mqtt_messages_.front();

    // if the last published failed, don't bother checking it. wait for the next retry
    if (mqtt_message.packet_id_ == 0) {
#if defined(EMSESP_DEBUG)
        LOG_DEBUG("ACK for failed message pid 0");
#endif
        return;
    }

    if (mqtt_message.packet_id_ != packetId) {
        LOG_ERROR("Mismatch, expecting PID %d, got %d", mqtt_message.packet_id_, packetId);
        mqtt_publish_fails_++; // increment error count
    }

    LOG_DEBUG("ACK pid %d", packetId);

    mqtt_messages_.pop_front(); // always remove from queue, regardless if there was a successful ACK
}

// called when MQTT settings have changed via the Web forms
void Mqtt::reset_mqtt() {
    if (!mqtt_enabled_) {
        mqtt_messages_.clear();
    }

    if (!mqttClient_) {
        return;
    }

    if (mqttClient_->connected()) {
        mqttClient_->disconnect(true); // force a disconnect
    }
}

void Mqtt::load_settings() {
    EMSESP::esp8266React.getMqttSettingsService()->read([&](MqttSettings & mqttSettings) {
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
        publish_time_other_      = mqttSettings.publish_time_other * 1000;
        publish_time_sensor_     = mqttSettings.publish_time_sensor * 1000;
        publish_time_heartbeat_  = mqttSettings.publish_time_heartbeat * 1000;
    });

    // create basename from base
    // by taking the MQTT base path and replacing all / with underscores
    mqtt_basename_ = mqtt_base_;
    std::replace(mqtt_basename_.begin(), mqtt_basename_.end(), '/', '_');
}

void Mqtt::start() {
    mqttClient_ = EMSESP::esp8266React.getMqttClient();

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

    mqttClient_->onConnect([this](bool sessionPresent) { on_connect(); });

    mqttClient_->onDisconnect([this](AsyncMqttClientDisconnectReason reason) {
        // only show the error once, not every 2 seconds
        if (!connecting_ && first_connect_attempted_) {
            return;
        }
        first_connect_attempted_ = true;
        connecting_              = false;
        if (reason == AsyncMqttClientDisconnectReason::TCP_DISCONNECTED) {
            LOG_WARNING("MQTT disconnected: TCP");
        } else if (reason == AsyncMqttClientDisconnectReason::MQTT_IDENTIFIER_REJECTED) {
            LOG_WARNING("MQTT disconnected: Identifier Rejected");
        } else if (reason == AsyncMqttClientDisconnectReason::MQTT_SERVER_UNAVAILABLE) {
            LOG_WARNING("MQTT disconnected: Server unavailable");
        } else if (reason == AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS) {
            LOG_WARNING("MQTT disconnected: Malformed credentials");
        } else if (reason == AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED) {
            LOG_WARNING("MQTT disconnected: Not authorized");
        } else {
            LOG_WARNING("MQTT disconnected: code %d", reason);
        }
    });

    // create last will topic with the base prefixed. It has to be static because asyncmqttclient destroys the reference
    static char will_topic[MQTT_TOPIC_MAX_SIZE];
    if (!mqtt_base_.empty()) {
        snprintf(will_topic, MQTT_TOPIC_MAX_SIZE, "%s/status", mqtt_base_.c_str());
    }

    mqttClient_->setWill(will_topic, 1, true, "offline"); // with qos 1, retain true

    mqttClient_->onMessage([this](const char * topic, const char * payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
        on_message(topic, payload, len); // receiving mqtt
    });

    mqttClient_->onPublish([this](uint16_t packetId) {
        on_publish(packetId); // publish
    });
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
    } else if (!publish_time_other_) {
        return true;
    }
    return false;
}

// MQTT onConnect - when an MQTT connect is established
void Mqtt::on_connect() {
    if (connecting_) {
        return; // prevent duplicated connections
    }

    LOG_INFO("MQTT connected");

    connecting_ = true;
    connectcount_++; // count # reconnects. not currently used.

    load_settings(); // reload MQTT settings - in case they have changes

    if (ha_enabled_) {
        queue_unsubscribe_message(discovery_prefix_ + "/+/" + mqtt_basename_ + "/#");
        EMSESP::reset_mqtt_ha(); // re-create all HA devices if there are any
        ha_status();             // create the EMS-ESP device in HA, which is MQTT retained
        ha_climate_reset(true);
    } else {
        // with disabled HA we subscribe and the broker sends all stored HA-emsesp-configs.
        // Around line 272 they are removed (search for "// remove HA topics if we don't use discover")
        // If HA is enabled the subscriptions are removed.
        // As described in the doc (https://emsesp.github.io/docs/#/Troubleshooting?id=home-assistant):
        // disable HA, wait 5 minutes (to allow the broker to send all), than reenable HA again.
        queue_subscribe_message(discovery_prefix_ + "/+/" + mqtt_basename_ + "/#");
    }

    // send initial MQTT messages for some of our services
    EMSESP::shower_.set_shower_state(false, true); // Send shower_activated as false
    EMSESP::system_.send_heartbeat();              // send heartbeat

    // re-subscribe to all custom registered MQTT topics
    resubscribe();

    // publish to the last will topic (see Mqtt::start() function) to say we're alive
    queue_publish_retain("status", "online", true); // with retain on

    mqtt_publish_fails_ = 0; // reset fail count to 0

    /*
    // for debugging only
    LOG_INFO("Queue size: %d", mqtt_messages_.size());
    for (const auto & message : mqtt_messages_) {
        auto content = message.content_;
        LOG_INFO(" [%02d] (%d) topic=%s payload=%s", message.id_, content->operation, content->topic.c_str(), content->payload.c_str());
    }
    */
}

// Home Assistant Discovery - the main master Device called EMS-ESP
// e.g. homeassistant/sensor/ems-esp/status/config
// all the values from the heartbeat payload will be added as attributes to the entity state
void Mqtt::ha_status() {
    StaticJsonDocument<EMSESP_JSON_SIZE_LARGE> doc;

    char uniq[70];
    if (Mqtt::entity_format() == 2) {
        snprintf(uniq, sizeof(uniq), "%s_system_status", mqtt_basename_.c_str());
    } else {
        strcpy(uniq, "system_status");
    }

    doc["uniq_id"] = uniq;
    doc["obj_id"]  = uniq;

    doc["stat_t"]   = mqtt_base_ + "/status";
    doc["name"]     = "EMS-ESP status";
    doc["pl_on"]    = "online";
    doc["pl_off"]   = "offline";
    doc["stat_cla"] = "measurement";
    doc["dev_cla"]  = "connectivity";
    doc["ent_cat"]  = "diagnostic";

    // doc["avty_t"]      = "~/status"; // commented out, as it causes errors in HA sometimes
    // doc["json_attr_t"] = "~/heartbeat"; // store also as HA attributes

    JsonObject dev = doc.createNestedObject("dev");
    dev["name"]    = "EMS-ESP";
    dev["sw"]      = "v" + std::string(EMSESP_APP_VERSION);
    dev["mf"]      = "proddy";
    dev["mdl"]     = "EMS-ESP";
#ifndef EMSESP_STANDALONE
    dev["cu"] = "http://" + (EMSESP::system_.ethernet_connected() ? ETH.localIP().toString() : WiFi.localIP().toString());
#endif
    JsonArray ids = dev.createNestedArray("ids");
    ids.add("ems-esp");

    char topic[MQTT_TOPIC_MAX_SIZE];
    snprintf(topic, sizeof(topic), "binary_sensor/%s/system_status/config", mqtt_basename_.c_str());
    Mqtt::queue_ha(topic, doc.as<JsonObject>()); // publish the config payload with retain flag

    // create the sensors - must match the MQTT payload keys
    // these are all from the heartbeat MQTT topic
    if (!EMSESP::system_.ethernet_connected() || WiFi.isConnected()) {
        publish_system_ha_sensor_config(DeviceValueType::INT, "WiFi RSSI", "rssi", DeviceValueUOM::DBM);
        publish_system_ha_sensor_config(DeviceValueType::INT, "WiFi strength", "wifistrength", DeviceValueUOM::PERCENT);
    }

    publish_system_ha_sensor_config(DeviceValueType::STRING, "EMS Bus", "bus_status", DeviceValueUOM::NONE);
    publish_system_ha_sensor_config(DeviceValueType::STRING, "Uptime", "uptime", DeviceValueUOM::NONE);
    publish_system_ha_sensor_config(DeviceValueType::INT, "Uptime (sec)", "uptime_sec", DeviceValueUOM::SECONDS);
    publish_system_ha_sensor_config(DeviceValueType::BOOL, "NTP status", "ntp_status", DeviceValueUOM::CONNECTIVITY);
    publish_system_ha_sensor_config(DeviceValueType::INT, "Free memory", "freemem", DeviceValueUOM::KB);
    publish_system_ha_sensor_config(DeviceValueType::INT, "Max Alloc", "max_alloc", DeviceValueUOM::KB);
    publish_system_ha_sensor_config(DeviceValueType::INT, "MQTT fails", "mqttfails", DeviceValueUOM::NONE);
    publish_system_ha_sensor_config(DeviceValueType::INT, "Rx received", "rxreceived", DeviceValueUOM::NONE);
    publish_system_ha_sensor_config(DeviceValueType::INT, "Rx fails", "rxfails", DeviceValueUOM::NONE);
    publish_system_ha_sensor_config(DeviceValueType::INT, "Tx reads", "txreads", DeviceValueUOM::NONE);
    publish_system_ha_sensor_config(DeviceValueType::INT, "Tx writes", "txwrites", DeviceValueUOM::NONE);
    publish_system_ha_sensor_config(DeviceValueType::INT, "Tx fails", "txfails", DeviceValueUOM::NONE);
}

// add sub or pub task to the queue.
// the base is not included in the topic
void Mqtt::queue_message(const uint8_t operation, const std::string & topic, const std::string & payload, bool retain) {
    if (topic.empty()) {
        return;
    }

#ifndef EMSESP_STANDALONE
    // anything below 60MB available free heap is dangerously low, so we stop adding to prevent a crash
    // instead of doing a mqtt_messages_.pop_front()
    if (mqtt_messages_.size() >= MAX_MQTT_MESSAGES || ESP.getFreeHeap() < (60 * 1024)) {
        LOG_WARNING("Queue overflow (queue count=%d, topic=%s)", mqtt_messages_.size(), topic.c_str());
        mqtt_publish_fails_++;
        return; // don't add to top of queue
    }
#endif

    // take the topic and prefix the base, unless its for HA
    std::shared_ptr<MqttMessage> message = std::make_shared<MqttMessage>(operation, topic, payload, retain);

    if (operation == Operation::PUBLISH) {
        if (message->payload.empty()) {
            LOG_DEBUG("Adding to queue: (remove) topic='%s'", message->topic.c_str());
        } else {
            LOG_DEBUG("Adding to queue: (publish) topic='%s' payload=%s", message->topic.c_str(), message->payload.c_str());
        }
    } else {
        LOG_DEBUG("Adding to queue: (subscribe) topic='%s'", message->topic.c_str());
    }

    mqtt_messages_.emplace_back(mqtt_message_id_++, std::move(message));

    return;
}

// add MQTT message to queue, payload is a string
void Mqtt::queue_publish_message(const std::string & topic, const std::string & payload, bool retain) {
    if (!enabled()) {
        return;
    };
    queue_message(Operation::PUBLISH, topic, payload, retain);
}

// add MQTT subscribe message to queue
void Mqtt::queue_subscribe_message(const std::string & topic) {
    queue_message(Operation::SUBSCRIBE, topic, "", false); // no payload
}

// add MQTT unsubscribe message to queue
void Mqtt::queue_unsubscribe_message(const std::string & topic) {
    queue_message(Operation::UNSUBSCRIBE, topic, "", false); // no payload
}

// MQTT Publish, using a user's retain flag
void Mqtt::queue_publish(const std::string & topic, const std::string & payload) {
    queue_publish_message(topic, payload, mqtt_retain_);
}

// MQTT Publish, using a user's retain flag - except for char * strings
void Mqtt::queue_publish(const char * topic, const char * payload) {
    queue_publish_message((topic), payload, mqtt_retain_);
}

// MQTT Publish, using a specific retain flag, topic is a flash string
void Mqtt::queue_publish(const char * topic, const std::string & payload) {
    queue_publish_message((topic), payload, mqtt_retain_);
}

void Mqtt::queue_publish(const char * topic, const JsonObject & payload) {
    queue_publish_retain(topic, payload, mqtt_retain_);
}

// publish json doc, only if its not empty
void Mqtt::queue_publish(const std::string & topic, const JsonObject & payload) {
    queue_publish_retain(topic, payload, mqtt_retain_);
}

// MQTT Publish, using a specific retain flag, topic is a flash string, forcing retain flag
void Mqtt::queue_publish_retain(const char * topic, const std::string & payload, bool retain) {
    queue_publish_message((topic), payload, retain);
}

// publish json doc, only if its not empty, using the retain flag
void Mqtt::queue_publish_retain(const std::string & topic, const JsonObject & payload, bool retain) {
    queue_publish_retain(topic.c_str(), payload, retain);
}

void Mqtt::queue_publish_retain(const char * topic, const JsonObject & payload, bool retain) {
    if (enabled() && payload.size()) {
        std::string payload_text;
        serializeJson(payload, payload_text); // convert json to string
        queue_publish_message(topic, payload_text, retain);
    }
}

// publish empty payload to remove the topic
void Mqtt::queue_remove_topic(const char * topic) {
    if (!enabled()) {
        return;
    }

    if (ha_enabled_) {
        queue_publish_message(Mqtt::discovery_prefix() + topic, "", true); // publish with retain to remove from broker
    } else {
        queue_publish_message(topic, "", true); // publish with retain to remove from broker
    }
}

// queue a Home Assistant config topic and payload, with retain flag off.
void Mqtt::queue_ha(const char * topic, const JsonObject & payload) {
    if (!enabled()) {
        return;
    }

    std::string payload_text;
    payload_text.reserve(measureJson(payload) + 1);
    serializeJson(payload, payload_text); // convert json to string

    queue_publish_message(Mqtt::discovery_prefix() + topic, payload_text, true); // with retain true
}

// take top from queue and perform the publish or subscribe action
// assumes there is an MQTT connection
void Mqtt::process_queue() {
    if (mqtt_messages_.empty()) {
        return;
    }

    // fetch first from queue and create the full topic name
    auto mqtt_message = mqtt_messages_.front();
    auto message      = mqtt_message.content_;
    char topic[MQTT_TOPIC_MAX_SIZE];

    if (message->topic.find(discovery_prefix_) == 0) {
        strlcpy(topic, message->topic.c_str(), sizeof(topic)); // leave topic as it is
    } else {
        // it's a discovery topic, added the mqtt base to the topic path
        snprintf(topic, MQTT_TOPIC_MAX_SIZE, "%s/%s", mqtt_base_.c_str(), message->topic.c_str()); // uses base
    }

    // if this has already been published and we're waiting for an ACK, don't publish again
    // it will have a real packet ID
    if (mqtt_message.packet_id_ > 0) {
        LOG_DEBUG("Waiting for QOS-ACK");
        // if we don't get the ack within 10 minutes, republish with new packet_id
        if (uuid::get_uptime_sec() - last_publish_queue_ < 600) {
            return;
        }
    }
    last_publish_queue_ = uuid::get_uptime_sec();

    // if we're subscribing...
    if (message->operation == Operation::SUBSCRIBE) {
        LOG_DEBUG("Subscribing to topic '%s'", topic);
        uint16_t packet_id = mqttClient_->subscribe(topic, mqtt_qos_);
        if (!packet_id) {
            if (++mqtt_messages_.front().retry_count_ < MQTT_PUBLISH_MAX_RETRY) {
                return;
            }
            LOG_ERROR("Error subscribing to topic '%s'", topic);
            mqtt_publish_fails_++; // increment failure counter
        }

        mqtt_messages_.pop_front(); // remove the message from the queue

        return;
    }

    // if we're unsubscribing...
    if (message->operation == Operation::UNSUBSCRIBE) {
        LOG_DEBUG("Subscribing to topic '%s'", topic);
        uint16_t packet_id = mqttClient_->unsubscribe(topic);
        if (!packet_id) {
            if (++mqtt_messages_.front().retry_count_ < MQTT_PUBLISH_MAX_RETRY) {
                return;
            }
            LOG_ERROR("Error unsubscribing to topic '%s'", topic);
            mqtt_publish_fails_++; // increment failure counter
        }

        mqtt_messages_.pop_front(); // remove the message from the queue

        return;
    }

    // else try and publish it
    // this is where the *real* publish happens
    uint16_t packet_id = mqttClient_->publish(topic, mqtt_qos_, message->retain, message->payload.c_str(), message->payload.size(), false, mqtt_message.id_);
    lasttopic_         = topic;
    lastpayload_       = message->payload;

    LOG_DEBUG("Published topic %s (#%02d, retain=%d, retry=%d, size=%d, pid=%d)",
              topic,
              mqtt_message.id_,
              message->retain,
              mqtt_message.retry_count_ + 1,
              message->payload.size(),
              packet_id);

    /*
    if (!message->payload.empty()) {
        LOG_DEBUG("Payload: %s", message->payload.c_str()); // extra message for #784
    }
    */

    if (packet_id == 0) {
        // it failed. if we retried n times, give up. remove from queue
        if (mqtt_message.retry_count_ == (MQTT_PUBLISH_MAX_RETRY - 1)) {
            LOG_ERROR("Failed to publish to %s after %d attempts", topic, mqtt_message.retry_count_ + 1);
            mqtt_publish_fails_++;      // increment failure counter
            mqtt_messages_.pop_front(); // delete
            return;
        } else {
            // update the record
            mqtt_messages_.front().retry_count_++;
            LOG_DEBUG("Failed to publish to %s. Trying again, #%d", topic, mqtt_message.retry_count_ + 1);
            return; // leave on queue for next time so it gets republished
        }
    }

    // if we have ACK set with QOS 1 or 2, leave on queue and let the ACK process remove it
    // but add the packet_id so we can check it later
    if (mqtt_qos_ != 0) {
        mqtt_messages_.front().packet_id_ = packet_id;
        LOG_DEBUG("Setting packetID for ACK to %d", packet_id);
        return;
    }

    mqtt_messages_.pop_front(); // remove the message from the queue
}

// create's a ha sensor config topic from a device value object
// and also takes a flag (create_device_config) used to also create the main HA device config. This is only needed for one entity
void Mqtt::publish_ha_sensor_config(DeviceValue & dv, const char * model, const char * brand, const bool remove, const bool create_device_config) {
    StaticJsonDocument<EMSESP_JSON_SIZE_LARGE> dev_json;

    // always create the ids
    JsonArray ids = dev_json.createNestedArray("ids");
    char      ha_device[40];
    auto      device_type_name = EMSdevice::device_type_2_device_name(dv.device_type);
    snprintf(ha_device, sizeof(ha_device), "ems-esp-%s", device_type_name);
    ids.add(ha_device);

    if (create_device_config) {
        auto cap_name = strdup(device_type_name);
        Helpers::CharToUpperUTF8(cap_name); // capitalize first letter
        dev_json["name"]       = std::string("EMS-ESP ") + cap_name;
        dev_json["mf"]         = brand;
        dev_json["mdl"]        = model;
        dev_json["via_device"] = "ems-esp";
        free(cap_name);
    }

    // calculate the min and max
    int16_t  dv_set_min;
    uint16_t dv_set_max;
    (void)dv.get_min_max(dv_set_min, dv_set_max);

    // determine if we're creating the command topics which we use special HA configs
    // unless the entity has been marked as read-only and so it'll default to using the sensor/ type
    bool has_cmd = dv.has_cmd && !dv.has_state(DeviceValueState::DV_READONLY);

    publish_ha_sensor_config(dv.type,
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
                             dev_json.as<JsonObject>());
}

// publish HA sensor for System using the heartbeat tag
void Mqtt::publish_system_ha_sensor_config(uint8_t type, const char * name, const char * entity, const uint8_t uom) {
    StaticJsonDocument<EMSESP_JSON_SIZE_LARGE> doc;
    JsonObject                                 dev_json = doc.createNestedObject("dev");

    JsonArray ids = dev_json.createNestedArray("ids");
    ids.add("ems-esp");

    publish_ha_sensor_config(type, DeviceValueTAG::TAG_HEARTBEAT, name, name, EMSdevice::DeviceType::SYSTEM, entity, uom, false, false, nullptr, 0, 0, 0, 0, dev_json);
}

// MQTT discovery configs
// entity must match the key/value pair in the *_data topic
// note: some extra string copying done here, it looks messy but does help with heap fragmentation issues
void Mqtt::publish_ha_sensor_config(uint8_t               type,        // EMSdevice::DeviceValueType
                                    uint8_t               tag,         // EMSdevice::DeviceValueTAG
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
                                    const int16_t         dv_set_max,
                                    const int8_t          num_op,
                                    const JsonObject &    dev_json) {
    // ignore if name (fullname) is empty
    if (!fullname || !en_name) {
        return;
    }

    // create the device name
    auto device_name = EMSdevice::device_type_2_device_name(device_type);

    bool has_tag = ((tag < DeviceValue::NUM_TAGS) && (tag != DeviceValue::DeviceValueTAG::TAG_NONE) && strlen(DeviceValue::DeviceValueTAG_s[tag][0]));

    // create entity by add the hc/wwc tag if present, separating with an _
    char entity_with_tag[50];
    if (tag >= DeviceValueTAG::TAG_HC1) {
        snprintf(entity_with_tag, sizeof(entity_with_tag), "%s_%s", EMSdevice::tag_to_mqtt(tag), entity);
    } else {
        snprintf(entity_with_tag, sizeof(entity_with_tag), "%s", entity);
    }

    // build unique identifier also used as object_id which also becomes the Entity ID in HA
    char uniq_id[80];
    if (Mqtt::entity_format() == 2) {
        // prefix base name to each uniq_id and use the shortname
        snprintf(uniq_id, sizeof(uniq_id), "%s_%s_%s", mqtt_basename_.c_str(), device_name, entity_with_tag);
    } else if (Mqtt::entity_format() == 1) {
        // shortname, no mqtt base. This is the default version.
        snprintf(uniq_id, sizeof(uniq_id), "%s_%s", device_name, entity_with_tag);
    } else {
        // entity_format is 0, the old v3.4 style
        // take en_name and replace all spaces
        char uniq_s[60];
        strlcpy(uniq_s, en_name, sizeof(uniq_s));
        Helpers::replace_char(uniq_s, ' ', '_');
        if (has_tag) {
            snprintf(uniq_id, sizeof(uniq_id), "%s_%s", device_name, Helpers::toLower(uniq_s).c_str());
        } else {
            snprintf(uniq_id, sizeof(uniq_id), "%s_%s_%s", device_name, DeviceValue::DeviceValueTAG_s[tag][0], Helpers::toLower(uniq_s).c_str());
        }
    }

    // build a config topic that will be prefix onto a HA type (e.g. number, switch)
    char config_topic[70];
    snprintf(config_topic, sizeof(config_topic), "%s/%s_%s/config", mqtt_basename_.c_str(), device_name, entity_with_tag);

    bool set_ha_classes = false; // set to true if we want to set the state class and device class

    // create the topic, depending on the type and whether the device entity is writable (a command)
    // https://developers.home-assistant.io/docs/core/entity
    char topic[MQTT_TOPIC_MAX_SIZE];
    // if it's a command then we can use Number, Switch, Select. Otherwise stick to Sensor
    if (has_cmd) {
        switch (type) {
        case DeviceValueType::INT:
        case DeviceValueType::UINT:
        case DeviceValueType::SHORT:
        case DeviceValueType::USHORT:
        case DeviceValueType::ULONG:
            if (discovery_type() == 0) {
                // Home Assistant
                // number - https://www.home-assistant.io/integrations/number.mqtt
                snprintf(topic, sizeof(topic), "number/%s", config_topic);
            } else {
                // Domoticz
                // Does not support number, use sensor
                snprintf(topic, sizeof(topic), "sensor/%s", config_topic);
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
        default:
            // plain old sensor
            snprintf(topic, sizeof(topic), "sensor/%s", config_topic);
            break;
        }
    } else {
        set_ha_classes = true; // these are Read only sensors. We can set the device class and state class
        // plain old read only device entity
        if (type == DeviceValueType::BOOL) {
            snprintf(topic, sizeof(topic), "binary_sensor/%s", config_topic); // binary sensor (for booleans)
        } else {
            snprintf(topic, sizeof(topic), "sensor/%s", config_topic); // normal HA sensor
        }
    }

    // if we're asking to remove this topic, send an empty payload and exit
    // https://github.com/emsesp/EMS-ESP32/issues/196
    if (remove) {
        LOG_DEBUG("Queuing removing topic %s", topic);
        queue_remove_topic(topic);
        return;
    }

    // build the payload
    StaticJsonDocument<EMSESP_JSON_SIZE_LARGE> doc;
    doc["uniq_id"] = uniq_id;
    doc["obj_id"]  = uniq_id; // same as unique_id

    const char * ic_ha  = "ic";           // icon - only set this if there is no device class
    const char * sc_ha  = "stat_cla";     // state class
    const char * uom_ha = "unit_of_meas"; // unit of measure

    char sample_val[30] = "0"; // sample, correct(!) entity value, used only to prevent warning/error in HA if real value is not published yet

    // handle commands, which are device entities that are writable
    // we add the command topic parameter
    // note: there is no way to handle strings in HA so datetimes (e.g. set_datetime, set_holiday, set_wwswitchtime etc) are excluded
    if (has_cmd) {
        // command topic back to EMS-ESP
        char command_topic[MQTT_TOPIC_MAX_SIZE];
        if (tag >= DeviceValueTAG::TAG_HC1) {
            snprintf(command_topic, sizeof(command_topic), "%s/%s/%s/%s", mqtt_basename_.c_str(), device_name, EMSdevice::tag_to_mqtt(tag), entity);
        } else {
            snprintf(command_topic, sizeof(command_topic), "%s/%s/%s", mqtt_basename_.c_str(), device_name, entity);
        }
        doc["cmd_t"] = command_topic;

        // for enums, add options
        if (type == DeviceValueType::ENUM) {
            JsonArray option_list = doc.createNestedArray("ops"); // options
            for (uint8_t i = 0; i < options_size; i++) {
                option_list.add(Helpers::translated_word(options[i]));
            }
            snprintf(sample_val, sizeof(sample_val), "'%s'", Helpers::translated_word(options[0]));
        } else if (type != DeviceValueType::STRING && type != DeviceValueType::BOOL) {
            // Must be Numeric....
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

        // set icons
        // since these don't have a device class we need to add the icon ourselves
        switch (uom) {
        case DeviceValueUOM::DEGREES:
        case DeviceValueUOM::DEGREES_R:
        case DeviceValueUOM::K:
            doc[ic_ha] = F_(icondegrees);
            break;
        case DeviceValueUOM::PERCENT:
            doc[ic_ha] = F_(iconpercent);
            break;
        default:
            break;
        }
    }

    // state topic
    char stat_t[MQTT_TOPIC_MAX_SIZE];
    snprintf(stat_t, sizeof(stat_t), "%s/%s", mqtt_basename_.c_str(), tag_to_topic(device_type, tag).c_str());
    doc["stat_t"] = stat_t;

    // friendly name = <tag> <name>
    char   ha_name[70];
    char * F_name = strdup(fullname);
    Helpers::CharToUpperUTF8(F_name); // capitalize first letter
    if (has_tag) {
        // exclude heartbeat tag
        snprintf(ha_name, sizeof(ha_name), "%s %s", DeviceValue::DeviceValueTAG_s[tag][0], F_name);
    } else {
        snprintf(ha_name, sizeof(ha_name), "%s", F_name); // no tag
    }
    free(F_name); // very important!
    doc["name"] = ha_name;

    // value template
    // if its nested mqtt format then use the appended entity name, otherwise take the original name
    char val_obj[100];
    char val_cond[200];
    if (is_nested() && tag >= DeviceValueTAG::TAG_HC1) {
        snprintf(val_obj, sizeof(val_obj), "value_json.%s.%s", EMSdevice::tag_to_mqtt(tag), entity);
        snprintf(val_cond, sizeof(val_cond), "value_json.%s is defined and %s is defined", EMSdevice::tag_to_mqtt(tag), val_obj);
    } else {
        snprintf(val_obj, sizeof(val_obj), "value_json.%s", entity);
        snprintf(val_cond, sizeof(val_cond), "%s is defined", val_obj);
    }

    // special case to handle booleans
    // applies to both Binary Sensor (read only) and a Switch (for a command)
    // has no unit of measure or icon
    if (type == DeviceValueType::BOOL) {
        if (EMSESP::system_.bool_format() == BOOL_FORMAT_TRUEFALSE) {
            doc["pl_on"]  = true;
            doc["pl_off"] = false;
            snprintf(sample_val, sizeof(sample_val), "false");
        } else if (EMSESP::system_.bool_format() == BOOL_FORMAT_10) {
            doc["pl_on"]  = 1;
            doc["pl_off"] = 0;
        } else {
            char result[12];
            doc["pl_on"]  = Helpers::render_boolean(result, true);
            doc["pl_off"] = Helpers::render_boolean(result, false);
            snprintf(sample_val, sizeof(sample_val), "'%s'", Helpers::render_boolean(result, false));
        }
    } else {
        // always set the uom, using the standards except for hours/minutes/seconds
        // using HA specific codes from https://github.com/home-assistant/core/blob/dev/homeassistant/const.py
        if (uom == DeviceValueUOM::HOURS) {
            doc[uom_ha] = "h";
        } else if (uom == DeviceValueUOM::MINUTES) {
            doc[uom_ha] = "min";
        } else if (uom == DeviceValueUOM::SECONDS) {
            doc[uom_ha] = "s";
        } else if (uom != DeviceValueUOM::NONE) {
            doc[uom_ha] = EMSdevice::uom_to_string(uom); // default
        }
    }
    doc["val_tpl"] = (std::string) "{{" + val_obj + " if " + val_cond + " else " + sample_val + "}}";

    // this next section is adding the state class, device class and sometimes the icon
    // used for Sensor and Binary Sensor Entities in HA
    if (set_ha_classes) {
        const char * dc_ha = "dev_cla"; // device class

        switch (uom) {
        case DeviceValueUOM::DEGREES:
        case DeviceValueUOM::DEGREES_R:
        case DeviceValueUOM::K:
            doc[sc_ha] = F_(measurement);
            doc[dc_ha] = "temperature"; // no icon needed
            break;
        case DeviceValueUOM::PERCENT:
            doc[sc_ha] = F_(measurement);
            doc[dc_ha] = "power_factor"; // no icon needed
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
            doc[ic_ha] = F_(iconkb);
            break;
        case DeviceValueUOM::LMIN:
            doc[ic_ha] = F_(iconlmin);
            doc[sc_ha] = F_(measurement);
            break;
        case DeviceValueUOM::WH:
            if (entity == FL_(energyToday)[0]) {
                doc[sc_ha] = F_(total_increasing);
            } else {
                doc[sc_ha] = F_(measurement);
            }
            doc[dc_ha] = "energy"; // no icon needed
            break;
        case DeviceValueUOM::KWH:
            doc[sc_ha] = F_(total_increasing);
            doc[dc_ha] = "energy"; // no icon needed
            break;
        case DeviceValueUOM::UA:
            doc[ic_ha] = F_(iconua);
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
                && (type == DeviceValueType::INT || type == DeviceValueType::UINT || type == DeviceValueType::SHORT || type == DeviceValueType::USHORT
                    || type == DeviceValueType::ULONG)) {
                doc[ic_ha] = F_(iconnum); // set icon
                // determine if its a measurement or total increasing
                // most of the values are measurement. for example Tx Reads will increment but can be reset to 0 after a restart
                // all the starts are increasing, and they are ULONGs
                if (type == DeviceValueType::ULONG) {
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

    // add category "diagnostic" for system entities
    if (device_type == EMSdevice::DeviceType::SYSTEM) {
        doc["ent_cat"] = "diagnostic";
    }

    // add the dev json object to the end
    doc["dev"] = dev_json;

    // add "availability" section
    add_avty_to_doc(stat_t, doc.as<JsonObject>(), val_cond);

    // TODO queue it or send it directly via publish?
    queue_ha(topic, doc.as<JsonObject>());
}

void Mqtt::publish_ha_climate_config(const uint8_t tag, const bool has_roomtemp, const bool remove, const int16_t min, const uint16_t max) {
    uint8_t hc_num = tag - DeviceValueTAG::TAG_HC1 + 1;

    char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
    char topic_t[Mqtt::MQTT_TOPIC_MAX_SIZE];
    char hc_mode_s[30];
    char seltemp_s[30];
    char currtemp_s[30];
    char hc_mode_cond[80];
    char seltemp_cond[80];
    char currtemp_cond[170];
    char mode_str_tpl[400];
    char name_s[10];
    char uniq_id_s[60];
    char temp_cmd_s[30];
    char mode_cmd_s[30];
    char min_s[10];
    char max_s[10];

    snprintf(topic, sizeof(topic), "climate/%s/thermostat_hc%d/config", mqtt_basename_.c_str(), hc_num);
    if (remove) {
        queue_remove_topic(topic); // publish empty payload with retain flag
        return;
    }

    if (Mqtt::is_nested()) {
        // nested format
        snprintf(hc_mode_s, sizeof(hc_mode_s), "value_json.hc%d.mode", hc_num);
        snprintf(hc_mode_cond, sizeof(hc_mode_cond), "value_json.hc%d is undefined or %s is undefined", hc_num, hc_mode_s);
        snprintf(seltemp_s, sizeof(seltemp_s), "value_json.hc%d.seltemp", hc_num);
        snprintf(seltemp_cond, sizeof(seltemp_cond), "value_json.hc%d is defined and %s is defined", hc_num, seltemp_s);

        if (has_roomtemp) {
            snprintf(currtemp_s, sizeof(currtemp_s), "value_json.hc%d.currtemp", hc_num);
            snprintf(currtemp_cond, sizeof(currtemp_cond), "value_json.hc%d is defined and %s is defined", hc_num, currtemp_s);
        }
        snprintf(topic_t, sizeof(topic_t), "~/%s", Mqtt::tag_to_topic(EMSdevice::DeviceType::THERMOSTAT, DeviceValueTAG::TAG_NONE).c_str());
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
        snprintf(topic_t, sizeof(topic_t), "~/%s", Mqtt::tag_to_topic(EMSdevice::DeviceType::THERMOSTAT, DeviceValueTAG::TAG_HC1 + hc_num - 1).c_str());
    }

    snprintf(mode_str_tpl,
             sizeof(mode_str_tpl),
             "{%%if %s%%}off{%%elif %s=='manual'%%}heat{%%elif %s=='day'%%}heat{%%elif %s=='night'%%}off{%%elif %s=='off'%%}off{%%else%%}auto{%%endif%%}",
             hc_mode_cond,
             hc_mode_s,
             hc_mode_s,
             hc_mode_s,
             hc_mode_s);

    snprintf(name_s, sizeof(name_s), "Hc%d", hc_num);

    if (Mqtt::entity_format() == 2) {
        snprintf(uniq_id_s, sizeof(uniq_id_s), "%s_thermostat_hc%d", mqtt_basename_.c_str(), hc_num); // add basename
    } else {
        snprintf(uniq_id_s, sizeof(uniq_id_s), "thermostat_hc%d", hc_num); // backward compatible with v3.4
    }

    snprintf(temp_cmd_s, sizeof(temp_cmd_s), "~/thermostat/hc%d/seltemp", hc_num);
    snprintf(mode_cmd_s, sizeof(temp_cmd_s), "~/thermostat/hc%d/mode", hc_num);

    StaticJsonDocument<EMSESP_JSON_SIZE_LARGE> doc; // doc is 787 typically so 1024 should be enough

    doc["~"]             = mqtt_base_;
    doc["uniq_id"]       = uniq_id_s;
    doc["obj_id"]        = uniq_id_s; // same as uniq_id
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

    doc["min_temp"]   = Helpers::render_value(min_s, min, 0, EMSESP::system_.fahrenheit() ? 2 : 0);
    doc["max_temp"]   = Helpers::render_value(max_s, max, 0, EMSESP::system_.fahrenheit() ? 2 : 0);
    doc["temp_step"]  = "0.5";
    doc["mode_cmd_t"] = mode_cmd_s;

    // the HA climate component only responds to auto, heat and off
    JsonArray modes = doc.createNestedArray("modes");

    modes.add("auto");
    modes.add("heat");
    modes.add("off");

    JsonObject dev = doc.createNestedObject("dev");
    JsonArray  ids = dev.createNestedArray("ids");
    ids.add("ems-esp-thermostat");

    // add "availability" section
    add_avty_to_doc(topic_t, doc.as<JsonObject>(), seltemp_cond, has_roomtemp ? currtemp_cond : nullptr, hc_mode_cond);

    queue_ha(topic, doc.as<JsonObject>()); // publish the config payload with retain flag
}

// based on the device and tag, create the MQTT topic name (without the basename)
// differs based on whether MQTT nested is enabled
// tag = EMSdevice::DeviceValueTAG
std::string Mqtt::tag_to_topic(uint8_t device_type, uint8_t tag) {
    // the system device is treated differently. The topic is 'heartbeat' and doesn't follow the usual convention
    if (device_type == EMSdevice::DeviceType::SYSTEM) {
        return EMSdevice::tag_to_mqtt(tag);
    }

    std::string topic = EMSdevice::device_type_2_device_name(device_type);

    // if there is a tag add it
    if ((tag == DeviceValueTAG::TAG_BOILER_DATA_WW) || (!is_nested() && tag >= DeviceValueTAG::TAG_HC1)) {
        return topic + "_data_" + EMSdevice::tag_to_mqtt(tag);
    } else {
        return topic + "_data";
    }
}

// adds "availability" section to HA Discovery config
void Mqtt::add_avty_to_doc(const char * state_t, const JsonObject & doc, const char * cond1, const char * cond2, const char * negcond) {
    const char * tpl_draft = "{{'online' if %s else 'offline'}}";
    char         tpl[150];
    JsonArray    avty = doc.createNestedArray("avty");

    StaticJsonDocument<512> avty_json;

    snprintf(tpl, sizeof(tpl), "%s/status", mqtt_base_.c_str());
    avty_json["t"] = tpl;
    snprintf(tpl, sizeof(tpl), tpl_draft, "value == 'online'");
    avty_json["val_tpl"] = tpl;
    avty.add(avty_json);
    avty.clear();

    avty_json["t"] = state_t;
    snprintf(tpl, sizeof(tpl), tpl_draft, cond1 == nullptr ? "value is defined" : cond1);
    avty_json["val_tpl"] = tpl;
    avty.add(avty_json);

    if (cond2 != nullptr) {
        avty.clear();
        snprintf(tpl, sizeof(tpl), tpl_draft, cond2);
        avty_json["val_tpl"] = tpl;
        avty.add(avty_json);
    }

    if (negcond != nullptr) {
        avty.clear();
        snprintf(tpl, sizeof(tpl), "{{'offline' if %s else 'online'}}", negcond);
        avty_json["val_tpl"] = tpl;
        avty.add(avty_json);
    }

    doc["avty_mode"] = "all";
}


} // namespace emsesp
