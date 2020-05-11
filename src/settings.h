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

#ifndef EMSESP_SETTINGS_H
#define EMSESP_SETTINGS_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <string>
#include <vector>

// we use LittleFS on the ESP8266 and the older SPIFFS on the ESP32
#if defined(ESP8266)
#include <FS.h>
#include <LittleFS.h>
#define EMSESP_FS LittleFS
#elif defined(ESP32)
#include <SPIFFS.h>
#define EMSESP_FS SPIFFS
#endif

#include <uuid/log.h>
#include <uuid/console.h>

#include "helpers.h"
#include "version.h"
#include "console.h"

// default settings - these can be customized from within the application
#define EMSESP_DEFAULT_HOSTNAME "ems-esp"
#define EMSESP_DEFAULT_ADMIN_PASSWORD "neo"
#define EMSESP_DEFAULT_BUS_ID 0x0B
#define EMSESP_DEFAULT_TX_MODE 1
#define EMSESP_DEFAULT_MQTT_ENABLED true
#define EMSESP_DEFAULT_MQTT_BASE "home"
#define EMSESP_DEFAULT_MQTT_PORT 1883
#define EMSESP_DEFAULT_MQTT_QOS 0
#define EMSESP_DEFAULT_MQTT_RETAIN false
#define EMSESP_DEFAULT_MQTT_NESTEDJSON true
#define EMSESP_DEFAULT_MQTT_HEARTBEAT true
#define EMSESP_DEFAULT_EMS_READ_ONLY false
#define EMSESP_DEFAULT_SHOWER_TIMER false
#define EMSESP_DEFAULT_SHOWER_ALERT false
#define EMSESP_DEFAULT_SYSLOG_INTERVAL 0
#define EMSESP_DEFAULT_MASTER_THERMOSTAT 0 // not set

#ifndef EMSESP_STANDALONE
#define EMSESP_DEFAULT_MQTT_PUBLISH_TIME 10
#else
#define EMSESP_DEFAULT_MQTT_PUBLISH_TIME 0
#endif

namespace emsesp {

class Settings {
  public:
    Settings();
    ~Settings() = default;

    void commit();
    void show_settings(uuid::console::Shell & shell);
    void format(uuid::console::Shell & shell);

    std::string app_version() const;
    void        app_version(const std::string & app_version);

    std::string admin_password() const;
    void        admin_password(const std::string & admin_password);

    std::string hostname() const;
    void        hostname(const std::string & hostname);

    std::string wifi_ssid() const;
    void        wifi_ssid(const std::string & wifi_ssid);

    std::string wifi_password() const;
    void        wifi_password(const std::string & wifi_password);

    std::string syslog_host() const;
    void        syslog_host(const std::string & syslog_host);

    uuid::log::Level syslog_level() const;
    void             syslog_level(uuid::log::Level syslog_level);

    unsigned long syslog_mark_interval() const;
    void          syslog_mark_interval(const unsigned long & syslog_mark_interval);

    uint8_t ems_bus_id() const;
    void    ems_bus_id(const uint8_t & ems_bus_id);

    uint8_t ems_tx_mode() const;
    void    ems_tx_mode(const uint8_t & ems_tx_mode);

    bool ems_read_only() const;
    void ems_read_only(const bool & ems_read_only);

    uint16_t mqtt_publish_time() const;
    void     mqtt_publish_time(const uint16_t & mqtt_publish_time);

    std::string mqtt_ip() const;
    void        mqtt_ip(const std::string & mqtt_ip);

    std::string mqtt_user() const;
    void        mqtt_user(const std::string & mqtt_user);

    std::string mqtt_password() const;
    void        mqtt_password(const std::string & mqtt_password);

    uint16_t mqtt_port() const;
    void     mqtt_port(const uint16_t & mqtt_port);

    bool mqtt_enabled() const;
    void mqtt_enabled(const bool & mqtt_enabled);

    std::string mqtt_base() const;
    void        mqtt_base(const std::string & mqtt_base);

    uint8_t mqtt_qos() const;
    void    mqtt_qos(const uint8_t & mqtt_qos);

    bool mqtt_retain() const;
    void mqtt_retain(const bool & mqtt_retain);

    bool mqtt_nestedjson() const;
    void mqtt_nestedjson(const bool & mqtt_nestedjson);

    bool mqtt_heartbeat() const;
    void mqtt_heartbeat(const bool & mqtt_heartbeat);

    bool shower_timer() const;
    void shower_timer(const bool & shower_timer);

    bool shower_alert() const;
    void shower_alert(const bool & shower_alert);

    uint8_t master_thermostat() const;
    void    master_thermostat(const uint8_t & master_thermostat);

  private:
    static constexpr size_t BUFFER_SIZE = 2048; // max size for the settings file

    static uuid::log::Logger logger_;

    // global, not saved
    static bool mounted_;
    static bool unavailable_;
    static bool loaded_;

    bool read_settings(const std::string & filename, bool load = true);
    bool write_settings(const std::string & filename);
    void read_settings(const ArduinoJson::JsonDocument & doc);
    void write_settings(ArduinoJson::JsonDocument & doc);

    static std::string app_version_;

    static std::string admin_password_;
    static std::string hostname_;
    static std::string wifi_password_;
    static std::string wifi_ssid_;

    static std::string      syslog_host_;
    static uuid::log::Level syslog_level_;
    static unsigned long    syslog_mark_interval_;

    static bool shower_timer_; // true if we want to report back on shower times
    static bool shower_alert_; // true if we want the alert of cold water

    static uint8_t master_thermostat_; // which thermostat device ID is leading

    static uint8_t ems_tx_mode_;   // Tx mode 1, 2 or 3
    static uint8_t ems_bus_id_;    // EMS bus ID, default 0x0B for Service Key
    static bool    ems_read_only_; // switch of Tx

    static std::string mqtt_ip_;
    static std::string mqtt_user_;
    static std::string mqtt_password_;
    static uint16_t    mqtt_port_;
    static bool        mqtt_enabled_;
    static std::string mqtt_base_;
    static uint8_t     mqtt_qos_;
    static bool        mqtt_retain_;
    static bool        mqtt_nestedjson_;
    static bool        mqtt_heartbeat_;
    static uint16_t    mqtt_publish_time_; // frequency of MQTT publish in seconds
};

} // namespace emsesp

#endif
