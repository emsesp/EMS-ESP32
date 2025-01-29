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

WebDataService::WebDataService(AsyncWebServer * server, SecurityManager * securityManager) {
    // write endpoints - POSTs
    securityManager->addEndpoint(server,
                                 EMSESP_WRITE_DEVICE_VALUE_SERVICE_PATH,
                                 AuthenticationPredicates::IS_ADMIN,
                                 [this](AsyncWebServerRequest * request, JsonVariant json) { write_device_value(request, json); });

    securityManager->addEndpoint(server,
                                 EMSESP_WRITE_TEMPERATURE_SENSOR_SERVICE_PATH,
                                 AuthenticationPredicates::IS_ADMIN,
                                 [this](AsyncWebServerRequest * request, JsonVariant json) { write_temperature_sensor(request, json); });

    securityManager->addEndpoint(server,
                                 EMSESP_WRITE_ANALOG_SENSOR_SERVICE_PATH,
                                 AuthenticationPredicates::IS_ADMIN,
                                 [this](AsyncWebServerRequest * request, JsonVariant json) { write_analog_sensor(request, json); });

    // GET's
    securityManager->addEndpoint(server, EMSESP_DEVICE_DATA_SERVICE_PATH, AuthenticationPredicates::IS_AUTHENTICATED, [this](AsyncWebServerRequest * request) {
        device_data(request);
    });

    securityManager->addEndpoint(server, EMSESP_CORE_DATA_SERVICE_PATH, AuthenticationPredicates::IS_AUTHENTICATED, [this](AsyncWebServerRequest * request) {
        core_data(request);
    });

    securityManager->addEndpoint(server, EMSESP_SENSOR_DATA_SERVICE_PATH, AuthenticationPredicates::IS_AUTHENTICATED, [this](AsyncWebServerRequest * request) {
        sensor_data(request);
    });

    securityManager->addEndpoint(server, EMSESP_DASHBOARD_DATA_SERVICE_PATH, AuthenticationPredicates::IS_AUTHENTICATED, [this](AsyncWebServerRequest * request) {
        dashboard_data(request);
    });
}

// this is used in the Devices page and contains all EMS device information
// /coreData endpoint
void WebDataService::core_data(AsyncWebServerRequest * request) {
    auto *     response = new AsyncJsonResponse(false);
    JsonObject root     = response->getRoot();

    // list is already sorted by device type
    JsonArray devices = root["devices"].to<JsonArray>();
    for (const auto & emsdevice : EMSESP::emsdevices) {
        // ignore controller
        if (emsdevice && (emsdevice->device_type() != EMSdevice::DeviceType::CONTROLLER || emsdevice->count_entities() > 0)) {
            JsonObject obj = devices.add<JsonObject>();
            obj["id"]      = emsdevice->unique_id();                            // a unique id
            obj["tn"]      = emsdevice->device_type_2_device_name_translated(); // translated device type name
            obj["t"]       = emsdevice->device_type();                          // device type number
            obj["b"]       = emsdevice->brand_to_char();                        // brand
            obj["n"]       = emsdevice->name();                                 // custom name
            obj["d"]       = emsdevice->device_id();                            // deviceid
            obj["p"]       = emsdevice->product_id();                           // productid
            obj["v"]       = emsdevice->version();                              // version
            obj["e"]       = emsdevice->count_entities();                       // number of entities
            obj["url"]     = emsdevice->device_type_name();                     // non-translated, lower-case, used for API URL in customization page
        }
    }

    // add any custom entities
    uint8_t customEntities = EMSESP::webCustomEntityService.count_entities();
    if (customEntities) {
        JsonObject obj = devices.add<JsonObject>();
        obj["id"]      = EMSdevice::DeviceTypeUniqueID::CUSTOM_UID;
        obj["tn"]      = Helpers::translated_word(FL_(custom_device));      // translated device type name
        obj["t"]       = EMSdevice::DeviceType::CUSTOM;                     // device type number
        obj["b"]       = Helpers::translated_word(FL_(na));                 // brand
        obj["n"]       = Helpers::translated_word(FL_(custom_device_name)); // name
        obj["d"]       = 0;                                                 // deviceid
        obj["p"]       = 0;                                                 // productid
        obj["v"]       = 0;                                                 // version
        obj["e"]       = customEntities;                                    // number of custom entities
    }

    root["connected"] = EMSESP::bus_status() != 2;

    response->setLength();
    request->send(response);
}

