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

#include "version.h" // firmware version of EMS-ESP

namespace emsesp {

uuid::log::Logger System::logger_{F_(system), uuid::log::Facility::KERN};

#ifndef EMSESP_STANDALONE
uuid::syslog::SyslogService System::syslog_;
#endif

// init statics
uint32_t    System::heap_start_     = 0;
int         System::reset_counter_  = 0;
bool        System::upload_status_  = false;
bool        System::hide_led_       = false;
uint8_t     System::led_gpio_       = 0;
uint16_t    System::analog_         = 0;
bool        System::analog_enabled_ = false;
std::string System::hostname_;

// send on/off to a gpio pin
// value: true = HIGH, false = LOW
bool System::command_pin(const char * value, const int8_t id) {
    bool v = false;
    if (Helpers::value2bool(value, v)) {
        pinMode(id, OUTPUT);
        digitalWrite(id, v);
        LOG_INFO(F("GPIO %d set to %s"), id, v ? "HIGH" : "LOW");
        return true;
    }

    return false;
}

// send raw
bool System::command_send(const char * value, const int8_t id) {
    EMSESP::send_raw_telegram(value); // ignore id
    return true;
}

// restart EMS-ESP
void System::restart() {
    LOG_NOTICE("Restarting system...");
    Shell::loop_all();
    delay(1000); // wait a second
#if defined(ESP8266)
    ESP.reset();
#elif defined(ESP32)
    ESP.restart();
#endif
}

// saves all settings
void System::wifi_reconnect() {
    LOG_NOTICE("The wifi will reconnect...");
    Shell::loop_all();
    delay(1000);                                                                // wait a second
    EMSESP::webSettingsService.save();                                          // local settings
    EMSESP::esp8266React.getWiFiSettingsService()->callUpdateHandlers("local"); // in case we've changed ssid or password
}

// format fs
// format the FS. Wipes everything.
void System::format(uuid::console::Shell & shell) {
    auto msg = F("Formatting file system. This will reset all settings to their defaults");
    shell.logger().warning(msg);
    shell.flush();

    EMSuart::stop();

#if defined(ESP8266)
    LittleFS.format();
#elif defined(ESP32)
    SPIFFS.format();
#endif

    System::restart();
}

// return free heap mem as a percentage
uint8_t System::free_mem() {
#ifndef EMSESP_STANDALONE
    uint32_t free_memory = ESP.getFreeHeap();
#else
    uint32_t free_memory = 1000;
#endif

    return (100 * free_memory / heap_start_);
}

void System::syslog_init() {
    // fetch settings
    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        syslog_level_         = settings.syslog_level;
        syslog_mark_interval_ = settings.syslog_mark_interval;
        syslog_host_          = settings.syslog_host;
    });

#ifndef EMSESP_STANDALONE
    syslog_.start(); // syslog service re-start

    // configure syslog
    IPAddress addr;
    if (!addr.fromString(syslog_host_.c_str())) {
        addr = (uint32_t)0;
    }
    syslog_.log_level((uuid::log::Level)syslog_level_);
    syslog_.mark_interval(syslog_mark_interval_);
    syslog_.destination(addr);
    EMSESP::esp8266React.getWiFiSettingsService()->read([&](WiFiSettings & wifiSettings) { syslog_.hostname(wifiSettings.hostname.c_str()); });
#endif
}

// first call. Sets memory and starts up the UART Serial bridge
void System::start() {
    // set the inital free mem
    if (heap_start_ == 0) {
#ifndef EMSESP_STANDALONE
        heap_start_ = ESP.getFreeHeap();
#else
        heap_start_ = 2000;
#endif
    }

    // print boot message
    EMSESP::esp8266React.getWiFiSettingsService()->read(
        [&](WiFiSettings & wifiSettings) { LOG_INFO(F("System %s booted (EMS-ESP version %s)"), wifiSettings.hostname.c_str(), EMSESP_APP_VERSION); });

    // these commands respond to the topic "system" and take a payload like {cmd:"", data:"", id:""}
    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        Command::add(EMSdevice::DeviceType::SYSTEM, settings.ems_bus_id, F_(pin), System::command_pin);
        Command::add(EMSdevice::DeviceType::SYSTEM, settings.ems_bus_id, F_(send), System::command_send);
        Command::add_with_json(EMSdevice::DeviceType::SYSTEM, F_(info), System::command_info);
        Command::add_with_json(EMSdevice::DeviceType::SYSTEM, F_(report), System::command_report);
    });

    syslog_init(); // init SysLog

    init();
}

