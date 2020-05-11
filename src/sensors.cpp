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

MAKE_PSTR(logger_name, "sensors")

namespace emsesp {

uuid::log::Logger Sensors::logger_{F_(logger_name), uuid::log::Facility::DAEMON};

void Sensors::start() {
    // copy over values from MQTT so we don't keep on quering the filesystem
    mqtt_nestedjson_ = Settings().mqtt_nestedjson();

#ifndef EMSESP_STANDALONE
    bus_.begin(SENSOR_GPIO);
#endif
}

void Sensors::loop() {
#ifndef EMSESP_STANDALONE
    if (state_ == State::IDLE) {
        if (millis() - last_activity_ >= READ_INTERVAL_MS) {
            // DEBUG_LOG(F("Read sensor temperature"));
            if (bus_.reset()) {
                bus_.skip();
                bus_.write(CMD_CONVERT_TEMP);

                state_ = State::READING;
            } else {
                // logger_.err(F("Bus reset failed"));
            }
            last_activity_ = millis();
        }
    } else if (state_ == State::READING) {
        if (temperature_convert_complete()) {
            // DEBUG_LOG(F("Scanning for sensors"));
            bus_.reset_search();
            found_.clear();

            state_         = State::SCANNING;
            last_activity_ = millis();
        } else if (millis() - last_activity_ > READ_TIMEOUT_MS) {
            // logger_.err(F("Sensor read timeout"));

            state_         = State::IDLE;
            last_activity_ = millis();
        }
    } else if (state_ == State::SCANNING) {
        if (millis() - last_activity_ > SCAN_TIMEOUT_MS) {
            // logger_.err(F("Sensor scan timeout"));
            state_         = State::IDLE;
            last_activity_ = millis();
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

                        // char result[10];
                        // DEBUG_LOG(F("Temperature of %s = %s"), found_.back().to_string().c_str(), Helpers::render_value(result, found_.back().temperature_c_, 2));
                        break;

                    default:
                        logger_.err(F("Unknown sensor %s"), Device(addr).to_string().c_str());
                        break;
                    }
                } else {
                    logger_.err(F("Invalid sensor %s"), Device(addr).to_string().c_str());
                }
            } else {
                bus_.depower();
                devices_ = std::move(found_);
                found_.clear();
                // DEBUG_LOG(F("Found %zu sensor(s)"), devices_.size());
                state_         = State::IDLE;
                last_activity_ = millis();
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
        logger_.err(F("Bus reset failed before reading scratchpad from %s"), Device(addr).to_string().c_str());
        return NAN;
    }

    uint8_t scratchpad[SCRATCHPAD_LEN] = {0};

    bus_.select(addr);
    bus_.write(CMD_READ_SCRATCHPAD);
    bus_.read_bytes(scratchpad, SCRATCHPAD_LEN);

    if (!bus_.reset()) {
        logger_.err(F("Bus reset failed after reading scratchpad from %s"), Device(addr).to_string().c_str());
        return NAN;
    }

    if (bus_.crc8(scratchpad, SCRATCHPAD_LEN - 1) != scratchpad[SCRATCHPAD_LEN - 1]) {
        logger_.warning(F("Invalid scratchpad CRC: %02X%02X%02X%02X%02X%02X%02X%02X%02X from device %s"),
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
    if (!mqtt_nestedjson_) {
        StaticJsonDocument<20> doc;
        for (const auto & device : devices_) {
            char s[5];
            doc["temp"] = Helpers::render_value(s, device.temperature_c_, 2);
            char topic[60];                // sensors{1-n}
            strlcpy(topic, "sensor_", 50); // create topic
            strlcat(topic, device.to_string().c_str(), 50);
            Mqtt::publish(topic, doc);
            doc.clear(); // clear json doc so we can reuse the buffer again
        }
        return;
    }

    // group all sensors together - https://github.com/proddy/EMS-ESP/issues/327
    // https://arduinojson.org/v6/assistant/
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
        char sensorID[10]; // sensor{1-n}
        strlcpy(sensorID, "sensor", 10);
        char s[5];
        strlcat(sensorID, Helpers::itoa(s, i++), 10);
        JsonObject dataSensor = doc.createNestedObject(sensorID);
        dataSensor["id"]      = device.to_string();
        dataSensor["temp"]    = Helpers::render_value(s, device.temperature_c_, 2);
    }

    Mqtt::publish("sensors", doc);
}

} // namespace emsesp
