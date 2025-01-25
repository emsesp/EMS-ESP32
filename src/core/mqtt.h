/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2024  emsesp.org - proddy, MichaelDvP
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

#include <espMqttClient.h>

#include "helpers.h"
#include "system.h"
#include "console.h"
#include "command.h"
#include "emsdevicevalue.h"

using uuid::console::Shell;

namespace emsesp {

using mqtt_sub_function_p = std::function<bool(const char * message)>;

class Mqtt {
  public:
    enum discoveryType : uint8_t { HOMEASSISTANT = 0, DOMOTICZ, DOMOTICZ_LATEST };

    // SINGLE_SHORT (1) and MULTI_SHORT (2) are the latest. Default is SINGLE_SHORT.
    // SINGLE_LONG (0) is v3.4 only
    // SINGLE_OLD (3) and MULTI_OLD (4) are for backwards compatibility with the older v3.6 style. https://github.com/emsesp/EMS-ESP32/issues/1714
    enum entityFormat : uint8_t { SINGLE_LONG = 0, SINGLE_SHORT, MULTI_SHORT, SINGLE_OLD, MULTI_OLD };

    void loop();
    void start();

    static void load_settings();

    void set_publish_time_boiler(uint16_t publish_time);
    void set_publish_time_thermostat(uint16_t publish_time);
    void set_publish_time_solar(uint16_t publish_time);
    void set_publish_time_mixer(uint16_t publish_time);
    void set_publish_time_water(uint16_t publish_time);
    void set_publish_time_other(uint16_t publish_time);
    void set_publish_time_sensor(uint16_t publish_time);
    void set_publish_time_heartbeat(uint16_t publish_time);
    bool get_publish_onchange(uint8_t device_type);

    enum Operation : uint8_t { PUBLISH, SUBSCRIBE, UNSUBSCRIBE };
    enum NestedFormat : uint8_t { NESTED = 1, SINGLE };

    static constexpr uint8_t  MQTT_TOPIC_MAX_SIZE = 128; // fixed, not a user setting anymore
    static constexpr uint16_t MQTT_QUEUE_MAX_SIZE = 300;

    static void on_connect();
    static void on_disconnect(espMqttClientTypes::DisconnectReason reason);
    static void on_message(const char * topic, const uint8_t * payload, size_t len);
    static void subscribe(const uint8_t device_type, const std::string & topic, mqtt_sub_function_p cb);
    static void subscribe(const std::string & topic);
    static void resubscribe();

    static bool queue_publish(const std::string & topic, const std::string & payload);
    static bool queue_publish(const char * topic, const char * payload);
    static bool queue_publish(const std::string & topic, const JsonObjectConst payload);
    static bool queue_publish(const char * topic, const JsonObjectConst payload);
    static bool queue_publish(const char * topic, const std::string & payload);
    static bool queue_publish_retain(const std::string & topic, const JsonObjectConst payload, const bool retain);
    static bool queue_publish_retain(const char * topic, const std::string & payload, const bool retain);
    static bool queue_publish_retain(const char * topic, const JsonObjectConst payload, const bool retain);
    static bool queue_ha(const char * topic, const JsonObjectConst payload);
    static bool queue_remove_topic(const char * topic);

    static bool publish_ha_sensor_config(DeviceValue & dv, const char * model, const char * brand, const bool remove, const bool create_device_config = false);
    static bool publish_ha_sensor_config(uint8_t               type,
                                         int8_t                tag,
                                         const char * const    fullname,
                                         const char * const    en_name,
                                         const uint8_t         device_type,
                                         const char * const    entity,
                                         const uint8_t         uom,
                                         const bool            remove,
                                         const bool            has_cmd,
                                         const char * const ** options,
                                         uint8_t               options_size,
                                         const int16_t         dv_set_min,
                                         const uint32_t        dv_set_max,
                                         const int8_t          num_op,
                                         const JsonObjectConst dev_json);

    static bool publish_system_ha_sensor_config(uint8_t type, const char * name, const char * entity, const uint8_t uom);
    static bool publish_ha_climate_config(const int8_t tag, const bool has_roomtemp, const bool remove = false, const int16_t min = 5, const uint32_t max = 30);

    static void show_topic_handlers(uuid::console::Shell & shell, const uint8_t device_type);
    static void show_mqtt(uuid::console::Shell & shell);

    static void ha_status();

#if defined(EMSESP_TEST)
    void incoming(const char * topic, const char * payload = ""); // for testing only
#endif

    static bool connected() {
        return mqttClient_ ? mqttClient_->connected() : false;
    }

    static void disconnect() {
        if (mqttClient_) {
            mqttClient_->disconnect();
        };
    }

