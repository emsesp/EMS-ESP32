/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2024  Paul Derbyshire
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

WebSettingsService::WebSettingsService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _httpEndpoint(WebSettings::read, WebSettings::update, this, server, EMSESP_SETTINGS_SERVICE_PATH, securityManager)
    , _fsPersistence(WebSettings::read, WebSettings::update, this, fs, EMSESP_SETTINGS_FILE) {
    server->on(EMSESP_BOARD_PROFILE_SERVICE_PATH,
               HTTP_GET,
               securityManager->wrapRequest([this](AsyncWebServerRequest * request) { board_profile(request); }, AuthenticationPredicates::IS_AUTHENTICATED));
    addUpdateHandler([this] { onUpdate(); }, false);
}

void WebSettings::read(WebSettings & settings, JsonObject root) {
    root["version"]               = settings.version;
    root["locale"]                = settings.locale;
    root["tx_mode"]               = settings.tx_mode;
    root["ems_bus_id"]            = settings.ems_bus_id;
    root["syslog_enabled"]        = settings.syslog_enabled;
    root["syslog_level"]          = settings.syslog_level;
    root["trace_raw"]             = settings.trace_raw;
    root["syslog_mark_interval"]  = settings.syslog_mark_interval;
    root["syslog_host"]           = settings.syslog_host;
    root["syslog_port"]           = settings.syslog_port;
    root["boiler_heatingoff"]     = settings.boiler_heatingoff;
    root["remote_timeout"]        = settings.remote_timeout;
    root["remote_timeout_en"]     = settings.remote_timeout_enabled;
    root["shower_timer"]          = settings.shower_timer;
    root["shower_alert"]          = settings.shower_alert;
    root["shower_alert_coldshot"] = settings.shower_alert_coldshot;
    root["shower_alert_trigger"]  = settings.shower_alert_trigger;
    root["shower_min_duration"]   = settings.shower_min_duration;
    root["rx_gpio"]               = settings.rx_gpio;
    root["tx_gpio"]               = settings.tx_gpio;
    root["dallas_gpio"]           = settings.dallas_gpio;
    root["dallas_parasite"]       = settings.dallas_parasite;
    root["led_gpio"]              = settings.led_gpio;
    root["hide_led"]              = settings.hide_led;
    root["low_clock"]             = settings.low_clock;
    root["telnet_enabled"]        = settings.telnet_enabled;
    root["notoken_api"]           = settings.notoken_api;
    root["readonly_mode"]         = settings.readonly_mode;
    root["analog_enabled"]        = settings.analog_enabled;
    root["pbutton_gpio"]          = settings.pbutton_gpio;
    root["solar_maxflow"]         = settings.solar_maxflow;
    root["board_profile"]         = settings.board_profile;
    root["fahrenheit"]            = settings.fahrenheit;
    root["bool_format"]           = settings.bool_format;
    root["bool_dashboard"]        = settings.bool_dashboard;
    root["enum_format"]           = settings.enum_format;
    root["weblog_level"]          = settings.weblog_level;
    root["weblog_buffer"]         = settings.weblog_buffer;
    root["weblog_compact"]        = settings.weblog_compact;
    root["phy_type"]              = settings.phy_type;
    root["eth_power"]             = settings.eth_power;
    root["eth_phy_addr"]          = settings.eth_phy_addr;
    root["eth_clock_mode"]        = settings.eth_clock_mode;
    String platform               = EMSESP_PLATFORM;
    root["platform"]              = (platform == "ESP32" && EMSESP::system_.PSram()) ? "ESP32R" : platform;
    root["modbus_enabled"]        = settings.modbus_enabled;
    root["modbus_port"]           = settings.modbus_port;
    root["modbus_max_clients"]    = settings.modbus_max_clients;
    root["modbus_timeout"]        = settings.modbus_timeout;
}

