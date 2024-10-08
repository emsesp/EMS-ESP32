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
    // write endpoints
    server->on(EMSESP_WRITE_DEVICE_VALUE_SERVICE_PATH,
               securityManager->wrapCallback([this](AsyncWebServerRequest * request, JsonVariant json) { write_device_value(request, json); },
                                             AuthenticationPredicates::IS_ADMIN));
    server->on(EMSESP_WRITE_TEMPERATURE_SENSOR_SERVICE_PATH,
               securityManager->wrapCallback([this](AsyncWebServerRequest * request, JsonVariant json) { write_temperature_sensor(request, json); },
                                             AuthenticationPredicates::IS_ADMIN));
    server->on(EMSESP_WRITE_ANALOG_SENSOR_SERVICE_PATH,
               securityManager->wrapCallback([this](AsyncWebServerRequest * request, JsonVariant json) { write_analog_sensor(request, json); },
                                             AuthenticationPredicates::IS_ADMIN));
    // GET's
    server->on(EMSESP_DEVICE_DATA_SERVICE_PATH,
               HTTP_GET,
               securityManager->wrapRequest([this](AsyncWebServerRequest * request) { device_data(request); }, AuthenticationPredicates::IS_AUTHENTICATED));

    server->on(EMSESP_CORE_DATA_SERVICE_PATH,
               HTTP_GET,
               securityManager->wrapRequest([this](AsyncWebServerRequest * request) { core_data(request); }, AuthenticationPredicates::IS_AUTHENTICATED));

    server->on(EMSESP_SENSOR_DATA_SERVICE_PATH,
               HTTP_GET,
               securityManager->wrapRequest([this](AsyncWebServerRequest * request) { sensor_data(request); }, AuthenticationPredicates::IS_AUTHENTICATED));

    server->on(EMSESP_DASHBOARD_DATA_SERVICE_PATH,
               HTTP_GET,
               securityManager->wrapRequest([this](AsyncWebServerRequest * request) { dashboard_data(request); }, AuthenticationPredicates::IS_AUTHENTICATED));
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
        obj["id"]      = 99;                                                // the last unique id
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
                obj["v"] = Helpers::transformNumFloat(sensor.value(), 0); // is optional and is a float
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

        auto * response = new AsyncJsonResponse(false, true); // use msgPack

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
        if (id == 99) {
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
        for (const auto & emsdevice : EMSESP::emsdevices) {
            if (emsdevice->unique_id() == unique_id) {
                // create JSON for output
                auto *     response = new AsyncJsonResponse(false);
                JsonObject output   = response->getRoot();

                // the data could be in any format, but we need string
                // authenticated is always true
                uint8_t return_code = CommandRet::NOT_FOUND;
                uint8_t device_type = emsdevice->device_type();
                // parse the command as it could have a hc or dhw prefixed, e.g. hc2/seltemp
                int8_t id = -1; // default
                if (device_type >= EMSdevice::DeviceType::BOILER) {
                    cmd = Command::parse_command_string(cmd, id); // extract hc or dhw
                }
                if (data.is<const char *>()) {
                    return_code = Command::call(device_type, cmd, data.as<const char *>(), true, id, output);
                } else if (data.is<int>()) {
                    char s[10];
                    return_code = Command::call(device_type, cmd, Helpers::render_value(s, data.as<int32_t>(), 0), true, id, output);
                } else if (data.is<float>()) {
                    char s[10];
                    return_code = Command::call(device_type, cmd, Helpers::render_value(s, data.as<float>(), 1), true, id, output);
                } else if (data.is<bool>()) {
                    return_code = Command::call(device_type, cmd, data.as<bool>() ? "true" : "false", true, id, output);
                }

                // write log
                if (return_code != CommandRet::OK) {
                    EMSESP::logger().err("Write command failed %s (%s)", (const char *)output["message"], Command::return_code_string(return_code));
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
        }

        // special check for custom entities (which have a unique id of 99)
        if (unique_id == 99) {
            auto *     response    = new AsyncJsonResponse(false);
            JsonObject output      = response->getRoot();
            uint8_t    return_code = CommandRet::NOT_FOUND;
            uint8_t    device_type = EMSdevice::DeviceType::CUSTOM;
            // parse the command as it could have a hc or dhw prefixed, e.g. hc2/seltemp
            int8_t id = -1;
            if (device_type >= EMSdevice::DeviceType::BOILER) {
                cmd = Command::parse_command_string(cmd, id); // extract hc or dhw
            }
            if (data.is<const char *>()) {
                return_code = Command::call(device_type, cmd, data.as<const char *>(), true, id, output);
            } else if (data.is<int>()) {
                char s[10];
                return_code = Command::call(device_type, cmd, Helpers::render_value(s, data.as<int32_t>(), 0), true, id, output);
            } else if (data.is<float>()) {
                char s[10];
                return_code = Command::call(device_type, cmd, Helpers::render_value(s, data.as<float>(), 1), true, id, output);
            }
            if (return_code != CommandRet::OK) {
                EMSESP::logger().err("Write command failed %s (%s)", (const char *)output["message"], Command::return_code_string(return_code));
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
    }

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
    auto * response = new AsyncJsonResponse(true, true); // its an Array and also msgpack'd

#if defined(EMSESP_STANDALONE)
    JsonDocument doc;
    JsonArray    root = doc.to<JsonArray>();
#else
    JsonArray root = response->getRoot();
#endif

    for (const auto & emsdevice : EMSESP::emsdevices) {
        if (emsdevice->count_entities_fav()) {
            JsonObject obj = root.add<JsonObject>();
            obj["id"]      = emsdevice->unique_id();   // it's unique id
            obj["n"]       = emsdevice->name();        // custom name
            obj["t"]       = emsdevice->device_type(); // device type number
            emsdevice->generate_values_web(obj, true); // is_dashboard = true
        }
    }

    // add custom entities, if we have any
    if (EMSESP::webCustomEntityService.count_entities()) {
        JsonObject obj = root.add<JsonObject>();
        obj["id"]      = 99;                                                // it's unique id
        obj["n"]       = Helpers::translated_word(FL_(custom_device_name)); // custom name
        obj["t"]       = EMSdevice::DeviceType::CUSTOM;                     // device type number
        EMSESP::webCustomEntityService.generate_value_web(obj, true);
    }

    // add temperature sensors

    // add analog sensors

    // show scheduler, with name, on/off - and pencil edit

#if defined(EMSESP_TEST) && defined(EMSESP_STANDALONE)
    Serial.println();
    Serial.print("ALL dashboard_data: ");
    serializeJsonPretty(root, Serial);
    Serial.println();
#endif

    response->setLength();
    request->send(response);
}


} // namespace emsesp