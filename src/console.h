/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
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

#include <uuid/console.h>
#include <uuid/log.h>

#include "helpers.h"
#include "system.h"
#include "mqtt.h"

using uuid::console::Commands;
using uuid::console::Shell;

static constexpr uint32_t INVALID_PASSWORD_DELAY_MS = 2000;

namespace emsesp {

using LogLevel    = ::uuid::log::Level;
using LogFacility = ::uuid::log::Facility;

#ifdef LOCAL
#undef LOCAL
#endif
enum CommandFlags : uint8_t { USER = 0, ADMIN = (1 << 0), LOCAL = (1 << 1) };

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
    std::string prompt_suffix() override;
    void        end_of_transmission() override;


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
    void start_serial();
    void start_telnet();

    uuid::log::Level log_level();

    static void load_standard_commands(unsigned int context);
    static void load_system_commands(unsigned int context);

  private:
    bool telnet_enabled_ = false; // telnet is default off
};

} // namespace emsesp

#endif