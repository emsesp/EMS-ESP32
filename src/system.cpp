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

#include "system.h"
#include "emsesp.h" // for send_raw_telegram() command

#if defined(EMSESP_TEST)
#include "test/test.h"
#endif

namespace emsesp {

uuid::log::Logger System::logger_{F_(system), uuid::log::Facility::KERN};

#ifndef EMSESP_STANDALONE
uuid::syslog::SyslogService System::syslog_;
#endif

// init statics
uint32_t System::heap_start_ = 1; // avoid using 0 to divide-by-zero later
PButton  System::myPButton_;

// send on/off to a gpio pin
// value: true = HIGH, false = LOW
// e.g. http://ems-esp/api?device=system&cmd=pin&data=1&id=2
bool System::command_pin(const char * value, const int8_t id) {
    if (id < 0) {
        return false;
    }

    bool v = false;
    if (Helpers::value2bool(value, v)) {
        pinMode(id, OUTPUT);
        digitalWrite(id, v);
        LOG_INFO(F("GPIO %d set to %s"), id, v ? "HIGH" : "LOW");
        return true;
    }

    return false;
}

// send raw to ems
bool System::command_send(const char * value, const int8_t id) {
    EMSESP::send_raw_telegram(value); // ignore id
    return true;
}

// fetch device values
bool System::command_fetch(const char * value, const int8_t id) {
    LOG_INFO(F("Requesting data from EMS devices"));
    EMSESP::fetch_device_values();
    return true;
}

// mqtt publish
bool System::command_publish(const char * value, const int8_t id) {
    std::string ha(10, '\0');
    if (Helpers::value2string(value, ha)) {
        if (ha == "ha") {
            EMSESP::publish_all(true); // includes HA
            LOG_INFO(F("Publishing all data to MQTT, including HA configs"));
            return true;
        }
    }

    EMSESP::publish_all(); // ignore value and id
    LOG_INFO(F("Publishing all data to MQTT"));
    return true;
}

// restart EMS-ESP
void System::restart() {
    LOG_INFO(F("Restarting system..."));
    Shell::loop_all();
    delay(1000); // wait a second
#ifndef EMSESP_STANDALONE
    ESP.restart();
#endif
}

// saves all settings
void System::wifi_reconnect() {
    LOG_INFO(F("Wifi reconnecting..."));
    Shell::loop_all();
    EMSESP::console_.loop();
    delay(1000);                                                                   // wait a second
    EMSESP::webSettingsService.save();                                             // local settings
    EMSESP::esp8266React.getNetworkSettingsService()->callUpdateHandlers("local"); // in case we've changed ssid or password
}

// format fs
// format the FS. Wipes everything.
void System::format(uuid::console::Shell & shell) {
    auto msg = F("Formatting file system. This will reset all settings to their defaults");
    shell.logger().warning(msg);
    shell.flush();

    EMSuart::stop();

#ifndef EMSESP_STANDALONE
    LITTLEFS.format();
#endif

    System::restart();
}

void System::syslog_init(bool refresh) {
    if (refresh) {
        get_settings();
    }

#ifndef EMSESP_STANDALONE
    // check for empty hostname
    IPAddress addr;
    if (!addr.fromString(syslog_host_.c_str())) {
        syslog_enabled_ = false;
    }

    // in case service is still running, this flushes the queue - https://github.com/proddy/EMS-ESP/issues/496
    if (!syslog_enabled_) {
        syslog_.log_level((uuid::log::Level)-1);
        syslog_.mark_interval(0);
        syslog_.destination((IPAddress)((uint32_t)0));
        return;
    }

    // start & configure syslog
    syslog_.start();
    syslog_.log_level((uuid::log::Level)syslog_level_);
    syslog_.mark_interval(syslog_mark_interval_);
    syslog_.destination(addr);
    syslog_.hostname(hostname_.c_str());

    EMSESP::logger().info(F("Syslog started"));
#endif
}

// read all the settings from the config files and store locally
void System::get_settings() {
    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        // BUTTON
        pbutton_gpio_ = settings.pbutton_gpio;

        // ADC
        analog_enabled_ = settings.analog_enabled;

        // SYSLOG
        syslog_enabled_       = settings.syslog_enabled;
        syslog_level_         = settings.syslog_level;
        syslog_mark_interval_ = settings.syslog_mark_interval;
        syslog_host_          = settings.syslog_host;

        // LED
        hide_led_ = settings.hide_led;
        led_gpio_ = settings.led_gpio;
    });

    EMSESP::esp8266React.getNetworkSettingsService()->read([&](NetworkSettings & networkSettings) {
        hostname(networkSettings.hostname.c_str());
        LOG_INFO(F("System %s booted (EMS-ESP version %s)"), networkSettings.hostname.c_str(), EMSESP_APP_VERSION); // print boot message
        ethernet_profile_ = networkSettings.ethernet_profile;
    });
}

