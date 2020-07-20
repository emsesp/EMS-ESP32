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

#include "system.h"
#include "emsesp.h" // for send_raw_telegram() command

#include "version.h" // firmware version of EMS-ESP

MAKE_PSTR_WORD(passwd)
MAKE_PSTR_WORD(hostname)
MAKE_PSTR_WORD(wifi)
MAKE_PSTR_WORD(ssid)
MAKE_PSTR_WORD(heartbeat)

MAKE_PSTR(host_fmt, "Host = %s")
MAKE_PSTR(hostname_fmt, "Hostname = %s")
MAKE_PSTR(mark_interval_fmt, "Mark interval = %lus");
MAKE_PSTR(wifi_ssid_fmt, "WiFi SSID = %s");
MAKE_PSTR(wifi_password_fmt, "WiFi Password = %S")
MAKE_PSTR(system_heartbeat_fmt, "Heartbeat = %s")

MAKE_PSTR(logger_name, "system")

namespace emsesp {

uuid::log::Logger System::logger_{F_(logger_name), uuid::log::Facility::KERN};

#ifndef EMSESP_STANDALONE
uuid::syslog::SyslogService System::syslog_;
#endif

uint32_t System::heap_start_ = 0;
int      System::reset_counter_;

// handle generic system related MQTT commands
void System::mqtt_commands(const char * message) {
    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_SMALL> doc;
    DeserializationError                           error = deserializeJson(doc, message);
    if (error) {
        LOG_DEBUG(F("MQTT error: payload %s, error %s"), message, error.c_str());
        return;
    }

    if (doc["send"] != nullptr) {
        const char * data = doc["send"];
        EMSESP::send_raw_telegram(data);
        LOG_INFO(F("Sending raw: %s"), data);
    }

#if defined(ESP8266)
    const uint8_t d0_ = 16;
    const uint8_t d1_ = 5;
    const uint8_t d2_ = 4;
    const uint8_t d3_ = 0;
#elif defined(ESP32)
    const uint8_t d0_ = 26;
    const uint8_t d1_ = 22;
    const uint8_t d2_ = 21;
    const uint8_t d3_ = 17;
#endif

#ifndef EMSESP_STANDALONE
    if (doc["D0"] != nullptr) {
        const int8_t set = doc["D0"];
        pinMode(d0_, OUTPUT);
        if (set == 1) {
            digitalWrite(d0_, HIGH);
        } else if (set == 0) {
            digitalWrite(d0_, LOW);
        }
        LOG_INFO(F("Port D0 set to %d"), set);
    }

    if (doc["D1"] != nullptr) {
        const int8_t set = doc["D1"];
        pinMode(d1_, OUTPUT);
        if (set == 1) {
            digitalWrite(d1_, HIGH);
        } else if (set == 0) {
            digitalWrite(d1_, LOW);
        }
        LOG_INFO(F("Port D1 set to %d"), set);
    }

    if (doc["D2"] != nullptr) {
        const int8_t set = doc["D2"];
        pinMode(d2_, OUTPUT);
        if (set == 1) {
            digitalWrite(d2_, HIGH);
        } else if (set == 0) {
            digitalWrite(d2_, LOW);
        }
        LOG_INFO(F("Port D2 set to %d"), set);
    }

    if (doc["D3"] != nullptr) {
        const int8_t set = doc["D3"];
        pinMode(d3_, OUTPUT);
        if (set == 1) {
            digitalWrite(d3_, HIGH);
        } else if (set == 0) {
            digitalWrite(d3_, LOW);
        }
        LOG_INFO(F("Port D3 set to %d"), set);
    }
#endif

    const char * command = doc["cmd"];
    if (command == nullptr) {
        return;
    }

    // send raw command
    if (strcmp(command, "send") == 0) {
        const char * data = doc["data"];
        if (data == nullptr) {
            return;
        }
        EMSESP::send_raw_telegram(data);
        LOG_INFO(F("Sending raw: %s"), data);
        return;
    }
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

// format fs
// format the FS. Wipes everything.
void System::format(uuid::console::Shell & shell) {
    auto msg = F("Formatting file system. This will also reset all settings to their defaults");
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
    EMSESP::emsespSettingsService.read([&](EMSESPSettings & settings) {
        syslog_level_         = settings.syslog_level;
        syslog_mark_interval_ = settings.syslog_mark_interval;
        syslog_host_          = settings.syslog_host;
    });

#ifndef EMSESP_STANDALONE
    syslog_.start(); // syslog service

    // configure syslog
    IPAddress addr;

    if (!addr.fromString(syslog_host_.c_str())) {
        addr = (uint32_t)0;
    }

    EMSESP::esp8266React.getWiFiSettingsService()->read([&](WiFiSettings & wifiSettings) { syslog_.hostname(wifiSettings.hostname.c_str()); });
    syslog_.log_level((uuid::log::Level)syslog_level_);
    syslog_.mark_interval(syslog_mark_interval_);
    syslog_.destination(addr);
#endif
}

void System::set_heartbeat(bool system_heartbeat) {
    system_heartbeat_ = system_heartbeat;
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

    // fetch settings
    EMSESP::emsespSettingsService.read([&](EMSESPSettings & settings) { tx_mode_ = settings.tx_mode; });
    EMSESP::esp8266React.getMqttSettingsService()->read([&](MqttSettings & settings) { system_heartbeat_ = settings.system_heartbeat; });

    syslog_init(); // init SysLog

#if defined(ESP32)
    LOG_INFO(F("System booted (EMS-ESP version %s ESP32)"), EMSESP_APP_VERSION);
#else
    LOG_INFO(F("System booted (EMS-ESP version %s)"), EMSESP_APP_VERSION);
#endif

    if (LED_GPIO) {
        pinMode(LED_GPIO, OUTPUT); // LED pin, 0 means disabled
    }

    // register MQTT system commands
    Mqtt::subscribe("cmd", std::bind(&System::mqtt_commands, this, _1));

#ifndef EMSESP_FORCE_SERIAL
    if (tx_mode_) {
        EMSuart::start(tx_mode_); // start UART, if tx_mode is not 0
    }
#endif
}

// checks system health and handles LED flashing wizardry
void System::loop() {
#ifndef EMSESP_STANDALONE
    syslog_.loop();
#endif
    led_monitor();  // check status and report back using the LED
    system_check(); // check system health

    // send out heartbeat
    uint32_t currentMillis = uuid::get_uptime();
    if (!last_heartbeat_ || (currentMillis - last_heartbeat_ > SYSTEM_HEARTBEAT_INTERVAL)) {
        last_heartbeat_ = currentMillis;
        if (system_heartbeat_) {
            send_heartbeat();
        }
    }
}

// send periodic MQTT message with system information
void System::send_heartbeat() {
    // don't send heartbeat if WiFi is not connected
    int rssid = wifi_quality();
    if (rssid == -1) {
        return;
    }

    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_SMALL> doc;
    doc["rssid"]            = rssid;
    doc["uptime"]           = uuid::log::format_timestamp_ms(uuid::get_uptime_ms(), 3);
    doc["uptime_sec"]       = uuid::get_uptime_sec();
    doc["freemem"]          = free_mem();
    doc["mqttpublishfails"] = Mqtt::publish_fails();
    doc["txfails"]          = EMSESP::txservice_.telegram_fail_count();

    Mqtt::publish("heartbeat", doc, false); // send to MQTT with retain off
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
                if (LED_GPIO) {
                    digitalWrite(LED_GPIO, LED_ON); // LED on, for ever
                }
            }
        }
    }
}

