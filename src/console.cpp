/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2023  Paul Derbyshire
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
#include "version.h"

#if defined(EMSESP_TEST)
#include "test/test.h"
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

static inline EMSESP & to_app(Shell & shell) {
    return to_shell(shell).emsesp_;
}

#define NO_ARGUMENTS                                                                                                                                           \
    std::vector<std::string> {}

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

static void setup_commands(std::shared_ptr<Commands> & commands) {
    // log, exit, help
    commands->add_command(ShellContext::MAIN, CommandFlags::USER, {F_(exit)}, EMSESPShell::main_exit_function);
    commands->add_command(ShellContext::MAIN, CommandFlags::USER, {F_(help)}, EMSESPShell::main_help_function);
    commands->add_command(ShellContext::MAIN, CommandFlags::USER, {F_(log)}, {F_(log_level_optional)}, console_log_level, log_level_autocomplete);

    //
    // Show commands
    //
    commands->add_command(ShellContext::MAIN,
                          CommandFlags::USER,
                          string_vector{F_(show), F_(system)},
                          [=](Shell & shell, const std::vector<std::string> & arguments) {
                              shell.println();
                              shell.printfln("%s%sEMS-ESP version %s%s", COLOR_BRIGHT_GREEN, COLOR_BOLD_ON, EMSESP_APP_VERSION, COLOR_RESET);
                              shell.println();
                              to_app(shell).system_.show_system(shell);
                              shell.println();
                          });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::ADMIN,
                          string_vector{F_(show), F_(users)},
                          [](Shell & shell, const std::vector<std::string> & arguments) { to_app(shell).system_.show_users(shell); });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::USER,
                          string_vector{F_(show), F_(devices)},
                          [](Shell & shell, const std::vector<std::string> & arguments) { to_app(shell).show_devices(shell); });


    commands->add_command(ShellContext::MAIN, CommandFlags::USER, string_vector{F_(show), F_(ems)}, [](Shell & shell, const std::vector<std::string> & arguments) {
        to_app(shell).show_ems(shell);
    });

    commands->add_command(ShellContext::MAIN, CommandFlags::USER, string_vector{F_(show), F_(values)}, [](Shell & shell, const std::vector<std::string> & arguments) {
        to_app(shell).show_device_values(shell);
        to_app(shell).show_sensor_values(shell);
    });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::USER,
                          string_vector{F_(show), F_(mqtt)},
                          [](Shell & shell, const std::vector<std::string> & arguments) { Mqtt::show_mqtt(shell); });


    commands->add_command(ShellContext::MAIN,
                          CommandFlags::USER,
                          string_vector{F_(show), F_(commands)},
                          [](Shell & shell, const std::vector<std::string> & arguments) { Command::show_all(shell); });


    //
    // System commands
    //

#if defined(EMSESP_TEST)
    // create commands test
    commands->add_command(ShellContext::MAIN,
                          CommandFlags::USER,
                          string_vector{"test"},
                          string_vector{F_(name_optional), F_(data_optional)},
                          [=](Shell & shell, const std::vector<std::string> & arguments) {
                              if (arguments.empty()) {
                                  Test::run_test(shell, "default");
                              } else if (arguments.size() == 1) {
                                  Test::run_test(shell, arguments.front());
                              } else {
                                  Test::run_test(shell, arguments[0].c_str(), arguments[1].c_str());
                              }
                          });