// adjust WiFi settings
// this for problem solving mesh and connection issues, and also get EMS bus-powered more stable by lowering power
void System::wifi_tweak() {
#if defined(EMSESP_WIFI_TWEAK)
    // Default Tx Power is 80 = 20dBm <-- default
    // WIFI_POWER_19_5dBm = 78,// 19.5dBm
    // WIFI_POWER_19dBm = 76,// 19dBm
    // WIFI_POWER_18_5dBm = 74,// 18.5dBm
    // WIFI_POWER_17dBm = 68,// 17dBm
    // WIFI_POWER_15dBm = 60,// 15dBm
    // WIFI_POWER_13dBm = 52,// 13dBm
    // WIFI_POWER_11dBm = 44,// 11dBm
    // WIFI_POWER_8_5dBm = 34,// 8.5dBm
    // WIFI_POWER_7dBm = 28,// 7dBm
    // WIFI_POWER_5dBm = 20,// 5dBm
    // WIFI_POWER_2dBm = 8,// 2dBm
    // WIFI_POWER_MINUS_1dBm = -4// -1dBm
    wifi_power_t p1  = WiFi.getTxPower();
    (void) WiFi.setTxPower(WIFI_POWER_19_5dBm);
    wifi_power_t p2  = WiFi.getTxPower();
    bool         s1  = WiFi.getSleep();
    WiFi.setSleep(false); // turn off sleep - WIFI_PS_NONE
    bool s2 = WiFi.getSleep();
    LOG_INFO(F("Adjusting Wifi - Tx power %d->%d, Sleep %d->%d"), p1, p2, s1, s2);
#endif
}

// first call. Sets memory and starts up the UART Serial bridge
void System::start(uint32_t heap_start) {
#if defined(EMSESP_DEBUG)
    show_mem("Startup");
#endif

    // set the inital free mem, only on first boot
    if (heap_start_ < 2) {
        heap_start_ = heap_start;
    }

    // load in all the settings first
    get_settings();

    commands_init();     // console & api commands
    led_init(false);     // init LED
    adc_init(false);     // analog ADC
    syslog_init(false);  // init SysLog
    button_init(false);  // the special button
    network_init(false); // network

    EMSESP::init_tx(); // start UART
}

// adc and bluetooth
void System::adc_init(bool refresh) {
    if (refresh) {
        get_settings();
    }
#ifndef EMSESP_STANDALONE
    // setCpuFrequencyMhz(160); // default is 240

    // disable bluetooth
    btStop();
    esp_bt_controller_disable();
    if (!analog_enabled_) {
        adc_power_off(); // turn off ADC to save power if not needed
    }
#endif
}

// button single click
void System::button_OnClick(PButton & b) {
    LOG_DEBUG(F("Button pressed - single click"));
}

// button double click
void System::button_OnDblClick(PButton & b) {
    LOG_DEBUG(F("Button pressed - double click - reconnect"));
    EMSESP::system_.wifi_reconnect();
}

// button long press
void System::button_OnLongPress(PButton & b) {
    LOG_DEBUG(F("Button pressed - long press"));
}

// button indefinite press
void System::button_OnVLongPress(PButton & b) {
    LOG_DEBUG(F("Button pressed - very long press"));
#ifndef EMSESP_STANDALONE
    LOG_WARNING(F("Performing factory reset..."));
    EMSESP::console_.loop();

#ifdef EMSESP_TEST
    Test::listDir(LITTLEFS, FS_CONFIG_DIRECTORY, 3);
#endif

    EMSESP::esp8266React.factoryReset();
#endif
}

// push button
void System::button_init(bool refresh) {
    if (refresh) {
        get_settings();
    }

    // Allow 0 for Boot-button on NodeMCU-32s?
    // if (pbutton_gpio_) {
    if (!myPButton_.init(pbutton_gpio_, HIGH)) {
        LOG_INFO(F("External multi-functional button not detected"));
    } else {
        LOG_INFO(F("External multi-functional button enabled"));
    }

    myPButton_.onClick(BUTTON_Debounce, button_OnClick);
    myPButton_.onDblClick(BUTTON_DblClickDelay, button_OnDblClick);
    myPButton_.onLongPress(BUTTON_LongPressDelay, button_OnLongPress);
    myPButton_.onVLongPress(BUTTON_VLongPressDelay, button_OnVLongPress);
    // }
}

// set the LED to on or off when in normal operating mode
void System::led_init(bool refresh) {
    if (refresh) {
        get_settings();
    }

    if (led_gpio_) {
        pinMode(led_gpio_, OUTPUT);                            // 0 means disabled
        digitalWrite(led_gpio_, hide_led_ ? !LED_ON : LED_ON); // LED on, for ever
    }
}

