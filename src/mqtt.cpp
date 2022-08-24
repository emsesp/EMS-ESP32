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
#include "emsdevice.h"

namespace emsesp {

AsyncMqttClient * Mqtt::mqttClient_;

// static parameters we make global
std::string Mqtt::system_hostname_; // copy from System::hostname()
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
bool        Mqtt::ha_enabled_;
uint8_t     Mqtt::nested_format_;
std::string Mqtt::discovery_prefix_;
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

// Home Assistant specific
// icons from https://materialdesignicons.com used with the UOMs (unit of measurements)
MAKE_PSTR_WORD(measurement)
MAKE_PSTR_WORD(total_increasing)
MAKE_PSTR(icondegrees, "mdi:coolant-temperature") // DeviceValueUOM::DEGREES
MAKE_PSTR(iconpercent, "mdi:percent-outline")     // DeviceValueUOM::PERCENT
MAKE_PSTR(icontime, "mdi:clock-outline")          // DeviceValueUOM::SECONDS MINUTES & HOURS
MAKE_PSTR(iconkb, "mdi:memory")                   // DeviceValueUOM::KB
MAKE_PSTR(iconlmin, "mdi:water-boiler")           // DeviceValueUOM::LMIN
// MAKE_PSTR(iconkwh, "mdi:transmission-tower")      // DeviceValueUOM::KWH & WH
MAKE_PSTR(iconua, "mdi:lightning-bolt-circle") // DeviceValueUOM::UA
// MAKE_PSTR(iconbar, "mdi:gauge")                   // DeviceValueUOM::BAR
// MAKE_PSTR(iconkw, "mdi:omega")                    // DeviceValueUOM::KW & W
// MAKE_PSTR(icondbm, "mdi:wifi-strength-2")         // DeviceValueUOM::DBM
MAKE_PSTR(iconnum, "mdi:counter")            // DeviceValueUOM::NONE
MAKE_PSTR(icondevice, "mdi:home-automation") // for devices in HA

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
        EMSESP::publish_other_values(); // switch and heatpump
    } else

        if (publish_time_sensor_ && (currentMillis - last_publish_sensor_ > publish_time_sensor_)) {
        last_publish_sensor_ = (currentMillis / publish_time_sensor_) * publish_time_sensor_;
        EMSESP::publish_sensor_values(true);
    }
}

// print MQTT log and other stuff to console
void Mqtt::show_mqtt(uuid::console::Shell & shell) {
    shell.printfln(F("MQTT is %s"), connected() ? read_flash_string(F_(connected)).c_str() : read_flash_string(F_(disconnected)).c_str());

    shell.printfln(F("MQTT publish errors: %lu"), mqtt_publish_fails_);
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

        // prefix base, only if it's not a discovery topic
        if (content->topic.compare(0, discovery_prefix().size(), discovery_prefix()) == 0) {
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
        LOG_DEBUG(F("Received topic `%s` => payload `%s` (length %d)"), topic, message, len);
    } else {
        LOG_DEBUG(F("Received topic `%s`"), topic);
    }
#endif
    // remove HA topics if we don't use discovery
    if (strncmp(topic, discovery_prefix().c_str(), discovery_prefix().size()) == 0) {
        if (!ha_enabled_ && len) { // don't ping pong the empty message
            queue_publish_message(topic, "", true);
            LOG_DEBUG(F("Remove topic %s"), topic);
        }
        return;
    }

    // check first against any of our subscribed topics
    for (const auto & mf : mqtt_subfunctions_) {
        // add the base back
        char full_topic[MQTT_TOPIC_MAX_SIZE];
        snprintf(full_topic, sizeof(full_topic), "%s/%s", mqtt_base_.c_str(), mf.topic_.c_str());
        if ((!strcmp(topic, full_topic)) && (mf.mqtt_subfunction_)) {
            if (!(mf.mqtt_subfunction_)(message)) {
                LOG_ERROR(F("error: invalid payload %s for this topic %s"), message, topic);
                if (send_response_) {
                    Mqtt::publish(F_(response), "error: invalid data");
                }
            }
            return;
        }
    }

    StaticJsonDocument<EMSESP_JSON_SIZE_SMALL>     input_doc;
    StaticJsonDocument<EMSESP_JSON_SIZE_LARGE_DYN> output_doc;
    JsonObject                                     input, output;

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
            Mqtt::publish(F_(response), error);
        }
    } else {
        // all good, send back json output from call
        if (send_response_) {
            Mqtt::publish(F_(response), output);
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

    // shell.print(F(" Subscribed MQTT topics: "));
    // for (const auto & mqtt_subfunction : mqtt_subfunctions_) {
    //     if (mqtt_subfunction.device_type_ == device_type) {
    //         shell.printf(F("%s "), mqtt_subfunction.topic_.c_str());
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

        // convert to milliseconds
        publish_time_boiler_     = mqttSettings.publish_time_boiler * 1000;
        publish_time_thermostat_ = mqttSettings.publish_time_thermostat * 1000;
        publish_time_solar_      = mqttSettings.publish_time_solar * 1000;
        publish_time_mixer_      = mqttSettings.publish_time_mixer * 1000;
        publish_time_other_      = mqttSettings.publish_time_other * 1000;
        publish_time_sensor_     = mqttSettings.publish_time_sensor * 1000;
    });

    // get a local copy of the system hostname
    system_hostname_ = EMSESP::system_.hostname();
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
            LOG_WARNING(F("MQTT disconnected: TCP"));
        } else if (reason == AsyncMqttClientDisconnectReason::MQTT_IDENTIFIER_REJECTED) {
            LOG_WARNING(F("MQTT disconnected: Identifier Rejected"));
        } else if (reason == AsyncMqttClientDisconnectReason::MQTT_SERVER_UNAVAILABLE) {
            LOG_WARNING(F("MQTT disconnected: Server unavailable"));
        } else if (reason == AsyncMqttClientDisconnectReason::MQTT_MALFORMED_CREDENTIALS) {
            LOG_WARNING(F("MQTT disconnected: Malformed credentials"));
        } else if (reason == AsyncMqttClientDisconnectReason::MQTT_NOT_AUTHORIZED) {
            LOG_WARNING(F("MQTT disconnected: Not authorized"));
        } else {
            LOG_WARNING(F("MQTT disconnected: code %d"), reason);
        }
    });

    // create will_topic with the base prefixed. It has to be static because asyncmqttclient destroys the reference
    static char will_topic[MQTT_TOPIC_MAX_SIZE];
    snprintf(will_topic, MQTT_TOPIC_MAX_SIZE, "%s/status", mqtt_base_.c_str());
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
    StaticJsonDocument<EMSESP_JSON_SIZE_MEDIUM> doc;
    // first time to connect
    if (connectcount_ == 1) {
        doc["event"] = FJSON("start");
    } else {
        doc["event"] = FJSON("reconnect");
    }

    doc["version"] = EMSESP_APP_VERSION;
