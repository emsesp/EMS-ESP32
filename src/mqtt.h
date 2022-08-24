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

#ifndef EMSESP_MQTT_H_
#define EMSESP_MQTT_H_

#include <AsyncMqttClient.h>

#include "helpers.h"
#include "system.h"
#include "console.h"
#include "command.h"
#include "emsdevicevalue.h"

using uuid::console::Shell;

// size of queue
static constexpr uint16_t MAX_MQTT_MESSAGES = 300;

namespace emsesp {

using mqtt_sub_function_p = std::function<bool(const char * message)>;
using cmdfunction_p       = std::function<bool(const char * data, const int8_t id)>;

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

    static void load_settings();

    void set_publish_time_boiler(uint16_t publish_time);
    void set_publish_time_thermostat(uint16_t publish_time);
    void set_publish_time_solar(uint16_t publish_time);
    void set_publish_time_mixer(uint16_t publish_time);
    void set_publish_time_other(uint16_t publish_time);
    void set_publish_time_sensor(uint16_t publish_time);
    bool get_publish_onchange(uint8_t device_type);

    enum Operation : uint8_t { PUBLISH, SUBSCRIBE, UNSUBSCRIBE };
    enum NestedFormat : uint8_t { NESTED = 1, SINGLE };

    static constexpr uint8_t MQTT_TOPIC_MAX_SIZE = 128; // fixed, not a user setting anymore

    static void on_connect();

    static void subscribe(const uint8_t device_type, const std::string & topic, mqtt_sub_function_p cb);
    static void subscribe(const std::string & topic);
    static void resubscribe();

    static void publish(const std::string & topic, const std::string & payload);
    static void publish(const __FlashStringHelper * topic, const char * payload);
    static void publish(const std::string & topic, const JsonObject & payload);
    static void publish(const __FlashStringHelper * topic, const JsonObject & payload);
    static void publish(const __FlashStringHelper * topic, const std::string & payload);
    static void publish_retain(const std::string & topic, const JsonObject & payload, bool retain);
    static void publish_retain(const __FlashStringHelper * topic, const std::string & payload, bool retain);
    static void publish_retain(const __FlashStringHelper * topic, const JsonObject & payload, bool retain);
    static void publish_ha(const std::string & topic, const JsonObject & payload);
    static void publish_ha(const __FlashStringHelper * topic, const JsonObject & payload);
    static void publish_ha(const std::string & topic);

    static void
    publish_ha_sensor_config(DeviceValue & dv, const std::string & model, const std::string & brand, const bool remove, const bool create_device_config = false);
    static void publish_ha_sensor_config(uint8_t                              type,
                                         uint8_t                              tag,
                                         const __FlashStringHelper * const    fullname,
                                         const uint8_t                        device_type,
                                         const __FlashStringHelper * const    entity,
                                         const uint8_t                        uom,
                                         const bool                           remove,
                                         const bool                           has_cmd,
                                         const __FlashStringHelper * const ** options,
                                         uint8_t                              options_size,
                                         const int16_t                        dv_set_min,
                                         const int16_t                        dv_set_max,
                                         const JsonObject &                   dev_json);

    static void publish_system_ha_sensor_config(uint8_t type, const __FlashStringHelper * name, const __FlashStringHelper * entity, const uint8_t uom);
    static void publish_ha_climate_config(uint8_t tag, bool has_roomtemp, bool remove = false);

    static void show_topic_handlers(uuid::console::Shell & shell, const uint8_t device_type);
    static void show_mqtt(uuid::console::Shell & shell);

    static void ha_status();

#if defined(EMSESP_DEBUG)
    void incoming(const char * topic, const char * payload = ""); // for testing only
#endif

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

    static void enabled(bool mqtt_enabled) {
        mqtt_enabled_ = mqtt_enabled;
    }

    static std::string base() {
        return mqtt_base_;
    }

    // returns the discovery MQTT topic prefix and adds a /
    static std::string discovery_prefix() {
        if (discovery_prefix_.empty()) {
            return std::string{};
        }
        return discovery_prefix_ + "/";
    }

