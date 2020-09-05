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

namespace emsesp {

uuid::log::Logger System::logger_{F_(system), uuid::log::Facility::KERN};

#ifndef EMSESP_STANDALONE
uuid::syslog::SyslogService System::syslog_;
#endif

// init statics
uint32_t System::heap_start_    = 0;
int      System::reset_counter_ = 0;
bool     System::upload_status_ = false;
bool     System::hide_led_      = false;
uint8_t  System::led_gpio_      = 0;
uint16_t System::analog_        = 0;

// send on/off to a gpio pin
// value: true = HIGH, false = LOW
void System::mqtt_command_pin(const char * value, const int8_t id) {
    bool v = false;
    if (Helpers::value2bool(value, v)) {
        pinMode(id, OUTPUT);
        digitalWrite(id, v);
        LOG_INFO(F("GPIO %d set to %s"), id, v ? "HIGH" : "LOW");
    }
}

// send raw
void System::mqtt_command_send(const char * value, const int8_t id) {
    EMSESP::send_raw_telegram(value); // ignore id
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
    EMSESP::emsespSettingsService.save();                                       // local settings
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
    EMSESP::emsespSettingsService.read([&](EMSESPSettings & settings) {
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

    // fetch system heartbeat
    EMSESP::esp8266React.getMqttSettingsService()->read([&](MqttSettings & settings) { system_heartbeat_ = settings.system_heartbeat; });

    // print boot message
    EMSESP::esp8266React.getWiFiSettingsService()->read(
        [&](WiFiSettings & wifiSettings) { LOG_INFO(F("System %s booted (EMS-ESP version %s)"), wifiSettings.hostname.c_str(), EMSESP_APP_VERSION); });

    syslog_init();     // init SysLog
    set_led();         // init LED
    EMSESP::init_tx(); // start UART
}

// set the LED to on or off when in normal operating mode
void System::set_led() {
    EMSESP::emsespSettingsService.read([&](EMSESPSettings & settings) {
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
    measure_analog();

    // send out heartbeat
    uint32_t currentMillis = uuid::get_uptime();
    if (!last_heartbeat_ || (currentMillis - last_heartbeat_ > SYSTEM_HEARTBEAT_INTERVAL)) {
        last_heartbeat_ = currentMillis;
        if (system_heartbeat_) {
            send_heartbeat();
        }
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
    doc["rxfails"]          = EMSESP::rxservice_.telegram_error_count();
    doc["adc"]              = analog_; //analogRead(A0);

    Mqtt::publish("heartbeat", doc, false); // send to MQTT with retain off. This will add to MQTT queue.
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
#endif
        static uint32_t sum_ = 0;

        if (!analog_) { // init first time
            analog_ = a;
            sum_    = a * 256;
        } else { // simple moving average filter
            sum_    = sum_ * 255 / 256 + a;
            analog_ = sum_ / 256;
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
    shell.printfln(F("Free heap:                %lu bytes"), (unsigned long)ESP.getFreeHeap());
    shell.printfln(F("Free mem:                 %d %%"), free_mem());
    shell.printfln(F("Maximum free block size:  %lu bytes"), (unsigned long)ESP.getMaxFreeBlockSize());
    shell.printfln(F("Heap fragmentation:       %u %%"), ESP.getHeapFragmentation());
    shell.printfln(F("Free continuations stack: %lu bytes"), (unsigned long)ESP.getFreeContStack());
#elif defined(ESP32)
    shell.printfln(F("SDK version:   %s"), ESP.getSdkVersion());
    shell.printfln(F("CPU frequency: %u MHz"), ESP.getCpuFreqMHz());
#endif
    shell.printfln(F("Sketch size:   %u bytes (%u bytes free)"), ESP.getSketchSize(), ESP.getFreeSketchSpace());
    shell.printfln(F("Free heap:                %lu bytes"), (unsigned long)ESP.getFreeHeap());
    shell.printfln(F("Free mem:                 %d  %%"), free_mem());
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

    EMSESP::emsespSettingsService.read([&](EMSESPSettings & settings) {
        shell.println();
        shell.printfln(F("Syslog:"));
        shell.print(" ");
        shell.printfln(F_(host_fmt), !settings.syslog_host.isEmpty() ? settings.syslog_host.c_str() : uuid::read_flash_string(F_(unset)).c_str());
        shell.print(" ");
        shell.printfln(F_(log_level_fmt), uuid::log::format_level_lowercase(static_cast<uuid::log::Level>(settings.syslog_level)));
        shell.print(" ");
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
                                               shell.print(" ");
                                               shell.printfln(F_(hostname_fmt),
                                                              wifiSettings.hostname.isEmpty() ? uuid::read_flash_string(F_(unset)).c_str()
                                                                                              : wifiSettings.hostname.c_str());
                                           });

                                           EMSESP::esp8266React.getWiFiSettingsService()->read([&](WiFiSettings & wifiSettings) {
                                               shell.print(" ");
                                               shell.printfln(F_(wifi_ssid_fmt),
                                                              wifiSettings.ssid.isEmpty() ? uuid::read_flash_string(F_(unset)).c_str()
                                                                                          : wifiSettings.ssid.c_str());
                                               shell.print(" ");
                                               shell.printfln(F_(wifi_password_fmt), wifiSettings.ssid.isEmpty() ? F_(unset) : F_(asterisks));
                                           });
                                       });
    /*
    EMSESPShell::commands->add_command(ShellContext::SYSTEM,
                                       CommandFlags::USER,
                                       flash_string_vector{F_(show), F_(mqtt)},
                                       [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) { Mqtt::show_mqtt(shell); });
*/
    EMSESPShell::commands->add_command(ShellContext::SYSTEM,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(show), F_(users)},
                                       [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) { System::show_users(shell); });

    EMSESPShell::commands->add_command(ShellContext::SYSTEM,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(pin)},
                                       flash_string_vector{F_(gpio_mandatory), F_(data_optional)},
                                       [](Shell & shell, const std::vector<std::string> & arguments) {
                                           if (arguments.size() == 1) {
                                               shell.printfln(F("use on/off, 1/0 or true/false"));
                                               return;
                                           }
                                           int pin = 0;
                                           if (Helpers::value2number(arguments[0].c_str(), pin)) {
                                               System::mqtt_command_pin(arguments[1].c_str(), pin);
                                           }
                                       });

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
        Serial.flush();
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
        Serial.flush();
        Serial.end();
#endif
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
            EMSESP::emsespSettingsService.begin();

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
                    mqttSettings.host             = mqtt["ip"] | FACTORY_MQTT_HOST;
                    mqttSettings.mqtt_format      = (mqtt["nestedjson"] ? MQTT_format::NESTED : MQTT_format::SINGLE);
                    mqttSettings.mqtt_qos         = mqtt["qos"] | 0;
                    mqttSettings.username         = mqtt["user"] | "";
                    mqttSettings.password         = mqtt["password"] | "";
                    mqttSettings.port             = mqtt["port"] | FACTORY_MQTT_PORT;
                    mqttSettings.clientId         = FACTORY_MQTT_CLIENT_ID;
                    mqttSettings.enabled          = mqtt["enabled"];
                    mqttSettings.system_heartbeat = mqtt["heartbeat"];
                    mqttSettings.keepAlive        = FACTORY_MQTT_KEEP_ALIVE;
                    mqttSettings.cleanSession     = FACTORY_MQTT_CLEAN_SESSION;
                    mqttSettings.maxTopicLength   = FACTORY_MQTT_MAX_TOPIC_LENGTH;

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
            EMSESP::emsespSettingsService.update(
                [&](EMSESPSettings & settings) {
                    settings.tx_mode              = custom_settings["tx_mode"] | EMSESP_DEFAULT_TX_MODE;
                    settings.shower_alert         = custom_settings["shower_alert"] | EMSESP_DEFAULT_SHOWER_ALERT;
                    settings.shower_timer         = custom_settings["shower_timer"] | EMSESP_DEFAULT_SHOWER_TIMER;
                    settings.master_thermostat    = custom_settings["master_thermostat"] | EMSESP_DEFAULT_MASTER_THERMOSTAT;
                    settings.ems_bus_id           = custom_settings["bus_id"] | EMSESP_DEFAULT_EMS_BUS_ID;
                    settings.syslog_host          = EMSESP_DEFAULT_SYSLOG_HOST;
                    settings.syslog_level         = EMSESP_DEFAULT_SYSLOG_LEVEL;
                    settings.syslog_mark_interval = EMSESP_DEFAULT_SYSLOG_MARK_INTERVAL;

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

} // namespace emsesp