// returns true if OTA is uploading
bool System::upload_status() {
#if defined(EMSESP_STANDALONE)
    return false;
#else
    return upload_status_ || Update.isRunning();
#endif
}

void System::upload_status(bool in_progress) {
    // if we've just started an upload
    if ((!upload_status_) && (in_progress)) {
        EMSuart::stop();
    }
    upload_status_ = in_progress;
}

// checks system health and handles LED flashing wizardry
void System::loop() {
#ifndef EMSESP_STANDALONE
    myPButton_.check(); // check button press

    if (syslog_enabled_) {
        syslog_.loop();
    }

    led_monitor();  // check status and report back using the LED
    system_check(); // check system health
    if (analog_enabled_) {
        measure_analog();
    }

    // send out heartbeat
    uint32_t currentMillis = uuid::get_uptime();
    if (!last_heartbeat_ || (currentMillis - last_heartbeat_ > SYSTEM_HEARTBEAT_INTERVAL)) {
        last_heartbeat_ = currentMillis;
        send_heartbeat();
    }

    /*
#ifndef EMSESP_STANDALONE
#if defined(EMSESP_DEBUG)
    static uint32_t last_memcheck_ = 0;
    if (currentMillis - last_memcheck_ > 10000) { // 10 seconds
        last_memcheck_ = currentMillis;
        show_mem("core");
    }
#endif
#endif
*/

#endif
}

void System::show_mem(const char * note) {
#ifndef EMSESP_STANDALONE
    static uint32_t old_free_heap = 0;
    uint32_t        free_heap     = ESP.getFreeHeap();
    LOG_INFO(F("(%s) Free heap: %lu (~%lu)"), note, free_heap, (uint32_t)Helpers::abs(free_heap - old_free_heap));
    old_free_heap = free_heap;
#endif
}

// send periodic MQTT message with system information
void System::send_heartbeat() {
    // don't send heartbeat if WiFi or MQTT is not connected
    if (!Mqtt::connected()) {
        return;
    }

    int8_t rssi = wifi_quality();
    if (rssi == -1) {
        return;
    }

    StaticJsonDocument<EMSESP_JSON_SIZE_SMALL> doc;

    uint8_t ems_status = EMSESP::bus_status();
    if (ems_status == EMSESP::BUS_STATUS_TX_ERRORS) {
        doc["status"] = FJSON("txerror");
    } else if (ems_status == EMSESP::BUS_STATUS_CONNECTED) {
        doc["status"] = FJSON("connected");
    } else {
        doc["status"] = FJSON("disconnected");
    }

    doc["rssi"]        = rssi;
    doc["uptime"]      = uuid::log::format_timestamp_ms(uuid::get_uptime_ms(), 3);
    doc["uptime_sec"]  = uuid::get_uptime_sec();
    doc["mqttfails"]   = Mqtt::publish_fails();
    doc["rxsent"]      = EMSESP::rxservice_.telegram_count();
    doc["rxfails"]     = EMSESP::rxservice_.telegram_error_count();
    doc["txread"]      = EMSESP::txservice_.telegram_read_count();
    doc["txwrite"]     = EMSESP::txservice_.telegram_write_count();
    doc["txfails"]     = EMSESP::txservice_.telegram_fail_count();
    doc["dallasfails"] = EMSESP::sensor_fails();
#ifndef EMSESP_STANDALONE
    doc["freemem"] = ESP.getFreeHeap();
#endif

    if (analog_enabled_) {
        doc["adc"] = analog_;
    }

    Mqtt::publish(F("heartbeat"), doc.as<JsonObject>()); // send to MQTT with retain off. This will add to MQTT queue.
}

// measure and moving average adc
void System::measure_analog() {
    static uint32_t measure_last_ = 0;

    if (!measure_last_ || (uint32_t)(uuid::get_uptime() - measure_last_) >= SYSTEM_MEASURE_ANALOG_INTERVAL) {
        measure_last_ = uuid::get_uptime();
#if defined(ESP32)
        uint16_t a = analogRead(36);
#else
        uint16_t a = 0; // standalone
#endif
        static uint32_t sum_ = 0;

        if (!analog_) { // init first time
            analog_ = a;
            sum_    = a * 512;
        } else { // simple moving average filter
            sum_    = (sum_ * 511) / 512 + a;
            analog_ = sum_ / 512;
        }
    }
}

// sets rate of led flash
void System::set_led_speed(uint32_t speed) {
    led_flash_speed_ = speed;
    led_monitor();
}

