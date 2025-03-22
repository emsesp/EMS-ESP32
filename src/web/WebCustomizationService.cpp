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

#include "emsesp.h"

namespace emsesp {

bool WebCustomization::_start = true;

WebCustomizationService::WebCustomizationService(AsyncWebServer * server, FS * fs, SecurityManager * securityManager)
    : _fsPersistence(WebCustomization::read, WebCustomization::update, this, fs, EMSESP_CUSTOMIZATION_FILE) {
    // GET
    securityManager->addEndpoint(server, EMSESP_DEVICE_ENTITIES_PATH, AuthenticationPredicates::IS_AUTHENTICATED, [this](AsyncWebServerRequest * request) {
        device_entities(request);
    });

    // POST
    securityManager->addEndpoint(
        server,
        EMSESP_RESET_CUSTOMIZATION_SERVICE_PATH,
        AuthenticationPredicates::IS_ADMIN,
        [this](AsyncWebServerRequest * request) { reset_customization(request); },
        HTTP_POST); // force POST

    securityManager->addEndpoint(server,
                                 EMSESP_WRITE_DEVICE_NAME_PATH,
                                 AuthenticationPredicates::IS_AUTHENTICATED,
                                 [this](AsyncWebServerRequest * request, JsonVariant json) { writeDeviceName(request, json); });

    securityManager->addEndpoint(server,
                                 EMSESP_CUSTOMIZATION_ENTITIES_PATH,
                                 AuthenticationPredicates::IS_AUTHENTICATED,
                                 [this](AsyncWebServerRequest * request, JsonVariant json) { customization_entities(request, json); });
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
        for (const std::string & entity_id : entityCustomization.entity_ids) {
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
        auto sensorsJsons = root["ts"].as<JsonArray>();
        for (const JsonObject sensorJson : sensorsJsons) {
            // create each of the sensor, overwriting any previous settings
            auto sensor   = SensorCustomization();
            sensor.id     = sensorJson["id"].as<std::string>();
            sensor.name   = sensorJson["name"].as<std::string>();
            sensor.offset = sensorJson["offset"];
            if (sensor.id == sensor.name) {
                sensor.name = ""; // no need to store id as name
            }
            std::replace(sensor.id.begin(), sensor.id.end(), '-', '_'); // change old ids to v3.7 style
            customizations.sensorCustomizations.push_back(sensor);      // add to list
        }
    }

    // Analog Sensor customization
    customizations.analogCustomizations.clear();
    if (root["as"].is<JsonArray>()) {
        auto analogJsons = root["as"].as<JsonArray>();
        for (const JsonObject analogJson : analogJsons) {
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
        auto masked_entities = root["masked_entities"].as<JsonArray>();
        for (const JsonObject masked_entity : masked_entities) {
            auto emsEntity        = EntityCustomization();
            emsEntity.product_id  = masked_entity["product_id"];
            emsEntity.device_id   = masked_entity["device_id"];
            emsEntity.custom_name = masked_entity["custom_name"] | "";

            auto masked_entity_ids = masked_entity["entity_ids"].as<JsonArray>();
            for (const JsonVariant masked_entity_id : masked_entity_ids) {
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
        emsesp::EMSESP::system_.systemStatus(
            emsesp::SYSTEM_STATUS::SYSTEM_STATUS_PENDING_RESTART); // will be handled by the main loop. We use pending for the Web's SystemMonitor
        return;
    }

    // failed
    AsyncWebServerResponse * response = request->beginResponse(400); // bad request
    request->send(response);
#endif
}

// send back list of device entities
void WebCustomizationService::device_entities(AsyncWebServerRequest * request) {
    uint8_t id;

    // for testing we hardcode the id to 1 - the boiler
#if defined(EMSESP_STANDALONE) && defined(EMSESP_TEST)
    if (1) {
        id = 1;
#else
    if (request->hasParam(F_(id))) {
        id = Helpers::atoint(request->getParam(F_(id))->value().c_str()); // get id from url
#endif

        auto * response = new AsyncMessagePackResponse(true); // array and msgpack

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
#else
                JsonDocument doc;
                JsonArray    output = doc.to<JsonArray>();
                emsdevice->generate_values_web_customization(output);
#endif

#if defined(EMSESP_DEBUG)
                size_t length = response->setLength();
                EMSESP::logger().debug("Customizations buffer used: %d", length);
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
                                for (const std::string & entity_id : entityCustomization.entity_ids) {
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
        sensor.id     = "01_0203_0405_0607";
        sensor.name   = "test_tempsensor1";
        sensor.offset = 0;
        webCustomization.sensorCustomizations.push_back(sensor);

        auto sensor2   = SensorCustomization();
        sensor2.id     = "0B_0C0D_0E0F_1011";
        sensor2.name   = "test_tempsensor2";
        sensor2.offset = 4;
        webCustomization.sensorCustomizations.push_back(sensor2);

        // Analog sensors
        // This actually adds the sensors as we use customizations to store them
        webCustomization.analogCustomizations.clear();
        auto analog   = AnalogCustomization();
        analog.gpio   = 36;
        analog.name   = "test_analogsensor1";
        analog.offset = 0;
        analog.factor = 0.1;
        analog.uom    = 17;
        analog.type   = 3; // ADC
        webCustomization.analogCustomizations.push_back(analog);

        analog        = AnalogCustomization();
        analog.gpio   = 37;
        analog.name   = "test_analogsensor2";
        analog.offset = 0;
        analog.factor = 1;
        analog.uom    = 0;
        analog.type   = 1; // DIGITAL_IN
        webCustomization.analogCustomizations.push_back(analog);

        analog        = AnalogCustomization();
        analog.gpio   = 38;
        analog.name   = "test_analogsensor3";
        analog.offset = 0;
        analog.factor = 1;
        analog.uom    = 0;
        analog.type   = 0; // disabled, not-used
        webCustomization.analogCustomizations.push_back(analog);

        analog        = AnalogCustomization();
        analog.gpio   = 33;
        analog.name   = "test_analogsensor4";
        analog.offset = 0;
        analog.factor = 1;
        analog.uom    = 0;
        analog.type   = 2; // COUNTER
        webCustomization.analogCustomizations.push_back(analog);

        // EMS entities, mark some as favorites
        webCustomization.entityCustomizations.clear();
        auto emsEntity        = EntityCustomization();
        emsEntity.product_id  = 123;
        emsEntity.device_id   = 8;
        emsEntity.custom_name = "My Custom Boiler";
        emsEntity.entity_ids.push_back("08heatingactive|is my heating on?");
        emsEntity.entity_ids.push_back("08tapwateractive");
        emsEntity.entity_ids.push_back("08selflowtemp|<90");
        webCustomization.entityCustomizations.push_back(emsEntity);

        // since custom device name is loaded at discovery, we need to force it here
        for (const auto & emsdevice : EMSESP::emsdevices) {
            if (emsdevice->is_device_id(emsEntity.device_id)) {
                emsdevice->custom_name(emsEntity.custom_name);
                break;
            }
        }

        // ...and the same with the custom masks and names for entity values. It's done in EMSdevice::add_device_value()
        // so we need to force it here
        for (const auto & emsdevice : EMSESP::emsdevices) {
            if (emsdevice->is_device_id(emsEntity.device_id)) {
                // find the device value and set the mask and custom name to match the above fake data
                for (auto & dv : emsdevice->devicevalues_) {
                    if (strcmp(dv.short_name, "heatingactive") == 0) {
                        dv.state           = DeviceValueState::DV_FAVORITE; // set as favorite
                        dv.custom_fullname = "is my heating on?";
                    } else if (strcmp(dv.short_name, "tapwateractive") == 0) {
                        dv.state = DeviceValueState::DV_FAVORITE; // set as favorite
                    } else if (strcmp(dv.short_name, "selflowtemp") == 0) {
                        dv.state = DeviceValueState::DV_FAVORITE; // set as favorite
                    }
                }
                break;
            }
        }

        return StateUpdateResult::CHANGED; // persist the changes
    });

    EMSESP::analogsensor_.reload(); // this is needed to active the analog sensors
}
#endif

} // namespace emsesp
