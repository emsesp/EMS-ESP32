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

#ifndef WebAPIService_h
#define WebAPIService_h

#define EMSESP_API_SERVICE_PATH "/api"

namespace emsesp {

class WebAPIService {
  public:
    WebAPIService(AsyncWebServer * server, SecurityManager * securityManager);

    void webAPIService(AsyncWebServerRequest * request, JsonVariant input);

#if defined(EMSESP_TEST)
    // for test.cpp and running unit tests
    void         webAPIService(AsyncWebServerRequest * request);
    void         storeResponse(JsonObject response);
    const char * getResponse();
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
};

} // namespace emsesp

#endif
