
/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2024  emsesp.org - proddy, MichaelDvP
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
#include "console_stream.h"
#include "emsesp.h"

#if defined(EMSESP_TEST)
#include "../test/test.h"
#endif

using ::uuid::console::Commands;
using ::uuid::console::Shell;
using LogLevel    = ::uuid::log::Level;
using LogFacility = ::uuid::log::Facility;

namespace emsesp {

static constexpr unsigned long INVALID_PASSWORD_DELAY_MS = 3000;

static inline EMSESPShell & to_shell(Shell & shell) {
    return static_cast<EMSESPShell &>(shell);
}

#define NO_ARGUMENTS                                                                                                                                           \
    std::vector<std::string> {                                                                                                                                 \
    }

// add static functions here....

static void console_log_level(Shell & shell, const std::vector<std::string> & arguments) {
    if (!arguments.empty()) {
        uuid::log::Level level;

        if (uuid::log::parse_level_lowercase(arguments[0], level)) {
            shell.log_level(level);
        } else {
            shell.printfln(F_(invalid_log_level));
            return;
        }
    }
    shell.printfln(F_(log_level_fmt), uuid::log::format_level_uppercase(shell.log_level()));
}

static std::vector<std::string> log_level_autocomplete(Shell & shell, const std::vector<std::string> & current_arguments, const std::string & next_argument) {
    return uuid::log::levels_lowercase();
}

static void setup_commands(std::shared_ptr<Commands> const & commands) {
    // exit, help, log
    commands->add_command(ShellContext::MAIN, CommandFlags::USER, {F_(exit)}, EMSESPShell::main_exit_function);
    commands->add_command(ShellContext::MAIN, CommandFlags::USER, {F_(help)}, EMSESPShell::main_help_function);
    commands->add_command(ShellContext::MAIN, CommandFlags::USER, {F_(log)}, {F_(log_level_optional)}, console_log_level, log_level_autocomplete);

    //
    // Show commands
    //
    commands->add_command(
        ShellContext::MAIN,
        CommandFlags::USER,
        {F_(show)},
        {F_(show_commands)},
        [](Shell & shell, const std::vector<std::string> & arguments) {
            if (arguments.empty()) {
                EMSESP::system_.show_system(shell);
                return;
            }

            auto const & command = arguments.front();
            if (command == F_(commands)) {
                Command::show_all(shell);
            } else if (command == F_(system)) {
                EMSESP::system_.show_system(shell);
            } else if (command == F_(users) && (shell.has_flags(CommandFlags::ADMIN))) {
                EMSESP::system_.show_users(shell); // admin only
            } else if (command == F_(devices)) {
                EMSESP::show_devices(shell);
            } else if (command == F_(log)) {
                EMSESP::webLogService.show(shell);
            } else if (command == F_(ems)) {
                EMSESP::show_ems(shell);
            } else if (command == F_(values)) {
                EMSESP::show_device_values(shell);
                EMSESP::show_sensor_values(shell);
            } else if (command == F_(mqtt)) {
                Mqtt::show_mqtt(shell);
            } else {
                shell.printfln("Unknown show command");
            }
        },
        [](Shell const & shell, const std::vector<std::string> & current_arguments, const std::string & next_argument) -> std::vector<std::string> {
            return std::vector<std::string>{"system", "users", "devices", "log", "ems", "values", "mqtt", "commands"};
        });


    //
    // System commands
    //
#if defined(EMSESP_TEST)
    // create commands test
    commands->add_command(ShellContext::MAIN,
                          CommandFlags::USER,
                          {"test"},
                          string_vector{F_(name_optional), F_(data_optional), F_(id_optional)},
                          [=](Shell & shell, const std::vector<std::string> & arguments) {
                              if (arguments.empty()) {
                                  Test::run_test(shell, "default");
                              } else if (arguments.size() == 1) {
                                  Test::run_test(shell, arguments.front());
                              } else if (arguments.size() == 2) {
                                  Test::run_test(shell, arguments[0].c_str(), arguments[1].c_str());
                              } else {
                                  Test::run_test(shell, arguments[0].c_str(), arguments[1].c_str(), arguments[2].c_str());
                              }
                          });
    commands->add_command(ShellContext::MAIN, CommandFlags::USER, {"t"}, [=](Shell & shell, const std::vector<std::string> & arguments) {
        Test::run_test(shell, "default");
    });
#endif

    commands->add_command(ShellContext::MAIN, CommandFlags::USER, {F_(su)}, [=](Shell & shell, const std::vector<std::string> & arguments) {
        auto become_admin = [](Shell & shell) {
            shell.logger().log(LogLevel::NOTICE, LogFacility::AUTH, F("Admin session opened on console %s"), to_shell(shell).console_name().c_str());
            shell.add_flags(CommandFlags::ADMIN);
        };

        if (shell.has_flags(CommandFlags::ADMIN)) {
            return;
        } else if (shell.has_flags(CommandFlags::LOCAL)) {
            become_admin(shell);
        } else {
            shell.enter_password(F_(password_prompt), [=](Shell & shell, bool completed, const std::string & password) {
                if (completed) {
                    uint64_t now = uuid::get_uptime_ms();

                    EMSESP::esp32React.getSecuritySettingsService()->read([&](SecuritySettings & securitySettings) {
                        if (!password.empty() && (securitySettings.jwtSecret.equals(password.c_str()))) {
                            become_admin(shell);
                        } else {
                            shell.delay_until(now + INVALID_PASSWORD_DELAY_MS, [](Shell & shell) {
                                shell.logger().log(LogLevel::NOTICE,
                                                   LogFacility::AUTH,
                                                   F("Invalid admin password on console %s"),
                                                   to_shell(shell).console_name().c_str());
                                shell.println(F("su: incorrect password"));
                            });
                        }
                    });
                }
            });
        }
    });

    commands->add_command(ShellContext::MAIN, CommandFlags::ADMIN, {F_(passwd)}, [](Shell & shell, const std::vector<std::string> & arguments) {
        shell.enter_password(F_(new_password_prompt1), [](Shell & shell, bool completed, const std::string & password1) {
            if (completed) {
                shell.enter_password(F_(new_password_prompt2), [password1](Shell & shell, bool completed, const std::string & password2) {
                    if (completed) {
                        if (password1 == password2) {
                            EMSESP::esp32React.getSecuritySettingsService()->update([&](SecuritySettings & securitySettings) {
                                securitySettings.jwtSecret = password2.c_str();
                                return StateUpdateResult::CHANGED;
                            });
                            shell.println("Admin password updated");
                        } else {
                            shell.println("Passwords do not match");
                        }
                    }
                });
            }
        });
    });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::ADMIN,
                          {F_(restart)},
                          {F_(partitionname_optional)},
                          [](Shell & shell, const std::vector<std::string> & arguments) {
                              if (arguments.size()) {
                                  EMSESP::system_.system_restart(arguments.front().c_str());
                              } else {
                                  EMSESP::system_.system_restart();
                              }
                          });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::ADMIN,
                          string_vector{F_(wifi), F_(reconnect)},
                          [](Shell & shell, const std::vector<std::string> & arguments) { EMSESP::system_.wifi_reconnect(); });

    //
    // SET commands
    //
    commands->add_command(ShellContext::MAIN,
                          CommandFlags::ADMIN,
                          string_vector{F_(set), F_(admin), F_(password)},
                          [](Shell & shell, const std::vector<std::string> & arguments) {
                              shell.enter_password(F_(new_password_prompt1), [](Shell & shell, bool completed, const std::string & password1) {
                                  if (completed) {
                                      shell.enter_password(F_(new_password_prompt2), [password1](Shell & shell, bool completed, const std::string & password2) {
                                          if (completed) {
                                              if (password1 == password2) {
#ifndef EMSESP_STANDALONE
                                                  EMSESP::esp32React.getSecuritySettingsService()->updateWithoutPropagation(
                                                      [&](SecuritySettings & securitySettings) {
                                                          // find the username 'admin' and update its password
                                                          for (auto & user : securitySettings.users) {
                                                              if (user.username == "admin") {
                                                                  user.password = password2.c_str();
                                                                  return StateUpdateResult::CHANGED;
                                                              }
                                                          }
                                                          // otherwise add a new admin user
                                                          securitySettings.users.emplace_back(F_(admin), password2.c_str(), true);
                                                          return StateUpdateResult::CHANGED;
                                                      });
#endif
                                                  shell.println("Admin user's password updated.");
                                              } else {
                                                  shell.println("Passwords do not match");
                                              }
                                          }
                                      });
                                  }
                              });
                          });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::ADMIN,
                          string_vector{F_(set), F_(wifi), F_(password)},
                          [](Shell & shell, const std::vector<std::string> & arguments) {
                              shell.enter_password(F_(new_password_prompt1), [](Shell & shell, bool completed, const std::string & password1) {
                                  if (completed) {
                                      shell.enter_password(F_(new_password_prompt2), [password1](Shell & shell, bool completed, const std::string & password2) {
                                          if (completed) {
                                              if (password1 == password2) {
                                                  EMSESP::esp32React.getNetworkSettingsService()->updateWithoutPropagation([&](NetworkSettings & networkSettings) {
                                                      networkSettings.password = password2.c_str();
                                                      return StateUpdateResult::CHANGED;
                                                  });
                                                  shell.println("WiFi password updated. Reconnecting...");
                                                  EMSESP::system_.wifi_reconnect();
                                              } else {
                                                  shell.println("Passwords do not match");
                                              }
                                          }
                                      });
                                  }
                              });
                          });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::ADMIN,
                          string_vector{F_(set), F_(hostname)},
                          {F_(name_mandatory)},
                          [](Shell & shell, const std::vector<std::string> & arguments) {
                              shell.println("The network connection will be reset...");
                              Shell::loop_all();
                              delay(1000); // wait a second
                              EMSESP::esp32React.getNetworkSettingsService()->update([&](NetworkSettings & networkSettings) {
                                  networkSettings.hostname = arguments.front().c_str();
                                  return StateUpdateResult::CHANGED;
                              });
                          });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::ADMIN,
                          string_vector{F_(set), F_(wifi), F_(ssid)},
                          {F_(name_mandatory)},
                          [](Shell & shell, const std::vector<std::string> & arguments) {
                              EMSESP::esp32React.getNetworkSettingsService()->updateWithoutPropagation([&](NetworkSettings & networkSettings) {
                                  networkSettings.ssid = arguments.front().c_str();
                                  return StateUpdateResult::CHANGED;
                              });
                              shell.println("WiFi ssid updated. Reconnecting...");
                              EMSESP::system_.wifi_reconnect();
                          });


    commands->add_command(
        ShellContext::MAIN,
        CommandFlags::ADMIN,
        string_vector{F_(set), F_(board_profile)},
        {F_(name_mandatory)},
        [](Shell & shell, const std::vector<std::string> & arguments) {
            std::vector<int8_t> data; // led, dallas, rx, tx, button, phy_type, eth_power, eth_phy_addr, eth_clock_mode
            std::string         board_profile = Helpers::toUpper(arguments.front());
            if (!EMSESP::system_.load_board_profile(data, board_profile)) {
                shell.println("Invalid board profile (S32, E32, E32V2, MH-ET, NODEMCU, LOLIN, OLIMEX, OLIMEXPOE, C3MINI, S2MINI, S3MINI, S32S3, CUSTOM)");
                return;
            }

            EMSESP::webSettingsService.update([&](WebSettings & settings) {
                settings.board_profile  = board_profile.c_str();
                settings.led_gpio       = data[0];
                settings.dallas_gpio    = data[1];
                settings.rx_gpio        = data[2];
                settings.tx_gpio        = data[3];
                settings.pbutton_gpio   = data[4];
                settings.phy_type       = data[5];
                settings.eth_power      = data[6]; // can be -1
                settings.eth_phy_addr   = data[7];
                settings.eth_clock_mode = data[8];
                return StateUpdateResult::CHANGED;
            });
            shell.printfln("Loaded board profile %s", board_profile.c_str());
            EMSESP::system_.network_init(true);
        });

    commands->add_command(
        ShellContext::MAIN,
        CommandFlags::ADMIN,
        string_vector{F_(set), F_(bus_id)},
        {F_(deviceid_mandatory)},
        [](Shell & shell, const std::vector<std::string> & arguments) {
            uint8_t device_id = Helpers::hextoint(arguments.front().c_str());
            if ((device_id == 0x0B) || (device_id == 0x0D) || (device_id == 0x0A) || (device_id == 0x0F) || (device_id == 0x12)) {
                EMSESP::webSettingsService.update([&](WebSettings & settings) {
                    settings.ems_bus_id = device_id;
                    shell.printfln(F_(bus_id_fmt), settings.ems_bus_id);
                    return StateUpdateResult::CHANGED;
                });
            } else {
                shell.println("Must be 0B, 0D, 0A, 0E, 0F, or 48 - 4D");
            }
        },
        [](Shell & shell, const std::vector<std::string> & current_arguments, const std::string & next_argument) -> std::vector<std::string> {
            return std::vector<std::string>{"0B", "0D", "0A", "0E", "0F", "48", "49", "4A", "4B", "4C", "4D"};
        });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::ADMIN,
                          string_vector{F_(set), F_(tx_mode)},
                          {F_(n_mandatory)},
                          [](Shell & shell, const std::vector<std::string> & arguments) {
                              uint8_t tx_mode = std::strtol(arguments[0].c_str(), nullptr, 10);
                              // save the tx_mode
                              EMSESP::webSettingsService.update([&](WebSettings & settings) {
                                  settings.tx_mode = tx_mode;
                                  shell.printfln(F_(tx_mode_fmt), settings.tx_mode);
                                  return StateUpdateResult::CHANGED;
                              });
                              EMSESP::uart_init();
                          });

    //
    // EMS device commands
    //

    commands->add_command(ShellContext::MAIN, CommandFlags::ADMIN, {F_(scan)}, {F_(deep_optional)}, [](Shell & shell, const std::vector<std::string> & arguments) {
        if (arguments.empty()) {
            EMSESP::scan_devices();
        } else {
            shell.printfln("Performing a deep scan...");
            EMSESP::clear_all_devices();
            // device IDs taken from device_library.h
            // send the read command with Version command
            const std::vector<uint8_t> Device_Ids = {0x02, 0x08, 0x09, 0x10, 0x11, 0x12, 0x15, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x20, 0x21, 0x22, 0x23,
                                                     0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x30, 0x38, 0x40, 0x41, 0x48, 0x50, 0x51, 0x60};
            for (const uint8_t device_id : Device_Ids) {
                EMSESP::send_read_request(EMSdevice::EMS_TYPE_VERSION, device_id);
            }
        }
    });

    // read <deviceID> <type ID> [offset] [length]
    commands->add_command(ShellContext::MAIN,
                          CommandFlags::USER,
                          {F_(read)},
                          string_vector{F_(deviceid_mandatory), F_(typeid_mandatory), F_(offset_optional), F_(length_optional)},
                          [=](Shell & shell, const std::vector<std::string> & arguments) {
                              // loop through arguments and add to data as text, separated by a space
                              std::string data;
                              for (const auto & arg : arguments) {
                                  if (!data.empty()) {
                                      data += " ";
                                  }
                                  data += arg;
                              }

                              if (!System::readCommand(data.c_str())) {
                                  shell.printfln("Invalid deviceID");
                                  return;
                              }
                          });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::USER,
                          {F_(watch)},
                          string_vector{F_(watch_format_optional), F_(watchid_optional)},
                          [](Shell & shell, const std::vector<std::string> & arguments) {
                              uint16_t watch_id = WATCH_ID_NONE;

                              // only use english commands, not the translations
                              if (!arguments.empty()) {
                                  // get raw/pretty
                                  if (arguments[0] == F_(raw)) {
                                      EMSESP::watch(EMSESP::WATCH_RAW); // raw
                                  } else if (arguments[0] == (FL_(on)[0])) {
                                      EMSESP::watch(EMSESP::WATCH_ON); // on
                                  } else if (arguments[0] == (FL_(off)[0])) {
                                      EMSESP::watch(EMSESP::WATCH_OFF); // off
                                  } else if (arguments[0] == (FL_(unknown)[0])) {
                                      EMSESP::watch(EMSESP::WATCH_UNKNOWN); // unknown
                                      watch_id = WATCH_ID_NONE;
                                  } else {
                                      watch_id = Helpers::hextoint(arguments[0].c_str());
                                      if (watch_id > 0 && ((EMSESP::watch() == EMSESP::WATCH_OFF) || (EMSESP::watch() == EMSESP::WATCH_UNKNOWN))) {
                                          EMSESP::watch(EMSESP::WATCH_ON); // on
                                      } else if (watch_id == 0) {
                                          EMSESP::watch(EMSESP::WATCH_OFF); // off
                                          return;
                                      }
                                  }

                                  if (arguments.size() == 2) {
                                      // get the watch_id if its set
                                      watch_id = Helpers::hextoint(arguments[1].c_str());
                                  }

                                  EMSESP::watch_id(watch_id);
                              } else {
                                  shell.printfln("Invalid: use watch raw|on|off|unknown|id [id]");
                                  return;
                              }

                              uint8_t watch = EMSESP::watch();
                              if (watch == EMSESP::WATCH_OFF) {
                                  shell.printfln("Watching telegrams is off");
                                  return;
                              }

                              // if logging is off, the watch won't show anything, show force it back to NOTICE
                              if (shell.log_level() < Level::NOTICE) {
                                  shell.log_level(Level::NOTICE);
                                  shell.printfln("Setting log level to Notice");
                              }

                              if (watch == EMSESP::WATCH_ON) {
                                  shell.printfln("Watching incoming telegrams, displayed in decoded format");
                              } else if (watch == EMSESP::WATCH_RAW) {
                                  shell.printfln("Watching incoming telegrams, displayed as raw bytes"); // WATCH_RAW
                              } else {
                                  shell.printfln("Watching unknown telegrams"); // WATCH_UNKNOWN
                              }

                              watch_id = EMSESP::watch_id();
                              if (watch_id > 0x80) {
                                  shell.printfln("Filtering only telegrams that match a telegram type of 0x%02X", watch_id);
                              } else if (watch_id != WATCH_ID_NONE) {
                                  shell.printfln("Filtering only telegrams that match a deviceID or telegram type of 0x%02X", watch_id);
                              }
                          });

    commands->add_command(
        ShellContext::MAIN,
        CommandFlags::ADMIN,
        {F_(call)},
        string_vector{F_(device_type_optional), F_(cmd_optional), F_(data_optional), F_(id_optional)},
        [&](Shell & shell, const std::vector<std::string> & arguments) {
            if (arguments.empty()) {
                Command::show_all(shell); // list options
                return;
            }

            // validate the device_type
            uint8_t device_type = EMSdevice::device_name_2_device_type(arguments[0].c_str());
            if (!Command::device_has_commands(device_type)) {
                shell.print("Invalid device. Available devices are: ");
                Command::show_devices(shell);
                return;
            }

            // validate that a command is present
            if (arguments.size() < 2) {
                shell.print("Missing command. Available commands are: ");
                Command::show(shell, device_type, false); // non-verbose mode
                return;
            }

            JsonDocument doc;
            int8_t       id          = -1;
            auto         arg         = Helpers::toLower(arguments[1]);
            const char * cmd         = arg.c_str(); // prevent loosing pointer after object destruction
            uint8_t      return_code = CommandRet::OK;
            JsonObject   json        = doc.to<JsonObject>();
            bool         has_data    = false;

            if (device_type >= EMSdevice::DeviceType::BOILER) {
                cmd = Command::parse_command_string(cmd, id); // extract hc or dhw
            }
            if (cmd == nullptr) {
                cmd = F_(values);
            }

            if (arguments.size() == 2) {
                // no value specified, just the cmd
                return_code = Command::call(device_type, cmd, nullptr, true, id, json);
            } else if (arguments.size() == 3) {
                if (!strcmp(cmd, F_(info)) || !strcmp(cmd, F_(values))) {
                    // info has a id but no value
                    return_code = Command::call(device_type, cmd, nullptr, true, atoi(arguments.back().c_str()), json);
                } else if (arguments[2] == "?") {
                    return_code = Command::call(device_type, cmd, nullptr, true, id, json);
                } else {
                    has_data = true;
                    // has a value but no id so use -1
                    return_code = Command::call(device_type, cmd, arguments.back().c_str(), true, id, json);
                }
            } else {
                // use value, which could be an id or hc
                if (arguments[2] == "?") {
                    return_code = Command::call(device_type, cmd, nullptr, true, atoi(arguments[3].c_str()), json);
                } else {
                    has_data    = true;
                    return_code = Command::call(device_type, cmd, arguments[2].c_str(), true, atoi(arguments[3].c_str()), json);
                }
            }

            if (return_code == CommandRet::OK) {
                if (json.size()) {
                    if (json["api_data"].is<std::string>()) {
                        std::string data = json["api_data"];
                        shell.println(data.c_str());
                        return;
                    }
                    serializeJsonPretty(doc, shell);
                    shell.println();
                    return;
                } else if (!has_data) {
                    // show message if no data returned (e.g. for analogsensor, temperaturesensor, custom)
                    shell.println("Command executed");
                    return;
                } else {
                    return;
                }
            } else if (return_code == CommandRet::NOT_FOUND) {
                shell.println("Unknown command");
                shell.print("Available commands are: ");
                Command::show(shell, device_type, false); // non-verbose mode
            } else if ((return_code == CommandRet::ERROR) || (return_code == CommandRet::FAIL)) {
                shell.printfln("Bad syntax. Check arguments.");
            } else {
                shell.printfln("Command failed with error code %d", return_code);
            }
        },
        [](Shell & shell, const std::vector<std::string> & current_arguments, const std::string & next_argument) -> std::vector<std::string> {
            if (current_arguments.empty()) {
                std::vector<std::string> devices_list;
                devices_list.emplace_back(EMSdevice::device_type_2_device_name(EMSdevice::DeviceType::SYSTEM));
                devices_list.emplace_back(EMSdevice::device_type_2_device_name(EMSdevice::DeviceType::TEMPERATURESENSOR));
                devices_list.emplace_back(EMSdevice::device_type_2_device_name(EMSdevice::DeviceType::ANALOGSENSOR));
                for (const auto & device_class : EMSFactory::device_handlers()) {
                    if (Command::device_has_commands(device_class.first)) {
                        devices_list.emplace_back(EMSdevice::device_type_2_device_name(device_class.first));
                    }
                }
                return devices_list;
            } else if (current_arguments.size() == 1) {
                uint8_t device_type = EMSdevice::device_name_2_device_type(current_arguments[0].c_str());
                if (Command::device_has_commands(device_type)) {
                    std::vector<std::string> command_list;
                    for (const auto & cf : Command::commands()) {
                        if (cf.device_type_ == device_type) {
                            command_list.emplace_back(cf.cmd_);
                        }
                    }
                    return command_list;
                }
            }

            return {};
        });
}

