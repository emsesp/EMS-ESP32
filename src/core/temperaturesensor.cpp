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

// code originally written by nomis - https://github.com/nomis

#include "temperaturesensor.h"
#include "emsesp.h"

#ifdef ESP32
#define YIELD
#else
#define YIELD yield()
#endif

namespace emsesp {

uuid::log::Logger TemperatureSensor::logger_{F_(temperaturesensor), uuid::log::Facility::DAEMON};

// start the 1-wire
void TemperatureSensor::start() {
    reload();

    if (!dallas_gpio_) {
        sensors_.clear();
        return; // disabled if dallas gpio is 0
    }

#ifndef EMSESP_STANDALONE
    bus_.begin(dallas_gpio_);
    LOG_INFO("Starting Temperature Sensor service");
#endif

    char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
    snprintf(topic, sizeof(topic), "%s/#", F_(temperaturesensor));
    Mqtt::subscribe(EMSdevice::DeviceType::TEMPERATURESENSOR, topic, nullptr); // use empty function callback
}

// load settings
void TemperatureSensor::reload() {
    // load the service settings
    EMSESP::webSettingsService.read([&](WebSettings const & settings) {
        dallas_gpio_ = settings.dallas_gpio;
        parasite_    = settings.dallas_parasite;
    });

    for (auto & sensor : sensors_) {
        remove_ha_topic(sensor.id());
        sensor.ha_registered = false; // force HA configs to be re-created
    }
}

void TemperatureSensor::loop() {
    if (!dallas_gpio_) {
        return; // dallas gpio is 0 (disabled)
    }

#ifndef EMSESP_STANDALONE
    uint32_t time_now = uuid::get_uptime();

    if (state_ == State::IDLE) {
        if (time_now - last_activity_ >= READ_INTERVAL_MS) {
#ifdef EMSESP_DEBUG_SENSOR
            LOG_DEBUG("Read sensor temperature");
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
                        LOG_DEBUG("Error: Bus reset failed");
#endif
#ifndef EMSESP_TEST
                        // don't reset if running in test mode where we simulate sensors
                        for (auto & sensor : sensors_) {
                            sensor.temperature_c = EMS_VALUE_INT16_NOTSET;
                        }
#endif
                    }
                }
            }
            last_activity_ = time_now;
        }
    } else if (state_ == State::READING) {
        if (temperature_convert_complete() && (time_now - last_activity_ > CONVERSION_MS)) {
#ifdef EMSESP_DEBUG_SENSOR
            LOG_DEBUG("Scanning for temperature sensors");
#endif
            bus_.reset_search();
            state_ = State::SCANNING;
        } else if (time_now - last_activity_ > READ_TIMEOUT_MS) {
#ifdef EMSESP_DEBUG_SENSOR
            LOG_WARNING("Sensor read timeout");
#endif
            state_ = State::IDLE;
            sensorfails_++;
        }
    } else if (state_ == State::SCANNING) {
        if (time_now - last_activity_ > SCAN_TIMEOUT_MS) {
#ifdef EMSESP_DEBUG_SENSOR
            LOG_ERROR("Sensor scan timeout");
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
                                        sensor.temperature_c = t;
                                        publish_sensor(sensor);
                                        changed_ |= true;
                                    }
                                    sensor.read = true;
                                    found       = true;
                                    break;
                                }
                            }
                            // add new sensor. this will create the id string, empty name and offset
                            if (!found && (sensors_.size() < (MAX_SENSORS - 1))) {
                                sensors_.emplace_back(addr);
                                sensors_.back().read = true;
                                changed_             = true;
                                // look in the customization service for an optional alias or offset for that particular sensor
                                sensors_.back().apply_customization();
                                sensors_.back().temperature_c = t + sensors_.back().offset();
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
                        LOG_ERROR("Unknown sensor %s", Sensor(addr).id().c_str());
                        break;
                    }
                } else {
                    sensorfails_++;
                    LOG_ERROR("Invalid sensor %s", Sensor(addr).id().c_str());
                }
            } else {
                if (!parasite_) {
                    bus_.depower();
                }
                // check for missing sensors after some samples
                // but don't do this if running in test mode where we simulate sensors
                if (++scancnt_ > SCAN_MAX) {
                    for (auto & sensor : sensors_) {
                        if (!sensor.read) {
                            sensor.temperature_c = EMS_VALUE_INT16_NOTSET;
                            changed_             = true;
                        }
                        sensor.read = false;
                    }
                    scancnt_ = 0;
                } else if (scancnt_ == SCAN_START + 1) { // startup
                    firstscan_ = sensors_.size();
                    // LOG_DEBUG("Adding %d sensor(s) from first scan", firstscan_);
                } else if ((scancnt_ <= 0) && (firstscan_ != sensors_.size())) { // check 2 times for no change of sensor #
                    scancnt_ = SCAN_START;
                    sensors_.clear(); // restart scanning and clear to get correct numbering
                }
                state_ = State::IDLE;
            }
        }
    }
