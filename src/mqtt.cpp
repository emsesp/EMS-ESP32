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

MAKE_PSTR_WORD(username)
MAKE_PSTR_WORD(qos)
MAKE_PSTR_WORD(base)
MAKE_PSTR_WORD(heartbeat)
MAKE_PSTR_WORD(ip)
MAKE_PSTR_WORD(nested)
MAKE_PSTR_WORD(single)
MAKE_PSTR_WORD(ha)
MAKE_PSTR_WORD(my)
MAKE_PSTR_WORD(publish_time)
MAKE_PSTR_WORD(publish)
MAKE_PSTR_WORD(connected)
MAKE_PSTR_WORD(disconnected)

MAKE_PSTR(mqtt_ip_fmt, "IP = %s")
MAKE_PSTR(mqtt_user_fmt, "Username = %s")
MAKE_PSTR(mqtt_password_fmt, "Password = %S")
MAKE_PSTR(mqtt_port_fmt, "Port = %d")
MAKE_PSTR(mqtt_enabled_fmt, "MQTT is %s")
MAKE_PSTR(mqtt_base_fmt, "Base = %s")
MAKE_PSTR(mqtt_qos_fmt, "QOS = %ld")
MAKE_PSTR(mqtt_retain_fmt, "Retain Flag = %s")
MAKE_PSTR(mqtt_format_fmt, "Format for JSON = %s")
MAKE_PSTR(mqtt_heartbeat_fmt, "Heartbeat = %s")
MAKE_PSTR(mqtt_publish_time_fmt, "Publish time = %d seconds")

MAKE_PSTR(logger_name, "mqtt")

