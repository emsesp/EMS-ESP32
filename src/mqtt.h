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
#include <deque>
#include <functional>

#ifndef EMSESP_STANDALONE
#include <AsyncMqttClient.h>
#endif

#include "helpers.h"
#include "settings.h"
#include "system.h"
#include "network.h"
#include "console.h"

#include <uuid/log.h>

using uuid::console::Shell;

#define EMSESP_MAX_JSON_SIZE_SMALL 200  // for smaller json docs
#define EMSESP_MAX_JSON_SIZE_MEDIUM 800 // for smaller json docs from ems devices
#define EMSESP_MAX_JSON_SIZE_LARGE 1500 // for large json docs from ems devices, like boiler or thermostat data

namespace emsesp {

using mqtt_function_p = std::function<void(const char * message)>;
using namespace std::placeholders; // for `_1`

struct MqttMessage {
    MqttMessage(uint8_t operation, const std::string & topic, const std::string & payload, bool retain);
    ~MqttMessage() = default;

    const uint8_t     operation;
    const std::string topic;
    const std::string payload;
    const bool        retain;
};

class Mqtt {
  public:
    void loop();
    void send_heartbeat();

    enum Operation { PUBLISH, SUBSCRIBE };

    // are static to be accessed from EMS devices
    static void subscribe(const uint8_t device_id, const std::string & topic, mqtt_function_p cb);
    static void subscribe(const std::string & topic, mqtt_function_p cb);

    static void publish(const std::string & topic, const std::string & payload);
    static void publish(const std::string & topic, const std::string & payload, bool retain);
    static void publish(const std::string & topic, const JsonDocument & payload);
    static void publish(const std::string & topic, const JsonDocument & payload, bool retain);
    static void publish(const std::string & topic, const bool value);
    static void publish(const std::string & topic);

    static void show_topic_handlers(uuid::console::Shell & shell, const uint8_t device_id);

    static void console_commands(Shell & shell, unsigned int context);

    void incoming(char * topic, char * payload); // for testing

    static constexpr uint8_t MQTT_TOPIC_MAX_SIZE = 60; // include host and base etc

    static bool connected() {
#ifdef EMSESP_STANDALONE
        return true;
#else
        return mqttClient_.connected();
#endif
    }

  private:
    static uuid::log::Logger logger_;

    class QueuedMqttMessage {
      public:
        QueuedMqttMessage(uint16_t id, std::shared_ptr<MqttMessage> && content);
        ~QueuedMqttMessage() = default;

        const uint16_t                           id_;
        const std::shared_ptr<const MqttMessage> content_;
        uint8_t                                  retry_count_;
        uint16_t                                 packet_id_;
    };
    static std::deque<QueuedMqttMessage> mqtt_messages_;

    void start();

#ifndef EMSESP_STANDALONE
    static AsyncMqttClient mqttClient_;
#endif

    void flush_message_queue();

    static constexpr size_t MAX_MQTT_MESSAGES = 50;
    static size_t           maximum_mqtt_messages_;
    static uint16_t         mqtt_message_id_;

    static constexpr uint8_t  MQTT_QUEUE_MAX_SIZE       = 50;
    static constexpr uint32_t MQTT_PUBLISH_WAIT         = 750; // delay between sending publishes, although it should be asynchronous!
    static constexpr uint8_t  MQTT_PUBLISH_MAX_RETRY    = 3;   // max retries for giving up on publishing
    static constexpr uint8_t  MQTT_KEEP_ALIVE           = 60;
    static constexpr uint32_t MQTT_RECONNECT_DELAY_MIN  = 2000;   // Try to reconnect in 2 seconds upon disconnection
    static constexpr uint32_t MQTT_RECONNECT_DELAY_STEP = 3000;   // Increase the reconnect delay in 3 seconds after each failed attempt
    static constexpr uint32_t MQTT_RECONNECT_DELAY_MAX  = 120000; // Set reconnect time to 2 minutes at most
    static constexpr uint32_t MQTT_HEARTBEAT_INTERVAL   = 60000;  // in milliseconds, how often the MQTT heartbeat is sent (1 min)

    static bool mqtt_retain_;

    static void queue_publish_message(const std::string & topic, const JsonDocument & payload, const bool retain);
    static void queue_publish_message(const std::string & topic, const std::string & payload, const bool retain);

    static void queue_subscribe_message(const std::string & topic);

    void          on_publish(uint16_t packetId);
    void          on_message(char * topic, char * payload, size_t len);
    void          on_connect();
    static char * make_topic(char * result, const std::string & topic);
    void          process_queue();
    void          process_all_queue();
    void          send_start_topic();
    static void   reconnect();
    void          init();

    static void show_mqtt(uuid::console::Shell & shell);

    class MQTTFunction {
      public:
        MQTTFunction(uint8_t device_id, const std::string && topic, mqtt_function_p mqtt_function);
        ~MQTTFunction() = default;

        uint8_t         device_id_; // which device ID owns this
        std::string     topic_;
        mqtt_function_p mqtt_function_;
    };
    static std::vector<MQTTFunction> mqtt_functions_; // list of mqtt callbacks for all devices

    static uint16_t mqtt_publish_fails_;

    uint32_t mqtt_last_connection_ = 0;
    uint32_t mqtt_reconnect_delay_ = MQTT_RECONNECT_DELAY_MIN;
    bool     mqtt_init_            = false;
    bool     mqtt_start_           = false;
    bool     mqtt_connecting_      = false;
    uint16_t mqtt_publish_time_;

    uint32_t last_heartbeat_ = 0;
    uint32_t last_mqtt_poll_ = 0;
    uint32_t last_publish_   = 0;

    static bool force_publish_;

    // settings
    static std::string mqtt_hostname_;
    static std::string mqtt_base_;
    static uint8_t     mqtt_qos_;
    static uint8_t     mqtt_format_;
    std::string        mqtt_ip_;
    std::string        mqtt_user_;
    std::string        mqtt_password_;
    bool               mqtt_enabled_ = true; // start off assuming we want to connect
    bool               mqtt_heartbeat_;
    uint16_t           mqtt_port_;
};

} // namespace emsesp

#endif
