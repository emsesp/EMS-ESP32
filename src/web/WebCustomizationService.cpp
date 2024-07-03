/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2024  Paul Derbyshire
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

bool WebCustomization::_start = true;

WebCustomizationService::WebCustomizationService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _fsPersistence(WebCustomization::read, WebCustomization::update, this, fs, EMSESP_CUSTOMIZATION_FILE) {
    // GET
    server->on(DEVICE_ENTITIES_PATH,
               HTTP_GET,
               securityManager->wrapRequest([this](AsyncWebServerRequest * request) { device_entities(request); }, AuthenticationPredicates::IS_AUTHENTICATED));

    server->on(DEVICES_SERVICE_PATH,
               HTTP_GET,
               securityManager->wrapRequest([this](AsyncWebServerRequest * request) { devices(request); }, AuthenticationPredicates::IS_AUTHENTICATED));

    // POST
    server->on(RESET_CUSTOMIZATION_SERVICE_PATH,
               HTTP_POST,
               securityManager->wrapRequest([this](AsyncWebServerRequest * request) { reset_customization(request); }, AuthenticationPredicates::IS_ADMIN));

    server->on(WRITE_DEVICE_NAME_PATH,
               securityManager->wrapCallback([this](AsyncWebServerRequest * request, JsonVariant json) { writeDeviceName(request, json); },
                                             AuthenticationPredicates::IS_AUTHENTICATED));

    server->on(CUSTOMIZATION_ENTITIES_PATH,
               securityManager->wrapCallback([this](AsyncWebServerRequest * request, JsonVariant json) { customization_entities(request, json); },
                                             AuthenticationPredicates::IS_AUTHENTICATED));
}

// this creates the customization file, saving it to the FS
void WebCustomization::read(WebCustomization & customizations, JsonObject root) {
    // Temperature Sensor customization
    JsonArray sensorsJson = root["ts"].to<JsonArray>();

    for (const SensorCustomization & sensor : customizations.sensorCustomizations) {
        JsonObject sensorJson = sensorsJson.add<JsonObject>();
        sensorJson["id"]      = sensor.id;     // ID of chip
        sensorJson["name"]    = sensor.name;   // n
        sensorJson["offset"]  = sensor.offset; // o
    }

    // Analog Sensor customization
    JsonArray analogJson = root["as"].to<JsonArray>();
    for (const AnalogCustomization & sensor : customizations.analogCustomizations) {
        JsonObject sensorJson = analogJson.add<JsonObject>();
        sensorJson["gpio"]    = sensor.gpio;   // g
        sensorJson["name"]    = sensor.name;   // n
        sensorJson["offset"]  = sensor.offset; // o
        sensorJson["factor"]  = sensor.factor; // f
        sensorJson["uom"]     = sensor.uom;    // u
        sensorJson["type"]    = sensor.type;   // t
    }

    // Masked entities customization and custom device name (optional)
    JsonArray masked_entitiesJson = root["masked_entities"].to<JsonArray>();
    for (const EntityCustomization & entityCustomization : customizations.entityCustomizations) {
        JsonObject entityJson     = masked_entitiesJson.add<JsonObject>();
        entityJson["product_id"]  = entityCustomization.product_id;
        entityJson["device_id"]   = entityCustomization.device_id;
        entityJson["custom_name"] = entityCustomization.custom_name;

        // entries are in the form <XX><shortname>[optional customname] e.g "08heatingactive|heating is on"
        JsonArray masked_entityJson = entityJson["entity_ids"].to<JsonArray>();
        for (std::string entity_id : entityCustomization.entity_ids) {
            masked_entityJson.add(entity_id);
        }
    }
}

