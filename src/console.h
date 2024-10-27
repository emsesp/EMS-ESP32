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

#ifndef EMSESP_CONSOLE_H
#define EMSESP_CONSOLE_H

#include <uuid/console.h>
#include <uuid/log.h>

#include "helpers.h"
#include "system.h"
#include "mqtt.h"

#ifdef LOCAL
#undef LOCAL
#endif

namespace emsesp {

enum ShellContext : uint8_t { MAIN = 0, SYSTEM, END };

class EMSESP;

class EMSESPShell : public uuid::console::Shell {
  public:
    ~EMSESPShell() override = default;

    virtual std::string console_name() = 0;

    static void main_help_function(Shell & shell, const std::vector<std::string> & arguments);
    static void main_exit_function(Shell & shell, const std::vector<std::string> & arguments);

    EMSESP & emsesp_;

  protected:
    EMSESPShell(EMSESP & emsesp, Stream & stream, unsigned int context, unsigned int flags);

    static std::shared_ptr<uuid::console::Commands> commands_;

    // our custom functions for Shell
    void        started() override;
    void        stopped() override;
    void        display_banner() override;
    std::string hostname_text() override;
    std::string context_text() override;
    std::string prompt_suffix() override;
    void        end_of_transmission() override;

  private:
    std::string console_hostname_;
};

} // namespace emsesp

#endif