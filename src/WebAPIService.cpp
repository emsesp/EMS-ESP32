/*
 * EMS-ESP - https://github.com/proddy/EMS-ESP
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

#include "WebAPIService.h"
#include "emsesp.h"
#include "command.h"
#include "emsdevice.h"
#include "system.h"

namespace emsesp {

WebAPIService::WebAPIService(AsyncWebServer * server) {
    server->on(EMSESP_API_SERVICE_PATH, HTTP_GET, std::bind(&WebAPIService::webAPIService, this, std::placeholders::_1));
}

// http://ems-esp/api?device=boiler&cmd=wwtemp&data=20&id=1
void WebAPIService::webAPIService(AsyncWebServerRequest * request) {
    // see if the API is enabled
    bool api_enabled;
    EMSESP::webSettingsService.read([&](WebSettings & settings) { api_enabled = settings.api_enabled; });

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
    if (!Command::find_command(device_type, cmd.c_str())) {
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

    DynamicJsonDocument doc(EMSESP_MAX_JSON_SIZE_LARGE);
    JsonObject          json = doc.to<JsonObject>();
    bool                ok   = false;

    // execute the command
    if (data.isEmpty()) {
        ok = Command::call(device_type, cmd.c_str(), nullptr, id.toInt(), json); // command only
    } else {
        if (api_enabled) {
            // we only allow commands with parameters if the API is enabled
            ok = Command::call(device_type, cmd.c_str(), data.c_str(), id.toInt(), json); // has cmd, data and id
        } else {
            request->send(401, "text/plain", F("Unauthorized"));
            return;
        }
    }

// debug
#if defined(EMSESP_DEBUG)
    std::string debug(200, '\0');
    snprintf_P(&debug[0],
               debug.capacity() + 1,
               PSTR("API: device=%s cmd=%s data=%s id=%s [%s]"),
               device.c_str(),
               cmd.c_str(),
               data.c_str(),
               id.c_str(),
               ok ? F("OK") : F("Invalid"));
    EMSESP::logger().info(debug.c_str());
    if (json.size()) {
        char buffer2[EMSESP_MAX_JSON_SIZE_LARGE];
        serializeJson(doc, buffer2);
        EMSESP::logger().info("json (max 255 chars): %s", buffer2);
    }
#endif

    // if we have returned data in JSON format, send this to the WEB
    if (json.size()) {
        doc.shrinkToFit();
        char buffer[EMSESP_MAX_JSON_SIZE_LARGE];
        serializeJsonPretty(doc, buffer);
        request->send(200, "text/plain", buffer);
    } else {
        request->send(200, "text/plain", ok ? F("OK") : F("Invalid"));
    }
}

} // namespace emsesp