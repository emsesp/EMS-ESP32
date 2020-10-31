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

#include "console.h"
#include "emsesp.h"
#include "version.h"

#if defined(EMSESP_DEBUG)
#include "test/test.h"
#endif

namespace emsesp {

std::shared_ptr<Commands> EMSESPShell::commands = [] {
    std::shared_ptr<Commands> commands = std::make_shared<Commands>();
    return commands;
}();

static std::shared_ptr<EMSESPShell> shell;

std::vector<bool> EMSESPStreamConsole::ptys_;

#ifndef EMSESP_STANDALONE
uuid::telnet::TelnetService telnet_([](Stream & stream, const IPAddress & addr, uint16_t port) -> std::shared_ptr<uuid::console::Shell> {
    return std::make_shared<EMSESPStreamConsole>(stream, addr, port);
});
#endif

EMSESPShell::EMSESPShell()
    : Shell() {
}

void EMSESPShell::started() {
    logger().log(LogLevel::INFO, LogFacility::CONSOLE, F("User session opened on console %s"), console_name().c_str());
}

void EMSESPShell::stopped() {
    if (has_flags(CommandFlags::ADMIN)) {
        logger().log(LogLevel::INFO, LogFacility::AUTH, F("su session closed on console %s"), console_name().c_str());
    }
    logger().log(LogLevel::INFO, LogFacility::CONSOLE, F("User session closed on console %s"), console_name().c_str());

    // remove all custom contexts
    commands->remove_all_commands();

    console_commands_loaded_ = false; // make sure they get reloaded next time a console is opened
}

// show welcome banner
// this is one of the first functions called when the shell is started
void EMSESPShell::display_banner() {
    println();
    printfln(F("┌──────────────────────────────────────────┐"));
    printfln(F("│ %sEMS-ESP version %-10s%s               │"), COLOR_BOLD_ON, EMSESP_APP_VERSION, COLOR_BOLD_OFF);
    printfln(F("│ %s%shttps://github.com/proddy/EMS-ESP%s        │"), COLOR_BRIGHT_GREEN, COLOR_UNDERLINE, COLOR_RESET);
    printfln(F("│                                          │"));
    printfln(F("│ type %shelp%s to show available commands     │"), COLOR_UNDERLINE, COLOR_RESET);
    printfln(F("└──────────────────────────────────────────┘"));
    println();

    // set console name
    EMSESP::esp8266React.getWiFiSettingsService()->read([&](WiFiSettings & wifiSettings) { console_hostname_ = wifiSettings.hostname.c_str(); });

    if (console_hostname_.empty()) {
        console_hostname_.resize(16, '\0');
#if defined(ESP8266)
        snprintf_P(&console_hostname_[0], console_hostname_.capacity() + 1, PSTR("esp8266"));
#else
        snprintf_P(&console_hostname_[0], console_hostname_.capacity() + 1, PSTR("esp32"));
#endif
    }

    // load the list of commands
    add_console_commands();

    // turn off watch, unless is test mode
    emsesp::EMSESP::watch_id(WATCH_ID_NONE);
#if defined(EMSESP_STANDALONE)
    emsesp::EMSESP::watch(EMSESP::WATCH_ON);
#else
    emsesp::EMSESP::watch(EMSESP::WATCH_OFF);
#endif
}

// pre-loads all the console commands into the MAIN context
// This is only done after a connection is established, to save on Heap memory
void EMSESPShell::add_console_commands() {
    // if we already have these commands loaded, stop adding duplicates
    // for example when opening multiple serial/telnet sessions
    if (console_commands_loaded_) {
        return;
    }

    // just in case, remove everything
    // commands->remove_context_commands(ShellContext::MAIN);
    commands->remove_all_commands();

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::USER,
                          flash_string_vector{F_(fetch)},
                          [&](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                              shell.printfln(F("Requesting data from EMS devices"));
                              EMSESP::fetch_device_values();
                          });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::ADMIN,
                          flash_string_vector{F_(publish)},
                          flash_string_vector{F_(ha_optional)},
                          [&](Shell & shell, const std::vector<std::string> & arguments) {
                              if (arguments.empty()) {
                                  EMSESP::publish_all();
                                  shell.printfln(F("Published all data to MQTT"));
                              } else {
                                  EMSESP::publish_all(true);
                                  shell.printfln(F("Published all data to MQTT, including HA configs"));
                              }
                          });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::USER,
                          flash_string_vector{F_(show)},
                          [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                              shell.printfln(F("%s%sEMS-ESP version %s%s"), COLOR_BRIGHT_GREEN, COLOR_BOLD_ON, EMSESP_APP_VERSION, COLOR_RESET);
                              shell.println();
                              EMSESP::show_device_values(shell);
                              EMSESP::show_sensor_values(shell);
                          });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::USER,
                          flash_string_vector{F_(show), F_(devices)},
                          [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) { EMSESP::show_devices(shell); });


    commands->add_command(ShellContext::MAIN,
                          CommandFlags::USER,
                          flash_string_vector{F_(show), F_(ems)},
                          [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) { EMSESP::show_ems(shell); });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::USER,
                          flash_string_vector{F_(show), F_(values)},
                          [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) { EMSESP::show_device_values(shell); });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::USER,
                          flash_string_vector{F_(show), F_(mqtt)},
                          [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) { Mqtt::show_mqtt(shell); });


    commands->add_command(ShellContext::MAIN,
                          CommandFlags::USER,
                          flash_string_vector{F_(show), F_(commands)},
                          [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) { Command::show_all(shell); });

    commands->add_command(
        ShellContext::MAIN,
        CommandFlags::ADMIN,
        flash_string_vector{F_(set), F_(bus_id)},
        flash_string_vector{F_(deviceid_mandatory)},
        [](Shell & shell, const std::vector<std::string> & arguments) {
            uint8_t device_id = Helpers::hextoint(arguments.front().c_str());
            if ((device_id == 0x0B) || (device_id == 0x0D) || (device_id == 0x0A) || (device_id == 0x0F) || (device_id == 0x12)) {
                EMSESP::webSettingsService.update(
                    [&](WebSettings & settings) {
                        settings.ems_bus_id = device_id;
                        shell.printfln(F_(bus_id_fmt), settings.ems_bus_id);
                        return StateUpdateResult::CHANGED;
                    },
                    "local");
            } else {
                shell.println(F("Must be 0B, 0D, 0A, 0F or 12"));
            }
        },
        [](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments __attribute__((unused))) -> const std::vector<std::string> {
            return std::vector<std::string>{
                read_flash_string(F("0B")),
                read_flash_string(F("0D")),
                read_flash_string(F("0A")),
                read_flash_string(F("0F")),
                read_flash_string(F("12")),

            };
        });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::ADMIN,
                          flash_string_vector{F_(set), F_(tx_mode)},
                          flash_string_vector{F_(n_mandatory)},
                          [](Shell & shell, const std::vector<std::string> & arguments) {
                              uint8_t tx_mode = std::strtol(arguments[0].c_str(), nullptr, 10);
                              // save the tx_mode
                              EMSESP::webSettingsService.update(
                                  [&](WebSettings & settings) {
                                      settings.tx_mode = tx_mode;
                                      shell.printfln(F_(tx_mode_fmt), settings.tx_mode);
                                      return StateUpdateResult::CHANGED;
                                  },
                                  "local");
                          });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::ADMIN,
                          flash_string_vector{F_(scan), F_(devices)},
                          flash_string_vector{F_(deep_optional)},
                          [](Shell & shell, const std::vector<std::string> & arguments) {
                              if (arguments.size() == 0) {
                                  EMSESP::scan_devices();
                              } else {
                                  shell.printfln(F("Performing a deep scan..."));
                                  EMSESP::clear_all_devices();
                                  std::vector<uint8_t> Device_Ids;

                                  Device_Ids.push_back(0x08); // Boilers - 0x08
                                  Device_Ids.push_back(0x38); // HeatPump - 0x38
                                  Device_Ids.push_back(0x30); // Solar Module - 0x30
                                  Device_Ids.push_back(0x09); // Controllers - 0x09
                                  Device_Ids.push_back(0x02); // Connect - 0x02
                                  Device_Ids.push_back(0x48); // Gateway - 0x48
                                  Device_Ids.push_back(0x20); // Mixer Devices - 0x20
                                  Device_Ids.push_back(0x21); // Mixer Devices - 0x21
                                  Device_Ids.push_back(0x22); // Mixer Devices - 0x22
                                  Device_Ids.push_back(0x23); // Mixer Devices - 0x23
                                  Device_Ids.push_back(0x28); // Mixer Devices WW- 0x28
                                  Device_Ids.push_back(0x29); // Mixer Devices WW- 0x29
                                  Device_Ids.push_back(0x10); // Thermostats - 0x10
                                  Device_Ids.push_back(0x17); // Thermostats - 0x17
                                  Device_Ids.push_back(0x18); // Thermostat remote - 0x18
                                  Device_Ids.push_back(0x19); // Thermostat remote - 0x19
                                  Device_Ids.push_back(0x1A); // Thermostat remote - 0x1A
                                  Device_Ids.push_back(0x1B); // Thermostat remote - 0x1B
                                  Device_Ids.push_back(0x11); // Switches - 0x11

                                  // send the read command with Version command
                                  for (const uint8_t device_id : Device_Ids) {
                                      EMSESP::send_read_request(EMSdevice::EMS_TYPE_VERSION, device_id);
                                  }
                              }
                          });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::USER,
                          flash_string_vector{F_(set)},
                          [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                              EMSESP::webSettingsService.read([&](WebSettings & settings) {
                                  shell.printfln(F_(tx_mode_fmt), settings.tx_mode);
                                  shell.printfln(F_(bus_id_fmt), settings.ems_bus_id);
                                  char buffer[4];
                                  shell.printfln(F_(master_thermostat_fmt),
                                                 settings.master_thermostat == 0 ? uuid::read_flash_string(F_(auto)).c_str()
                                                                                 : Helpers::hextoa(buffer, settings.master_thermostat));
                              });
                          });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::ADMIN,
                          flash_string_vector{F_(read)},
                          flash_string_vector{F_(deviceid_mandatory), F_(typeid_mandatory), F_(offset_optional)},
                          [=](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments) {
                              uint8_t  device_id = Helpers::hextoint(arguments.front().c_str());
                              uint16_t type_id   = Helpers::hextoint(arguments[1].c_str());
                              EMSESP::set_read_id(type_id);
                              if (arguments.size() == 3) {
                                  uint16_t offset = Helpers::hextoint(arguments.back().c_str());
                                  EMSESP::send_read_request(type_id, device_id, offset);
                              } else {
                                  EMSESP::send_read_request(type_id, device_id);
                              }
                          });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::ADMIN,
                          flash_string_vector{F_(set), F_(master), F_(thermostat)},
                          flash_string_vector{F_(deviceid_mandatory)},
                          [](Shell & shell, const std::vector<std::string> & arguments) {
                              uint8_t value = Helpers::hextoint(arguments.front().c_str());
                              EMSESP::webSettingsService.update(
                                  [&](WebSettings & settings) {
                                      settings.master_thermostat = value;
                                      EMSESP::actual_master_thermostat(value); // set the internal value too
                                      char buffer[5];
                                      shell.printfln(F_(master_thermostat_fmt),
                                                     !value ? uuid::read_flash_string(F_(auto)).c_str() : Helpers::hextoa(buffer, value));
                                      return StateUpdateResult::CHANGED;
                                  },
                                  "local");
                          });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::ADMIN,
                          flash_string_vector{F_(send), F_(telegram)},
                          flash_string_vector{F_(data_mandatory)},
                          [](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments) {
                              EMSESP::send_raw_telegram(arguments.front().c_str());
                          });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::USER,
                          flash_string_vector{F_(watch)},
                          flash_string_vector{F_(watch_format_optional), F_(watchid_optional)},
                          [](Shell & shell, const std::vector<std::string> & arguments) {
                              uint16_t watch_id = WATCH_ID_NONE;

                              if (!arguments.empty()) {
                                  // get raw/pretty
                                  if (arguments[0] == read_flash_string(F_(raw))) {
                                      emsesp::EMSESP::watch(EMSESP::WATCH_RAW); // raw
                                  } else if (arguments[0] == read_flash_string(F_(on))) {
                                      emsesp::EMSESP::watch(EMSESP::WATCH_ON); // on
                                  } else if (arguments[0] == read_flash_string(F_(off))) {
                                      emsesp::EMSESP::watch(EMSESP::WATCH_OFF); // off
                                  } else if (emsesp::EMSESP::watch() == EMSESP::WATCH_OFF) {
                                      shell.printfln(F_(invalid_watch));
                                      return;
                                  } else {
                                      watch_id = Helpers::hextoint(arguments[0].c_str());
                                  }

                                  if (arguments.size() == 2) {
                                      // get the watch_id if its set
                                      watch_id = Helpers::hextoint(arguments[1].c_str());
                                  }

                                  emsesp::EMSESP::watch_id(watch_id);
                              }

                              uint8_t watch = emsesp::EMSESP::watch();
                              if (watch == EMSESP::WATCH_OFF) {
                                  shell.printfln(F("Watching telegrams is off"));
                                  return;
                              }

                              // if logging is off, the watch won't show anything, show force it back to NOTICE
                              if (!shell.logger().enabled(Level::NOTICE)) {
                                  shell.log_level(Level::NOTICE);
                              }

                              if (watch == EMSESP::WATCH_ON) {
                                  shell.printfln(F("Watching incoming telegrams, displayed in decoded format"));
                              } else {
                                  shell.printfln(F("Watching incoming telegrams, displayed as raw bytes")); // WATCH_RAW
                              }

                              watch_id = emsesp::EMSESP::watch_id();
                              if (watch_id > 0x80) {
                                  shell.printfln(F("Filtering only telegrams that match a telegram type of 0x%02X"), watch_id);
                              } else if (watch_id != WATCH_ID_NONE) {
                                  shell.printfln(F("Filtering only telegrams that match a device ID or telegram type of 0x%02X"), watch_id);
                              }
                          });

    commands->add_command(
        ShellContext::MAIN,
        CommandFlags::ADMIN,
        flash_string_vector{F_(call)},
        flash_string_vector{F_(device_type_optional), F_(cmd_optional), F_(data_optional), F_(id_optional)},
        [&](Shell & shell, const std::vector<std::string> & arguments) {
            if (arguments.empty()) {
                Command::show_all(shell); // list options
                return;
            }

            // validate the device_type
            uint8_t device_type = EMSdevice::device_name_2_device_type(arguments[0].c_str());
            if (!Command::device_has_commands(device_type)) {
                shell.print(F("Invalid device. Available devices are: "));
                Command::show_devices(shell);
                return;
            }

            // validate the command
            if (arguments.size() < 2) {
                shell.print(F("Available commands are: "));
                Command::show(shell, device_type);
                return;
            }

            const char * cmd = arguments[1].c_str();
            if (!Command::find_command(device_type, cmd)) {
                shell.print(F("Unknown command. Available commands are: "));
                Command::show(shell, device_type);
                return;
            }

            DynamicJsonDocument doc(EMSESP_MAX_JSON_SIZE_LARGE);
            JsonObject          json = doc.to<JsonObject>();

            bool ok = false;
            if (arguments.size() == 2) {
                // no value specified, just the cmd
                ok = Command::call(device_type, cmd, nullptr, -1, json);
            } else if (arguments.size() == 3) {
                // has a value but no id
                ok = Command::call(device_type, cmd, arguments.back().c_str(), -1, json);
            } else {
                // use value, which could be an id or hc
                ok = Command::call(device_type, cmd, arguments[2].c_str(), atoi(arguments[3].c_str()), json);
            }

            if (ok && json.size()) {
                doc.shrinkToFit();
                serializeJsonPretty(doc, shell);
                shell.println();
            }
        },
        [&](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments) -> std::vector<std::string> {
            if (arguments.size() == 0) {
                std::vector<std::string> devices_list;
                devices_list.emplace_back(EMSdevice::device_type_2_device_name(EMSdevice::DeviceType::SYSTEM));
                devices_list.emplace_back(EMSdevice::device_type_2_device_name(EMSdevice::DeviceType::DALLASSENSOR));
                for (const auto & device_class : EMSFactory::device_handlers()) {
                    if (Command::device_has_commands(device_class.first)) {
                        devices_list.emplace_back(EMSdevice::device_type_2_device_name(device_class.first));
                    }
                }
                return devices_list;
            } else if (arguments.size() == 1) {
                std::vector<std::string> command_list;
                uint8_t                  device_type = EMSdevice::device_name_2_device_type(arguments[0].c_str());
                if (Command::device_has_commands(device_type)) {
                    for (const auto & cf : Command::commands()) {
                        if (cf.device_type_ == device_type) {
                            command_list.emplace_back(uuid::read_flash_string(cf.cmd_));
                        }
                    }
                    return command_list;
                }
            }

            return {};
        });

    /*
     * add all the submenu contexts...
     */

    // System
    commands->add_command(ShellContext::MAIN,
                          CommandFlags::USER,
                          flash_string_vector{F_(system)},
                          [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                              System::console_commands(shell, ShellContext::SYSTEM);
                          });

    Console::load_standard_commands(ShellContext::MAIN);

    console_commands_loaded_ = true;
}