// init stuff. This is called when settings are changed in the web
void System::init() {
    set_led(); // init LED

    // set the boolean format used for rendering booleans
    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        Helpers::bool_format(settings.bool_format);
        analog_enabled_ = settings.analog_enabled;
    });

    EMSESP::esp8266React.getWiFiSettingsService()->read([&](WiFiSettings & settings) { hostname(settings.hostname.c_str()); });

    EMSESP::init_tx(); // start UART
}

// set the LED to on or off when in normal operating mode
void System::set_led() {
    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        hide_led_ = settings.hide_led;
        led_gpio_ = settings.led_gpio;
        if (led_gpio_) {
            pinMode(led_gpio_, OUTPUT);                            // 0 means disabled
            digitalWrite(led_gpio_, hide_led_ ? !LED_ON : LED_ON); // LED on, for ever
        }
    });
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
    syslog_.loop();
#endif
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

#if defined(ESP8266)
#if defined(EMSESP_DEBUG)
    static uint32_t last_memcheck_ = 0;
    if (currentMillis - last_memcheck_ > 10000) { // 10 seconds
        last_memcheck_ = currentMillis;
        show_mem("core");
    }
#endif
#endif
}

void System::show_mem(const char * note) {
#if defined(ESP8266)
#if defined(EMSESP_DEBUG)
    LOG_INFO(F("(%s) Free heap: %d%% (%lu), frag:%u%%"), note, free_mem(), (unsigned long)ESP.getFreeHeap(), ESP.getHeapFragmentation());
#endif
#endif
}

// send periodic MQTT message with system information
void System::send_heartbeat() {
    // don't send heartbeat if WiFi is not connected
    int rssi = wifi_quality();
    if (rssi == -1) {
        return;
    }

    uint32_t free_memory = free_mem();

#if defined(ESP8266)
    uint8_t frag_memory = ESP.getHeapFragmentation();
#endif

    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_SMALL> doc;

    uint8_t ems_status = EMSESP::bus_status();
    if (ems_status == EMSESP::BUS_STATUS_TX_ERRORS) {
        doc["status"] = "txerror";
    } else if (ems_status == EMSESP::BUS_STATUS_CONNECTED) {
        doc["status"] = "connected";
    } else {
        doc["status"] = "disconnected";
    }

    doc["rssi"]       = rssi;
    doc["uptime"]     = uuid::log::format_timestamp_ms(uuid::get_uptime_ms(), 3);
    doc["uptime_sec"] = uuid::get_uptime_sec();
    doc["mqttfails"]  = Mqtt::publish_fails();
    doc["txfails"]    = EMSESP::txservice_.telegram_fail_count();
    doc["rxfails"]    = EMSESP::rxservice_.telegram_error_count();
    doc["freemem"]    = free_memory;
#if defined(ESP8266)
    doc["fragmem"] = frag_memory;
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
#if defined(ESP8266)
        uint16_t a = analogRead(A0);
#elif defined(ESP32)
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

// check health of system, done every few seconds
void System::system_check() {
    static uint32_t last_system_check_ = 0;

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
            system_healthy_ = false;
            set_led_speed(LED_WARNING_BLINK); // flash every 1/2 second from now on
            // LOG_ERROR(F("Error: No connection to the EMS bus"));
        } else {
            // if it was unhealthy but now we're better, make sure the LED is solid again cos we've been healed
            if (!system_healthy_) {
                system_healthy_ = true;
                if (led_gpio_) {
                    digitalWrite(led_gpio_, hide_led_ ? !LED_ON : LED_ON); // LED on, for ever
                }
            }
        }
    }
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
#ifndef EMSESP_STANDALONE
    if (WiFi.status() != WL_CONNECTED) {
        return -1;
    }
    int dBm = WiFi.RSSI();
