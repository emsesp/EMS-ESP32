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

#ifndef WebSettingsConfig_h
#define WebSettingsConfig_h

#include <HttpEndpoint.h>
#include <FSPersistence.h>

#define EMSESP_SETTINGS_FILE "/config/emsespSettings.json"
#define EMSESP_SETTINGS_SERVICE_PATH "/rest/emsespSettings"

#define EMSESP_DEFAULT_TX_MODE 1       // EMS1.0
#define EMSESP_DEFAULT_TX_DELAY 0      // no delay
#define EMSESP_DEFAULT_EMS_BUS_ID 0x0B // service key
#define EMSESP_DEFAULT_SYSLOG_ENABLED false
#define EMSESP_DEFAULT_SYSLOG_LEVEL 3 // ERR
#define EMSESP_DEFAULT_SYSLOG_MARK_INTERVAL 0
#define EMSESP_DEFAULT_SYSLOG_HOST ""
#define EMSESP_DEFAULT_TRACELOG_RAW false
#define EMSESP_DEFAULT_MASTER_THERMOSTAT 0 // not set
#define EMSESP_DEFAULT_SHOWER_TIMER false
#define EMSESP_DEFAULT_SHOWER_ALERT false
#define EMSESP_DEFAULT_HIDE_LED true
#define EMSESP_DEFAULT_DALLAS_PARASITE false
#define EMSESP_DEFAULT_API_ENABLED false // turn off, because its insecure
#define EMSESP_DEFAULT_BOOL_FORMAT 1     // on/off
#define EMSESP_DEFAULT_ANALOG_ENABLED false

// Default GPIO PIN definitions
#if defined(ESP32)
#define EMSESP_DEFAULT_RX_GPIO 23     // D7 on Wemos D1-32, OR 17 for UART2 on Lolin D32
#define EMSESP_DEFAULT_TX_GPIO 5      // D8 on Wemos D1-32, OR 16 for UART2 on Lolin D32
#define EMSESP_DEFAULT_DALLAS_GPIO 18 // 18 on Wemos D1-32, 14 on LOLIN D32
#define EMSESP_DEFAULT_LED_GPIO 2     // 2 on Wemos D1-32, 5 on LOLIN D32
#define EMSESP_DEFAULT_PBUTTON_GPIO 0 // default GPIO is 0 (off)
#else
// for standalone
#define EMSESP_DEFAULT_RX_GPIO 0
#define EMSESP_DEFAULT_TX_GPIO 0
#define EMSESP_DEFAULT_DALLAS_GPIO 0
#define EMSESP_DEFAULT_LED_GPIO 0
#define EMSESP_DEFAULT_PBUTTON_GPIO 0
#endif

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

    static void              read(WebSettings & settings, JsonObject & root);
    static StateUpdateResult update(JsonObject & root, WebSettings & settings);

    enum ChangeFlags : uint8_t {

        NONE   = 0,
        UART   = (1 << 0),
        SYSLOG = (1 << 1),
        ADC    = (1 << 2),
        DALLAS = (1 << 3),
        SHOWER = (1 << 4),
        LED    = (1 << 5),
        BUTTON = (1 << 6)

    };

    static void add_flags(uint8_t flags) {
        flags_ |= flags;
    }

    static bool has_flags(uint8_t flags) {
        return (flags_ & flags) == flags;
    }

    static void reset_flags() {
        flags_ = ChangeFlags::NONE;
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

    void onUpdate();
};

} // namespace emsesp

#endif