std::string EMSESPShell::hostname_text() {
    return console_hostname_;
}

// remove commands from the current context to save memory before exiting
bool EMSESPShell::exit_context() {
    unsigned int current_context = context();
    commands->remove_context_commands(current_context);

    if (current_context == ShellContext::MAIN) {
        Shell::stop();
        return true;
    }
    return Shell::exit_context();
}

// enter a custom context (sub-menu)
void Console::enter_custom_context(Shell & shell, unsigned int context) {
    load_standard_commands(context);

    // don't go into the new context if it's already the root (to prevent double loading)
    if (context != ShellContext::MAIN) {
        shell.enter_context(context);
    }
}

// each custom context has the common commands like log, help, exit, su etc
void Console::load_standard_commands(unsigned int context) {
#if defined(EMSESP_DEBUG)
    EMSESPShell::commands->add_command(context,
                                       CommandFlags::USER,
                                       flash_string_vector{F_(test)},
                                       flash_string_vector{F_(name_optional)},
                                       [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                                           if (arguments.size() == 0) {
                                               Test::run_test(shell, "default");
                                           } else {
                                               Test::run_test(shell, arguments.front());
                                           }
                                       });
#endif

#if defined(EMSESP_STANDALONE)
    EMSESPShell::commands->add_command(context,
                                       CommandFlags::USER,
                                       flash_string_vector{F("t")},
                                       flash_string_vector{F_(name_optional)},
                                       [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                                           if (arguments.size() == 0) {
                                               Test::run_test(shell, "default");
                                           } else {
                                               Test::run_test(shell, arguments.front());
                                           }
                                       });
#endif

    EMSESPShell::commands->add_command(
        context,
        CommandFlags::USER,
        flash_string_vector{F_(log)},
        flash_string_vector{F_(log_level_optional)},
        [](Shell & shell, const std::vector<std::string> & arguments) {
            if (!arguments.empty()) {
                uuid::log::Level level;
                if (uuid::log::parse_level_lowercase(arguments[0], level)) {
                    shell.log_level(level);
                } else {
                    shell.printfln(F_(invalid_log_level));
                    return;
                }
            } else {
                shell.print(F("levels: "));
                std::vector<std::string> v = uuid::log::levels_lowercase();
                size_t                   i = v.size();
                while (i--) {
                    shell.printf(v[i].c_str());
                    shell.print(' ');
                }
                shell.println();
            }
            shell.printfln(F_(log_level_fmt), uuid::log::format_level_lowercase(shell.log_level()));
        },
        [](Shell & shell __attribute__((unused)), const std::vector<std::string> & arguments __attribute__((unused))) -> std::vector<std::string> {
            return uuid::log::levels_lowercase();
        });

    EMSESPShell::commands->add_command(context,
                                       CommandFlags::USER,
                                       flash_string_vector{F_(help)},
                                       [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                                           shell.print_all_available_commands();
                                       });

    EMSESPShell::commands->add_command(context,
                                       CommandFlags::USER,
                                       flash_string_vector{F_(exit)},
                                       [=](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                                           // delete MAIN console stuff first to save memory
                                           EMSESPShell::commands->remove_context_commands(context);
                                           shell.exit_context();
                                       });

    EMSESPShell::commands->add_command(context,
                                       CommandFlags::USER,
                                       flash_string_vector{F_(su)},
                                       [=](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                                           auto become_admin = [](Shell & shell) {
                                               shell.logger().log(LogLevel::NOTICE, LogFacility::AUTH, F("su session opened on console"));
                                               shell.add_flags(CommandFlags::ADMIN);
                                           };

                                           if (shell.has_flags(CommandFlags::LOCAL)) {
                                               become_admin(shell);
                                           } else {
                                               shell.enter_password(F_(password_prompt), [=](Shell & shell, bool completed, const std::string & password) {
                                                   if (completed) {
                                                       uint64_t now = uuid::get_uptime_ms();

                                                       EMSESP::esp8266React.getSecuritySettingsService()->read([&](SecuritySettings & securitySettings) {
                                                           if (securitySettings.jwtSecret.equals(password.c_str())) {
                                                               become_admin(shell);
                                                           } else {
                                                               shell.delay_until(now + INVALID_PASSWORD_DELAY_MS, [](Shell & shell) {
                                                                   shell.logger().log(LogLevel::NOTICE, LogFacility::AUTH, F("Invalid su password on console"));
                                                                   shell.println(F("su: incorrect password"));
                                                               });
                                                           }
                                                       });
                                                   }
                                               });
                                           }
                                       });
}