#else
    int8_t dBm = -70;
#endif
    if (dBm <= -100) {
        return 0;
    }
    if (dBm >= -50) {
        return 100;
    }
    return 2 * (dBm + 100);
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
#if defined(ESP8266)
    shell.printfln(F("Chip ID:       0x%08x"), ESP.getChipId());
    shell.printfln(F("SDK version:   %s"), ESP.getSdkVersion());
    shell.printfln(F("Core version:  %s"), ESP.getCoreVersion().c_str());
    shell.printfln(F("Full version:  %s"), ESP.getFullVersion().c_str());
    shell.printfln(F("Boot version:  %u"), ESP.getBootVersion());
    shell.printfln(F("Boot mode:     %u"), ESP.getBootMode());
    shell.printfln(F("CPU frequency: %u MHz"), ESP.getCpuFreqMHz());
    shell.printfln(F("Flash chip:    0x%08X (%u bytes)"), ESP.getFlashChipId(), ESP.getFlashChipRealSize());
    shell.printfln(F("Reset reason:  %s"), ESP.getResetReason().c_str());
    shell.printfln(F("Reset info:    %s"), ESP.getResetInfo().c_str());
#elif defined(ESP32)
    shell.printfln(F("SDK version:   %s"), ESP.getSdkVersion());
    shell.printfln(F("CPU frequency: %u MHz"), ESP.getCpuFreqMHz());
#endif
    shell.printfln(F("Sketch size:   %u bytes (%u bytes free)"), ESP.getSketchSize(), ESP.getFreeSketchSpace());
    shell.printfln(F("Free heap:                %lu bytes"), (unsigned long)ESP.getFreeHeap());
    shell.printfln(F("Free mem:                 %d  %%"), free_mem());
#if defined(ESP8266)
    shell.printfln(F("Heap fragmentation:       %u %%"), ESP.getHeapFragmentation());
    shell.printfln(F("Maximum free block size:  %lu bytes"), (unsigned long)ESP.getMaxFreeBlockSize());
    shell.printfln(F("Free continuations stack: %lu bytes"), (unsigned long)ESP.getFreeContStack());
#endif
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

#if defined(ESP8266)
        shell.printfln(F("Hostname: %s"), WiFi.hostname().c_str());
#elif defined(ESP32)
        shell.printfln(F("Hostname: %s"), WiFi.getHostname());
#endif
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

    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        shell.println();
        shell.printfln(F("Syslog:"));
        shell.print(F_(1space));
        shell.printfln(F_(host_fmt), !settings.syslog_host.isEmpty() ? settings.syslog_host.c_str() : uuid::read_flash_string(F_(unset)).c_str());
        shell.print(F_(1space));
        shell.printfln(F_(log_level_fmt), uuid::log::format_level_lowercase(static_cast<uuid::log::Level>(settings.syslog_level)));
        shell.print(F_(1space));
        shell.printfln(F_(mark_interval_fmt), settings.syslog_mark_interval);
    });

#endif
}

