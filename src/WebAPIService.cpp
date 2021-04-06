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

using namespace std::placeholders; // for `_1` etc

namespace emsesp {

WebAPIService::WebAPIService(AsyncWebServer * server) {
    server->on(EMSESP_API_SERVICE_PATH, HTTP_GET, std::bind(&WebAPIService::webAPIService, this, _1));
}

// e.g. http://ems-esp/api?device=boiler&cmd=wwtemp&data=20&id=1
void WebAPIService::webAPIService(AsyncWebServerRequest * request) {
    // must have device and cmd parameters
    if ((!request->hasParam(F_(device))) || (!request->hasParam(F_(cmd)))) {
        request->send(400, "text/plain", F("Invalid syntax"));
        return;
    }

    // get device
    String  device      = request->getParam(F_(device))->value();
    uint8_t device_type = EMSdevice::device_name_2_device_type(device.c_str());
    if (device_type == emsesp::EMSdevice::DeviceType::UNKNOWN) {
        request->send(400, "text/plain", F("Invalid device"));
        return;
    }

    // get cmd, we know we have one
    String cmd = request->getParam(F_(cmd))->value();

    // look up command in our list
    if (Command::find_command(device_type, cmd.c_str()) == nullptr) {
        request->send(400, "text/plain", F("Invalid cmd"));
        return;
    }

    String data;
    if (request->hasParam(F_(data))) {
        data = request->getParam(F_(data))->value();
    }

    String id;
    if (request->hasParam(F_(id))) {
        id = request->getParam(F_(id))->value();
    }

    if (id.isEmpty()) {
        id = "-1";
    }

    DynamicJsonDocument doc(EMSESP_JSON_SIZE_XLARGE_DYN);
    JsonObject          json = doc.to<JsonObject>();
    bool                ok   = false;

    // execute the command
    if (data.isEmpty()) {
        ok = Command::call(device_type, cmd.c_str(), nullptr, id.toInt(), json); // command only
    } else {
        // we only allow commands with parameters if the API is enabled
        bool api_enabled;
        EMSESP::webSettingsService.read([&](WebSettings & settings) { api_enabled = settings.api_enabled; });
        if (api_enabled) {
            ok = Command::call(device_type, cmd.c_str(), data.c_str(), id.toInt(), json); // has cmd, data and id
        } else {
            request->send(401, "text/plain", F("Unauthorized"));
            return;
        }
    }
    if (ok && json.size()) {
        // send json output back to web
        doc.shrinkToFit();
        std::string buffer;
        serializeJsonPretty(doc, buffer);
        request->send(200, "text/plain", buffer.c_str());
        return;
    }
    request->send(200, "text/plain", ok ? F("OK") : F("Invalid"));
}

} // namespace emsesp