// call on initialization and also when the page is saved via web UI
// this loads the data into the internal class
StateUpdateResult WebCustomization::update(JsonObject root, WebCustomization & customizations) {
    // Temperature Sensor customization
    customizations.sensorCustomizations.clear();
    if (root["ts"].is<JsonArray>()) {
        for (const JsonObject sensorJson : root["ts"].as<JsonArray>()) {
            // create each of the sensor, overwriting any previous settings
            auto sensor   = SensorCustomization();
            sensor.id     = sensorJson["id"].as<std::string>();
            sensor.name   = sensorJson["name"].as<std::string>();
            sensor.offset = sensorJson["offset"];
            customizations.sensorCustomizations.push_back(sensor); // add to list
        }
    }

    // Analog Sensor customization
    customizations.analogCustomizations.clear();
    if (root["as"].is<JsonArray>()) {
        for (const JsonObject analogJson : root["as"].as<JsonArray>()) {
            // create each of the sensor, overwriting any previous settings
            auto analog   = AnalogCustomization();
            analog.gpio   = analogJson["gpio"];
            analog.name   = analogJson["name"].as<std::string>();
            analog.offset = analogJson["offset"];
            analog.factor = analogJson["factor"];
            analog.uom    = analogJson["uom"];
            analog.type   = analogJson["type"];
            if (_start && analog.type == EMSESP::analogsensor_.AnalogType::DIGITAL_OUT && analog.uom > DeviceValue::DeviceValueUOM::NONE) {
                analog.offset = analog.uom - 1;
            }
            customizations.analogCustomizations.push_back(analog); // add to list
        }
    }

    _start = false;
    // load array of entities id's with masks, building up the object class
    customizations.entityCustomizations.clear();
    if (root["masked_entities"].is<JsonArray>()) {
        for (const JsonObject masked_entities : root["masked_entities"].as<JsonArray>()) {
            auto emsEntity        = EntityCustomization();
            emsEntity.product_id  = masked_entities["product_id"];
            emsEntity.device_id   = masked_entities["device_id"];
            emsEntity.custom_name = masked_entities["custom_name"].as<std::string>();

            for (const JsonVariant masked_entity_id : masked_entities["entity_ids"].as<JsonArray>()) {
                if (masked_entity_id.is<std::string>()) {
                    emsEntity.entity_ids.push_back(masked_entity_id.as<std::string>()); // add entity list
                }
            }

            customizations.entityCustomizations.push_back(emsEntity); // save the new object
        }
    }

    return StateUpdateResult::CHANGED;
}

