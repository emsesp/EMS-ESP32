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
    , _fsPersistence(WebCustomization::read, WebCustomization::update, this, fs, EMSESP_CUSTOMIZATION_FILE)
    , _exclude_entities_handler(EXCLUDE_ENTITIES_PATH,
                                securityManager->wrapCallback(std::bind(&WebCustomizationService::exclude_entities, this, _1, _2),
                                                              AuthenticationPredicates::IS_AUTHENTICATED))
    , _device_entities_handler(DEVICE_ENTITIES_PATH,
                               securityManager->wrapCallback(std::bind(&WebCustomizationService::device_entities, this, _1, _2),
                                                             AuthenticationPredicates::IS_AUTHENTICATED)) {
    server->on(DEVICES_SERVICE_PATH,
               HTTP_GET,
               securityManager->wrapRequest(std::bind(&WebCustomizationService::devices, this, _1), AuthenticationPredicates::IS_AUTHENTICATED));

    server->on(RESET_CUSTOMIZATION_SERVICE_PATH,
               HTTP_POST,
               securityManager->wrapRequest(std::bind(&WebCustomizationService::reset_customization, this, _1), AuthenticationPredicates::IS_ADMIN));

    _exclude_entities_handler.setMethod(HTTP_POST);
    _exclude_entities_handler.setMaxContentLength(256);
    server->addHandler(&_exclude_entities_handler);

    _device_entities_handler.setMethod(HTTP_POST);
    _device_entities_handler.setMaxContentLength(256);
    server->addHandler(&_device_entities_handler);
}

// this creates the customization file, saving to the FS
void WebCustomization::read(WebCustomization & settings, JsonObject & root) {
    // Dallas Sensor customization
    JsonArray sensorsJson = root.createNestedArray("sensors");
    for (const SensorCustomization & sensor : settings.sensorCustomizations) {
        JsonObject sensorJson = sensorsJson.createNestedObject();
        sensorJson["id_str"]  = sensor.id_str; // is
        sensorJson["name"]    = sensor.name;   // n
        sensorJson["offset"]  = sensor.offset; // o
    }

    // Analog Sensor customization
    JsonArray analogJson = root.createNestedArray("analogs");
    for (const AnalogCustomization & sensor : settings.analogCustomizations) {
        JsonObject sensorJson = analogJson.createNestedObject();
        sensorJson["id"]      = sensor.id;     // i
        sensorJson["name"]    = sensor.name;   // n
        sensorJson["offset"]  = sensor.offset; // o
        sensorJson["factor"]  = sensor.factor; // f
        sensorJson["uom"]     = sensor.uom;    // u
        sensorJson["type"]    = sensor.type;   // t
    }

    // Exclude entities customization
    JsonArray exclude_entitiesJson = root.createNestedArray("exclude_entities");
    for (const EntityCustomization & entityCustomization : settings.entityCustomizations) {
        JsonObject entityJson    = exclude_entitiesJson.createNestedObject();
        entityJson["product_id"] = entityCustomization.product_id;
        entityJson["device_id"]  = entityCustomization.device_id;

        JsonArray exclude_entityJson = entityJson.createNestedArray("entity_ids");
        for (uint8_t entity_id : entityCustomization.entity_ids) {
            exclude_entityJson.add(entity_id);
        }
    }
}

// call on initialization and also when the page is saved via web
// this loads the data into the internal class
StateUpdateResult WebCustomization::update(JsonObject & root, WebCustomization & settings) {
    // Dallas Sensor customization
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

    // Analog Sensor customization
    settings.analogCustomizations.clear();
    if (root["analogs"].is<JsonArray>()) {
        for (const JsonObject analogJson : root["analogs"].as<JsonArray>()) {
            // create each of the sensor, overwritting any previous settings
            AnalogCustomization sensor = AnalogCustomization();
            sensor.id                  = analogJson["id"];
            sensor.name                = analogJson["name"].as<std::string>();
            sensor.offset              = analogJson["offset"];
            sensor.factor              = analogJson["factor"];
            sensor.uom                 = analogJson["uom"];
            sensor.type                = analogJson["type"];
            settings.analogCustomizations.push_back(sensor); // add to list
        }
    }

    // load array of entities id's to exclude, building up the object class
    settings.entityCustomizations.clear();
    if (root["exclude_entities"].is<JsonArray>()) {
        for (const JsonObject exclude_entities : root["exclude_entities"].as<JsonArray>()) {
            EntityCustomization new_entry = EntityCustomization();
            new_entry.product_id          = exclude_entities["product_id"];
            new_entry.device_id           = exclude_entities["device_id"];

            for (const JsonVariant exclude_entity_id : exclude_entities["entity_ids"].as<JsonArray>()) {
                new_entry.entity_ids.push_back(exclude_entity_id.as<uint8_t>()); // add entity list
            }
            settings.entityCustomizations.push_back(new_entry); // save the new object
        }
    }

    return StateUpdateResult::CHANGED;
}