// initializes network
void System::network_init(bool refresh) {
    if (refresh) {
        get_settings();
    }

    // check ethernet profile
    // ethernet uses lots of additional memory so we only start it when it's explicitly set in the config
    if (ethernet_profile_ == 0) {
        return;
    }

    uint8_t          phy_addr;   // I²C-address of Ethernet PHY (0 or 1 for LAN8720, 31 for TLK110)
    int              power;      // Pin# of the enable signal for the external crystal oscillator (-1 to disable for internal APLL source)
    int              mdc;        // Pin# of the I²C clock signal for the Ethernet PHY
    int              mdio;       // Pin# of the I²C IO signal for the Ethernet PHY
    eth_phy_type_t   type;       // Type of the Ethernet PHY (LAN8720 or TLK110)
    eth_clock_mode_t clock_mode; // ETH_CLOCK_GPIO0_IN or ETH_CLOCK_GPIO0_OUT, ETH_CLOCK_GPIO16_OUT, ETH_CLOCK_GPIO17_OUT for 50Hz inverted clock

    if (ethernet_profile_ == 1) {
        // LAN8720
        phy_addr   = 0;
        power      = -1;
        mdc        = 23;
        mdio       = 18;
        type       = ETH_PHY_LAN8720;
        clock_mode = ETH_CLOCK_GPIO0_IN;
    } else if (ethernet_profile_ == 2) {
        // TLK110
        phy_addr   = 31;
        power      = -1;
        mdc        = 23;
        mdio       = 18;
        type       = ETH_PHY_TLK110;
        clock_mode = ETH_CLOCK_GPIO0_IN;
    }

#ifndef EMSESP_STANDALONE
    if (ETH.begin(phy_addr, power, mdc, mdio, type, clock_mode)) {
        // disable ssid and AP when using Ethernet
        EMSESP::esp8266React.getNetworkSettingsService()->update(
            [&](NetworkSettings & settings) {
                settings.ssid == ""; // remove SSID
                return StateUpdateResult::CHANGED;
            },
            "local");

        EMSESP::esp8266React.getAPSettingsService()->update(
            [&](APSettings & settings) {
                settings.provisionMode = AP_MODE_NEVER;
                return StateUpdateResult::CHANGED;
            },
            "local");
    }
#endif

    last_system_check_ = 0; // force the LED to go from fast flash to pulse
    send_heartbeat();
}

// check health of system, done every few seconds
void System::system_check() {
    if (!last_system_check_ || ((uint32_t)(uuid::get_uptime() - last_system_check_) >= SYSTEM_CHECK_FREQUENCY)) {
        last_system_check_ = uuid::get_uptime();

#ifndef EMSESP_STANDALONE
        if (WiFi.status() != WL_CONNECTED) {
            set_led_speed(LED_WARNING_BLINK_FAST);
            system_healthy_ = false;
            return;
        }
#endif

        // not healthy if bus not connected
        if (!EMSbus::bus_connected()) {
            if (system_healthy_) {
                LOG_ERROR(F("Error: No connection to the EMS bus"));
            }
            system_healthy_ = false;
            set_led_speed(LED_WARNING_BLINK); // flash every 1/2 second from now on
        } else {
            // if it was unhealthy but now we're better, make sure the LED is solid again cos we've been healed
            if (!system_healthy_) {
                system_healthy_ = true;
                send_heartbeat();
                if (led_gpio_) {
                    digitalWrite(led_gpio_, hide_led_ ? !LED_ON : LED_ON); // LED on, for ever
                }
            }
        }
    }
}

// commands - takes static function pointers
// these commands respond to the topic "system" and take a payload like {cmd:"", data:"", id:""}
void System::commands_init() {
    Command::add(EMSdevice::DeviceType::SYSTEM, F_(pin), System::command_pin);
    Command::add(EMSdevice::DeviceType::SYSTEM, F_(send), System::command_send);
    Command::add(EMSdevice::DeviceType::SYSTEM, F_(publish), System::command_publish);
    Command::add(EMSdevice::DeviceType::SYSTEM, F_(fetch), System::command_fetch);
    Command::add_with_json(EMSdevice::DeviceType::SYSTEM, F_(info), System::command_info);
    Command::add_with_json(EMSdevice::DeviceType::SYSTEM, F_(settings), System::command_settings);
#if defined(EMSESP_TEST)
    Command::add(EMSdevice::DeviceType::SYSTEM, F("test"), System::command_test);
#endif
}

// flashes the LED
void System::led_monitor() {
    if (!led_gpio_) {
        return;
    }

    static uint32_t led_last_blink_ = 0;

    if (!led_last_blink_ || (uint32_t)(uuid::get_uptime() - led_last_blink_) >= led_flash_speed_) {
        led_last_blink_ = uuid::get_uptime();

        // if bus_not_connected or network not connected, start flashing
        if (!system_healthy_) {
            digitalWrite(led_gpio_, !digitalRead(led_gpio_));
        }
    }
}