std::shared_ptr<Commands> EMSESPShell::commands_ = [] {
    auto commands = std::make_shared<Commands>();
    setup_commands(commands);
    return commands;
}();

EMSESPShell::EMSESPShell(EMSESP & emsesp, Stream & stream, unsigned int context, unsigned int flags)
    : Shell(stream, commands_, context, flags)
    , emsesp_(emsesp) {
}

void EMSESPShell::started() {
    logger().log(LogLevel::INFO, LogFacility::CONSOLE, F("User session opened on console %s"), console_name().c_str());
}

void EMSESPShell::stopped() {
    if (has_flags(CommandFlags::ADMIN)) {
        logger().log(LogLevel::INFO, LogFacility::AUTH, F("Admin session closed on console %s"), console_name().c_str());
    }
    logger().log(LogLevel::INFO, LogFacility::CONSOLE, F("User session closed on console %s"), console_name().c_str());
}

// show welcome banner
void EMSESPShell::display_banner() {
    println();
    printfln("┌───────────────────────────────────────┐");
    printfln("│  %sEMS-ESP version %-20s%s │", COLOR_BOLD_ON, EMSESP_APP_VERSION, COLOR_BOLD_OFF);
    printfln("│                                       │");
    printfln("│  %shelp%s to show available commands      │", COLOR_UNDERLINE, COLOR_RESET);
    printfln("│  %ssu%s to access admin commands          │", COLOR_UNDERLINE, COLOR_RESET);
    printfln("│                                       │");
    printfln("│  %s%shttps://github.com/emsesp/EMS-ESP32%s  │", COLOR_BRIGHT_GREEN, COLOR_UNDERLINE, COLOR_RESET);
    printfln("│                                       │");
    printfln("└───────────────────────────────────────┘");
    println();

    // set console name
    EMSESP::esp32React.getNetworkSettingsService()->read([&](NetworkSettings & networkSettings) { console_hostname_ = networkSettings.hostname.c_str(); });
    if (console_hostname_.empty()) {
        console_hostname_ = "ems-esp";
    }
}

