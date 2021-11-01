/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020  Paul Derbyshire
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

namespace emsesp {

AsyncMqttClient * Mqtt::mqttClient_;

// static parameters we make global
std::string Mqtt::mqtt_base_;
uint8_t     Mqtt::mqtt_qos_;
bool        Mqtt::mqtt_retain_;
uint32_t    Mqtt::publish_time_boiler_;
uint32_t    Mqtt::publish_time_thermostat_;
uint32_t    Mqtt::publish_time_solar_;
uint32_t    Mqtt::publish_time_mixer_;
uint32_t    Mqtt::publish_time_sensor_;
uint32_t    Mqtt::publish_time_other_;
bool        Mqtt::mqtt_enabled_;
uint8_t     Mqtt::ha_climate_format_;
bool        Mqtt::ha_enabled_;
uint8_t     Mqtt::nested_format_;
bool        Mqtt::send_response_;

std::deque<Mqtt::QueuedMqttMessage> Mqtt::mqtt_messages_;
std::vector<Mqtt::MQTTSubFunction>  Mqtt::mqtt_subfunctions_;

uint16_t Mqtt::mqtt_publish_fails_ = 0;
bool     Mqtt::connecting_         = false;
bool     Mqtt::initialized_        = false;
uint8_t  Mqtt::connectcount_       = 0;
uint16_t Mqtt::mqtt_message_id_    = 0;
char     will_topic_[Mqtt::MQTT_TOPIC_MAX_SIZE]; // because MQTT library keeps only char pointer

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

// resubscribe to all MQTT topics
// if it's already in the queue, ignore it
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

    // publish top item from MQTT queue to stop flooding
    if ((uint32_t)(currentMillis - last_mqtt_poll_) > MQTT_PUBLISH_WAIT) {
        last_mqtt_poll_ = currentMillis;
        process_queue();
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
        EMSESP::publish_other_values();
    } else

        if (publish_time_sensor_ && (currentMillis - last_publish_sensor_ > publish_time_sensor_)) {
        last_publish_sensor_ = (currentMillis / publish_time_sensor_) * publish_time_sensor_;
        EMSESP::publish_sensor_values(true);
    }
}

// print MQTT log and other stuff to console
void Mqtt::show_mqtt(uuid::console::Shell & shell) {
    shell.printfln(F("MQTT is %s"), connected() ? uuid::read_flash_string(F_(connected)).c_str() : uuid::read_flash_string(F_(disconnected)).c_str());

    shell.printfln(F("MQTT publish fails count: %lu"), mqtt_publish_fails_);
    shell.println();

    // show subscriptions
    shell.printfln(F("MQTT topic subscriptions:"));
    for (const auto & mqtt_subfunction : mqtt_subfunctions_) {
        shell.printfln(F(" %s/%s"), mqtt_base_.c_str(), mqtt_subfunction.topic_.c_str());
    }
    shell.println();

    // show queues
    if (mqtt_messages_.empty()) {
        shell.printfln(F("MQTT queue is empty"));
        shell.println();
        return;
    }

    shell.printfln(F("MQTT queue (%d/%d messages):"), mqtt_messages_.size(), MAX_MQTT_MESSAGES);

    for (const auto & message : mqtt_messages_) {
        auto content = message.content_;
        char topic[MQTT_TOPIC_MAX_SIZE];
        if ((strncmp(content->topic.c_str(), "homeassistant/", 13) != 0)) {
            snprintf(topic, sizeof(topic), "%s/%s", Mqtt::base().c_str(), content->topic.c_str());
        } else {
            snprintf(topic, sizeof(topic), "%s", content->topic.c_str());
        }

        if (content->operation == Operation::PUBLISH) {
            // Publish messages
            if (message.retry_count_ == 0) {
                if (message.packet_id_ == 0) {
                    shell.printfln(F(" [%02d] (Pub) topic=%s payload=%s"), message.id_, topic, content->payload.c_str());
                } else {
                    shell.printfln(F(" [%02d] (Pub) topic=%s payload=%s (pid %d)"), message.id_, topic, content->payload.c_str(), message.packet_id_);
                }
            } else {
                shell.printfln(F(" [%02d] (Pub) topic=%s payload=%s (pid %d, retry #%d)"),
                               message.id_,
                               topic,
                               content->payload.c_str(),
                               message.packet_id_,
                               message.retry_count_);
            }
        } else {
            // Subscribe messages
            shell.printfln(F(" [%02d] (Sub) topic=%s"), message.id_, topic);
        }
    }
    shell.println();
}