// sensor data - sends back to web
// /sensorData endpoint
void WebDataService::sensor_data(AsyncWebServerRequest * request) {
    auto *     response = new AsyncJsonResponse(false);
    JsonObject root     = response->getRoot();

    // temperature sensors
    JsonArray sensors = root["ts"].to<JsonArray>();
    if (EMSESP::temperaturesensor_.have_sensors()) {
        for (const auto & sensor : EMSESP::temperaturesensor_.sensors()) {
            JsonObject obj = sensors.add<JsonObject>();
            obj["id"]      = sensor.id();   // id as string
            obj["n"]       = sensor.name(); // name
            if (EMSESP::system_.fahrenheit()) {
                if (Helpers::hasValue(sensor.temperature_c)) {
                    obj["t"] = (float)sensor.temperature_c * 0.18 + 32;
                }
                obj["u"] = DeviceValueUOM::FAHRENHEIT;
                obj["o"] = (float)sensor.offset() * 0.18;
            } else {
                if (Helpers::hasValue(sensor.temperature_c)) {
                    obj["t"] = (float)sensor.temperature_c / 10;
                }
                obj["u"] = DeviceValueUOM::DEGREES;
                obj["o"] = (float)(sensor.offset()) / 10;
            }
        }
    }

    // analog sensors
    JsonArray analogs = root["as"].to<JsonArray>();
    if (EMSESP::analog_enabled() && EMSESP::analogsensor_.have_sensors()) {
        uint8_t count = 0;
        for (const auto & sensor : EMSESP::analogsensor_.sensors()) {
            JsonObject obj = analogs.add<JsonObject>();
            obj["id"]      = ++count; // needed for sorting table
            obj["g"]       = sensor.gpio();
            obj["n"]       = sensor.name();
            obj["u"]       = sensor.uom();
            obj["o"]       = sensor.offset();
            obj["f"]       = sensor.factor();
            obj["t"]       = sensor.type();

            if (sensor.type() != AnalogSensor::AnalogType::NOTUSED) {
                obj["v"] = Helpers::transformNumFloat(sensor.value()); // is optional and is a float
            } else {
                obj["v"] = 0; // must have a value for web sorting to work
            }
        }
    }

    root["analog_enabled"] = EMSESP::analog_enabled();
    root["platform"]       = EMSESP_PLATFORM;

    response->setLength();
    request->send(response);
}

// The unique_id is the unique record ID from the Web table to identify which device to load
// endpoint /rest/deviceData?id=n
// Compresses the JSON using MsgPack https://msgpack.org/index.html
void WebDataService::device_data(AsyncWebServerRequest * request) {
    uint8_t id;
    if (request->hasParam(F_(id))) {
        id = Helpers::atoint(request->getParam(F_(id))->value().c_str()); // get id from url

        auto * response = new AsyncMessagePackResponse();

        // check size
        // while (!response) {
        //     delete response;
        //     buffer -= 1024;
        //     response = new MsgpackAsyncJsonResponse(false, buffer);
        // }

        for (const auto & emsdevice : EMSESP::emsdevices) {
            if (emsdevice->unique_id() == id) {
                // wait max 2.5 sec for updated data (post_send_delay is 2 sec)
                for (uint16_t i = 0; i < (emsesp::TxService::POST_SEND_DELAY + 500) && EMSESP::wait_validate(); i++) {
                    delay(1);
                }
                EMSESP::wait_validate(0); // reset in case of timeout
#ifndef EMSESP_STANDALONE
                JsonObject output = response->getRoot();
                emsdevice->generate_values_web(output);
#endif

#if defined(EMSESP_DEBUG)
                size_t length = response->setLength();
                EMSESP::logger().debug("Dashboard buffer used: %d", length);
#else
                response->setLength();
#endif
                request->send(response);
                return;
            }
        }

#ifndef EMSESP_STANDALONE
        if (id == EMSdevice::DeviceTypeUniqueID::CUSTOM_UID) {
            JsonObject output = response->getRoot();
            EMSESP::webCustomEntityService.generate_value_web(output);
            response->setLength();
            request->send(response);
            return;
        }
#endif
    }

    // invalid
    AsyncWebServerResponse * response = request->beginResponse(400); // bad request
    request->send(response);
}

