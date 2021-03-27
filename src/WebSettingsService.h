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

#ifndef WebSettingsConfig_h
#define WebSettingsConfig_h

#include <HttpEndpoint.h>
#include <FSPersistence.h>

#define EMSESP_SETTINGS_FILE "/config/emsespSettings.json"
#define EMSESP_SETTINGS_SERVICE_PATH "/rest/emsespSettings"
#define EMSESP_BOARD_PROFILE_SERVICE_PATH "/rest/boardProfile"

#define EMSESP_DEFAULT_TX_MODE 1       // EMS1.0
#define EMSESP_DEFAULT_TX_DELAY 0      // no delay
#define EMSESP_DEFAULT_EMS_BUS_ID 0x0B // service key
#define EMSESP_DEFAULT_SYSLOG_ENABLED false
#define EMSESP_DEFAULT_SYSLOG_LEVEL 3 // ERR
#define EMSESP_DEFAULT_SYSLOG_MARK_INTERVAL 0
#define EMSESP_DEFAULT_SYSLOG_HOST ""
#define EMSESP_DEFAULT_SYSLOG_PORT 514
#define EMSESP_DEFAULT_TRACELOG_RAW false
#define EMSESP_DEFAULT_MASTER_THERMOSTAT 0 // not set
#define EMSESP_DEFAULT_SHOWER_TIMER false
#define EMSESP_DEFAULT_SHOWER_ALERT false
#define EMSESP_DEFAULT_HIDE_LED true
#define EMSESP_DEFAULT_DALLAS_PARASITE false
#define EMSESP_DEFAULT_API_ENABLED false // turn off, because its insecure
#define EMSESP_DEFAULT_BOOL_FORMAT 1     // on/off
#define EMSESP_DEFAULT_ANALOG_ENABLED false

#ifndef EMSESP_DEFAULT_BOARD_PROFILE
#define EMSESP_DEFAULT_BOARD_PROFILE "S32" // Gateway S32
#endif

// Default GPIO PIN definitions - based on Wemos/Nodemcu
#define EMSESP_DEFAULT_RX_GPIO 23 // D7
#define EMSESP_DEFAULT_TX_GPIO 5  // D8
#define EMSESP_DEFAULT_DALLAS_GPIO 18
#define EMSESP_DEFAULT_LED_GPIO 2
#define EMSESP_DEFAULT_PBUTTON_GPIO 0

namespace emsesp {

class WebSettings {
  public:
    uint8_t  tx_mode;
    uint8_t  tx_delay;
    uint8_t  ems_bus_id;
    uint8_t  master_thermostat;
    bool     shower_timer;
    bool     shower_alert;
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
    bool     api_enabled;
    bool     analog_enabled;
    uint8_t  pbutton_gpio;
    String   board_profile;

    static void              read(WebSettings & settings, JsonObject & root);
    static StateUpdateResult update(JsonObject & root, WebSettings & settings);

    enum ChangeFlags : uint8_t {

        NONE   = 0,
        UART   = (1 << 0), // 1
        SYSLOG = (1 << 1), // 2
        ADC    = (1 << 2), // 4
        DALLAS = (1 << 3), // 8
        SHOWER = (1 << 4), // 16
        LED    = (1 << 5), // 32
        BUTTON = (1 << 6)  // 64

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
    HttpEndpoint<WebSettings>   _httpEndpoint;
    FSPersistence<WebSettings>  _fsPersistence;
    AsyncCallbackJsonWebHandler _boardProfileHandler;

    void board_profile(AsyncWebServerRequest * request, JsonVariant & json);

    void onUpdate();
};

} // namespace emsesp

#endif