    static MqttClient * client() {
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

    static std::string basename() {
        return mqtt_basename_;
    }

    // returns the discovery MQTT topic prefix and adds a /
    static std::string discovery_prefix() {
        if (discovery_prefix_.empty()) {
            return std::string{};
        }
        return discovery_prefix_ + "/";
    }

    static uint32_t publish_count() {
        return mqtt_message_id_;
    }

    static uint32_t publish_fails() {
        return mqtt_publish_fails_;
    }

    static uint32_t publish_queued() {
        return queuecount_;
    }

    static uint8_t connect_count() {
        return connectcount_;
    }

    static void reset_mqtt();

    static bool is_nested() {
        return nested_format_ == NestedFormat::NESTED;
    }

    static uint8_t entity_format() {
        return entity_format_;
    }

    static void entity_format(uint8_t n) {
        entity_format_ = n;
    }

    static uint8_t discovery_type() {
        return discovery_type_;
    }

    static void nested_format(uint8_t nested_format) {
        nested_format_ = nested_format;
    }

    static bool publish_single() {
        return mqtt_enabled_ && publish_single_;
    }

    static bool publish_single2cmd() {
        return publish_single2cmd_;
    }

    static void publish_single(bool publish_single) {
        publish_single_ = publish_single;
    }

    static bool ha_enabled() {
        return mqtt_enabled_ && ha_enabled_;
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

    static std::string get_response() {
        return lastresponse_;
    }

    static void clear_response() {
        lastresponse_.clear();
    }

    void set_qos(uint8_t mqtt_qos) const {
        mqtt_qos_ = mqtt_qos;
    }

    void set_retain(bool mqtt_retain) const {
        mqtt_retain_ = mqtt_retain;
    }

    static std::string tag_to_topic(uint8_t device_type, int8_t tag);

    static void add_ha_uom(JsonObject doc, const uint8_t type, const uint8_t uom, const char * entity = nullptr, bool is_discovery = true);

    static void add_ha_sections_to_doc(const char *   name,
                                       const char *   state_t,
                                       JsonDocument & config,
                                       const bool     is_first = false,
                                       const char *   cond1    = nullptr,
                                       const char *   cond2    = nullptr,
                                       const char *   negcond  = nullptr);
    static void add_ha_bool(JsonDocument & config);

  private:
    static uuid::log::Logger logger_;

    static MqttClient * mqttClient_;
    static uint32_t     mqtt_message_id_;

    static bool queue_message(const uint8_t operation, const std::string & topic, const std::string & payload, const bool retain);
    static bool queue_publish_message(const std::string & topic, const std::string & payload, const bool retain);
    static void queue_subscribe_message(const std::string & topic);
    static void queue_unsubscribe_message(const std::string & topic);

    void on_publish(uint16_t packetId) const;

    // function handlers for MQTT subscriptions
    struct MQTTSubFunction {
        uint8_t             device_type_;      // which device type, from DeviceType::
        const std::string   topic_;            // short topic name
        mqtt_sub_function_p mqtt_subfunction_; // can be empty

        // replaced &&topic with &topic in 3.7.0-dev.43, so we prevent the std:move later
        MQTTSubFunction(uint8_t device_type, const std::string & topic, mqtt_sub_function_p mqtt_subfunction)
            : device_type_(device_type)
            , topic_(topic)
            , mqtt_subfunction_(mqtt_subfunction) {
        }
    };

    static std::vector<MQTTSubFunction> mqtt_subfunctions_; // list of mqtt subscribe callbacks for all devices

    uint32_t last_publish_boiler_     = 0;
    uint32_t last_publish_thermostat_ = 0;
    uint32_t last_publish_solar_      = 0;
    uint32_t last_publish_mixer_      = 0;
    uint32_t last_publish_water_      = 0;
    uint32_t last_publish_other_      = 0;
    uint32_t last_publish_sensor_     = 0;
    uint32_t last_publish_heartbeat_  = 0;
    // uint32_t last_publish_queue_      = 0;

    static bool     connecting_;
    static bool     initialized_;
    static uint32_t mqtt_publish_fails_;
    static uint16_t queuecount_;
    static uint8_t  connectcount_;
    static bool     ha_climate_reset_;

    static std::string lasttopic_;
    static std::string lastpayload_;
    static std::string lastresponse_;

    // settings, copied over
    static std::string mqtt_base_;
    static std::string mqtt_basename_;
    static uint8_t     mqtt_qos_;
    static bool        mqtt_retain_;
    static uint32_t    publish_time_;
    static uint32_t    publish_time_boiler_;
    static uint32_t    publish_time_thermostat_;
    static uint32_t    publish_time_solar_;
    static uint32_t    publish_time_mixer_;
    static uint32_t    publish_time_water_;
    static uint32_t    publish_time_other_;
    static uint32_t    publish_time_sensor_;
    static uint32_t    publish_time_heartbeat_;
    static bool        mqtt_enabled_;
    static bool        ha_enabled_;
    static uint8_t     nested_format_;
    static uint8_t     entity_format_;
    static std::string discovery_prefix_;
    static uint8_t     discovery_type_;
    static bool        publish_single_;
    static bool        publish_single2cmd_;
    static bool        send_response_;
};

} // namespace emsesp

#endif