// assumes the service has been checked for admin authentication
void WebDataService::write_device_value(AsyncWebServerRequest * request, JsonVariant json) {
    if (json.is<JsonObject>()) {
        uint8_t      unique_id = json["id"]; // unique ID
        const char * cmd       = json["c"];  // the command
        JsonVariant  data      = json["v"];  // the value in any format

        // quit on bad values
        if (strlen(cmd) == 0 || data.isNull()) {
            AsyncWebServerResponse * response = request->beginResponse(400); // bad request
            request->send(response);
            return;
        }

        // using the unique ID from the web find the real device type
        uint8_t device_type = EMSdevice::DeviceType::UNKNOWN;
        switch (unique_id) {
        case EMSdevice::DeviceTypeUniqueID::CUSTOM_UID:
            device_type = EMSdevice::DeviceType::CUSTOM;
            break;
        case EMSdevice::DeviceTypeUniqueID::SCHEDULER_UID:
            device_type = EMSdevice::DeviceType::SCHEDULER;
            break;
        case EMSdevice::DeviceTypeUniqueID::TEMPERATURESENSOR_UID:
            device_type = EMSdevice::DeviceType::TEMPERATURESENSOR;
            break;
        case EMSdevice::DeviceTypeUniqueID::ANALOGSENSOR_UID:
            device_type = EMSdevice::DeviceType::ANALOGSENSOR;
            break;
        default:
            for (const auto & emsdevice : EMSESP::emsdevices) {
                if (emsdevice->unique_id() == unique_id) {
                    device_type = emsdevice->device_type();
                    break;
                }
            }
            break;
        }
        if (device_type == EMSdevice::DeviceType::UNKNOWN) {
            EMSESP::logger().warning("Write command failed, bad device id: %d", unique_id);
            AsyncWebServerResponse * response = request->beginResponse(400); // bad request
            request->send(response);
            return;
        }
        // create JSON for output
        auto *     response = new AsyncJsonResponse(false);
        JsonObject output   = response->getRoot();
        // the data could be in any format, but we need string
        // authenticated is always true
        uint8_t return_code = CommandRet::NOT_FOUND;
        // parse the command as it could have a hc or dhw prefixed, e.g. hc2/seltemp
        int8_t id = -1; // default
        if (device_type >= EMSdevice::DeviceType::BOILER) {
            cmd = Command::parse_command_string(cmd, id); // extract hc or dhw
        }
        if (data.is<const char *>()) {
            return_code = Command::call(device_type, cmd, data.as<const char *>(), true, id, output);
        } else if (data.is<int>()) {
            char s[20];
            return_code = Command::call(device_type, cmd, Helpers::render_value(s, data.as<int32_t>(), 0), true, id, output);
        } else if (data.is<float>()) {
            char s[20];
            return_code = Command::call(device_type, cmd, Helpers::render_value(s, data.as<float>(), 1), true, id, output);
        } else if (data.is<bool>()) {
            return_code = Command::call(device_type, cmd, data.as<bool>() ? "true" : "false", true, id, output);
        }

        // write log
        if (return_code != CommandRet::OK) {
            // is already logged by command and message contains code
            // EMSESP::logger().err("Write command failed %s (%s)", (const char *)output["message"], Command::return_code_string(return_code));
        } else {
#if defined(EMSESP_DEBUG)
            EMSESP::logger().debug("Write command successful");
#endif
        }

        response->setCode((return_code == CommandRet::OK) ? 200 : 400); // bad request
        response->setLength();
        request->send(response);
        return;
    }

    EMSESP::logger().warning("Write command failed, bad json");

    // if we reach here, fail
    AsyncWebServerResponse * response = request->beginResponse(400); // bad request
    request->send(response);
}

