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
std::string Mqtt::hostname_;
uint8_t     Mqtt::mqtt_qos_;
bool        Mqtt::mqtt_retain_;
uint32_t    Mqtt::publish_time_boiler_;
uint32_t    Mqtt::publish_time_thermostat_;
uint32_t    Mqtt::publish_time_solar_;
uint32_t    Mqtt::publish_time_mixer_;
uint32_t    Mqtt::publish_time_other_;
uint32_t    Mqtt::publish_time_sensor_;
uint8_t     Mqtt::mqtt_format_;
bool        Mqtt::mqtt_enabled_;

std::vector<Mqtt::MQTTSubFunction> Mqtt::mqtt_subfunctions_;

uint16_t                           Mqtt::mqtt_publish_fails_    = 0;
size_t                             Mqtt::maximum_mqtt_messages_ = Mqtt::MAX_MQTT_MESSAGES;
uint16_t                           Mqtt::mqtt_message_id_       = 0;
std::list<Mqtt::QueuedMqttMessage> Mqtt::mqtt_messages_;
char                               will_topic_[Mqtt::MQTT_TOPIC_MAX_SIZE]; // because MQTT library keeps only char pointer

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

    // register in our libary with the callback function.
    // We store both the original topic and the fully-qualified one
    mqtt_subfunctions_.emplace_back(device_type, std::move(topic), std::move(message->topic), std::move(cb));
}

// subscribe to the command topic if it doesn't exist yet
void Mqtt::register_command(const uint8_t device_type, const uint8_t device_id, const __FlashStringHelper * cmd, cmdfunction_p cb) {
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

// resubscribe to all MQTT topics again
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

    // create publish messages for each of the EMS device values, adding to queue
    if (publish_time_boiler_ && (currentMillis - last_publish_boiler_ > publish_time_boiler_)) {
        last_publish_boiler_ = currentMillis;
        EMSESP::publish_device_values(EMSdevice::DeviceType::BOILER);
    }

    if (publish_time_thermostat_ && (currentMillis - last_publish_thermostat_ > publish_time_thermostat_)) {
        last_publish_thermostat_ = currentMillis;
        EMSESP::publish_device_values(EMSdevice::DeviceType::THERMOSTAT);
    }

    if (publish_time_solar_ && (currentMillis - last_publish_solar_ > publish_time_solar_)) {
        last_publish_solar_ = currentMillis;
        EMSESP::publish_device_values(EMSdevice::DeviceType::SOLAR);
    }

    if (publish_time_mixer_ && (currentMillis - last_publish_mixer_ > publish_time_mixer_)) {
        last_publish_mixer_ = currentMillis;
        EMSESP::publish_device_values(EMSdevice::DeviceType::MIXER);
    }

    if (publish_time_other_ && (currentMillis - last_publish_other_ > publish_time_other_)) {
        last_publish_other_ = currentMillis;
        EMSESP::publish_other_values();
    }

    if (currentMillis - last_publish_sensor_ > publish_time_sensor_) {
        last_publish_sensor_ = currentMillis;
        EMSESP::publish_sensor_values(publish_time_sensor_ != 0);
    }

    // publish top item from MQTT queue to stop flooding
    if ((uint32_t)(currentMillis - last_mqtt_poll_) > MQTT_PUBLISH_WAIT) {
        last_mqtt_poll_ = currentMillis;
        process_queue();
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
        shell.printfln(F(" %s"), mqtt_subfunction.full_topic_.c_str());
    }
    shell.println();

    // show queues
    if (mqtt_messages_.empty()) {
        shell.printfln(F("MQTT queue is empty"));
        shell.println();
        return;
    }

    shell.printfln(F("MQTT queue (%d messages):"), mqtt_messages_.size());

    for (const auto & message : mqtt_messages_) {
        auto content = message.content_;
        if (content->operation == Operation::PUBLISH) {
            // Publish messages
            if (message.retry_count_ == 0) {
                if (message.packet_id_ == 0) {
                    shell.printfln(F(" [%02d] (Pub) topic=%s payload=%s"), message.id_, content->topic.c_str(), content->payload.c_str());
                } else {
                    shell.printfln(F(" [%02d] (Pub) topic=%s payload=%s (pid %d)"),
                                   message.id_,
                                   content->topic.c_str(),
                                   content->payload.c_str(),
                                   message.packet_id_);
                }
            } else {
                shell.printfln(F(" [%02d] (Pub) topic=%s payload=%s (pid %d, retry #%d)"),
                               message.id_,
                               content->topic.c_str(),
                               content->payload.c_str(),
                               message.packet_id_,
                               message.retry_count_);
            }
        } else {
            // Subscribe messages
            shell.printfln(F(" [%02d] (Sub) topic=%s"), message.id_, content->topic.c_str());
        }
    }
    shell.println();
} // namespace emsesp