namespace emsesp {

// exposing static stuff to compiler/linker
#ifndef EMSESP_STANDALONE
AsyncMqttClient Mqtt::mqttClient_;
#endif

std::vector<Mqtt::MQTTFunction>     Mqtt::mqtt_functions_;
bool                                Mqtt::mqtt_retain_;
uint8_t                             Mqtt::mqtt_qos_;
std::string                         Mqtt::mqtt_hostname_; // copy of hostname
uint8_t                             Mqtt::mqtt_format_;
std::string                         Mqtt::mqtt_base_;
uint16_t                            Mqtt::mqtt_publish_fails_    = 0;
size_t                              Mqtt::maximum_mqtt_messages_ = Mqtt::MAX_MQTT_MESSAGES;
bool                                Mqtt::force_publish_         = false;
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

// empty queue
void Mqtt::flush_message_queue() {
    mqtt_messages_.clear();
    mqtt_message_id_ = 0;
}

// restart MQTT services
void Mqtt::reconnect() {
#ifndef EMSESP_STANDALONE
    mqttClient_.disconnect();
#endif
    LOG_DEBUG(F("Reconnecting..."));
}

// MQTT setup
void Mqtt::setup() {
    // exit if already initialized
    if (mqtt_start_) {
        return;
    }
    mqtt_start_ = true;

    // get some settings and store them locally. This is also because the asyncmqtt library uses references for char *
    Settings settings;
    mqtt_enabled_      = settings.mqtt_enabled();
    mqtt_hostname_     = settings.hostname();
    mqtt_base_         = settings.mqtt_base();
    mqtt_qos_          = settings.mqtt_qos();
    mqtt_format_       = settings.mqtt_format();
    mqtt_retain_       = settings.mqtt_retain();
    mqtt_heartbeat_    = settings.mqtt_heartbeat();
    mqtt_publish_time_ = settings.mqtt_publish_time() * 1000; // convert to seconds
    mqtt_ip_           = settings.mqtt_ip();
    mqtt_user_         = settings.mqtt_user();
    mqtt_password_     = settings.mqtt_password();
    mqtt_port_         = settings.mqtt_port();

    // if IP is empty, disable MQTT
    if (settings.mqtt_ip().empty()) {
        mqtt_enabled_ = false;
    }

    init(); // set up call backs. only done once.

#ifdef EMSESP_STANDALONE
    mqtt_enabled_ = true; // force it on for debugging standalone
#else
    mqttClient_.setServer(mqtt_ip_.c_str(), mqtt_port_);
    mqttClient_.setClientId(mqtt_hostname_.c_str());
    mqttClient_.setWill(make_topic(will_topic_, "status"), 1, true, "offline"); // qos 1, retain true
    mqttClient_.setKeepAlive(MQTT_KEEP_ALIVE);
    mqttClient_.setCleanSession(false);

    // set credentials if we have them
    if (!mqtt_user_.empty()) {
        mqttClient_.setCredentials(mqtt_user_.c_str(), mqtt_password_.c_str());
    }
#endif

    mqtt_connecting_      = false;
    mqtt_last_connection_ = uuid::get_uptime();
    mqtt_reconnect_delay_ = Mqtt::MQTT_RECONNECT_DELAY_MIN;

    LOG_DEBUG(F("Configuring MQTT service..."));
}

// MQTT init callbacks
// This should only be executed once
void Mqtt::init() {
    if (mqtt_init_) {
        return;
    }
    mqtt_init_ = true;

#ifndef EMSESP_STANDALONE
    mqttClient_.onConnect([this](bool sessionPresent) { on_connect(); });

    mqttClient_.onDisconnect([this](AsyncMqttClientDisconnectReason reason) {
        if (reason == AsyncMqttClientDisconnectReason::TCP_DISCONNECTED) {
            LOG_DEBUG(F("Disconnected from server"));
        }
        if (reason == AsyncMqttClientDisconnectReason::MQTT_IDENTIFIER_REJECTED) {
            LOG_ERROR(F("Server identifier Rejected"));
        }
        if (reason == AsyncMqttClientDisconnectReason::MQTT_SERVER_UNAVAILABLE) {
            LOG_ERROR(F("Server unavailable"));
        }
        if (reason == AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS) {
            LOG_ERROR(F("Malformed credentials"));
        }
        if (reason == AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED) {
            LOG_ERROR(F("Not authorized"));
        }

        // Reset reconnection delay
        mqtt_last_connection_ = uuid::get_uptime();
        mqtt_connecting_      = false;
        mqtt_start_           = false; // will force a new start()
    });

    // mqttClient.onSubscribe([this](uint16_t packetId, uint8_t qos) { LOG_DEBUG(F("Subscribe ACK for PID %d"), packetId); });

    mqttClient_.onPublish([this](uint16_t packetId) { on_publish(packetId); });

    mqttClient_.onMessage([this](char * topic, char * payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
        on_message(topic, payload, len);
    });
#endif
}

Mqtt::MQTTFunction::MQTTFunction(uint8_t device_id, const std::string && topic, mqtt_function_p mqtt_function)
    : device_id_(device_id)
    , topic_(topic)
    , mqtt_function_(mqtt_function) {
}

// subscribe to an MQTT topic, and store the associated callback function
void Mqtt::subscribe(const uint8_t device_id, const std::string & topic, mqtt_function_p cb) {
    // We don't want to store the whole topic string in our lookup, just the last cmd, as this is wasteful.
    // strip out everything until the last /
    size_t found = topic.find_last_of("/");                                          // returns npos which is -1
    mqtt_functions_.emplace_back(device_id, std::move(topic.substr(found + 1)), cb); // register a call back function for a specific telegram type

    queue_subscribe_message(topic); // add subscription to queue
}

// subscribe to an MQTT topic, and store the associated callback function. For generic functions not tied to a device
void Mqtt::subscribe(const std::string & topic, mqtt_function_p cb) {
    subscribe(0, topic, cb); // no device_id needed, if generic to EMS-ESP
}

// Main MQTT loop
// Checks for connection, establishes a connection if not
// sends out top item on publish queue
void Mqtt::loop() {
    // exit if MQTT is not enabled, there is no WIFI or we're still in the MQTT connection process
#ifndef EMSESP_STANDALONE
    if (!mqtt_enabled_ || mqtt_connecting_ || (WiFi.status() != WL_CONNECTED)) {
#else
    if (false) {
#endif
        return;
    }

    // if we're already connected....
    if (connected()) {
        if (force_publish_) {
            force_publish_ = false;
            send_heartbeat();             // create a heartbeat payload
            EMSESP::publish_all_values(); // add sensors and mqtt to queue
            process_all_queue();          // publish everything on queue
        }

        // send out heartbeat
        uint32_t currentMillis = uuid::get_uptime();
        if ((currentMillis - last_heartbeat_ > MQTT_HEARTBEAT_INTERVAL)) {
            last_heartbeat_ = currentMillis;
            send_heartbeat();
        }

        // create publish messages for each of the EMS device values, adding to queue
        if (currentMillis - last_publish_ >= mqtt_publish_time_) {
            last_publish_ = currentMillis;
            EMSESP::publish_all_values();
        }

        // publish top item from MQTT queue to stop flooding
        if ((uint32_t)(currentMillis - last_mqtt_poll_) > MQTT_PUBLISH_WAIT) {
            last_mqtt_poll_ = currentMillis;
            process_queue();
        }

        return;
    }

    // We need to reconnect. Check when was the last time we tried this
    if (mqtt_last_connection_ && (uuid::get_uptime() - mqtt_last_connection_ < mqtt_reconnect_delay_)) {
        return;
    }

    mqtt_connecting_ = true; // we're doing a connection now

    // Increase the reconnect delay for next time
    mqtt_reconnect_delay_ += MQTT_RECONNECT_DELAY_STEP;
    if (mqtt_reconnect_delay_ > MQTT_RECONNECT_DELAY_MAX) {
        mqtt_reconnect_delay_ = MQTT_RECONNECT_DELAY_MAX;
    }

#ifndef EMSESP_STANDALONE
    setup();
    LOG_INFO(F("Connecting to the MQTT server..."));
    mqttClient_.connect(); // Connect to the MQTT broker
#endif
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

// simulate receiving a MQTT message, only for testing
void Mqtt::incoming(char * topic, char * payload) {
    on_message(topic, payload, strlen(payload));
}

// received MQTT message
void Mqtt::on_message(char * topic, char * payload, size_t len) {
    if (len == 0) {
        return;
    }

    // convert payload to a null-terminated char string
    char message[len + 2];
    strlcpy(message, payload, len + 1);

#ifdef EMSESP_DEBUG
    LOG_DEBUG(F("Received %s => %s (length %d)"), topic, message, len);
#endif

    // strip out everything until the last /
    char * topic_magnitude = strrchr(topic, '/');
    if (topic_magnitude != nullptr) {
        topic = topic_magnitude + 1;
    }

    // Send message event to custom service
    // this will pick the first topic that matches, so for multiple devices of the same type it's gonna fail
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

    shell.print(F(" These MQTT topics are registered: "));
    for (const auto & mqtt_function : mqtt_functions_) {
        if (mqtt_function.device_id_ == device_id) {
            shell.printf(F("%s "), mqtt_function.topic_.c_str());
        }
    }
    shell.println();
}

// called when an MQTT Publish ACK is received
// its a poor-mans QOS we assume the ACK represents the last Publish sent
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

// builds up a topic by prefixing the base and hostname
char * Mqtt::make_topic(char * result, const std::string & topic) {
    if (!mqtt_base_.empty()) {
        strlcpy(result, mqtt_base_.c_str(), MQTT_TOPIC_MAX_SIZE);
        strlcat(result, "/", MQTT_TOPIC_MAX_SIZE);
        strlcat(result, mqtt_hostname_.c_str(), MQTT_TOPIC_MAX_SIZE);
    } else {
        strlcpy(result, mqtt_hostname_.c_str(), MQTT_TOPIC_MAX_SIZE);
    }

    strlcat(result, "/", MQTT_TOPIC_MAX_SIZE);
    strlcat(result, topic.c_str(), MQTT_TOPIC_MAX_SIZE);

    return result;
}

void Mqtt::start() {
    send_start_topic();
}

// send online appended with the version information as JSON
void Mqtt::send_start_topic() {
    StaticJsonDocument<90> doc;
    doc["event"]   = "start";
    doc["version"] = Settings().app_version();
    publish("info", doc, false); // send with retain off
}

// MQTT onConnect - when a connect is established
void Mqtt::on_connect() {
    mqtt_reconnect_delay_ = Mqtt::MQTT_RECONNECT_DELAY_MIN;
    mqtt_last_connection_ = uuid::get_uptime();
    mqtt_connecting_      = false;

    publish("status", "online", true); // say we're alive to the Last Will topic, with retain on

    send_heartbeat(); // send heartbeat if enabled

    LOG_INFO(F("MQTT connected"));
}

// send periodic MQTT message with system information
void Mqtt::send_heartbeat() {
    if (!mqtt_heartbeat_) {
        return;
    }

    StaticJsonDocument<90> doc;

    doc["rssid"]            = Network::wifi_quality();
    doc["uptime"]           = uuid::log::format_timestamp_ms(uuid::get_uptime_ms(), 3);
    doc["freemem"]          = System::free_mem();
    doc["mqttpublishfails"] = mqtt_publish_fails_;

    publish("heartbeat", doc, false); // send to MQTT with retain off
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
    LOG_DEBUG(F("Adding a subscription for %s"), topic.c_str());

    // if the queue is full, make room but removing the last one
    if (mqtt_messages_.size() >= maximum_mqtt_messages_) {
        mqtt_messages_.pop_front();
    }

    mqtt_messages_.emplace_back(mqtt_message_id_++, std::move(message));
}

// Publish using the user's custom retain flag
void Mqtt::publish(const std::string & topic, const std::string & payload) {
    publish(topic, payload, mqtt_retain_);
}
void Mqtt::publish(const std::string & topic, const JsonDocument & payload) {
    publish(topic, payload, mqtt_retain_);
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
    queue_publish_message(topic, value ? "1" : "0", mqtt_retain_);
}

// no payload
void Mqtt::publish(const std::string & topic) {
    queue_publish_message(topic, "", mqtt_retain_);
}

// publish all queued messages to MQTT
void Mqtt::process_all_queue() {
    while (!mqtt_messages_.empty()) {
        process_queue();
    }
}

// take top from queue and try and publish it
// assumes there is an MQTT connection
void Mqtt::process_queue() {
    if (mqtt_messages_.empty()) {
        return;
    }

    // fetch first from queue and create the full topic name
    auto mqtt_message = mqtt_messages_.front();
    auto message      = mqtt_message.content_;

    // append the hostname and base to the topic, unless we're doing native HA which has a different format
    char full_topic[MQTT_TOPIC_MAX_SIZE];

    // if the topic starts with "homeassistant" we leave it untouched, otherwise append host and base
    if (strncmp(message->topic.c_str(), "homeassistant/", 13) == 0) {
        strcpy(full_topic, message->topic.c_str());
    } else {
        make_topic(full_topic, message->topic);
    }

    // if we're subscribing...
    if (message->operation == Operation::SUBSCRIBE) {
        LOG_DEBUG(F("Subscribing to topic: %s"), full_topic);
#ifndef EMSESP_STANDALONE
        uint16_t packet_id = mqttClient_.subscribe(full_topic, mqtt_qos_);
#else
        uint16_t packet_id = 1;
#endif
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

#ifndef EMSESP_STANDALONE
    // uint16_t packet_id = mqttClient_.publish(full_topic, mqtt_qos_, message->retain, message->payload.c_str());
    uint16_t packet_id = mqttClient_.publish(full_topic, mqtt_qos_, message->retain, message->payload.c_str(), message->payload.size(), false, mqtt_message.id_);
#else
    uint16_t packet_id = 1;
#endif
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

// add console commands
void Mqtt::console_commands(Shell & shell, unsigned int context) {
    EMSESPShell::commands->add_command(
        ShellContext::MQTT,
        CommandFlags::ADMIN,
        flash_string_vector{F_(set), F_(heartbeat)},
        flash_string_vector{F_(bool_mandatory)},
        [](Shell & shell, const std::vector<std::string> & arguments) {
            Settings settings;
            if (arguments[0] == read_flash_string(F_(on))) {
                settings.mqtt_heartbeat(true);
                settings.commit();
            } else if (arguments[0] == read_flash_string(F_(off))) {
                settings.mqtt_heartbeat(false);
                settings.commit();
            } else {
                shell.println(F("Must be on or off"));
                return;
            }
        },
        [](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments __attribute__((unused))) -> const std::vector<std::string> {
            return std::vector<std::string>{read_flash_string(F_(on)), read_flash_string(F_(off))};
        });

    EMSESPShell::commands->add_command(
        ShellContext::MQTT,
        CommandFlags::ADMIN,
        flash_string_vector{F_(set), F_(format)},
        flash_string_vector{F_(name_mandatory)},
        [](Shell & shell, const std::vector<std::string> & arguments) {
            Settings settings;
            uint8_t  value;
            if (arguments[0] == read_flash_string(F_(single))) {
                value = Settings::MQTT_format::SINGLE;
            } else if (arguments[0] == read_flash_string(F_(nested))) {
                value = Settings::MQTT_format::NESTED;
            } else if (arguments[0] == read_flash_string(F_(ha))) {
                value = Settings::MQTT_format::HA;
            } else if (arguments[0] == read_flash_string(F_(my))) {
                value = Settings::MQTT_format::MY;
            } else {
                shell.println(F("Must be single, nested or ha"));
                return;
            }
            settings.mqtt_format(value);
            settings.commit();
            shell.println(F("Please restart EMS-ESP"));
        },
        [](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments __attribute__((unused))) -> const std::vector<std::string> {
            return std::vector<std::string>{read_flash_string(F_(single)), read_flash_string(F_(nested)), read_flash_string(F_(ha)), read_flash_string(F_(my))};
        });

    EMSESPShell::commands->add_command(ShellContext::MQTT,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(set), F_(publish_time)},
                                       flash_string_vector{F_(seconds_mandatory)},
                                       [](Shell & shell, const std::vector<std::string> & arguments) {
                                           uint16_t publish_time = Helpers::atoint(arguments.front().c_str());
                                           Settings settings;
                                           settings.mqtt_publish_time(publish_time);
                                           settings.commit();
                                           shell.printfln(F_(mqtt_publish_time_fmt), settings.mqtt_publish_time());
                                       });

    EMSESPShell::commands->add_command(
        ShellContext::MQTT,
        CommandFlags::ADMIN,
        flash_string_vector{F_(set), F_(enabled)},
        flash_string_vector{F_(bool_mandatory)},
        [](Shell & shell, const std::vector<std::string> & arguments) {
            Settings settings;
            if (arguments[0] == read_flash_string(F_(on))) {
                settings.mqtt_enabled(true);
                settings.commit();
                reconnect();
            } else if (arguments[0] == read_flash_string(F_(off))) {
                settings.mqtt_enabled(false);
                settings.commit();
                reconnect();
            } else {
                shell.println(F("Value must be on or off"));
                return;
            }
        },
        [](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments __attribute__((unused))) -> const std::vector<std::string> {
            return std::vector<std::string>{read_flash_string(F_(on)), read_flash_string(F_(off))};
        });

    EMSESPShell::commands->add_command(ShellContext::MQTT,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(set), F_(ip)},
                                       flash_string_vector{F_(ip_address_mandatory)},
                                       [](Shell & shell, const std::vector<std::string> & arguments) {
                                           Settings settings;

                                           if (!arguments.empty()) {
                                               settings.mqtt_ip(arguments.front());
                                               settings.commit();
                                           }
                                           auto ip = settings.mqtt_ip();
                                           shell.printfln(F_(mqtt_ip_fmt), ip.empty() ? uuid::read_flash_string(F_(unset)).c_str() : ip.c_str());
                                           reconnect();
                                       });

    EMSESPShell::commands->add_command(ShellContext::MQTT,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(set), F_(base)},
                                       flash_string_vector{F_(name_mandatory)},
                                       [](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments) {
                                           Settings settings;
                                           if (!arguments.empty()) {
                                               settings.mqtt_base(arguments.front());
                                               settings.commit();
                                           }
                                           reconnect();
                                       });

    EMSESPShell::commands->add_command(ShellContext::MQTT,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(set), F_(qos)},
                                       flash_string_vector{F_(n_mandatory)},
                                       [](Shell & shell, const std::vector<std::string> & arguments) {
                                           uint8_t value = (arguments[0]).at(0) - '0';

                                           if (value <= 2) {
                                               Settings settings;
                                               settings.mqtt_qos(value);
                                               settings.commit();
                                               reconnect();
                                           } else {
                                               shell.printfln(F("Value must be 0, 1 or 2"));
                                           }
                                       });

    EMSESPShell::commands->add_command(ShellContext::MQTT,
                                       CommandFlags::USER,
                                       flash_string_vector{F_(publish)},
                                       [=](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                                           shell.printfln(F("Publishing all values to MQTT"));
                                           force_publish_ = true;
                                       });

    EMSESPShell::commands->add_command(ShellContext::MQTT,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(reconnect)},
                                       [](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments __attribute__((unused))) {
                                           reconnect();
                                       });

