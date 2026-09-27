/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2026  emsesp.org
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

#ifndef FACTORY_EMAIL_ENABLE
#define FACTORY_EMAIL_ENABLE false
#endif
#ifndef FACTORY_EMAIL_SECURITY
#define FACTORY_EMAIL_SECURITY 2
#endif
#ifndef FACTORY_EMAIL_PORT
#define FACTORY_EMAIL_PORT 587
#endif
#ifndef FACTORY_EMAIL_SERVER
#define FACTORY_EMAIL_SERVER "smtp.example.net"
#endif
#ifndef FACTORY_EMAIL_LOGIN
#define FACTORY_EMAIL_LOGIN ""
#endif
#ifndef FACTORY_EMAIL_PASSWORD
#define FACTORY_EMAIL_PASSWORD ""
#endif
#ifndef FACTORY_EMAIL_FROM
#define FACTORY_EMAIL_FROM "ems-esp@example.net"
#endif
#ifndef FACTORY_EMAIL_TO
#define FACTORY_EMAIL_TO ""
#endif
#ifndef FACTORY_EMAIL_SUBJECT
#define FACTORY_EMAIL_SUBJECT "ems-esp notification"
#endif
namespace emsesp {

class WebSettings {
  public:
    String   version = EMSESP_APP_VERSION;
    String   system_name;
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

    int8_t   weblog_level;
    uint16_t weblog_buffer;
    bool     weblog_compact;

    bool fahrenheit;

    bool     modbus_enabled;
    uint16_t modbus_port;
    uint8_t  modbus_max_clients;
    uint32_t modbus_timeout;
    bool     email_enabled;
    uint8_t  email_security;
    String   email_server;
    uint16_t email_port;
    String   email_login;
    String   email_pass;
    String   email_sender;
    String   email_recp;
    String   email_subject;

    uint8_t phy_type;
    int8_t  eth_power; // -1 means disabled
    uint8_t eth_phy_addr;
    uint8_t eth_clock_mode;

    bool developer_mode; // developer mode
    bool disable_reset;  // disable reset
    bool auto_fw_check;  // periodically check emsesp.org for a newer firmware version

    static void              read(WebSettings & settings, JsonObject root);
    static StateUpdateResult update(JsonObject root, WebSettings & settings);

    enum ChangeFlags : uint16_t {
        NONE               = 0,
        UART               = (1 << 0), // 1 - uart
        SYSLOG             = (1 << 1), // 2 - syslog
        ANALOG_SENSOR      = (1 << 2), // 4 - analog
        TEMPERATURE_SENSOR = (1 << 3), // 8 - dallas sensor
        SHOWER             = (1 << 4), // 16 - shower timer and alert
        LED                = (1 << 5), // 32 - led
        BUTTON             = (1 << 6), // 64 - button
        MQTT               = (1 << 7), // 128 - mqtt
        MODBUS             = (1 << 8), // 256 - modbus
        RESTART            = 0xFFFF    // restart request (all changes)
    };

    static bool     check_flag(int prev_v, int new_v, uint16_t flag);
    static void     add_flags(uint16_t flags);
    static bool     has_flags(uint16_t flags);
    static void     reset_flags();
    static uint16_t get_flags();

  private:
    static void set_board_profile(WebSettings & settings);

    static uint16_t flags_;
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