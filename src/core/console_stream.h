/*
 * mcu-app - Microcontroller application framework
 * Copyright 2022  Simon Arlott
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

#pragma once

#include <uuid/console.h>

#include "console.h"

namespace emsesp {

class EMSESPConsole : public EMSESPShell {
  public:
    EMSESPConsole(EMSESP & emsesp, Stream & stream, bool local);
#ifndef ENV_NATIVE
    EMSESPConsole(EMSESP & emsesp, Stream & stream, const IPAddress & addr, uint16_t port);
#endif
    ~EMSESPConsole() override;

    std::string console_name() override;

  private:
#ifndef EMSESP_STANDALONE
    static std::vector<bool> ptys_;
#endif

    std::string name_;
#ifndef EMSESP_STANDALONE
    size_t    pty_;
    IPAddress addr_;
    uint16_t  port_;
#endif
};

} // namespace emsesp