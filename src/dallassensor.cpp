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
    LOG_INFO(F("Starting Dallas sensor service"));
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

    Mqtt::subscribe(EMSdevice::DeviceType::DALLASSENSOR, "dallasssensor/#", nullptr); // use empty function callback
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
                                if (sensor.internal_id() == get_id(addr)) {
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
                                // look in the customization service for an optional alias or offset for that particular sensor
                                sensors_.back().apply_customization();
                                publish_sensor(sensors_.back()); // call publish single
                                // sort the sensors based on name
                                // std::sort(sensors_.begin(), sensors_.end(), [](const Sensor & a, const Sensor & b) { return a.name() < b.name(); });
                            }
                        } else {
                            sensorfails_++;
                        }
                        break;

                    default:
                        sensorfails_++;
                        LOG_ERROR(F("Unknown dallas sensor %s"), Sensor(addr).id().c_str());
                        break;
                    }
                } else {
                    sensorfails_++;
                    LOG_ERROR(F("Invalid dallas sensor %s"), Sensor(addr).id().c_str());
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
        LOG_ERROR(F("Bus reset failed before reading scratchpad from %s"), Sensor(addr).id().c_str());
        return EMS_VALUE_SHORT_NOTSET;
    }
    YIELD;

    uint8_t scratchpad[SCRATCHPAD_LEN] = {0};
    bus_.select(addr);
    bus_.write(CMD_READ_SCRATCHPAD);
    bus_.read_bytes(scratchpad, SCRATCHPAD_LEN);
    YIELD;

    if (!bus_.reset()) {
        LOG_ERROR(F("Bus reset failed after reading scratchpad from %s"), Sensor(addr).id().c_str());
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
                    Sensor(addr).id().c_str());
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
bool DallasSensor::update(const std::string & id, const std::string & name, int16_t offset) {
    // find the sensor
    for (auto & sensor : sensors_) {
        if (sensor.id() == id) {
            // found a match, update the sensor object

            // if HA is enabled then delete the old record
            if (Mqtt::ha_enabled()) {
                remove_ha_topic(id);
            }

            sensor.set_name(name);
            sensor.set_offset(offset);

            // store the new name and offset in our configuration
            EMSESP::webCustomizationService.update(
                [&](WebCustomization & settings) {
                    // look it up to see if it exists
                    bool found = false;
                    for (auto & SensorCustomization : settings.sensorCustomizations) {
                        if (SensorCustomization.id == id) {
                            SensorCustomization.name   = name;
                            SensorCustomization.offset = offset;
                            found                      = true;
                            LOG_DEBUG(F("Customizing existing sensor ID %s"), id.c_str());
                            break;
                        }
                    }
                    if (!found) {
                        SensorCustomization newSensor = SensorCustomization();
                        newSensor.id                  = id;
                        newSensor.name                = name;
                        newSensor.offset              = offset;
                        settings.sensorCustomizations.push_back(newSensor);
                        LOG_DEBUG(F("Adding new customization for sensor ID %s"), id.c_str());
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
    if (sensors_.empty()) {
        return false;
    }

    for (const auto & sensor : sensors_) {
        if (id == -1) { // show number and id
            JsonObject dataSensor = output.createNestedObject(sensor.name());
            dataSensor["id"]      = sensor.id();
            if (Helpers::hasValue(sensor.temperature_c)) {
                dataSensor["temp"] = Helpers::transformNumFloat((float)(sensor.temperature_c), 10, EMSESP::system_.fahrenheit() ? 2 : 0);
            }
        } else if (Helpers::hasValue(sensor.temperature_c)) {
            output[sensor.name()] = Helpers::transformNumFloat((float)(sensor.temperature_c), 10, EMSESP::system_.fahrenheit() ? 2 : 0);
        }
    }

    return (output.size() > 0);
}

// called from emsesp.cpp, similar to the emsdevice->get_value_info
bool DallasSensor::get_value_info(JsonObject & output, const char * cmd, const int8_t id) {
    // make a copy of the string command for parsing
    char command_s[30];
    strlcpy(command_s, cmd, sizeof(command_s));
    char * attribute_s = nullptr;

    // check specific attribute to fetch instead of the complete record
    char * breakp = strchr(command_s, '/');
    if (breakp) {
        *breakp     = '\0';
        attribute_s = breakp + 1;
    }

    for (const auto & sensor : sensors_) {
        if (strcmp(command_s, sensor.name().c_str()) == 0) {
            output["id"]   = sensor.id();
            output["name"] = sensor.name();
            if (Helpers::hasValue(sensor.temperature_c)) {
                output["value"] = Helpers::transformNumFloat((float)(sensor.temperature_c), 10, EMSESP::system_.fahrenheit() ? 2 : 0);
            }
            output["type"]      = F_(number);
            output["min"]       = Helpers::transformNumFloat(-55, 0, EMSESP::system_.fahrenheit() ? 2 : 0);
            output["max"]       = Helpers::transformNumFloat(125, 0, EMSESP::system_.fahrenheit() ? 2 : 0);
            output["uom"]       = EMSdevice::uom_to_string(DeviceValueUOM::DEGREES);
            output["writeable"] = false;
            // if we're filtering on an attribute, go find it
            if (attribute_s) {
                if (output.containsKey(attribute_s)) {
                    JsonVariant data = output[attribute_s];
                    output.clear();
                    output["api_data"] = data;
                    return true;
                } else {
                    char error[100];
                    snprintf(error, sizeof(error), "cannot find attribute %s in entity %s", attribute_s, command_s);
                    output.clear();
                    output["message"] = error;
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}

// publish a single sensor to MQTT
void DallasSensor::publish_sensor(const Sensor & sensor) {
    if (Mqtt::publish_single()) {
        char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
        if (Mqtt::publish_single2cmd()) {
            snprintf(topic, sizeof(topic), "%s/%s", read_flash_string(F_(dallassensor)).c_str(), sensor.name().c_str());
        } else {
            snprintf(topic, sizeof(topic), "%s%s/%s", read_flash_string(F_(dallassensor)).c_str(), "_data", sensor.name().c_str());
        }
        char payload[10];
        Mqtt::publish(topic, Helpers::render_value(payload, sensor.temperature_c, 10, EMSESP::system_.fahrenheit() ? 2 : 0));
    }
}

// send empty config topic to remove the entry from HA
void DallasSensor::remove_ha_topic(const std::string & id) {
    if (!Mqtt::ha_enabled()) {
        return;
    }
#ifdef EMSESP_DEBUG
    LOG_DEBUG(F("Removing HA config for temperature sensor ID %s"), id.c_str());
#endif
    // use '_' as HA doesn't like '-' in the topic name
    std::string sensorid = id;
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
    }

    DynamicJsonDocument doc(120 * num_sensors);

    for (auto & sensor : sensors_) {
        bool has_value = Helpers::hasValue(sensor.temperature_c);
        if (Mqtt::is_nested() || Mqtt::ha_enabled()) {
            JsonObject dataSensor = doc.createNestedObject(sensor.id());
            dataSensor["name"]    = sensor.name();
            if (has_value) {
                dataSensor["temp"] = Helpers::transformNumFloat((float)(sensor.temperature_c), 10, EMSESP::system_.fahrenheit() ? 2 : 0);
            }
        } else if (has_value) {
            doc[sensor.name()] = Helpers::transformNumFloat((float)(sensor.temperature_c), 10, EMSESP::system_.fahrenheit() ? 2 : 0);
        }

        // create the HA MQTT config
        // to e.g. homeassistant/sensor/ems-esp/dallassensor_28-233D-9497-0C03/config
        if (Mqtt::ha_enabled()) {
            if (!sensor.ha_registered || force) {
                LOG_DEBUG(F("Recreating HA config for sensor ID %s"), sensor.id().c_str());

                StaticJsonDocument<EMSESP_JSON_SIZE_MEDIUM> config;
                config["dev_cla"] = FJSON("temperature");

                char stat_t[50];
                snprintf(stat_t, sizeof(stat_t), "%s/dallassensor_data", Mqtt::base().c_str());
                config["stat_t"] = stat_t;

                config["unit_of_meas"] = EMSdevice::uom_to_string(DeviceValueUOM::DEGREES);

                char str[50];
                snprintf(str, sizeof(str), "{{value_json['%s'].temp}}", sensor.id().c_str());
                config["val_tpl"] = str;

                snprintf(str, sizeof(str), "temperature_sensor_%s", sensor.name().c_str());
                config["object_id"] = str;

                snprintf(str, sizeof(str), "%s", sensor.name().c_str());
                config["name"] = str;

                snprintf(str, sizeof(str), "dallasensor_%s", sensor.id().c_str());
                config["uniq_id"] = str;

                JsonObject dev = config.createNestedObject("dev");
                JsonArray  ids = dev.createNestedArray("ids");
                ids.add("ems-esp");

                char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
                // use '_' as HA doesn't like '-' in the topic name
                std::string sensorid = sensor.id();
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
    : internal_id_(((uint64_t)addr[0] << 48) | ((uint64_t)addr[1] << 40) | ((uint64_t)addr[2] << 32) | ((uint64_t)addr[3] << 24) | ((uint64_t)addr[4] << 16)
                   | ((uint64_t)addr[5] << 8) | ((uint64_t)addr[6])) {
    // create ID string
    char id_s[20];
    snprintf(id_s,
             sizeof(id_s),
             "%02X-%04X-%04X-%04X",
             (unsigned int)(internal_id_ >> 48) & 0xFF,
             (unsigned int)(internal_id_ >> 32) & 0xFFFF,
             (unsigned int)(internal_id_ >> 16) & 0xFFFF,
             (unsigned int)(internal_id_)&0xFFFF);
    id_     = std::string(id_s);
    name_   = std::string{}; // name (alias) is empty
    offset_ = 0;             // 0 degrees offset
}

uint64_t DallasSensor::get_id(const uint8_t addr[]) {
    return (((uint64_t)addr[0] << 48) | ((uint64_t)addr[1] << 40) | ((uint64_t)addr[2] << 32) | ((uint64_t)addr[3] << 24) | ((uint64_t)addr[4] << 16)
            | ((uint64_t)addr[5] << 8) | ((uint64_t)addr[6]));
}

// find the name from the customization service
// if empty, return the ID as a string
std::string DallasSensor::Sensor::name() const {
    if (name_.empty()) {
        return id_;
    }
    return name_;
}

// look up in customization service for a specific sensor
// and set the name and offset from that entry if it exists
bool DallasSensor::Sensor::apply_customization() {
    EMSESP::webCustomizationService.read([&](WebCustomization & settings) {
        auto sensors = settings.sensorCustomizations;
        if (!sensors.empty()) {
            for (const auto & sensor : sensors) {
#if defined(EMSESP_DEBUG)
                LOG_DEBUG(F("Loading customization for dallas sensor %s"), sensor.id.c_str());
#endif
                if (id_ == sensor.id) {
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
