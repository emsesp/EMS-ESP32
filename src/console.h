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

#ifndef EMSESP_CONSOLE_H
#define EMSESP_CONSOLE_H

#include <Arduino.h>

#include <string>
#include <vector>

#include <uuid/console.h>
#include <uuid/log.h>

#include "helpers.h"
#include "system.h"
#include "mqtt.h"

using uuid::flash_string_vector;
using uuid::read_flash_string;
using uuid::console::Commands;
using uuid::console::Shell;
using uuid::log::Level;

#define LOG_DEBUG(...) logger_.debug(__VA_ARGS__)
#define LOG_INFO(...) logger_.info(__VA_ARGS__)
#define LOG_TRACE(...) logger_.trace(__VA_ARGS__)
#define LOG_NOTICE(...) logger_.notice(__VA_ARGS__)
#define LOG_WARNING(...) logger_.warning(__VA_ARGS__)
#define LOG_ERROR(...) logger_.err(__VA_ARGS__)

// clang-format off
// strings stored 32 bit aligned on ESP8266/ESP32
#define MAKE_PSTR(string_name, string_literal) static const char __pstr__##string_name[] __attribute__((__aligned__(sizeof(uint32_t)))) PROGMEM = string_literal;
#define MAKE_PSTR_WORD(string_name) MAKE_PSTR(string_name, #string_name)
#define F_(string_name) FPSTR(__pstr__##string_name)
#define MAKE_PSTR_LIST(list_name, ...) static const __FlashStringHelper * const __pstr__##list_name[] PROGMEM = {__VA_ARGS__, nullptr};
#define FL_(list_name) (__pstr__##list_name)
// clang-format on

// localizations
#include "locale_EN.h"

#ifdef LOCAL
#undef LOCAL
#endif

static constexpr uint32_t INVALID_PASSWORD_DELAY_MS = 2000;

namespace emsesp {

using LogLevel    = ::uuid::log::Level;
using LogFacility = ::uuid::log::Facility;

enum CommandFlags : uint8_t {

    USER  = 0,
    ADMIN = (1 << 0),
    LOCAL = (1 << 1)

};

enum ShellContext : uint8_t {

    MAIN = 0,
    SYSTEM,

};

class EMSESPShell : virtual public uuid::console::Shell {
  public:
    ~EMSESPShell() override = default;

    virtual std::string console_name() = 0;

    static std::shared_ptr<uuid::console::Commands> commands;
    static std::shared_ptr<EMSESPShell>             shell;

  protected:
    EMSESPShell();

    // our custom functions for Shell
    void        started() override;
    void        stopped() override;
    void        display_banner() override;
    std::string hostname_text() override;
    std::string context_text() override;
    std::string prompt_suffix() override;
    void        end_of_transmission() override;
    bool        exit_context() override;

  private:
    void        add_console_commands();
    bool        console_commands_loaded_ = false; // set to true when the initial commands are loaded
    std::string console_hostname_;
};

class EMSESPStreamConsole : public uuid::console::StreamConsole, public EMSESPShell {
  public:
    EMSESPStreamConsole(Stream & stream, bool local);
    EMSESPStreamConsole(Stream & stream, const IPAddress & addr, uint16_t port);
    ~EMSESPStreamConsole() override;

    std::string console_name() override;

  private:
    static std::vector<bool> ptys_;

    std::string name_;
    size_t      pty_;
    IPAddress   addr_;
    uint16_t    port_;
};

class Console {
  public:
    void loop();
    void start();

    uuid::log::Level log_level();

    static void enter_custom_context(Shell & shell, unsigned int context);
    static void load_standard_commands(unsigned int context);
};

} // namespace emsesp

#endif