#endif
}

bool TemperatureSensor::temperature_convert_complete() {
#ifndef EMSESP_STANDALONE
    if (parasite_) {
        return true; // don't care, use the minimum time in loop
    }
    return bus_.read_bit() == 1;
#else
    return true;
#endif
}

int16_t TemperatureSensor::get_temperature_c(const uint8_t addr[]) {
#ifndef EMSESP_STANDALONE
    if (!bus_.reset()) {
        LOG_ERROR("Bus reset failed before reading scratchpad from %s", Sensor(addr).id().c_str());
        return EMS_VALUE_INT16_NOTSET;
    }
    YIELD;

    uint8_t scratchpad[SCRATCHPAD_LEN] = {0};
    bus_.select(addr);
    bus_.write(CMD_READ_SCRATCHPAD);
    bus_.read_bytes(scratchpad, SCRATCHPAD_LEN);
    YIELD;

    if (!bus_.reset()) {
        LOG_ERROR("Bus reset failed after reading scratchpad from %s", Sensor(addr).id().c_str());
        return EMS_VALUE_INT16_NOTSET;
    }
    YIELD;

    if (bus_.crc8(scratchpad, SCRATCHPAD_LEN - 1) != scratchpad[SCRATCHPAD_LEN - 1]) {
        LOG_WARNING("Invalid scratchpad CRC: %02X%02X%02X%02X%02X%02X%02X%02X%02X from sensor %s",
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
        return EMS_VALUE_INT16_NOTSET;
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
        default: // 12
            break;
        }
    }
    raw_value = ((int32_t)raw_value * 625 + 500) / 1000; // round to 0.1
    return raw_value;
#else
    return EMS_VALUE_INT16_NOTSET;
#endif
}

// update temperature sensor information name and offset
bool TemperatureSensor::update(const std::string & id, const std::string & name, int16_t offset) {
    // find the sensor
    for (auto & sensor : sensors_) {
        if (sensor.id() == id) {
            // found a match, update the sensor object

            // if HA is enabled then delete the old record
            if (Mqtt::ha_enabled()) {
                remove_ha_topic(id);
                sensor.ha_registered = false; // force HA configs to be re-created
            }

            sensor.set_name(name);
            sensor.set_offset(offset);

            // store the new name and offset in our configuration
            EMSESP::webCustomizationService.update([&id, &name, &offset, &sensor](WebCustomization & settings) {
                // look it up to see if it exists
                bool found = false;
                for (auto & SensorCustomization : settings.sensorCustomizations) {
                    if (SensorCustomization.id == id) {
                        SensorCustomization.name   = name;
                        SensorCustomization.offset = offset;
                        found                      = true;
                        LOG_DEBUG("Customizing existing sensor ID %s", id.c_str());
                        break;
                    }
                }
                if (!found) {
                    auto newSensor   = SensorCustomization();
                    newSensor.id     = id;
                    newSensor.name   = name;
                    newSensor.offset = offset;
                    settings.sensorCustomizations.push_back(newSensor);
                    LOG_DEBUG("Adding new customization for sensor ID %s", id.c_str());
                }
                sensor.ha_registered = false; // it's changed so we may need to recreate the HA config
                return StateUpdateResult::CHANGED;
            });
            return true;
        }
    }

    return true; // not found, nothing updated
}

