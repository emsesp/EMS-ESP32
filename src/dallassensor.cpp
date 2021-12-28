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

// code originally written by nomis - https://github.com/nomis

#include "dallassensor.h"
#include "emsesp.h"

#ifdef ESP32
#define YIELD
#else
#define YIELD yield()
#endif

namespace emsesp {

uuid::log::Logger DallasSensor::logger_{F_(dallassensor), uuid::log::Facility::DAEMON};

// start the 1-wire
void DallasSensor::start() {
    reload();

    if (!dallas_gpio_) {
        return; // disabled if dallas gpio is 0
    }

#ifndef EMSESP_STANDALONE
    bus_.begin(dallas_gpio_);
    LOG_INFO("Starting Dallas service");
#endif

    // Add API calls
    Command::add(
        EMSdevice::DeviceType::DALLASSENSOR,
        F_(info),
        [&](const char * value, const int8_t id, JsonObject & output) { return command_info(value, id, output); },
        F_(info_cmd));
    Command::add(
        EMSdevice::DeviceType::DALLASSENSOR,
        F_(commands),
        [&](const char * value, const int8_t id, JsonObject & output) { return command_commands(value, id, output); },
        F_(commands_cmd));
}

// load settings
void DallasSensor::reload() {
    // load the service settings
    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        dallas_gpio_ = settings.dallas_gpio;
        parasite_    = settings.dallas_parasite;
    });

    for (auto & sensor : sensors_) {
        sensor.ha_registered = false; // force HA configs to be re-created
    }
}

void DallasSensor::loop() {
    if (!dallas_gpio_) {
        return; // dallas gpio is 0 (disabled)
    }

#ifndef EMSESP_STANDALONE
    uint32_t time_now = uuid::get_uptime();

    if (state_ == State::IDLE) {
        if (time_now - last_activity_ >= READ_INTERVAL_MS) {
#ifdef EMSESP_DEBUG_SENSOR
            LOG_DEBUG(F("[DEBUG] Read sensor temperature"));
#endif
            if (bus_.reset() || parasite_) {
                YIELD;
                bus_.skip();
                bus_.write(CMD_CONVERT_TEMP, parasite_ ? 1 : 0);
                state_     = State::READING;
                scanretry_ = 0;
            } else {
                // no sensors found
                if (sensors_.size()) {
                    sensorfails_++;
                    if (++scanretry_ > SCAN_MAX) { // every 30 sec
                        scanretry_ = 0;
#ifdef EMSESP_DEBUG_SENSOR
                        LOG_ERROR(F("Bus reset failed"));
#endif
                        for (auto & sensor : sensors_) {
                            sensor.temperature_c = EMS_VALUE_SHORT_NOTSET;
                        }
                    }
                }
            }
            last_activity_ = time_now;
        }
    } else if (state_ == State::READING) {
        if (temperature_convert_complete() && (time_now - last_activity_ > CONVERSION_MS)) {
#ifdef EMSESP_DEBUG_SENSOR
            LOG_DEBUG(F("Scanning for sensors"));
#endif
            bus_.reset_search();
            state_ = State::SCANNING;
        } else if (time_now - last_activity_ > READ_TIMEOUT_MS) {
#ifdef EMSESP_DEBUG_SENSOR
            LOG_WARNING(F("Dallas sensor read timeout"));
#endif
            state_ = State::IDLE;
            sensorfails_++;
        }
    } else if (state_ == State::SCANNING) {
        if (time_now - last_activity_ > SCAN_TIMEOUT_MS) {
#ifdef EMSESP_DEBUG_SENSOR
            LOG_ERROR(F("Dallas sensor scan timeout"));
#endif
            state_ = State::IDLE;
            sensorfails_++;
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
                            sensorreads_++;
                            // check if we already have this sensor
                            bool found = false;
                            for (auto & sensor : sensors_) {
                                if (sensor.id() == get_id(addr)) {
                                    t += sensor.offset();
                                    if (t != sensor.temperature_c) {
                                        publish_sensor(sensor);
                                        changed_ |= true;
                                    }
                                    sensor.temperature_c = t;
                                    sensor.read          = true;
                                    found                = true;
                                    break;
                                }
                            }
                            // add new sensor. this will create the id string, empty name and offset
                            if (!found && (sensors_.size() < (MAX_SENSORS - 1))) {
                                sensors_.emplace_back(addr);
                                sensors_.back().temperature_c = t + sensors_.back().offset();
                                sensors_.back().read          = true;
                                changed_                      = true;
                                // look in the customization service for an optional alias or offset
                                sensors_.back().apply_customization();
                                publish_sensor(sensors_.back());
                            }
                        } else {
                            sensorfails_++;
                        }
                        break;

                    default:
                        sensorfails_++;
                        LOG_ERROR(F("Unknown dallas sensor %s"), Sensor(addr).id_str().c_str());
                        break;
                    }
                } else {
                    sensorfails_++;
                    LOG_ERROR(F("Invalid dallas sensor %s"), Sensor(addr).id_str().c_str());
                }
            } else {
                if (!parasite_) {
                    bus_.depower();
                }
                // check for missing sensors after some samples
                if (++scancnt_ > SCAN_MAX) {
                    for (auto & sensor : sensors_) {
                        if (!sensor.read) {
                            sensor.temperature_c = EMS_VALUE_SHORT_NOTSET;
                            changed_             = true;
                        }
                        sensor.read = false;
                    }
                    scancnt_ = 0;
                } else if (scancnt_ == SCAN_START + 1) { // startup
                    firstscan_ = sensors_.size();
                    // LOG_DEBUG(F("Adding %d dallas sensor(s) from first scan"), firstscan_);
                } else if ((scancnt_ <= 0) && (firstscan_ != sensors_.size())) { // check 2 times for no change of sensor #
                    scancnt_ = SCAN_START;
                    sensors_.clear(); // restart scaning and clear to get correct numbering
                }
                state_ = State::IDLE;
            }
        }
    }
