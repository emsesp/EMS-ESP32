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

WebDataService::WebDataService(AsyncWebServer * server, SecurityManager * securityManager)
    : _device_data_handler(DEVICE_DATA_SERVICE_PATH,
                           securityManager->wrapCallback(std::bind(&WebDataService::device_data, this, _1, _2), AuthenticationPredicates::IS_AUTHENTICATED))
    , _write_value_handler(WRITE_VALUE_SERVICE_PATH,
                           securityManager->wrapCallback(std::bind(&WebDataService::write_value, this, _1, _2), AuthenticationPredicates::IS_ADMIN))
    , _write_sensor_handler(WRITE_SENSOR_SERVICE_PATH,
                            securityManager->wrapCallback(std::bind(&WebDataService::write_sensor, this, _1, _2), AuthenticationPredicates::IS_ADMIN))
    , _write_analog_handler(WRITE_ANALOG_SERVICE_PATH,
                            securityManager->wrapCallback(std::bind(&WebDataService::write_analog, this, _1, _2), AuthenticationPredicates::IS_ADMIN)) {
    server->on(CORE_DATA_SERVICE_PATH,
               HTTP_GET,
               securityManager->wrapRequest(std::bind(&WebDataService::core_data, this, _1), AuthenticationPredicates::IS_AUTHENTICATED));

    server->on(SENSOR_DATA_SERVICE_PATH,
               HTTP_GET,
               securityManager->wrapRequest(std::bind(&WebDataService::sensor_data, this, _1), AuthenticationPredicates::IS_AUTHENTICATED));

    server->on(SCAN_DEVICES_SERVICE_PATH,
               HTTP_POST,
               securityManager->wrapRequest(std::bind(&WebDataService::scan_devices, this, _1), AuthenticationPredicates::IS_ADMIN));

    _device_data_handler.setMethod(HTTP_POST);
    _device_data_handler.setMaxContentLength(256);
    server->addHandler(&_device_data_handler);

    _write_value_handler.setMethod(HTTP_POST);
    _write_value_handler.setMaxContentLength(256);
    server->addHandler(&_write_value_handler);

    _write_sensor_handler.setMethod(HTTP_POST);
    _write_sensor_handler.setMaxContentLength(256);
    server->addHandler(&_write_sensor_handler);

    _write_analog_handler.setMethod(HTTP_POST);
    _write_analog_handler.setMaxContentLength(256);
    server->addHandler(&_write_analog_handler);
}

// scan devices service
void WebDataService::scan_devices(AsyncWebServerRequest * request) {
    EMSESP::logger().info("Scanning devices...");
    EMSESP::scan_devices();
    request->send(200);
}

// this is used in the dashboard and contains all ems device information
// /coreData endpoint
void WebDataService::core_data(AsyncWebServerRequest * request) {
    auto *     response = new AsyncJsonResponse(false, EMSESP_JSON_SIZE_XXLARGE);
    JsonObject root     = response->getRoot();

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
            obj["e"]       = emsdevice->count_entities();                       // number of entities (device values)
        }
    }
    if (EMSESP::webEntityService.count_entities()) {
        JsonObject obj = devices.createNestedObject();
        obj["id"]      = 99;                                                // the last unique id
        obj["tn"]      = Helpers::translated_word(FL_(custom_device));      // translated device type name
        obj["t"]       = EMSdevice::DeviceType::CUSTOM;                     // device type number
        obj["b"]       = Helpers::translated_word(FL_(na));                 // brand
        obj["n"]       = Helpers::translated_word(FL_(custom_device_name)); // name
        obj["d"]       = 0;                                                 // deviceid
        obj["p"]       = 0;                                                 // productid
        obj["v"]       = 0;                                                 // version
        obj["e"]       = EMSESP::webEntityService.count_entities();         // number of entities (device values)
    }

    // sensors stuff
    root["s_n"]            = Helpers::translated_word(FL_(sensors_device));
    root["active_sensors"] = EMSESP::dallassensor_.no_sensors() + (EMSESP::analogsensor_.analog_enabled() ? EMSESP::analogsensor_.no_sensors() : 0);
    root["analog_enabled"] = EMSESP::analogsensor_.analog_enabled();
    root["connected"]      = EMSESP::bus_status() != 2;

    response->setLength();
    request->send(response);
}