#if defined(EMSESP_DEBUG)
// simulate receiving a MQTT message, used only for testing
void Mqtt::incoming(const char * topic, const char * payload) {
    on_message(topic, payload, strlen(payload));
}
#endif

// received an MQTT message that we subscribed too
// topic is the full path
// payload is json or a single string and converted to a json with key 'value'
void Mqtt::on_message(const char * topic, const char * payload, size_t len) {
    // sometimes the payload is not terminated correctly, so make a copy
    // convert payload to a null-terminated char string
    char message[len + 2];
    strlcpy(message, payload, len + 1);

#if defined(EMSESP_DEBUG)
    if (len) {
        LOG_DEBUG(F("Received topic `%s` => payload `%s` (length %d)"), topic, message, len);
    } else {
        LOG_DEBUG(F("Received topic `%s`"), topic);
    }
#endif

    // check first againts any of our subscribed topics
    for (const auto & mf : mqtt_subfunctions_) {
        // add the base back
        char full_topic[MQTT_TOPIC_MAX_SIZE];
        snprintf(full_topic, sizeof(full_topic), "%s/%s", mqtt_base_.c_str(), mf.topic_.c_str());
        if (!strcmp(topic, full_topic)) {
            if (mf.mqtt_subfunction_) {
                if (!(mf.mqtt_subfunction_)(message)) {
                    LOG_ERROR(F("error: invalid payload %s for this topic %s"), message, topic);
                    if (send_response_) {
                        Mqtt::publish(F_(response), "error: invalid data");
                    }
                }
                return;
            }
        }
    }

    // convert payload into a json doc, if it's not empty
    // if the payload is a single parameter (not JSON) create a JSON with the key 'value'
    StaticJsonDocument<EMSESP_JSON_SIZE_SMALL> input;
    if (len != 0) {
        DeserializationError error = deserializeJson(input, message);
        if (error == DeserializationError::Code::InvalidInput) {
            input.clear();                          // this is important to clear first
            input["value"] = (const char *)message; // always a string
        }
    }

    // parse and call the command
    StaticJsonDocument<EMSESP_JSON_SIZE_LARGE_DYN> output_doc;
    JsonObject                                     output      = output_doc.to<JsonObject>();
    uint8_t                                        return_code = Command::process(topic, true, input.as<JsonObject>(), output); // mqtt is always authenticated


    // send MQTT response if enabled
    if (!send_response_ || output.isNull()) {
        return;
    }

    if (return_code != CommandRet::OK) {
        Mqtt::publish(F_(response), (const char *)output["message"]);
    } else {
        Mqtt::publish(F_(response), output); // output response from call
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

    shell.print(F(" Subscribed MQTT topics: "));
    for (const auto & mqtt_subfunction : mqtt_subfunctions_) {
        if (mqtt_subfunction.device_type_ == device_type) {
            shell.printf(F("%s "), mqtt_subfunction.topic_.c_str());
        }
    }
    shell.println();
}

// called when an MQTT Publish ACK is received
// its a poor man's QOS we assume the ACK represents the last Publish sent
// check if ACK matches the last Publish we sent, if not report an error. Only if qos is 1 or 2
// and always remove from queue
void Mqtt::on_publish(uint16_t packetId) {
    // find the MQTT message in the queue and remove it
    if (mqtt_messages_.empty()) {
#if defined(EMSESP_DEBUG)
        LOG_DEBUG(F("[DEBUG] No message stored for ACK pid %d"), packetId);
#endif
        return;
    }

    auto mqtt_message = mqtt_messages_.front();

    // if the last published failed, don't bother checking it. wait for the next retry
    if (mqtt_message.packet_id_ == 0) {
#if defined(EMSESP_DEBUG)
        LOG_DEBUG(F("[DEBUG] ACK for failed message pid 0"));
#endif
        return;
    }

    if (mqtt_message.packet_id_ != packetId) {
        LOG_ERROR(F("Mismatch, expecting PID %d, got %d"), mqtt_message.packet_id_, packetId);
        mqtt_publish_fails_++; // increment error count
    }

#if defined(EMSESP_DEBUG)
    LOG_DEBUG(F("[DEBUG] ACK pid %d"), packetId);
#endif

    mqtt_messages_.pop_front(); // always remove from queue, regardless if there was a successful ACK
}

// called when MQTT settings have changed via the Web forms
void Mqtt::reset_mqtt() {
    if (!mqttClient_) {
        return;
    }

    if (mqttClient_->connected()) {
        mqttClient_->disconnect(true); // force a disconnect
    }
}

void Mqtt::load_settings() {
    EMSESP::esp8266React.getMqttSettingsService()->read([&](MqttSettings & mqttSettings) {
        mqtt_base_         = mqttSettings.base.c_str(); // Convert String to std::string
        mqtt_qos_          = mqttSettings.mqtt_qos;
        mqtt_retain_       = mqttSettings.mqtt_retain;
        mqtt_enabled_      = mqttSettings.enabled;
        ha_enabled_        = mqttSettings.ha_enabled;
        ha_climate_format_ = mqttSettings.ha_climate_format;
        nested_format_     = mqttSettings.nested_format;
        send_response_     = mqttSettings.send_response;

        // convert to milliseconds
        publish_time_boiler_     = mqttSettings.publish_time_boiler * 1000;
        publish_time_thermostat_ = mqttSettings.publish_time_thermostat * 1000;
        publish_time_solar_      = mqttSettings.publish_time_solar * 1000;
        publish_time_mixer_      = mqttSettings.publish_time_mixer * 1000;
        publish_time_other_      = mqttSettings.publish_time_other * 1000;
        publish_time_sensor_     = mqttSettings.publish_time_sensor * 1000;
    });
}

void Mqtt::start() {
    mqttClient_ = EMSESP::esp8266React.getMqttClient();

    load_settings(); // fetch MQTT settings

    if (!mqtt_enabled_) {
        return; // quit, not using MQTT
    }

    // if already initialized, don't do it again
    if (initialized_) {
        return;
    }
    initialized_ = true;

    mqttClient_->onConnect([this](bool sessionPresent) { on_connect(); });

    mqttClient_->onDisconnect([this](AsyncMqttClientDisconnectReason reason) {
        if (!connecting_) {
            return;
        }
        connecting_ = false;
        if (reason == AsyncMqttClientDisconnectReason::TCP_DISCONNECTED) {
            LOG_INFO(F("MQTT disconnected: TCP"));
        }
        if (reason == AsyncMqttClientDisconnectReason::MQTT_IDENTIFIER_REJECTED) {
            LOG_INFO(F("MQTT disconnected: Identifier Rejected"));
        }
        if (reason == AsyncMqttClientDisconnectReason::MQTT_SERVER_UNAVAILABLE) {
            LOG_INFO(F("MQTT disconnected: Server unavailable"));
        }
        if (reason == AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS) {
            LOG_INFO(F("MQTT disconnected: Malformed credentials"));
        }
        if (reason == AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED) {
            LOG_INFO(F("MQTT disconnected: Not authorized"));
        }
        // remove message with pending ack
        if (!mqtt_messages_.empty()) {
            auto mqtt_message = mqtt_messages_.front();
            if (mqtt_message.packet_id_ != 0) {
                mqtt_messages_.pop_front();
            }
        }
        // mqtt_messages_.clear();
    });

    // create will_topic with the base prefixed. It has to be static because asyncmqttclient destroys the reference
    static char will_topic[MQTT_TOPIC_MAX_SIZE];
    snprintf(will_topic, MQTT_TOPIC_MAX_SIZE, "%s/status", mqtt_base_.c_str());
    mqttClient_->setWill(will_topic, 1, true, "offline"); // with qos 1, retain true

    mqttClient_->onMessage([this](char * topic, char * payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
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

bool Mqtt::get_publish_onchange(uint8_t device_type) {
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
// send out some inital MQTT messages
void Mqtt::on_connect() {
    if (connecting_) { // prevent duplicated connections
        return;
    }

    LOG_INFO(F("MQTT connected"));

    connecting_ = true;
    connectcount_++;

    load_settings(); // reload MQTT settings - in case they have changes

    // send info topic appended with the version information as JSON
    StaticJsonDocument<EMSESP_JSON_SIZE_SMALL> doc;
    // first time to connect
    if (connectcount_ == 1) {
        doc["event"] = FJSON("start");
    } else {
        doc["event"] = FJSON("reconnect");
    }

    doc["version"] = EMSESP_APP_VERSION;
#ifndef EMSESP_STANDALONE
    if (EMSESP::system_.ethernet_connected()) {
        doc["ip"] = ETH.localIP().toString();
        if (ETH.localIPv6().toString() != "0000:0000:0000:0000:0000:0000:0000:0000") {
            doc["ipv6"] = ETH.localIPv6().toString();
        }
    } else {
        doc["ip"] = WiFi.localIP().toString();
        if (WiFi.localIPv6().toString() != "0000:0000:0000:0000:0000:0000:0000:0000") {
            doc["ipv6"] = WiFi.localIPv6().toString();
        }
    }
#endif
    publish(F_(info), doc.as<JsonObject>()); // topic called "info"

    // create the EMS-ESP device in HA, which is MQTT retained
    if (ha_enabled()) {
        ha_status();
    }

    // send initial MQTT messages for some of our services
    EMSESP::shower_.send_mqtt_stat(false); // Send shower_activated as false
    EMSESP::system_.send_heartbeat();      // send heatbeat

    // re-subscribe to all custom registered MQTT topics
    resubscribe();

    EMSESP::reset_mqtt_ha(); // re-create all HA devices if there are any

    publish_retain(F("status"), "online", true); // say we're alive to the Last Will topic, with retain on

    mqtt_publish_fails_ = 0; // reset fail count to 0

    /*
    // for debugging only
    LOG_INFO("Queue size: %d", mqtt_messages_.size());
    for (const auto & message : mqtt_messages_) {
        auto content = message.content_;
        LOG_INFO(F(" [%02d] (%d) topic=%s payload=%s"), message.id_, content->operation, content->topic.c_str(), content->payload.c_str());
    }
    */
}

// Home Assistant Discovery - the main master Device called EMS-ESP
// e.g. homeassistant/sensor/ems-esp/status/config
// all the values from the heartbeat payload will be added as attributes to the entity state
void Mqtt::ha_status() {
    StaticJsonDocument<EMSESP_JSON_SIZE_HA_CONFIG> doc;

    doc["uniq_id"] = FJSON("ems-esp-system");
    doc["~"]       = mqtt_base_; // default ems-esp
    // doc["avty_t"]      = FJSON("~/status"); // commented out, as it causes errors in HA sometimes
    // doc["json_attr_t"] = FJSON("~/heartbeat"); // store also as HA attributes
    doc["stat_t"]  = FJSON("~/heartbeat");
    doc["name"]    = FJSON("EMS-ESP status");
    doc["ic"]      = F_(icondevice);
    doc["val_tpl"] = FJSON("{{value_json['status']}}");

    JsonObject dev = doc.createNestedObject("dev");
    dev["name"]    = F_(EMSESP); // "EMS-ESP"
    dev["sw"]      = EMSESP_APP_VERSION;
    dev["mf"]      = FJSON("proddy");
    dev["mdl"]     = F_(EMSESP); // "EMS-ESP"
    JsonArray ids  = dev.createNestedArray("ids");
    ids.add("ems-esp");

    char topic[MQTT_TOPIC_MAX_SIZE];
    snprintf(topic, sizeof(topic), "sensor/%s/system/config", mqtt_base_.c_str());
    Mqtt::publish_ha(topic, doc.as<JsonObject>()); // publish the config payload with retain flag

    // create the sensors - must match the MQTT payload keys
    if (!EMSESP::system_.ethernet_connected()) {
        publish_ha_sensor(DeviceValueType::INT, DeviceValueTAG::TAG_HEARTBEAT, F("WiFi RSSI"), EMSdevice::DeviceType::SYSTEM, F("rssi"), DeviceValueUOM::DBM);
        publish_ha_sensor(DeviceValueType::INT,
                          DeviceValueTAG::TAG_HEARTBEAT,
                          F("WiFi strength"),
                          EMSdevice::DeviceType::SYSTEM,
                          F("wifistrength"),
                          DeviceValueUOM::PERCENT);
    }

    publish_ha_sensor(DeviceValueType::INT, DeviceValueTAG::TAG_HEARTBEAT, F("Uptime"), EMSdevice::DeviceType::SYSTEM, F("uptime"), DeviceValueUOM::NONE);
    publish_ha_sensor(DeviceValueType::INT, DeviceValueTAG::TAG_HEARTBEAT, F("Uptime (sec)"), EMSdevice::DeviceType::SYSTEM, F("uptime_sec"), DeviceValueUOM::SECONDS);
    publish_ha_sensor(DeviceValueType::INT, DeviceValueTAG::TAG_HEARTBEAT, F("Free memory"), EMSdevice::DeviceType::SYSTEM, F("freemem"), DeviceValueUOM::KB);
    publish_ha_sensor(DeviceValueType::INT, DeviceValueTAG::TAG_HEARTBEAT, F("MQTT fails"), EMSdevice::DeviceType::SYSTEM, F("mqttfails"), DeviceValueUOM::TIMES);
    publish_ha_sensor(DeviceValueType::INT, DeviceValueTAG::TAG_HEARTBEAT, F("Rx received"), EMSdevice::DeviceType::SYSTEM, F("rxreceived"), DeviceValueUOM::TIMES);
    publish_ha_sensor(DeviceValueType::INT, DeviceValueTAG::TAG_HEARTBEAT, F("Rx fails"), EMSdevice::DeviceType::SYSTEM, F("rxfails"), DeviceValueUOM::TIMES);
    publish_ha_sensor(DeviceValueType::INT, DeviceValueTAG::TAG_HEARTBEAT, F("Tx reads"), EMSdevice::DeviceType::SYSTEM, F("txreads"), DeviceValueUOM::TIMES);
    publish_ha_sensor(DeviceValueType::INT, DeviceValueTAG::TAG_HEARTBEAT, F("Tx writes"), EMSdevice::DeviceType::SYSTEM, F("txwrites"), DeviceValueUOM::TIMES);
    publish_ha_sensor(DeviceValueType::INT, DeviceValueTAG::TAG_HEARTBEAT, F("Tx fails"), EMSdevice::DeviceType::SYSTEM, F("txfails"), DeviceValueUOM::TIMES);
}

// add sub or pub task to the queue.
// returns a pointer to the message created
// the base is not included in the topic
std::shared_ptr<const MqttMessage> Mqtt::queue_message(const uint8_t operation, const std::string & topic, const std::string & payload, bool retain) {
    if (topic.empty()) {
        return nullptr;
    }

    // if it's a publish and the payload is empty, stop
    if ((operation == Operation::PUBLISH) && (payload.empty())) {
        return nullptr;
    }

    // take the topic and prefix the base, unless its for HA
    std::shared_ptr<MqttMessage> message;
    message = std::make_shared<MqttMessage>(operation, topic, payload, retain);

#ifdef EMSESP_DEBUG
    if (operation == Operation::PUBLISH) {
        LOG_INFO("[DEBUG] Adding to queue: (Publish) topic='%s' payload=%s", message->topic.c_str(), message->payload.c_str());
    } else {
        LOG_INFO("[DEBUG] Adding to queue: (Subscribe) topic='%s'", message->topic.c_str());
    }
#endif

    // if the queue is full, make room but removing the last one
    if (mqtt_messages_.size() >= MAX_MQTT_MESSAGES) {
        mqtt_messages_.pop_front();
    }
    mqtt_messages_.emplace_back(mqtt_message_id_++, std::move(message));

    return mqtt_messages_.back().content_; // this is because the message has been moved
}

// add MQTT message to queue, payload is a string
std::shared_ptr<const MqttMessage> Mqtt::queue_publish_message(const std::string & topic, const std::string & payload, bool retain) {
    if (!enabled()) {
        return nullptr;
    };
    return queue_message(Operation::PUBLISH, topic, payload, retain);
}

// add MQTT subscribe message to queue
std::shared_ptr<const MqttMessage> Mqtt::queue_subscribe_message(const std::string & topic) {
    return queue_message(Operation::SUBSCRIBE, topic, "", false); // no payload
}

// MQTT Publish, using a user's retain flag
void Mqtt::publish(const std::string & topic, const std::string & payload) {
    queue_publish_message(topic, payload, mqtt_retain_);
}

// MQTT Publish, using a user's retain flag - except for char * strings
void Mqtt::publish(const __FlashStringHelper * topic, const char * payload) {
    queue_publish_message(uuid::read_flash_string(topic), payload, mqtt_retain_);
}

// MQTT Publish, using a specific retain flag, topic is a flash string
void Mqtt::publish(const __FlashStringHelper * topic, const std::string & payload) {
    queue_publish_message(uuid::read_flash_string(topic), payload, mqtt_retain_);
}

void Mqtt::publish(const __FlashStringHelper * topic, const JsonObject & payload) {
    publish(uuid::read_flash_string(topic), payload);
}

// publish json doc, only if its not empty
void Mqtt::publish(const std::string & topic, const JsonObject & payload) {
    publish_retain(topic, payload, mqtt_retain_);
}

// no payload
void Mqtt::publish(const std::string & topic) {
    queue_publish_message(topic, "", false);
}

// MQTT Publish, using a specific retain flag, topic is a flash string, forcing retain flag
void Mqtt::publish_retain(const __FlashStringHelper * topic, const std::string & payload, bool retain) {
    queue_publish_message(uuid::read_flash_string(topic), payload, retain);
}

// publish json doc, only if its not empty, using the retain flag
void Mqtt::publish_retain(const std::string & topic, const JsonObject & payload, bool retain) {
    if (enabled() && payload.size()) {
        std::string payload_text;
        serializeJson(payload, payload_text); // convert json to string
        queue_publish_message(topic, payload_text, retain);
    }
}

void Mqtt::publish_retain(const __FlashStringHelper * topic, const JsonObject & payload, bool retain) {
    publish_retain(uuid::read_flash_string(topic), payload, retain);
}

void Mqtt::publish_ha(const __FlashStringHelper * topic, const JsonObject & payload) {
    publish_ha(uuid::read_flash_string(topic), payload);
}

// publish a Home Assistant config topic and payload, with retain flag off.
void Mqtt::publish_ha(const std::string & topic, const JsonObject & payload) {
    if (!enabled()) {
        return;
    }

    // empty payload will remove the previous config
    // publish(topic);

    std::string payload_text;
    payload_text.reserve(measureJson(payload) + 1);
    serializeJson(payload, payload_text); // convert json to string

    std::string fulltopic = uuid::read_flash_string(F_(homeassistant)) + topic;
#if defined(EMSESP_STANDALONE)
    LOG_DEBUG(F("Publishing HA topic=%s, payload=%s"), fulltopic.c_str(), payload_text.c_str());
#elif defined(EMSESP_DEBUG)
    LOG_DEBUG(F("[debug] Publishing HA topic=%s, payload=%s"), fulltopic.c_str(), payload_text.c_str());
#endif

    // queue messages if the MQTT connection is not yet established. to ensure we don't miss messages
    queue_publish_message(fulltopic, payload_text, true); // with retain true
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

    if (message->topic.find(uuid::read_flash_string(F_(homeassistant))) == 0) {
        strcpy(topic, message->topic.c_str()); // leave topic as it is
    } else {
        snprintf(topic, MQTT_TOPIC_MAX_SIZE, "%s/%s", mqtt_base_.c_str(), message->topic.c_str());
    }

    // if we're subscribing...
    if (message->operation == Operation::SUBSCRIBE) {
        LOG_DEBUG(F("Subscribing to topic '%s'"), topic);
        uint16_t packet_id = mqttClient_->subscribe(topic, mqtt_qos_);
        if (!packet_id) {
            LOG_ERROR(F("Error subscribing to topic '%s'"), topic);
        }

        mqtt_messages_.pop_front(); // remove the message from the queue

        return;
    }

    // if this has already been published and we're waiting for an ACK, don't publish again
    // it will have a real packet ID
    if (mqtt_message.packet_id_ > 0) {
#if defined(EMSESP_DEBUG)
        LOG_DEBUG(F("[DEBUG] Waiting for QOS-ACK"));
#endif
        return;
    }

    // else try and publish it
    uint16_t packet_id = mqttClient_->publish(topic, mqtt_qos_, message->retain, message->payload.c_str(), message->payload.size(), false, mqtt_message.id_);
    LOG_DEBUG(F("Publishing topic %s (#%02d, retain=%d, retry=%d, size=%d, pid=%d)"),
              topic,
              mqtt_message.id_,
              message->retain,
              mqtt_message.retry_count_ + 1,
              message->payload.size(),
              packet_id);

    if (packet_id == 0) {
        // it failed. if we retried n times, give up. remove from queue
        if (mqtt_message.retry_count_ == (MQTT_PUBLISH_MAX_RETRY - 1)) {
            LOG_ERROR(F("Failed to publish to %s after %d attempts"), topic, mqtt_message.retry_count_ + 1);
            mqtt_publish_fails_++;      // increment failure counter
            mqtt_messages_.pop_front(); // delete
            return;
        } else {
            // update the record
            mqtt_messages_.front().retry_count_++;
            LOG_DEBUG(F("Failed to publish to %s. Trying again, #%d"), topic, mqtt_message.retry_count_ + 1);
            return; // leave on queue for next time so it gets republished
        }
    }

    // if we have ACK set with QOS 1 or 2, leave on queue and let the ACK process remove it
    // but add the packet_id so we can check it later
    if (mqtt_qos_ != 0) {
        mqtt_messages_.front().packet_id_ = packet_id;
#if defined(EMSESP_DEBUG)
        LOG_DEBUG(F("[DEBUG] Setting packetID for ACK to %d"), packet_id);
#endif
        return;
    }

    mqtt_messages_.pop_front(); // remove the message from the queue
}

// HA config for a sensor and binary_sensor entity
// entity must match the key/value pair in the *_data topic
// note: some string copying here into chars, it looks messy but does help with heap fragmentation issues
void Mqtt::publish_ha_sensor(uint8_t                     type, // EMSdevice::DeviceValueType
                             uint8_t                     tag,  // EMSdevice::DeviceValueTAG
                             const __FlashStringHelper * name,
                             const uint8_t               device_type, // EMSdevice::DeviceType
                             const __FlashStringHelper * entity,
                             const uint8_t               uom, // EMSdevice::DeviceValueUOM (0=NONE)
                             const bool                  has_cmd) {
    // ignore if name (fullname) is empty
    if (name == nullptr) {
        return;
    }

    bool have_tag = !EMSdevice::tag_to_string(tag).empty();

    // nested_format is 1 if nested, otherwise 2 for single topics
    bool is_nested = (nested_format_ == 1);

    char device_name[50];
    strlcpy(device_name, EMSdevice::device_type_2_device_name(device_type).c_str(), sizeof(device_name));

    DynamicJsonDocument doc(EMSESP_JSON_SIZE_HA_CONFIG);

    doc["~"] = mqtt_base_;

    // create entity by add the hc/wwc tag if present, seperating with a .
    char new_entity[50];
    if (tag >= DeviceValueTAG::TAG_HC1) {
        snprintf(new_entity, sizeof(new_entity), "%s.%s", EMSdevice::tag_to_string(tag).c_str(), uuid::read_flash_string(entity).c_str());
    } else {
        snprintf(new_entity, sizeof(new_entity), "%s", uuid::read_flash_string(entity).c_str());
    }

    // build unique identifier which will be used in the topic
    // and replacing all . with _ as not to break HA
    std::string uniq(50, '\0');
    snprintf(&uniq[0], uniq.capacity() + 1, "%s_%s", device_name, new_entity);
    std::replace(uniq.begin(), uniq.end(), '.', '_');
    doc["uniq_id"] = uniq;

    // state topic
    char stat_t[MQTT_TOPIC_MAX_SIZE];
    snprintf(stat_t, sizeof(stat_t), "~/%s", tag_to_topic(device_type, tag).c_str());
    doc["stat_t"] = stat_t;

    // name = <device> <tag> <name>
    char new_name[80];
    if (have_tag) {
        snprintf(new_name, sizeof(new_name), "%s %s %s", device_name, EMSdevice::tag_to_string(tag).c_str(), uuid::read_flash_string(name).c_str());
    } else {
        snprintf(new_name, sizeof(new_name), "%s %s", device_name, uuid::read_flash_string(name).c_str());
    }
    new_name[0] = toupper(new_name[0]); // capitalize first letter
    doc["name"] = new_name;

    // value template
    // if its nested mqtt format then use the appended entity name, otherwise take the original
    char val_tpl[50];
    if (is_nested) {
        snprintf(val_tpl, sizeof(val_tpl), "{{value_json.%s}}", new_entity);
    } else {
        snprintf(val_tpl, sizeof(val_tpl), "{{value_json.%s}}", uuid::read_flash_string(entity).c_str());
    }
    doc["val_tpl"] = val_tpl;

    char topic[MQTT_TOPIC_MAX_SIZE];

    // look at the device value type
    if (type == DeviceValueType::BOOL) {
        // binary sensor
        snprintf(topic, sizeof(topic), "binary_sensor/%s/%s/config", mqtt_base_.c_str(), uniq.c_str()); // topic

        // how to render boolean. HA only accepts String values
        char result[10];
        doc[F("payload_on")]  = Helpers::render_boolean(result, true);
        doc[F("payload_off")] = Helpers::render_boolean(result, false);
    } else {
        // normal HA sensor, not a boolean one
        snprintf(topic, sizeof(topic), "sensor/%s/%s/config", mqtt_base_.c_str(), uniq.c_str()); // topic

        // set default state and device class for HA
        auto set_state_class  = State_class::NONE;
        auto set_device_class = Device_class::NONE;

        // unit of measure and map the HA icon
        if (uom != DeviceValueUOM::NONE) {
            doc["unit_of_meas"] = EMSdevice::uom_to_string(uom);
        }

        switch (uom) {
        case DeviceValueUOM::DEGREES:
            doc["ic"]        = F_(icondegrees);
            set_device_class = Device_class::TEMPERATURE;
            break;
        case DeviceValueUOM::PERCENT:
            doc["ic"]        = F_(iconpercent);
            set_device_class = Device_class::POWER_FACTOR;
            break;
        case DeviceValueUOM::SECONDS:
        case DeviceValueUOM::MINUTES:
        case DeviceValueUOM::HOURS:
            doc["ic"] = F_(icontime);
            break;
        case DeviceValueUOM::KB:
            doc["ic"] = F_(iconkb);
            break;
        case DeviceValueUOM::LMIN:
            doc["ic"] = F_(iconlmin);
            break;
        case DeviceValueUOM::WH:
        case DeviceValueUOM::KWH:
            doc["ic"]        = F_(iconkwh);
            set_state_class  = State_class::TOTAL_INCREASING;
            set_device_class = Device_class::ENERGY;
            break;
        case DeviceValueUOM::UA:
            doc["ic"] = F_(iconua);
            break;
        case DeviceValueUOM::BAR:
            doc["ic"]        = F_(iconbar);
            set_device_class = Device_class::PRESSURE;
            break;
        case DeviceValueUOM::W:
        case DeviceValueUOM::KW:
            doc["ic"]        = F_(iconkw);
            set_state_class  = State_class::MEASUREMENT;
            set_device_class = Device_class::POWER;
            break;
        case DeviceValueUOM::DBM:
            doc["ic"]        = F_(icondbm);
            set_device_class = Device_class::SIGNAL_STRENGTH;
            break;
        case DeviceValueUOM::NONE:
            if (type == DeviceValueType::INT || type == DeviceValueType::UINT || type == DeviceValueType::SHORT || type == DeviceValueType::USHORT
                || type == DeviceValueType::ULONG) {
                doc["ic"] = F_(iconnum);
            }
            break;
        case DeviceValueUOM::TIMES:
            set_state_class = State_class::TOTAL_INCREASING;
            doc["ic"]       = F_(iconnum);
            break;
        default:
            break;
        }

        // see if we need to set the state_class and device_class
        // ignore any commands
        if (!has_cmd) {
            // state class
            if (set_state_class == State_class::MEASUREMENT) {
                doc["state_class"] = F("measurement");
            } else if (set_state_class == State_class::TOTAL_INCREASING) {
                doc["state_class"] = F("total_increasing");
            }

            // device class
            switch (set_device_class) {
            case Device_class::ENERGY:
                doc["device_class"] = F("energy");
                break;
            case Device_class::POWER:
                doc["device_class"] = F("power");
                break;
            case Device_class::POWER_FACTOR:
                doc["device_class"] = F("power_factor");
                break;
            case Device_class::PRESSURE:
                doc["device_class"] = F("pressure");
                break;
            case Device_class::SIGNAL_STRENGTH:
                doc["device_class"] = F("signal_strength");
                break;
            case Device_class::TEMPERATURE:
                doc["device_class"] = F("temperature");
                break;
            default:
                break;
            }
        }
    }

    JsonObject dev = doc.createNestedObject("dev");
    JsonArray  ids = dev.createNestedArray("ids");

    // for System commands we'll use the ID EMS-ESP
    if (device_type == EMSdevice::DeviceType::SYSTEM) {
        ids.add("ems-esp");
    } else {
        char ha_device[40];
        snprintf(ha_device, sizeof(ha_device), "ems-esp-%s", device_name);
        ids.add(ha_device);
    }

    publish_ha(topic, doc.as<JsonObject>());
}

// based on the device and tag, create the MQTT topic name (without the basename)
// differs based on whether MQTT nested is enabled
// tag = EMSdevice::DeviceValueTAG
const std::string Mqtt::tag_to_topic(uint8_t device_type, uint8_t tag) {
    // the system device is treated differently. The topic is 'heartbeat' and doesn't follow the usual convention
    if (device_type == EMSdevice::DeviceType::SYSTEM) {
        return EMSdevice::tag_to_mqtt(tag);
    }

    // if there is a tag add it
    if ((EMSdevice::tag_to_mqtt(tag).empty()) || ((nested_format_ == 1) && (device_type != EMSdevice::DeviceType::BOILER))) {
        return EMSdevice::device_type_2_device_name(device_type) + "_data";
    } else {
        return EMSdevice::device_type_2_device_name(device_type) + "_data_" + EMSdevice::tag_to_mqtt(tag);
    }
}

} // namespace emsesp