std::string EMSESPShell::hostname_text() {
    return console_hostname_;
}

std::string EMSESPShell::context_text() {
    return std::string{};

    // auto shell_context = static_cast<ShellContext>(context());
    // if (shell_context == ShellContext::MAIN) {
    //     return std::string{'/'};
    // } else if (shell_context == ShellContext::FILESYSTEM) {
    //         return "/fs");
    // } else {
    //         return std::string{};
    // }
}

// when in su (admin) show # as the prompt suffix
std::string EMSESPShell::prompt_suffix() {
#ifndef EMSESP_UNITY
    if (has_flags(CommandFlags::ADMIN)) {
        return std::string{'#'};
    } else {
        return std::string{'$'};
    }
#else
    // don't bother with prompt suffix if we're testing Unity output
    return "";
#endif
}

void EMSESPShell::end_of_transmission() {
    invoke_command(F_(exit));
}

void EMSESPShell::main_help_function(Shell & shell, const std::vector<std::string> & arguments) {
    shell.println();
#if defined(EMSESP_DEBUG)
    shell.printfln("%s%sEMS-ESP version %s%s", COLOR_BRIGHT_GREEN, COLOR_BOLD_ON, EMSESP_APP_VERSION, COLOR_RESET);
#else
    shell.printfln("%s%sEMS-ESP version %s%s (DEBUG)", COLOR_BRIGHT_GREEN, COLOR_BOLD_ON, EMSESP_APP_VERSION, COLOR_RESET);
#endif
    shell.println();
    shell.print_all_available_commands();
}