// Return the quality (Received Signal Strength Indicator) of the WiFi network as a %. Or -1 if disconnected.
//  High quality: 90% ~= -55dBm
//  Medium quality: 50% ~= -75dBm
//  Low quality: 30% ~= -85dBm
//  Unusable quality: 8% ~= -96dBm
int8_t System::wifi_quality() {
#ifdef EMSESP_STANDALONE
    return 100;
#else
    if (WiFi.status() != WL_CONNECTED) {
        return -1;
    }
    int32_t dBm = WiFi.RSSI();
    if (dBm <= -100) {
        return 0;
    }

    if (dBm >= -50) {
        return 100;
    }
    return 2 * (dBm + 100);
#endif
}

// print users to console
void System::show_users(uuid::console::Shell & shell) {
    shell.printfln(F("Users:"));

#ifndef EMSESP_STANDALONE
    EMSESP::esp8266React.getSecuritySettingsService()->read([&](SecuritySettings & securitySettings) {
        for (User user : securitySettings.users) {
            shell.printfln(F(" username: %s, password: %s, is_admin: %s"), user.username.c_str(), user.password.c_str(), user.admin ? F("yes") : F("no"));
        }
    });
#endif

    shell.println();
}

void System::show_system(uuid::console::Shell & shell) {
    shell.printfln(F("Uptime:        %s"), uuid::log::format_timestamp_ms(uuid::get_uptime_ms(), 3).c_str());

#ifndef EMSESP_STANDALONE
    shell.printfln(F("SDK version:   %s"), ESP.getSdkVersion());
    shell.printfln(F("CPU frequency: %u MHz"), ESP.getCpuFreqMHz());
    shell.printfln(F("Free heap:     %lu bytes"), (uint32_t)ESP.getFreeHeap());
    shell.println();

    switch (WiFi.status()) {
    case WL_IDLE_STATUS:
        shell.printfln(F("WiFi: Idle"));
        break;

    case WL_NO_SSID_AVAIL:
        shell.printfln(F("WiFi: Network not found"));
        break;

    case WL_SCAN_COMPLETED:
        shell.printfln(F("WiFi: Network scan complete"));
        break;

    case WL_CONNECTED: {
        shell.printfln(F("WiFi: Connected"));
        shell.printfln(F("SSID: %s"), WiFi.SSID().c_str());
        shell.printfln(F("BSSID: %s"), WiFi.BSSIDstr().c_str());
        shell.printfln(F("RSSI: %d dBm (%d %%)"), WiFi.RSSI(), wifi_quality());
        shell.printfln(F("MAC address: %s"), WiFi.macAddress().c_str());
        shell.printfln(F("Hostname: %s"), WiFi.getHostname());
        shell.printfln(F("IPv4 address: %s/%s"), uuid::printable_to_string(WiFi.localIP()).c_str(), uuid::printable_to_string(WiFi.subnetMask()).c_str());
        shell.printfln(F("IPv4 gateway: %s"), uuid::printable_to_string(WiFi.gatewayIP()).c_str());
        shell.printfln(F("IPv4 nameserver: %s"), uuid::printable_to_string(WiFi.dnsIP()).c_str());
    } break;

    case WL_CONNECT_FAILED:
        shell.printfln(F("WiFi: Connection failed"));
        break;

    case WL_CONNECTION_LOST:
        shell.printfln(F("WiFi: Connection lost"));
        break;

    case WL_DISCONNECTED:
        shell.printfln(F("WiFi: Disconnected"));
        break;

    case WL_NO_SHIELD:
    default:
        shell.printfln(F("WiFi: Unknown"));
        break;
    }

    shell.println();

    // show Ethernet
    if (ethernet_connected_) {
        shell.printfln(F("Ethernet: Connected"));
        shell.printfln(F("MAC address: %s"), ETH.macAddress().c_str());
        shell.printfln(F("Hostname: %s"), ETH.getHostname());
        shell.printfln(F("IPv4 address: %s/%s"), uuid::printable_to_string(ETH.localIP()).c_str(), uuid::printable_to_string(ETH.subnetMask()).c_str());
        shell.printfln(F("IPv4 gateway: %s"), uuid::printable_to_string(ETH.gatewayIP()).c_str());
        shell.printfln(F("IPv4 nameserver: %s"), uuid::printable_to_string(ETH.dnsIP()).c_str());
    } else {
        shell.printfln(F("Ethernet: disconnected"));
    }

    shell.println();
    if (!syslog_enabled_) {
        shell.printfln(F("Syslog: disabled"));
    } else {
        shell.printfln(F("Syslog:"));
        shell.print(F(" "));
        shell.printfln(F_(host_fmt), !syslog_host_.isEmpty() ? syslog_host_.c_str() : uuid::read_flash_string(F_(unset)).c_str());
        shell.print(F(" "));
        shell.printfln(F_(log_level_fmt), uuid::log::format_level_lowercase(static_cast<uuid::log::Level>(syslog_level_)));
        shell.print(F(" "));
        shell.printfln(F_(mark_interval_fmt), syslog_mark_interval_);
    }

#endif
}

