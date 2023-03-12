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

#ifndef WebDataService_h
#define WebDataService_h

// GET
#define CORE_DATA_SERVICE_PATH "/rest/coreData"
#define SCAN_DEVICES_SERVICE_PATH "/rest/scanDevices"
#define DEVICE_DATA_SERVICE_PATH "/rest/deviceData"
#define SENSOR_DATA_SERVICE_PATH "/rest/sensorData"

// POST
#define WRITE_VALUE_SERVICE_PATH "/rest/writeValue"
#define WRITE_SENSOR_SERVICE_PATH "/rest/writeSensor"
#define WRITE_ANALOG_SERVICE_PATH "/rest/writeAnalog"

namespace emsesp {

class WebDataService {
  public:
    WebDataService(AsyncWebServer * server, SecurityManager * securityManager);

// make all functions public so we can test in the debug and standalone mode
#ifndef EMSESP_STANDALONE
  private:
#endif

    // GET
    void core_data(AsyncWebServerRequest * request);
    void sensor_data(AsyncWebServerRequest * request);

    // POST
    void device_data(AsyncWebServerRequest * request, JsonVariant & json);
    void write_value(AsyncWebServerRequest * request, JsonVariant & json);
    void write_sensor(AsyncWebServerRequest * request, JsonVariant & json);
    void write_analog(AsyncWebServerRequest * request, JsonVariant & json);
    void scan_devices(AsyncWebServerRequest * request);

    AsyncCallbackJsonWebHandler _device_data_handler, _write_value_handler, _write_sensor_handler, _write_analog_handler;
};

} // namespace emsesp

#endif
