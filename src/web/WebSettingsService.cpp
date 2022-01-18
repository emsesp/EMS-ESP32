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

#include "emsesp.h"

namespace emsesp {

uint8_t WebSettings::flags_ = 0;

using namespace std::placeholders; // for `_1` etc

WebSettingsService::WebSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _httpEndpoint(WebSettings::read, WebSettings::update, this, server, EMSESP_SETTINGS_SERVICE_PATH, securityManager)
    , _fsPersistence(WebSettings::read, WebSettings::update, this, fs, EMSESP_SETTINGS_FILE)
    , _boardProfileHandler(EMSESP_BOARD_PROFILE_SERVICE_PATH,
                           securityManager->wrapCallback(std::bind(&WebSettingsService::board_profile, this, _1, _2), AuthenticationPredicates::IS_ADMIN)) {
    _boardProfileHandler.setMethod(HTTP_POST);
    _boardProfileHandler.setMaxContentLength(256);
    server->addHandler(&_boardProfileHandler);

    addUpdateHandler([&](const String & originId) { onUpdate(); }, false);
}

void WebSettings::read(WebSettings & settings, JsonObject & root) {
    root["tx_mode"]              = settings.tx_mode;
    root["ems_bus_id"]           = settings.ems_bus_id;
    root["syslog_enabled"]       = settings.syslog_enabled;
    root["syslog_level"]         = settings.syslog_level;
    root["trace_raw"]            = settings.trace_raw;
    root["syslog_mark_interval"] = settings.syslog_mark_interval;
    root["syslog_host"]          = settings.syslog_host;
    root["syslog_port"]          = settings.syslog_port;
    root["master_thermostat"]    = settings.master_thermostat;
    root["shower_timer"]         = settings.shower_timer;
    root["shower_alert"]         = settings.shower_alert;
    root["rx_gpio"]              = settings.rx_gpio;
    root["tx_gpio"]              = settings.tx_gpio;
    root["dallas_gpio"]          = settings.dallas_gpio;
    root["dallas_parasite"]      = settings.dallas_parasite;
    root["led_gpio"]             = settings.led_gpio;
    root["hide_led"]             = settings.hide_led;
    root["low_clock"]            = settings.low_clock;
    root["telnet_enabled"]       = settings.telnet_enabled;
    root["notoken_api"]          = settings.notoken_api;
    root["readonly_mode"]        = settings.readonly_mode;
    root["analog_enabled"]       = settings.analog_enabled;
    root["pbutton_gpio"]         = settings.pbutton_gpio;
    root["solar_maxflow"]        = settings.solar_maxflow;
    root["board_profile"]        = settings.board_profile;
    root["fahrenheit"]           = settings.fahrenheit;
    root["bool_format"]          = settings.bool_format;
    root["enum_format"]          = settings.enum_format;
    root["weblog_level"]         = settings.weblog_level;
    root["weblog_buffer"]        = settings.weblog_buffer;
    root["weblog_compact"]       = settings.weblog_compact;
    root["phy_type"]             = settings.phy_type;
    root["eth_power"]            = settings.eth_power;
    root["eth_phy_addr"]         = settings.eth_phy_addr;
    root["eth_clock_mode"]       = settings.eth_clock_mode;
}

// call on initialization and also when settings are updated via web or console
StateUpdateResult WebSettings::update(JsonObject & root, WebSettings & settings) {
    // load default GPIO configuration based on board profile
    std::vector<int8_t> data; //  // led, dallas, rx, tx, button, phy_type, eth_power, eth_phy_addr, eth_clock_mode

    settings.board_profile = root["board_profile"] | EMSESP_DEFAULT_BOARD_PROFILE;
    if (!System::load_board_profile(data, settings.board_profile.c_str())) {
        settings.board_profile = EMSESP_DEFAULT_BOARD_PROFILE; // invalid board configuration, override the default in case it has been misspelled
    }

    uint8_t default_led_gpio       = data[0];
    uint8_t default_dallas_gpio    = data[1];
    uint8_t default_rx_gpio        = data[2];
    uint8_t default_tx_gpio        = data[3];
    uint8_t default_pbutton_gpio   = data[4];
    uint8_t default_phy_type       = data[5];
    uint8_t default_eth_power      = data[6];
    uint8_t default_eth_phy_addr   = data[7];
    uint8_t default_eth_clock_mode = data[8];

    int prev;
    reset_flags();

    // tx_mode, rx and tx pins
    prev             = settings.tx_mode;
    settings.tx_mode = root["tx_mode"] | EMSESP_DEFAULT_TX_MODE;
    check_flag(prev, settings.tx_mode, ChangeFlags::UART);
    prev             = settings.rx_gpio;
    settings.rx_gpio = root["rx_gpio"] | default_rx_gpio;
    check_flag(prev, settings.rx_gpio, ChangeFlags::UART);
    prev             = settings.tx_gpio;
    settings.tx_gpio = root["tx_gpio"] | default_tx_gpio;
    check_flag(prev, settings.tx_gpio, ChangeFlags::UART);

    // syslog
    prev                    = settings.syslog_enabled;
    settings.syslog_enabled = root["syslog_enabled"] | EMSESP_DEFAULT_SYSLOG_ENABLED;
    check_flag(prev, settings.syslog_enabled, ChangeFlags::SYSLOG);

    prev                  = settings.syslog_level;
    settings.syslog_level = root["syslog_level"] | EMSESP_DEFAULT_SYSLOG_LEVEL;
    check_flag(prev, settings.syslog_level, ChangeFlags::SYSLOG);

    prev                          = settings.syslog_mark_interval;
    settings.syslog_mark_interval = root["syslog_mark_interval"] | EMSESP_DEFAULT_SYSLOG_MARK_INTERVAL;
    check_flag(prev, settings.syslog_mark_interval, ChangeFlags::SYSLOG);

    prev                 = settings.syslog_port;
    settings.syslog_port = root["syslog_port"] | EMSESP_DEFAULT_SYSLOG_PORT;
    check_flag(prev, settings.syslog_port, ChangeFlags::SYSLOG);

#ifndef EMSESP_STANDALONE
    String old_syslog_host = settings.syslog_host;
    settings.syslog_host   = root["syslog_host"] | EMSESP_DEFAULT_SYSLOG_HOST;
    if (!old_syslog_host.equals(settings.syslog_host)) {
        add_flags(ChangeFlags::SYSLOG);
    }
#endif

    // button
    prev                  = settings.pbutton_gpio;
    settings.pbutton_gpio = root["pbutton_gpio"] | default_pbutton_gpio;
    check_flag(prev, settings.pbutton_gpio, ChangeFlags::BUTTON);

    // dallas
    prev                 = settings.dallas_gpio;
    settings.dallas_gpio = root["dallas_gpio"] | default_dallas_gpio;
    check_flag(prev, settings.dallas_gpio, ChangeFlags::DALLAS);
    prev                     = settings.dallas_parasite;
    settings.dallas_parasite = root["dallas_parasite"] | EMSESP_DEFAULT_DALLAS_PARASITE;
    check_flag(prev, settings.dallas_parasite, ChangeFlags::DALLAS);

    // shower
    prev                  = settings.shower_timer;
    settings.shower_timer = root["shower_timer"] | EMSESP_DEFAULT_SHOWER_TIMER;
    check_flag(prev, settings.shower_timer, ChangeFlags::SHOWER);
    prev                  = settings.shower_alert;
    settings.shower_alert = root["shower_alert"] | EMSESP_DEFAULT_SHOWER_ALERT;
    check_flag(prev, settings.shower_alert, ChangeFlags::SHOWER);

    // led
    prev              = settings.led_gpio;
    settings.led_gpio = root["led_gpio"] | default_led_gpio;
    check_flag(prev, settings.led_gpio, ChangeFlags::LED);
    prev              = settings.hide_led;
    settings.hide_led = root["hide_led"] | EMSESP_DEFAULT_HIDE_LED;
    check_flag(prev, settings.hide_led, ChangeFlags::LED);

    // adc
    prev                    = settings.analog_enabled;
    settings.analog_enabled = root["analog_enabled"] | EMSESP_DEFAULT_ANALOG_ENABLED;
    check_flag(prev, settings.analog_enabled, ChangeFlags::ADC);

    //
    // these need reboots to be applied...
    //
    prev                    = settings.telnet_enabled;
    settings.telnet_enabled = root["telnet_enabled"] | EMSESP_DEFAULT_TELNET_ENABLED;
    check_flag(prev, settings.telnet_enabled, ChangeFlags::RESTART);

    prev                = settings.ems_bus_id;
    settings.ems_bus_id = root["ems_bus_id"] | EMSESP_DEFAULT_EMS_BUS_ID;
    check_flag(prev, settings.ems_bus_id, ChangeFlags::RESTART);

    prev               = settings.low_clock;
    settings.low_clock = root["low_clock"] | false;
    check_flag(prev, settings.low_clock, ChangeFlags::RESTART);

    prev                       = settings.master_thermostat;
    settings.master_thermostat = root["master_thermostat"] | EMSESP_DEFAULT_MASTER_THERMOSTAT;
    check_flag(prev, settings.master_thermostat, ChangeFlags::RESTART);

    // use whatever came from the board profile
    prev              = settings.phy_type;
    settings.phy_type = root["phy_type"] | default_phy_type;
    check_flag(prev, settings.phy_type, ChangeFlags::RESTART);

    prev               = settings.eth_power;
    settings.eth_power = root["eth_power"] | default_eth_power;
    check_flag(prev, settings.eth_power, ChangeFlags::RESTART);

    prev                  = settings.eth_phy_addr;
    settings.eth_phy_addr = root["eth_phy_addr"] | default_eth_phy_addr;
    check_flag(prev, settings.eth_phy_addr, ChangeFlags::RESTART);

    prev                    = settings.eth_clock_mode;
    settings.eth_clock_mode = root["eth_clock_mode"] | default_eth_clock_mode;
    check_flag(prev, settings.eth_clock_mode, ChangeFlags::RESTART);

    // without checks...

    settings.trace_raw = root["trace_raw"] | EMSESP_DEFAULT_TRACELOG_RAW;
    EMSESP::trace_raw(settings.trace_raw);

    settings.notoken_api   = root["notoken_api"] | EMSESP_DEFAULT_NOTOKEN_API;
    settings.solar_maxflow = root["solar_maxflow"] | EMSESP_DEFAULT_SOLAR_MAXFLOW;

    settings.fahrenheit = root["fahrenheit"] | false;
    EMSESP::system_.fahrenheit(settings.fahrenheit);

    settings.readonly_mode = root["readonly_mode"] | false;
    EMSESP::system_.readonly_mode(settings.readonly_mode);

    settings.bool_format = root["bool_format"] | EMSESP_DEFAULT_BOOL_FORMAT;
    EMSESP::system_.bool_format(settings.bool_format);

    settings.enum_format = root["enum_format"] | EMSESP_DEFAULT_ENUM_FORMAT;
    EMSESP::system_.enum_format(settings.enum_format);

    settings.weblog_level   = root["weblog_level"] | EMSESP_DEFAULT_WEBLOG_LEVEL;
    settings.weblog_buffer  = root["weblog_buffer"] | EMSESP_DEFAULT_WEBLOG_BUFFER;
    settings.weblog_compact = root["weblog_compact"] | EMSESP_DEFAULT_WEBLOG_COMPACT;

    // save the settings
    if (flags_ == WebSettings::ChangeFlags::RESTART) {
        return StateUpdateResult::CHANGED_RESTART; // tell WebUI that a restart is needed
    }

    return StateUpdateResult::CHANGED;
}

// this is called after any of the settings have been persisted to the filesystem
// either via the Web UI or via the Console
void WebSettingsService::onUpdate() {
    if (WebSettings::has_flags(WebSettings::ChangeFlags::SHOWER)) {
        EMSESP::shower_.start();
    }

    if (WebSettings::has_flags(WebSettings::ChangeFlags::DALLAS)) {
        EMSESP::dallassensor_.start();
    }

    if (WebSettings::has_flags(WebSettings::ChangeFlags::UART)) {
        EMSESP::init_uart();
    }

    if (WebSettings::has_flags(WebSettings::ChangeFlags::SYSLOG)) {
        EMSESP::system_.syslog_start(); // re-start (or stop)
    }

    if (WebSettings::has_flags(WebSettings::ChangeFlags::ADC)) {
        EMSESP::analogsensor_.start();
    }

    if (WebSettings::has_flags(WebSettings::ChangeFlags::BUTTON)) {
        EMSESP::system_.button_init(true); // reload settings
    }

    if (WebSettings::has_flags(WebSettings::ChangeFlags::LED)) {
        EMSESP::system_.led_init(true); // reload settings
    }

    WebSettings::reset_flags();
}

void WebSettingsService::begin() {
    _fsPersistence.readFromFS();
}

void WebSettingsService::save() {
    _fsPersistence.writeToFS();
}

// build the json profile to send back
void WebSettingsService::board_profile(AsyncWebServerRequest * request, JsonVariant & json) {
    if (json.is<JsonObject>()) {
        AsyncJsonResponse * response = new AsyncJsonResponse(false, EMSESP_JSON_SIZE_MEDIUM);
        JsonObject          root     = response->getRoot();

        if (json.containsKey("board_profile")) {
            String              board_profile = json["board_profile"];
            std::vector<int8_t> data; // led, dallas, rx, tx, button, phy_type, eth_power, eth_phy_addr, eth_clock_mode
            (void)System::load_board_profile(data, board_profile.c_str());
            root["led_gpio"]       = data[0];
            root["dallas_gpio"]    = data[1];
            root["rx_gpio"]        = data[2];
            root["tx_gpio"]        = data[3];
            root["pbutton_gpio"]   = data[4];
            root["phy_type"]       = data[5];
            root["eth_power"]      = data[6];
            root["eth_phy_addr"]   = data[7];
            root["eth_clock_mode"] = data[8];

            response->setLength();
            request->send(response);
            return;
        }
    }

    AsyncWebServerResponse * response = request->beginResponse(200);
    request->send(response);
}

} // namespace emsesp