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

#ifndef EMSESP_MQTT_H_
#define EMSESP_MQTT_H_

#include <Arduino.h>
#include <ArduinoJson.h>

#include <string>
#include <vector>
#include <list>
#include <functional>

#include <AsyncMqttClient.h>

#include "helpers.h"
#include "system.h"
#include "console.h"

#include <uuid/log.h>

using uuid::console::Shell;

#define EMSESP_MAX_JSON_SIZE_SMALL 200  // for smaller json docs when using StaticJsonDocument
#define EMSESP_MAX_JSON_SIZE_MEDIUM 800 // for smaller json docs from ems devices, when using StaticJsonDocument
#define EMSESP_MAX_JSON_SIZE_LARGE 1500 // for large json docs from ems devices, like boiler or thermostat data. Using DynamicJsonDocument

namespace emsesp {

using mqtt_subfunction_p = std::function<void(const char * message)>;
using mqtt_cmdfunction_p = std::function<void(const char * data, const int8_t id)>;

struct MqttMessage {
    ~MqttMessage() = default;

    const uint8_t     operation;
    const std::string topic;
    const std::string payload;
    const bool        retain;

    MqttMessage(const uint8_t operation, const std::string & topic, const std::string && payload, bool retain)
        : operation(operation)
        , topic(topic)
        , payload(payload)
        , retain(retain) {
    }
};

class Mqtt {
  public:
    void loop();
    void start();

    void set_publish_time_boiler(uint16_t publish_time);
    void set_publish_time_thermostat(uint16_t publish_time);
    void set_publish_time_solar(uint16_t publish_time);
    void set_publish_time_mixing(uint16_t publish_time);
    void set_publish_time_other(uint16_t publish_time);
    void set_publish_time_sensor(uint16_t publish_time);
    void set_qos(uint8_t mqtt_qos);
    bool get_publish_onchange(uint8_t device_type);

    enum Operation { PUBLISH, SUBSCRIBE };

    static constexpr uint8_t MQTT_TOPIC_MAX_SIZE = 100;

    static void subscribe(const uint8_t device_type, const std::string & topic, mqtt_subfunction_p cb);
    static void subscribe(const std::string & topic, mqtt_subfunction_p cb);
    static void resubscribe();

    static void add_command(const uint8_t device_type, const uint8_t device_id, const __FlashStringHelper * cmd, mqtt_cmdfunction_p cb);

    static void publish(const std::string & topic, const std::string & payload, bool retain = false);
    static void publish(const std::string & topic, const JsonDocument & payload, bool retain = false);
    static void publish(const std::string & topic, const bool value);
    static void publish(const std::string & topic);

    static void show_topic_handlers(uuid::console::Shell & shell, const uint8_t device_type);
    static void show_mqtt(uuid::console::Shell & shell);

    static void on_connect();

    static bool call_command(const uint8_t device_type, const char * cmd, const char * value, const int8_t id);

    void disconnect() {
        mqttClient_->disconnect();
    }

#if defined(EMSESP_DEBUG)
    void incoming(const char * topic, const char * payload); // for testing only
#endif

    static bool connected() {
        return mqttClient_->connected();
    }

    static uint32_t publish_fails() {
        return mqtt_publish_fails_;
    }

    static void reset_publish_fails() {
        mqtt_publish_fails_ = 0;
    }

    struct MQTTCmdFunction {
        uint8_t                     device_type_;
        uint8_t                     device_id_;
        const __FlashStringHelper * cmd_;
        mqtt_cmdfunction_p          mqtt_cmdfunction_;

        MQTTCmdFunction(uint8_t device_type, uint8_t device_id, const __FlashStringHelper * cmd, mqtt_cmdfunction_p mqtt_cmdfunction)
            : device_type_(device_type)
            , device_id_(device_id)
            , cmd_(cmd)
            , mqtt_cmdfunction_(mqtt_cmdfunction) {
        }
    };

    static std::vector<MQTTCmdFunction> commands() {
        return mqtt_cmdfunctions_;
    }

  private:
    static uuid::log::Logger logger_;

    class QueuedMqttMessage {
      public:
        const uint16_t                           id_;
        const std::shared_ptr<const MqttMessage> content_;
        uint8_t                                  retry_count_;
        uint16_t                                 packet_id_;

        ~QueuedMqttMessage() = default;
        QueuedMqttMessage(uint16_t id, std::shared_ptr<MqttMessage> && content)
            : id_(id)
            , content_(std::move(content)) {
            retry_count_ = 0;
            packet_id_   = 0;
        }
    };
    static std::list<QueuedMqttMessage> mqtt_messages_;

    static AsyncMqttClient * mqttClient_;

    static size_t   maximum_mqtt_messages_;
    static uint16_t mqtt_message_id_;
    static bool     mqtt_retain_;

    static constexpr size_t   MAX_MQTT_MESSAGES      = 20;  // size of queue
    static constexpr uint32_t MQTT_PUBLISH_WAIT      = 200; // delay between sending publishes, to account for large payloads
    static constexpr uint8_t  MQTT_PUBLISH_MAX_RETRY = 3;   // max retries for giving up on publishing

    static std::shared_ptr<const MqttMessage> queue_message(const uint8_t operation, const std::string & topic, const std::string & payload, const bool retain);
    static std::shared_ptr<const MqttMessage> queue_publish_message(const std::string & topic, const std::string & payload, const bool retain);
    static std::shared_ptr<const MqttMessage> queue_subscribe_message(const std::string & topic);

    void on_publish(uint16_t packetId);
    void on_message(const char * topic, const char * payload, size_t len);
    void process_queue();

    static uint16_t mqtt_publish_fails_;

    // function handlers for MQTT subscriptions
    struct MQTTSubFunction {
        uint8_t            device_type_;      // which device type, from DeviceType::
        const std::string  topic_;            // short topic name
        const std::string  full_topic_;       // the fully qualified topic name, usually with the hostname prefixed
        mqtt_subfunction_p mqtt_subfunction_; // can be empty

        MQTTSubFunction(uint8_t device_type, const std::string && topic, const std::string && full_topic, mqtt_subfunction_p mqtt_subfunction)
            : device_type_(device_type)
            , topic_(topic)
            , full_topic_(full_topic)
            , mqtt_subfunction_(mqtt_subfunction) {
        }
    };

    static std::vector<MQTTSubFunction> mqtt_subfunctions_; // list of mqtt subscribe callbacks for all devices
    static std::vector<MQTTCmdFunction> mqtt_cmdfunctions_; // list of commands

    uint32_t last_mqtt_poll_          = 0;
    uint32_t last_publish_boiler_     = 0;
    uint32_t last_publish_thermostat_ = 0;
    uint32_t last_publish_solar_      = 0;
    uint32_t last_publish_mixing_     = 0;
    uint32_t last_publish_other_      = 0;
    uint32_t last_publish_sensor_     = 0;

    // settings, copied over
    static std::string hostname_;
    static uint8_t     mqtt_qos_;
    static uint32_t    publish_time_;
    static uint8_t     bus_id_;
    static uint32_t    publish_time_boiler_;
    static uint32_t    publish_time_thermostat_;
    static uint32_t    publish_time_solar_;
    static uint32_t    publish_time_mixing_;
    static uint32_t    publish_time_other_;
    static uint32_t    publish_time_sensor_;
};

} // namespace emsesp

#endif
