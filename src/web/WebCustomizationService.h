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

#ifndef WebCustomizationService_h
#define WebCustomizationService_h

#include <HttpEndpoint.h>
#include <FSPersistence.h>
#include <list>

#include "../default_settings.h"

#define EMSESP_CUSTOMIZATION_FILE "/config/emsespCustomization.json"
#define EMSESP_CUSTOMIZATION_SERVICE_PATH "/rest/customization"

namespace emsesp {

// Customization for dallas sensor
class SensorCustomization {
  public:
    std::string id_str;
    std::string name;
    uint16_t    offset;
};

class WebCustomization {
  public:
    std::list<SensorCustomization> sensorCustomizations;

    static void              read(WebCustomization & settings, JsonObject & root);
    static StateUpdateResult update(JsonObject & root, WebCustomization & settings);
};

class WebCustomizationService : public StatefulService<WebCustomization> {
  public:
    WebCustomizationService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager);

    void begin();
    void save();

  private:
    HttpEndpoint<WebCustomization>  _httpEndpoint;
    FSPersistence<WebCustomization> _fsPersistence;
};

} // namespace emsesp

#endif