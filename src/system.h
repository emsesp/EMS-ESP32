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

#ifndef EMSESP_SYSTEM_H_
#define EMSESP_SYSTEM_H_

#include <Arduino.h>
#include <ArduinoJson.h>

#include "helpers.h"
#include "console.h"
#include "mqtt.h"
#include "telegram.h"

#ifndef EMSESP_STANDALONE
#include <uuid/syslog.h>
#endif

#include <uuid/log.h>

using uuid::console::Shell;

namespace emsesp {

class System {
  public:
    void start();
    void loop();

    // commands
    static void restart();
    static void format(uuid::console::Shell & shell);

    static void console_commands(Shell & shell, unsigned int context);

    static void mqtt_command_pin(const char * value, const int8_t id);
    static void mqtt_command_send(const char * value, const int8_t id);

    static uint8_t free_mem();
    static void    upload_status(bool in_progress);
    static bool    upload_status();
    void           syslog_init();
    void           set_heartbeat(bool system_heartbeat);
    void           send_heartbeat();
    static void    show_mem(const char * note);
    static void    set_led();
    bool           check_upgrade();

  private:
    static uuid::log::Logger logger_;

#ifndef EMSESP_STANDALONE
    static uuid::syslog::SyslogService syslog_;
#endif

    static constexpr uint32_t SYSTEM_CHECK_FREQUENCY         = 5000;  // check every 5 seconds
    static constexpr uint32_t LED_WARNING_BLINK              = 1000;  // pulse to show no connection, 1 sec
    static constexpr uint32_t LED_WARNING_BLINK_FAST         = 100;   // flash quickly for boot up sequence
    static constexpr uint32_t SYSTEM_HEARTBEAT_INTERVAL      = 60000; // in milliseconds, how often the MQTT heartbeat is sent (1 min)
    static constexpr uint32_t SYSTEM_MEASURE_ANALOG_INTERVAL = 1100;

    // internal LED
    static constexpr uint8_t LED_ON = LOW;

    void led_monitor();
    void set_led_speed(uint32_t speed);
    void system_check();
    void measure_analog();

    static void   show_system(uuid::console::Shell & shell);
    static void   show_users(uuid::console::Shell & shell);
    static void   wifi_reconnect();
    static int8_t wifi_quality();

    bool            system_healthy_  = false;
    uint32_t        led_flash_speed_ = LED_WARNING_BLINK_FAST; // default boot flashes quickly
    static uint32_t heap_start_;
    static int      reset_counter_;
    uint32_t        last_heartbeat_ = 0;
    static bool     upload_status_; // true if we're in the middle of a OTA firmware upload
    static uint16_t analog_;

    // settings
    bool           system_heartbeat_;
    static bool    hide_led_;
    uint8_t        syslog_level_;
    uint32_t       syslog_mark_interval_;
    String         syslog_host_;
    static uint8_t led_gpio_;
};

} // namespace emsesp

#endif