#ifndef EMSESP_STANDALONE
    if (WiFi.status() == WL_CONNECTED) {
        doc["connection"]      = F("WiFi");
        doc["hostname"]        = WiFi.getHostname();
        doc["SSID"]            = WiFi.SSID();
        doc["BSSID"]           = WiFi.BSSIDstr();
        doc["RSSI"]            = WiFi.RSSI();
        doc["MAC"]             = WiFi.macAddress();
        doc["IPv4 address"]    = uuid::printable_to_string(WiFi.localIP()) + "/" + uuid::printable_to_string(WiFi.subnetMask());
        doc["IPv4 gateway"]    = uuid::printable_to_string(WiFi.gatewayIP());
        doc["IPv4 nameserver"] = uuid::printable_to_string(WiFi.dnsIP());
        if (WiFi.localIPv6().toString() != "0000:0000:0000:0000:0000:0000:0000:0000") {
            doc["IPv6 address"] = uuid::printable_to_string(WiFi.localIPv6());
        }
    } else if (EMSESP::system_.ethernet_connected()) {
        doc["connection"]      = F("Ethernet");
        doc["hostname"]        = ETH.getHostname();
        doc["MAC"]             = ETH.macAddress();
        doc["IPv4 address"]    = uuid::printable_to_string(ETH.localIP()) + "/" + uuid::printable_to_string(ETH.subnetMask());
        doc["IPv4 gateway"]    = uuid::printable_to_string(ETH.gatewayIP());
        doc["IPv4 nameserver"] = uuid::printable_to_string(ETH.dnsIP());
        if (ETH.localIPv6().toString() != "0000:0000:0000:0000:0000:0000:0000:0000") {
            doc["IPv6 address"] = uuid::printable_to_string(ETH.localIPv6());
        }
    }
