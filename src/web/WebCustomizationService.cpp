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

void WebCustomization::read(WebCustomization & settings, JsonObject & root) {
    // Sensor customization
    // It's an array or id, name and offset
    JsonArray sensorsJson = root.createNestedArray("sensors");
    for (SensorCustomization sensor : settings.sensorCustomizations) {
        JsonObject sensorJson = sensorsJson.createNestedObject();
        sensorJson["id_str"]  = sensor.id_str; // key
        sensorJson["name"]    = sensor.name;
        sensorJson["offset"]  = sensor.offset;
    }
}

// call on initialization and also when settings are updated via web or console
StateUpdateResult WebCustomization::update(JsonObject & root, WebCustomization & settings) {
    // Sensor customization
    settings.sensorCustomizations.clear();
    if (root["sensors"].is<JsonArray>()) {
        for (JsonObject sensorJson : root["sensors"].as<JsonArray>()) {
            // create the sensor
            SensorCustomization sensor = SensorCustomization();
            sensor.id_str              = sensorJson["id_str"].as<std::string>();
            sensor.name                = sensorJson["name"].as<std::string>();
            sensor.offset              = sensorJson["offset"];
            settings.sensorCustomizations.push_back(sensor); // add to list
        }
    }

    return StateUpdateResult::CHANGED;
}

void WebCustomizationService::begin() {
    _fsPersistence.readFromFS();
}

void WebCustomizationService::save() {
    _fsPersistence.writeToFS();
}


} // namespace emsesp