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

#ifndef WebActivityService_h
#define WebActivityService_h

#define EMSESP_ACTIVITY_SERVICE_PATH "/rest/activity"

namespace emsesp {

class WebActivityService {
  public:
    WebActivityService(AsyncWebServer * server, SecurityManager * securityManager);

  private:
    void webActivityService(AsyncWebServerRequest * request);
};

} // namespace emsesp

#endif