// takes a temperaturesensor name and optional offset from the WebUI and update the customization settings
// via the temperaturesensor service
void WebDataService::write_temperature_sensor(AsyncWebServerRequest * request, JsonVariant json) {
    bool ok = false;
    if (json.is<JsonObject>()) {
        JsonObject sensor = json;

        std::string id   = sensor["id"]; // this is the key
        std::string name = sensor["name"];

        // calculate offset. We'll convert it to an int and * 10
        float   offset   = sensor["offset"];
        int16_t offset10 = offset * 10;
        if (EMSESP::system_.fahrenheit()) {
            offset10 = offset / 0.18;
        }

        ok = EMSESP::temperaturesensor_.update(id, name, offset10);
    }

    AsyncWebServerResponse * response = request->beginResponse(ok ? 200 : 400); // bad request
    request->send(response);
}

// update the analog record, or create a new one
void WebDataService::write_analog_sensor(AsyncWebServerRequest * request, JsonVariant json) {
    bool ok = false;
    if (json.is<JsonObject>()) {
        JsonObject analog = json;

        uint8_t     gpio    = analog["gpio"];
        std::string name    = analog["name"];
        double      factor  = analog["factor"];
        double      offset  = analog["offset"];
        uint8_t     uom     = analog["uom"];
        int8_t      type    = analog["type"];
        bool        deleted = analog["deleted"];
        ok                  = EMSESP::analogsensor_.update(gpio, name, offset, factor, uom, type, deleted);
    }

    AsyncWebServerResponse * response = request->beginResponse(ok ? 200 : 400); // ok or bad request
    request->send(response);
}