// check to see if values have been updated
bool TemperatureSensor::updated_values() {
    if (changed_) {
        changed_ = false;
        return true;
    }
    return false;
}

// called from emsesp.cpp for commands
bool TemperatureSensor::get_value_info(JsonObject output, const char * cmd, const int8_t id) {
    // return empty json if there are no sensors
    if (sensors_.empty()) {
        return true;
    }

    if (!strcmp(cmd, F_(info)) || !strcmp(cmd, F_(values))) {
        for (const auto & sensor : sensors_) {
            if (Helpers::hasValue(sensor.temperature_c)) {
                char val[10];
                output[sensor.name()] = serialized(Helpers::render_value(val, sensor.temperature_c, 10, EMSESP::system_.fahrenheit() ? 2 : 0));
            }
        }
        return true;
    }

    if (!strcmp(cmd, F_(entities))) {
        for (const auto & sensor : sensors_) {
            get_value_json(output[sensor.name()].to<JsonObject>(), sensor);
        }
        return true;
    }

    // this is for a specific sensor
    const char * attribute_s = Command::get_attribute(cmd);

    for (const auto & sensor : sensors_) {
        // match custom name or sensor ID
        if (cmd == Helpers::toLower(sensor.name()) || cmd == Helpers::toLower(sensor.id())) {
            get_value_json(output, sensor);
            return Command::set_attribute(output, cmd, attribute_s);
        }
    }

    return false; // not found
}

// note we don't add the device and state classes here, as we do in the custom entity service
void TemperatureSensor::get_value_json(JsonObject output, const Sensor & sensor) {
    output["id"]       = sensor.id();
    output["name"]     = sensor.name();
    output["fullname"] = sensor.name();
    if (Helpers::hasValue(sensor.temperature_c)) {
        char val[10];
        output["value"] = serialized(Helpers::render_value(val, sensor.temperature_c, 10, EMSESP::system_.fahrenheit() ? 2 : 0));
    }

    output["type"]      = F_(number);
    output["uom"]       = EMSdevice::uom_to_string(DeviceValueUOM::DEGREES);
    output["readable"]  = true;
    output["writeable"] = false;
    output["visible"]   = true;
}

// publish a single sensor to MQTT
void TemperatureSensor::publish_sensor(const Sensor & sensor) {
    if (Mqtt::enabled() && Mqtt::publish_single()) {
        char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
        if (Mqtt::publish_single2cmd()) {
            snprintf(topic, sizeof(topic), "%s/%s", F_(temperaturesensor), sensor.name().c_str());
        } else {
            snprintf(topic, sizeof(topic), "%s%s/%s", F_(temperaturesensor), "_data", sensor.name().c_str());
        }
        char payload[10];
        Mqtt::queue_publish(topic, Helpers::render_value(payload, sensor.temperature_c, 10, EMSESP::system_.fahrenheit() ? 2 : 0));
    }
    char cmd[COMMAND_MAX_LENGTH];
    snprintf(cmd, sizeof(cmd), "%s/%s", F_(temperaturesensor), sensor.name().c_str());
    EMSESP::webSchedulerService.onChange(cmd);
}

// send empty config topic to remove the entry from HA
void TemperatureSensor::remove_ha_topic(const std::string & id) {
    if (!Mqtt::ha_enabled()) {
        return;
    }

    LOG_DEBUG("Removing HA config for temperature sensor ID %s", id.c_str());
    // use '_' as HA doesn't like '-' in the topic name
    std::string sensorid = id;
    std::replace(sensorid.begin(), sensorid.end(), '-', '_');
    char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
    snprintf(topic, sizeof(topic), "sensor/%s/temperaturesensor_%s/config", Mqtt::basename().c_str(), sensorid.c_str());
    Mqtt::queue_remove_topic(topic);
}

