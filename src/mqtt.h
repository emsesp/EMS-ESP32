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
#include "command.h"

#include <uuid/log.h>

using uuid::console::Shell;

#define MQTT_HA_PUBLISH_DELAY 50

namespace emsesp {

using mqtt_subfunction_p = std::function<bool(const char * message)>;
using cmdfunction_p      = std::function<bool(const char * data, const int8_t id)>;

struct MqttMessage {
    const uint8_t     operation;
    const std::string topic;
    const std::string payload;
    const bool        retain;

    MqttMessage(const uint8_t operation, const std::string & topic, const std::string & payload, bool retain)
        : operation(operation)
        , topic(topic)
        , payload(payload)
        , retain(retain) {
    }
    ~MqttMessage() = default;
};

class Mqtt {
  public:
    void loop();
    void start();

    void set_publish_time_boiler(uint16_t publish_time);
    void set_publish_time_thermostat(uint16_t publish_time);
    void set_publish_time_solar(uint16_t publish_time);
    void set_publish_time_mixer(uint16_t publish_time);
    void set_publish_time_other(uint16_t publish_time);
    void set_publish_time_sensor(uint16_t publish_time);
    void set_qos(uint8_t mqtt_qos);
    void set_retain(bool mqtt_retain);
    void set_format(uint8_t mqtt_format);
    bool get_publish_onchange(uint8_t device_type);

    enum Operation { PUBLISH, SUBSCRIBE };

    enum Format : uint8_t { NONE = 0, SINGLE, NESTED, HA };

    static constexpr uint8_t MQTT_TOPIC_MAX_SIZE = 128; // note this should really match the user setting in mqttSettings.maxTopicLength

    static void on_connect();

    static void subscribe(const uint8_t device_type, const std::string & topic, mqtt_subfunction_p cb);
    static void subscribe(const std::string & topic, mqtt_subfunction_p cb);
    static void resubscribe();

    static void publish(const std::string & topic, const std::string & payload);
    static void publish(const __FlashStringHelper * topic, const char * payload);
    static void publish(const std::string & topic, const JsonObject & payload);
    static void publish(const __FlashStringHelper * topic, const JsonObject & payload);
    static void publish(const __FlashStringHelper * topic, const std::string & payload);
    static void publish(const std::string & topic);
    static void publish_retain(const std::string & topic, const JsonObject & payload, bool retain);
    static void publish_retain(const __FlashStringHelper * topic, const std::string & payload, bool retain);
    static void publish_retain(const __FlashStringHelper * topic, const JsonObject & payload, bool retain);
    static void publish_ha(const std::string & topic, const JsonObject & payload);
    static void publish_ha(const __FlashStringHelper * topic, const JsonObject & payload);

    static void register_mqtt_ha_binary_sensor(const __FlashStringHelper * name, const uint8_t device_type, const char * entity);
    static void register_mqtt_ha_sensor(const char *                prefix,
                                        const __FlashStringHelper * suffix,
                                        const __FlashStringHelper * name,
                                        const uint8_t               device_type,
                                        const char *                entity,
                                        const __FlashStringHelper * uom,
                                        const __FlashStringHelper * icon);
    static void register_command(const uint8_t device_type, const uint8_t device_id, const __FlashStringHelper * cmd, cmdfunction_p cb);

    static void show_topic_handlers(uuid::console::Shell & shell, const uint8_t device_type);
    static void show_mqtt(uuid::console::Shell & shell);

    static void ha_status();

    void disconnect() {
        mqttClient_->disconnect();
    }

    void incoming(const char * topic, const char * payload); // for testing only

    static bool connected() {
#if defined(EMSESP_STANDALONE)
        return true;
#else
        return mqttClient_->connected();
#endif
    }

    static bool enabled() {
        return mqtt_enabled_;
    }

    static uint32_t publish_fails() {
        return mqtt_publish_fails_;
    }

    static void reset_publish_fails() {
        mqtt_publish_fails_ = 0;
    }

    static uint8_t mqtt_format() {
        return mqtt_format_;
    }

    static AsyncMqttClient * client() {
        return mqttClient_;
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
    static uint16_t          mqtt_message_id_;

#if defined(EMSESP_STANDALONE)
    static constexpr size_t MAX_MQTT_MESSAGES = 70; // size of queue
#elif defined(ESP32)
    static constexpr size_t MAX_MQTT_MESSAGES = 100; // size of queue
#else
    static constexpr size_t MAX_MQTT_MESSAGES = 20; // size of queue
#endif

    static constexpr uint32_t MQTT_PUBLISH_WAIT      = 200; // delay between sending publishes, to account for large payloads
    static constexpr uint8_t  MQTT_PUBLISH_MAX_RETRY = 3;   // max retries for giving up on publishing

    static std::shared_ptr<const MqttMessage> queue_message(const uint8_t operation, const std::string & topic, const std::string & payload, bool retain);
    static std::shared_ptr<const MqttMessage> queue_publish_message(const std::string & topic, const std::string & payload, bool retain);
    static std::shared_ptr<const MqttMessage> queue_subscribe_message(const std::string & topic);

    void on_publish(uint16_t packetId);
    void on_message(const char * topic, const char * payload, size_t len);
    void process_queue();

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

    uint32_t last_mqtt_poll_          = 0;
    uint32_t last_publish_boiler_     = 0;
    uint32_t last_publish_thermostat_ = 0;
    uint32_t last_publish_solar_      = 0;
    uint32_t last_publish_mixer_      = 0;
    uint32_t last_publish_other_      = 0;
    uint32_t last_publish_sensor_     = 0;

    static bool     connecting_;
    static bool     initialized_;
    static uint16_t mqtt_publish_fails_;
    static uint8_t  connectcount_;

    // settings, copied over
    static std::string hostname_;
    static uint8_t     mqtt_qos_;
    static bool        mqtt_retain_;
    static uint32_t    publish_time_;
    static uint32_t    publish_time_boiler_;
    static uint32_t    publish_time_thermostat_;
    static uint32_t    publish_time_solar_;
    static uint32_t    publish_time_mixer_;
    static uint32_t    publish_time_other_;
    static uint32_t    publish_time_sensor_;
    static uint8_t     mqtt_format_;
    static bool        mqtt_enabled_;
};

} // namespace emsesp

#endif
