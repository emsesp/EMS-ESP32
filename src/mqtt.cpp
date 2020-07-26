/*
 * EMS-ESP - https://github.com/proddy/EMS-ESP
 * Copyright 2019  Paul Derbyshire
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

MAKE_PSTR_WORD(connected)
MAKE_PSTR_WORD(disconnected)
MAKE_PSTR(system_heartbeat_fmt, "Heartbeat is %s")

MAKE_PSTR(logger_name, "mqtt")

namespace emsesp {

AsyncMqttClient * Mqtt::mqttClient_;

// static parameters we make global
std::string Mqtt::hostname_;
uint8_t     Mqtt::mqtt_qos_;
uint16_t    Mqtt::publish_time_;

std::vector<Mqtt::MQTTSubFunction>  Mqtt::mqtt_subfunctions_;
uint16_t                            Mqtt::mqtt_publish_fails_    = 0;
size_t                              Mqtt::maximum_mqtt_messages_ = Mqtt::MAX_MQTT_MESSAGES;
uint16_t                            Mqtt::mqtt_message_id_       = 0;
std::deque<Mqtt::QueuedMqttMessage> Mqtt::mqtt_messages_;
char                                will_topic_[Mqtt::MQTT_TOPIC_MAX_SIZE]; // because MQTT library keeps only char pointer

uuid::log::Logger Mqtt::logger_{F_(logger_name), uuid::log::Facility::DAEMON};

Mqtt::QueuedMqttMessage::QueuedMqttMessage(uint16_t id, std::shared_ptr<MqttMessage> && content)
    : id_(id)
    , content_(std::move(content)) {
    retry_count_ = 0;
    packet_id_   = 0;
}

MqttMessage::MqttMessage(const uint8_t operation, const std::string & topic, const std::string & payload, bool retain)
    : operation(operation)
    , topic(topic)
    , payload(payload)
    , retain(retain) {
}

Mqtt::MQTTSubFunction::MQTTSubFunction(const uint8_t device_id, const std::string && topic, mqtt_subfunction_p mqtt_subfunction)
    : device_id_(device_id)
    , topic_(topic)
    , mqtt_subfunction_(mqtt_subfunction) {
}

// subscribe to an MQTT topic, and store the associated callback function
void Mqtt::subscribe(const uint8_t device_id, const std::string & topic, mqtt_subfunction_p cb) {
    auto message = queue_subscribe_message(topic); // add subscription to queue. The hostname will automatically be appended

    if (message == nullptr) {
        return;
    }

    // the message will contain the full topic, with the hostname prefixed
    // check if we already have the topic subscribed, if so don't add it again
    bool exists = false;
    if (!mqtt_subfunctions_.empty()) {
        for (const auto & mqtt_subfunction : mqtt_subfunctions_) {
            if ((mqtt_subfunction.device_id_ == device_id) && (strcmp(mqtt_subfunction.topic_.c_str(), message->topic.c_str()) == 0)) {
                exists = true;
            }
        }
    }

    if (!exists) {
        mqtt_subfunctions_.emplace_back(device_id, std::move(message->topic), cb); // register a call back function for a specific telegram type
    }
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
        queue_message(Operation::SUBSCRIBE, mqtt_subfunction.topic_, "", false, true); // no payload, no topic prefixing
    }
}

// Main MQTT loop
// Checks for connection, establishes a connection if not
// sends out top item on publish queue
void Mqtt::loop() {
    // exit if MQTT is not enabled or if there is no WIFI
    if (!connected()) {
        return;
    }

    uint32_t currentMillis = uuid::get_uptime();

    // create publish messages for each of the EMS device values, adding to queue
    if (publish_time_ && (currentMillis - last_publish_ > publish_time_)) {
        last_publish_ = currentMillis;
        EMSESP::publish_all_values();
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

    bool system_heartbeat;
    EMSESP::esp8266React.getMqttSettingsService()->read([&](MqttSettings & settings) { system_heartbeat = settings.system_heartbeat; });
    shell.printfln(F_(system_heartbeat_fmt), system_heartbeat ? F_(enabled) : F_(disabled));

    shell.printfln(F("MQTT publish fails: %lu"), mqtt_publish_fails_);
    shell.println();

    // show subscriptions
    shell.printfln(F("MQTT subscriptions:"));
    for (const auto & mqtt_subfunction : mqtt_subfunctions_) {
        shell.printfln(F(" %s"), mqtt_subfunction.topic_.c_str());
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
}

// simulate receiving a MQTT message, used only for testing
void Mqtt::incoming(char * topic, char * payload) {
    on_message(topic, payload, strlen(payload));
}

// received an MQTT message that we subscribed too
void Mqtt::on_message(char * topic, char * payload, size_t len) {
    if (len == 0) {
        return;
    }

    // convert payload to a null-terminated char string
    char message[len + 2];
    strlcpy(message, payload, len + 1);
    LOG_DEBUG(F("[DEBUG] Received %s => %s (length %d)"), topic, message, len);

    // see if we have this topic in our subscription list, then call its callback handler
    // note: this will pick the first topic that matches, so for multiple devices of the same type it's gonna fail. Not sure if this is going to be an issue?
    for (const auto & mf : mqtt_subfunctions_) {
        if (strcmp(topic, mf.topic_.c_str()) == 0) {
            (mf.mqtt_subfunction_)(message);
            return;
        }
    }

    // if we got here we didn't find a topic match
    LOG_DEBUG(F("No responding handler found for topic %s"), topic);
}

// print all the topics related to a specific device_id
void Mqtt::show_topic_handlers(uuid::console::Shell & shell, const uint8_t device_id) {
    if (std::count_if(mqtt_subfunctions_.cbegin(),
                      mqtt_subfunctions_.cend(),
                      [=](MQTTSubFunction const & mqtt_subfunction) { return device_id == mqtt_subfunction.device_id_; })
        == 0) {
        return;
    }

    shell.print(F(" Subscribed MQTT topics: "));
    for (const auto & mqtt_subfunction : mqtt_subfunctions_) {
        if (mqtt_subfunction.device_id_ == device_id) {
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
        LOG_DEBUG(F("Mismatch, expecting PID %d, got %d"), mqtt_message.packet_id_, packetId);
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
        publish_time_ = mqttSettings.publish_time * 1000; // convert to milliseconds
        mqtt_qos_     = mqttSettings.mqtt_qos;
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
        on_message(topic, payload, len);
        mqttClient_->onPublish([this](uint16_t packetId) { on_publish(packetId); });
    });
}

void Mqtt::set_publish_time(uint16_t publish_time) {
    publish_time_ = publish_time * 1000; // convert to milliseconds
}

void Mqtt::set_qos(uint8_t mqtt_qos) {
    mqtt_qos_ = mqtt_qos;
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
    publish("info", doc, false); // send with retain off

    publish("status", "online", true); // say we're alive to the Last Will topic, with retain on

    reset_publish_fails(); // reset fail count to 0

    resubscribe(); // in case this is a reconnect, re-subscribe again to all MQTT topics

    // add the system MQTT subscriptions, only if its a fresh start with no previous subscriptions
    if (mqtt_subfunctions_.empty()) {
        Mqtt::subscribe("cmd", System::mqtt_commands);
    }

    LOG_INFO(F("MQTT connected"));
}

// add sub or pub task to the queue. When the message is created, the topic will have
// automatically the hostname prefixed.
std::shared_ptr<const MqttMessage>
Mqtt::queue_message(const uint8_t operation, const std::string & topic, const std::string & payload, const bool retain, bool no_prefix) {
    if (topic.empty()) {
        return nullptr;
    }

    // take the topic and prefix the hostname, unless its for HA
    std::shared_ptr<MqttMessage> message;
    if ((strncmp(topic.c_str(), "homeassistant/", 13) == 0) || no_prefix) {
        // leave topic as it is
        message = std::make_shared<MqttMessage>(operation, topic, payload, retain);
    } else {
        // prefix the hostname
        std::string full_topic = Mqtt::hostname_ + "/" + topic;
        message                = std::make_shared<MqttMessage>(operation, full_topic, payload, retain);
    }

    // if the queue is full, make room but removing the last one
    if (mqtt_messages_.size() >= maximum_mqtt_messages_) {
        mqtt_messages_.pop_front();
    }
    mqtt_messages_.emplace_back(mqtt_message_id_++, std::move(message));

    return mqtt_messages_.back().content_; // this is because the message has been moved
}

// add MQTT message to queue, payload is a string
std::shared_ptr<const MqttMessage> Mqtt::queue_publish_message(const std::string & topic, const std::string & payload, const bool retain) {
    return queue_message(Operation::PUBLISH, topic, payload, retain);
}

// add MQTT subscribe message to queue
std::shared_ptr<const MqttMessage> Mqtt::queue_subscribe_message(const std::string & topic) {
    return queue_message(Operation::SUBSCRIBE, topic, "", false); // no payload
}

// MQTT Publish, using a specific retain flag
void Mqtt::publish(const std::string & topic, const std::string & payload, bool retain) {
    queue_publish_message(topic, payload, retain);
}

void Mqtt::publish(const std::string & topic, const JsonDocument & payload, bool retain) {
    std::string payload_text;
    serializeJson(payload, payload_text); // convert json to string
    queue_publish_message(topic, payload_text, retain);
}

// for booleans, which get converted to string values 1 and 0
void Mqtt::publish(const std::string & topic, const bool value) {
    queue_publish_message(topic, value ? "1" : "0", false);
}

// no payload
void Mqtt::publish(const std::string & topic) {
    queue_publish_message(topic, "", false);
}

// publish all queued messages to MQTT
void Mqtt::process_all_queue() {
    while (!mqtt_messages_.empty()) {
        process_queue();
    }
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
            LOG_DEBUG(F("Error subscribing to %s, error %d"), message->topic.c_str(), packet_id);
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
    LOG_DEBUG(F("Publishing topic %s (#%02d, attempt #%d, pid %d)"), message->topic.c_str(), mqtt_message.id_, mqtt_message.retry_count_ + 1, packet_id);

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
        // LOG_DEBUG(F("Setting packetID for ACK to %d"), packet_id);
        return;
    }

    mqtt_messages_.pop_front(); // remove the message from the queue
}

} // namespace emsesp
