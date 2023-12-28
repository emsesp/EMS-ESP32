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

WebDataService::WebDataService(PsychicHttpServer * server, SecurityManager * securityManager)
    : _server(server)
    , _securityManager(securityManager) {
}

void WebDataService::registerURI() {
    // GET's
    _server->on(CORE_DATA_SERVICE_PATH,
                HTTP_GET,
                _securityManager->wrapRequest(std::bind(&WebDataService::core_data, this, _1), AuthenticationPredicates::IS_AUTHENTICATED));

    _server->on(DEVICE_DATA_SERVICE_PATH,
                HTTP_GET,
                _securityManager->wrapRequest(std::bind(&WebDataService::device_data, this, _1), AuthenticationPredicates::IS_AUTHENTICATED));

    _server->on(SENSOR_DATA_SERVICE_PATH,
                HTTP_GET,
                _securityManager->wrapRequest(std::bind(&WebDataService::sensor_data, this, _1), AuthenticationPredicates::IS_AUTHENTICATED));

    // POST's
    _server->on(SCAN_DEVICES_SERVICE_PATH,
                HTTP_POST,
                _securityManager->wrapRequest(std::bind(&WebDataService::scan_devices, this, _1), AuthenticationPredicates::IS_ADMIN));

    _server->on(WRITE_DEVICE_VALUE_SERVICE_PATH,
                HTTP_POST,
                _securityManager->wrapCallback(std::bind(&WebDataService::write_device_value, this, _1, _2), AuthenticationPredicates::IS_AUTHENTICATED));

    _server->on(WRITE_TEMPERATURE_SENSOR_SERVICE_PATH,
                HTTP_POST,
                _securityManager->wrapCallback(std::bind(&WebDataService::write_temperature_sensor, this, _1, _2), AuthenticationPredicates::IS_AUTHENTICATED));

    _server->on(WRITE_ANALOG_SENSOR_SERVICE_PATH,
                HTTP_POST,
                _securityManager->wrapCallback(std::bind(&WebDataService::write_analog_sensor, this, _1, _2), AuthenticationPredicates::IS_AUTHENTICATED));
}


// scan devices service
esp_err_t WebDataService::scan_devices(PsychicRequest * request) {
    EMSESP::logger().info("Scanning devices...");
    EMSESP::scan_devices();
    return request->reply(200);
}