// flashes the LED
void System::led_monitor() {
    if (!LED_GPIO) {
        return;
    }

    static uint32_t led_last_blink_ = 0;

    if (!led_last_blink_ || (uint32_t)(uuid::get_uptime() - led_last_blink_) >= led_flash_speed_) {
        led_last_blink_ = uuid::get_uptime();

        // if bus_not_connected or network not connected, start flashing
        if (!system_healthy_) {
            digitalWrite(LED_GPIO, !digitalRead(LED_GPIO));
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

void System::show_system(uuid::console::Shell & shell) {
    shell.print(F("Uptime: "));
    shell.print(uuid::log::format_timestamp_ms(uuid::get_uptime_ms(), 3));
    shell.println();

#if defined(ESP8266)
    shell.printfln(F("Chip ID:       0x%08x"), ESP.getChipId());
    shell.printfln(F("SDK version:   %s"), ESP.getSdkVersion());
    shell.printfln(F("Core version:  %s"), ESP.getCoreVersion().c_str());
    shell.printfln(F("Full version:  %s"), ESP.getFullVersion().c_str());
    shell.printfln(F("Boot version:  %u"), ESP.getBootVersion());
    shell.printfln(F("Boot mode:     %u"), ESP.getBootMode());
    shell.printfln(F("CPU frequency: %u MHz"), ESP.getCpuFreqMHz());
    shell.printfln(F("Flash chip:    0x%08X (%u bytes)"), ESP.getFlashChipId(), ESP.getFlashChipRealSize());
    shell.printfln(F("Sketch size:   %u bytes (%u bytes free)"), ESP.getSketchSize(), ESP.getFreeSketchSpace());
    shell.printfln(F("Reset reason:  %s"), ESP.getResetReason().c_str());
    shell.printfln(F("Reset info:    %s"), ESP.getResetInfo().c_str());

    shell.printfln(F("Free heap:                %lu bytes"), (unsigned long)ESP.getFreeHeap());
    shell.printfln(F("Free mem:                 %d  %%"), free_mem());
    shell.printfln(F("Maximum free block size:  %lu bytes"), (unsigned long)ESP.getMaxFreeBlockSize());
    shell.printfln(F("Heap fragmentation:       %u%"), ESP.getHeapFragmentation());
    shell.printfln(F("Free continuations stack: %lu bytes"), (unsigned long)ESP.getFreeContStack());
#elif defined(ESP32)
    shell.printfln(F("SDK version:   %s"), ESP.getSdkVersion());
    shell.printfln(F("CPU frequency: %u MHz"), ESP.getCpuFreqMHz());
    shell.printfln(F("Sketch size:   %u bytes (%u bytes free)"), ESP.getSketchSize(), ESP.getFreeSketchSpace());
    shell.printfln(F("Free heap:                %lu bytes"), (unsigned long)ESP.getFreeHeap());
    shell.printfln(F("Free mem:                 %d  %%"), free_mem());
#endif

    shell.println();

#ifndef EMSESP_STANDALONE

    switch (WiFi.status()) {
    case WL_IDLE_STATUS:
        shell.printfln(F("WiFi: idle"));
        break;

    case WL_NO_SSID_AVAIL:
        shell.printfln(F("WiFi: network not found"));
        break;

    case WL_SCAN_COMPLETED:
        shell.printfln(F("WiFi: network scan complete"));
        break;

    case WL_CONNECTED: {
        shell.printfln(F("WiFi: connected"));
        shell.println();

        shell.printfln(F("SSID: %s"), WiFi.SSID().c_str());
        shell.printfln(F("BSSID: %s"), WiFi.BSSIDstr().c_str());
        shell.printfln(F("RSSI: %d dBm (%d %%)"), WiFi.RSSI(), wifi_quality());
        shell.println();

        shell.printfln(F("MAC address: %s"), WiFi.macAddress().c_str());
#if defined(ESP8266)
        shell.printfln(F("Hostname: %s"), WiFi.hostname().c_str());
#elif defined(ESP32)
        shell.printfln(F("Hostname: %s"), WiFi.getHostname());
#endif

        shell.println();
        shell.printfln(F("IPv4 address: %s/%s"), uuid::printable_to_string(WiFi.localIP()).c_str(), uuid::printable_to_string(WiFi.subnetMask()).c_str());
        shell.printfln(F("IPv4 gateway: %s"), uuid::printable_to_string(WiFi.gatewayIP()).c_str());
        shell.printfln(F("IPv4 nameserver: %s"), uuid::printable_to_string(WiFi.dnsIP()).c_str());
    } break;

    case WL_CONNECT_FAILED:
        shell.printfln(F("WiFi: connection failed"));
        break;

    case WL_CONNECTION_LOST:
        shell.printfln(F("WiFi: connection lost"));
        break;

    case WL_DISCONNECTED:
        shell.printfln(F("WiFi: disconnected"));
        break;

    case WL_NO_SHIELD:
    default:
        shell.printfln(F("WiFi: unknown"));
        break;
    }

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
                                                                                    shell.println(F("Admin password updated"));
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
                                           show_system(shell); // has to be static
                                           shell.println();
                                       });

    EMSESPShell::commands->add_command(ShellContext::SYSTEM,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(set), F_(wifi), F_(hostname)},
                                       flash_string_vector{F_(name_mandatory)},
                                       [](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments) {
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
                                           EMSESP::esp8266React.getWiFiSettingsService()->update(
                                               [&](WiFiSettings & wifiSettings) {
                                                   wifiSettings.ssid = arguments.front().c_str();
                                                   return StateUpdateResult::CHANGED;
                                               },
                                               "local");
                                           shell.println(F("WiFi SSID updated"));
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
                                                                                    EMSESP::esp8266React.getWiFiSettingsService()->update(
                                                                                        [&](WiFiSettings & wifiSettings) {
                                                                                            wifiSettings.password = password2.c_str();
                                                                                            return StateUpdateResult::CHANGED;
                                                                                        },
                                                                                        "local");
                                                                                    shell.println(F("WiFi password updated"));
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
        CommandFlags::USER,
        flash_string_vector{F_(set)},
        [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
            EMSESP::esp8266React.getWiFiSettingsService()->read([&](WiFiSettings & wifiSettings) {
                shell.printfln(F_(hostname_fmt), wifiSettings.hostname.isEmpty() ? uuid::read_flash_string(F_(unset)).c_str() : wifiSettings.hostname.c_str());
            });

            if (shell.has_flags(CommandFlags::ADMIN)) {
                shell.printfln("Wifi:");
                EMSESP::esp8266React.getWiFiSettingsService()->read([&](WiFiSettings & wifiSettings) {
                    shell.print(" ");
                    shell.printfln(F_(wifi_ssid_fmt), wifiSettings.ssid.isEmpty() ? uuid::read_flash_string(F_(unset)).c_str() : wifiSettings.ssid.c_str());
                    shell.print(" ");
                    shell.printfln(F_(wifi_password_fmt), wifiSettings.ssid.isEmpty() ? F_(unset) : F_(asterisks));
                });

                EMSESP::emsespSettingsService.read([&](EMSESPSettings & settings) {
                    shell.printfln(F("Syslog:"));
                    shell.print(" ");
                    shell.printfln(F_(host_fmt), !settings.syslog_host.isEmpty() ? settings.syslog_host.c_str() : uuid::read_flash_string(F_(unset)).c_str());
                    shell.print(" ");
                    shell.printfln(F_(log_level_fmt), uuid::log::format_level_uppercase(static_cast<uuid::log::Level>(settings.syslog_level)));
                    shell.print(" ");
                    shell.printfln(F_(mark_interval_fmt), settings.syslog_mark_interval);
                    shell.println();
                    bool system_heartbeat;
                    EMSESP::esp8266React.getMqttSettingsService()->read([&](MqttSettings & settings) { system_heartbeat = settings.system_heartbeat; });
                    shell.printfln(F_(system_heartbeat_fmt), system_heartbeat ? F_(enabled) : F_(disabled));
                });
            }
        });

    EMSESPShell::commands->add_command(ShellContext::SYSTEM,
                                       CommandFlags::USER,
                                       flash_string_vector{F_(show), F_(mqtt)},
                                       [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) { Mqtt::show_mqtt(shell); });


    // enter the context
    Console::enter_custom_context(shell, context);
}


} // namespace emsesp
