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

#ifndef WebViewLevelService_h
#define WebViewLevelService_h

// The Web UI menu shows the values of a page at two levels: Normal and All values. The Web UI has a built-in
// default for every well known value; this service keeps what the user changed, so it is the same in every
// browser. It is a small object {"boiler:dhw/mode": 1, ...} with the key of a value and its level (1 = Normal,
// 2 = only in All values). The gateway only stores it and sends it back; it is not part of the customizations.

#define EMSESP_VIEWLEVELS_SERVICE_PATH "/rest/viewLevels" // GET: all levels, POST: replace them (admin only)

#define EMSESP_VIEWLEVELS_FILE "/viewlevels.json"

namespace emsesp {

class WebViewLevelService {
  public:
    // limits, also used by the tests
    static constexpr size_t MAX_ENTRIES    = 1000;
    static constexpr size_t MAX_KEY_LENGTH = 80;

    WebViewLevelService(AsyncWebServer * server, SecurityManager * securityManager);

    // true for a key of printable ASCII characters, without quotes and backslashes
    static bool validKey(const char * key);

  private:
    void read(AsyncWebServerRequest * request);
    void write(AsyncWebServerRequest * request, JsonVariant json);
};

} // namespace emsesp

#endif
