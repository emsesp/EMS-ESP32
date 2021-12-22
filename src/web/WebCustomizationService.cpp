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

#include "emsesp.h"

namespace emsesp {

using namespace std::placeholders; // for `_1` etc

WebCustomizationService::WebCustomizationService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _httpEndpoint(WebCustomization::read,
                    WebCustomization::update,
                    this,
                    server,
                    EMSESP_CUSTOMIZATION_SERVICE_PATH,
                    securityManager,
                    AuthenticationPredicates::IS_AUTHENTICATED)
    , _fsPersistence(WebCustomization::read, WebCustomization::update, this, fs, EMSESP_CUSTOMIZATION_FILE) {
}

// this creates the settings file
void WebCustomization::read(WebCustomization & settings, JsonObject & root) {
    // Sensor customization
    JsonArray sensorsJson = root.createNestedArray("sensors");
    for (SensorCustomization sensor : settings.sensorCustomizations) {
        JsonObject sensorJson = sensorsJson.createNestedObject();
        sensorJson["id_str"]  = sensor.id_str; // key, is
        sensorJson["name"]    = sensor.name;   // n
        sensorJson["offset"]  = sensor.offset; // o
    }

    // write back the array of excluded entity id's as an array
    JsonObject exclude_entitiesJson = root.createNestedObject("exclude_entities");
    char       s[3];
    for (EntityCustomization entityCustomization : settings.entityCustomizations) {
        // array key must be a string
        JsonArray exclude_entityJson = exclude_entitiesJson.createNestedArray(Helpers::smallitoa(s, entityCustomization.id));
        for (uint8_t entity_id : entityCustomization.entity_ids) {
            exclude_entityJson.add(entity_id);
        }
    }
}

// call on initialization and also when settings are updated via web or console
// this loads the data into the internal class
StateUpdateResult WebCustomization::update(JsonObject & root, WebCustomization & settings) {
    // Sensor customization
    settings.sensorCustomizations.clear();
    if (root["sensors"].is<JsonArray>()) {
        for (const JsonObject sensorJson : root["sensors"].as<JsonArray>()) {
            // create each of the sensor, overwritting any previous settings
            SensorCustomization sensor = SensorCustomization();
            sensor.id_str              = sensorJson["id_str"].as<std::string>();
            sensor.name                = sensorJson["name"].as<std::string>();
            sensor.offset              = sensorJson["offset"];
            settings.sensorCustomizations.push_back(sensor); // add to list
        }
    }

    // load array of entities id's to exclude, building up the object class
    settings.entityCustomizations.clear();
    if (root["exclude_entities"].is<JsonObject>()) {
        JsonObject exclude_entities = root["exclude_entities"].as<JsonObject>();
        for (JsonPair p : exclude_entities) {
            auto                key = p.key();
            EntityCustomization new_entry;
            new_entry.id = Helpers::atoint(key.c_str());
            for (const JsonVariant exclude_entity_id : p.value().as<JsonArray>()) {
                new_entry.entity_ids.push_back(exclude_entity_id);  // add entity list
                settings.entityCustomizations.push_back(new_entry); // save it back
            }
        }
    }

    return StateUpdateResult::CHANGED;
}

// load the settings when the service starts
void WebCustomizationService::begin() {
    _fsPersistence.readFromFS();
}

} // namespace emsesp