// prompt, change per context
std::string EMSESPShell::context_text() {
    switch (static_cast<ShellContext>(context())) {
    case ShellContext::MAIN:
        return std::string{'/'};

    case ShellContext::SYSTEM:
        return std::string{"/system"};

    default:
        return std::string{};
    }
}

// when in su (admin) show # as the prompt suffix
std::string EMSESPShell::prompt_suffix() {
    if (has_flags(CommandFlags::ADMIN)) {
        return std::string{'#'};
    } else {
        return std::string{'$'};
    }
}

void EMSESPShell::end_of_transmission() {
    invoke_command(uuid::read_flash_string(F_(exit)));
}

EMSESPStreamConsole::EMSESPStreamConsole(Stream & stream, bool local)
    : uuid::console::Shell(commands, ShellContext::MAIN, local ? (CommandFlags::USER | CommandFlags::LOCAL) : CommandFlags::USER)
    , uuid::console::StreamConsole(stream)
    , EMSESPShell()
    , name_(uuid::read_flash_string(F("Serial")))
    , pty_(std::numeric_limits<size_t>::max())
    , addr_()
    , port_(0) {
}

EMSESPStreamConsole::EMSESPStreamConsole(Stream & stream, const IPAddress & addr, uint16_t port)
    : uuid::console::Shell(commands, ShellContext::MAIN, CommandFlags::USER)
    , uuid::console::StreamConsole(stream)
    , EMSESPShell()
    , addr_(addr)
    , port_(port) {
    std::vector<char> text(16);

    pty_ = 0;
    while (pty_ < ptys_.size() && ptys_[pty_])
        pty_++;
    if (pty_ == ptys_.size()) {
        ptys_.push_back(true);
    } else {
        ptys_[pty_] = true;
    }

    snprintf_P(text.data(), text.size(), PSTR("pty%u"), pty_);
    name_ = text.data();
#ifndef EMSESP_STANDALONE
    logger().info(F("Allocated console %s for connection from [%s]:%u"), name_.c_str(), uuid::printable_to_string(addr_).c_str(), port_);
#endif
}

