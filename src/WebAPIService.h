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

#ifndef WebAPIService_h
#define WebAPIService_h

#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>

#include <string>
#include <unordered_map>
#include <vector>

#define EMSESP_API_SERVICE_PATH "/api"

namespace emsesp {

typedef std::unordered_map<std::string, std::string> KeyValueMap_t;
typedef std::vector<std::string>                     Folder_t;

class SUrlParser {
  private:
    KeyValueMap_t m_keysvalues;
    Folder_t      m_folders;

  public:
    SUrlParser(){};
    SUrlParser(const char * url);

    bool parse(const char * url);

    Folder_t & paths() {
        return m_folders;
    };

    KeyValueMap_t & params() {
        return m_keysvalues;
    };

    std::string path();
};

class WebAPIService {
  public:
    WebAPIService(AsyncWebServer * server, SecurityManager * securityManager);

  private:
    SecurityManager *           _securityManager;
    AsyncCallbackJsonWebHandler _apiHandler; // for POSTs

    void webAPIService_post(AsyncWebServerRequest * request, JsonVariant & json); // for POSTs
    void webAPIService_get(AsyncWebServerRequest * request);                      // for GETs

    void parse(AsyncWebServerRequest * request, std::string & device, std::string & cmd, int id, std::string & value);
    void send_message_response(AsyncWebServerRequest * request, uint16_t error_code, const char * error_message = nullptr);
};

} // namespace emsesp

#endif