// deletes the customization file
void WebCustomizationService::reset_customization(AsyncWebServerRequest * request) {
#ifndef EMSESP_STANDALONE
    if (LittleFS.remove(EMSESP_CUSTOMIZATION_FILE)) {
        AsyncWebServerResponse * response = request->beginResponse(205); // restart needed
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
    auto *     response = new AsyncJsonResponse(false);
    JsonObject root     = response->getRoot();

    // list is already sorted by device type
    // controller is ignored since it doesn't have any associated entities
    JsonArray devices = root["devices"].to<JsonArray>();
    for (const auto & emsdevice : EMSESP::emsdevices) {
        if (emsdevice->has_entities()) {
            JsonObject obj = devices.add<JsonObject>();
            obj["i"]       = emsdevice->unique_id();                                         // its unique id
            obj["s"]       = emsdevice->name();                                              // custom name
            obj["t"]       = emsdevice->device_type();                                       // internal device type ID
            obj["tn"]      = std::string(emsdevice->device_type_2_device_name_translated()); // translated device type name
            obj["url"]     = emsdevice->device_type_name();                                  // non-translated, lower-case, used for API URL
        }
    }

    response->setLength();
    request->send(response);
}

// send back list of device entities
void WebCustomizationService::device_entities(AsyncWebServerRequest * request) {
    uint8_t id;
    if (request->hasParam(F_(id))) {
        id = Helpers::atoint(request->getParam(F_(id))->value().c_str()); // get id from url

        auto * response = new AsyncJsonResponse(true, true); // array and msgpack

        // while (!response) {
        //     delete response;
        //     buffer -= 1024;
        //     response = new MsgpackAsyncJsonResponse(true, buffer);
        // }

        for (const auto & emsdevice : EMSESP::emsdevices) {
            if (emsdevice->unique_id() == id) {
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

// renames a device
// takes the unique ID and the custom name
void WebCustomizationService::writeDeviceName(AsyncWebServerRequest * request, JsonVariant json) {
    if (json.is<JsonObject>()) {
        // find the device using the unique_id
        for (const auto & emsdevice : EMSESP::emsdevices) {
            if (emsdevice) {
                uint8_t unique_device_id = json["id"];
                // find product id and device id using the unique id
                if (emsdevice->unique_id() == unique_device_id) {
                    uint8_t product_id  = emsdevice->product_id();
                    uint8_t device_id   = emsdevice->device_id();
                    auto    custom_name = json["name"].as<std::string>();

                    // emsesp::EMSESP::logger().info("Found Device ID: %d, Product ID: %d", device_id, product_id);

                    // updates current record or creates a new one
                    bool entry_exists = false;
                    update([&](WebCustomization & settings) {
                        for (auto it = settings.entityCustomizations.begin(); it != settings.entityCustomizations.end();) {
                            if ((*it).product_id == product_id && (*it).device_id == device_id) {
                                (*it).custom_name = custom_name;
                                entry_exists      = true;
                                break;
                            } else {
                                ++it;
                            }
                        }

                        // if we don't have any customization for this device, create a new entry
                        if (!entry_exists) {
                            EntityCustomization new_entry;
                            new_entry.product_id  = product_id;
                            new_entry.device_id   = device_id;
                            new_entry.custom_name = custom_name;
                            settings.entityCustomizations.push_back(new_entry);
                        }

                        return StateUpdateResult::CHANGED;
                    });

                    // update the EMS Device record real-time
                    emsdevice->custom_name(custom_name);
                }
            }
        }
    }

    AsyncWebServerResponse * response = request->beginResponse(200);
    request->send(response);
}

// takes a list of updated entities with new masks from the web UI
// saves it in the customization service
// and updates the entity list real-time
void WebCustomizationService::customization_entities(AsyncWebServerRequest * request, JsonVariant json) {
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
                            emsdevice->setCustomizationEntity(id_s);
                        }
                    }

                    // add deleted entities from file
                    read([&](WebCustomization & settings) {
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
                    emsdevice->getCustomizationEntities(entity_ids);

                    // Save the list to the customization file
                    update([&](WebCustomization & settings) {
                        // see if we already have a mask list for this device, if so remove the entry
                        for (auto it = settings.entityCustomizations.begin(); it != settings.entityCustomizations.end();) {
                            if ((*it).product_id == product_id && (*it).device_id == device_id) {
                                it = settings.entityCustomizations.erase(it);
                                break;
                            } else {
                                ++it;
                            }
                        }

                        // re-create a new entry
                        EntityCustomization new_entry;
                        new_entry.product_id = product_id;
                        new_entry.device_id  = device_id;
                        new_entry.entity_ids = entity_ids;
                        settings.entityCustomizations.push_back(new_entry);

                        return StateUpdateResult::CHANGED; // save the changes
                    });

                    break;
                }
            }
        }
    }

    AsyncWebServerResponse * response = request->beginResponse(need_reboot ? 205 : 200); // reboot or just OK
    request->send(response);
}

// load the settings when the service starts
void WebCustomizationService::begin() {
    _fsPersistence.readFromFS();
}

// hard coded tests
#ifdef EMSESP_TEST
void WebCustomizationService::test() {
    update([&](WebCustomization & webCustomization) {
        // Temperature sensors
        webCustomization.sensorCustomizations.clear();
        auto sensor   = SensorCustomization();
        sensor.id     = "01-0203-0405-0607";
        sensor.name   = "test_sensor1";
        sensor.offset = 0;
        webCustomization.sensorCustomizations.push_back(sensor);

        auto sensor2   = SensorCustomization();
        sensor2.id     = "0B-0C0D-0E0F-1011";
        sensor2.name   = "test_sensor2";
        sensor2.offset = 4;
        webCustomization.sensorCustomizations.push_back(sensor2);

        // Analog sensors
        // This actually adds the sensors as we use customizations to store them
        webCustomization.analogCustomizations.clear();
        auto analog   = AnalogCustomization();
        analog.gpio   = 36;
        analog.name   = "test_analog1";
        analog.offset = 0;
        analog.factor = 0.1;
        analog.uom    = 17;
        analog.type   = 3;
        webCustomization.analogCustomizations.push_back(analog);

        analog        = AnalogCustomization();
        analog.gpio   = 37;
        analog.name   = "test_analog2";
        analog.offset = 0;
        analog.factor = 1;
        analog.uom    = 0;
        analog.type   = 1;
        webCustomization.analogCustomizations.push_back(analog);

        // EMS entities
        webCustomization.entityCustomizations.clear();
        auto emsEntity        = EntityCustomization();
        emsEntity.product_id  = 123;
        emsEntity.device_id   = 8;
        emsEntity.custom_name = "Custom Name!!";
        emsEntity.entity_ids.push_back("08heatingactive|is my heating on?");
        webCustomization.entityCustomizations.push_back(emsEntity);

        return StateUpdateResult::CHANGED; // persist the changes
    });

    EMSESP::analogsensor_.reload(); // this is needed to active the analog sensors
}
#endif

} // namespace emsesp