#endif

    commands->add_command(ShellContext::MAIN, CommandFlags::USER, string_vector{F_(su)}, [=](Shell & shell, const std::vector<std::string> & arguments) {
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

                    to_app(shell).esp8266React.getSecuritySettingsService()->read([&](SecuritySettings & securitySettings) {
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

    commands->add_command(ShellContext::MAIN, CommandFlags::ADMIN, string_vector{F_(passwd)}, [](Shell & shell, const std::vector<std::string> & arguments) {
        shell.enter_password(F_(new_password_prompt1), [](Shell & shell, bool completed, const std::string & password1) {
            if (completed) {
                shell.enter_password(F_(new_password_prompt2), [password1](Shell & shell, bool completed, const std::string & password2) {
                    if (completed) {
                        if (password1 == password2) {
                            to_app(shell).esp8266React.getSecuritySettingsService()->update(
                                [&](SecuritySettings & securitySettings) {
                                    securitySettings.jwtSecret = password2.c_str();
                                    return StateUpdateResult::CHANGED;
                                },
                                "local");
                            shell.println("Admin password updated");
                        } else {
                            shell.println("Passwords do not match");
                        }
                    }
                });
            }
        });
    });

    commands->add_command(ShellContext::MAIN, CommandFlags::ADMIN, string_vector{F_(restart)}, [](Shell & shell, const std::vector<std::string> & arguments) {
        to_app(shell).system_.system_restart();
    });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::ADMIN,
                          string_vector{F_(wifi), F_(reconnect)},
                          [](Shell & shell, const std::vector<std::string> & arguments) { to_app(shell).system_.wifi_reconnect(); });

    commands->add_command(ShellContext::MAIN, CommandFlags::ADMIN, string_vector{F_(format)}, [](Shell & shell, const std::vector<std::string> & arguments) {
        shell.enter_password(F_(password_prompt), [=](Shell & shell, bool completed, const std::string & password) {
            if (completed) {
                to_app(shell).esp8266React.getSecuritySettingsService()->read([&](SecuritySettings & securitySettings) {
                    if (securitySettings.jwtSecret.equals(password.c_str())) {
                        to_app(shell).system_.format(shell);
                    } else {
                        shell.println("incorrect password");
                    }
                });
            }
        });
    });

    //
    // SET commands
    //
    commands->add_command(ShellContext::MAIN,
                          CommandFlags::ADMIN,
                          string_vector{F_(set), F_(wifi), F_(password)},
                          [](Shell & shell, const std::vector<std::string> & arguments) {
                              shell.enter_password(F_(new_password_prompt1), [](Shell & shell, bool completed, const std::string & password1) {
                                  if (completed) {
                                      shell.enter_password(F_(new_password_prompt2), [password1](Shell & shell, bool completed, const std::string & password2) {
                                          if (completed) {
                                              if (password1 == password2) {
                                                  to_app(shell).esp8266React.getNetworkSettingsService()->updateWithoutPropagation(
                                                      [&](NetworkSettings & networkSettings) {
                                                          networkSettings.password = password2.c_str();
                                                          return StateUpdateResult::CHANGED;
                                                      });
                                                  shell.println("Use `wifi reconnect` to save and apply the new settings");
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
                          string_vector{F_(name_mandatory)},
                          [](Shell & shell, const std::vector<std::string> & arguments) {
                              shell.println("The network connection will be reset...");
                              Shell::loop_all();
                              delay(1000); // wait a second
                              to_app(shell).esp8266React.getNetworkSettingsService()->update(
                                  [&](NetworkSettings & networkSettings) {
                                      networkSettings.hostname = arguments.front().c_str();
                                      return StateUpdateResult::CHANGED;
                                  },
                                  "local");
                          });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::ADMIN,
                          string_vector{F_(set), F_(wifi), F_(ssid)},
                          string_vector{F_(name_mandatory)},
                          [](Shell & shell, const std::vector<std::string> & arguments) {
                              to_app(shell).esp8266React.getNetworkSettingsService()->updateWithoutPropagation([&](NetworkSettings & networkSettings) {
                                  networkSettings.ssid = arguments.front().c_str();
                                  return StateUpdateResult::CHANGED;
                              });
                              shell.println("Use `wifi reconnect` to save and apply the new settings");
                          });


    commands->add_command(ShellContext::MAIN,
                          CommandFlags::ADMIN,
                          string_vector{F_(set), F_(board_profile)},
                          string_vector{F_(name_mandatory)},
                          [](Shell & shell, const std::vector<std::string> & arguments) {
                              std::vector<int8_t> data; // led, dallas, rx, tx, button, phy_type, eth_power, eth_phy_addr, eth_clock_mode
                              std::string         board_profile = Helpers::toUpper(arguments.front());
                              if (!to_app(shell).system_.load_board_profile(data, board_profile)) {
                                  shell.println("Invalid board profile (S32, E32, MH-ET, NODEMCU, OLIMEX, OLIMEXPOE, C3MINI, S2MINI, S3MINI, CUSTOM)");
                                  return;
                              }
                              to_app(shell).webSettingsService.update(
                                  [&](WebSettings & settings) {
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
                                  },
                                  "local");
                              shell.printfln("Loaded board profile %s", board_profile.c_str());
                              to_app(shell).system_.network_init(true);
                          });

    commands->add_command(
        ShellContext::MAIN,
        CommandFlags::ADMIN,
        string_vector{F_(set), F_(bus_id)},
        string_vector{F_(deviceid_mandatory)},
        [](Shell & shell, const std::vector<std::string> & arguments) {
            uint8_t device_id = Helpers::hextoint(arguments.front().c_str());
            if ((device_id == 0x0B) || (device_id == 0x0D) || (device_id == 0x0A) || (device_id == 0x0F) || (device_id == 0x12)) {
                to_app(shell).webSettingsService.update(
                    [&](WebSettings & settings) {
                        settings.ems_bus_id = device_id;
                        shell.printfln(F_(bus_id_fmt), settings.ems_bus_id);
                        return StateUpdateResult::CHANGED;
                    },
                    "local");
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
                          string_vector{F_(n_mandatory)},
                          [](Shell & shell, const std::vector<std::string> & arguments) {
                              uint8_t tx_mode = std::strtol(arguments[0].c_str(), nullptr, 10);
                              // save the tx_mode
                              to_app(shell).webSettingsService.update(
                                  [&](WebSettings & settings) {
                                      settings.tx_mode = tx_mode;
                                      shell.printfln(F_(tx_mode_fmt), settings.tx_mode);
                                      return StateUpdateResult::CHANGED;
                                  },
                                  "local");
                          });

    commands->add_command(ShellContext::MAIN, CommandFlags::USER, string_vector{F_(set)}, [](Shell & shell, const std::vector<std::string> & arguments) {
        to_app(shell).webSettingsService.read([&](WebSettings & settings) {
            shell.printfln("Language: %s", settings.locale.c_str());
            shell.printfln(F_(tx_mode_fmt), settings.tx_mode);
            shell.printfln(F_(bus_id_fmt), settings.ems_bus_id);
            shell.printfln(F_(board_profile_fmt), settings.board_profile.c_str());
        });
    });

    //
    // EMS device commands
    //

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::ADMIN,
                          string_vector{F_(scan), F_(devices)},
                          string_vector{F_(deep_optional)},
                          [](Shell & shell, const std::vector<std::string> & arguments) {
                              if (arguments.size() == 0) {
                                  to_app(shell).scan_devices();
                              } else {
                                  shell.printfln("Performing a deep scan...");
                                  to_app(shell).clear_all_devices();
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
                                      to_app(shell).send_read_request(EMSdevice::EMS_TYPE_VERSION, device_id);
                                  }
                              }
                          });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::USER,
                          string_vector{F_(read)},
                          string_vector{F_(deviceid_mandatory), F_(typeid_mandatory), F_(offset_optional), F_(length_optional)},
                          [=](Shell & shell, const std::vector<std::string> & arguments) {
                              uint8_t device_id = Helpers::hextoint(arguments.front().c_str());

                              if (!to_app(shell).valid_device(device_id)) {
                                  shell.printfln("Invalid deviceID");
                                  return;
                              }

                              uint16_t type_id = Helpers::hextoint(arguments[1].c_str());
                              if (arguments.size() == 4) {
                                  uint16_t offset = Helpers::hextoint(arguments[2].c_str());
                                  uint8_t  length = Helpers::hextoint(arguments.back().c_str());
                                  to_app(shell).send_read_request(type_id, device_id, offset, length);
                              } else if (arguments.size() == 3) {
                                  uint16_t offset = Helpers::hextoint(arguments.back().c_str());
                                  to_app(shell).send_read_request(type_id, device_id, offset, EMS_MAX_TELEGRAM_LENGTH);
                              } else {
                                  // send with length to send immediately and trigger publish read_id
                                  to_app(shell).send_read_request(type_id, device_id, 0, EMS_MAX_TELEGRAM_LENGTH);
                              }
                          });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::USER,
                          string_vector{F_(watch)},
                          string_vector{F_(watch_format_optional), F_(watchid_optional)},
                          [](Shell & shell, const std::vector<std::string> & arguments) {
                              uint16_t watch_id = WATCH_ID_NONE;

                              // only use english commands, not the translations
                              if (!arguments.empty()) {
                                  // get raw/pretty
                                  if (arguments[0] == (F_(raw))) {
                                      to_app(shell).watch(to_app(shell).WATCH_RAW); // raw
                                  } else if (arguments[0] == (FL_(on)[0])) {
                                      to_app(shell).watch(to_app(shell).WATCH_ON); // on
                                  } else if (arguments[0] == (FL_(off)[0])) {
                                      to_app(shell).watch(to_app(shell).WATCH_OFF); // off
                                  } else if (arguments[0] == (FL_(unknown)[0])) {
                                      to_app(shell).watch(to_app(shell).WATCH_UNKNOWN); // unknown
                                      watch_id = WATCH_ID_NONE;
                                  } else {
                                      watch_id = Helpers::hextoint(arguments[0].c_str());
                                      if (watch_id > 0
                                          && ((to_app(shell).watch() == to_app(shell).WATCH_OFF) || (to_app(shell).watch() == to_app(shell).WATCH_UNKNOWN))) {
                                          to_app(shell).watch(to_app(shell).WATCH_ON); // on
                                      } else if (watch_id == 0) {
                                          to_app(shell).watch(to_app(shell).WATCH_OFF); // off
                                          return;
                                      }
                                  }

                                  if (arguments.size() == 2) {
                                      // get the watch_id if its set
                                      watch_id = Helpers::hextoint(arguments[1].c_str());
                                  }

                                  to_app(shell).watch_id(watch_id);
                              } else {
                                  shell.printfln("Invalid: use watch raw|on|off|unknown|id [id]");
                                  return;
                              }

                              uint8_t watch = to_app(shell).watch();
                              if (watch == to_app(shell).WATCH_OFF) {
                                  shell.printfln("Watching telegrams is off");
                                  return;
                              }

                              // if logging is off, the watch won't show anything, show force it back to NOTICE
                              if (shell.log_level() < Level::NOTICE) {
                                  shell.log_level(Level::NOTICE);
                                  shell.printfln("Setting log level to Notice");
                              }

                              if (watch == to_app(shell).WATCH_ON) {
                                  shell.printfln("Watching incoming telegrams, displayed in decoded format");
                              } else if (watch == to_app(shell).WATCH_RAW) {
                                  shell.printfln("Watching incoming telegrams, displayed as raw bytes"); // WATCH_RAW
                              } else {
                                  shell.printfln("Watching unknown telegrams"); // WATCH_UNKNOWN
                              }

                              watch_id = to_app(shell).watch_id();
                              if (watch_id > 0x80) {
                                  shell.printfln("Filtering only telegrams that match a telegram type of 0x%02X", watch_id);
                              } else if (watch_id != WATCH_ID_NONE) {
                                  shell.printfln("Filtering only telegrams that match a deviceID or telegram type of 0x%02X", watch_id);
                              }
                          });

    commands->add_command(
        ShellContext::MAIN,
        CommandFlags::ADMIN,
        string_vector{F_(call)},
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

            DynamicJsonDocument doc(EMSESP_JSON_SIZE_XXXLARGE);
            int8_t              id          = -1;
            const char *        cmd         = Command::parse_command_string(arguments[1].c_str(), id);
            uint8_t             return_code = CommandRet::OK;
            JsonObject          json        = doc.to<JsonObject>();

            if (cmd == nullptr) {
                cmd = device_type == EMSdevice::DeviceType::SYSTEM ? F_(info) : F_(values);
            }

            if (arguments.size() == 2) {
                // no value specified, just the cmd
                return_code = Command::call(device_type, cmd, nullptr, true, id, json);
            } else if (arguments.size() == 3) {
                if ((strncmp(cmd, F_(info), 4) == 0) || strncmp(cmd, F_(values), 6) == 0) {
                    // info has a id but no value
                    return_code = Command::call(device_type, cmd, nullptr, true, atoi(arguments.back().c_str()), json);
                } else if (arguments[2] == "?") {
                    return_code = Command::call(device_type, cmd, nullptr, true, id, json);
                } else {
                    // has a value but no id so use -1
                    return_code = Command::call(device_type, cmd, arguments.back().c_str(), true, id, json);
                }
            } else {
                // use value, which could be an id or hc
                if (arguments[2] == "?") {
                    return_code = Command::call(device_type, cmd, nullptr, true, atoi(arguments[3].c_str()), json);
                } else {
                    return_code = Command::call(device_type, cmd, arguments[2].c_str(), true, atoi(arguments[3].c_str()), json);
                }
            }

            if (return_code == CommandRet::OK && json.size()) {
                if (json.containsKey("api_data")) {
                    JsonVariant data = json["api_data"];
                    shell.println(data.as<const char *>());
                    return;
                }
                serializeJsonPretty(doc, shell);
                shell.println();
                return;
            }

            if (return_code == CommandRet::NOT_FOUND) {
                shell.println("Unknown command");
                shell.print("Available commands are: ");
                Command::show(shell, device_type, false); // non-verbose mode
            } else if (return_code != CommandRet::OK) {
                shell.printfln("Bad syntax (error code %d)", return_code);
            }
        },
        [](Shell & shell, const std::vector<std::string> & current_arguments, const std::string & next_argument) -> std::vector<std::string> {
            if (current_arguments.size() == 0) {
                std::vector<std::string> devices_list;
                devices_list.emplace_back(EMSdevice::device_type_2_device_name(EMSdevice::DeviceType::SYSTEM));
                devices_list.emplace_back(EMSdevice::device_type_2_device_name(EMSdevice::DeviceType::DALLASSENSOR));
                devices_list.emplace_back(EMSdevice::device_type_2_device_name(EMSdevice::DeviceType::ANALOGSENSOR));
                for (const auto & device_class : EMSFactory::device_handlers()) {
                    if (Command::device_has_commands(device_class.first)) {
                        devices_list.emplace_back(EMSdevice::device_type_2_device_name(device_class.first));
                    }
                }
                return devices_list;
            } else if (current_arguments.size() == 1) {
                std::vector<std::string> command_list;
                uint8_t                  device_type = EMSdevice::device_name_2_device_type(current_arguments[0].c_str());
                if (Command::device_has_commands(device_type)) {
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
    std::shared_ptr<Commands> commands = std::make_shared<Commands>();
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
    printfln("┌──────────────────────────────────────┐");
    printfln("│ %sEMS-ESP version %-12s%s         │", COLOR_BOLD_ON, EMSESP_APP_VERSION, COLOR_BOLD_OFF);
    printfln("│ %s%shttps://github.com/emsesp/EMS-ESP32%s  │", COLOR_BRIGHT_GREEN, COLOR_UNDERLINE, COLOR_RESET);
    printfln("│                                      │");
    printfln("│ type %shelp%s to show available commands │", COLOR_UNDERLINE, COLOR_RESET);
    printfln("└──────────────────────────────────────┘");
    println();

    // set console name
    EMSESP::esp8266React.getNetworkSettingsService()->read([&](NetworkSettings & networkSettings) { console_hostname_ = networkSettings.hostname.c_str(); });
    if (console_hostname_.empty()) {
        console_hostname_ = "ems-esp";
    }
}

std::string EMSESPShell::hostname_text() {
    return console_hostname_;
}

std::string EMSESPShell::context_text() {
    auto shell_context = static_cast<ShellContext>(context());

    if (shell_context == ShellContext::MAIN) {
        return std::string{};
        // return std::string{'/'};
        // } else if (shell_context == ShellContext::FILESYSTEM) {
        // 	return "/fs");
    } else {
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
    invoke_command(F_(exit));
}

void EMSESPShell::main_help_function(Shell & shell, const std::vector<std::string> & arguments) {
    shell.print_all_available_commands();
}

void EMSESPShell::main_exit_function(Shell & shell, const std::vector<std::string> & arguments) {
    shell.stop();
}

//  **** EMSESPConsole *****

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
