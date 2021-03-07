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

#ifndef EMSESP_SYSTEM_H_
#define EMSESP_SYSTEM_H_

#include <Arduino.h>
#include <ArduinoJson.h>

#include "helpers.h"
#include "console.h"
#include "mqtt.h"
#include "telegram.h"

#ifndef EMSESP_STANDALONE
#include "driver/adc.h"
#include <esp_wifi.h>
#include <esp_bt.h>
#include <ETH.h>
#include <uuid/syslog.h>
#endif

#include <uuid/log.h>
#include <PButton.h>

using uuid::console::Shell;

namespace emsesp {

class System {
  public:
    void start(uint32_t heap_start);
    void loop();

    // commands
    static void console_commands(Shell & shell, unsigned int context);
    static bool command_pin(const char * value, const int8_t id);
    static bool command_send(const char * value, const int8_t id);
    static bool command_publish(const char * value, const int8_t id);
    static bool command_fetch(const char * value, const int8_t id);
    static bool command_info(const char * value, const int8_t id, JsonObject & json);
    static bool command_settings(const char * value, const int8_t id, JsonObject & json);
#if defined(EMSESP_TEST)
    static bool command_test(const char * value, const int8_t id);
#endif

    void restart();
    void format(uuid::console::Shell & shell);
    void upload_status(bool in_progress);
    bool upload_status();
    void show_mem(const char * note);
    void get_settings();
    void wifi_tweak();

    void led_init(bool refresh);
    void syslog_init(bool refresh);
    void adc_init(bool refresh);
    void network_init(bool refresh);
    void button_init(bool refresh);
    void commands_init();

    bool check_upgrade();
    void send_heartbeat();

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

  private:
    static uuid::log::Logger logger_;
    static uint32_t          heap_start_;

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

    static constexpr uint32_t SYSTEM_CHECK_FREQUENCY         = 5000;  // check every 5 seconds
    static constexpr uint32_t LED_WARNING_BLINK              = 1000;  // pulse to show no connection, 1 sec
    static constexpr uint32_t LED_WARNING_BLINK_FAST         = 100;   // flash quickly for boot up sequence
    static constexpr uint32_t SYSTEM_HEARTBEAT_INTERVAL      = 60000; // in milliseconds, how often the MQTT heartbeat is sent (1 min)
    static constexpr uint32_t SYSTEM_MEASURE_ANALOG_INTERVAL = 500;
    static constexpr uint8_t  LED_ON                         = LOW; // internal LED

#ifndef EMSESP_STANDALONE
    static uuid::syslog::SyslogService syslog_;
#endif

    void led_monitor();
    void set_led_speed(uint32_t speed);
    void system_check();
    void measure_analog();

    void   show_system(uuid::console::Shell & shell);
    void   show_users(uuid::console::Shell & shell);
    void   wifi_reconnect();
    int8_t wifi_quality();

    bool     system_healthy_    = false;
    uint32_t led_flash_speed_   = LED_WARNING_BLINK_FAST; // default boot flashes quickly
    uint32_t last_heartbeat_    = 0;
    uint32_t last_system_check_ = 0;
    bool     upload_status_     = false; // true if we're in the middle of a OTA firmware upload
    bool     ethernet_connected_;
    uint16_t analog_;

    // settings
    std::string hostname_;
    bool        hide_led_;
    uint8_t     led_gpio_;
    bool        syslog_enabled_;
    bool        analog_enabled_;
    uint8_t     ethernet_profile_;
    uint8_t     pbutton_gpio_;
    int8_t      syslog_level_;
    uint32_t    syslog_mark_interval_;
    String      syslog_host_;
};

} // namespace emsesp

#endif