#if defined(EMSESP_DEBUG)
// simulate receiving a MQTT message, used only for testing
void Mqtt::incoming(const char * topic, const char * payload) {
    on_message(topic, payload, strlen(payload));
}
#endif

// received an MQTT message that we subscribed too
void Mqtt::on_message(const char * topic, const char * payload, size_t len) {
    if (len == 0) {
        return; // ignore empty payloads
    }

    // convert payload to a null-terminated char string
    char message[len + 2];
    strlcpy(message, payload, len + 1);

#ifdef EMSESP_DEBUG
    LOG_DEBUG(F("[DEBUG] Received %s => %s (length %d)"), topic, message, len);
#endif

    // see if we have this topic in our subscription list, then call its callback handler
    for (const auto & mf : mqtt_subfunctions_) {
        if (strcmp(topic, mf.full_topic_.c_str()) == 0) {
            if (mf.mqtt_subfunction_) {
                // matching function, call it. If it returns true keep quit
                if ((mf.mqtt_subfunction_)(message)) {
                    return; // function executed successfully
                }
            }

            // empty function. It's a command then. Find the command from the json and call it directly.
            StaticJsonDocument<EMSESP_MAX_JSON_SIZE_SMALL> doc;
            DeserializationError                           error = deserializeJson(doc, message);
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

            JsonObject json; // empty object

            if (data.is<char *>()) {
                cmd_known = Command::call(mf.device_type_, command, data.as<char *>(), n, json);
            } else if (data.is<int>()) {
                char data_str[10];
                cmd_known = Command::call(mf.device_type_, command, Helpers::itoa(data_str, (int16_t)data.as<int>()), n, json);
            } else if (data.is<float>()) {
                char data_str[10];
                cmd_known = Command::call(mf.device_type_, command, Helpers::render_value(data_str, (float)data.as<float>(), 2), n, json);
            } else if (data.isNull()) {
                cmd_known = Command::call(mf.device_type_, command, "", n, json);
            }

            if (!cmd_known) {
                LOG_ERROR(F("MQTT: no matching cmd (%s), invalid data or command failed"), command);
            }

            return;
        }
    }

    // if we got here we didn't find a topic match
    LOG_ERROR(F("No MQTT handler found for topic %s and payload %s"), topic, message);
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
        return;
    }

    auto mqtt_message = mqtt_messages_.front();

    // if the last published failed, don't bother checking it. wait for the next retry
    if (mqtt_message.packet_id_ == 0) {
        return;
    }

    if (mqtt_message.packet_id_ != packetId) {
        LOG_ERROR(F("Mismatch, expecting PID %d, got %d"), mqtt_message.packet_id_, packetId);
        mqtt_publish_fails_++; // increment error count
    }

    mqtt_messages_.pop_front(); // always remove from queue, regardless if there was a successful ACK
}

void Mqtt::start() {
    mqttClient_ = EMSESP::esp8266React.getMqttClient();

    // get the hostname, which we'll use to prefix to all topics
    EMSESP::esp8266React.getWiFiSettingsService()->read([&](WiFiSettings & wifiSettings) { hostname_ = wifiSettings.hostname.c_str(); });

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
        mqtt_format_             = mqttSettings.mqtt_format;
        mqtt_enabled_            = mqttSettings.enabled;
    });

    mqttClient_->onConnect([this](bool sessionPresent) { on_connect(); });

    mqttClient_->onDisconnect([this](AsyncMqttClientDisconnectReason reason) {
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
    });

    // create will_topic with the hostname prefixed. It has to be static because asyncmqttclient destroys the reference
    static char will_topic[MQTT_TOPIC_MAX_SIZE];
    strlcpy(will_topic, hostname_.c_str(), MQTT_TOPIC_MAX_SIZE);
    strlcat(will_topic, "/", MQTT_TOPIC_MAX_SIZE);
    strlcat(will_topic, "status", MQTT_TOPIC_MAX_SIZE);
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
    mqtt_subfunctions_.reserve(10);
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

void Mqtt::set_qos(uint8_t mqtt_qos) {
    mqtt_qos_ = mqtt_qos;
}