// console commands to add
void System::console_commands(Shell & shell, unsigned int context) {
    EMSESPShell::commands->add_command(ShellContext::SYSTEM,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(restart)},
                                       [](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments __attribute__((unused))) { EMSESP::system_.restart(); });

    EMSESPShell::commands->add_command(ShellContext::SYSTEM,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(wifi), F_(reconnect)},
                                       [](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments __attribute__((unused))) { EMSESP::system_.wifi_reconnect(); });

    EMSESPShell::commands->add_command(ShellContext::SYSTEM, CommandFlags::ADMIN, flash_string_vector{F_(format)}, [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
        shell.enter_password(F_(password_prompt), [=](Shell & shell, bool completed, const std::string & password) {
            if (completed) {
                EMSESP::esp8266React.getSecuritySettingsService()->read([&](SecuritySettings & securitySettings) {
                    if (securitySettings.jwtSecret.equals(password.c_str())) {
                        EMSESP::system_.format(shell);
                    } else {
                        shell.println(F("incorrect password"));
                    }
                });
            }
        });
    });

    EMSESPShell::commands->add_command(ShellContext::SYSTEM, CommandFlags::ADMIN, flash_string_vector{F_(passwd)}, [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
        shell.enter_password(F_(new_password_prompt1), [](Shell & shell, bool completed, const std::string & password1) {
            if (completed) {
                shell.enter_password(F_(new_password_prompt2), [password1](Shell & shell, bool completed, const std::string & password2) {
                    if (completed) {
                        if (password1 == password2) {
                            EMSESP::esp8266React.getSecuritySettingsService()->update(
                                [&](SecuritySettings & securitySettings) {
                                    securitySettings.jwtSecret = password2.c_str();
                                    return StateUpdateResult::CHANGED;
                                },
                                "local");
                            shell.println(F("su password updated"));
                        } else {
                            shell.println(F("Passwords do not match"));
                        }
                    }
                });
            }
        });
    });

    EMSESPShell::commands->add_command(ShellContext::SYSTEM, CommandFlags::USER, flash_string_vector{F_(show)}, [=](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
        EMSESP::system_.show_system(shell);
        shell.println();
    });

    EMSESPShell::commands->add_command(ShellContext::SYSTEM,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(set), F_(hostname)},
                                       flash_string_vector{F_(name_mandatory)},
                                       [](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments) {
                                           shell.println("The network connection will be reset...");
                                           Shell::loop_all();
                                           delay(1000); // wait a second
                                           EMSESP::esp8266React.getNetworkSettingsService()->update(
                                               [&](NetworkSettings & networkSettings) {
                                                   networkSettings.hostname = arguments.front().c_str();
                                                   return StateUpdateResult::CHANGED;
                                               },
                                               "local");
                                       });

    EMSESPShell::commands->add_command(ShellContext::SYSTEM,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(set), F_(wifi), F_(ssid)},
                                       flash_string_vector{F_(name_mandatory)},
                                       [](Shell & shell, const std::vector<std::string> & arguments) {
                                           EMSESP::esp8266React.getNetworkSettingsService()->updateWithoutPropagation([&](NetworkSettings & networkSettings) {
                                               networkSettings.ssid = arguments.front().c_str();
                                               return StateUpdateResult::CHANGED;
                                           });
                                           shell.println("Use `wifi reconnect` to save and apply the new settings");
                                       });

    EMSESPShell::commands->add_command(ShellContext::SYSTEM, CommandFlags::ADMIN, flash_string_vector{F_(set), F_(wifi), F_(password)}, [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
        shell.enter_password(F_(new_password_prompt1), [](Shell & shell, bool completed, const std::string & password1) {
            if (completed) {
                shell.enter_password(F_(new_password_prompt2), [password1](Shell & shell, bool completed, const std::string & password2) {
                    if (completed) {
                        if (password1 == password2) {
                            EMSESP::esp8266React.getNetworkSettingsService()->updateWithoutPropagation([&](NetworkSettings & networkSettings) {
                                networkSettings.password = password2.c_str();
                                return StateUpdateResult::CHANGED;
                            });
                            shell.println("Use `wifi reconnect` to save and apply the new settings");
                        } else {
                            shell.println(F("Passwords do not match"));
                        }
                    }
                });
            }
        });
    });

    EMSESPShell::commands->add_command(
        ShellContext::SYSTEM,
        CommandFlags::ADMIN,
        flash_string_vector{F_(set), F_(ethernet)},
        flash_string_vector{F_(n_mandatory)},
        [](Shell & shell, const std::vector<std::string> & arguments) {
            uint8_t n = Helpers::hextoint(arguments.front().c_str());
            if (n <= 2) {
                EMSESP::esp8266React.getNetworkSettingsService()->update(
                    [&](NetworkSettings & networkSettings) {
                        networkSettings.ethernet_profile = n;
                        shell.printfln(F_(ethernet_option_fmt), networkSettings.ethernet_profile);
                        return StateUpdateResult::CHANGED;
                    },
                    "local");
                EMSESP::system_.network_init(true);
            } else {
                shell.println(F("Must be 0, 1 or 2"));
            }
        },
        [](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments __attribute__((unused))) -> const std::vector<std::string> {
            return std::vector<std::string>{read_flash_string(F("0")), read_flash_string(F("1")), read_flash_string(F("2"))};
        });

    EMSESPShell::commands->add_command(ShellContext::SYSTEM, CommandFlags::USER, flash_string_vector{F_(set)}, [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
        EMSESP::esp8266React.getNetworkSettingsService()->read([&](NetworkSettings & networkSettings) {
            shell.print(F(" "));
            shell.printfln(F_(hostname_fmt), networkSettings.hostname.isEmpty() ? uuid::read_flash_string(F_(unset)).c_str() : networkSettings.hostname.c_str());
            shell.print(F(" "));
            shell.printfln(F_(wifi_ssid_fmt), networkSettings.ssid.isEmpty() ? uuid::read_flash_string(F_(unset)).c_str() : networkSettings.ssid.c_str());
            shell.print(F(" "));
            shell.printfln(F_(wifi_password_fmt), networkSettings.ssid.isEmpty() ? F_(unset) : F_(asterisks));
            shell.print(F(" "));
            shell.printfln(F_(ethernet_option_fmt), networkSettings.ethernet_profile);
        });
    });

    EMSESPShell::commands->add_command(ShellContext::SYSTEM, CommandFlags::ADMIN, flash_string_vector{F_(show), F_(users)}, [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
        EMSESP::system_.show_users(shell);
    });

    // enter the context
    Console::enter_custom_context(shell, context);
}