#endif
    publish(F_(info), doc.as<JsonObject>()); // topic called "info"

    if (ha_enabled_) {
        LOG_INFO(F("start removing topics %s/+/%s/#"), discovery_prefix_.c_str(), system_hostname_.c_str());
        queue_unsubscribe_message(discovery_prefix_ + "/climate/" + system_hostname_ + "/#");
        queue_unsubscribe_message(discovery_prefix_ + "/sensor/" + system_hostname_ + "/#");
        queue_unsubscribe_message(discovery_prefix_ + "/binary_sensor/" + system_hostname_ + "/#");
        queue_unsubscribe_message(discovery_prefix_ + "/number/" + system_hostname_ + "/#");
        queue_unsubscribe_message(discovery_prefix_ + "/select/" + system_hostname_ + "/#");
        queue_unsubscribe_message(discovery_prefix_ + "/switch/" + system_hostname_ + "/#");
        EMSESP::reset_mqtt_ha(); // re-create all HA devices if there are any
        ha_status();             // create the EMS-ESP device in HA, which is MQTT retained
        ha_climate_reset(true);
    } else {
        // TODO commented out - not sure why we need to subscribe if not using discovery. Check with Michael!
        /*
        queue_subscribe_message(discovery_prefix_ + "/climate/" + system_hostname_ + "/#");
        queue_subscribe_message(discovery_prefix_ + "/sensor/" + system_hostname_ + "/#");
        queue_subscribe_message(discovery_prefix_ + "/binary_sensor/" + system_hostname_ + "/#");
        queue_subscribe_message(discovery_prefix_ + "/number/" + system_hostname_ + "/#");
        queue_subscribe_message(discovery_prefix_ + "/select/" + system_hostname_ + "/#");
        queue_subscribe_message(discovery_prefix_ + "/switch/" + system_hostname_ + "/#");
        */
    }

    // send initial MQTT messages for some of our services
    EMSESP::shower_.set_shower_state(false, true); // Send shower_activated as false
    EMSESP::system_.send_heartbeat();              // send heartbeat

    // re-subscribe to all custom registered MQTT topics
    resubscribe();

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
    doc["stat_t"]    = FJSON("~/heartbeat");
    doc["object_id"] = FJSON("ems_esp_status");
    doc["name"]      = FJSON("EMS-ESP status");
    doc["ic"]        = F_(icondevice);
    doc["val_tpl"]   = FJSON("{{value_json['bus_status']}}");

    JsonObject dev = doc.createNestedObject("dev");
    dev["name"]    = F_(EMSESP); // "EMS-ESP"
    dev["sw"]      = "v" + std::string(EMSESP_APP_VERSION);
    dev["mf"]      = FJSON("proddy");
    dev["mdl"]     = F_(EMSESP); // "EMS-ESP"
    JsonArray ids  = dev.createNestedArray("ids");
    ids.add("ems-esp");

    char topic[MQTT_TOPIC_MAX_SIZE];
    snprintf(topic, sizeof(topic), "sensor/%s/system/config", system_hostname_.c_str());
    Mqtt::publish_ha(topic, doc.as<JsonObject>()); // publish the config payload with retain flag

    // create the sensors - must match the MQTT payload keys
    if (!EMSESP::system_.ethernet_connected()) {
        publish_system_ha_sensor_config(DeviceValueType::INT, F("WiFi RSSI"), F("rssi"), DeviceValueUOM::DBM);
        publish_system_ha_sensor_config(DeviceValueType::INT, F("WiFi strength"), F("wifistrength"), DeviceValueUOM::PERCENT);
    }
    publish_system_ha_sensor_config(DeviceValueType::INT, F("Uptime"), F("uptime"), DeviceValueUOM::NONE);
    publish_system_ha_sensor_config(DeviceValueType::INT, F("Uptime (sec)"), F("uptime_sec"), DeviceValueUOM::SECONDS);
    publish_system_ha_sensor_config(DeviceValueType::BOOL, F("NTP status"), F("ntp_status"), DeviceValueUOM::NONE);
    publish_system_ha_sensor_config(DeviceValueType::INT, F("Free memory"), F("freemem"), DeviceValueUOM::KB);
    publish_system_ha_sensor_config(DeviceValueType::INT, F("MQTT fails"), F("mqttfails"), DeviceValueUOM::NONE);
    publish_system_ha_sensor_config(DeviceValueType::INT, F("Rx received"), F("rxreceived"), DeviceValueUOM::NONE);
    publish_system_ha_sensor_config(DeviceValueType::INT, F("Rx fails"), F("rxfails"), DeviceValueUOM::NONE);
    publish_system_ha_sensor_config(DeviceValueType::INT, F("Tx reads"), F("txreads"), DeviceValueUOM::NONE);
    publish_system_ha_sensor_config(DeviceValueType::INT, F("Tx writes"), F("txwrites"), DeviceValueUOM::NONE);
    publish_system_ha_sensor_config(DeviceValueType::INT, F("Tx fails"), F("txfails"), DeviceValueUOM::NONE);
}

