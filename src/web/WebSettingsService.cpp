/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2025  emsesp.org - proddy, MichaelDvP
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
    securityManager->addEndpoint(server, EMSESP_BOARD_PROFILE_SERVICE_PATH, AuthenticationPredicates::IS_AUTHENTICATED, [this](AsyncWebServerRequest * request) {
        board_profile(request);
    });

    addUpdateHandler([this] { onUpdate(); }, false);
}

void WebSettings::read(WebSettings & settings, JsonObject root) {
    root["version"]               = settings.version;
    root["board_profile"]         = settings.board_profile;
    root["platform"]              = EMSESP_PLATFORM;
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
    root["led_type"]              = settings.led_type;
    root["low_clock"]             = settings.low_clock;
    root["telnet_enabled"]        = settings.telnet_enabled;
    root["notoken_api"]           = settings.notoken_api;
    root["readonly_mode"]         = settings.readonly_mode;
    root["analog_enabled"]        = settings.analog_enabled;
    root["pbutton_gpio"]          = settings.pbutton_gpio;
    root["solar_maxflow"]         = settings.solar_maxflow;
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
    root["modbus_enabled"]        = settings.modbus_enabled;
    root["modbus_port"]           = settings.modbus_port;
    root["modbus_max_clients"]    = settings.modbus_max_clients;
    root["modbus_timeout"]        = settings.modbus_timeout;
    root["developer_mode"]        = settings.developer_mode;
}