void Mqtt::set_retain(bool mqtt_retain) {
    mqtt_retain_ = mqtt_retain;
}

void Mqtt::set_format(uint8_t mqtt_format) {
    mqtt_format_ = mqtt_format;
}

// MQTT onConnect - when a connect is established
void Mqtt::on_connect() {
    // send info topic appended with the version information as JSON
    StaticJsonDocument<90> doc;
    doc["event"]   = "start";
    doc["version"] = EMSESP_APP_VERSION;
#ifndef EMSESP_STANDALONE
    doc["ip"] = WiFi.localIP().toString();
#endif
    publish(F_(info), doc.as<JsonObject>());

    publish_retain(F("status"), "online", true); // say we're alive to the Last Will topic, with retain on

    reset_publish_fails(); // reset fail count to 0

    resubscribe(); // in case this is a reconnect, re-subscribe again to all MQTT topics

    if (mqtt_format() == Format::HA) {
        ha_status(); // create a device in HA
    }

    LOG_INFO(F("MQTT connected"));
}

// Home Assistant Discovery - the main master Device
// homeassistant/sensor/ems-esp/status/config
// all the values from the heartbeat payload will be added as attributes to the entity state
void Mqtt::ha_status() {
    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_MEDIUM> doc;

    doc["name"]        = F("EMS-ESP status");
    doc["uniq_id"]     = F("status");
    doc["~"]           = System::hostname(); // ems-esp
    doc["avty_t"]      = F("~/status");
    doc["json_attr_t"] = F("~/heartbeat");
    doc["stat_t"]      = F("~/heartbeat");
    doc["val_tpl"]     = F("{{value_json['status']}}");
    doc["ic"]          = F("mdi:home-thermometer-outline");

    JsonObject dev = doc.createNestedObject("dev");
    dev["name"]    = F("EMS-ESP");
    dev["sw"]      = EMSESP_APP_VERSION;
    dev["mf"]      = F("proddy");
    dev["mdl"]     = F("EMS-ESP");
    JsonArray ids  = dev.createNestedArray("ids");
    ids.add("ems-esp");

    Mqtt::publish_retain(F("homeassistant/sensor/ems-esp/status/config"), doc.as<JsonObject>(), true); // publish the config payload with retain flag
}