// send all temperature sensor values as a JSON package to MQTT
void TemperatureSensor::publish_values(const bool force) {
    if (!Mqtt::enabled()) {
        return;
    }

    uint8_t num_sensors = sensors_.size();
    if (num_sensors == 0) {
        return;
    }

    if (force && Mqtt::publish_single()) {
        for (const auto & sensor : sensors_) {
            publish_sensor(sensor);
        }
    }

    JsonDocument doc;

    for (auto & sensor : sensors_) {
        bool has_value = Helpers::hasValue(sensor.temperature_c);
        if (has_value) {
            char val[10];
            if (Mqtt::is_nested()) {
                JsonObject dataSensor = doc[sensor.id()].to<JsonObject>();
                dataSensor["name"]    = sensor.name();
                dataSensor["temp"]    = serialized(Helpers::render_value(val, sensor.temperature_c, 10, EMSESP::system_.fahrenheit() ? 2 : 0));
            } else {
                doc[sensor.name()] = serialized(Helpers::render_value(val, sensor.temperature_c, 10, EMSESP::system_.fahrenheit() ? 2 : 0));
            }
        }

        // create the HA MQTT config
        // to e.g. homeassistant/sensor/ems-esp/temperaturesensor_28-233D-9497-0C03/config
        if (Mqtt::ha_enabled()) {
            if (!has_value && sensor.ha_registered) {
                remove_ha_topic(sensor.id());
                sensor.ha_registered = false;
            } else if (!sensor.ha_registered || force) {
                LOG_DEBUG("Recreating HA config for sensor ID %s", sensor.id().c_str());

                JsonDocument config;
                config["dev_cla"]  = "temperature";
                config["stat_cla"] = "measurement";

                char stat_t[50];
                snprintf(stat_t, sizeof(stat_t), "%s/%s_data", Mqtt::base().c_str(), F_(temperaturesensor)); // use base path
                config["stat_t"] = stat_t;

                config["unit_of_meas"] = EMSdevice::uom_to_string(DeviceValueUOM::DEGREES);

                char val_obj[70];
                char val_cond[170];
                if (Mqtt::is_nested()) {
                    snprintf(val_obj, sizeof(val_obj), "value_json['%s']['temp']", sensor.id().c_str());
                    snprintf(val_cond, sizeof(val_cond), "value_json['%s'] is defined and %s is defined", sensor.id().c_str(), val_obj);
                } else {
                    snprintf(val_obj, sizeof(val_obj), "value_json['%s']", sensor.name().c_str());
                    snprintf(val_cond, sizeof(val_cond), "%s is defined", val_obj);
                }

                // for the value template, there's a problem still with Domoticz probably due to the special characters.
                // See https://github.com/emsesp/EMS-ESP32/issues/1360
                if (Mqtt::discovery_type() == Mqtt::discoveryType::HOMEASSISTANT) {
                    config["val_tpl"] = (std::string) "{{" + val_obj + " if " + val_cond + " else -55}}";
                } else {
                    config["val_tpl"] = (std::string) "{{" + val_obj + "}}"; // omit value conditional Jinja2 template code
                }

                char uniq_s[70];
                if (Mqtt::entity_format() == Mqtt::entityFormat::MULTI_SHORT) {
                    snprintf(uniq_s, sizeof(uniq_s), "%s_%s_%s", Mqtt::basename().c_str(), F_(temperaturesensor), sensor.id().c_str());
                } else {
                    snprintf(uniq_s, sizeof(uniq_s), "%s_%s", F_(temperaturesensor), sensor.id().c_str());
                }

                config["obj_id"]  = uniq_s;
                config["uniq_id"] = uniq_s; // same as object_id/obj_id

                char name[50];
                snprintf(name, sizeof(name), "%s", sensor.name().c_str());
                config["name"] = name;

                // see if we need to create the [devs] discovery section, as this needs only to be done once for all sensors
                bool is_ha_device_created = false;
                for (const auto & sensor : sensors_) {
                    if (sensor.ha_registered) {
                        is_ha_device_created = true;
                        break;
                    }
                }

                Mqtt::add_ha_sections_to_doc("temperature", stat_t, config, !is_ha_device_created, val_cond);

                char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
                snprintf(topic, sizeof(topic), "sensor/%s/%s_%s/config", Mqtt::basename().c_str(), F_(temperaturesensor), sensor.id().c_str());

                sensor.ha_registered = Mqtt::queue_ha(topic, config.as<JsonObject>());
            }
        }
    }

    char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
    snprintf(topic, sizeof(topic), "%s_data", F_(temperaturesensor));
    Mqtt::queue_publish(topic, doc.as<JsonObject>());
}