EMSESPStreamConsole::~EMSESPStreamConsole() {
    if (pty_ != SIZE_MAX) {
#ifndef EMSESP_STANDALONE
        logger().info(F("Shutdown console %s for connection from [%s]:%u"), name_.c_str(), uuid::printable_to_string(addr_).c_str(), port_);
#endif
        ptys_[pty_] = false;
        ptys_.shrink_to_fit();

        EMSESPShell::commands->remove_all_commands();
    }
}

std::string EMSESPStreamConsole::console_name() {
    return name_;
}

// Start up telnet and logging
// Log order is off, err, warning, notice, info, debug, trace, all
void Console::start() {
// if we've detected a boot into safe mode on ESP8266, start the Serial console too
// Serial is always on with the ESP32 as it has 2 UARTs
#if defined(ESP32) || defined(EMSESP_STANDALONE) || defined(EMSESP_FORCE_SERIAL)
    serial_console_.begin(SERIAL_CONSOLE_BAUD_RATE);
    serial_console_.println();

    shell = std::make_shared<EMSESPStreamConsole>(serial_console_, true);
    shell->maximum_log_messages(100); // default is 50
    shell->start();
#endif

#ifndef ESP8266
#if defined(EMSESP_DEBUG)
    shell->log_level(uuid::log::Level::DEBUG);
#endif
#endif

#if defined(EMSESP_FORCE_SERIAL)
    shell->log_level(uuid::log::Level::DEBUG);
#endif

#if defined(EMSESP_STANDALONE)
    // always start in su/admin mode when running tests
    shell->add_flags(CommandFlags::ADMIN);
#endif

// start the telnet service
// default idle is 10 minutes, default write timeout is 0 (automatic)
// note, this must be started after the network/wifi for ESP32 otherwise it'll crash
#ifndef EMSESP_STANDALONE
    telnet_.start();
    telnet_.default_write_timeout(1000); // in ms, socket timeout 1 second
#endif

    // turn watch off in case it was still set in the last session
    // emsesp::EMSESP::watch(EMSESP::WATCH_OFF);
}

// handles telnet sync and logging to console
void Console::loop() {
    uuid::loop();

#ifndef EMSESP_STANDALONE
    telnet_.loop();
#endif

#if defined(ESP8266)
    if (!EMSuart::sending()) {
        Shell::loop_all();
    }
#else
    Shell::loop_all();
#endif
}

} // namespace emsesp
