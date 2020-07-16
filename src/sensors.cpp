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

// code written by nomis - https://github.com/nomis

#include "sensors.h"
#include "emsesp.h"

MAKE_PSTR(logger_name, "sensors")

namespace emsesp {

uuid::log::Logger Sensors::logger_{F_(logger_name), uuid::log::Facility::DAEMON};

void Sensors::start() {
    // copy over values from MQTT so we don't keep on quering the filesystem
    EMSESP::esp8266React.getMqttSettingsService()->read([&](MqttSettings & settings) {
        mqtt_format_ = settings.mqtt_format; // single, nested or ha
    });

    // if we're using HA MQTT Discovery, send out the config
    // currently we just do this for a single sensor (sensor1)
    if (mqtt_format_ == MQTT_format::HA) {
        // Mqtt::publish(topic); // empty payload, this remove any previous config sent to HA

        StaticJsonDocument<EMSESP_MAX_JSON_SIZE_MEDIUM> doc;
        doc["dev_cla"]      = "temperature";
        doc["name"]         = "ems-esp-sensor1";
        doc["uniq_id"]      = "ems-esp-sensor1";
        doc["~"]            = "homeassistant/sensor/ems-esp/external";
        doc["stat_t"]       = "~/state";
        doc["unit_of_meas"] = "°C";
        doc["val_tpl"]      = "{{value_json.sensor1.temp}}";

        Mqtt::publish("homeassistant/sensor/ems-esp/external/config", doc, true); // publish the config payload with retain flag
    }

#ifndef EMSESP_STANDALONE
    bus_.begin(SENSOR_GPIO);
#endif
}

void Sensors::loop() {
#ifndef EMSESP_STANDALONE
    uint32_t time_now = uuid::get_uptime();

    if (state_ == State::IDLE) {
        if (time_now - last_activity_ >= READ_INTERVAL_MS) {
            // LOG_DEBUG(F("Read sensor temperature")); // uncomment for debug
            if (bus_.reset()) {
                yield();
                bus_.skip();
                bus_.write(CMD_CONVERT_TEMP);

                state_ = State::READING;
            } else {
                // no sensors found
                // LOG_ERROR(F("Bus reset failed")); // uncomment for debug
                devices_.clear(); // remove all know devices incase we have a disconnect
            }
            last_activity_ = time_now;
        }
    } else if (state_ == State::READING) {
        if (temperature_convert_complete() && (time_now - last_activity_ > CONVERSION_MS)) {
            // LOG_DEBUG(F("Scanning for sensors")); // uncomment for debug
            bus_.reset_search();
            found_.clear();

            state_         = State::SCANNING;
            last_activity_ = time_now;
        } else if (time_now - last_activity_ > READ_TIMEOUT_MS) {
            LOG_ERROR(F("Sensor read timeout"));

            state_         = State::IDLE;
            last_activity_ = time_now;
        }
    } else if (state_ == State::SCANNING) {
        if (time_now - last_activity_ > SCAN_TIMEOUT_MS) {
            LOG_ERROR(F("Sensor scan timeout"));
            state_         = State::IDLE;
            last_activity_ = time_now;
        } else {
            uint8_t addr[ADDR_LEN] = {0};

            if (bus_.search(addr)) {
                bus_.depower();

                if (bus_.crc8(addr, ADDR_LEN - 1) == addr[ADDR_LEN - 1]) {
                    switch (addr[0]) {
                    case TYPE_DS18B20:
                    case TYPE_DS18S20:
                    case TYPE_DS1822:
                    case TYPE_DS1825:
                        found_.emplace_back(addr);
                        found_.back().temperature_c_ = get_temperature_c(addr);

                        /*
                        // comment out for debugging
                        char result[10];
                        LOG_DEBUG(F("Temp of %s = %s"),
                                  found_.back().to_string().c_str(),
                                  Helpers::render_value(result, found_.back().temperature_c_, 2)); 
                        */
                        break;

                    default:
                        LOG_ERROR(F("Unknown sensor %s"), Device(addr).to_string().c_str());
                        break;
                    }
                } else {
                    LOG_ERROR(F("Invalid sensor %s"), Device(addr).to_string().c_str());
                }
            } else {
                bus_.depower();
                devices_ = std::move(found_);
                found_.clear();
                // LOG_DEBUG(F("Found %zu sensor(s). Adding them."), devices_.size()); // uncomment for debug
                state_         = State::IDLE;
                last_activity_ = time_now;
            }
        }
    }
#endif
}

bool Sensors::temperature_convert_complete() {
#ifndef EMSESP_STANDALONE
    return bus_.read_bit() == 1;
#else
    return 1;
#endif
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

float Sensors::get_temperature_c(const uint8_t addr[]) {
#ifndef EMSESP_STANDALONE
    if (!bus_.reset()) {
        LOG_ERROR(F("Bus reset failed before reading scratchpad from %s"), Device(addr).to_string().c_str());
        return NAN;
    }
    yield();
    uint8_t scratchpad[SCRATCHPAD_LEN] = {0};
    bus_.select(addr);
    bus_.write(CMD_READ_SCRATCHPAD);
    bus_.read_bytes(scratchpad, SCRATCHPAD_LEN);
    yield();
    if (!bus_.reset()) {
        LOG_ERROR(F("Bus reset failed after reading scratchpad from %s"), Device(addr).to_string().c_str());
        return NAN;
    }
    yield();
    if (bus_.crc8(scratchpad, SCRATCHPAD_LEN - 1) != scratchpad[SCRATCHPAD_LEN - 1]) {
        LOG_WARNING(F("Invalid scratchpad CRC: %02X%02X%02X%02X%02X%02X%02X%02X%02X from device %s"),
                    scratchpad[0],
                    scratchpad[1],
                    scratchpad[2],
                    scratchpad[3],
                    scratchpad[4],
                    scratchpad[5],
                    scratchpad[6],
                    scratchpad[7],
                    scratchpad[8],
                    Device(addr).to_string().c_str());
        return NAN;
    }

    int16_t raw_value = ((int16_t)scratchpad[SCRATCHPAD_TEMP_MSB] << 8) | scratchpad[SCRATCHPAD_TEMP_LSB];

    // Adjust based on device resolution
    int resolution = 9 + ((scratchpad[SCRATCHPAD_CONFIG] >> 5) & 0x3);
    switch (resolution) {
    case 9:
        raw_value &= ~0x1;
        break;

    case 10:
        raw_value &= ~0x3;
        break;

    case 11:
        raw_value &= ~0x7;
        break;

    case 12:
        break;
    }

    return (float)raw_value / 16;
#else
    return NAN;
#endif
}

#pragma GCC diagnostic pop

const std::vector<Sensors::Device> Sensors::devices() const {
    return devices_;
}

Sensors::Device::Device(const uint8_t addr[])
    : id_(((uint64_t)addr[0] << 56) | ((uint64_t)addr[1] << 48) | ((uint64_t)addr[2] << 40) | ((uint64_t)addr[3] << 32) | ((uint64_t)addr[4] << 24)
          | ((uint64_t)addr[5] << 16) | ((uint64_t)addr[6] << 8) | (uint64_t)addr[7]) {
}

uint64_t Sensors::Device::id() const {
    return id_;
}

std::string Sensors::Device::to_string() const {
    std::string str(20, '\0');
    snprintf_P(&str[0],
               str.capacity() + 1,
               PSTR("%02X-%04X-%04X-%04X-%02X"),
               (unsigned int)(id_ >> 56) & 0xFF,
               (unsigned int)(id_ >> 40) & 0xFFFF,
               (unsigned int)(id_ >> 24) & 0xFFFF,
               (unsigned int)(id_ >> 8) & 0xFFFF,
               (unsigned int)(id_)&0xFF);
    return str;
}


// send all dallas sensor values as a JSON package to MQTT
// assumes there are devices
void Sensors::publish_values() {
    uint8_t num_devices = devices_.size();

    if (num_devices == 0) {
        return;
    }

    // if we're not using nested JSON, send each sensor out seperately
    // sensor1, sensor2 etc...
    // e.g. sensor_1 = {"temp":20.2}
    if (mqtt_format_ == MQTT_format::SINGLE) {
        StaticJsonDocument<100> doc;
        for (const auto & device : devices_) {
            char s[5];
            doc["temp"] = Helpers::render_value(s, device.temperature_c_, 2);
            char topic[60];                // sensors{1-n}
            strlcpy(topic, "sensor_", 50); // create topic, e.g. home/ems-esp/sensor_28-EA41-9497-0E03-5F
            strlcat(topic, device.to_string().c_str(), 60);
            Mqtt::publish(topic, doc);
            doc.clear(); // clear json doc so we can reuse the buffer again
        }
        return;
    }

    // group all sensors together - https://github.com/proddy/EMS-ESP/issues/327
    // This is used for both NESTED and HA modes
    //  sensors = {
    // "sensor1":{"id":"28-EA41-9497-0E03-5F","temp":"23.25"},
    // "sensor2":{"id":"28-EA41-9497-0E03-5F","temp":"23.25"},
    // "sensor3":{"id":"28-EA41-9497-0E03-5F","temp":"23.25"},
    // "sensor4":{"id":"28-EA41-9497-0E03-5F","temp":"23.25"}
    // }

    // const size_t        capacity = num_devices * JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(num_devices);
    DynamicJsonDocument doc(100 * num_devices);

    uint8_t i = 1;
    for (const auto & device : devices_) {
        if (mqtt_format_ == MQTT_format::CUSTOM) {
            char s[5];
            doc[device.to_string()] = Helpers::render_value(s, device.temperature_c_, 2);
        } else {
            char sensorID[10]; // sensor{1-n}
            strlcpy(sensorID, "sensor", 10);
            char s[5];
            strlcat(sensorID, Helpers::itoa(s, i++), 10);
            JsonObject dataSensor = doc.createNestedObject(sensorID);
            dataSensor["id"]      = device.to_string();
            dataSensor["temp"]    = Helpers::render_value(s, device.temperature_c_, 2);
        }
    }

    if (mqtt_format_ == MQTT_format::HA) {
        Mqtt::publish("homeassistant/sensor/ems-esp/external/state", doc);
    } else {
        Mqtt::publish("sensors", doc);
    }
}

} // namespace emsesp