// call on initialization and also when settings are updated/saved via web or console
// note, settings is empty when the service starts
StateUpdateResult WebSettings::update(JsonObject root, WebSettings & settings) {
    // make a copy of the settings to compare to later
    const WebSettings original_settings(settings);

    reset_flags();

    settings.version       = root["version"] | EMSESP_DEFAULT_VERSION; // save the version, we use it later in System::check_upgrade()
    settings.board_profile = root["board_profile"] | EMSESP_DEFAULT_BOARD_PROFILE;

    // get current values that are related to the board profile
    settings.led_gpio       = root["led_gpio"];
    settings.dallas_gpio    = root["dallas_gpio"];
    settings.rx_gpio        = root["rx_gpio"];
    settings.tx_gpio        = root["tx_gpio"];
    settings.pbutton_gpio   = root["pbutton_gpio"];
    settings.phy_type       = root["phy_type"];
    settings.eth_power      = root["eth_power"];
    settings.eth_phy_addr   = root["eth_phy_addr"];
    settings.eth_clock_mode = root["eth_clock_mode"];
    settings.led_type       = root["led_type"]; // 1 = RGB-LED

#if defined(EMSESP_DEBUG)
    EMSESP::logger().debug("NVS boot value=[%s], current board_profile=[%s], new board_profile=[%s]",
                           EMSESP::nvs_.getString("boot").c_str(),
                           original_settings.board_profile.c_str(),
                           settings.board_profile.c_str());
#endif

    // see if the user has changed the board profile
    // this will set: led_gpio, dallas_gpio, rx_gpio, tx_gpio, pbutton_gpio, phy_type, eth_power, eth_phy_addr, eth_clock_mode, led_type
    // this will always run when EMS-ESP starts since original_settings{} is empty
    if (original_settings.board_profile != settings.board_profile) {
        set_board_profile(settings);
        add_flags(ChangeFlags::RESTART);
    }

    // if any of the GPIOs have changed and re-validate them
    bool have_valid_gpios = true;

    // free old gpios from used list to allow remapping
    EMSESP::system_.remove_gpio(original_settings.led_gpio);
    EMSESP::system_.remove_gpio(original_settings.dallas_gpio);
    EMSESP::system_.remove_gpio(original_settings.pbutton_gpio);
    EMSESP::system_.remove_gpio(original_settings.rx_gpio);
    EMSESP::system_.remove_gpio(original_settings.tx_gpio);

    // now add new gpio assignment, start with rx/tx
    check_flag(original_settings.rx_gpio, settings.rx_gpio, ChangeFlags::UART);
    have_valid_gpios = have_valid_gpios && EMSESP::system_.add_gpio(settings.rx_gpio, "UART Rx");

    check_flag(original_settings.tx_gpio, settings.tx_gpio, ChangeFlags::UART);
    have_valid_gpios = have_valid_gpios && EMSESP::system_.add_gpio(settings.tx_gpio, "UART Tx");

    check_flag(original_settings.led_gpio, settings.led_gpio, ChangeFlags::LED);
    if (settings.led_gpio != 0 && !EMSESP::system_.add_gpio(settings.led_gpio, "LED")) {
        settings.led_gpio = 0; // 0 means disabled
        have_valid_gpios  = false;
    }

    check_flag(original_settings.dallas_gpio, settings.dallas_gpio, ChangeFlags::TEMPERATURE_SENSOR);
    if (settings.dallas_gpio != 0 && !EMSESP::system_.add_gpio(settings.dallas_gpio, "Dallas")) {
        settings.dallas_gpio = 0; // 0 means disabled
        have_valid_gpios     = false;
    }

    check_flag(original_settings.pbutton_gpio, settings.pbutton_gpio, ChangeFlags::BUTTON);
    have_valid_gpios = have_valid_gpios && EMSESP::system_.add_gpio(settings.pbutton_gpio, "Button");

    check_flag(original_settings.phy_type, settings.phy_type, ChangeFlags::RESTART);
    // ETH has changed, so we need to check the ethernet pins. Only if ETH is being used.
    if (settings.phy_type != PHY_type::PHY_TYPE_NONE) {
        check_flag(original_settings.eth_power, settings.eth_power, ChangeFlags::RESTART);
        check_flag(original_settings.eth_clock_mode, settings.eth_clock_mode, ChangeFlags::RESTART);
        if (settings.eth_power != -1) { // Ethernet Power -1 means disabled
            EMSESP::system_.remove_gpio(settings.eth_power, true);
        }
        // remove the ethernet pins from valid list, regardless of whether the GPIOs are valid or not
        EMSESP::system_.remove_gpio(23, true); // MDC
        EMSESP::system_.remove_gpio(18, true); // MDIO
        if (settings.eth_clock_mode < 2) {
            EMSESP::system_.remove_gpio(0, true); // ETH.clock input
        } else if (settings.eth_clock_mode == 2) {
            EMSESP::system_.remove_gpio(16, true); // ETH.clock output
        } else if (settings.eth_clock_mode == 3) {
            EMSESP::system_.remove_gpio(17, true); // ETH.clock output
        }
    }

    // check if the LED type, eth_phy_addr or eth_clock_mode have changed
    check_flag(original_settings.led_type, settings.led_type, ChangeFlags::LED);
    check_flag(original_settings.eth_phy_addr, settings.eth_phy_addr, ChangeFlags::RESTART);
    check_flag(original_settings.eth_clock_mode, settings.eth_clock_mode, ChangeFlags::RESTART);

    // tx_mode
    settings.tx_mode = root["tx_mode"] | EMSESP_DEFAULT_TX_MODE;
    check_flag(original_settings.tx_mode, settings.tx_mode, ChangeFlags::UART);

    // syslog
    settings.syslog_enabled = root["syslog_enabled"] | EMSESP_DEFAULT_SYSLOG_ENABLED;
    check_flag(original_settings.syslog_enabled, settings.syslog_enabled, ChangeFlags::SYSLOG);
    settings.syslog_level = root["syslog_level"] | EMSESP_DEFAULT_SYSLOG_LEVEL;
    check_flag(original_settings.syslog_level, settings.syslog_level, ChangeFlags::SYSLOG);
    settings.syslog_mark_interval = root["syslog_mark_interval"] | EMSESP_DEFAULT_SYSLOG_MARK_INTERVAL;
    check_flag(original_settings.syslog_mark_interval, settings.syslog_mark_interval, ChangeFlags::SYSLOG);
    settings.syslog_port = root["syslog_port"] | EMSESP_DEFAULT_SYSLOG_PORT;
    check_flag(original_settings.syslog_port, settings.syslog_port, ChangeFlags::SYSLOG);

#ifndef EMSESP_STANDALONE
    settings.syslog_host = root["syslog_host"] | EMSESP_DEFAULT_SYSLOG_HOST;
    if (original_settings.syslog_host != settings.syslog_host) {
        add_flags(ChangeFlags::SYSLOG);
    }
#endif

    // temperature sensor
    settings.dallas_parasite = root["dallas_parasite"] | EMSESP_DEFAULT_DALLAS_PARASITE;
    check_flag(original_settings.dallas_parasite, settings.dallas_parasite, ChangeFlags::TEMPERATURE_SENSOR);

    // shower
    settings.shower_timer = root["shower_timer"] | EMSESP_DEFAULT_SHOWER_TIMER;
    check_flag(original_settings.shower_timer, settings.shower_timer, ChangeFlags::SHOWER);
    settings.shower_alert = root["shower_alert"] | EMSESP_DEFAULT_SHOWER_ALERT;
    check_flag(original_settings.shower_alert, settings.shower_alert, ChangeFlags::SHOWER);
    settings.shower_alert_trigger = root["shower_alert_trigger"] | EMSESP_DEFAULT_SHOWER_ALERT_TRIGGER;
    check_flag(original_settings.shower_alert_trigger, settings.shower_alert_trigger, ChangeFlags::SHOWER);
    settings.shower_min_duration = root["shower_min_duration"] | EMSESP_DEFAULT_SHOWER_MIN_DURATION;
    check_flag(original_settings.shower_min_duration, settings.shower_min_duration, ChangeFlags::SHOWER);
    settings.shower_alert_coldshot = root["shower_alert_coldshot"] | EMSESP_DEFAULT_SHOWER_ALERT_COLDSHOT;
    check_flag(original_settings.shower_alert_coldshot, settings.shower_alert_coldshot, ChangeFlags::SHOWER);

    // LED
    settings.hide_led = root["hide_led"] | EMSESP_DEFAULT_HIDE_LED;
    check_flag(original_settings.hide_led, settings.hide_led, ChangeFlags::LED);

    // adc
    settings.analog_enabled = root["analog_enabled"] | EMSESP_DEFAULT_ANALOG_ENABLED;
    check_flag(original_settings.analog_enabled, settings.analog_enabled, ChangeFlags::ANALOG_SENSOR);

    // telnet, ems bus id and low clock
    settings.telnet_enabled = root["telnet_enabled"] | EMSESP_DEFAULT_TELNET_ENABLED;
    check_flag(original_settings.telnet_enabled, settings.telnet_enabled, ChangeFlags::RESTART);
    settings.ems_bus_id = root["ems_bus_id"] | EMSESP_DEFAULT_EMS_BUS_ID;
    check_flag(original_settings.ems_bus_id, settings.ems_bus_id, ChangeFlags::RESTART);
    settings.low_clock = root["low_clock"];
    check_flag(original_settings.low_clock, settings.low_clock, ChangeFlags::RESTART);

    // Modbus settings
    settings.modbus_enabled = root["modbus_enabled"] | EMSESP_DEFAULT_MODBUS_ENABLED;
    check_flag(original_settings.modbus_enabled, settings.modbus_enabled, ChangeFlags::RESTART);
    settings.modbus_port = root["modbus_port"] | EMSESP_DEFAULT_MODBUS_PORT;
    check_flag(original_settings.modbus_port, settings.modbus_port, ChangeFlags::RESTART);
    settings.modbus_max_clients = root["modbus_max_clients"] | EMSESP_DEFAULT_MODBUS_MAX_CLIENTS;
    check_flag(original_settings.modbus_max_clients, settings.modbus_max_clients, ChangeFlags::RESTART);
    settings.modbus_timeout = root["modbus_timeout"] | EMSESP_DEFAULT_MODBUS_TIMEOUT;
    check_flag(original_settings.modbus_timeout, settings.modbus_timeout, ChangeFlags::RESTART);

    //
    // these may need mqtt restart to rebuild HA discovery topics
    //
    settings.bool_format = root["bool_format"] | EMSESP_DEFAULT_BOOL_FORMAT;
    EMSESP::system_.bool_format(settings.bool_format);
    if (Mqtt::ha_enabled()) {
        check_flag(original_settings.bool_format, settings.bool_format, ChangeFlags::MQTT);
    }

    settings.enum_format = root["enum_format"] | EMSESP_DEFAULT_ENUM_FORMAT;
    EMSESP::system_.enum_format(settings.enum_format);
    if (Mqtt::ha_enabled()) {
        check_flag(original_settings.enum_format, settings.enum_format, ChangeFlags::MQTT);
    }

    settings.locale = root["locale"] | EMSESP_DEFAULT_LOCALE;
    EMSESP::system_.locale(settings.locale);
    if (Mqtt::ha_enabled() && original_settings.locale != settings.locale) {
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
    Roomctrl::set_timeout(settings.remote_timeout_enabled ? settings.remote_timeout : 0);

    settings.fahrenheit = root["fahrenheit"];
    EMSESP::system_.fahrenheit(settings.fahrenheit);

    settings.readonly_mode = root["readonly_mode"];
    EMSESP::system_.readonly_mode(settings.readonly_mode);

    settings.developer_mode = root["developer_mode"];
    EMSESP::system_.developer_mode(settings.developer_mode);

    settings.bool_dashboard = root["bool_dashboard"] | EMSESP_DEFAULT_BOOL_FORMAT;
    EMSESP::system_.bool_dashboard(settings.bool_dashboard);

    settings.weblog_level   = root["weblog_level"] | EMSESP_DEFAULT_WEBLOG_LEVEL;
    settings.weblog_compact = root["weblog_compact"] | EMSESP_DEFAULT_WEBLOG_COMPACT;

    // if no psram limit weblog buffer to 25 messages
    if (EMSESP::system_.PSram() > 0) {
        settings.weblog_buffer = root["weblog_buffer"] | EMSESP_DEFAULT_WEBLOG_BUFFER;
    } else {
        settings.weblog_buffer = root["weblog_buffer"] | 25; // limit to 25 messages if no psram
    }

    // save the setting internally, for reference later
    EMSESP::system_.store_settings(settings);

    // save the settings if changed from the webUI
    // if we encountered an invalid GPIO, don't save settings and report the error
    if (!have_valid_gpios) {
#if defined(EMSESP_DEBUG)
        EMSESP::logger().debug("Warning: one or more GPIOs are invalid");
#endif
        EMSESP::system_.systemStatus(SYSTEM_STATUS::SYSTEM_STATUS_INVALID_GPIO);
        return StateUpdateResult::CHANGED; // save the settings anyway, without restart
    }

    if (has_flags(WebSettings::ChangeFlags::RESTART)) {
        return StateUpdateResult::CHANGED_RESTART;
    }

    return StateUpdateResult::CHANGED;
}

// this is called after any of the settings have been persisted to the filesystem
// either via the Web UI or via the Console
void WebSettingsService::onUpdate() {
    // skip if we're restarting anyway

    if (WebSettings::has_flags(WebSettings::ChangeFlags::RESTART)) {
        return;
    }

    if (WebSettings::has_flags(WebSettings::ChangeFlags::SHOWER)) {
        EMSESP::shower_.start();
    }

    if (WebSettings::has_flags(WebSettings::ChangeFlags::TEMPERATURE_SENSOR)) {
        EMSESP::temperaturesensor_.start();
    }

    if (WebSettings::has_flags(WebSettings::ChangeFlags::UART)) {
        EMSESP::system_.uart_init();
    }

    if (WebSettings::has_flags(WebSettings::ChangeFlags::SYSLOG)) {
        EMSESP::system_.syslog_init(); // re-start (or stop)
    }

    if (WebSettings::has_flags(WebSettings::ChangeFlags::ANALOG_SENSOR)) {
        EMSESP::analogsensor_.start();
    }

    if (WebSettings::has_flags(WebSettings::ChangeFlags::BUTTON)) {
        EMSESP::system_.button_init();
    }

    if (WebSettings::has_flags(WebSettings::ChangeFlags::LED)) {
        EMSESP::system_.led_init();
    }

    if (WebSettings::has_flags(WebSettings::ChangeFlags::MQTT)) {
        Mqtt::reset_mqtt(); // reload MQTT, init HA etc
    }

    WebSettings::reset_flags();
}

void WebSettingsService::begin() {
    _fsPersistence.readFromFS();
}

void WebSettingsService::save() {
    _fsPersistence.writeToFS();
}

// send the board profile as JSON
void WebSettingsService::board_profile(AsyncWebServerRequest * request) {
    if (request->hasParam("boardProfile")) {
        std::string board_profile = request->getParam("boardProfile")->value().c_str();

        auto *     response = new AsyncJsonResponse(false);
        JsonObject root     = response->getRoot();

        // 0=led, 1=dallas, 2=rx, 3=tx, 4=button, 5=phy_type, 6=eth_power, 7=eth_phy_addr, 8=eth_clock_mode, 9=led_type
        std::vector<int8_t> data;
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
        root["led_type"]       = data[9];

        response->setLength();
        request->send(response);
        return;
    }

    AsyncWebServerResponse * response = request->beginResponse(200);
    request->send(response);
}

// loads the board profile to set the gpios
// if the board profile is not found, or default, it will try to autodetect the board profile
void WebSettings::set_board_profile(WebSettings & settings) {
    // The optional NVS boot value has priority and overrides any board_profile setting.
    // This is only done for BBQKees boards
    // Note 1: we never set the NVS boot value in the code - this is done on initial pre-loading
    // Note 2: The board profile is dynamically changed for the session, but the value in the settings file on the FS remains untouched
    if (EMSESP::system_.getBBQKeesGatewayDetails(FUSE_VALUE::MFG).startsWith("BBQKees")) {
        String bbq_board = EMSESP::system_.getBBQKeesGatewayDetails(FUSE_VALUE::BOARD);
        if (!bbq_board.isEmpty()) {
#if defined(EMSESP_DEBUG)
            EMSESP::logger().info("Overriding board profile with fuse value %s", bbq_board.c_str());
#endif
            settings.board_profile = bbq_board;
        }
    }

    // if it's CUSTOM no need to load the board profile from the settings
    // as it's already set
    if (settings.board_profile == "CUSTOM") {
        EMSESP::logger().info("Using CUSTOM board profile");
        return;
    }

    // load the board profile into the data vector
    // 0=led, 1=dallas, 2=rx, 3=tx, 4=button, 5=phy_type, 6=eth_power, 7=eth_phy_addr, 8=eth_clock_mode, 9=led_type
    std::vector<int8_t> data(10, 0); // initialize with 0 for all values
    if (settings.board_profile != "default") {
        if (!System::load_board_profile(data, settings.board_profile.c_str())) {
#if defined(EMSESP_DEBUG)
            EMSESP::logger().debug("Unable to identify board profile %s", settings.board_profile.c_str());
#endif
            settings.board_profile = "default"; // can't find profile, fallback to "default"
        }
    }

    // we still don't have a valid board profile. Let's see if we can determine one from the build config or hardware
    if (settings.board_profile == "default") {
        EMSESP::logger().info("Autodetecting board profile");
#if CONFIG_IDF_TARGET_ESP32
        // check for no PSRAM, could be a E32 or S32?
        if (!ESP.getPsramSize()) {
#if ESP_ARDUINO_VERSION_MAJOR < 3
            if (ETH.begin(1, 16, 23, 18, ETH_PHY_LAN8720, ETH_CLOCK_GPIO0_IN)) {
#else
            if (ETH.begin(ETH_PHY_LAN8720, 1, 23, 18, 16, ETH_CLOCK_GPIO0_IN)) {
#endif
                settings.board_profile = "E32"; // Ethernet without PSRAM
            } else {
                settings.board_profile = "S32"; // ESP32 standard WiFi without PSRAM
            }
        } else {
// check for boards with PSRAM, could be a E32V2 otherwise default back to the S32
#if ESP_ARDUINO_VERSION_MAJOR < 3
            if (ETH.begin(0, 15, 23, 18, ETH_PHY_LAN8720, ETH_CLOCK_GPIO0_OUT)) {
#else
            if (ETH.begin(ETH_PHY_LAN8720, 0, 23, 18, 15, ETH_CLOCK_GPIO0_OUT)) {
#endif

                if (analogReadMilliVolts(39) > 700) {   // core voltage > 2.6V
                    settings.board_profile = "E32V2_2"; // Ethernet, PSRAM, internal sensors
                } else {
                    settings.board_profile = "E32V2"; // Ethernet and PSRAM
                }
            } else {
                settings.board_profile = "S32"; // ESP32 standard WiFi with PSRAM
            }
        }
// override if we know the target from the build config like C3, S2, S3 etc..
#elif CONFIG_IDF_TARGET_ESP32C3
        settings.board_profile = "C3MINI";
#elif CONFIG_IDF_TARGET_ESP32S2
        settings.board_profile = "S2MINI";
#elif CONFIG_IDF_TARGET_ESP32S3
        settings.board_profile = "S32S3"; // BBQKees Gateway S3
#endif
        // apply the new board profile setting
        System::load_board_profile(data, settings.board_profile.c_str());
    }

    EMSESP::logger().info("Loaded board profile %s", settings.board_profile.c_str());

    // apply the new board profile settings
    // 0=led, 1=dallas, 2=rx, 3=tx, 4=button, 5=phy_type, 6=eth_power, 7=eth_phy_addr, 8=eth_clock_mode, 9=led_type
    settings.led_gpio       = data[0]; // LED GPIO
    settings.dallas_gpio    = data[1]; // Dallas GPIO
    settings.rx_gpio        = data[2]; // UART Rx GPIO
    settings.tx_gpio        = data[3]; // UART Tx GPIO
    settings.pbutton_gpio   = data[4]; // Button GPIO
    settings.phy_type       = data[5]; // PHY Type
    settings.eth_power      = data[6]; // Ethernet Power GPIO
    settings.eth_phy_addr   = data[7]; // Ethernet PHY Address
    settings.eth_clock_mode = data[8]; // Ethernet Clock Mode
    settings.led_type       = data[9]; // LED Type
}


// returns true if the value was changed
bool WebSettings::check_flag(int prev_v, int new_v, uint8_t flag) {
    if (prev_v != new_v) {
        add_flags(flag);
#if defined(EMSESP_DEBUG)
        // EMSESP::logger().debug("check_flag: flag %d, prev_v=%d, new_v=%d", flag, prev_v, new_v);
#endif
        return true;
    }
    return false;
}

void WebSettings::add_flags(uint8_t flags) {
    flags_ |= flags;
}

bool WebSettings::has_flags(uint8_t flags) {
    return (flags_ & flags) == flags;
}

void WebSettings::reset_flags() {
    flags_ = ChangeFlags::NONE;
}

uint8_t WebSettings::get_flags() {
    return flags_;
}

} // namespace emsesp