// console commands to add
void System::console_commands(Shell & shell, unsigned int context) {
    EMSESPShell::commands->add_command(ShellContext::SYSTEM,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(restart)},
                                       [](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments __attribute__((unused))) {
                                           restart();
                                       });

    EMSESPShell::commands->add_command(ShellContext::SYSTEM,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(wifi), F_(reconnect)},
                                       [](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments __attribute__((unused))) {
                                           wifi_reconnect();
                                       });

    EMSESPShell::commands->add_command(ShellContext::SYSTEM,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(format)},
                                       [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                                           shell.enter_password(F_(password_prompt), [=](Shell & shell, bool completed, const std::string & password) {
                                               if (completed) {
                                                   EMSESP::esp8266React.getSecuritySettingsService()->read([&](SecuritySettings & securitySettings) {
                                                       if (securitySettings.jwtSecret.equals(password.c_str())) {
                                                           format(shell);
                                                       } else {
                                                           shell.println(F("incorrect password"));
                                                       }
                                                   });
                                               }
                                           });
                                       });

    EMSESPShell::commands->add_command(ShellContext::SYSTEM,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(passwd)},
                                       [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                                           shell.enter_password(F_(new_password_prompt1), [](Shell & shell, bool completed, const std::string & password1) {
                                               if (completed) {
                                                   shell.enter_password(F_(new_password_prompt2),
                                                                        [password1](Shell & shell, bool completed, const std::string & password2) {
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

    EMSESPShell::commands->add_command(ShellContext::SYSTEM,
                                       CommandFlags::USER,
                                       flash_string_vector{F_(show)},
                                       [=](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                                           show_system(shell);
                                           shell.println();
                                       });

    EMSESPShell::commands->add_command(ShellContext::SYSTEM,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(set), F_(wifi), F_(hostname)},
                                       flash_string_vector{F_(name_mandatory)},
                                       [](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments) {
                                           shell.println("The wifi connection will be reset...");
                                           Shell::loop_all();
                                           delay(1000); // wait a second
                                           EMSESP::esp8266React.getWiFiSettingsService()->update(
                                               [&](WiFiSettings & wifiSettings) {
                                                   wifiSettings.hostname = arguments.front().c_str();
                                                   return StateUpdateResult::CHANGED;
                                               },
                                               "local");
                                       });

    EMSESPShell::commands->add_command(ShellContext::SYSTEM,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(set), F_(wifi), F_(ssid)},
                                       flash_string_vector{F_(name_mandatory)},
                                       [](Shell & shell, const std::vector<std::string> & arguments) {
                                           EMSESP::esp8266React.getWiFiSettingsService()->updateWithoutPropagation([&](WiFiSettings & wifiSettings) {
                                               wifiSettings.ssid = arguments.front().c_str();
                                               return StateUpdateResult::CHANGED;
                                           });
                                           shell.println("Use `wifi reconnect` to apply the new settings");
                                       });

    EMSESPShell::commands->add_command(ShellContext::SYSTEM,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(set), F_(wifi), F_(password)},
                                       [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                                           shell.enter_password(F_(new_password_prompt1), [](Shell & shell, bool completed, const std::string & password1) {
                                               if (completed) {
                                                   shell.enter_password(F_(new_password_prompt2),
                                                                        [password1](Shell & shell, bool completed, const std::string & password2) {
                                                                            if (completed) {
                                                                                if (password1 == password2) {
                                                                                    EMSESP::esp8266React.getWiFiSettingsService()->updateWithoutPropagation(
                                                                                        [&](WiFiSettings & wifiSettings) {
                                                                                            wifiSettings.password = password2.c_str();
                                                                                            return StateUpdateResult::CHANGED;
                                                                                        });
                                                                                    shell.println("Use `wifi reconnect` to apply the new settings");
                                                                                } else {
                                                                                    shell.println(F("Passwords do not match"));
                                                                                }
                                                                            }
                                                                        });
                                               }
                                           });
                                       });

    EMSESPShell::commands->add_command(ShellContext::SYSTEM,
                                       CommandFlags::USER,
                                       flash_string_vector{F_(set)},
                                       [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                                           EMSESP::esp8266React.getWiFiSettingsService()->read([&](WiFiSettings & wifiSettings) {
                                               shell.print(F_(1space));
                                               shell.printfln(F_(hostname_fmt),
                                                              wifiSettings.hostname.isEmpty() ? uuid::read_flash_string(F_(unset)).c_str()
                                                                                              : wifiSettings.hostname.c_str());
                                           });

                                           EMSESP::esp8266React.getWiFiSettingsService()->read([&](WiFiSettings & wifiSettings) {
                                               shell.print(F_(1space));
                                               shell.printfln(F_(wifi_ssid_fmt),
                                                              wifiSettings.ssid.isEmpty() ? uuid::read_flash_string(F_(unset)).c_str()
                                                                                          : wifiSettings.ssid.c_str());
                                               shell.print(F_(1space));
                                               shell.printfln(F_(wifi_password_fmt), wifiSettings.ssid.isEmpty() ? F_(unset) : F_(asterisks));
                                           });
                                       });

    EMSESPShell::commands->add_command(ShellContext::SYSTEM,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(show), F_(users)},
                                       [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) { System::show_users(shell); });

    // enter the context
    Console::enter_custom_context(shell, context);
}

// upgrade from previous versions of EMS-ESP, based on SPIFFS on an ESP8266
// returns true if an upgrade was done
// the logic is bit abnormal (loading both filesystems and testing) but this was the only way I could get it to work reliably
bool System::check_upgrade() {
#if defined(ESP8266)
    LittleFSConfig l_cfg;
    l_cfg.setAutoFormat(false);
    LittleFS.setConfig(l_cfg); // do not auto format if it can't find LittleFS
    if (LittleFS.begin()) {
#if defined(EMSESP_DEBUG)
        Serial.begin(115200);
        Serial.println(F("FS is Littlefs"));
        Serial.end();
#endif
        return false;
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

    SPIFFSConfig cfg;
    cfg.setAutoFormat(false); // prevent formatting when opening SPIFFS filesystem
    SPIFFS.setConfig(cfg);
    if (!SPIFFS.begin()) {
#if defined(EMSESP_DEBUG)
        Serial.begin(115200);
        Serial.println(F("No old SPIFFS found!"));
        Serial.end();
#endif
        // if there is neither SPIFFS or LittleFS we can assume the ESP8266 has been erased
        l_cfg.setAutoFormat(true); // reset to normal behaviour
        LittleFS.setConfig(l_cfg);
        return false;
    }

    Serial.begin(115200);

    bool                     failed = false;
    File                     file;
    JsonObject               network, general, mqtt, custom_settings;
    StaticJsonDocument<1024> doc;

    // open the system settings:
    // {
    // "command":"configfile",
    // "network":{"ssid":"xxxx","password":"yyyy","wmode":1,"staticip":null,"gatewayip":null,"nmask":null,"dnsip":null},
    // "general":{"password":"admin","serial":false,"hostname":"ems-esp","log_events":false,"log_ip":null,"version":"1.9.5"},
    // "mqtt":{"enabled":false,"heartbeat":false,"ip":null,"user":null,"port":1883,"qos":0,"keepalive":60,"retain":false,"password":null,"base":null,"nestedjson":false},
    // "ntp":{"server":"pool.ntp.org","interval":720,"enabled":false,"timezone":2}
    // }
    file = SPIFFS.open("/myesp.json", "r");
    if (!file) {
        Serial.println(F("Unable to read the system config file"));
        failed = true;
    } else {
        DeserializationError error = deserializeJson(doc, file);
        if (error) {
            Serial.printf(PSTR("Error. Failed to deserialize system json, error %s\n"), error.c_str());
            failed = true;
        } else {
            Serial.println(F("Migrating settings from EMS-ESP v1.9..."));
#if defined(EMSESP_DEBUG)
            serializeJson(doc, Serial);
            Serial.println();
#endif
            network = doc["network"];
            general = doc["general"];
            mqtt    = doc["mqtt"];

            // start up LittleFS. If it doesn't exist it will format it
            l_cfg.setAutoFormat(true);
            LittleFS.setConfig(l_cfg);
            LittleFS.begin();
            EMSESP::esp8266React.begin();
            EMSESP::webSettingsService.begin();

            EMSESP::esp8266React.getWiFiSettingsService()->update(
                [&](WiFiSettings & wifiSettings) {
                    wifiSettings.hostname = general["hostname"] | FACTORY_WIFI_HOSTNAME;
                    wifiSettings.ssid     = network["ssid"] | FACTORY_WIFI_SSID;
                    wifiSettings.password = network["password"] | FACTORY_WIFI_PASSWORD;

                    wifiSettings.staticIPConfig = false;
                    JsonUtils::readIP(network, "staticip", wifiSettings.localIP);
                    JsonUtils::readIP(network, "dnsip", wifiSettings.dnsIP1);
                    JsonUtils::readIP(network, "gatewayip", wifiSettings.gatewayIP);
                    JsonUtils::readIP(network, "nmask", wifiSettings.subnetMask);

                    return StateUpdateResult::CHANGED;
                },
                "local");

            EMSESP::esp8266React.getSecuritySettingsService()->update(
                [&](SecuritySettings & securitySettings) {
                    securitySettings.jwtSecret = general["password"] | FACTORY_JWT_SECRET;

                    return StateUpdateResult::CHANGED;
                },
                "local");

            EMSESP::esp8266React.getMqttSettingsService()->update(
                [&](MqttSettings & mqttSettings) {
                    mqttSettings.host           = mqtt["ip"] | FACTORY_MQTT_HOST;
                    mqttSettings.mqtt_format    = (mqtt["nestedjson"] ? Mqtt::Format::NESTED : Mqtt::Format::SINGLE);
                    mqttSettings.mqtt_qos       = mqtt["qos"] | 0;
                    mqttSettings.mqtt_retain    = mqtt["retain"];
                    mqttSettings.username       = mqtt["user"] | "";
                    mqttSettings.password       = mqtt["password"] | "";
                    mqttSettings.port           = mqtt["port"] | FACTORY_MQTT_PORT;
                    mqttSettings.clientId       = FACTORY_MQTT_CLIENT_ID;
                    mqttSettings.enabled        = mqtt["enabled"];
                    mqttSettings.keepAlive      = FACTORY_MQTT_KEEP_ALIVE;
                    mqttSettings.cleanSession   = FACTORY_MQTT_CLEAN_SESSION;
                    mqttSettings.maxTopicLength = FACTORY_MQTT_MAX_TOPIC_LENGTH;

                    return StateUpdateResult::CHANGED;
                },
                "local");
        }
    }
    file.close();

    if (failed) {
#if defined(EMSESP_DEBUG)
        Serial.println(F("Failed to read system config. Quitting."));
#endif
        SPIFFS.end();
        Serial.end();
        return false;
    }

    // open the custom settings file next:
    // {
    // "command":"custom_configfile",
    // "settings":{"led":true,"led_gpio":2,"dallas_gpio":14,"dallas_parasite":false,"listen_mode":false,"shower_timer":false,"shower_alert":false,"publish_time":0,"tx_mode":1,"bus_id":11,"master_thermostat":0,"known_devices":""}
    // }
    doc.clear();
    failed = false;
    file   = SPIFFS.open("/customconfig.json", "r");
    if (!file) {
        Serial.println(F("Unable to read custom config file"));
        failed = true;
    } else {
        DeserializationError error = deserializeJson(doc, file);
        if (error) {
            Serial.printf(PSTR("Error. Failed to deserialize custom json, error %s\n"), error.c_str());
            failed = true;
        } else {
#if defined(EMSESP_DEBUG)
            serializeJson(doc, Serial);
            Serial.println();
#endif
            custom_settings = doc["settings"];
            EMSESP::webSettingsService.update(
                [&](WebSettings & settings) {
                    settings.tx_mode              = custom_settings["tx_mode"] | EMSESP_DEFAULT_TX_MODE;
                    settings.shower_alert         = custom_settings["shower_alert"] | EMSESP_DEFAULT_SHOWER_ALERT;
                    settings.shower_timer         = custom_settings["shower_timer"] | EMSESP_DEFAULT_SHOWER_TIMER;
                    settings.master_thermostat    = custom_settings["master_thermostat"] | EMSESP_DEFAULT_MASTER_THERMOSTAT;
                    settings.ems_bus_id           = custom_settings["bus_id"] | EMSESP_DEFAULT_EMS_BUS_ID;
                    settings.syslog_host          = EMSESP_DEFAULT_SYSLOG_HOST;
                    settings.syslog_level         = EMSESP_DEFAULT_SYSLOG_LEVEL;
                    settings.syslog_mark_interval = EMSESP_DEFAULT_SYSLOG_MARK_INTERVAL;
                    settings.dallas_gpio          = custom_settings["dallas_gpio"] | EMSESP_DEFAULT_DALLAS_GPIO;
                    settings.dallas_parasite      = custom_settings["dallas_parasite"] | EMSESP_DEFAULT_DALLAS_PARASITE;
                    settings.led_gpio             = custom_settings["led_gpio"] | EMSESP_DEFAULT_LED_GPIO;
                    settings.analog_enabled       = EMSESP_DEFAULT_ANALOG_ENABLED;

                    return StateUpdateResult::CHANGED;
                },
                "local");
        }
    }
    file.close();

    SPIFFS.end();

    if (failed) {
#if defined(EMSESP_DEBUG)
        Serial.println(F("Failed to read custom config. Quitting."));
#endif
        Serial.end();
        return false;
    }

#pragma GCC diagnostic pop

    Serial.println(F("Restarting..."));
    Serial.flush();
    delay(1000);
    Serial.end();
    delay(1000);
    restart();
    return true;
#else
    return false;
#endif
}

// export all settings to JSON text
// http://ems-esp/api?device=system&cmd=info
// value and id are ignored
bool System::command_info(const char * value, const int8_t id, JsonObject & json) {
#ifdef EMSESP_STANDALONE
    json["test"] = "testing system info command";
#else
    EMSESP::esp8266React.getWiFiSettingsService()->read([&](WiFiSettings & settings) {
        char       s[7];
        JsonObject node = json.createNestedObject("WIFI");
        node["ssid"]    = settings.ssid;
        // node["password"]         = settings.password;
        node["hostname"]         = settings.hostname;
        node["static_ip_config"] = Helpers::render_boolean(s, settings.staticIPConfig);
        JsonUtils::writeIP(node, "local_ip", settings.localIP);
        JsonUtils::writeIP(node, "gateway_ip", settings.gatewayIP);
        JsonUtils::writeIP(node, "subnet_mask", settings.subnetMask);
        JsonUtils::writeIP(node, "dns_ip_1", settings.dnsIP1);
        JsonUtils::writeIP(node, "dns_ip_2", settings.dnsIP2);
    });

    EMSESP::esp8266React.getAPSettingsService()->read([&](APSettings & settings) {
        JsonObject node        = json.createNestedObject("AP");
        node["provision_mode"] = settings.provisionMode;
        node["ssid"]           = settings.ssid;
        // node["password"]       = settings.password;
        node["local_ip"]    = settings.localIP.toString();
        node["gateway_ip"]  = settings.gatewayIP.toString();
        node["subnet_mask"] = settings.subnetMask.toString();
    });

    EMSESP::esp8266React.getMqttSettingsService()->read([&](MqttSettings & settings) {
        char       s[7];
        JsonObject node  = json.createNestedObject("MQTT");
        node["enabled"]  = Helpers::render_boolean(s, settings.enabled);
        node["host"]     = settings.host;
        node["port"]     = settings.port;
        node["username"] = settings.username;
        // node["password"]                = settings.password;
        node["client_id"]               = settings.clientId;
        node["keep_alive"]              = settings.keepAlive;
        node["clean_session"]           = Helpers::render_boolean(s, settings.cleanSession);
        node["max_topic_length"]        = settings.maxTopicLength;
        node["publish_time_boiler"]     = settings.publish_time_boiler;
        node["publish_time_thermostat"] = settings.publish_time_thermostat;
        node["publish_time_solar"]      = settings.publish_time_solar;
        node["publish_time_mixer"]      = settings.publish_time_mixer;
        node["publish_time_other"]      = settings.publish_time_other;
        node["publish_time_sensor"]     = settings.publish_time_sensor;
        node["mqtt_format"]             = settings.mqtt_format;
        node["mqtt_qos"]                = settings.mqtt_qos;
        node["mqtt_retain"]             = Helpers::render_boolean(s, settings.mqtt_retain);
    });

    EMSESP::esp8266React.getNTPSettingsService()->read([&](NTPSettings & settings) {
        char       s[7];
        JsonObject node   = json.createNestedObject("NTP");
        node["enabled"]   = Helpers::render_boolean(s, settings.enabled);
        node["server"]    = settings.server;
        node["tz_label"]  = settings.tzLabel;
        node["tz_format"] = settings.tzFormat;
    });

    EMSESP::esp8266React.getOTASettingsService()->read([&](OTASettings & settings) {
        char       s[7];
        JsonObject node = json.createNestedObject("OTA");
        node["enabled"] = Helpers::render_boolean(s, settings.enabled);
        node["port"]    = settings.port;
        // node["password"] = settings.password;
    });

    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        char       s[7];
        JsonObject node              = json.createNestedObject("Settings");
        node["tx_mode"]              = settings.tx_mode;
        node["ems_bus_id"]           = settings.ems_bus_id;
        node["syslog_level"]         = settings.syslog_level;
        node["syslog_mark_interval"] = settings.syslog_mark_interval;
        node["syslog_host"]          = settings.syslog_host;
        node["master_thermostat"]    = settings.master_thermostat;
        node["shower_timer"]         = Helpers::render_boolean(s, settings.shower_timer);
        node["shower_alert"]         = Helpers::render_boolean(s, settings.shower_alert);
        node["rx_gpio"]              = settings.rx_gpio;
        node["tx_gpio"]              = settings.tx_gpio;
        node["dallas_gpio"]          = settings.dallas_gpio;
        node["dallas_parasite"]      = Helpers::render_boolean(s, settings.dallas_parasite);
        node["led_gpio"]             = settings.led_gpio;
        node["hide_led"]             = Helpers::render_boolean(s, settings.hide_led);
        node["api_enabled"]          = Helpers::render_boolean(s, settings.api_enabled);
        node["bool_format"]          = settings.bool_format;
        node["analog_enabled"]       = Helpers::render_boolean(s, settings.analog_enabled);
    });

#endif
    return true;
}