#endif
}

bool DallasSensor::temperature_convert_complete() {
#ifndef EMSESP_STANDALONE
    if (parasite_) {
        return true; // don't care, use the minimum time in loop
    }
    return bus_.read_bit() == 1;
#else
    return true;
#endif
}

int16_t DallasSensor::get_temperature_c(const uint8_t addr[]) {
#ifndef EMSESP_STANDALONE
    if (!bus_.reset()) {
        LOG_ERROR(F("Bus reset failed before reading scratchpad from %s"), Sensor(addr).id_str().c_str());
        return EMS_VALUE_SHORT_NOTSET;
    }
    YIELD;

    uint8_t scratchpad[SCRATCHPAD_LEN] = {0};
    bus_.select(addr);
    bus_.write(CMD_READ_SCRATCHPAD);
    bus_.read_bytes(scratchpad, SCRATCHPAD_LEN);
    YIELD;

    if (!bus_.reset()) {
        LOG_ERROR(F("Bus reset failed after reading scratchpad from %s"), Sensor(addr).id_str().c_str());
        return EMS_VALUE_SHORT_NOTSET;
    }
    YIELD;

    if (bus_.crc8(scratchpad, SCRATCHPAD_LEN - 1) != scratchpad[SCRATCHPAD_LEN - 1]) {
        LOG_WARNING(F("Invalid scratchpad CRC: %02X%02X%02X%02X%02X%02X%02X%02X%02X from sensor %s"),
                    scratchpad[0],
                    scratchpad[1],
                    scratchpad[2],
                    scratchpad[3],
                    scratchpad[4],
                    scratchpad[5],
                    scratchpad[6],
                    scratchpad[7],
                    scratchpad[8],
                    Sensor(addr).id_str().c_str());
        return EMS_VALUE_SHORT_NOTSET;
    }

    int16_t raw_value = ((int16_t)scratchpad[SCRATCHPAD_TEMP_MSB] << 8) | scratchpad[SCRATCHPAD_TEMP_LSB];

    if (addr[0] == TYPE_DS18S20) {
        raw_value = (raw_value << 3) + 12 - scratchpad[SCRATCHPAD_CNT_REM];
    } else {
        // Adjust based on sensor resolution
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

// update dallas information name and offset
bool DallasSensor::update(const std::string & id_str, const std::string & name, int16_t offset) {
    // find the sensor
    for (auto & sensor : sensors_) {
        if (sensor.id_str() == id_str) {
            // found a match, update the sensor object

            // if HA is enabled then delete the old record
            if (Mqtt::ha_enabled()) {
                remove_ha_topic(id_str);
            }

            sensor.set_name(name);
            sensor.set_offset(offset);

            // store the new name and offset in our configuration
            EMSESP::webCustomizationService.update(
                [&](WebCustomization & settings) {
                    // look it up to see if it exists
                    bool found = false;
                    for (auto & SensorCustomization : settings.sensorCustomizations) {
                        if (SensorCustomization.id_str == id_str) {
                            SensorCustomization.name   = name;
                            SensorCustomization.offset = offset;
                            found                      = true;
                            LOG_DEBUG(F("Customizing existing sensor ID %s"), id_str.c_str());
                            break;
                        }
                    }
                    if (!found) {
                        SensorCustomization newSensor = SensorCustomization();
                        newSensor.id_str              = id_str;
                        newSensor.name                = name;
                        newSensor.offset              = offset;
                        settings.sensorCustomizations.push_back(newSensor);
                        LOG_DEBUG(F("Adding new customization for sensor ID %s"), id_str.c_str());
                    }
                    sensor.ha_registered = false; // it's changed so we may need to recreate the HA config
                    return StateUpdateResult::CHANGED;
                },
                "local");
            return true;
        }
    }

    return true; // not found, nothing updated
}

// check to see if values have been updated
bool DallasSensor::updated_values() {
    if (changed_) {
        changed_ = false;
        return true;
    }
    return false;
}

// list commands
bool DallasSensor::command_commands(const char * value, const int8_t id, JsonObject & output) {
    return Command::list(EMSdevice::DeviceType::DALLASSENSOR, output);
}

// creates JSON doc from values
// returns false if there are no sensors
bool DallasSensor::command_info(const char * value, const int8_t id, JsonObject & output) {
    if (sensors_.size() == 0) {
        return false;
    }

    for (const auto & sensor : sensors_) {
        if (id == -1) { // show number and id
            JsonObject dataSensor = output.createNestedObject(sensor.name());
            dataSensor["id_str"]  = sensor.id_str();
            if (Helpers::hasValue(sensor.temperature_c)) {
                dataSensor["temp"] = (float)(sensor.temperature_c) / 10;
            }
        } else if (Helpers::hasValue(sensor.temperature_c)) {
            output[sensor.name()] = (float)(sensor.temperature_c) / 10;
        }
    }

    return (output.size() > 0);
}

// called from emsesp.cpp, similar to the emsdevice->get_value_info
bool DallasSensor::get_value_info(JsonObject & output, const char * cmd, const int8_t id) {
    for (const auto & sensor : sensors_) {
        if (strcmp(cmd, sensor.name().c_str()) == 0) {
            output["id_str"] = sensor.id_str();
            output["name"]   = sensor.name();
            if (Helpers::hasValue(sensor.temperature_c)) {
                output["value"] = (float)(sensor.temperature_c) / 10;
            }
            output["type"]      = F_(number);
            output["min"]       = -55;
            output["max"]       = 125;
            output["unit"]      = EMSdevice::uom_to_string(DeviceValueUOM::DEGREES);
            output["writeable"] = false;
            return true;
        }
    }
    return false;
}

// publish a single sensor to MQTT
void DallasSensor::publish_sensor(Sensor sensor) {
    if (Mqtt::publish_single()) {
        char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
        snprintf(topic, sizeof(topic), "%s/%s", read_flash_string(F_(dallassensor)).c_str(), sensor.name().c_str());
        char payload[10];
        Mqtt::publish(topic, Helpers::render_value(payload, sensor.temperature_c, 10, EMSESP::system_.fahrenheit() ? 2 : 0));
    }
}

// send empty config topic to remove the entry from HA
void DallasSensor::remove_ha_topic(const std::string & id_str) {
    if (!Mqtt::ha_enabled()) {
        return;
    }
#ifdef EMSESP_DEBUG
    LOG_DEBUG(F("Removing HA config for sensor ID %s"), id_str.c_str());
#endif
    // use '_' as HA doesn't like '-' in the topic name
    std::string sensorid = id_str;
    std::replace(sensorid.begin(), sensorid.end(), '-', '_');
    char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
    snprintf(topic, sizeof(topic), "sensor/%s/dallassensor_%s/config", Mqtt::base().c_str(), sensorid.c_str());
    Mqtt::publish_ha(topic);
}

// send all dallas sensor values as a JSON package to MQTT
void DallasSensor::publish_values(const bool force) {
    uint8_t num_sensors = sensors_.size();

    if (num_sensors == 0) {
        return;
    }

    if (force && Mqtt::publish_single()) {
        for (const auto & sensor : sensors_) {
            publish_sensor(sensor);
        }
        // return;
    }

    DynamicJsonDocument doc(120 * num_sensors);

    for (auto & sensor : sensors_) {
        bool has_value = Helpers::hasValue(sensor.temperature_c);
        if (Mqtt::is_nested() || Mqtt::ha_enabled()) {
            JsonObject dataSensor = doc.createNestedObject(sensor.id_str());
            dataSensor["name"]    = sensor.name();
            if (has_value) {
                dataSensor["temp"] = (float)(sensor.temperature_c) / 10;
            }
        } else if (has_value) {
            doc[sensor.name()] = (float)(sensor.temperature_c) / 10;
        }

        // create the HA MQTT config
        // to e.g. homeassistant/sensor/ems-esp/dallassensor_28-233D-9497-0C03/config
        if (Mqtt::ha_enabled()) {
            if (!sensor.ha_registered || force) {
                LOG_DEBUG(F("Recreating HA config for sensor ID %s"), sensor.id_str().c_str());

                StaticJsonDocument<EMSESP_JSON_SIZE_MEDIUM> config;
                config["dev_cla"] = FJSON("temperature");

                char stat_t[50];
                snprintf(stat_t, sizeof(stat_t), "%s/dallassensor_data", Mqtt::base().c_str());
                config["stat_t"] = stat_t;

                if (EMSESP::system_.fahrenheit()) {
                    config["unit_of_meas"] = FJSON("°F");
                } else {
                    config["unit_of_meas"] = FJSON("°C");
                }

                char str[50];
                snprintf(str, sizeof(str), "{{value_json['%s'].temp}}", sensor.id_str().c_str());
                config["val_tpl"] = str;

                snprintf(str, sizeof(str), "Temperature Sensor %s", sensor.name().c_str());
                config["name"] = str;

                snprintf(str, sizeof(str), "dallasensor_%s", sensor.id_str().c_str());
                config["uniq_id"] = str;

                JsonObject dev = config.createNestedObject("dev");
                JsonArray  ids = dev.createNestedArray("ids");
                ids.add("ems-esp");

                char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
                // use '_' as HA doesn't like '-' in the topic name
                std::string sensorid = sensor.id_str();
                std::replace(sensorid.begin(), sensorid.end(), '-', '_');

                snprintf(topic, sizeof(topic), "sensor/%s/dallassensor_%s/config", Mqtt::base().c_str(), sensorid.c_str());

                Mqtt::publish_ha(topic, config.as<JsonObject>());

                sensor.ha_registered = true;
            }
        }
    }

    Mqtt::publish(F("dallassensor_data"), doc.as<JsonObject>());
}


// skip crc from id
DallasSensor::Sensor::Sensor(const uint8_t addr[])
    : id_(((uint64_t)addr[0] << 48) | ((uint64_t)addr[1] << 40) | ((uint64_t)addr[2] << 32) | ((uint64_t)addr[3] << 24) | ((uint64_t)addr[4] << 16)
          | ((uint64_t)addr[5] << 8) | ((uint64_t)addr[6])) {
    id_str_ = to_string();   // create ID string
    name_   = std::string{}; // name (alias) is empty
    offset_ = 0;             // 0 degrees offset
}

uint64_t DallasSensor::get_id(const uint8_t addr[]) {
    return (((uint64_t)addr[0] << 48) | ((uint64_t)addr[1] << 40) | ((uint64_t)addr[2] << 32) | ((uint64_t)addr[3] << 24) | ((uint64_t)addr[4] << 16)
            | ((uint64_t)addr[5] << 8) | ((uint64_t)addr[6]));
}

// create's the ID string
std::string DallasSensor::Sensor::to_string() const {
    std::string str(17, '\0');
    snprintf(&str[0],
             str.capacity() + 1,
             "%02X-%04X-%04X-%04X",
             (unsigned int)(id_ >> 48) & 0xFF,
             (unsigned int)(id_ >> 32) & 0xFFFF,
             (unsigned int)(id_ >> 16) & 0xFFFF,
             (unsigned int)(id_)&0xFFFF);
    return str;
}

// find the name from the customization service
// if empty, return the ID as a string
std::string DallasSensor::Sensor::name() const {
    if (name_.empty()) {
        return id_str_;
    }
    return name_;
}

// look up in customization service for a specific sensor
// and set the name and offset from that entry if it exists
bool DallasSensor::Sensor::apply_customization() {
    EMSESP::webCustomizationService.read([&](WebCustomization & settings) {
        auto sensors = settings.sensorCustomizations;
        if (sensors.size() != 0) {
            for (auto & sensor : sensors) {
#if defined(EMSESP_DEBUG)
                LOG_DEBUG("Loading customization for dallas sensor %s", sensor.id_str.c_str());
#endif
                if (id_str_ == sensor.id_str) {
                    set_name(sensor.name);
                    set_offset(sensor.offset);
                    return true;
                }
            }
        }
        return false;
    });

    return false; // not found, will use default ID as name and 0 for offset
}

// hard coded tests
#ifdef EMSESP_DEBUG
void DallasSensor::test() {
    // add 2 dallas sensors
    uint8_t addr[ADDR_LEN] = {1, 2, 3, 4, 5, 6, 7, 8};
    sensors_.emplace_back(addr);
    sensors_.back().temperature_c = 123;
    sensors_.back().read          = true;
    sensors_.back().apply_customization();

    uint8_t addr2[ADDR_LEN] = {11, 12, 13, 14, 15, 16, 17, 18};
    sensors_.emplace_back(addr2);
    sensors_.back().temperature_c = 456;
    sensors_.back().read          = true;
    sensors_.back().apply_customization();
}
#endif

} // namespace emsesp
