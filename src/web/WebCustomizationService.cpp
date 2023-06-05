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
    , _masked_entities_handler(CUSTOM_ENTITIES_PATH,
                               securityManager->wrapCallback(std::bind(&WebCustomizationService::custom_entities, this, _1, _2),
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

    _masked_entities_handler.setMethod(HTTP_POST);
    _masked_entities_handler.setMaxContentLength(2048);
    _masked_entities_handler.setMaxJsonBufferSize(2048);
    server->addHandler(&_masked_entities_handler);

    _device_entities_handler.setMethod(HTTP_POST);
    _device_entities_handler.setMaxContentLength(256);
    server->addHandler(&_device_entities_handler);
}

// this creates the customization file, saving it to the FS
void WebCustomization::read(WebCustomization & settings, JsonObject & root) {
    // Temperature Sensor customization
    JsonArray sensorsJson = root.createNestedArray("ts");
    for (const SensorCustomization & sensor : settings.sensorCustomizations) {
        JsonObject sensorJson = sensorsJson.createNestedObject();
        sensorJson["id"]      = sensor.id;     // ID of chip
        sensorJson["name"]    = sensor.name;   // n
        sensorJson["offset"]  = sensor.offset; // o
    }

    // Analog Sensor customization
    JsonArray analogJson = root.createNestedArray("as");
    for (const AnalogCustomization & sensor : settings.analogCustomizations) {
        JsonObject sensorJson = analogJson.createNestedObject();
        sensorJson["gpio"]    = sensor.gpio;   // g
        sensorJson["name"]    = sensor.name;   // n
        sensorJson["offset"]  = sensor.offset; // o
        sensorJson["factor"]  = sensor.factor; // f
        sensorJson["uom"]     = sensor.uom;    // u
        sensorJson["type"]    = sensor.type;   // t
    }

    // Masked entities customization
    JsonArray masked_entitiesJson = root.createNestedArray("masked_entities");
    for (const EntityCustomization & entityCustomization : settings.entityCustomizations) {
        JsonObject entityJson    = masked_entitiesJson.createNestedObject();
        entityJson["product_id"] = entityCustomization.product_id;
        entityJson["device_id"]  = entityCustomization.device_id;

        // entries are in the form <XX><shortname>[|optional customname] e.g "08heatingactive|heating is on"
        JsonArray masked_entityJson = entityJson.createNestedArray("entity_ids");
        for (std::string entity_id : entityCustomization.entity_ids) {
            masked_entityJson.add(entity_id);
        }
    }
}

// call on initialization and also when the page is saved via web UI
// this loads the data into the internal class
StateUpdateResult WebCustomization::update(JsonObject & root, WebCustomization & settings) {
#ifdef EMSESP_STANDALONE
    // invoke some fake data for testing
    const char * json = "{\"ts\":[],\"as\":[],\"masked_entities\":[{\"product_id\":123,\"device_id\":8,\"entity_ids\":[\"08heatingactive|my custom "
                        "name for heating active\",\"08tapwateractive\"]}]}";

    StaticJsonDocument<500> doc;
    deserializeJson(doc, json);
    root = doc.as<JsonObject>();

    Serial.println(COLOR_BRIGHT_MAGENTA);
    Serial.print("Using custom file: ");
    serializeJson(root, Serial);
    Serial.println(COLOR_RESET);
#endif

    // Temperature Sensor customization
    settings.sensorCustomizations.clear();
    if (root["ts"].is<JsonArray>()) {
        for (const JsonObject sensorJson : root["ts"].as<JsonArray>()) {
            // create each of the sensor, overwriting any previous settings
            auto sensor   = SensorCustomization();
            sensor.id     = sensorJson["id"].as<std::string>();
            sensor.name   = sensorJson["name"].as<std::string>();
            sensor.offset = sensorJson["offset"];
            settings.sensorCustomizations.push_back(sensor); // add to list
        }
    }

    // Analog Sensor customization
    settings.analogCustomizations.clear();
    if (root["as"].is<JsonArray>()) {
        for (const JsonObject analogJson : root["as"].as<JsonArray>()) {
            // create each of the sensor, overwriting any previous settings
            auto sensor   = AnalogCustomization();
            sensor.gpio   = analogJson["gpio"];
            sensor.name   = analogJson["name"].as<std::string>();
            sensor.offset = analogJson["offset"];
            sensor.factor = analogJson["factor"];
            sensor.uom    = analogJson["uom"];
            sensor.type   = analogJson["type"];
            settings.analogCustomizations.push_back(sensor); // add to list
        }
    }

    // load array of entities id's with masks, building up the object class
    settings.entityCustomizations.clear();
    if (root["masked_entities"].is<JsonArray>()) {
        for (const JsonObject masked_entities : root["masked_entities"].as<JsonArray>()) {
            auto new_entry       = EntityCustomization();
            new_entry.product_id = masked_entities["product_id"];
            new_entry.device_id  = masked_entities["device_id"];

            for (const JsonVariant masked_entity_id : masked_entities["entity_ids"].as<JsonArray>()) {
                if (masked_entity_id.is<std::string>()) {
                    new_entry.entity_ids.push_back(masked_entity_id.as<std::string>()); // add entity list
                }
            }

            settings.entityCustomizations.push_back(new_entry); // save the new object
        }
    }

    return StateUpdateResult::CHANGED;
}

// deletes the customization file
void WebCustomizationService::reset_customization(AsyncWebServerRequest * request) {
#ifndef EMSESP_STANDALONE
    if (LittleFS.remove(EMSESP_CUSTOMIZATION_FILE)) {
        AsyncWebServerResponse * response = request->beginResponse(200); // OK
        request->send(response);
        EMSESP::system_.restart_requested(true);
        return;
    }
    // failed
    AsyncWebServerResponse * response = request->beginResponse(400); // bad request
    request->send(response);
#endif
}

// send back a list of devices used in the customization web page
void WebCustomizationService::devices(AsyncWebServerRequest * request) {
    auto *     response = new AsyncJsonResponse(false, EMSESP_JSON_SIZE_XLARGE);
    JsonObject root     = response->getRoot();

    // list is already sorted by device type
    // controller is ignored since it doesn't have any associated entities
    JsonArray devices = root.createNestedArray("devices");
    for (const auto & emsdevice : EMSESP::emsdevices) {
        if (emsdevice->has_entities()) {
            JsonObject obj = devices.createNestedObject();
            obj["i"]       = emsdevice->unique_id();                                                                     // its unique id
            obj["s"]  = std::string(emsdevice->device_type_2_device_name_translated()) + " (" + emsdevice->name() + ")"; // shortname, is device type translated
            obj["tn"] = emsdevice->device_type_name();                                                                   // non-translated, lower-case
            obj["t"]  = emsdevice->device_type();                                                                        // internal device type ID
        }
    }

    response->setLength();
    request->send(response);
}

// send back list of device entities
void WebCustomizationService::device_entities(AsyncWebServerRequest * request, JsonVariant & json) {
    if (json.is<JsonObject>()) {
        size_t buffer   = EMSESP_JSON_SIZE_XXXXLARGE;
        auto * response = new MsgpackAsyncJsonResponse(true, buffer);
        while (!response->getSize()) {
            delete response;
            buffer -= 1024;
            response = new MsgpackAsyncJsonResponse(true, buffer);
        }
        for (const auto & emsdevice : EMSESP::emsdevices) {
            if (emsdevice->unique_id() == json["id"]) {
#ifndef EMSESP_STANDALONE
                JsonArray output = response->getRoot();
                emsdevice->generate_values_web_customization(output);
#endif
#if defined(EMSESP_DEBUG)
                size_t length = response->setLength();
                EMSESP::logger().debug("Customization buffer used: %d", length);
#else
                response->setLength();
#endif
                request->send(response);
                return;
            }
        }
    }

    // invalid, but send OK anyway
    AsyncWebServerResponse * response = request->beginResponse(200);
    request->send(response);
}

// takes a list of updated entities with new masks from the web UI
// saves it in the customization service
// and updates the entity list real-time
void WebCustomizationService::custom_entities(AsyncWebServerRequest * request, JsonVariant & json) {
    bool need_reboot = false;
    if (json.is<JsonObject>()) {
        // find the device using the unique_id
        for (const auto & emsdevice : EMSESP::emsdevices) {
            if (emsdevice) {
                uint8_t unique_device_id = json["id"];
                if (emsdevice->unique_id() == unique_device_id) {
                    uint8_t product_id = emsdevice->product_id();
                    uint8_t device_id  = emsdevice->device_id();

                    // and set the mask and custom names immediately for any listed entities
                    JsonArray                entity_ids_json = json["entity_ids"];
                    std::vector<std::string> entity_ids;
                    for (const JsonVariant id : entity_ids_json) {
                        std::string id_s = id.as<std::string>();
                        if (id_s[0] == '8') {
                            entity_ids.push_back(id_s);
                            need_reboot = true;
                        } else {
                            emsdevice->setCustomEntity(id_s);
                        }
                        // emsesp::EMSESP::logger().info(id.as<const char *>());
                    }
                    // add deleted entities from file
                    EMSESP::webCustomizationService.read([&](WebCustomization & settings) {
                        for (EntityCustomization entityCustomization : settings.entityCustomizations) {
                            if (entityCustomization.device_id == device_id) {
                                for (std::string entity_id : entityCustomization.entity_ids) {
                                    uint8_t     mask = Helpers::hextoint(entity_id.substr(0, 2).c_str());
                                    std::string name = DeviceValue::get_name(entity_id);
                                    if (mask & 0x80) {
                                        bool is_set = false;
                                        for (const JsonVariant id : entity_ids_json) {
                                            std::string id_s = id.as<std::string>();
                                            if (name == DeviceValue::get_name(id_s)) {
                                                is_set      = true;
                                                need_reboot = true;
                                                break;
                                            }
                                        }
                                        if (!is_set) {
                                            entity_ids.push_back(entity_id);
                                        }
                                    }
                                }
                                break;
                            }
                        }
                    });
                    // get list of entities that have masks set or a custom fullname
                    emsdevice->getCustomEntities(entity_ids);

                    // Save the list to the customization file
                    EMSESP::webCustomizationService.update(
                        [&](WebCustomization & settings) {
                            // see if we already have a mask list for this device, if so remove it
                            for (auto it = settings.entityCustomizations.begin(); it != settings.entityCustomizations.end();) {
                                if ((*it).product_id == product_id && (*it).device_id == device_id) {
                                    it = settings.entityCustomizations.erase(it);
                                    break;
                                } else {
                                    ++it;
                                }
                            }

                            // create a new entry for this device if there are values
                            EntityCustomization new_entry;
                            new_entry.product_id = product_id;
                            new_entry.device_id  = device_id;

                            new_entry.entity_ids = entity_ids;

                            // add the record and save
                            settings.entityCustomizations.push_back(new_entry);
                            return StateUpdateResult::CHANGED;
                        },
                        "local");

                    break;
                }
            }
        }
    }

    AsyncWebServerResponse * response = request->beginResponse(need_reboot ? 201 : 200); // OK
    request->send(response);
}

// load the settings when the service starts
void WebCustomizationService::begin() {
    _fsPersistence.readFromFS();
}

} // namespace emsesp