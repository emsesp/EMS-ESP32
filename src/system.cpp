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

#include "version.h" // version of EMS-ESP

MAKE_PSTR_WORD(syslog)
MAKE_PSTR_WORD(mark)
MAKE_PSTR_WORD(level)
MAKE_PSTR_WORD(host)
MAKE_PSTR_WORD(passwd)
MAKE_PSTR_WORD(hostname)
MAKE_PSTR_WORD(wifi)
MAKE_PSTR_WORD(ssid)

MAKE_PSTR(host_fmt, "Host = %s")
MAKE_PSTR(hostname_fmt, "Hostname = %s")
MAKE_PSTR(mark_interval_fmt, "Mark interval = %lus");
MAKE_PSTR(wifi_ssid_fmt, "WiFi SSID = %s");
MAKE_PSTR(wifi_password_fmt, "WiFi Password = %S")

MAKE_PSTR(logger_name, "system")

namespace emsesp {

uuid::log::Logger System::logger_{F_(logger_name), uuid::log::Facility::KERN};

#ifndef EMSESP_STANDALONE
uuid::syslog::SyslogService System::syslog_;
#endif

#if defined(ESP8266)
RTCVars System::state_;
#endif

uint32_t System::heap_start_ = 0;
bool     System::safe_mode_  = false;
int      System::reset_counter_;

// handle generic system related MQTT commands
void System::mqtt_commands(const char * message) {
    StaticJsonDocument<EMSESP_MAX_JSON_SIZE_SMALL> doc;
    DeserializationError                           error = deserializeJson(doc, message);
    if (error) {
        LOG_DEBUG(F("MQTT error: payload %s, error %s"), message, error.c_str());
        return;
    }

    // restart EMS-ESP
    if (strcmp(message, "restart") == 0) {
        LOG_INFO(F("Restart command received"));
        restart();
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
// mode = safe mode. true to enable on next boot
void System::restart(bool mode) {
    // check for safe mode
    if (mode) {
        LOG_NOTICE("Restarting system in safe mode...");
    } else {
        LOG_NOTICE("Restarting system...");
    }

    Shell::loop_all();
    save_safe_mode(mode);
    delay(1000); // wait a second
#if defined(ESP8266)
    ESP.reset();
#elif defined(ESP32)
    ESP.restart();
#endif
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

#ifndef EMSESP_STANDALONE
    syslog_.start(); // syslog service
#endif

    config_syslog();

    Settings settings;

    // update the version to the latest build
    settings.app_version(EMSESP_APP_VERSION);
    settings.commit();

#if defined(ESP32)
    LOG_INFO(F("System booted (EMS-ESP version %s ESP32)"), settings.app_version().c_str());
#else
    LOG_INFO(F("System booted (EMS-ESP version %s)"), settings.app_version().c_str());
#endif

    if (LED_GPIO) {
        pinMode(LED_GPIO, OUTPUT); // LED pin, 0 is disabled
    }

    // register MQTT system commands
    Mqtt::subscribe("cmd", std::bind(&System::mqtt_commands, this, _1));

// RTC state variables - only for ESP8266
#if defined(ESP8266)
    state_.registerVar(&reset_counter_); // we send a pointer to each of our variables
    state_.registerVar(&safe_mode_);
    state_.loadFromRTC();
#endif

// safe mode is forced at compile time
#ifdef EMSESP_SAFE_MODE
    safe_mode_ = true;
#endif

    // if there is no wifi ssid set (like an inital setup) go into safe mode
    if (settings.wifi_ssid().empty()) {
        safe_mode_ = true;
    }

#ifndef EMSESP_STANDALONE
    // if we're in safe switch is back so next boot it'll be normal. Safe mode is a one-off
    if (safe_mode()) {
    } else {
        save_safe_mode(false); // next time boot up in normal mode
        EMSuart::start(settings.ems_tx_mode());
    }
#endif
}

// gets/sets the safe mode from RTC
bool System::safe_mode() {
    return safe_mode_;
}

// set safe mode and save it to RTC
void System::safe_mode(const bool safe_mode) {
    safe_mode_ = safe_mode;
}

// set safe mode and save it to RTC
void System::save_safe_mode(const bool safe_mode) {
    bool curr_save_mode = safe_mode_; // remember current setting
    safe_mode_          = safe_mode;
#if defined(ESP8266)
    state_.saveToRTC();
#endif
    safe_mode_ = curr_save_mode; // restore
}

// sets up syslog
void System::config_syslog() {
#ifndef EMSESP_STANDALONE
    Settings  settings;
    IPAddress addr;

    if (!addr.fromString(settings.syslog_host().c_str())) {
        addr = (uint32_t)0;
    }

    syslog_.hostname(settings.hostname());
    syslog_.log_level(settings.syslog_level());
    syslog_.mark_interval(settings.syslog_mark_interval());
    syslog_.destination(addr);

#endif
}

// checks system health and handles LED flashing wizardry
void System::loop() {
#ifndef EMSESP_STANDALONE
    syslog_.loop();
#endif

    if (LED_GPIO) {
        led_monitor(); // check status and report back using the LED
    }

    system_check(); // check system health
}

void System::show_mem(const char * text) {
#ifndef EMSESP_STANDALONE
    uint32_t mem = ESP.getFreeHeap();
#else
    uint32_t mem = 1000;
#endif
    LOG_NOTICE(F("{%s} Free mem: %ld (%d%%)"), text, mem, (100 * mem / heap_start_));
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
        if ((WiFi.status() != WL_CONNECTED) || safe_mode()) {
            set_led_speed(LED_WARNING_BLINK_FAST);
            system_healthy_ = false;
            return;
        }
#endif

        // not healthy if bus not connected
        if (!EMSbus::bus_connected()) {
            system_healthy_ = false;
            set_led_speed(LED_WARNING_BLINK); // flash every 1/2 second from now on
            LOG_ERROR(F("No connection to the EMS bus!"));
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
    static uint32_t led_last_blink_ = 0;

    if (!led_last_blink_ || (uint32_t)(uuid::get_uptime() - led_last_blink_) >= led_flash_speed_) {
        led_last_blink_ = uuid::get_uptime();

        // if bus_not_connected or network not connected, start flashing
        if (!system_healthy_) {
            digitalWrite(LED_GPIO, !digitalRead(LED_GPIO));
        }
    }
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

    FSInfo info;
    if (EMSESP_FS.info(info)) {
        shell.printfln(F("FS size:   %zu bytes (block size %zu bytes, page size %zu bytes)"), info.totalBytes, info.blockSize, info.pageSize);
        shell.printfln(F("FS used:   %zu bytes"), info.usedBytes);
    }
#elif defined(ESP32)

    shell.printfln(F("SDK version:   %s"), ESP.getSdkVersion());
    shell.printfln(F("CPU frequency: %u MHz"), ESP.getCpuFreqMHz());
    shell.printfln(F("Sketch size:   %u bytes (%u bytes free)"), ESP.getSketchSize(), ESP.getFreeSketchSpace());
    shell.printfln(F("Free heap:                %lu bytes"), (unsigned long)ESP.getFreeHeap());
    shell.printfln(F("Free mem:                 %d  %%"), free_mem());

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
                                                   Settings settings;
                                                   if ((settings.admin_password().empty()) || (!password.empty() && password == settings.admin_password())) {
                                                       settings.format(shell);
                                                   } else {
                                                       shell.println(F("incorrect password"));
                                                   }
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
                                                                                    Settings settings;
                                                                                    settings.admin_password(password2);
                                                                                    settings.commit();
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
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(wifi), F_(disconnect)},
                                       [](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments __attribute__((unused))) {
                                           Network::disconnect();
                                       });

    EMSESPShell::commands->add_command(ShellContext::SYSTEM,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(wifi), F_(reconnect)},
                                       [](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments __attribute__((unused))) {
                                           Network::reconnect();
                                       });

    EMSESPShell::commands->add_command(ShellContext::SYSTEM,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(wifi), F_(scan)},
                                       [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) { Network::scan(shell); });