// add sub or pub task to the queue.
// returns a pointer to the message created
// the base is not included in the topic
std::shared_ptr<const MqttMessage> Mqtt::queue_message(const uint8_t operation, const std::string & topic, const std::string & payload, bool retain) {
    if (topic.empty()) {
        return nullptr;
    }

    // take the topic and prefix the base, unless its for HA
    std::shared_ptr<MqttMessage> message;
    message = std::make_shared<MqttMessage>(operation, topic, payload, retain);

#ifdef EMSESP_DEBUG
    if (operation == Operation::PUBLISH) {
        if (message->payload.empty()) {
            LOG_INFO("[DEBUG] Adding to queue: (Publish) topic='%s' empty payload", message->topic.c_str());
        } else {
            LOG_INFO("[DEBUG] Adding to queue: (Publish) topic='%s' payload=%s", message->topic.c_str(), message->payload.c_str());
        }
    } else {
        LOG_INFO("[DEBUG] Adding to queue: (Subscribe) topic='%s'", message->topic.c_str());
    }
#endif

    // if the queue is full, make room but removing the last one
    if (mqtt_messages_.size() >= MAX_MQTT_MESSAGES) {
        mqtt_messages_.pop_front();
        LOG_WARNING(F("Queue overflow, removing one message"));
        mqtt_publish_fails_++;
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

// add MQTT unsubscribe message to queue
std::shared_ptr<const MqttMessage> Mqtt::queue_unsubscribe_message(const std::string & topic) {
    return queue_message(Operation::UNSUBSCRIBE, topic, "", false); // no payload
}

// MQTT Publish, using a user's retain flag
void Mqtt::publish(const std::string & topic, const std::string & payload) {
    queue_publish_message(topic, payload, mqtt_retain_);
}

// MQTT Publish, using a user's retain flag - except for char * strings
void Mqtt::publish(const __FlashStringHelper * topic, const char * payload) {
    queue_publish_message(read_flash_string(topic), payload, mqtt_retain_);
}

// MQTT Publish, using a specific retain flag, topic is a flash string
void Mqtt::publish(const __FlashStringHelper * topic, const std::string & payload) {
    queue_publish_message(read_flash_string(topic), payload, mqtt_retain_);
}

void Mqtt::publish(const __FlashStringHelper * topic, const JsonObject & payload) {
    publish(read_flash_string(topic), payload);
}

// publish json doc, only if its not empty
void Mqtt::publish(const std::string & topic, const JsonObject & payload) {
    publish_retain(topic, payload, mqtt_retain_);
}

// MQTT Publish, using a specific retain flag, topic is a flash string, forcing retain flag
void Mqtt::publish_retain(const __FlashStringHelper * topic, const std::string & payload, bool retain) {
    queue_publish_message(read_flash_string(topic), payload, retain);
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
    publish_retain(read_flash_string(topic), payload, retain);
}

void Mqtt::publish_ha(const __FlashStringHelper * topic, const JsonObject & payload) {
    publish_ha(read_flash_string(topic), payload);
}

// publish empty payload to remove the topic
void Mqtt::publish_ha(const std::string & topic) {
    if (!enabled()) {
        return;
    }

    std::string fulltopic = Mqtt::discovery_prefix() + topic;
#if defined(EMSESP_DEBUG)
    LOG_DEBUG(F("[DEBUG] Publishing empty HA topic=%s"), fulltopic.c_str());
#endif

    queue_publish_message(fulltopic, "", true); // publish with retain to remove from broker
}

// publish a Home Assistant config topic and payload, with retain flag off.
void Mqtt::publish_ha(const std::string & topic, const JsonObject & payload) {
    if (!enabled()) {
        return;
    }

    std::string payload_text;
    payload_text.reserve(measureJson(payload) + 1);
    serializeJson(payload, payload_text); // convert json to string

    std::string fulltopic = Mqtt::discovery_prefix() + topic;
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

    if (message->topic.find(discovery_prefix_) == 0) {
        strlcpy(topic, message->topic.c_str(), sizeof(topic)); // leave topic as it is
    } else {
        snprintf(topic, MQTT_TOPIC_MAX_SIZE, "%s/%s", mqtt_base_.c_str(), message->topic.c_str()); // uses base
    }

    // if this has already been published and we're waiting for an ACK, don't publish again
    // it will have a real packet ID
    if (mqtt_message.packet_id_ > 0) {
#if defined(EMSESP_DEBUG)
        LOG_DEBUG(F("[DEBUG] Waiting for QOS-ACK"));
#endif
        // if we don't get the ack within 10 minutes, republish with new packet_id
        if (uuid::get_uptime_sec() - last_publish_queue_ < 600) {
            return;
        }
    }
    last_publish_queue_ = uuid::get_uptime_sec();

    // if we're subscribing...
    if (message->operation == Operation::SUBSCRIBE) {
        LOG_DEBUG(F("Subscribing to topic '%s'"), topic);
        uint16_t packet_id = mqttClient_->subscribe(topic, mqtt_qos_);
        if (!packet_id) {
            if (++mqtt_messages_.front().retry_count_ < MQTT_PUBLISH_MAX_RETRY) {
                return;
            }
            LOG_ERROR(F("Error subscribing to topic '%s'"), topic);
            mqtt_publish_fails_++; // increment failure counter
        }

        mqtt_messages_.pop_front(); // remove the message from the queue

        return;
    }

    // if we're unsubscribing...
    if (message->operation == Operation::UNSUBSCRIBE) {
        LOG_DEBUG(F("Subscribing to topic '%s'"), topic);
        uint16_t packet_id = mqttClient_->unsubscribe(topic);
        if (!packet_id) {
            if (++mqtt_messages_.front().retry_count_ < MQTT_PUBLISH_MAX_RETRY) {
                return;
            }
            LOG_ERROR(F("Error unsubscribing to topic '%s'"), topic);
            mqtt_publish_fails_++; // increment failure counter
        }

        mqtt_messages_.pop_front(); // remove the message from the queue

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

// create's a ha sensor config topic from a device value object
// and also takes a flag (create_device_config) used to also create the main HA device config. This is only needed for one entity
void Mqtt::publish_ha_sensor_config(DeviceValue & dv, const std::string & model, const std::string & brand, const bool remove, const bool create_device_config) {
    StaticJsonDocument<EMSESP_JSON_SIZE_HA_CONFIG> dev_json;

    // always create the ids
    JsonArray   ids = dev_json.createNestedArray("ids");
    char        ha_device[40];
    std::string device_type_name = EMSdevice::device_type_2_device_name(dv.device_type);
    snprintf(ha_device, sizeof(ha_device), "ems-esp-%s", device_type_name.c_str());
    ids.add(ha_device);

    if (create_device_config) {
        device_type_name[0]    = toupper(device_type_name[0]); // capitalize
        dev_json["name"]       = "EMS-ESP " + device_type_name;
        dev_json["mf"]         = brand;
        dev_json["mdl"]        = model;
        dev_json["via_device"] = "ems-esp";
    }

    // calculate the min and max
    int16_t dv_set_min;
    int16_t dv_set_max;
    (void)dv.get_min_max(dv_set_min, dv_set_max);

    // determine if we're creating the command topics which we use special HA configs
    // unless the entity has been marked as read-only and so it'll default to using the sensor/ type
    bool has_cmd = dv.has_cmd && !dv.has_state(DeviceValueState::DV_READONLY);

    publish_ha_sensor_config(dv.type,
                             dv.tag,
                             Helpers::translated_fword(dv.fullname),
                             dv.device_type,
                             dv.short_name,
                             dv.uom,
                             remove,
                             has_cmd,
                             dv.options,
                             dv.options_size,
                             dv_set_min,
                             dv_set_max,
                             dev_json.as<JsonObject>());
}

// publish HA sensor for System using the heartbeat tag
void Mqtt::publish_system_ha_sensor_config(uint8_t type, const __FlashStringHelper * name, const __FlashStringHelper * entity, const uint8_t uom) {
    StaticJsonDocument<EMSESP_JSON_SIZE_HA_CONFIG> doc;
    JsonObject                                     dev_json = doc.createNestedObject("dev");

    JsonArray ids = dev_json.createNestedArray("ids");
    ids.add("ems-esp");

    publish_ha_sensor_config(type, DeviceValueTAG::TAG_HEARTBEAT, name, EMSdevice::DeviceType::SYSTEM, entity, uom, false, false, nullptr, 0, 0, 0, dev_json);
}

// MQTT discovery configs
// entity must match the key/value pair in the *_data topic
// note: some extra string copying done here, it looks messy but does help with heap fragmentation issues
void Mqtt::publish_ha_sensor_config(uint8_t                              type,        // EMSdevice::DeviceValueType
                                    uint8_t                              tag,         // EMSdevice::DeviceValueTAG
                                    const __FlashStringHelper * const    fullname,    // fullname, already translated
                                    const uint8_t                        device_type, // EMSdevice::DeviceType
                                    const __FlashStringHelper * const    entity,      // same as shortname
                                    const uint8_t                        uom,         // EMSdevice::DeviceValueUOM (0=NONE)
                                    const bool                           remove,      // true if we want to remove this topic
                                    const bool                           has_cmd,
                                    const __FlashStringHelper * const ** options,
                                    uint8_t                              options_size,
                                    const int16_t                        dv_set_min,
                                    const int16_t                        dv_set_max,
                                    const JsonObject &                   dev_json) {
    // ignore if name (fullname) is empty
    if (fullname == nullptr) {
        return;
    }

    // create the device name
    char device_name[50];
    strlcpy(device_name, EMSdevice::device_type_2_device_name(device_type).c_str(), sizeof(device_name));

    // create entity by add the hc/wwc tag if present, separating with a .
    char new_entity[50];
    if (tag >= DeviceValueTAG::TAG_HC1) {
        snprintf(new_entity, sizeof(new_entity), "%s.%s", EMSdevice::tag_to_string(tag).c_str(), read_flash_string(entity).c_str());
    } else {
        snprintf(new_entity, sizeof(new_entity), "%s", read_flash_string(entity).c_str());
    }

    // build unique identifier which will be used in the topic, replacing all . with _ as not to break HA
    char uniq[101];
    snprintf(uniq, sizeof(uniq), "%s_%s", device_name, new_entity);
    Helpers::replace_char(uniq, '.', '_');

    // use_ha_sensor is true if we're using the Sensor Entity https://developers.home-assistant.io/docs/core/entity/sensor
    bool use_ha_sensor = false;

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
            // number - https://www.home-assistant.io/integrations/number.mqtt
            // https://developers.home-assistant.io/docs/core/entity/number

            snprintf(topic, sizeof(topic), "number/%s/%s/config", system_hostname_.c_str(), uniq);
            break;
        case DeviceValueType::BOOL:
            // switch - https://www.home-assistant.io/integrations/switch.mqtt
            snprintf(topic, sizeof(topic), "switch/%s/%s/config", system_hostname_.c_str(), uniq);
            break;
        case DeviceValueType::ENUM:
            // select - https://www.home-assistant.io/integrations/select.mqtt
            snprintf(topic, sizeof(topic), "select/%s/%s/config", system_hostname_.c_str(), uniq);
            break;
        default:
            // plain old sensor
            snprintf(topic, sizeof(topic), "sensor/%s/%s/config", system_hostname_.c_str(), uniq);
            break;
        }
    } else {
        // plain old read only device entity
        if (type == DeviceValueType::BOOL) {
            snprintf(topic, sizeof(topic), "binary_sensor/%s/%s/config", system_hostname_.c_str(), uniq); // binary sensor
        } else {
            use_ha_sensor = true;
            snprintf(topic, sizeof(topic), "sensor/%s/%s/config", system_hostname_.c_str(), uniq); // normal HA sensor, not a boolean one
        }
    }

    // if we're asking to remove this topic, send an empty payload and exit
    // https://github.com/emsesp/EMS-ESP32/issues/196
    if (remove) {
        LOG_DEBUG(F("Removing HA config for %s"), uniq);
        publish_ha(topic);
        return;
    }

    bool have_tag = !EMSdevice::tag_to_string(tag).empty();

    // build the payload
    DynamicJsonDocument doc(EMSESP_JSON_SIZE_HA_CONFIG);
    doc["~"]       = mqtt_base_;
    doc["uniq_id"] = uniq;

    const char * ic_ha  = "ic";           // icon - only set this if there is no device class
    const char * sc_ha  = "state_class";  // state class
    const char * uom_ha = "unit_of_meas"; // unit of measure

    // handle commands, which are device entities that are writable
    // we add the command topic parameter
    // note: there is no way to handle strings in HA so datetimes (e.g. set_datetime, set_holiday, set_wwswitchtime etc) are excluded
    if (has_cmd) {
        // command topic back to EMS-ESP
        char command_topic[105];
        snprintf(command_topic, sizeof(command_topic), "~/%s", uniq);
        Helpers::replace_char(command_topic, '_', '/');
        doc["command_topic"] = command_topic;

        // for enums, add options
        if (type == DeviceValueType::ENUM) {
            JsonArray option_list = doc.createNestedArray("options");
            for (uint8_t i = 0; i < options_size; i++) {
                option_list.add(Helpers::translated_word(options[i]));
            }
        } else if (type != DeviceValueType::STRING) {
            // Must be Numeric....
            // mode can be auto, slider or box. Because its fiddly and error prone, force conversion to box
            // but... this is not currently supported in HA MQTT Number yet!
            // doc["mode"] = "box";
        }

        // set min and max values, if we have a valid range
        if (dv_set_min != 0 || dv_set_max != 0) {
            doc["min"] = dv_set_min;
            doc["max"] = dv_set_max;
            if ((uom == DeviceValueUOM::DEGREES) || (uom == DeviceValueUOM::DEGREES_R)) {
                doc["step"] = 0.5;
            }
        }

        // set icons
        // since these don't have a device class we need to add the icon ourselves
        switch (uom) {
        case DeviceValueUOM::DEGREES:
        case DeviceValueUOM::DEGREES_R:
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
    snprintf(stat_t, sizeof(stat_t), "~/%s", tag_to_topic(device_type, tag).c_str());
    doc["stat_t"] = stat_t;

    // friendly name = <tag> <name>
    char ha_name[70];
    if (have_tag) {
        snprintf(ha_name, sizeof(ha_name), "%s %s", EMSdevice::tag_to_string(tag).c_str(), read_flash_string(fullname).c_str());
    } else {
        snprintf(ha_name, sizeof(ha_name), "%s", read_flash_string(fullname).c_str());
    }
    ha_name[0]  = toupper(ha_name[0]); // capitalize first letter
    doc["name"] = ha_name;

    // entity id is generated from the name, see https://www.home-assistant.io/docs/mqtt/discovery/#use-object_id-to-influence-the-entity-id
    // so we override it to make it unique using entity_id
    // emsesp_<device>_<tag>_<name>
    char object_id[130];
    snprintf(object_id, sizeof(object_id), "emsesp_%s_%s", device_name, ha_name);
    doc["object_id"] = object_id;

    // value template
    // if its nested mqtt format then use the appended entity name, otherwise take the original
    char val_tpl[75];
    if (is_nested()) {
        snprintf(val_tpl, sizeof(val_tpl), "{{value_json.%s}}", new_entity);
    } else {
        snprintf(val_tpl, sizeof(val_tpl), "{{value_json.%s}}", read_flash_string(entity).c_str());
    }
    doc["val_tpl"] = val_tpl;

    // special case to handle booleans
    // applies to both Binary Sensor (read only) and a Switch (for a command)
    // always render boolean as strings true & false
    // and has no unit of measure or icon
    if (type == DeviceValueType::BOOL) {
        char result[10];
        doc[F("payload_on")]  = Helpers::render_boolean(result, true);
        doc[F("payload_off")] = Helpers::render_boolean(result, false);
        doc[sc_ha]            = F_(measurement);
    } else {
        // always set the uom
        if (uom != DeviceValueUOM::NONE) {
            doc[uom_ha] = EMSdevice::uom_to_string(uom);
        }
    }

    // this next section is building using the Sensor Entity
    // https://developers.home-assistant.io/docs/core/entity/sensor
    // for read only sensors. It uses a device class to determine icon
    // and state class

    if (use_ha_sensor) {
        const char * dc_ha = "device_class"; // device class

        switch (uom) {
        case DeviceValueUOM::DEGREES:
        case DeviceValueUOM::DEGREES_R:
            doc[sc_ha] = F_(measurement);
            doc[dc_ha] = F("temperature"); // no icon needed
            break;
        case DeviceValueUOM::PERCENT:
            doc[sc_ha] = F_(measurement);
            doc[dc_ha] = F("power_factor"); // no icon needed
            break;
        case DeviceValueUOM::SECONDS:
        case DeviceValueUOM::MINUTES:
        case DeviceValueUOM::HOURS:
            doc[ic_ha] = F_(icontime);
            if (type == DeviceValueType::TIME) {
                doc[sc_ha] = F_(total_increasing);
            } else {
                doc[sc_ha] = F_(measurement);
            }
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
            doc[dc_ha] = F("energy"); // no icon needed
            break;
        case DeviceValueUOM::KWH:
            doc[sc_ha] = F_(total_increasing);
            doc[dc_ha] = F("energy"); // no icon needed
            break;
        case DeviceValueUOM::UA:
            doc[ic_ha] = F_(iconua);
            doc[sc_ha] = F_(measurement);
            break;
        case DeviceValueUOM::BAR:
            doc[sc_ha] = F_(measurement);
            doc[dc_ha] = F("pressure");
            break;
        case DeviceValueUOM::W:
        case DeviceValueUOM::KW:
            doc[sc_ha] = F_(measurement);
            doc[dc_ha] = F("power");
            break;
        case DeviceValueUOM::DBM:
            doc[sc_ha] = F_(measurement);
            doc[dc_ha] = F("signal_strength");
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

    // add the dev json object to the end
    doc["dev"] = dev_json;

    publish_ha(topic, doc.as<JsonObject>());
}

void Mqtt::publish_ha_climate_config(uint8_t tag, bool has_roomtemp, bool remove) {
    uint8_t hc_num = tag - DeviceValueTAG::TAG_HC1 + 1;

    char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
    char topic_t[Mqtt::MQTT_TOPIC_MAX_SIZE];
    char hc_mode_s[30];
    char seltemp_s[30];
    char currtemp_s[30];
    char mode_str_tpl[400];
    char name_s[10];
    char id_s[20];
    char uniq_id_s[30];
    char temp_cmd_s[30];
    char mode_cmd_s[30];
    char min_s[10];
    char max_s[10];

    snprintf(topic, sizeof(topic), "climate/%s/thermostat_hc%d/config", Mqtt::base().c_str(), hc_num);
    if (remove) {
        publish_ha(topic); // publish empty payload with retain flag
        return;
    }

    if (Mqtt::is_nested()) {
        // nested format
        snprintf(hc_mode_s, sizeof(hc_mode_s), "value_json.hc%d.mode", hc_num);
        snprintf(seltemp_s, sizeof(seltemp_s), "{{value_json.hc%d.seltemp}}", hc_num);
        if (has_roomtemp) {
            snprintf(currtemp_s, sizeof(currtemp_s), "{{value_json.hc%d.currtemp}}", hc_num);
        }
        snprintf(topic_t, sizeof(topic_t), "~/%s", Mqtt::tag_to_topic(EMSdevice::DeviceType::THERMOSTAT, DeviceValueTAG::TAG_NONE).c_str());
    } else {
        // single format
        snprintf(hc_mode_s, sizeof(hc_mode_s), "value_json.mode");
        snprintf(seltemp_s, sizeof(seltemp_s), "{{value_json.seltemp}}");
        if (has_roomtemp) {
            snprintf(currtemp_s, sizeof(currtemp_s), "{{value_json.currtemp}}");
        }
        snprintf(topic_t, sizeof(topic_t), "~/%s", Mqtt::tag_to_topic(EMSdevice::DeviceType::THERMOSTAT, DeviceValueTAG::TAG_HC1 + hc_num - 1).c_str());
    }

    snprintf(mode_str_tpl,
             sizeof(mode_str_tpl),
             "{%%if %s=='manual'%%}heat{%%elif %s=='day'%%}heat{%%elif %s=='night'%%}off{%%elif %s=='off'%%}off{%%else%%}auto{%%endif%%}",
             hc_mode_s,
             hc_mode_s,
             hc_mode_s,
             hc_mode_s);

    snprintf(id_s, sizeof(id_s), "thermostat_hc%d", hc_num);
    snprintf(name_s, sizeof(name_s), "Hc%d", hc_num);
    snprintf(uniq_id_s, sizeof(uniq_id_s), "thermostat_hc%d", hc_num);
    snprintf(temp_cmd_s, sizeof(temp_cmd_s), "~/thermostat/hc%d/seltemp", hc_num);
    snprintf(mode_cmd_s, sizeof(temp_cmd_s), "~/thermostat/hc%d/mode", hc_num);

    StaticJsonDocument<EMSESP_JSON_SIZE_HA_CONFIG> doc;

    doc["~"]             = base();
    doc["object_id"]     = id_s;
    doc["name"]          = name_s;
    doc["uniq_id"]       = uniq_id_s;
    doc["mode_stat_t"]   = topic_t;
    doc["mode_stat_tpl"] = mode_str_tpl;
    doc["temp_cmd_t"]    = temp_cmd_s;
    doc["temp_stat_t"]   = topic_t;
    doc["temp_stat_tpl"] = seltemp_s;
    doc["mode_cmd_t"]    = mode_cmd_s;

    if (has_roomtemp) {
        doc["curr_temp_t"]   = topic_t;
        doc["curr_temp_tpl"] = currtemp_s;
    }

    doc["min_temp"]  = Helpers::render_value(min_s, 5, 0, EMSESP::system_.fahrenheit() ? 2 : 0);
    doc["max_temp"]  = Helpers::render_value(max_s, 30, 0, EMSESP::system_.fahrenheit() ? 2 : 0);
    doc["temp_step"] = "0.5";

    // the HA climate component only responds to auto, heat and off
    JsonArray modes = doc.createNestedArray("modes");

    modes.add("auto");
    modes.add("heat");
    modes.add("off");

    JsonObject dev = doc.createNestedObject("dev");
    JsonArray  ids = dev.createNestedArray("ids");
    ids.add("ems-esp-thermostat");

    publish_ha(topic, doc.as<JsonObject>()); // publish the config payload with retain flag
}

// based on the device and tag, create the MQTT topic name (without the basename)
// differs based on whether MQTT nested is enabled
// tag = EMSdevice::DeviceValueTAG
std::string Mqtt::tag_to_topic(uint8_t device_type, uint8_t tag) {
    // the system device is treated differently. The topic is 'heartbeat' and doesn't follow the usual convention
    if (device_type == EMSdevice::DeviceType::SYSTEM) {
        return EMSdevice::tag_to_mqtt(tag);
    }

    // if there is a tag add it
    if (!EMSdevice::tag_to_mqtt(tag).empty() && ((tag == DeviceValueTAG::TAG_BOILER_DATA_WW) || (!is_nested() && tag >= DeviceValueTAG::TAG_HC1))) {
        return EMSdevice::device_type_2_device_name(device_type) + "_data_" + EMSdevice::tag_to_mqtt(tag);
    } else {
        return EMSdevice::device_type_2_device_name(device_type) + "_data";
    }
}

} // namespace emsesp
