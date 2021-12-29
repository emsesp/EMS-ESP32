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

#define EMSESP_CUSTOMIZATION_FILE "/config/emsespCustomization.json"

// GET
#define DEVICES_SERVICE_PATH "/rest/devices"
#define EMSESP_CUSTOMIZATION_SERVICE_PATH "/rest/customization"

// POST
#define DEVICE_ENTITIES_PATH "/rest/deviceEntities"
#define EXCLUDE_ENTITIES_PATH "/rest/excludeEntities"
#define RESET_CUSTOMIZATION_SERVICE_PATH "/rest/resetCustomizations"

namespace emsesp {

// Customization for dallas sensor
class SensorCustomization {
  public:
    std::string id_str;
    std::string name;
    uint16_t    offset;
};

class AnalogCustomization {
  public:
    uint8_t     id;
    std::string name;
    uint16_t    offset;
    float       factor;
    uint8_t     uom;
    int8_t      type; // -1 for deletion
};

// we use product_id and device_id to make the device unique
class EntityCustomization {
  public:
    uint8_t              product_id; // device's product id
    uint8_t              device_id;  // device's device id
    std::vector<uint8_t> entity_ids; // array of entity ids to exclude
};

class WebCustomization {
  public:
    std::list<SensorCustomization> sensorCustomizations; // for sensor names and offsets
    std::list<AnalogCustomization> analogCustomizations; // for analog sensors
    std::list<EntityCustomization> entityCustomizations; // for a list of entities that should be excluded from the device list

    static void              read(WebCustomization & settings, JsonObject & root);
    static StateUpdateResult update(JsonObject & root, WebCustomization & settings);
};

class WebCustomizationService : public StatefulService<WebCustomization> {
  public:
    WebCustomizationService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager);

    void begin();

// make all functions public so we can test in the debug and standalone mode
#ifndef EMSESP_STANDALONE
  private:
#endif

    HttpEndpoint<WebCustomization>  _httpEndpoint;
    FSPersistence<WebCustomization> _fsPersistence;

    // GET
    void devices(AsyncWebServerRequest * request);

    // POST
    void exclude_entities(AsyncWebServerRequest * request, JsonVariant & json);
    void device_entities(AsyncWebServerRequest * request, JsonVariant & json);
    void reset_customization(AsyncWebServerRequest * request);

    AsyncCallbackJsonWebHandler _exclude_entities_handler, _device_entities_handler;
};

} // namespace emsesp

#endif