    EMSESPShell::commands->add_command(ShellContext::MQTT,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(set), F_(password)},
                                       [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                                           shell.enter_password(F_(new_password_prompt1), [=](Shell & shell, bool completed, const std::string & password1) {
                                               if (completed) {
                                                   shell.enter_password(F_(new_password_prompt2),
                                                                        [password1](Shell & shell, bool completed, const std::string & password2) {
                                                                            if (completed) {
                                                                                if (password1 == password2) {
                                                                                    Settings settings;
                                                                                    settings.mqtt_password(password2);
                                                                                    settings.commit();
                                                                                    shell.println(F("MQTT password updated"));
                                                                                    reconnect();
                                                                                } else {
                                                                                    shell.println(F("Passwords do not match"));
                                                                                }
                                                                            }
                                                                        });
                                               }
                                           });
                                       });

    EMSESPShell::commands->add_command(ShellContext::MQTT,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(set), F_(username)},
                                       flash_string_vector{F_(name_optional)},
                                       [](Shell & shell, const std::vector<std::string> & arguments) {
                                           Settings settings;
                                           if (arguments.empty()) {
                                               settings.mqtt_user("");
                                           } else {
                                               settings.mqtt_user(arguments.front());
                                           }
                                           settings.commit();
                                           shell.printfln(F_(mqtt_user_fmt),
                                                          settings.mqtt_user().empty() ? uuid::read_flash_string(F_(unset)).c_str()
                                                                                       : settings.mqtt_user().c_str());
                                           reconnect();
                                       });

    EMSESPShell::commands->add_command(ShellContext::MQTT,
                                       CommandFlags::USER,
                                       flash_string_vector{F_(show)},
                                       [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) { show_mqtt(shell); });


    EMSESPShell::commands->add_command(
        ShellContext::MQTT, CommandFlags::USER, flash_string_vector{F_(set)}, [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
            Settings settings;
            shell.printfln(F_(mqtt_enabled_fmt), settings.mqtt_enabled() ? F_(enabled) : F_(disabled));
            shell.printfln(F_(mqtt_ip_fmt), settings.mqtt_ip().empty() ? uuid::read_flash_string(F_(unset)).c_str() : settings.mqtt_ip().c_str());
            shell.printfln(F_(mqtt_user_fmt), settings.mqtt_user().empty() ? uuid::read_flash_string(F_(unset)).c_str() : settings.mqtt_user().c_str());
            shell.printfln(F_(mqtt_password_fmt), settings.mqtt_password().empty() ? F_(unset) : F_(asterisks));
            shell.printfln(F_(mqtt_port_fmt), settings.mqtt_port());
            shell.printfln(F_(mqtt_base_fmt), settings.mqtt_base().empty() ? uuid::read_flash_string(F_(unset)).c_str() : settings.mqtt_base().c_str());
            shell.printfln(F_(mqtt_qos_fmt), settings.mqtt_qos());
            shell.printfln(F_(mqtt_retain_fmt), settings.mqtt_retain() ? F_(enabled) : F_(disabled));
            if (settings.mqtt_format() == Settings::MQTT_format::SINGLE) {
                shell.printfln(F_(mqtt_format_fmt), F_(single));
            } else if (settings.mqtt_format() == Settings::MQTT_format::NESTED) {
                shell.printfln(F_(mqtt_format_fmt), F_(nested));
            } else if (settings.mqtt_format() == Settings::MQTT_format::HA) {
                shell.printfln(F_(mqtt_format_fmt), F_(ha));
            } else if (settings.mqtt_format() == Settings::MQTT_format::MY) {
                shell.printfln(F_(mqtt_format_fmt), F_(my));
            }
            shell.printfln(F_(mqtt_heartbeat_fmt), settings.mqtt_heartbeat() ? F_(enabled) : F_(disabled));
            shell.printfln(F_(mqtt_publish_time_fmt), settings.mqtt_publish_time());
            shell.println();
        });

    // enter the context
    Console::enter_custom_context(shell, context);
}

} // namespace emsesp
