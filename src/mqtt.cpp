/*
 * EMS-ESP - https://github.com/proddy/EMS-ESP
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
uint8_t     Mqtt::dallas_format_;
uint8_t     Mqtt::bool_format_;
uint8_t     Mqtt::ha_climate_format_;
bool        Mqtt::ha_enabled_;
bool        Mqtt::nested_format_;

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
void Mqtt::subscribe(const uint8_t device_type, const std::string & topic, mqtt_subfunction_p cb) {
    if (!enabled()) {
        return;
    }

    // check if we already have the topic subscribed, if so don't add it again
    if (!mqtt_subfunctions_.empty()) {
        for (auto & mqtt_subfunction : mqtt_subfunctions_) {
            if ((mqtt_subfunction.device_type_ == device_type) && (strcmp(mqtt_subfunction.topic_.c_str(), topic.c_str()) == 0)) {
                // add the function, in case its not there
                if (cb) {
                    mqtt_subfunction.mqtt_subfunction_ = cb;
                }
                return; // it exists, exit
            }
        }
    }

    LOG_DEBUG(F("Subscribing MQTT topic %s for device type %s"), topic.c_str(), EMSdevice::device_type_2_device_name(device_type).c_str());

    // add to MQTT queue as a subscribe operation
    auto message = queue_subscribe_message(topic);

    if (message == nullptr) {
        return;
    }

    // register in our libary with the callback function.
    // We store both the original topic and the fully-qualified one
    mqtt_subfunctions_.emplace_back(device_type, std::move(topic), std::move(cb));
}

// subscribe to the command topic if it doesn't exist yet
void Mqtt::register_command(const uint8_t device_type, const __FlashStringHelper * cmd, cmdfunction_p cb) {
    std::string cmd_topic = EMSdevice::device_type_2_device_name(device_type);

    bool exists = false;
    if (!mqtt_subfunctions_.empty()) {
        for (const auto & mqtt_subfunction : mqtt_subfunctions_) {
            if ((mqtt_subfunction.device_type_ == device_type) && (strcmp(mqtt_subfunction.topic_.c_str(), cmd_topic.c_str()) == 0)) {
                exists = true;
            }
        }
    }

    if (!exists) {
        Mqtt::subscribe(device_type, cmd_topic, nullptr); // use an empty function handler to signal this is a command function
    }

    LOG_DEBUG(F("Registering MQTT cmd %s with topic %s"), uuid::read_flash_string(cmd).c_str(), EMSdevice::device_type_2_device_name(device_type).c_str());
}

// subscribe to an MQTT topic, and store the associated callback function. For generic functions not tied to a specific device
void Mqtt::subscribe(const std::string & topic, mqtt_subfunction_p cb) {
    subscribe(0, topic, cb); // no device_id needed, if generic to EMS-ESP
}

// resubscribe to all MQTT topics
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
    // exit if MQTT is not enabled or if there is no WIFI
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
        if (content->operation == Operation::PUBLISH) {
            // Publish messages
            if (message.retry_count_ == 0) {
                if (message.packet_id_ == 0) {
                    shell.printfln(F(" [%02d] (Pub) topic=%s payload=%s"), message.id_, content->topic.c_str(), content->payload.c_str());
                } else {
                    shell.printfln(F(" [%02d] (Pub) topic=%s payload=%s (pid %d)"), message.id_, content->topic.c_str(), content->payload.c_str(), message.packet_id_);
                }
            } else {
                shell.printfln(F(" [%02d] (Pub) topic=%s payload=%s (pid %d, retry #%d)"), message.id_, content->topic.c_str(), content->payload.c_str(), message.packet_id_, message.retry_count_);
            }
        } else {
            // Subscribe messages
            shell.printfln(F(" [%02d] (Sub) topic=%s"), message.id_, content->topic.c_str());
        }
    }
    shell.println();
}

// simulate receiving a MQTT message, used only for testing
void Mqtt::incoming(const char * topic, const char * payload) {
    on_message(topic, payload, strlen(payload));
}

// received an MQTT message that we subscribed too
void Mqtt::on_message(const char * fulltopic, const char * payload, size_t len) {
    if (len == 0) {
        LOG_DEBUG(F("Received empty message %s"), fulltopic);
        return; // ignore empty payloads
    }
    if (strncmp(fulltopic, mqtt_base_.c_str(), strlen(mqtt_base_.c_str())) != 0) {
        LOG_DEBUG(F("Received unknown message %s - %s"), fulltopic, payload);
        return; // not for us
    }
    char topic[100];
    strlcpy(topic, &fulltopic[1 + strlen(mqtt_base_.c_str())], 100);

    // convert payload to a null-terminated char string
    char message[len + 2];
    strlcpy(message, payload, len + 1);

    LOG_DEBUG(F("Received %s => %s (length %d)"), topic, message, len);

    // see if we have this topic in our subscription list, then call its callback handler
    for (const auto & mf : mqtt_subfunctions_) {
        if (strcmp(topic, mf.topic_.c_str()) == 0) {
            // if we have call back function then call it
            // otherwise proceed as process as a command
            if (mf.mqtt_subfunction_) {
                if (!(mf.mqtt_subfunction_)(message)) {
                    LOG_ERROR(F("MQTT error: invalid payload %s for this topic %s"), message, topic);
                }
                return;
            }

            // It's a command then with the payload being JSON like {"cmd":"<cmd>", "data":<data>, "id":<n>}
            // Find the command from the json and call it directly
            StaticJsonDocument<EMSESP_JSON_SIZE_SMALL> doc;
            DeserializationError                       error = deserializeJson(doc, message);
            if (error) {
                LOG_ERROR(F("MQTT error: payload %s, error %s"), message, error.c_str());
                return;
            }

            const char * command = doc["cmd"];
            if (command == nullptr) {
                LOG_ERROR(F("MQTT error: invalid payload cmd format. message=%s"), message);
                return;
            }

            // check for hc and id, and convert to int
            int8_t n = -1; // no value
            if (doc.containsKey("hc")) {
                n = doc["hc"];
            } else if (doc.containsKey("id")) {
                n = doc["id"];
            }

            bool        cmd_known = false;
            JsonVariant data      = doc["data"];

            if (data.is<char *>()) {
                cmd_known = Command::call(mf.device_type_, command, data.as<char *>(), n);
            } else if (data.is<int>()) {
                char data_str[10];
                cmd_known = Command::call(mf.device_type_, command, Helpers::itoa(data_str, (int16_t)data.as<int>()), n);
            } else if (data.is<float>()) {
                char data_str[10];
                cmd_known = Command::call(mf.device_type_, command, Helpers::render_value(data_str, (float)data.as<float>(), 2), n);
            } else if (data.isNull()) {
                cmd_known = Command::call(mf.device_type_, command, "", n);
            }

            if (!cmd_known) {
                LOG_ERROR(F("No matching cmd (%s), invalid data or command failed"), command);
            }

            return;
        }
    }

    // if we got here we didn't find a topic match
    LOG_ERROR(F("No MQTT handler found for topic %s and payload %s"), topic, message);
}

// print all the topics related to a specific device type
void Mqtt::show_topic_handlers(uuid::console::Shell & shell, const uint8_t device_type) {
    if (std::count_if(mqtt_subfunctions_.cbegin(), mqtt_subfunctions_.cend(), [=](MQTTSubFunction const & mqtt_subfunction) { return device_type == mqtt_subfunction.device_type_; }) == 0) {
        return;
    }

    shell.print(F(" Subscribed MQTT topics: "));
    for (const auto & mqtt_subfunction : mqtt_subfunctions_) {
        if (mqtt_subfunction.device_type_ == device_type) {
            shell.printf(F("%s/%s "), mqtt_base_.c_str(), mqtt_subfunction.topic_.c_str());
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

void Mqtt::start() {
    mqttClient_ = EMSESP::esp8266React.getMqttClient();

    // fetch MQTT settings, to see if MQTT is enabled
    EMSESP::esp8266React.getMqttSettingsService()->read([&](MqttSettings & mqttSettings) {
        mqtt_enabled_ = mqttSettings.enabled;
        mqtt_base_    = mqttSettings.base.c_str(); // Convert String to std::string
        if (!mqtt_enabled_) {
            return; // quit, not using MQTT
        }
    });

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
    snprintf_P(will_topic, MQTT_TOPIC_MAX_SIZE, PSTR("%s/status"), mqtt_base_.c_str());
    mqttClient_->setWill(will_topic, 1, true, "offline"); // with qos 1, retain true

    mqttClient_->onMessage([this](char * topic, char * payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
        // receiving mqtt
        on_message(topic, payload, len);
    });

    mqttClient_->onPublish([this](uint16_t packetId) {
        // publish
        on_publish(packetId);
    });

    // create space for command buffer, to avoid heap memory fragmentation
    mqtt_subfunctions_.reserve(50);
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
    if (connecting_) { // prevent duplicating connections
        return;
    }

    LOG_INFO(F("MQTT connected"));

    connecting_ = true;
    connectcount_++;

    // fetch MQTT settings
    EMSESP::esp8266React.getMqttSettingsService()->read([&](MqttSettings & mqttSettings) {
        publish_time_boiler_     = mqttSettings.publish_time_boiler * 1000; // convert to milliseconds
        publish_time_thermostat_ = mqttSettings.publish_time_thermostat * 1000;
        publish_time_solar_      = mqttSettings.publish_time_solar * 1000;
        publish_time_mixer_      = mqttSettings.publish_time_mixer * 1000;
        publish_time_other_      = mqttSettings.publish_time_other * 1000;
        publish_time_sensor_     = mqttSettings.publish_time_sensor * 1000;
        mqtt_qos_                = mqttSettings.mqtt_qos;
        mqtt_retain_             = mqttSettings.mqtt_retain;
        mqtt_enabled_            = mqttSettings.enabled;
        ha_enabled_              = mqttSettings.ha_enabled;
        ha_climate_format_       = mqttSettings.ha_climate_format;
        dallas_format_           = mqttSettings.dallas_format;
        bool_format_             = mqttSettings.bool_format;
        nested_format_           = mqttSettings.nested_format;
    });

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
    doc["ip"] = WiFi.localIP().toString();
#endif
    publish(F_(info), doc.as<JsonObject>());

    // create the EMS-ESP device in HA, which is MQTT retained
    if (ha_enabled()) {
        ha_status();
    }

    // send initial MQTT messages for some of our services
    EMSESP::shower_.send_mqtt_stat(false); // Send shower_activated as false
    EMSESP::system_.send_heartbeat();      // send heatbeat

    // } else {
    if (connectcount_ > 1) {
        // we doing a re-connect from a TCP break
        // only re-subscribe again to all MQTT topics
        resubscribe();
        EMSESP::reset_mqtt_ha(); // re-create all HA devices if there are any
    }

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
    doc["val_tpl"] = FJSON("{{value_json['status']}}");

    JsonObject dev = doc.createNestedObject("dev");
    dev["name"]    = F_(EMSESP); // "EMS-ESP"
    dev["sw"]      = EMSESP_APP_VERSION;
    dev["mf"]      = FJSON("proddy");
    dev["mdl"]     = F_(EMSESP); // "EMS-ESP"
    JsonArray ids  = dev.createNestedArray("ids");
    ids.add("ems-esp");

    char topic[MQTT_TOPIC_MAX_SIZE];
    snprintf_P(topic, sizeof(topic), PSTR("homeassistant/sensor/%s/system/config"), mqtt_base_.c_str());
    Mqtt::publish_ha(topic, doc.as<JsonObject>()); // publish the config payload with retain flag

    // create the sensors
    publish_mqtt_ha_sensor(DeviceValueType::INT, DeviceValueTAG::TAG_NONE, F("Wifi strength"), EMSdevice::DeviceType::SYSTEM, F("rssi"));
    publish_mqtt_ha_sensor(DeviceValueType::INT, DeviceValueTAG::TAG_NONE, F("Uptime"), EMSdevice::DeviceType::SYSTEM, F("uptime"));
    publish_mqtt_ha_sensor(DeviceValueType::INT, DeviceValueTAG::TAG_NONE, F("Uptime (sec)"), EMSdevice::DeviceType::SYSTEM, F("uptime_sec"));
    publish_mqtt_ha_sensor(DeviceValueType::INT, DeviceValueTAG::TAG_NONE, F("Free heap memory"), EMSdevice::DeviceType::SYSTEM, F("freemem"));
    publish_mqtt_ha_sensor(DeviceValueType::INT, DeviceValueTAG::TAG_NONE, F("# Failed MQTT publishes"), EMSdevice::DeviceType::SYSTEM, F("mqttfails"));
    publish_mqtt_ha_sensor(DeviceValueType::INT, DeviceValueTAG::TAG_NONE, F("# Rx Sent"), EMSdevice::DeviceType::SYSTEM, F("rxsent"));
    publish_mqtt_ha_sensor(DeviceValueType::INT, DeviceValueTAG::TAG_NONE, F("# Rx Fails"), EMSdevice::DeviceType::SYSTEM, F("rxfails"));
    publish_mqtt_ha_sensor(DeviceValueType::INT, DeviceValueTAG::TAG_NONE, F("# Tx Reads"), EMSdevice::DeviceType::SYSTEM, F("txread"));
    publish_mqtt_ha_sensor(DeviceValueType::INT, DeviceValueTAG::TAG_NONE, F("# Tx Writes"), EMSdevice::DeviceType::SYSTEM, F("txwrite"));
    publish_mqtt_ha_sensor(DeviceValueType::INT, DeviceValueTAG::TAG_NONE, F("# Tx Fails"), EMSdevice::DeviceType::SYSTEM, F("txfails"));
}

// add sub or pub task to the queue.
// a fully-qualified topic is created by prefixing the base, unless it's HA
// returns a pointer to the message created
std::shared_ptr<const MqttMessage> Mqtt::queue_message(const uint8_t operation, const std::string & topic, const std::string & payload, bool retain) {
    if (topic.empty()) {
        return nullptr;
    }

    // take the topic and prefix the base, unless its for HA
    std::shared_ptr<MqttMessage> message;
    message = std::make_shared<MqttMessage>(operation, topic, payload, retain);

    // LOG_INFO("Added to queue: %s %s", message->topic.c_str(), message->payload.c_str()); // debugging only

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

#if defined(EMSESP_STANDALONE)
    LOG_DEBUG(F("Publishing HA topic=%s, payload=%s"), topic.c_str(), payload_text.c_str());
#elif defined(EMSESP_DEBUG)
    LOG_DEBUG(F("[debug] Publishing HA topic=%s, payload=%s"), topic.c_str(), payload_text.c_str());
#endif

    // queue messages if the MQTT connection is not yet established. to ensure we don't miss messages
    queue_publish_message(topic, payload_text, true); // with retain true
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
    if ((strncmp(message->topic.c_str(), "homeassistant/", 13) == 0)) {
        // leave topic as it is
        strcpy(topic, message->topic.c_str());
    } else {
        snprintf_P(topic, MQTT_TOPIC_MAX_SIZE, PSTR("%s/%s"), mqtt_base_.c_str(), message->topic.c_str());
    }

    // if we're subscribing...
    if (message->operation == Operation::SUBSCRIBE) {
        LOG_DEBUG(F("Subscribing to topic: %s"), topic);
        uint16_t packet_id = mqttClient_->subscribe(topic, mqtt_qos_);
        if (!packet_id) {
            LOG_DEBUG(F("Error subscribing to %s"), topic);
        }

        mqtt_messages_.pop_front(); // remove the message from the queue

        return;
    }

    // if this has already been published and we're waiting for an ACK, don't publish again
    // it will have a real packet ID
    if (mqtt_message.packet_id_ > 0) {
#if defined(EMSESP_DEBUG)
        LOG_DEBUG(F("[DEBUG] Waitig for QOS-ACK"));
#endif
        return;
    }

    // else try and publish it
    uint16_t packet_id = mqttClient_->publish(topic, mqtt_qos_, message->retain, message->payload.c_str(), message->payload.size(), false, mqtt_message.id_);
    LOG_DEBUG(F("Publishing topic %s (#%02d, retain=%d, try#%d, size %d, pid %d)"), topic, mqtt_message.id_, message->retain, mqtt_message.retry_count_ + 1, message->payload.size(), packet_id);

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
// e.g. homeassistant/sensor/ems-esp32/thermostat_hc1_seltemp/config
// with:
// {"uniq_id":"thermostat_hc1_seltemp","stat_t":"ems-esp32/thermostat_data","name":"Thermostat hc1 Setpoint room temperature",
//  "val_tpl":"{{value_json.hc1.seltemp}}","unit_of_meas":"°C","ic":"mdi:temperature-celsius","dev":{"ids":["ems-esp-thermostat"]}}
//
// note: some string copying here into chars, it looks messy but does help with heap fragmentation issues
void Mqtt::publish_mqtt_ha_sensor(uint8_t                     type, // EMSdevice::DeviceValueType
                                  uint8_t                     tag,
                                  const __FlashStringHelper * name,
                                  const uint8_t               device_type, // EMSdevice::DeviceType
                                  const __FlashStringHelper * entity,
                                  const uint8_t               uom) { // DeviceValueUOM (0=NONE)
    // ignore if name (fullname) is empty
    if (name == nullptr) {
        return;
    }

    DynamicJsonDocument doc(EMSESP_JSON_SIZE_HA_CONFIG);

    // special case for boiler - don't use the prefix
    bool have_prefix = ((tag != DeviceValueTAG::TAG_NONE) && (device_type != EMSdevice::DeviceType::BOILER));

    // create entity by inserting any given prefix
    // we ignore the tag if BOILER
    char new_entity[50];
    if (have_prefix) {
        snprintf_P(new_entity, sizeof(new_entity), PSTR("%s.%s"), EMSdevice::tag_to_string(tag).c_str(), uuid::read_flash_string(entity).c_str());
    } else {
        snprintf_P(new_entity, sizeof(new_entity), PSTR("%s"), uuid::read_flash_string(entity).c_str());
    }

    // device name
    char device_name[50];
    strncpy(device_name, EMSdevice::device_type_2_device_name(device_type).c_str(), sizeof(device_name));

    // build unique identifier which will be used in the topic
    // and replacing all . with _ as not to break HA
    std::string uniq(50, '\0');
    snprintf_P(&uniq[0], uniq.capacity() + 1, PSTR("%s_%s"), device_name, new_entity);
    std::replace(uniq.begin(), uniq.end(), '.', '_');
    doc["uniq_id"] = uniq;

    doc["~"] = mqtt_base_;

    // state topic
    // if its a boiler we use the tag
    char stat_t[MQTT_TOPIC_MAX_SIZE];
    if (device_type == EMSdevice::DeviceType::BOILER) {
        snprintf_P(stat_t, sizeof(stat_t), PSTR("~/%s"), EMSdevice::tag_to_string(tag).c_str());
    } else if (device_type == EMSdevice::DeviceType::SYSTEM) {
        snprintf_P(stat_t, sizeof(stat_t), PSTR("~/heartbeat"));
    } else {
        snprintf_P(stat_t, sizeof(stat_t), PSTR("~/%s_data"), device_name);
    }
    doc["stat_t"] = stat_t;

    // name
    char new_name[50];
    if (have_prefix) {
        snprintf_P(new_name, sizeof(new_name), PSTR("%s %s %s"), device_name, EMSdevice::tag_to_string(tag).c_str(), uuid::read_flash_string(name).c_str());
    } else {
        snprintf_P(new_name, sizeof(new_name), PSTR("%s %s"), device_name, uuid::read_flash_string(name).c_str());
    }
    new_name[0] = toupper(new_name[0]); // capitalize first letter
    doc["name"] = new_name;

    // value template
    char val_tpl[50];
    snprintf_P(val_tpl, sizeof(val_tpl), PSTR("{{value_json.%s}}"), new_entity);
    doc["val_tpl"] = val_tpl;

    char topic[MQTT_TOPIC_MAX_SIZE]; // reserved for topic

    // look at the device value type
    if (type == DeviceValueType::BOOL) {
        // binary sensor
        snprintf_P(topic, sizeof(topic), PSTR("homeassistant/binary_sensor/%s/%s/config"), mqtt_base_.c_str(), uniq.c_str()); // topic

        // how to render boolean
        // HA only accepts String values
        char result[10];
        doc[F("payload_on")]  = Helpers::render_boolean(result, true);
        doc[F("payload_off")] = Helpers::render_boolean(result, false);
    } else {
        // normal HA sensor, not a boolean one
        snprintf_P(topic, sizeof(topic), PSTR("homeassistant/sensor/%s/%s/config"), mqtt_base_.c_str(), uniq.c_str()); // topic

        // unit of measure and map the HA icon
        if (uom != DeviceValueUOM::NONE && uom != DeviceValueUOM::PUMP) {
            doc["unit_of_meas"] = EMSdevice::uom_to_string(uom);
        }
        switch (uom) {
        case DeviceValueUOM::DEGREES:
            doc["ic"] = F_(icontemperature);
            break;
        case DeviceValueUOM::PERCENT:
            doc["ic"] = F_(iconpercent);
            break;
        case DeviceValueUOM::PUMP:
            doc["ic"] = F_(iconpump);
            break;
        case DeviceValueUOM::NONE:
        default:
            break;
        }
    }

    JsonObject dev = doc.createNestedObject("dev");

    JsonArray ids = dev.createNestedArray("ids");

    // for System commands we'll use the ID EMS-ESP
    if (device_type == EMSdevice::DeviceType::SYSTEM) {
        ids.add("ems-esp");
    } else {
        char ha_device[40];
        snprintf_P(ha_device, sizeof(ha_device), PSTR("ems-esp-%s"), device_name);
        ids.add(ha_device);
    }

    publish_ha(topic, doc.as<JsonObject>());
}

} // namespace emsesp