// this is used in the dashboard and contains all ems device information
// /dashboardData endpoint
void WebDataService::dashboard_data(AsyncWebServerRequest * request) {
    auto * response = new AsyncMessagePackResponse();

#if defined(EMSESP_STANDALONE)
    JsonDocument doc;
    JsonObject   root = doc.to<JsonObject>();
#else
    JsonObject root = response->getRoot();
#endif

    // add state of EMS bus
    root["connected"] = EMSESP::bus_status() != 2;

    // add all the data
    JsonArray nodes = root["nodes"].to<JsonArray>();

    // first fetch all the recognized devices
    for (const auto & emsdevice : EMSESP::emsdevices) {
        if (emsdevice->count_entities_fav()) {
            JsonObject obj = nodes.add<JsonObject>();
            obj["id"]      = emsdevice->unique_id();   // it's unique id
            obj["n"]       = emsdevice->name();        // custom name
            obj["t"]       = emsdevice->device_type(); // device type number
            emsdevice->generate_values_web(obj, true); // is_dashboard = true
        }
    }

    // add custom entities, if we have any
    if (EMSESP::webCustomEntityService.count_entities()) {
        JsonObject obj = nodes.add<JsonObject>();
        obj["id"]      = EMSdevice::DeviceTypeUniqueID::CUSTOM_UID; // it's unique id
        obj["t"]       = EMSdevice::DeviceType::CUSTOM;             // device type number
        EMSESP::webCustomEntityService.generate_value_web(obj, true);
    }

    // add temperature sensors, if we have any
    if (EMSESP::temperaturesensor_.have_sensors()) {
        JsonObject obj  = nodes.add<JsonObject>();
        obj["id"]       = EMSdevice::DeviceTypeUniqueID::TEMPERATURESENSOR_UID; // it's unique id
        obj["t"]        = EMSdevice::DeviceType::TEMPERATURESENSOR;             // device type number
        JsonArray nodes = obj["nodes"].to<JsonArray>();
        uint8_t   count = 0;
        for (const auto & sensor : EMSESP::temperaturesensor_.sensors()) {
            JsonObject node = nodes.add<JsonObject>();
            node["id"]      = (EMSdevice::DeviceTypeUniqueID::TEMPERATURESENSOR_UID * 100) + count++;

            JsonObject dv = node["dv"].to<JsonObject>();
            dv["id"]      = "00" + sensor.name();
            if (EMSESP::system_.fahrenheit()) {
                if (Helpers::hasValue(sensor.temperature_c)) {
                    dv["v"] = (float)sensor.temperature_c * 0.18 + 32;
                }
                dv["u"] = DeviceValueUOM::FAHRENHEIT;
            } else {
                if (Helpers::hasValue(sensor.temperature_c)) {
                    dv["v"] = (float)sensor.temperature_c / 10;
                }
                dv["u"] = DeviceValueUOM::DEGREES;
            }
        }
    }

    // add analog sensors, count excludes disabled entries
    if (EMSESP::analog_enabled() && EMSESP::analogsensor_.count_entities(false)) {
        JsonObject obj  = nodes.add<JsonObject>();
        obj["id"]       = EMSdevice::DeviceTypeUniqueID::ANALOGSENSOR_UID; // it's unique id
        obj["t"]        = EMSdevice::DeviceType::ANALOGSENSOR;             // device type number
        JsonArray nodes = obj["nodes"].to<JsonArray>();
        uint8_t   count = 0;
        for (const auto & sensor : EMSESP::analogsensor_.sensors()) {
            if (sensor.type() != AnalogSensor::AnalogType::NOTUSED) { // ignore disabled
                JsonObject node = nodes.add<JsonObject>();
                node["id"]      = (EMSdevice::DeviceTypeUniqueID::ANALOGSENSOR_UID * 100) + count++;

                JsonObject dv = node["dv"].to<JsonObject>();
                dv["id"]      = "00" + sensor.name();
#if CONFIG_IDF_TARGET_ESP32
                if (sensor.type() == AnalogSensor::AnalogType::DIGITAL_OUT && (sensor.gpio() == 25 || sensor.gpio() == 26)) {
                    obj["v"] = Helpers::transformNumFloat(sensor.value());
                } else
#elif CONFIG_IDF_TARGET_ESP32S2
                if (sensor.type() == AnalogSensor::AnalogType::DIGITAL_OUT && (sensor.gpio() == 17 || sensor.gpio() == 18)) {
                    obj["v"] = Helpers::transformNumFloat(sensor.value());
                } else
#endif
                    if (sensor.type() == AnalogSensor::AnalogType::DIGITAL_OUT || sensor.type() == AnalogSensor::AnalogType::DIGITAL_IN) {
                    char s[12];
                    dv["v"]     = Helpers::render_boolean(s, sensor.value() != 0, true);
                    JsonArray l = dv["l"].to<JsonArray>();
                    l.add(Helpers::render_boolean(s, false, true));
                    l.add(Helpers::render_boolean(s, true, true));
                } else {
                    dv["v"] = Helpers::transformNumFloat(sensor.value());
                    dv["u"] = sensor.uom();
                }
                if (sensor.type() == AnalogSensor::AnalogType::COUNTER || sensor.type() >= AnalogSensor::AnalogType::DIGITAL_OUT) {
                    dv["c"] = sensor.name();
                }
            }
        }
    }

    // show scheduler, with name, on/off
    if (EMSESP::webSchedulerService.count_entities(true)) {
        JsonObject obj  = nodes.add<JsonObject>();
        obj["id"]       = EMSdevice::DeviceTypeUniqueID::SCHEDULER_UID; // it's unique id
        obj["t"]        = EMSdevice::DeviceType::SCHEDULER;             // device type number
        JsonArray nodes = obj["nodes"].to<JsonArray>();
        uint8_t   count = 0;

        EMSESP::webSchedulerService.read([&](const WebScheduler & webScheduler) {
            for (const ScheduleItem & scheduleItem : webScheduler.scheduleItems) {
                // only add if we have a name - we don't need a u (UOM) for this
                if (!scheduleItem.name.empty()) {
                    JsonObject node = nodes.add<JsonObject>();
                    node["id"]      = (EMSdevice::DeviceTypeUniqueID::SCHEDULER_UID * 100) + count++;

                    JsonObject dv = node["dv"].to<JsonObject>();
                    dv["id"]      = "00" + scheduleItem.name;
                    dv["c"]       = scheduleItem.name;
                    char s[12];
                    dv["v"]     = Helpers::render_boolean(s, scheduleItem.active, true);
                    JsonArray l = dv["l"].to<JsonArray>();
                    l.add(Helpers::render_boolean(s, false, true));
                    l.add(Helpers::render_boolean(s, true, true));
                }
            }
        });
    }

#if defined(EMSESP_TEST) && defined(EMSESP_STANDALONE)
    Serial.println();
    Serial.print("All dashboard_data: ");
    serializeJson(root, Serial);
    Serial.println();
#endif

    response->setLength();
    request->send(response);
}

} // namespace emsesp