// call on initialization and also when settings are updated via web or console
StateUpdateResult WebSettings::update(JsonObject root, WebSettings & settings) {
    // load the version of the settings
    // will be picked up in System::check_upgrade()
    settings.version = root["version"] | EMSESP_DEFAULT_VERSION;

    // load default GPIO configuration based on board profile
    std::vector<int8_t> data; //  // led, dallas, rx, tx, button, phy_type, eth_power, eth_phy_addr, eth_clock_mode
    settings.board_profile = root["board_profile"] | EMSESP_DEFAULT_BOARD_PROFILE;
    // for -D compile setting store it in NVS
    if ((String)EMSESP_DEFAULT_BOARD_PROFILE != "default" && EMSESP::nvs_.getString("boot") == "") {
        EMSESP::nvs_.putString("boot", (const char *)EMSESP_DEFAULT_BOARD_PROFILE);
    }

#ifndef EMSESP_STANDALONE
    bool psram = ESP.getPsramSize() > 0; // System::PSram() is initialized later
#else
    bool psram = false;
#endif

    if (System::load_board_profile(data, settings.board_profile.c_str())) {
        if (settings.board_profile == "CUSTOM") { //read pins, fallback to S32
            data = {(int8_t)(root["led_gpio"] | 2),
                    (int8_t)(root["dallas_gpio"] | 18),
                    (int8_t)(root["rx_gpio"] | 23),
                    (int8_t)(root["tx_gpio"] | 5),
                    (int8_t)(root["pbutton_gpio"] | 0),
                    (int8_t)(root["phy_type"] | PHY_type::PHY_TYPE_NONE),
                    (int8_t)(root["eth_power"] | 0),
                    (int8_t)(root["eth_phy_addr"] | 0),
                    (int8_t)(root["eth_clock_mode"] | 0)};
        }
        // check valid pins in this board profile
        if (!System::is_valid_gpio(data[0], psram) || !System::is_valid_gpio(data[1], psram) || !System::is_valid_gpio(data[2], psram)
            || !System::is_valid_gpio(data[3], psram) || !System::is_valid_gpio(data[4], psram) || !System::is_valid_gpio(data[6], psram)) {
            settings.board_profile = ""; // reset to factory default
        }
    } else {
        settings.board_profile = ""; // reset to factory default
    }
    if (settings.board_profile == "") {
        // unknown, check for NVS or scan for ethernet, use default E32/E32V2/S32
        settings.board_profile = EMSESP::nvs_.getString("boot");
        if (!System::load_board_profile(data, settings.board_profile.c_str())) {
#if defined(EMSESP_STANDALONE)
            settings.board_profile = "S32";
#elif CONFIG_IDF_TARGET_ESP32
            if (settings.board_profile == "" && !psram) { // empty: new test for E32
#if ESP_ARDUINO_VERSION_MAJOR < 3
                if (ETH.begin(1, 16, 23, 18, ETH_PHY_LAN8720, ETH_CLOCK_GPIO0_IN)) {
#else
                if (ETH.begin(ETH_PHY_LAN8720, 1, 23, 18, 16, ETH_CLOCK_GPIO0_IN)) {
#endif
                    EMSESP::nvs_.putString("boot", "E32");
                } else {
                    EMSESP::nvs_.putString("boot", "Test");
                }
            } else if (settings.board_profile == "Test" || psram) { // test E32V2
#if ESP_ARDUINO_VERSION_MAJOR < 3
                if (ETH.begin(0, 15, 23, 18, ETH_PHY_LAN8720, ETH_CLOCK_GPIO0_OUT)) {
#else
                if (ETH.begin(ETH_PHY_LAN8720, 0, 23, 18, 15, ETH_CLOCK_GPIO0_OUT)) {
#endif
                    EMSESP::nvs_.putString("boot", "E32V2");
                } else {
                    EMSESP::nvs_.putString("boot", "S32");
                }
            } else {
                EMSESP::nvs_.putString("boot", "S32");
            }
            ESP.restart();
#elif CONFIG_IDF_TARGET_ESP32C3
            settings.board_profile = "C3MINI";
#elif CONFIG_IDF_TARGET_ESP32S2
            settings.board_profile = "S2MINI";
#elif CONFIG_IDF_TARGET_ESP32S3
            settings.board_profile = "S32S3"; // BBQKees Gateway S3
#else
            settings.board_profile = "S32";
#endif
            System::load_board_profile(data, settings.board_profile.c_str());
        }
        EMSESP::logger().info("No board profile found. Re-setting to %s", settings.board_profile.c_str());
    } else {
        EMSESP::logger().info("Loading board profile %s", settings.board_profile.c_str());
    }

    int prev;
    reset_flags();

    // pins
    prev              = settings.led_gpio;
    settings.led_gpio = data[0];
    check_flag(prev, settings.led_gpio, ChangeFlags::LED);
    prev                 = settings.dallas_gpio;
    settings.dallas_gpio = data[1];
    check_flag(prev, settings.dallas_gpio, ChangeFlags::SENSOR);
    prev             = settings.rx_gpio;
    settings.rx_gpio = data[2];
    check_flag(prev, settings.rx_gpio, ChangeFlags::RESTART);
    prev             = settings.tx_gpio;
    settings.tx_gpio = data[3];
    check_flag(prev, settings.tx_gpio, ChangeFlags::RESTART);
    prev                  = settings.pbutton_gpio;
    settings.pbutton_gpio = data[4];
    check_flag(prev, settings.pbutton_gpio, ChangeFlags::BUTTON);
    prev              = settings.phy_type;
    settings.phy_type = data[5];
    check_flag(prev, settings.phy_type, ChangeFlags::RESTART);
    prev               = settings.eth_power;
    settings.eth_power = data[6];
    check_flag(prev, settings.eth_power, ChangeFlags::RESTART);
    prev                  = settings.eth_phy_addr;
    settings.eth_phy_addr = data[7];
    check_flag(prev, settings.eth_phy_addr, ChangeFlags::RESTART);
    prev                    = settings.eth_clock_mode;
    settings.eth_clock_mode = data[8];
    check_flag(prev, settings.eth_clock_mode, ChangeFlags::RESTART);

    // tx_mode, rx and tx pins
    prev             = settings.tx_mode;
    settings.tx_mode = root["tx_mode"] | EMSESP_DEFAULT_TX_MODE;
    check_flag(prev, settings.tx_mode, ChangeFlags::UART);

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
    if (old_syslog_host != settings.syslog_host) {
        add_flags(ChangeFlags::SYSLOG);
    }
#endif

    // temperaturesensor
    prev                     = settings.dallas_parasite;
    settings.dallas_parasite = root["dallas_parasite"] | EMSESP_DEFAULT_DALLAS_PARASITE;
    check_flag(prev, settings.dallas_parasite, ChangeFlags::SENSOR);

    // shower
    prev                  = settings.shower_timer;
    settings.shower_timer = root["shower_timer"] | EMSESP_DEFAULT_SHOWER_TIMER;
    check_flag(prev, settings.shower_timer, ChangeFlags::SHOWER);
    prev                  = settings.shower_alert;
    settings.shower_alert = root["shower_alert"] | EMSESP_DEFAULT_SHOWER_ALERT;
    check_flag(prev, settings.shower_alert, ChangeFlags::SHOWER);
    prev                          = settings.shower_alert_trigger;
    settings.shower_alert_trigger = root["shower_alert_trigger"] | EMSESP_DEFAULT_SHOWER_ALERT_TRIGGER;
    check_flag(prev, settings.shower_alert_trigger, ChangeFlags::SHOWER);
    prev                         = settings.shower_min_duration;
    settings.shower_min_duration = root["shower_min_duration"] | EMSESP_DEFAULT_SHOWER_MIN_DURATION;
    check_flag(prev, settings.shower_min_duration, ChangeFlags::SHOWER);
    prev                           = settings.shower_alert_coldshot;
    settings.shower_alert_coldshot = root["shower_alert_coldshot"] | EMSESP_DEFAULT_SHOWER_ALERT_COLDSHOT;
    check_flag(prev, settings.shower_alert_coldshot, ChangeFlags::SHOWER);

    // led
    prev              = settings.hide_led;
    settings.hide_led = root["hide_led"] | EMSESP_DEFAULT_HIDE_LED;
    check_flag(prev, settings.hide_led, ChangeFlags::LED);

    // adc
    prev                    = settings.analog_enabled;
    settings.analog_enabled = root["analog_enabled"] | EMSESP_DEFAULT_ANALOG_ENABLED;
    check_flag(prev, settings.analog_enabled, ChangeFlags::ADC);

    //
    // these need system restarts first before settings are activated...
    //
    prev                    = settings.telnet_enabled;
    settings.telnet_enabled = root["telnet_enabled"] | EMSESP_DEFAULT_TELNET_ENABLED;
    check_flag(prev, settings.telnet_enabled, ChangeFlags::RESTART);

    prev                = settings.ems_bus_id;
    settings.ems_bus_id = root["ems_bus_id"] | EMSESP_DEFAULT_EMS_BUS_ID;
    check_flag(prev, settings.ems_bus_id, ChangeFlags::RESTART);

    prev               = settings.low_clock;
    settings.low_clock = root["low_clock"];
    check_flag(prev, settings.low_clock, ChangeFlags::RESTART);

    prev                    = settings.modbus_enabled;
    settings.modbus_enabled = root["modbus_enabled"] | EMSESP_DEFAULT_MODBUS_ENABLED;
    check_flag(prev, settings.modbus_enabled, ChangeFlags::RESTART);

    prev                 = settings.modbus_port;
    settings.modbus_port = root["modbus_port"] | EMSESP_DEFAULT_MODBUS_PORT;
    check_flag(prev, settings.modbus_port, ChangeFlags::RESTART);

    prev                        = settings.modbus_max_clients;
    settings.modbus_max_clients = root["modbus_max_clients"] | EMSESP_DEFAULT_MODBUS_MAX_CLIENTS;
    check_flag(prev, settings.modbus_max_clients, ChangeFlags::RESTART);

    prev                    = settings.modbus_timeout;
    settings.modbus_timeout = root["modbus_timeout"] | EMSESP_DEFAULT_MODBUS_TIMEOUT;
    check_flag(prev, settings.modbus_timeout, ChangeFlags::RESTART);

    //
    // these may need mqtt restart to rebuild HA discovery topics
    //
    prev                 = settings.bool_format;
    settings.bool_format = root["bool_format"] | EMSESP_DEFAULT_BOOL_FORMAT;
    EMSESP::system_.bool_format(settings.bool_format);
    if (Mqtt::ha_enabled()) {
        check_flag(prev, settings.bool_format, ChangeFlags::MQTT);
    }

    prev                 = settings.enum_format;
    settings.enum_format = root["enum_format"] | EMSESP_DEFAULT_ENUM_FORMAT;
    EMSESP::system_.enum_format(settings.enum_format);
    if (Mqtt::ha_enabled()) {
        check_flag(prev, settings.enum_format, ChangeFlags::MQTT);
    }

    String old_locale = settings.locale;
    settings.locale   = root["locale"] | EMSESP_DEFAULT_LOCALE;
    EMSESP::system_.locale(settings.locale);
    if (Mqtt::ha_enabled() && old_locale != settings.locale) {
        add_flags(ChangeFlags::MQTT);
    }

    //
    // without checks or necessary restarts...
    //
    settings.trace_raw = root["trace_raw"] | EMSESP_DEFAULT_TRACELOG_RAW;
    EMSESP::trace_raw(settings.trace_raw);

    settings.notoken_api            = root["notoken_api"] | EMSESP_DEFAULT_NOTOKEN_API;
    settings.solar_maxflow          = root["solar_maxflow"] | EMSESP_DEFAULT_SOLAR_MAXFLOW;
    settings.boiler_heatingoff      = root["boiler_heatingoff"] | EMSESP_DEFAULT_BOILER_HEATINGOFF;
    settings.remote_timeout         = root["remote_timeout"] | EMSESP_DEFAULT_REMOTE_TIMEOUT;
    settings.remote_timeout_enabled = root["remote_timeout_en"] | EMSESP_DEFAULT_REMOTE_TIMEOUT_EN;
    emsesp::Roomctrl::set_timeout(settings.remote_timeout_enabled ? settings.remote_timeout : 0);

    settings.fahrenheit = root["fahrenheit"];
    EMSESP::system_.fahrenheit(settings.fahrenheit);

    settings.readonly_mode = root["readonly_mode"];
    EMSESP::system_.readonly_mode(settings.readonly_mode);

    settings.bool_dashboard = root["bool_dashboard"] | EMSESP_DEFAULT_BOOL_FORMAT;
    EMSESP::system_.bool_dashboard(settings.bool_dashboard);

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

    if (WebSettings::has_flags(WebSettings::ChangeFlags::SENSOR)) {
        EMSESP::temperaturesensor_.start();
    }

    if (WebSettings::has_flags(WebSettings::ChangeFlags::UART)) {
        EMSESP::uart_init();
    }

    if (WebSettings::has_flags(WebSettings::ChangeFlags::SYSLOG)) {
        EMSESP::system_.syslog_init(); // re-start (or stop)
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

    if (WebSettings::has_flags(WebSettings::ChangeFlags::MQTT)) {
        emsesp::Mqtt::reset_mqtt(); // reload MQTT, init HA etc
    }

    WebSettings::reset_flags();
}

void WebSettingsService::begin() {
    _fsPersistence.readFromFS();
    WebSettings::reset_flags();
}

void WebSettingsService::save() {
    _fsPersistence.writeToFS();
}

// build the json profile to send back
void WebSettingsService::board_profile(AsyncWebServerRequest * request) {
    if (request->hasParam("boardProfile")) {
        std::string board_profile = request->getParam("boardProfile")->value().c_str();

        auto *     response = new AsyncJsonResponse(false);
        JsonObject root     = response->getRoot();

        std::vector<int8_t> data; // led, dallas, rx, tx, button, phy_type, eth_power, eth_phy_addr, eth_clock_mode
        (void)System::load_board_profile(data, board_profile);
        root["board_profile"]  = board_profile;
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

    AsyncWebServerResponse * response = request->beginResponse(200);
    request->send(response);
}

} // namespace emsesp
