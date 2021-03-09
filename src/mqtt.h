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
#include <deque>
#include <functional>

#include <AsyncMqttClient.h>

#include "helpers.h"
#include "system.h"
#include "console.h"
#include "command.h"

#include <uuid/log.h>

using uuid::console::Shell;

// time between HA publishes
#define MQTT_HA_PUBLISH_DELAY 50

// size of queue
#define MAX_MQTT_MESSAGES 200

enum { BOOL_FORMAT_ONOFF = 1, BOOL_FORMAT_TRUEFALSE, BOOL_FORMAT_10, BOOL_FORMAT_ONOFF_CAP }; // matches Web UI settings

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
    bool get_publish_onchange(uint8_t device_type);

    enum Operation { PUBLISH, SUBSCRIBE };

    enum Dallas_Format : uint8_t { SENSORID = 1, NUMBER };
    enum HA_Climate_Format : uint8_t { CURRENT = 1, SETPOINT, ZERO };

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

    static void publish_mqtt_ha_sensor(uint8_t type, uint8_t tag, const __FlashStringHelper * name, const uint8_t device_type, const __FlashStringHelper * entity, const uint8_t uom = 0);
    static void register_command(const uint8_t device_type, const __FlashStringHelper * cmd, cmdfunction_p cb);

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

    static AsyncMqttClient * client() {
        return mqttClient_;
    }

    static bool enabled() {
        return mqtt_enabled_;
    }

    static std::string base() {
        return mqtt_base_;
    }

    static uint32_t publish_fails() {
        return mqtt_publish_fails_;
    }

    static void reset_mqtt();

    static uint8_t ha_climate_format() {
        return ha_climate_format_;
    }

    static uint8_t dallas_format() {
        return dallas_format_;
    }

    static uint8_t bool_format() {
        return bool_format_;
    }

    static bool nested_format() {
        return nested_format_;
    }

    static bool ha_enabled() {
        return ha_enabled_;
    }

    static void ha_climate_format(uint8_t ha_climate_format) {
        ha_climate_format_ = ha_climate_format;
    }

    static void dallas_format(uint8_t dallas_format) {
        dallas_format_ = dallas_format;
    }

    static void ha_enabled(bool ha_enabled) {
        ha_enabled_ = ha_enabled;
    }

    void set_qos(uint8_t mqtt_qos) {
        mqtt_qos_ = mqtt_qos;
    }

    void set_retain(bool mqtt_retain) {
        mqtt_retain_ = mqtt_retain;
    }

    static bool is_empty() {
        return mqtt_messages_.empty();
    }

    struct QueuedMqttMessage {
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
    static std::deque<QueuedMqttMessage> mqtt_messages_;

  private:
    static uuid::log::Logger logger_;

    static AsyncMqttClient * mqttClient_;
    static uint16_t          mqtt_message_id_;

    static constexpr uint32_t MQTT_PUBLISH_WAIT      = 100; // delay between sending publishes, to account for large payloads
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
        mqtt_subfunction_p mqtt_subfunction_; // can be empty

        MQTTSubFunction(uint8_t device_type, const std::string && topic, mqtt_subfunction_p mqtt_subfunction)
            : device_type_(device_type)
            , topic_(topic)
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
    static std::string mqtt_base_;
    static uint8_t     mqtt_qos_;
    static bool        mqtt_retain_;
    static uint32_t    publish_time_;
    static uint32_t    publish_time_boiler_;
    static uint32_t    publish_time_thermostat_;
    static uint32_t    publish_time_solar_;
    static uint32_t    publish_time_mixer_;
    static uint32_t    publish_time_other_;
    static uint32_t    publish_time_sensor_;
    static bool        mqtt_enabled_;
    static uint8_t     dallas_format_;
    static uint8_t     bool_format_;
    static uint8_t     ha_climate_format_;
    static bool        ha_enabled_;
    static bool        nested_format_;
};

} // namespace emsesp

#endif
