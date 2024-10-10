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

#ifndef WebDataService_h
#define WebDataService_h

// GET
#define EMSESP_CORE_DATA_SERVICE_PATH "/rest/coreData"
#define EMSESP_DEVICE_DATA_SERVICE_PATH "/rest/deviceData"
#define EMSESP_SENSOR_DATA_SERVICE_PATH "/rest/sensorData"
#define EMSESP_DASHBOARD_DATA_SERVICE_PATH "/rest/dashboardData"

// POST
#define EMSESP_WRITE_DEVICE_VALUE_SERVICE_PATH "/rest/writeDeviceValue"
#define EMSESP_WRITE_TEMPERATURE_SENSOR_SERVICE_PATH "/rest/writeTemperatureSensor"
#define EMSESP_WRITE_ANALOG_SENSOR_SERVICE_PATH "/rest/writeAnalogSensor"

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
    void device_data(AsyncWebServerRequest * request);
    void dashboard_data(AsyncWebServerRequest * request);

    // POST
    void write_device_value(AsyncWebServerRequest * request, JsonVariant json);
    void write_temperature_sensor(AsyncWebServerRequest * request, JsonVariant json);
    void write_analog_sensor(AsyncWebServerRequest * request, JsonVariant json);
};

} // namespace emsesp

#endif
