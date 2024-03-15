/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2024  Paul Derbyshire
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

#ifndef WebAPIService_h
#define WebAPIService_h

#define EMSESP_API_SERVICE_PATH "/api"

#define GET_SETTINGS_PATH "/rest/getSettings"
#define GET_CUSTOMIZATIONS_PATH "/rest/getCustomizations"
#define GET_SCHEDULE_PATH "/rest/getSchedule"
#define GET_ENTITIES_PATH "/rest/getEntities"

namespace emsesp {

class WebAPIService {
  public:
    WebAPIService(AsyncWebServer * server, SecurityManager * securityManager);

    void webAPIService(AsyncWebServerRequest * request, JsonVariant json);

#ifdef EMSESP_TEST
    // for test.cpp
    void webAPIService(AsyncWebServerRequest * request);
#endif

    static uint32_t api_count() {
        return api_count_;
    }

    static uint16_t api_fails() {
        return api_fails_;
    }

  private:
    SecurityManager * _securityManager;

    static uint32_t api_count_;
    static uint16_t api_fails_;

    void parse(AsyncWebServerRequest * request, JsonObject input);
    void getSettings(AsyncWebServerRequest * request);
    void getCustomizations(AsyncWebServerRequest * request);
    void getSchedule(AsyncWebServerRequest * request);
    void getEntities(AsyncWebServerRequest * request);
};

} // namespace emsesp

#endif
