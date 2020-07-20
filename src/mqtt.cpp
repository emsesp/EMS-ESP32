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

MAKE_PSTR(logger_name, "mqtt")

namespace emsesp {

AsyncMqttClient * Mqtt::mqttClient_;

// static parameters we make global
std::string Mqtt::hostname_;
uint8_t     Mqtt::mqtt_qos_;
uint16_t    Mqtt::publish_time_;

std::vector<Mqtt::MQTTFunction>     Mqtt::mqtt_functions_;
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

MqttMessage::MqttMessage(uint8_t operation, const std::string & topic, const std::string & payload, bool retain)
    : operation(operation)
    , topic(topic)
    , payload(payload)
    , retain(retain) {
}

Mqtt::MQTTFunction::MQTTFunction(uint8_t device_id, const std::string && topic, mqtt_function_p mqtt_function)
    : device_id_(device_id)
    , topic_(topic)
    , mqtt_function_(mqtt_function) {
}

// subscribe to an MQTT topic, and store the associated callback function
void Mqtt::subscribe(const uint8_t device_id, const std::string & topic, mqtt_function_p cb) {
    /*
    // We don't want to store the whole topic string in our lookup, just the last cmd, as this can take up too much memory
    // strip out everything until the last /
    size_t topic_pos = topic.find_last_of("/"); // returns npos which is -1
    topic_pos += 1;                             // skip the /
    */

    // convert the topic to it's full path, so either prefixed with the hostname unless hardcoded like 'homeassistant'
    char full_topic[MQTT_TOPIC_MAX_SIZE];
    make_topic(full_topic, topic);

    // check if we already have the topic subscribed, if so don't add it again
    bool exists = false;
    if (!mqtt_functions_.empty()) {
        for (const auto & mqtt_function : mqtt_functions_) {
            if ((mqtt_function.device_id_ == device_id) && (strcmp(mqtt_function.topic_.c_str(), full_topic) == 0)) {
                exists = true;
            }
        }
    }

    if (!exists) {
        mqtt_functions_.emplace_back(device_id, std::move(full_topic), cb); // register a call back function for a specific telegram type
    }

    queue_subscribe_message(topic); // add subscription to queue
}

// resubscribe to all MQTT topics again
void Mqtt::resubscribe() {
    if (mqtt_functions_.empty()) {
        return;
    }

    for (const auto & mqtt_function : mqtt_functions_) {
        queue_subscribe_message(mqtt_function.topic_);
    }
}

// subscribe to an MQTT topic, and store the associated callback function. For generic functions not tied to a specific device
void Mqtt::subscribe(const std::string & topic, mqtt_function_p cb) {
    subscribe(0, topic, cb); // no device_id needed, if generic to EMS-ESP
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
    shell.printfln(F("MQTT publish fails: %lu"), mqtt_publish_fails_);
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

    /*
    // strip out everything until the last /
    char * topic_magnitude = strrchr(topic, '/');
    if (topic_magnitude != nullptr) {
        topic = topic_magnitude + 1;
    }
    */

    // see if we have this topic in our subscription list, then call its callback handler
    // note: this will pick the first topic that matches, so for multiple devices of the same type it's gonna fail. Not sure if this is going to be an issue?
    for (const auto & mf : mqtt_functions_) {
        if (strcmp(topic, mf.topic_.c_str()) == 0) {
            (mf.mqtt_function_)(message);
            return;
        }
    }

    // if we got here we didn't find a topic match
    LOG_DEBUG(F("No responding handler found for topic %s"), topic);
}

// print all the topics related to a specific device_id
void Mqtt::show_topic_handlers(uuid::console::Shell & shell, const uint8_t device_id) {
    if (std::count_if(mqtt_functions_.cbegin(), mqtt_functions_.cend(), [=](MQTTFunction const & mqtt_function) { return device_id == mqtt_function.device_id_; })
        == 0) {
        return;
    }

    shell.print(F(" Subscribed MQTT topics: "));
    for (const auto & mqtt_function : mqtt_functions_) {
        if (mqtt_function.device_id_ == device_id) {
            shell.printf(F("%s "), mqtt_function.topic_.c_str());
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

// builds up a topic by prefixing the hostname
// unless it's hardcoded like "homeassistant"
char * Mqtt::make_topic(char * result, const std::string & topic) {
    // check for homesassistant
    if (strncmp(topic.c_str(), "homeassistant/", 13) == 0) {
        strlcpy(result, topic.c_str(), MQTT_TOPIC_MAX_SIZE);
        return result;
    }

    strlcpy(result, hostname_.c_str(), MQTT_TOPIC_MAX_SIZE);
    strlcat(result, "/", MQTT_TOPIC_MAX_SIZE);
    strlcat(result, topic.c_str(), MQTT_TOPIC_MAX_SIZE);

    return result;
}

void Mqtt::start(AsyncMqttClient * mqttClient) {
    mqttClient_ = mqttClient; // copy over from esp8266-react's MQTT service

    // get the hostname, which we'll use to prefix to all topics
    EMSESP::esp8266React.getWiFiSettingsService()->read([&](WiFiSettings & wifiSettings) { hostname_ = wifiSettings.hostname.c_str(); });

    // fetch MQTT settings
    EMSESP::esp8266React.getMqttSettingsService()->read([&](MqttSettings & mqttSettings) {
        publish_time_ = mqttSettings.publish_time * 1000; // convert to milliseconds
        mqtt_qos_     = mqttSettings.mqtt_qos;
    });


    mqttClient_->onConnect([this](bool sessionPresent) { on_connect(); });
    mqttClient_->setWill(make_topic(will_topic_, "status"), 1, true, "offline"); // with qos 1, retain true
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

    LOG_INFO(F("MQTT connected"));
}

// add MQTT message to queue, payload is a string
void Mqtt::queue_publish_message(const std::string & topic, const std::string & payload, const bool retain) {
    // can't have bogus topics, but empty payloads are ok
    if (topic.empty()) {
        return;
    }

    auto message = std::make_shared<MqttMessage>(Operation::PUBLISH, topic, payload, retain);

    // if the queue is full, make room but removing the last one
    if (mqtt_messages_.size() >= maximum_mqtt_messages_) {
        mqtt_messages_.pop_front();
    }

    mqtt_messages_.emplace_back(mqtt_message_id_++, std::move(message));
}

// add MQTT subscribe message to queue
void Mqtt::queue_subscribe_message(const std::string & topic) {
    if (topic.empty()) {
        return;
    }

    auto message = std::make_shared<MqttMessage>(Operation::SUBSCRIBE, topic, "", false);
#ifdef DEBUG
    LOG_DEBUG(F("Adding a subscription for %s"), topic.c_str());
#endif

    // if the queue is full, make room but removing the last one
    if (mqtt_messages_.size() >= maximum_mqtt_messages_) {
        mqtt_messages_.pop_front();
    }

    mqtt_messages_.emplace_back(mqtt_message_id_++, std::move(message));
}

// MQTT Publish, using a specific retain flag
void Mqtt::publish(const std::string & topic, const std::string & payload, bool retain) {
    queue_publish_message(topic, payload, retain);
}

void Mqtt::publish(const std::string & topic, const JsonDocument & payload, bool retain) {
    // convert json to string
    std::string payload_text;
    serializeJson(payload, payload_text);
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

    // fetch first from queue and create the full topic name
    auto mqtt_message = mqtt_messages_.front();
    auto message      = mqtt_message.content_;

    // append the hostname to the topic, unless we're doing native HA which has a different format
    // if the topic starts with "homeassistant" we leave it untouched, otherwise append host
    char full_topic[MQTT_TOPIC_MAX_SIZE];
    make_topic(full_topic, message->topic);

    // if we're subscribing...
    if (message->operation == Operation::SUBSCRIBE) {
        LOG_DEBUG(F("Subscribing to topic: %s"), full_topic);
        uint16_t packet_id = mqttClient_->subscribe(full_topic, mqtt_qos_);
        if (!packet_id) {
            LOG_DEBUG(F("Error subscribing to %s, error %d"), full_topic, packet_id);
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
    uint16_t packet_id = mqttClient_->publish(full_topic, mqtt_qos_, message->retain, message->payload.c_str(), message->payload.size(), false, mqtt_message.id_);
    LOG_DEBUG(F("Publishing topic %s (#%02d, attempt #%d, pid %d)"), full_topic, mqtt_message.id_, mqtt_message.retry_count_ + 1, packet_id);

    if (packet_id == 0) {
        // it failed. if we retried n times, give up. remove from queue
        if (mqtt_message.retry_count_ == (MQTT_PUBLISH_MAX_RETRY - 1)) {
            LOG_ERROR(F("Failed to publish to %s after %d attempts"), full_topic, mqtt_message.retry_count_ + 1);
            mqtt_publish_fails_++;      // increment failure counter
            mqtt_messages_.pop_front(); // delete
            return;
        } else {
            mqtt_messages_.front().retry_count_++;
            LOG_DEBUG(F("Failed to publish to %s. Trying again, #%d"), full_topic, mqtt_message.retry_count_ + 1);
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