// add sub or pub task to the queue.
// a fully-qualified topic is created by prefixing the hostname, unless it's HA
// returns a pointer to the message created
std::shared_ptr<const MqttMessage> Mqtt::queue_message(const uint8_t operation, const std::string & topic, const std::string & payload, bool retain) {
    if (topic.empty()) {
        return nullptr;
    }

    // take the topic and prefix the hostname, unless its for HA
    std::shared_ptr<MqttMessage> message;
    if ((strncmp(topic.c_str(), "homeassistant/", 13) == 0)) {
        // leave topic as it is
        message = std::make_shared<MqttMessage>(operation, topic, payload, retain);
    } else {
        // prefix the hostname
        std::string full_topic(100, '\0');
        snprintf_P(&full_topic[0], full_topic.capacity() + 1, PSTR("%s/%s"), hostname_.c_str(), topic.c_str());
        message = std::make_shared<MqttMessage>(operation, full_topic, payload, retain);
    }

    // if the queue is full, make room but removing the last one
    if (mqtt_messages_.size() >= maximum_mqtt_messages_) {
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
    /*
    // for HA, empty payload will remove the previous config
    if (retain == true) {
        publish(topic);
    }
    */

    if (enabled() && payload.size()) {
        std::string payload_text;
        serializeJson(payload, payload_text); // convert json to string
        queue_publish_message(topic, payload_text, retain);
    }
}

void Mqtt::publish_retain(const __FlashStringHelper * topic, const JsonObject & payload, bool retain) {
    publish_retain(uuid::read_flash_string(topic), payload, retain);
}

// take top from queue and perform the publish or subscribe action
// assumes there is an MQTT connection
void Mqtt::process_queue() {
    if (mqtt_messages_.empty()) {
        return;
    }

    // show queue - Debug only
    /*
    Serial.printf("MQTT queue:\n\r");
    for (const auto & message : mqtt_messages_) {
        auto content = message.content_;
        if (content->operation == Operation::PUBLISH) {
            // Publish messages
            Serial.printf(" [%02d] (Pub) topic=%s payload=%s (pid %d, retry #%d)\n\r",
                          message.id_,
                          content->topic.c_str(),
                          content->payload.c_str(),
                          message.packet_id_,
                          message.retry_count_);
        } else {
            // Subscribe messages
            Serial.printf(" [%02d] (Sub) topic=%s\n\r", message.id_, content->topic.c_str());
        }
    }
    */

    // fetch first from queue and create the full topic name
    auto mqtt_message = mqtt_messages_.front();
    auto message      = mqtt_message.content_;

    // if we're subscribing...
    if (message->operation == Operation::SUBSCRIBE) {
        LOG_DEBUG(F("Subscribing to topic: %s"), message->topic.c_str());
        uint16_t packet_id = mqttClient_->subscribe(message->topic.c_str(), mqtt_qos_);
        if (!packet_id) {
            LOG_DEBUG(F("Error subscribing to %s"), message->topic.c_str());
        }

        mqtt_messages_.pop_front(); // remove the message from the queue

        return;
    }

    // if this has already been published and we're waiting for an ACK, don't publish again
    // it will have a real packet ID
    if (mqtt_message.packet_id_ > 0) {
        return;
    }

    // else try and publish it
    uint16_t packet_id =
        mqttClient_->publish(message->topic.c_str(), mqtt_qos_, message->retain, message->payload.c_str(), message->payload.size(), false, mqtt_message.id_);
    LOG_DEBUG(F("Publishing topic %s (#%02d, attempt #%d, size %d, pid %d)"),
              message->topic.c_str(),
              mqtt_message.id_,
              mqtt_message.retry_count_ + 1,
              message->payload.size(),
              packet_id);

    if (packet_id == 0) {
        // it failed. if we retried n times, give up. remove from queue
        if (mqtt_message.retry_count_ == (MQTT_PUBLISH_MAX_RETRY - 1)) {
            LOG_ERROR(F("Failed to publish to %s after %d attempts"), message->topic.c_str(), mqtt_message.retry_count_ + 1);
            mqtt_publish_fails_++;      // increment failure counter
            mqtt_messages_.pop_front(); // delete
            return;
        } else {
            mqtt_messages_.front().retry_count_++;
            LOG_DEBUG(F("Failed to publish to %s. Trying again, #%d"), message->topic.c_str(), mqtt_message.retry_count_ + 1);
            return; // leave on queue for next time so it gets republished
        }
    }

    // if we have ACK set with QOS 1 or 2, leave on queue and let the ACK process remove it
    // but add the packet_id so we can check it later
    if (mqtt_qos_ != 0) {
        mqtt_messages_.front().packet_id_ = packet_id;
#if defined(EMSESP_DEBUG)
        LOG_DEBUG(F("Setting packetID for ACK to %d"), packet_id);
#endif
        return;
    }

    mqtt_messages_.pop_front(); // remove the message from the queue
}

// HA config for a binary_sensor
void Mqtt::register_mqtt_ha_binary_sensor(const __FlashStringHelper * name, const uint8_t device_type, const char * entity) {
    if (mqtt_format() != Format::HA) {
        return;
    }

    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_MEDIUM> doc;

    doc["name"]    = name;
    doc["uniq_id"] = entity;

    char state_t[50];
    snprintf_P(state_t, sizeof(state_t), PSTR("%s/%s"), hostname_.c_str(), entity);
    doc["stat_t"] = state_t;

    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        if (settings.bool_format == BOOL_FORMAT_ONOFF) {
            doc[F("payload_on")]  = F("on");
            doc[F("payload_off")] = F("off");
        } else if (settings.bool_format == BOOL_FORMAT_TRUEFALSE) {
            doc[F("payload_on")]  = F("true");
            doc[F("payload_off")] = F("false");
        } else {
            doc[F("payload_on")]  = "1";
            doc[F("payload_off")] = "0";
        }
    });

    JsonObject dev = doc.createNestedObject("dev");
    JsonArray  ids = dev.createNestedArray("ids");
    char       ha_device[40];
    snprintf_P(ha_device, sizeof(ha_device), PSTR("ems-esp-%s"), EMSdevice::device_type_2_device_name(device_type).c_str());
    ids.add(ha_device);

    char topic[MQTT_TOPIC_MAX_SIZE];
    snprintf_P(topic, sizeof(topic), PSTR("homeassistant/binary_sensor/ems-esp/%s/config"), entity);

    // convert json to string and publish immediately with retain forced to true
    std::string payload_text;
    serializeJson(doc, payload_text); // convert json to string
    uint16_t packet_id = mqttClient_->publish(topic, 0, true, payload_text.c_str(), payload_text.size());