// upgrade from previous versions of EMS-ESP
// returns true if an upgrade was done
bool System::check_upgrade() {
    return false;
}

// export all settings to JSON text
// e.g. http://ems-esp/api?device=system&cmd=settings
// value and id are ignored
bool System::command_settings(const char * value, const int8_t id, JsonObject & json) {
    EMSESP::esp8266React.getNetworkSettingsService()->read([&](NetworkSettings & settings) {
        JsonObject node          = json.createNestedObject("WIFI");
        node["ssid"]             = settings.ssid;
        node["hostname"]         = settings.hostname;
        node["static_ip_config"] = settings.staticIPConfig;
        JsonUtils::writeIP(node, "local_ip", settings.localIP);
        JsonUtils::writeIP(node, "gateway_ip", settings.gatewayIP);
        JsonUtils::writeIP(node, "subnet_mask", settings.subnetMask);
        JsonUtils::writeIP(node, "dns_ip_1", settings.dnsIP1);
        JsonUtils::writeIP(node, "dns_ip_2", settings.dnsIP2);
    });

#ifndef EMSESP_STANDALONE
    EMSESP::esp8266React.getAPSettingsService()->read([&](APSettings & settings) {
        JsonObject node        = json.createNestedObject("AP");
        node["provision_mode"] = settings.provisionMode;
        node["ssid"]           = settings.ssid;
        node["local_ip"]       = settings.localIP.toString();
        node["gateway_ip"]     = settings.gatewayIP.toString();
        node["subnet_mask"]    = settings.subnetMask.toString();
    });
#endif

    EMSESP::esp8266React.getMqttSettingsService()->read([&](MqttSettings & settings) {
        JsonObject node = json.createNestedObject("MQTT");
        node["enabled"] = settings.enabled;
#ifndef EMSESP_STANDALONE
        node["host"]          = settings.host;
        node["port"]          = settings.port;
        node["username"]      = settings.username;
        node["client_id"]     = settings.clientId;
        node["keep_alive"]    = settings.keepAlive;
        node["clean_session"] = settings.cleanSession;
#endif
        node["publish_time_boiler"]     = settings.publish_time_boiler;
        node["publish_time_thermostat"] = settings.publish_time_thermostat;
        node["publish_time_solar"]      = settings.publish_time_solar;
        node["publish_time_mixer"]      = settings.publish_time_mixer;
        node["publish_time_other"]      = settings.publish_time_other;
        node["publish_time_sensor"]     = settings.publish_time_sensor;
        node["dallas_format"]           = settings.dallas_format;
        node["bool_format"]             = settings.bool_format;
        node["ha_climate_format"]       = settings.ha_climate_format;
        node["ha_enabled"]              = settings.ha_enabled;
        node["mqtt_qos"]                = settings.mqtt_qos;
        node["mqtt_retain"]             = settings.mqtt_retain;
    });

#ifndef EMSESP_STANDALONE
    EMSESP::esp8266React.getNTPSettingsService()->read([&](NTPSettings & settings) {
        JsonObject node   = json.createNestedObject("NTP");
        node["enabled"]   = settings.enabled;
        node["server"]    = settings.server;
        node["tz_label"]  = settings.tzLabel;
        node["tz_format"] = settings.tzFormat;
    });

    EMSESP::esp8266React.getOTASettingsService()->read([&](OTASettings & settings) {
        JsonObject node = json.createNestedObject("OTA");
        node["enabled"] = settings.enabled;
        node["port"]    = settings.port;
    });
#endif

    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        JsonObject node              = json.createNestedObject("Settings");
        node["tx_mode"]              = settings.tx_mode;
        node["ems_bus_id"]           = settings.ems_bus_id;
        node["syslog_enabled"]       = settings.syslog_enabled;
        node["syslog_level"]         = settings.syslog_level;
        node["syslog_mark_interval"] = settings.syslog_mark_interval;
        node["syslog_host"]          = settings.syslog_host;
        node["master_thermostat"]    = settings.master_thermostat;
        node["shower_timer"]         = settings.shower_timer;
        node["shower_alert"]         = settings.shower_alert;
        node["rx_gpio"]              = settings.rx_gpio;
        node["tx_gpio"]              = settings.tx_gpio;
        node["dallas_gpio"]          = settings.dallas_gpio;
        node["dallas_parasite"]      = settings.dallas_parasite;
        node["led_gpio"]             = settings.led_gpio;
        node["hide_led"]             = settings.hide_led;
        node["api_enabled"]          = settings.api_enabled;
        node["analog_enabled"]       = settings.analog_enabled;
        node["pbutton_gpio"]         = settings.pbutton_gpio;
    });

    return true;
}

