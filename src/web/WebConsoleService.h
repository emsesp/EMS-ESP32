/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2026  emsesp.org
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

#ifndef WebConsoleService_h
#define WebConsoleService_h

#define EMSESP_CONSOLE_PATH "/rest/console"

namespace emsesp {

class EMSESP;

// runs a console command line from the Web UI and returns what it printed
class WebConsoleService {
  public:
    WebConsoleService(AsyncWebServer * server, SecurityManager * securityManager);

    void begin(EMSESP & emsesp);

  private:
    static constexpr size_t MAX_COMMAND_LENGTH = 200;
    static constexpr size_t MAX_OUTPUT         = 8 * 1024;  // without PSRAM, in bytes
    static constexpr size_t MAX_OUTPUT_PSRAM   = 64 * 1024; // with PSRAM, in bytes

    void runCommand(AsyncWebServerRequest * request, JsonVariant json);

    SecurityManager * securityManager_;
    EMSESP *          emsesp_ = nullptr;
};

} // namespace emsesp

#endif