#if defined(EMSESP_STANDALONE)
    LOG_DEBUG(F("Publishing topic %s"), topic);
#else
    if (!packet_id) {
        LOG_ERROR(F("Failed to publish topic %s"), topic);
    } else {
        LOG_DEBUG(F("Publishing topic %s"), topic);
    }
#endif

    delay(MQTT_PUBLISH_WAIT);
}

// HA config for a normal 'sensor' type
// entity must match the key/value pair in the _data topic
void Mqtt::register_mqtt_ha_sensor(const char *                prefix,
                                   const __FlashStringHelper * suffix,
                                   const __FlashStringHelper * name,
                                   const uint8_t               device_type,
                                   const char *                entity,
                                   const __FlashStringHelper * uom,
                                   const __FlashStringHelper * icon) {
    if (mqtt_format() != Format::HA) {
        return;
    }

    // create entity by prefixing any given prefix
    char new_entity[50];
    if (prefix != nullptr) {
        snprintf_P(new_entity, sizeof(new_entity), PSTR("%s.%s"), prefix, entity);
    } else {
        strcpy(new_entity, entity);
    }

    std::string device_name = EMSdevice::device_type_2_device_name(device_type);

    // build unique identifier, replacing all . with _ as not to break HA
    std::string uniq(50, '\0');
    snprintf_P(&uniq[0], uniq.capacity() + 1, PSTR("%s_%s"), device_name.c_str(), new_entity);
    std::replace(uniq.begin(), uniq.end(), '.', '_');

    // topic
    char topic[MQTT_TOPIC_MAX_SIZE];
    snprintf_P(topic, sizeof(topic), PSTR("homeassistant/sensor/ems-esp/%s/config"), uniq.c_str());

    // state topic
    char stat_t[MQTT_TOPIC_MAX_SIZE];
    if (suffix != nullptr) {
        snprintf_P(stat_t, sizeof(stat_t), PSTR("%s/%s_data%s"), hostname_.c_str(), device_name.c_str(), uuid::read_flash_string(suffix).c_str());
    } else {
        snprintf_P(stat_t, sizeof(stat_t), PSTR("%s/%s_data"), hostname_.c_str(), device_name.c_str());
    }

    // value template
    char val_tpl[50];
    snprintf_P(val_tpl, sizeof(val_tpl), PSTR("{{value_json.%s}}"), new_entity);

    // ha device
    char ha_device[40];
    snprintf_P(ha_device, sizeof(ha_device), PSTR("ems-esp-%s"), device_name.c_str());

    // name
    char new_name[50];
    if (prefix != nullptr) {
        snprintf_P(new_name, sizeof(new_name), PSTR("%s %s %s"), device_name.c_str(), prefix, uuid::read_flash_string(name).c_str());
    } else {
        snprintf_P(new_name, sizeof(new_name), PSTR("%s %s"), device_name.c_str(), uuid::read_flash_string(name).c_str());
    }
    new_name[0] = toupper(new_name[0]); // capitalize first letter

    DynamicJsonDocument doc(EMSESP_MAX_JSON_SIZE_MEDIUM);
    doc["name"]    = new_name;
    doc["uniq_id"] = uniq;
    if (uom != nullptr) {
        doc["unit_of_meas"] = uom;
    }
    doc["stat_t"]  = stat_t;
    doc["val_tpl"] = val_tpl;
    if (icon != nullptr) {
        doc["ic"] = icon;
    }
    JsonObject dev = doc.createNestedObject("dev");
    JsonArray  ids = dev.createNestedArray("ids");
    ids.add(ha_device);

    // convert json to string and publish immediately with retain forced to true
    doc.shrinkToFit();
    std::string payload_text;
    serializeJson(doc, payload_text); // convert json to string
    uint16_t packet_id = mqttClient_->publish(topic, 0, true, payload_text.c_str(), payload_text.size());
    if (!packet_id) {
        LOG_ERROR(F("Failed to publish topic %s"), topic);
    } else {
#if defined(EMSESP_STANDALONE)
        LOG_DEBUG(F("Publishing topic=%s, payload=%s"), topic, payload_text.c_str());
#else
        LOG_DEBUG(F("Publishing topic %s"), topic);
#endif
    }

    delay(MQTT_PUBLISH_WAIT); // don't flood asynctcp
}

} // namespace emsesp