void EMSESPShell::main_exit_function(Shell & shell, const std::vector<std::string> & arguments) {
    shell.stop();
}

//  **** EMSESPConsole Class *****

#ifndef EMSESP_STANDALONE
std::vector<bool> EMSESPConsole::ptys_;
#endif

EMSESPConsole::EMSESPConsole(EMSESP & emsesp, Stream & stream, bool local)
    : EMSESPShell(emsesp, stream, ShellContext::MAIN, local ? (CommandFlags::USER | CommandFlags::LOCAL) : CommandFlags::USER)
    , name_("ttyS0")
#ifndef EMSESP_STANDALONE
    , pty_(std::numeric_limits<size_t>::max())
    , addr_()
    , port_(0)
#endif
{
}

#ifndef EMSESP_STANDALONE
EMSESPConsole::EMSESPConsole(EMSESP & emsesp, Stream & stream, const IPAddress & addr, uint16_t port)
    : EMSESPShell(emsesp, stream, ShellContext::MAIN, CommandFlags::USER)
    , addr_(addr)
    , port_(port) {
    std::array<char, 16> text;

    pty_ = 0;
    while (pty_ < ptys_.size() && ptys_[pty_])
        pty_++;
    if (pty_ == ptys_.size()) {
        ptys_.push_back(true);
    } else {
        ptys_[pty_] = true;
    }

    snprintf(text.data(), text.size(), "pty%u", pty_);
    name_ = text.data();

    logger().info("Allocated console %s for connection from [%s]:%u", name_.c_str(), uuid::printable_to_string(addr_).c_str(), port_);
}
#endif

EMSESPConsole::~EMSESPConsole() {
#ifndef EMSESP_STANDALONE
    if (pty_ != SIZE_MAX) {
        logger().info("Shutdown console %s for connection from [%s]:%u", name_.c_str(), uuid::printable_to_string(addr_).c_str(), port_);

        ptys_[pty_] = false;
        ptys_.shrink_to_fit();
    }
#endif
}

std::string EMSESPConsole::console_name() {
    return name_;
}

} // namespace emsesp