// export debug information
// http://ems-esp/api?device=system&cmd=report
bool System::command_report(const char * value, const int8_t id, JsonObject & json) {
    JsonObject node;

    node = json.createNestedObject("System");

    node["version"] = EMSESP_APP_VERSION;
    node["uptime"]  = uuid::log::format_timestamp_ms(uuid::get_uptime_ms(), 3);
    node["freemem"] = free_mem();
#if defined(ESP8266)
    node["fragmem"] = ESP.getHeapFragmentation();
#endif

    node = json.createNestedObject("Settings");

    EMSESP::esp8266React.getMqttSettingsService()->read([&](MqttSettings & settings) {
        char s[7];
        node["enabled"]                 = Helpers::render_boolean(s, settings.enabled);
        node["publish_time_boiler"]     = settings.publish_time_boiler;
        node["publish_time_thermostat"] = settings.publish_time_thermostat;
        node["publish_time_solar"]      = settings.publish_time_solar;
        node["publish_time_mixer"]      = settings.publish_time_mixer;
        node["publish_time_other"]      = settings.publish_time_other;
        node["publish_time_sensor"]     = settings.publish_time_sensor;
        node["mqtt_format"]             = settings.mqtt_format;
        node["mqtt_qos"]                = settings.mqtt_qos;
        node["mqtt_retain"]             = Helpers::render_boolean(s, settings.mqtt_retain);
    });

    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        char s[7];
        node["tx_mode"]           = settings.tx_mode;
        node["ems_bus_id"]        = settings.ems_bus_id;
        node["master_thermostat"] = settings.master_thermostat;
        node["rx_gpio"]           = settings.rx_gpio;
        node["tx_gpio"]           = settings.tx_gpio;
        node["dallas_gpio"]       = settings.dallas_gpio;
        node["dallas_parasite"]   = Helpers::render_boolean(s, settings.dallas_parasite);
        node["led_gpio"]          = settings.led_gpio;
        node["hide_led"]          = Helpers::render_boolean(s, settings.hide_led);
        node["api_enabled"]       = Helpers::render_boolean(s, settings.api_enabled);
        node["bool_format"]       = settings.bool_format;
        node["analog_enabled"]    = Helpers::render_boolean(s, settings.analog_enabled);
    });

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

} // namespace emsesp