// this is used in the dashboard and contains all ems device information
// /coreData endpoint
esp_err_t WebDataService::core_data(PsychicRequest * request) {
    PsychicJsonResponse response = PsychicJsonResponse(request, false, EMSESP_JSON_SIZE_XXLARGE);
    JsonObject          root     = response.getRoot();

    // list is already sorted by device type
    JsonArray devices = root.createNestedArray("devices");
    for (const auto & emsdevice : EMSESP::emsdevices) {
        // ignore controller
        if (emsdevice && (emsdevice->device_type() != EMSdevice::DeviceType::CONTROLLER || emsdevice->count_entities() > 0)) {
            JsonObject obj = devices.createNestedObject();
            obj["id"]      = emsdevice->unique_id();                            // a unique id
            obj["tn"]      = emsdevice->device_type_2_device_name_translated(); // translated device type name
            obj["t"]       = emsdevice->device_type();                          // device type number
            obj["b"]       = emsdevice->brand_to_char();                        // brand
            obj["n"]       = emsdevice->name();                                 // name
            obj["d"]       = emsdevice->device_id();                            // deviceid
            obj["p"]       = emsdevice->product_id();                           // productid
            obj["v"]       = emsdevice->version();                              // version
            obj["e"]       = emsdevice->count_entities();                       // number of entities
        }
    }

    // add any custom entities
    uint8_t customEntities = EMSESP::webCustomEntityService.count_entities();
    if (customEntities) {
        JsonObject obj = devices.createNestedObject();
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

    return response.send();
}

// sensor data - sends back to web
// /sensorData endpoint
esp_err_t WebDataService::sensor_data(PsychicRequest * request) {
    PsychicJsonResponse response = PsychicJsonResponse(request, false, EMSESP_JSON_SIZE_XXLARGE);
    JsonObject          root     = response.getRoot();

    // temperature sensors
    JsonArray sensors = root.createNestedArray("ts");
    if (EMSESP::temperaturesensor_.have_sensors()) {
        for (const auto & sensor : EMSESP::temperaturesensor_.sensors()) {
            JsonObject obj = sensors.createNestedObject();
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
    JsonArray analogs = root.createNestedArray("as");
    if (EMSESP::analog_enabled() && EMSESP::analogsensor_.have_sensors()) {
        uint8_t count = 0;
        for (const auto & sensor : EMSESP::analogsensor_.sensors()) {
            JsonObject obj = analogs.createNestedObject();
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

    return response.send();
}

// The unique_id is the unique record ID from the Web table to identify which device to load
// Compresses the JSON using MsgPack https://msgpack.org/index.html
esp_err_t WebDataService::device_data(PsychicRequest * request) {
    uint8_t id;
    if (request->hasParam(F_(id))) {
        id = Helpers::atoint(request->getParam(F_(id))->value().c_str()); // get id from url

        PsychicJsonResponse response = PsychicJsonResponse(request, false, EMSESP_JSON_SIZE_XXXXLARGE, true); // is jsonobject and also msgpack
        JsonObject          output   = response.getRoot();

        // TODO add back memory managegement. Be careful we do need to free()/delete() any object we extend with new()
        // check size
        // while (!response) {
        //     delete response;
        //     buffer -= 1024;
        //     response = new MsgpackAsyncJsonResponse(false, buffer);
        // }

        for (const auto & emsdevice : EMSESP::emsdevices) {
            if (emsdevice->unique_id() == id) {
                // wait max 2.5 sec for updated data (post_send_delay is 2 sec)
                for (uint16_t i = 0; i < (TxService::POST_SEND_DELAY + 500) && EMSESP::wait_validate(); i++) {
                    delay(1);
                }
                EMSESP::wait_validate(0); // reset in case of timeout
#ifndef EMSESP_STANDALONE
                emsdevice->generate_values_web(output);
#endif

                return response.send();
            }
        }

#ifndef EMSESP_STANDALONE
        if (id == 99) {
            EMSESP::webCustomEntityService.generate_value_web(output);
            return response.send();
        }
#endif
    }

    return request->reply(400); // Invalid/bad request
}

// assumes the service has been checked for admin authentication
esp_err_t WebDataService::write_device_value(PsychicRequest * request, JsonVariant & json) {
    if (json.is<JsonObject>()) {
        uint8_t      unique_id = json["id"]; // unique ID
        const char * cmd       = json["c"];  // the command
        JsonVariant  data      = json["v"];  // the value in any format

        // quit on bad values
        if (strlen(cmd) == 0 || data.isNull()) {
            return request->reply(400); // Invalid/bad request
        }

        // using the unique ID from the web find the real device type
        for (const auto & emsdevice : EMSESP::emsdevices) {
            if (emsdevice->unique_id() == unique_id) {
                // parse the command as it could have a hc or wwc prefixed, e.g. hc2/seltemp
                int8_t id = -1;                                     // default
                cmd       = Command::parse_command_string(cmd, id); // extract hc or wwc

                // create JSON for output
                PsychicJsonResponse response = PsychicJsonResponse(request, false, EMSESP_JSON_SIZE_SMALL);
                JsonObject          output   = response.getRoot();

                // the data could be in any format, but we need string
                // authenticated is always true
                uint8_t return_code = CommandRet::NOT_FOUND;
                uint8_t device_type = emsdevice->device_type();
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

                // write debug
                if (return_code != CommandRet::OK) {
                    EMSESP::logger().err("Write command failed %s (%s)", (const char *)output["message"], Command::return_code_string(return_code).c_str());
                } else {
#if defined(EMSESP_DEBUG)
                    EMSESP::logger().debug("Write command successful");
#endif
                }

                response.setCode((return_code == CommandRet::OK) ? 200 : 400); // bad request
                return response.send();
            }
        }

        // special check for custom entities (which have a unique id of 99)
        if (unique_id == 99) {
            // parse the command as it could have a hc or wwc prefixed, e.g. hc2/seltemp
            int8_t id = -1;
            cmd       = Command::parse_command_string(cmd, id);

            PsychicJsonResponse response = PsychicJsonResponse(request, false, EMSESP_JSON_SIZE_SMALL);
            JsonObject          output   = response.getRoot();

            uint8_t return_code = CommandRet::NOT_FOUND;
            uint8_t device_type = EMSdevice::DeviceType::CUSTOM;
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
                EMSESP::logger().err("Write command failed %s (%s)", (const char *)output["message"], Command::return_code_string(return_code).c_str());
            } else {
#if defined(EMSESP_DEBUG)
                EMSESP::logger().debug("Write command successful");
#endif
            }

            response.setCode((return_code == CommandRet::OK) ? 200 : 400); // bad request
            return response.send();
        }
    }

    // if we reach here, fail
    return request->reply(400); // Invalid/Bad Request
}

// takes a temperaturesensor name and optional offset from the WebUI and update the customization settings
// via the temperaturesensor service
esp_err_t WebDataService::write_temperature_sensor(PsychicRequest * request, JsonVariant & json) {
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

    return request->reply(ok ? 200 : 400); // bad request); // Invalid/Bad Request
}

// update the analog record, or create a new one
esp_err_t WebDataService::write_analog_sensor(PsychicRequest * request, JsonVariant & json) {
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

    return request->reply(ok ? 200 : 400); // bad request); // Invalid/Bad Request
}

} // namespace emsesp