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
#define DEVICE_DATA_SERVICE_PATH "/rest/deviceData"
#define SENSOR_DATA_SERVICE_PATH "/rest/sensorData"

// POST
#define WRITE_DEVICE_VALUE_SERVICE_PATH "/rest/writeDeviceValue"
#define WRITE_TEMPERATURE_SENSOR_SERVICE_PATH "/rest/writeTemperatureSensor"
#define WRITE_ANALOG_SENSOR_SERVICE_PATH "/rest/writeAnalogSensor"
#define SCAN_DEVICES_SERVICE_PATH "/rest/scanDevices"

namespace emsesp {

class WebDataService {
  public:
    WebDataService(PsychicHttpServer * server, SecurityManager * securityManager);
    void registerURI();

// make all functions public so we can test in the debug and standalone mode
#ifndef EMSESP_STANDALONE
  private:
#endif

    SecurityManager *   _securityManager;
    PsychicHttpServer * _server;

    // GET
    esp_err_t core_data(PsychicRequest * request);
    esp_err_t sensor_data(PsychicRequest * request);
    esp_err_t device_data(PsychicRequest * request);

    // POST
    esp_err_t write_device_value(PsychicRequest * request, JsonVariant & json);
    esp_err_t write_temperature_sensor(PsychicRequest * request, JsonVariant & json);
    esp_err_t write_analog_sensor(PsychicRequest * request, JsonVariant & json);
    esp_err_t scan_devices(PsychicRequest * request); // command
};

} // namespace emsesp

#endif