    EMSESPShell::commands->add_command(ShellContext::SYSTEM,
                                       CommandFlags::USER,
                                       flash_string_vector{F_(show)},
                                       [=](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                                           Network::show_network(shell);
                                           show_system(shell);
                                           shell.println();
                                       });

    EMSESPShell::commands->add_command(ShellContext::SYSTEM,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(set), F_(syslog), F_(host)},
                                       flash_string_vector{F_(ip_address_optional)},
                                       [](Shell & shell, const std::vector<std::string> & arguments) {
                                           Settings settings;
                                           if (!arguments.empty()) {
                                               settings.syslog_host(arguments[0]);
                                               settings.commit();
                                               shell.println(F("Please restart EMS-ESP"));
                                           }
                                           auto host = settings.syslog_host();
                                           shell.printfln(F_(host_fmt), !host.empty() ? host.c_str() : uuid::read_flash_string(F_(unset)).c_str());
                                       });

    EMSESPShell::commands->add_command(
        ShellContext::SYSTEM,
        CommandFlags::ADMIN,
        flash_string_vector{F_(set), F_(syslog), F_(level)},
        flash_string_vector{F_(log_level_optional)},
        [](Shell & shell, const std::vector<std::string> & arguments) {
            Settings settings;
            if (!arguments.empty()) {
                uuid::log::Level level;

                if (uuid::log::parse_level_lowercase(arguments[0], level)) {
                    settings.syslog_level(level);
                    settings.commit();
                    shell.println(F("Please restart EMS-ESP"));
                } else {
                    shell.printfln(F_(invalid_log_level));
                    return;
                }
            }
            shell.printfln(F_(log_level_fmt), uuid::log::format_level_uppercase(settings.syslog_level()));
        },
        [](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments __attribute__((unused))) -> std::vector<std::string> {
            return uuid::log::levels_lowercase();
        });

    EMSESPShell::commands->add_command(ShellContext::SYSTEM,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(set), F_(syslog), F_(mark)},
                                       flash_string_vector{F_(seconds_optional)},
                                       [](Shell & shell, const std::vector<std::string> & arguments) {
                                           Settings settings;
                                           if (!arguments.empty()) {
                                               settings.syslog_mark_interval(String(arguments[0].c_str()).toInt());
                                               settings.commit();
                                               shell.println(F("Please restart EMS-ESP"));
                                           }
                                           shell.printfln(F_(mark_interval_fmt), settings.syslog_mark_interval());
                                       });

    EMSESPShell::commands->add_command(ShellContext::SYSTEM,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(set), F_(wifi), F_(hostname)},
                                       flash_string_vector{F_(name_mandatory)},
                                       [](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments) {
                                           Settings settings;
                                           settings.hostname(arguments.front());
                                           settings.commit();
                                       });

    EMSESPShell::commands->add_command(
        ShellContext::SYSTEM,
        CommandFlags::ADMIN,
        flash_string_vector{F_(set), F_(wifi), F_(ssid)},
        flash_string_vector{F_(name_mandatory)},
        [](Shell & shell, const std::vector<std::string> & arguments) {
            Settings settings;
            settings.wifi_ssid(arguments.front());
            settings.commit();
            shell.printfln(F_(wifi_ssid_fmt), settings.wifi_ssid().empty() ? uuid::read_flash_string(F_(unset)).c_str() : settings.wifi_ssid().c_str());
            shell.println(F("WiFi SSID updated. Please restart"));
            Network::reconnect();
        },
        [](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments __attribute__((unused))) -> std::vector<std::string> {
            Settings settings;
            return std::vector<std::string>{settings.wifi_ssid()};
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
                                                                                    Settings settings;
                                                                                    settings.wifi_password(password2);
                                                                                    settings.commit();
                                                                                    shell.println(F("WiFi password updated. Please restart"));
                                                                                    Network::reconnect();
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
                                           Settings settings;
                                           shell.printfln(F_(hostname_fmt),
                                                          settings.hostname().empty() ? uuid::read_flash_string(F_(unset)).c_str() : settings.hostname().c_str());

                                           if (shell.has_flags(CommandFlags::ADMIN)) {
                                               shell.printfln("Wifi:");
                                               shell.print(" ");
                                               shell.printfln(F_(wifi_ssid_fmt),
                                                              settings.wifi_ssid().empty() ? uuid::read_flash_string(F_(unset)).c_str()
                                                                                           : settings.wifi_ssid().c_str());
                                               shell.print(" ");
                                               shell.printfln(F_(wifi_password_fmt), settings.wifi_password().empty() ? F_(unset) : F_(asterisks));
                                               shell.printfln(F("Syslog:"));
                                               auto host = settings.syslog_host();
                                               shell.print(" ");
                                               shell.printfln(F_(host_fmt), !host.empty() ? host.c_str() : uuid::read_flash_string(F_(unset)).c_str());
                                               shell.print(" ");
                                               shell.printfln(F_(log_level_fmt), uuid::log::format_level_uppercase(settings.syslog_level()));
                                               shell.print(" ");
                                               shell.printfln(F_(mark_interval_fmt), settings.syslog_mark_interval());
                                           }
                                       });

    // enter the context
    Console::enter_custom_context(shell, context);
}


} // namespace emsesp