    static void base(const char * base) {
        mqtt_base_ = base;
    }

    static uint32_t publish_count() {
        return mqtt_message_id_;
    }

    static uint32_t publish_fails() {
        return mqtt_publish_fails_;
    }

    static void reset_mqtt();

    static bool is_nested() {
        return nested_format_ == NestedFormat::NESTED;
    }

    static void nested_format(uint8_t nested_format) {
        nested_format_ = nested_format;
    }

    static bool publish_single() {
        return publish_single_;
    }

    static bool publish_single2cmd() {
        return publish_single2cmd_;
    }

    static void publish_single(bool publish_single) {
        publish_single_ = publish_single;
    }

    static bool ha_enabled() {
        return ha_enabled_;
    }

    static void ha_enabled(bool ha_enabled) {
        ha_enabled_ = ha_enabled;
    }

    static bool ha_climate_reset() {
        return ha_climate_reset_;
    }

    static void ha_climate_reset(bool reset) {
        ha_climate_reset_ = reset;
    }

    static bool send_response() {
        return send_response_;
    }

    static void send_response(bool send_response) {
        send_response_ = send_response;
    }

    void set_qos(uint8_t mqtt_qos) const {
        mqtt_qos_ = mqtt_qos;
    }

    void set_retain(bool mqtt_retain) const {
        mqtt_retain_ = mqtt_retain;
    }

    static bool is_empty() {
        return mqtt_messages_.empty();
    }

    static std::string tag_to_topic(uint8_t device_type, uint8_t tag);

    struct QueuedMqttMessage {
        const uint32_t                           id_;
        const std::shared_ptr<const MqttMessage> content_;
        uint8_t                                  retry_count_ = 0;
        uint16_t                                 packet_id_   = 0;

        ~QueuedMqttMessage() = default;
        QueuedMqttMessage(uint32_t id, std::shared_ptr<MqttMessage> && content)
            : id_(id)
            , content_(std::move(content)) {
        }
    };
    static std::deque<QueuedMqttMessage> mqtt_messages_;


  private:
    static uuid::log::Logger logger_;

    static AsyncMqttClient * mqttClient_;
    static uint32_t          mqtt_message_id_;

    static constexpr uint32_t MQTT_PUBLISH_WAIT      = 100; // delay between sending publishes, to account for large payloads
    static constexpr uint8_t  MQTT_PUBLISH_MAX_RETRY = 3;   // max retries for giving up on publishing

    static std::shared_ptr<const MqttMessage> queue_message(const uint8_t operation, const std::string & topic, const std::string & payload, bool retain);
    static std::shared_ptr<const MqttMessage> queue_publish_message(const std::string & topic, const std::string & payload, bool retain);
    static std::shared_ptr<const MqttMessage> queue_subscribe_message(const std::string & topic);
    static std::shared_ptr<const MqttMessage> queue_unsubscribe_message(const std::string & topic);

    void on_publish(uint16_t packetId) const;
    void on_message(const char * topic, const char * payload, size_t len) const;
    void process_queue();

    // function handlers for MQTT subscriptions
    struct MQTTSubFunction {
        uint8_t             device_type_;      // which device type, from DeviceType::
        const std::string   topic_;            // short topic name
        mqtt_sub_function_p mqtt_subfunction_; // can be empty

        MQTTSubFunction(uint8_t device_type, const std::string && topic, mqtt_sub_function_p mqtt_subfunction)
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
    uint32_t last_publish_queue_      = 0;

    static bool     connecting_;
    static bool     initialized_;
    static uint32_t mqtt_publish_fails_;
    static uint8_t  connectcount_;
    static bool     ha_climate_reset_;

    // settings, copied over
    static std::string system_hostname_;

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
    static bool        ha_enabled_;
    static uint8_t     nested_format_;
    static std::string discovery_prefix_;
    static bool        publish_single_;
    static bool        publish_single2cmd_;
    static bool        send_response_;
};

} // namespace emsesp

#endif