// skip crc from id
TemperatureSensor::Sensor::Sensor(const uint8_t addr[])
    : internal_id_(((uint64_t)addr[0] << 48) | ((uint64_t)addr[1] << 40) | ((uint64_t)addr[2] << 32) | ((uint64_t)addr[3] << 24) | ((uint64_t)addr[4] << 16)
                   | ((uint64_t)addr[5] << 8) | ((uint64_t)addr[6])) {
    // create ID string
    char id_s[20];
    snprintf(id_s,
             sizeof(id_s),
             "%02X_%04X_%04X_%04X",
             (unsigned int)(internal_id_ >> 48) & 0xFF,
             (unsigned int)(internal_id_ >> 32) & 0xFFFF,
             (unsigned int)(internal_id_ >> 16) & 0xFFFF,
             (unsigned int)(internal_id_) & 0xFFFF);
    id_     = std::string(id_s);
    name_   = std::string{}; // name (alias) is empty
    offset_ = 0;             // 0 degrees offset
}

uint64_t TemperatureSensor::get_id(const uint8_t addr[]) {
    return (((uint64_t)addr[0] << 48) | ((uint64_t)addr[1] << 40) | ((uint64_t)addr[2] << 32) | ((uint64_t)addr[3] << 24) | ((uint64_t)addr[4] << 16)
            | ((uint64_t)addr[5] << 8) | ((uint64_t)addr[6]));
}

// find the name from the customization service
// if empty, return the ID as a string
std::string TemperatureSensor::Sensor::name() const {
    if (name_.empty()) {
        return id_;
    }
    return name_;
}

// look up in customization service for a specific sensor
// and set the name and offset from that entry if it exists
bool TemperatureSensor::Sensor::apply_customization() {
    EMSESP::webCustomizationService.read([&](const WebCustomization & settings) {
        auto const & sensors = settings.sensorCustomizations;
        if (!sensors.empty()) {
            for (const auto & sensor : sensors) {
                if (id_ == sensor.id) {
                    LOG_DEBUG("Loading customization for temperature sensor %s", sensor.id.c_str());
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
#if defined(EMSESP_TEST)
void TemperatureSensor::test() {
    // add 2 temperature sensors
    // Sensor ID: 01_0203_0405_0607
    uint8_t addr[ADDR_LEN] = {1, 2, 3, 4, 5, 6, 7, 8};
    sensors_.emplace_back(addr);
    sensors_.back().apply_customization();
    sensors_.back().temperature_c = 123; // 12.3
    sensors_.back().read          = true;
    publish_sensor(sensors_.back()); // call publish single

    // Sensor ID: 0B_0C0D_0E0F_1011
    uint8_t addr2[ADDR_LEN] = {11, 12, 13, 14, 15, 16, 17, 18};
    sensors_.emplace_back(addr2);
    sensors_.back().apply_customization();
    sensors_.back().temperature_c = 456; // 45.6
    sensors_.back().read          = true;
    publish_sensor(sensors_.back()); // call publish single
}
#endif

} // namespace emsesp