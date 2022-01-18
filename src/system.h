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

#ifndef EMSESP_SYSTEM_H_
#define EMSESP_SYSTEM_H_

#include <Arduino.h>
#include <ArduinoJson.h>

#include "helpers.h"
#include "console.h"
#include "mqtt.h"
#include "telegram.h"

#ifndef EMSESP_STANDALONE
#include <esp_wifi.h>
#include <esp_bt.h>
#include <ETH.h>
#include <uuid/syslog.h>
#endif

#include <uuid/log.h>
#include <PButton.h>

using uuid::console::Shell;

namespace emsesp {

enum PHY_type : uint8_t { PHY_TYPE_NONE = 0, PHY_TYPE_LAN8720, PHY_TYPE_TLK110 };

class System {
  public:
    void start(uint32_t heap_start);
    void loop();

    // commands
    static bool command_pin(const char * value, const int8_t id);
    static bool command_send(const char * value, const int8_t id);
    static bool command_publish(const char * value, const int8_t id);
    static bool command_fetch(const char * value, const int8_t id);
    static bool command_restart(const char * value, const int8_t id);
#if defined(EMSESP_DEBUG)
    static bool command_test(const char * value, const int8_t id);
#endif
    static bool command_syslog_level(const char * value, const int8_t id);
    static bool command_watch(const char * value, const int8_t id);

    static bool command_info(const char * value, const int8_t id, JsonObject & output);
    static bool command_settings(const char * value, const int8_t id, JsonObject & output);
    static bool command_customizations(const char * value, const int8_t id, JsonObject & output);
    static bool command_commands(const char * value, const int8_t id, JsonObject & output);

    const std::string reset_reason(uint8_t cpu);

    void system_restart();
    void format(uuid::console::Shell & shell);
    void upload_status(bool in_progress);
    bool upload_status();
    void show_mem(const char * note);
    void get_settings();
    void wifi_tweak();
    void syslog_start();
    bool check_upgrade();
    bool heartbeat_json(JsonObject & output);
    void send_heartbeat();

    void led_init(bool refresh);
    void network_init(bool refresh);
    void button_init(bool refresh);
    void commands_init();

    static bool is_valid_gpio(uint8_t pin);
    static bool load_board_profile(std::vector<int8_t> & data, const std::string & board_profile);

    static void restart_requested(bool restart_requested) {
        restart_requested_ = restart_requested;
    }

    static bool restart_requested() {
        return restart_requested_;
    }

    bool telnet_enabled() {
        return telnet_enabled_;
    }

    bool analog_enabled() {
        return analog_enabled_;
    }

    bool readonly_mode() {
        return readonly_mode_;
    }

    void readonly_mode(bool readonly_mode) {
        readonly_mode_ = readonly_mode;
    }

    uint8_t bool_format() {
        return bool_format_;
    }

    // see default_settings.h
    // BOOL_FORMAT_ONOFF_STR = 1,
    // BOOL_FORMAT_ONOFF_STR_CAP = 2
    // BOOL_FORMAT_TRUEFALSE_STR = 3
    // BOOL_FORMAT_TRUEFALSE = 4
    // BOOL_FORMAT_10_STR = 5
    // BOOL_FORMAT_10 = 6
    void bool_format(uint8_t format) {
        bool_format_ = format;
    }

    uint8_t enum_format() {
        return enum_format_;
    }

    void enum_format(uint8_t format) {
        enum_format_ = format;
    }

    std::string board_profile() {
        return std::string(board_profile_.c_str());
    }

    std::string hostname() {
        return hostname_;
    }

    void hostname(std::string hostname) {
        hostname_ = hostname;
    }

    bool ethernet_connected() {
        return ethernet_connected_;
    }

    void ethernet_connected(bool b) {
        ethernet_connected_ = b;
    }

    bool network_connected() {
#ifndef EMSESP_STANDALONE
        return (ethernet_connected() || WiFi.isConnected());
#else
        return true;
#endif
    }

    void fahrenheit(bool b) {
        fahrenheit_ = b;
    }

    bool fahrenheit() {
        return fahrenheit_;
    }

    void healthcheck(uint8_t healthcheck) {
        healthcheck_ = healthcheck;
    }

    void show_system(uuid::console::Shell & shell);
    void wifi_reconnect();
    void show_users(uuid::console::Shell & shell);

  private:
    static uuid::log::Logger logger_;
    static uint32_t          heap_start_;
    static bool              restart_requested_;

    // button
    static PButton            myPButton_; // PButton instance
    static void               button_OnClick(PButton & b);
    static void               button_OnDblClick(PButton & b);
    static void               button_OnLongPress(PButton & b);
    static void               button_OnVLongPress(PButton & b);
    static constexpr uint32_t BUTTON_Debounce        = 40;   // Debounce period to prevent flickering when pressing or releasing the button (in ms)
    static constexpr uint32_t BUTTON_DblClickDelay   = 250;  // Max period between clicks for a double click event (in ms)
    static constexpr uint32_t BUTTON_LongPressDelay  = 750;  // Hold period for a long press event (in ms)
    static constexpr uint32_t BUTTON_VLongPressDelay = 9000; // Hold period for a very long press event (in ms)

    // healthcheck
    static constexpr uint32_t SYSTEM_CHECK_FREQUENCY          = 5000; // do a system check every 5 seconds
    static constexpr uint32_t HEALTHCHECK_LED_LONG_DUARATION  = 1500;
    static constexpr uint32_t HEALTHCHECK_LED_FLASH_DUARATION = 150;
    static constexpr uint8_t  HEALTHCHECK_NO_BUS              = (1 << 0); // 1
    static constexpr uint8_t  HEALTHCHECK_NO_NETWORK          = (1 << 1); // 2
    static constexpr uint32_t SYSTEM_HEARTBEAT_INTERVAL       = 60000;    // in milliseconds, how often the MQTT heartbeat is sent (1 min)
    static constexpr uint8_t  LED_ON                          = HIGH;     // LED on

#ifndef EMSESP_STANDALONE
    static uuid::syslog::SyslogService syslog_;
#endif

    void led_monitor();
    void system_check();

    int8_t wifi_quality(int8_t dBm);

    uint8_t  healthcheck_       = HEALTHCHECK_NO_NETWORK | HEALTHCHECK_NO_BUS; // start with all flags set, no wifi and no ems bus connection
    uint32_t last_heartbeat_    = 0;
    uint32_t last_system_check_ = 0;

    bool upload_status_      = false; // true if we're in the middle of a OTA firmware upload
    bool ethernet_connected_ = false;

    // EMS-ESP settings
    // copies from WebSettings class in WebSettingsService.h
    std::string hostname_ = FACTORY_WIFI_HOSTNAME;
    bool        hide_led_;
    uint8_t     led_gpio_;
    bool        analog_enabled_;
    bool        low_clock_;
    String      board_profile_;
    uint8_t     pbutton_gpio_;
    uint8_t     rx_gpio_;
    uint8_t     tx_gpio_;
    uint8_t     dallas_gpio_;
    bool        telnet_enabled_;
    bool        syslog_enabled_;
    int8_t      syslog_level_;
    uint32_t    syslog_mark_interval_;
    String      syslog_host_;
    uint16_t    syslog_port_;
    bool        fahrenheit_;
    uint8_t     bool_format_;
    uint8_t     enum_format_;
    bool        readonly_mode_;

    // ethernet
    uint8_t phy_type_;
    int8_t  eth_power_;
    uint8_t eth_phy_addr_;
    uint8_t eth_clock_mode_;
};

} // namespace emsesp

#endif
