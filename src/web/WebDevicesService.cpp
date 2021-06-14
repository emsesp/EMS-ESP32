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

WebDevicesService::WebDevicesService(AsyncWebServer * server, SecurityManager * securityManager)
    : _device_dataHandler(DEVICE_DATA_SERVICE_PATH,
                          securityManager->wrapCallback(std::bind(&WebDevicesService::device_data, this, _1, _2), AuthenticationPredicates::IS_AUTHENTICATED))
    , _writevalue_dataHandler(WRITE_VALUE_SERVICE_PATH,
                              securityManager->wrapCallback(std::bind(&WebDevicesService::write_value, this, _1, _2), AuthenticationPredicates::IS_ADMIN)) {
    server->on(EMSESP_DEVICES_SERVICE_PATH,
               HTTP_GET,
               securityManager->wrapRequest(std::bind(&WebDevicesService::all_devices, this, _1), AuthenticationPredicates::IS_AUTHENTICATED));
    server->on(SCAN_DEVICES_SERVICE_PATH,
               HTTP_GET,
               securityManager->wrapRequest(std::bind(&WebDevicesService::scan_devices, this, _1), AuthenticationPredicates::IS_AUTHENTICATED));

    _device_dataHandler.setMethod(HTTP_POST);
    _device_dataHandler.setMaxContentLength(256);
    server->addHandler(&_device_dataHandler);

    _writevalue_dataHandler.setMethod(HTTP_POST);
    _writevalue_dataHandler.setMaxContentLength(256);
    server->addHandler(&_writevalue_dataHandler);
}

void WebDevicesService::scan_devices(AsyncWebServerRequest * request) {
    EMSESP::scan_devices();
    request->send(200);
}

void WebDevicesService::all_devices(AsyncWebServerRequest * request) {
    AsyncJsonResponse * response = new AsyncJsonResponse(false, EMSESP_JSON_SIZE_LARGE_DYN);
    JsonObject          root     = response->getRoot();

    JsonArray devices = root.createNestedArray("devices");
    for (const auto & emsdevice : EMSESP::emsdevices) {
        if (emsdevice) {
            JsonObject obj   = devices.createNestedObject();
            obj["id"]        = emsdevice->unique_id();
            obj["type"]      = emsdevice->device_type_name();
            obj["brand"]     = emsdevice->brand_to_string();
            obj["name"]      = emsdevice->name();
            obj["deviceid"]  = emsdevice->device_id();
            obj["productid"] = emsdevice->product_id();
            obj["version"]   = emsdevice->version();
        }
    }

    JsonArray sensors = root.createNestedArray("sensors");
    if (EMSESP::have_sensors()) {
        uint8_t i = 1;
        char    s[8];
        for (const auto & sensor : EMSESP::sensor_devices()) {
            JsonObject obj = sensors.createNestedObject();
            obj["no"]      = i++;
            obj["id"]      = sensor.to_string();
            obj["temp"]    = Helpers::render_value(s, sensor.temperature_c, 10);
        }
    }

    response->setLength();
    request->send(response);
}

// The unique_id is the unique record ID from the Web table to identify which device to load
// Compresses the JSON using MsgPack https://msgpack.org/index.html
void WebDevicesService::device_data(AsyncWebServerRequest * request, JsonVariant & json) {
    if (json.is<JsonObject>()) {
        MsgpackAsyncJsonResponse * response = new MsgpackAsyncJsonResponse(false, EMSESP_JSON_SIZE_XXLARGE_DYN);
        for (const auto & emsdevice : EMSESP::emsdevices) {
            if (emsdevice) {
                if (emsdevice->unique_id() == json["id"]) {
#ifndef EMSESP_STANDALONE
                    JsonObject root = response->getRoot();
                    emsdevice->generate_values_json_web(root);
#endif
                    response->setLength();
                    request->send(response);
                    return;
                }
            }
        }
    }

    // invalid
    AsyncWebServerResponse * response = request->beginResponse(200);
    request->send(response);
}

// takes a command and its data value from a specific Device, from the Web
void WebDevicesService::write_value(AsyncWebServerRequest * request, JsonVariant & json) {
    if (json.is<JsonObject>()) {
        JsonObject dv = json["devicevalue"];
        uint8_t    id = json["id"];

        // using the unique ID from the web find the real device type
        for (const auto & emsdevice : EMSESP::emsdevices) {
            if (emsdevice) {
                if (emsdevice->unique_id() == id) {
                    const char * cmd         = dv["c"];
                    uint8_t      device_type = emsdevice->device_type();
                    bool         ok          = false;
                    char         s[10];
                    // the data could be in any format, but we need string
                    JsonVariant data = dv["v"];
                    if (data.is<const char *>()) {
                        ok = Command::call(device_type, cmd, data.as<const char *>());
                    } else if (data.is<int>()) {
                        ok = Command::call(device_type, cmd, Helpers::render_value(s, data.as<int16_t>(), 0));
                    } else if (data.is<float>()) {
                        ok = Command::call(device_type, cmd, Helpers::render_value(s, (float)data.as<float>(), 1));
                    } else if (data.is<bool>()) {
                        ok = Command::call(device_type, cmd, data.as<bool>() ? "true" : "false");
                    }

                    // send "Write command sent to device" or "Write command failed"
                    AsyncWebServerResponse * response = request->beginResponse(ok ? 200 : 204);
                    request->send(response);
                    return;
                }
            }
        }
    }

    AsyncWebServerResponse * response = request->beginResponse(204); // Write command failed
    request->send(response);
}

} // namespace emsesp