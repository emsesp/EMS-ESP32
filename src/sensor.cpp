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

// code originally written by nomis - https://github.com/nomis

#include "sensor.h"
#include "emsesp.h"

#ifdef ESP32
#define YIELD
#else
#define YIELD yield()
#endif

namespace emsesp {

uuid::log::Logger Sensor::logger_{F_(sensor), uuid::log::Facility::DAEMON};

// start the 1-wire
void Sensor::start() {
    reload();

#ifndef EMSESP_STANDALONE
    if (dallas_gpio_) {
        bus_.begin(dallas_gpio_);
    }
#endif

    // API call
    Command::add_with_json(EMSdevice::DeviceType::SENSOR, F("info"), [&](const char * value, const int8_t id, JsonObject & object) {
        return command_info(value, id, object);
    });
}

// load the MQTT settings
void Sensor::reload() {
    EMSESP::emsespSettingsService.read([&](EMSESPSettings & settings) {
        dallas_gpio_ = settings.dallas_gpio;
        parasite_    = settings.dallas_parasite;
    });

    if (Mqtt::mqtt_format() == Mqtt::Format::HA) {
        for (uint8_t i = 0; i < MAX_SENSORS; registered_ha_[i++] = false)
            ;
    }
}

void Sensor::loop() {
#ifndef EMSESP_STANDALONE
    uint32_t time_now = uuid::get_uptime();

    if (state_ == State::IDLE) {
        if (time_now - last_activity_ >= READ_INTERVAL_MS) {
            // LOG_DEBUG(F("Read sensor temperature")); // uncomment for debug
            if (bus_.reset() || parasite_) {
                YIELD;
                bus_.skip();
                bus_.write(CMD_CONVERT_TEMP, parasite_ ? 1 : 0);
                state_ = State::READING;
            } else {
                // no sensors found
                // LOG_ERROR(F("Bus reset failed")); // uncomment for debug
            }
            last_activity_ = time_now;
        }
    } else if (state_ == State::READING) {
        if (temperature_convert_complete() && (time_now - last_activity_ > CONVERSION_MS)) {
            // LOG_DEBUG(F("Scanning for sensors")); // uncomment for debug
            bus_.reset_search();
            state_ = State::SCANNING;
        } else if (time_now - last_activity_ > READ_TIMEOUT_MS) {
            LOG_ERROR(F("Sensor read timeout"));
            state_ = State::IDLE;
        }
    } else if (state_ == State::SCANNING) {
        if (time_now - last_activity_ > SCAN_TIMEOUT_MS) {
            LOG_ERROR(F("Sensor scan timeout"));
            state_ = State::IDLE;
        } else {
            uint8_t addr[ADDR_LEN] = {0};

            if (bus_.search(addr)) {
                if (!parasite_) {
                    bus_.depower();
                }
                if (bus_.crc8(addr, ADDR_LEN - 1) == addr[ADDR_LEN - 1]) {
                    switch (addr[0]) {
                    case TYPE_DS18B20:
                    case TYPE_DS18S20:
                    case TYPE_DS1822:
                    case TYPE_DS1825:
                        int16_t t;
                        t = get_temperature_c(addr);
                        if ((t >= -550) && (t <= 1250)) {
                            // check if we have this sensor already
                            bool found = false;
                            for (auto & device : devices_) {
                                if (device.id() == get_id(addr)) {
                                    changed_ |= (t != device.temperature_c);
                                    device.temperature_c = t;
                                    device.read          = true;
                                    found                = true;
                                    break;
                                }
                            }
                            // add new sensor
                            if (!found && (devices_.size() < (MAX_SENSORS - 1))) {
                                devices_.emplace_back(addr);
                                devices_.back().temperature_c = t;
                                devices_.back().read          = true;
                                changed_                      = true;
                            }
                        }
                        break;

                    default:
                        LOG_ERROR(F("Unknown sensor %s"), Device(addr).to_string().c_str());
                        break;
                    }
                } else {
                    LOG_ERROR(F("Invalid sensor %s"), Device(addr).to_string().c_str());
                }
            } else {
                if (!parasite_) {
                    bus_.depower();
                }
                // check for missing sensors after some samples
                if (++scancnt_ > 5) {
                    for (auto & device : devices_) {
                        if (!device.read) {
                            device.temperature_c = EMS_VALUE_SHORT_NOTSET;
                            changed_             = true;
                        }
                        device.read = false;
                    }
                    scancnt_ = 0;
                } else if (scancnt_ == -2) { // startup
                    firstscan_ = devices_.size();
                } else if ((scancnt_ <= 0) && (firstscan_ != devices_.size())) { // check 2 times for no change of sensorno.
                    scancnt_ = -3;
                    devices_.clear(); // restart scaning and clear to get correct numbering
                }
                // LOG_DEBUG(F("Found %zu sensor(s). Adding them."), devices_.size()); // uncomment for debug
                state_ = State::IDLE;
            }
        }
    }
#endif
}

bool Sensor::temperature_convert_complete() {
#ifndef EMSESP_STANDALONE
    if (parasite_) {
        return true; // don't care, use the minimum time in loop
    }
    return bus_.read_bit() == 1;
#else
    return true;
#endif
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

int16_t Sensor::get_temperature_c(const uint8_t addr[]) {
#ifndef EMSESP_STANDALONE
    if (!bus_.reset()) {
        LOG_ERROR(F("Bus reset failed before reading scratchpad from %s"), Device(addr).to_string().c_str());
        return EMS_VALUE_SHORT_NOTSET;
    }
    YIELD;

    uint8_t scratchpad[SCRATCHPAD_LEN] = {0};
    bus_.select(addr);
    bus_.write(CMD_READ_SCRATCHPAD);
    bus_.read_bytes(scratchpad, SCRATCHPAD_LEN);
    YIELD;

    if (!bus_.reset()) {
        LOG_ERROR(F("Bus reset failed after reading scratchpad from %s"), Device(addr).to_string().c_str());
        return EMS_VALUE_SHORT_NOTSET;
    }
    YIELD;

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
        return EMS_VALUE_SHORT_NOTSET;
    }

    int16_t raw_value = ((int16_t)scratchpad[SCRATCHPAD_TEMP_MSB] << 8) | scratchpad[SCRATCHPAD_TEMP_LSB];

    if (addr[0] == TYPE_DS18S20) {
        raw_value = (raw_value << 3) + 12 - scratchpad[SCRATCHPAD_CNT_REM];
    } else {
        // Adjust based on device resolution
        int resolution = 9 + ((scratchpad[SCRATCHPAD_CONFIG] >> 5) & 0x3);
        switch (resolution) {
        case 9:
            raw_value &= ~0x7;
            break;
        case 10:
            raw_value &= ~0x3;
            break;
        case 11:
            raw_value &= ~0x1;
            break;
        case 12:
            break;
        }
    }
    raw_value = ((int32_t)raw_value * 625 + 500) / 1000; // round to 0.1
    return raw_value;
#else
    return EMS_VALUE_SHORT_NOTSET;
#endif
}

#pragma GCC diagnostic pop

const std::vector<Sensor::Device> Sensor::devices() const {
    return devices_;
}

// skip crc from id.
Sensor::Device::Device(const uint8_t addr[])
    : id_(((uint64_t)addr[0] << 48) | ((uint64_t)addr[1] << 40) | ((uint64_t)addr[2] << 32) | ((uint64_t)addr[3] << 24) | ((uint64_t)addr[4] << 16)
          | ((uint64_t)addr[5] << 8) | ((uint64_t)addr[6])) {
}

uint64_t Sensor::get_id(const uint8_t addr[]) {
    return (((uint64_t)addr[0] << 48) | ((uint64_t)addr[1] << 40) | ((uint64_t)addr[2] << 32) | ((uint64_t)addr[3] << 24) | ((uint64_t)addr[4] << 16)
            | ((uint64_t)addr[5] << 8) | ((uint64_t)addr[6]));
}

uint64_t Sensor::Device::id() const {
    return id_;
}

std::string Sensor::Device::to_string() const {
    std::string str(20, '\0');
    snprintf_P(&str[0],
               str.capacity() + 1,
               PSTR("%02X-%04X-%04X-%04X"),
               (unsigned int)(id_ >> 48) & 0xFF,
               (unsigned int)(id_ >> 32) & 0xFFFF,
               (unsigned int)(id_ >> 16) & 0xFFFF,
               (unsigned int)(id_)&0xFFFF);
    return str;
}

// check to see if values have been updated
bool Sensor::updated_values() {
    if (changed_) {
        changed_ = false;
        return true;
    }
    return false;
}

bool Sensor::command_info(const char * value, const int8_t id, JsonObject & output) {
    return (export_values(output));
}

// creates JSON doc from values
// returns false if empty
// e.g. sensor_data = {"sensor1":{"id":"28-EA41-9497-0E03-5F","temp":"23.30"},"sensor2":{"id":"28-233D-9497-0C03-8B","temp":"24.0"}}
bool Sensor::export_values(JsonObject & output) {
    if (devices_.size() == 0) {
        return false;
    }
    uint8_t i = 1; // sensor count
    for (const auto & device : devices_) {
        char s[7];
        char sensorID[10]; // sensor{1-n}
        snprintf_P(sensorID, 10, PSTR("sensor%d"), i);
        JsonObject dataSensor = output.createNestedObject(sensorID);
        dataSensor["id"]      = device.to_string();
        dataSensor["temp"]    = Helpers::render_value(s, device.temperature_c, 10);
        i++;
    }

    return true;
}

// send all dallas sensor values as a JSON package to MQTT
// assumes there are devices
void Sensor::publish_values() {
    uint8_t num_devices = devices_.size();

    if (num_devices == 0) {
        return;
    }

    uint8_t mqtt_format_ = Mqtt::mqtt_format();
    /*
    // single mode as one topic per sensor e.g. ems-esp/sensor_data1 = 20.2
    if (mqtt_format_ == Mqtt::Format::SINGLE) {
        uint8_t i = 1;
        for (const auto & device : devices_) {
            char topic[20];
            snprintf_P(topic, 20, PSTR("sensor_data%d"), i);
            char s[7]; // to support -55.00 to 125.00
            Mqtt::publish(topic, Helpers::render_value(s, device.temperature_c, 10));
            i++;
        }
        return;
    }
    */

    DynamicJsonDocument doc(100 * num_devices);
    uint8_t             sensor_no = 1; // sensor count
    for (const auto & device : devices_) {
        char sensorID[10]; // sensor{1-n}
        snprintf_P(sensorID, 10, PSTR("sensor%d"), sensor_no);
        if (mqtt_format_ == Mqtt::Format::SINGLE) {
            // e.g. sensor_data = {"sensor1":23.3,"sensor2":24.0}
            // doc[sensorID] = Helpers::render_value(s, device.temperature_c, 10);
            doc[sensorID] = (float)(device.temperature_c) / 10;
        } else if (mqtt_format_ == Mqtt::Format::NESTED) {
            // e.g. sensor_data = {"sensor1":{"id":"28-EA41-9497-0E03","temp":"23.3"},"sensor2":{"id":"28-233D-9497-0C03","temp":"24.0"}}
            JsonObject dataSensor = doc.createNestedObject(sensorID);
            dataSensor["id"]      = device.to_string();
            // dataSensor["temp"]    = Helpers::render_value(s, device.temperature_c, 10);
            dataSensor["temp"] = (float)(device.temperature_c) / 10;
        } else if (mqtt_format_ == Mqtt::Format::HA) {
            // e.g. sensor_data = {"28-EA41-9497-0E03":23.3,"28-233D-9497-0C03":24.0}
            // doc[device.to_string()] = Helpers::render_value(s, device.temperature_c, 10);
            JsonObject dataSensor = doc.createNestedObject(sensorID);
            dataSensor["id"]      = device.to_string();
            dataSensor["temp"]    = (float)(device.temperature_c) / 10;
            // create the config if this hasn't already been done
            // to e.g. homeassistant/sensor/ems-esp/dallas_28-233D-9497-0C03/config
            if (!(registered_ha_[sensor_no - 1])) {
                StaticJsonDocument<EMSESP_MAX_JSON_SIZE_MEDIUM> config;
                config["dev_cla"] = F("temperature");

                char stat_t[50];
                snprintf_P(stat_t, sizeof(stat_t), PSTR("%s/sensor_data"), System::hostname().c_str());
                config["stat_t"] = stat_t;

                config["unit_of_meas"] = F("°C");

                char str[50];
                snprintf_P(str, sizeof(str), PSTR("{{value_json.sensor%d.temp}}"), sensor_no);
                config["val_tpl"] = str;

                // name as sensor number not the long unique ID
                snprintf_P(str, sizeof(str), PSTR("Dallas Sensor %d"), sensor_no);
                config["name"] = str;

                snprintf_P(str, sizeof(str), PSTR("dallas_%s"), device.to_string().c_str());
                config["uniq_id"] = str;

                JsonObject dev = config.createNestedObject("dev");
                JsonArray  ids = dev.createNestedArray("ids");
                ids.add("ems-esp");

                std::string topic(100, '\0');
                snprintf_P(&topic[0], 100, PSTR("homeassistant/sensor/ems-esp/dallas_%s/config"), device.to_string().c_str());
                Mqtt::publish_retain(topic, config.as<JsonObject>(), true); // publish the config payload with retain flag

                registered_ha_[sensor_no - 1] = true;
            }
        }
        sensor_no++; // increment sensor count
    }

    Mqtt::publish(F("sensor_data"), doc.as<JsonObject>());
}

} // namespace emsesp