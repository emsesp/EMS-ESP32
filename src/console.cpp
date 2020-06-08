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

#include "console.h"
#include "emsesp.h"

#ifdef EMSESP_DEBUG
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
        logger().log(LogLevel::INFO, LogFacility::AUTH, F("Admin session closed on console %s"), console_name().c_str());
    }
    logger().log(LogLevel::INFO, LogFacility::CONSOLE, F("User session closed on console %s"), console_name().c_str());

    // remove all custom contexts
    commands->remove_all_commands();
    _console_commands_loaded = false; // make sure they got loaded next time a console is opened
}

// show welcome banner
void EMSESPShell::display_banner() {
    Settings settings;

    println();
    printfln(F("┌──────────────────────────────────────────┐"));
    printfln(F("│ %sEMS-ESP version %-10s%s               │"), COLOR_BOLD_ON, settings.app_version().c_str(), COLOR_BOLD_OFF);
    printfln(F("│ %s%shttps://github.com/proddy/EMS-ESP%s        │"), COLOR_BRIGHT_GREEN, COLOR_UNDERLINE, COLOR_RESET);
    printfln(F("│                                          │"));

    if (System::safe_mode()) {
        printfln(F("│ %sIN SAFE MODE. EMS BUS IS DISABLED.%s       │"), COLOR_BRIGHT_RED_BACKGROUND, COLOR_RESET);
#ifdef EMSESP_SAFE_MODE
        printfln(F("│ %s!FORCED AT COMPILE TIME!%s                 │"), COLOR_BRIGHT_RED, COLOR_RESET);
#endif
        printfln(F("│                                          │"));
    }

    printfln(F("│ type %shelp%s to show available commands     │"), COLOR_UNDERLINE, COLOR_RESET);
    printfln(F("└──────────────────────────────────────────┘"));
    println();

    // load the list of commands
    add_console_commands();
}

// pre-loads all the console commands into the MAIN context
// This is only done after a connection is established, to save on Heap memory
void EMSESPShell::add_console_commands() {
    // if we already have these commands loaded, stop adding duplicates
    // for example when opening multiple serial/telnet sessions
    if (_console_commands_loaded) {
        return;
    }

    // just in case, remove everything
    // commands->remove_context_commands(ShellContext::MAIN);
    commands->remove_all_commands();

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::USER,
                          flash_string_vector{F_(refresh)},
                          [&](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                              shell.printfln(F("Requesting data from EMS devices"));
                              _console_commands_loaded = false;
                              add_console_commands();
                              EMSESP::fetch_device_values();
                          });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::USER,
                          flash_string_vector{F_(show), F_(version)},
                          [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                              shell.printfln(F("%s%sEMS-ESP version %s%s"), COLOR_BRIGHT_GREEN, COLOR_BOLD_ON, Settings().app_version().c_str(), COLOR_RESET);
                              shell.println();
                          });

    commands->add_command(ShellContext::MAIN,
                          CommandFlags::USER,
                          flash_string_vector{F_(show)},
                          [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                              //   shell.printfln(F("%s%sEMS-ESP version %s%s"), COLOR_BRIGHT_GREEN, COLOR_BOLD_ON, Settings().app_version().c_str(), COLOR_RESET);
                              //   shell.println();
                              //   EMSESP::show_emsbus(shell);
                              //   EMSESP::show_devices(shell);
                              EMSESP::show_values(shell);
                          });

    /*
     * add all the submenu contexts...
     */

    // MQTT
    commands->add_command(ShellContext::MAIN,
                          CommandFlags::USER,
                          flash_string_vector{F_(mqtt)},
                          [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                              Mqtt::console_commands(shell, ShellContext::MQTT);
                          });

    // EMS
    commands->add_command(ShellContext::MAIN,
                          CommandFlags::USER,
                          flash_string_vector{F_(ems)},
                          [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                              EMSESP::console_commands(shell, ShellContext::EMS);
                          });

    // System
    commands->add_command(ShellContext::MAIN,
                          CommandFlags::USER,
                          flash_string_vector{F_(system)},
                          [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                              System::console_commands(shell, ShellContext::SYSTEM);
                          });

    // add all the context menus for the connected devices
    // this assumes they devices have been detected and pre-registered
    EMSESP::add_context_menus();

    Console::load_standard_commands(ShellContext::MAIN);

    _console_commands_loaded = true;
}

std::string EMSESPShell::hostname_text() {
    Settings    settings;
    std::string hostname = settings.hostname();

    if (hostname.empty()) {
        hostname.resize(16, '\0');

#if defined(ESP8266)
        snprintf_P(&hostname[0], hostname.capacity() + 1, PSTR("esp8266"));
#else
        snprintf_P(&hostname[0], hostname.capacity() + 1, PSTR("esp32"));
#endif
    }

    /*
    if (System::safe_mode()) {
        return std::string{"safemode@"} + hostname;
    }
    */

    return hostname;
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

    // don't enter context if we're already at the root
    if (context != ShellContext::MAIN) {
        shell.enter_context(context);
    }
}