// sensor data - sends back to web
// /sensorData endpoint
// the "sensors" and "analogs" are arrays and must exist
void WebDataService::sensor_data(AsyncWebServerRequest * request) {
    auto *     response = new AsyncJsonResponse(false, EMSESP_JSON_SIZE_XXLARGE);
    JsonObject root     = response->getRoot();

    // dallas sensors
    JsonArray sensors = root.createNestedArray("sensors");
    if (EMSESP::dallassensor_.have_sensors()) {
        for (const auto & sensor : EMSESP::dallassensor_.sensors()) {
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
    JsonArray analogs = root.createNestedArray("analogs");
    if (EMSESP::analog_enabled() && EMSESP::analogsensor_.have_sensors()) {
        uint8_t count = 0;
        char    buffer[3];
        for (const auto & sensor : EMSESP::analogsensor_.sensors()) {
            // don't send if it's marked for removal
            if (sensor.type() != AnalogSensor::AnalogType::MARK_DELETED) {
                count++;
                JsonObject obj = analogs.createNestedObject();
                obj["id"]      = Helpers::smallitoa(buffer, count); // needed for sorting table
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
    }

    response->setLength();
    request->send(response);
}

// The unique_id is the unique record ID from the Web table to identify which device to load
// Compresses the JSON using MsgPack https://msgpack.org/index.html
void WebDataService::device_data(AsyncWebServerRequest * request, JsonVariant & json) {
    if (json.is<JsonObject>()) {
        size_t buffer   = EMSESP_JSON_SIZE_XXXXLARGE;
        auto * response = new MsgpackAsyncJsonResponse(false, buffer);
        while (!response->getSize()) {
            delete response;
            buffer -= 1024;
            response = new MsgpackAsyncJsonResponse(false, buffer);
        }
        for (const auto & emsdevice : EMSESP::emsdevices) {
            if (emsdevice->unique_id() == json["id"]) {
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
        if (json["id"] == 99) {
            JsonObject output = response->getRoot();
            EMSESP::webEntityService.generate_value_web(output);
            response->setLength();
            request->send(response);
            return;
        }
#endif
    }

    // invalid but send ok
    AsyncWebServerResponse * response = request->beginResponse(200);
    request->send(response);
}


// takes a command and its data value from a specific EMS Device, from the Web
// assumes the service has been checked for admin authentication
void WebDataService::write_value(AsyncWebServerRequest * request, JsonVariant & json) {
    if (json.is<JsonObject>()) {
        JsonObject dv        = json["devicevalue"];
        uint8_t    unique_id = json["id"];

        // using the unique ID from the web find the real device type
        // id is the selected device
        for (const auto & emsdevice : EMSESP::emsdevices) {
            if (emsdevice->unique_id() == unique_id) {
                // parse the command as it could have a hc or wwc prefixed, e.g. hc2/seltemp
                const char * cmd = dv["c"];                                // the command
                int8_t       id  = -1;                                     // default
                cmd              = Command::parse_command_string(cmd, id); // extract hc or wwc

                // create JSON for output
                auto *     response = new AsyncJsonResponse(false, EMSESP_JSON_SIZE_SMALL);
                JsonObject output   = response->getRoot();

                // the data could be in any format, but we need string
                // authenticated is always true
                JsonVariant data        = dv["v"]; // the value in any format
                uint8_t     return_code = CommandRet::OK;
                uint8_t     device_type = emsdevice->device_type();
                if (data.is<const char *>()) {
                    return_code = Command::call(device_type, cmd, data.as<const char *>(), true, id, output);
                } else if (data.is<int>()) {
                    char s[10];
                    return_code = Command::call(device_type, cmd, Helpers::render_value(s, data.as<int16_t>(), 0), true, id, output);
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

                response->setCode((return_code == CommandRet::OK) ? 200 : 204);
                response->setLength();
                request->send(response);
                return;
            }
        }
        if (unique_id == 99) {
            // parse the command as it could have a hc or wwc prefixed, e.g. hc2/seltemp
            const char * cmd        = dv["c"];
            int8_t       id         = -1;
            cmd                     = Command::parse_command_string(cmd, id);
            auto *      response    = new AsyncJsonResponse(false, EMSESP_JSON_SIZE_SMALL);
            JsonObject  output      = response->getRoot();
            JsonVariant data        = dv["v"]; // the value in any format
            uint8_t     return_code = CommandRet::OK;
            uint8_t     device_type = EMSdevice::DeviceType::CUSTOM;
            if (data.is<int>()) {
                char s[10];
                return_code = Command::call(device_type, cmd, Helpers::render_value(s, data.as<int16_t>(), 0), true, id, output);
            } else if (data.is<float>()) {
                char s[10];
                return_code = Command::call(device_type, cmd, Helpers::render_value(s, data.as<float>(), 1), true, id, output);
            } else if (data.is<bool>()) {
                return_code = Command::call(device_type, cmd, data.as<bool>() ? "true" : "false", true, id, output);
            }
            if (return_code != CommandRet::OK) {
                EMSESP::logger().err("Write command failed %s (%s)", (const char *)output["message"], Command::return_code_string(return_code).c_str());
            } else {
#if defined(EMSESP_DEBUG)
                EMSESP::logger().debug("Write command successful");
#endif
            }
            response->setCode((return_code == CommandRet::OK) ? 200 : 204);
            response->setLength();
            request->send(response);
            return;
        }
    }

    AsyncWebServerResponse * response = request->beginResponse(204); // Write command failed
    request->send(response);
}

// takes a dallas sensor name and optional offset from the WebUI and update the customization settings
// via the Dallas service
void WebDataService::write_sensor(AsyncWebServerRequest * request, JsonVariant & json) {
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
        ok = EMSESP::dallassensor_.update(id, name, offset10);
    }

    AsyncWebServerResponse * response = request->beginResponse(ok ? 200 : 204);
    request->send(response);
}

// update the analog record, or create a new one
void WebDataService::write_analog(AsyncWebServerRequest * request, JsonVariant & json) {
    bool ok = false;
    if (json.is<JsonObject>()) {
        JsonObject analog = json;

        uint8_t     gpio   = analog["gpio"]; // this is the unique key, the GPIO
        std::string name   = analog["name"];
        double      factor = analog["factor"];
        double      offset = analog["offset"];
        uint8_t     uom    = analog["uom"];
        int8_t      type   = analog["type"];
        ok                 = EMSESP::analogsensor_.update(gpio, name, offset, factor, uom, type);
    }

    AsyncWebServerResponse * response = request->beginResponse(ok ? 200 : 204);
    request->send(response);
}

} // namespace emsesp