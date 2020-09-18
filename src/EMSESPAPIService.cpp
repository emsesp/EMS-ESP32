/*
 * EMS-ESP - https://github.com/proddy/EMS-ESP
 * Copyright 2019  Paul Derbyshire
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

#include "EMSESPAPIService.h"
#include "emsesp.h"
#include "command.h"
#include "emsdevice.h"
#include "system.h"

namespace emsesp {

EMSESPAPIService::EMSESPAPIService(AsyncWebServer * server) {
    server->on(EMSESP_API_SERVICE_PATH, HTTP_GET, std::bind(&EMSESPAPIService::emsespAPIService, this, std::placeholders::_1));
}

// http://ems-esp/api?device=boiler&cmd=wwtemp&data=20&id=1
void EMSESPAPIService::emsespAPIService(AsyncWebServerRequest * request) {
    // must have device and cmd parameters
    if ((!request->hasParam(F_(device))) || (!request->hasParam(F_(cmd)))) {
        request->send(400, "text/plain", F("invalid syntax"));
        return;
    }

    // get device
    String  device      = request->getParam(F_(device))->value();
    uint8_t device_type = EMSdevice::device_name_2_device_type(device.c_str());
    if (device_type == emsesp::EMSdevice::DeviceType::UNKNOWN) {
        request->send(400, "text/plain", F("invalid device"));
        return;
    }

    // get cmd, we know we have one
    String cmd = request->getParam(F_(cmd))->value();

    // first test for special service commands
    // e.g. http://ems-esp/api?device=system&cmd=info
    if (device.equals("system")) {
        if (cmd.equals("info")) {
            request->send(200, "text/plain", System::export_settings());
            EMSESP::logger().info(F("Sent settings json to web UI"));
            return;
        }
    }

    // look up command in our list
    if (!Command::find(device_type, cmd.c_str())) {
        request->send(400, "text/plain", F("invalid cmd"));
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

    // execute the command
    bool ok = false;
    if (data.isEmpty()) {
        ok = Command::call_command(device_type, cmd.c_str(), nullptr, -1); // command only
    } else if (id.isEmpty()) {
        ok = Command::call_command(device_type, cmd.c_str(), data.c_str(), -1); // only ID
    } else {
        ok = Command::call_command(device_type, cmd.c_str(), data.c_str(), id.toInt()); // has cmd, data and id
    }

// debug
#if defined(EMSESP_DEBUG)
    std::string output(200, '\0');
    snprintf_P(&output[0],
               output.capacity() + 1,
               PSTR("API: device=%s cmd=%s data=%s id=%s [%s]"),
               device.c_str(),
               cmd.c_str(),
               data.c_str(),
               id.c_str(),
               ok ? F("OK") : F("Failed"));
    EMSESP::logger().info(output.c_str());
#endif

    request->send(200, "text/plain", ok ? F("OK") : F("Failed"));
}

} // namespace emsesp