// export status information including some basic settings
// e.g. http://ems-esp/api?device=system&cmd=info
bool System::command_info(const char * value, const int8_t id, JsonObject & json) {
    JsonObject node;

    node = json.createNestedObject("System");

    node["version"] = EMSESP_APP_VERSION;
    node["uptime"]  = uuid::log::format_timestamp_ms(uuid::get_uptime_ms(), 3);

    node = json.createNestedObject("Status");

    switch (EMSESP::bus_status()) {
    case EMSESP::BUS_STATUS_OFFLINE:
        node["bus"] = (F("disconnected"));
        break;
    case EMSESP::BUS_STATUS_TX_ERRORS:
        node["bus"] = (F("connected, instable tx"));
        break;
    case EMSESP::BUS_STATUS_CONNECTED:
    default:
        node["bus"] = (F("connected"));
        break;
    }

    if (EMSESP::bus_status() != EMSESP::BUS_STATUS_OFFLINE) {
        node["bus protocol"]          = EMSbus::is_ht3() ? F("HT3") : F("Buderus");
        node["#telegrams received"]   = EMSESP::rxservice_.telegram_count();
        node["#read requests sent"]   = EMSESP::txservice_.telegram_read_count();
        node["#write requests sent"]  = EMSESP::txservice_.telegram_write_count();
        node["#incomplete telegrams"] = EMSESP::rxservice_.telegram_error_count();
        node["#tx fails"]             = TxService::MAXIMUM_TX_RETRIES, EMSESP::txservice_.telegram_fail_count();
        node["rx line quality"]       = EMSESP::rxservice_.quality();
        node["tx line quality"]       = EMSESP::txservice_.quality();
        node["#MQTT publish fails"]   = Mqtt::publish_fails();
        node["#dallas sensors"]       = EMSESP::sensor_devices().size();
        node["#dallas fails"]         = EMSESP::sensor_fails();
    }

    JsonArray devices2 = json.createNestedArray("Devices");

    for (const auto & device_class : EMSFactory::device_handlers()) {
        for (const auto & emsdevice : EMSESP::emsdevices) {
            if ((emsdevice) && (emsdevice->device_type() == device_class.first)) {
                JsonObject obj = devices2.createNestedObject();
                obj["type"]    = emsdevice->device_type_name();
                obj["name"]    = emsdevice->to_string();
                char result[200];
                obj["handlers"] = emsdevice->show_telegram_handlers(result);
            }
        }
    }

    return true;
}

#if defined(EMSESP_TEST)
// run a test, e.g. http://ems-esp/api?device=system&cmd=test&data=boiler
bool System::command_test(const char * value, const int8_t id) {
    Test::run_test(value, id);
    return true;
}
#endif


} // namespace emsesp