// deletes the customization file
void WebCustomizationService::reset_customization(AsyncWebServerRequest * request) {
#ifndef EMSESP_STANDALONE
    if (LITTLEFS.remove(EMSESP_CUSTOMIZATION_FILE)) {
        AsyncWebServerResponse * response = request->beginResponse(200); // OK
        request->send(response);
        EMSESP::system_.restart_requested(true);
        return;
    }
    // failed
    AsyncWebServerResponse * response = request->beginResponse(204); // no content error
    request->send(response);
#endif
}

// send back a short list devices used in the customization page
void WebCustomizationService::devices(AsyncWebServerRequest * request) {
    AsyncJsonResponse * response = new AsyncJsonResponse(false, EMSESP_JSON_SIZE_LARGE_DYN);
    JsonObject          root     = response->getRoot();

    JsonArray devices = root.createNestedArray("devices");
    for (auto & emsdevice : EMSESP::emsdevices) {
        if (emsdevice->has_entities()) {
            JsonObject obj = devices.createNestedObject();
            obj["i"]       = emsdevice->unique_id(); // a unique id

            // shortname - we prefix the count to make it unique
            uint8_t device_index = EMSESP::device_index(emsdevice->device_type(), emsdevice->unique_id());
            if (device_index) {
                char s[10];
                obj["s"] = emsdevice->device_type_name() + Helpers::smallitoa(s, device_index);
            } else {
                obj["s"] = emsdevice->device_type_name();
            }
        }
    }

    response->setLength();
    request->send(response);
}

// send back filtered list of device entities
void WebCustomizationService::device_entities(AsyncWebServerRequest * request, JsonVariant & json) {
    if (json.is<JsonObject>()) {
        MsgpackAsyncJsonResponse * response = new MsgpackAsyncJsonResponse(true, EMSESP_JSON_SIZE_XXLARGE_DYN);
        for (const auto & emsdevice : EMSESP::emsdevices) {
            if (emsdevice) {
                if (emsdevice->unique_id() == json["id"]) {
#ifndef EMSESP_STANDALONE
                    JsonArray output = response->getRoot();
                    emsdevice->generate_values_web_all(output);
#endif
                    response->setLength();
                    request->send(response);
                    return;
                }
            }
        }
    }

    // invalid, but send OK anyway
    AsyncWebServerResponse * response = request->beginResponse(200);
    request->send(response);
}

// takes a list of excluded ids send from the webUI
// saves it in the customization service
// and updates the entity list real-time
void WebCustomizationService::exclude_entities(AsyncWebServerRequest * request, JsonVariant & json) {
    if (json.is<JsonObject>()) {
        // find the device using the unique_id
        for (const auto & emsdevice : EMSESP::emsdevices) {
            if (emsdevice) {
                uint8_t unique_device_id = json["id"];
                if (emsdevice->unique_id() == unique_device_id) {
                    JsonArray entity_ids = json["entity_ids"];

                    std::vector<uint8_t> temp;
                    for (JsonVariant id : entity_ids) {
                        uint8_t entity_id = id.as<int>();
                        emsdevice->exclude_entity(entity_id); // this will have immediate affect
                        temp.push_back(entity_id);
                    }

                    // Save the list to the customization file
                    uint8_t product_id = emsdevice->product_id();
                    uint8_t device_id  = emsdevice->device_id();

                    EMSESP::webCustomizationService.update(
                        [&](WebCustomization & settings) {
                            // if it exists (productid and deviceid match) overwrite it
                            for (auto & entityCustomization : settings.entityCustomizations) {
                                if ((entityCustomization.product_id == product_id) && (entityCustomization.device_id == device_id)) {
                                    // already exists, clear the list and add the new values
                                    entityCustomization.entity_ids.clear();
                                    for (uint8_t i = 0; i < temp.size(); i++) {
                                        entityCustomization.entity_ids.push_back(temp[i]);
                                    }
                                    return StateUpdateResult::CHANGED;
                                }
                            }
                            // create a new entry in the list
                            EntityCustomization new_entry;
                            new_entry.product_id = product_id;
                            new_entry.device_id  = device_id;
                            for (uint8_t i = 0; i < temp.size(); i++) {
                                new_entry.entity_ids.push_back(temp[i]);
                            }
                            settings.entityCustomizations.push_back(new_entry);
                            return StateUpdateResult::CHANGED;
                        },
                        "local");

                    break;
                }
            }
        }
    }

    AsyncWebServerResponse * response = request->beginResponse(200); // OK
    request->send(response);
}

// load the settings when the service starts
void WebCustomizationService::begin() {
    _fsPersistence.readFromFS();
}

} // namespace emsesp