// each custom context has the common commands like log, help, exit, su etc
void Console::load_standard_commands(unsigned int context) {
#ifdef EMSESP_DEBUG
    EMSESPShell::commands->add_command(context,
                                       CommandFlags::ADMIN,
                                       flash_string_vector{F_(test)},
                                       flash_string_vector{F_(name_mandatory)},
                                       [](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
                                           Test::run_test(shell, arguments.front());
                                       });
#endif

    EMSESPShell::commands->add_command(
        context,
        CommandFlags::USER,
        flash_string_vector{F_(log)},
        flash_string_vector{F_(log_level_optional), F_(trace_format_optional), F_(traceid_optional)},
        [](Shell & shell, const std::vector<std::string> & arguments) {
            uint16_t watch_id;
            if (!arguments.empty()) {
                uuid::log::Level level;

                if (uuid::log::parse_level_lowercase(arguments[0], level)) {
                    shell.log_level(level);
                } else {
                    shell.printfln(F_(invalid_log_level));
                    return;
                }

                // trace logic
                if (level == uuid::log::Level::TRACE || level == uuid::log::Level::DEBUG) {
                    watch_id = LOG_TRACE_WATCH_NONE; // no watch ID set
                    if (arguments.size() > 1) {
                        // next argument is raw or full
                        if (arguments[1] == read_flash_string(F_(raw))) {
                            emsesp::EMSESP::trace_raw(true);
                        } else if (arguments[1] == read_flash_string(F_(pretty))) {
                            emsesp::EMSESP::trace_raw(false);
                        } else {
                            emsesp::EMSESP::trace_watch_id(Helpers::hextoint(arguments[1].c_str()));
                        }

                        // get the watch_id if its set
                        if (arguments.size() == 3) {
                            emsesp::EMSESP::trace_watch_id(Helpers::hextoint(arguments[2].c_str()));
                        }
                    } else {
                        // it was "log trace" so reset the watch id and switch back to pretty
                        emsesp::EMSESP::trace_raw(false);
                        emsesp::EMSESP::trace_watch_id(LOG_TRACE_WATCH_NONE);
                    }
                }
            }

            // print out logging settings
            shell.printfln(F_(log_level_fmt), uuid::log::format_level_uppercase(shell.log_level()));
            watch_id = emsesp::EMSESP::trace_watch_id();
            if (watch_id == LOG_TRACE_WATCH_NONE) {
                shell.printfln(F("Tracing all telegrams"));
            } else {
                shell.printfln(F("Tracing only telegrams that match a device ID or telegram type of 0x%02X"), watch_id);
            }
            shell.printfln(F_(trace_raw_fmt), emsesp::EMSESP::trace_raw() ? F("as raw bytes") : F("in decoded format"));
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
                                               shell.logger().log(LogLevel::NOTICE, LogFacility::AUTH, F("Admin session opened on console"));
                                               shell.add_flags(CommandFlags::ADMIN);
                                           };

                                           if (shell.has_flags(CommandFlags::LOCAL)) {
                                               become_admin(shell);
                                           } else {
                                               shell.enter_password(F_(password_prompt), [=](Shell & shell, bool completed, const std::string & password) {
                                                   if (completed) {
                                                       uint64_t now = uuid::get_uptime_ms();

                                                       if (!password.empty() && password == Settings().admin_password()) {
                                                           become_admin(shell);
                                                       } else {
                                                           shell.delay_until(now + INVALID_PASSWORD_DELAY_MS, [](Shell & shell) {
                                                               shell.logger().log(LogLevel::NOTICE, LogFacility::AUTH, F("Invalid admin password on console"));
                                                               shell.println(F("su: incorrect password"));
                                                           });
                                                       }
                                                   }
                                               });
                                           }
                                       });

#ifdef EMSESP_DEBUG
    EMSESPShell::commands->add_command(
        context, CommandFlags::ADMIN, flash_string_vector{F_(debug)}, [&](Shell & shell, const std::vector<std::string> & arguments __attribute__((unused))) {
            shell.printfln(F("%s%sEMS-ESP version %s%s"), COLOR_BRIGHT_GREEN, COLOR_BOLD_ON, Settings().app_version().c_str(), COLOR_RESET);
            Settings settings;
            settings.commit();
            settings.show_settings(shell);
            shell.println();
        });
#endif
}

// prompt, change per context
std::string EMSESPShell::context_text() {
    switch (static_cast<ShellContext>(context())) {
    case ShellContext::MAIN:
        return std::string{'/'};

    case ShellContext::EMS:
        return std::string{"/ems"};

    case ShellContext::MQTT:
        return std::string{"/mqtt"};

    case ShellContext::BOILER:
        return std::string{"/boiler"};

    case ShellContext::SYSTEM:
        return std::string{"/system"};

    case ShellContext::THERMOSTAT:
        return std::string{"/thermostat"};

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
void Console::start() {
// if we've detected a boot into safe mode on ESP8266, start the Serial console too
// Serial is always on with the ESP32 as it has 2 UARTs
#if defined(ESP32) || defined(EMSESP_STANDALONE)
    if (true) {
#elif defined(ESP8266)
    if (System::safe_mode()) {
#endif
        serial_console_.begin(SERIAL_CONSOLE_BAUD_RATE);
        serial_console_.println();

        shell = std::make_shared<EMSESPStreamConsole>(serial_console_, true);
        shell->maximum_log_messages(100); // default is 50
        shell->start();
        shell->log_level(uuid::log::Level::DEBUG); // order is: err, warning, notice, info, trace, debug, all

#if defined(EMSESP_STANDALONE)
        shell->add_flags(CommandFlags::ADMIN);
#endif
    }

// always start the telnet service, except on an ESP8266
// default idle is 10 minutes, default write timeout is 0 (automatic)
// note, this must be started after the network/wifi for ESP32 otherwise it'll crash
#ifndef EMSESP_STANDALONE
    telnet_.start();
    telnet_.default_write_timeout(1000); // in ms, socket timeout 1 second
#endif
}

// handles telnet sync and logging to console
void Console::loop() {
    uuid::loop();

#ifndef EMSESP_STANDALONE
    telnet_.loop();
#endif

    Shell::loop_all();
}

} // namespace emsesp
