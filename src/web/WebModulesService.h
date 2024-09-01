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

#ifndef WebModulesService_h
#define WebModulesService_h

#define EMSESP_MODULES_FILE "/config/emsespModules.json"
#define EMSESP_MODULES_SERVICE_PATH "/rest/modules" // GET and POST

namespace emsesp {

class WebModules {
  public:
    static void              read(WebModules & webModules, JsonObject root);
    static StateUpdateResult update(JsonObject root, WebModules & webModules);
};

class WebModulesService : public StatefulService<WebModules> {
  public:
    WebModulesService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager);

    void begin();
    void loop();

#ifndef EMSESP_STANDALONE
  private:
#endif

    HttpEndpoint<WebModules>  _httpEndpoint;
    FSPersistence<WebModules> _fsPersistence;
};

} // namespace emsesp

#endif