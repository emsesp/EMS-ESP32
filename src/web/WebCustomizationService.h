/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2023  Paul Derbyshire
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
#define CUSTOM_ENTITIES_PATH "/rest/customEntities"
#define RESET_CUSTOMIZATION_SERVICE_PATH "/rest/resetCustomizations"

namespace emsesp {

// Customization for dallas sensor
class SensorCustomization {
  public:
    std::string id;
    std::string name;
    uint16_t    offset;
};

class AnalogCustomization {
  public:
    uint8_t     gpio;
    std::string name;
    double      offset;
    double      factor;
    uint8_t     uom;  // 0 is none
    int8_t      type; // -1 is for deletion

    // used for removing from a list
    bool operator==(const AnalogCustomization & a) const {
        return gpio == a.gpio;
    }
    bool operator!=(const AnalogCustomization & a) const {
        return !operator==(a);
    }
};

// we use product_id and device_id to make the device unique
class EntityCustomization {
  public:
    uint8_t                  product_id; // device's product id
    uint8_t                  device_id;  // device's device id
    std::vector<std::string> entity_ids; // array of entity ids with masks and optional custom fullname
};

class WebCustomization {
  public:
    std::list<SensorCustomization> sensorCustomizations; // for sensor names and offsets
    std::list<AnalogCustomization> analogCustomizations; // for analog sensors
    std::list<EntityCustomization> entityCustomizations; // for a list of entities that have a special mask set
    static void                    read(WebCustomization & settings, JsonObject & root);
    static StateUpdateResult       update(JsonObject & root, WebCustomization & settings);
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
    void custom_entities(AsyncWebServerRequest * request, JsonVariant & json);
    void device_entities(AsyncWebServerRequest * request, JsonVariant & json);
    void reset_customization(AsyncWebServerRequest * request);

    AsyncCallbackJsonWebHandler _masked_entities_handler, _device_entities_handler;
};

} // namespace emsesp

#endif