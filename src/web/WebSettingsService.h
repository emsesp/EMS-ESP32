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

#ifndef WebSettingsService_h
#define WebSettingsService_h

#include "../core/default_settings.h"

#define EMSESP_SETTINGS_FILE "/config/emsespSettings.json"

#define EMSESP_SETTINGS_SERVICE_PATH "/rest/settings"
#define EMSESP_BOARD_PROFILE_SERVICE_PATH "/rest/boardProfile"

namespace emsesp {

class WebSettings {
  public:
    String   version;
    String   locale;
    uint8_t  tx_mode;
    uint8_t  ems_bus_id;
    bool     boiler_heatingoff;
    uint8_t  remote_timeout;
    bool     remote_timeout_enabled;
    bool     shower_timer;
    bool     shower_alert;
    uint8_t  shower_alert_trigger;  // minutes
    uint8_t  shower_alert_coldshot; // seconds
    uint32_t shower_min_duration;   // seconds
    bool     syslog_enabled;
    int8_t   syslog_level; // uuid::log::Level
    uint32_t syslog_mark_interval;
    String   syslog_host;
    uint16_t syslog_port;
    bool     trace_raw;
    uint8_t  rx_gpio;
    uint8_t  tx_gpio;
    uint8_t  dallas_gpio;
    bool     dallas_parasite;
    uint8_t  led_gpio;
    bool     hide_led;
    uint8_t  led_type;
    bool     low_clock;
    bool     telnet_enabled;
    bool     notoken_api;
    bool     readonly_mode;
    bool     analog_enabled;
    uint8_t  pbutton_gpio;
    uint8_t  solar_maxflow;
    String   board_profile;
    uint8_t  bool_format;
    uint8_t  bool_dashboard;
    uint8_t  enum_format;

    int8_t  weblog_level;
    uint8_t weblog_buffer;
    bool    weblog_compact;

    bool fahrenheit;

    bool     modbus_enabled;
    uint16_t modbus_port;
    uint8_t  modbus_max_clients;
    uint32_t modbus_timeout;

    uint8_t phy_type;
    int8_t  eth_power; // -1 means disabled
    uint8_t eth_phy_addr;
    uint8_t eth_clock_mode;

    bool developer_mode; // developer mode

    static void              read(WebSettings & settings, JsonObject root);
    static StateUpdateResult update(JsonObject root, WebSettings & settings);

    enum ChangeFlags : uint8_t {

        NONE    = 0,
        UART    = (1 << 0), // 1
        SYSLOG  = (1 << 1), // 2
        ADC     = (1 << 2), // 4 - analog
        SENSOR  = (1 << 3), // 8
        SHOWER  = (1 << 4), // 16
        LED     = (1 << 5), // 32
        BUTTON  = (1 << 6), // 64
        MQTT    = (1 << 7), // 128
        RESTART = 0xFF

    };

    static void check_flag(int prev_v, int new_v, uint8_t flag) {
        if (prev_v != new_v) {
            add_flags(flag);
        }
    }

    static void add_flags(uint8_t flags) {
        flags_ |= flags;
    }

    static bool has_flags(uint8_t flags) {
        return (flags_ & flags) == flags;
    }

    static void reset_flags() {
        flags_ = ChangeFlags::NONE;
    }

    static uint8_t get_flags() {
        return flags_;
    }

  private:
    static uint8_t flags_;
};

class WebSettingsService : public StatefulService<WebSettings> {
  public:
    WebSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager);

    void begin();
    void save();

  private:
    HttpEndpoint<WebSettings>  _httpEndpoint;
    FSPersistence<WebSettings> _fsPersistence;

    void board_profile(AsyncWebServerRequest * request);

    void onUpdate();
};

} // namespace emsesp

#endif