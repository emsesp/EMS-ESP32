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

#include "analogsensor.h"
#include "emsesp.h"

namespace emsesp {

uuid::log::Logger AnalogSensor::logger_{F_(analogsensor), uuid::log::Facility::DAEMON};

void AnalogSensor::start() {
    reload(); // fetch the list of sensors from our customization service

    if (analog_enabled_) {
        analogSetAttenuation(ADC_2_5db); // for all channels
    }

    // Add API call for /info
    Command::add(
        EMSdevice::DeviceType::ANALOGSENSOR,
        F_(info),
        [&](const char * value, const int8_t id, JsonObject & output) { return command_info(value, id, output); },
        F_(info_cmd));
}

// load settings from the customization file, sorts them and initializes the GPIOs
void AnalogSensor::reload() {
    EMSESP::webSettingsService.read([&](WebSettings & settings) { analog_enabled_ = settings.analog_enabled; });

#if defined(EMSESP_STANDALONE)
    analog_enabled_ = true; // for local offline testing
#endif

    // load the list of analog sensors from the customization service
    // and store them locally and then activate them
    EMSESP::webCustomizationService.read([&](WebCustomization & settings) {
        auto sensors = settings.analogCustomizations;
        sensors_.clear(); // start with an empty list
        if (sensors.size() != 0) {
            for (auto & sensor : sensors) {
                sensors_.emplace_back(sensor.id, sensor.name, sensor.offset, sensor.factor, sensor.uom, sensor.type);
                sensors_.back().ha_registered = false; // this will trigger recrate of the HA config
            }
        }
        return true;
    });

    // sort the list based on GPIO (id)
    std::sort(sensors_.begin(), sensors_.end(), [](const Sensor & a, const Sensor & b) { return a.id() < b.id(); });

    // activate each sensor
    for (auto & sensor : sensors_) {
        sensor.ha_registered = false; // force HA configs to be re-created
        if (sensor.type() == AnalogType::ADC) {
            LOG_DEBUG(F("Adding analog ADC sensor on GPIO%d"), sensor.id());
            // note sensor.id is the GPIO
            analogSetPinAttenuation(sensor.id(), ADC_2_5db); // 1500mV

            // MichaelDvP's comments:
            // setting attenuator to 0 = 1100, 1500, 2200, 3900 mV
            // analogSetAttenuation(sensor.id(), analog_enabled_ - 1);
        } else if (sensor.type() == AnalogType::IOCOUNTER) {
            LOG_DEBUG(F("Adding analog I/O Counter sensor on GPIO%d"), sensor.id());
            pinMode(sensor.id(), INPUT_PULLUP);
            sensor.set_value(0); // reset count
            sensor.set_uom(0);   // no uom, just for safe measures
        } else if (sensor.type() == AnalogType::READ) {
            LOG_DEBUG(F("Adding analog Read sensor on GPIO%d"), sensor.id());
            // pinMode(sensor.id(), INPUT_PULLUP);
            sensor.set_value(0); // initial value
            sensor.set_uom(0);   // no uom, just for safe measures
        }
    }
}

// measure and moving average adc
void AnalogSensor::measure() {
    static uint32_t measure_last_ = 0;

    if (!measure_last_ || (uint32_t)(uuid::get_uptime() - measure_last_) >= MEASURE_ANALOG_INTERVAL) {
        measure_last_ = uuid::get_uptime();

        // go through the list of sensors
        for (auto & sensor : sensors_) {
            auto old_value = sensor.value(); // remember current value before reading

            if (sensor.type() == AnalogType::ADC) {
                uint16_t a = analogReadMilliVolts(sensor.id()); // e.g. ADC1_CHANNEL_0_GPIO_NUM
                if (!sensor.analog_) {                          // init first time
                    sensor.analog_ = a;
                    sensor.sum_    = a * 512;
                } else { // simple moving average filter
                    sensor.sum_    = (sensor.sum_ * 511) / 512 + a;
                    sensor.analog_ = sensor.sum_ / 512;
                }
                sensor.set_value((sensor.analog_ - sensor.offset()) * sensor.factor());
            } else if (sensor.type() == AnalogType::READ) {
                sensor.set_value(digitalRead(sensor.id()));
            } else if (sensor.type() == AnalogType::IOCOUNTER) {
                // capture reading and compare with the last one to see if there is high/low change
                // add back the debounce uuid::get_uptime() diff > 15
                uint16_t current_reading = digitalRead(sensor.id());
                if (!current_reading && current_reading != sensor.last_reading_) {
                    sensor.set_value(old_value + 1);
                    sensor.last_reading_ = current_reading;
                }
            }

            // see if there is a change and increment # reads
            if (old_value != sensor.value()) {
                sensorreads_++;
                changed_ = true;
            }
        }
    }
}

void AnalogSensor::loop() {
    if (!analog_enabled_) {
        return;
    }

    measure(); // take the measurements
}

// update analog information name and offset
bool AnalogSensor::update(uint8_t id, const std::string & name, int16_t offset, float factor, uint8_t uom, int8_t type) {
    boolean found_sensor = false; // see if we can find the sensor in our customization list

    EMSESP::webCustomizationService.update(
        [&](WebCustomization & settings) {
            for (auto & AnalogCustomization : settings.analogCustomizations) {
                if (AnalogCustomization.id == id) {
                    found_sensor = true; // found the record
                    // see if it's marked for deletion
                    if (type == AnalogType::MARK_DELETED) {
                        LOG_DEBUG(F("Removing analog sensor ID %d"), id);
                        settings.analogCustomizations.remove(AnalogCustomization);
                    } else {
                        // update existing record
                        AnalogCustomization.name   = name;
                        AnalogCustomization.offset = offset;
                        AnalogCustomization.factor = factor;
                        AnalogCustomization.uom    = uom;
                        AnalogCustomization.type   = type;
                        LOG_DEBUG(F("Customizing existing analog sensor ID %d"), id);
                    }
                    return StateUpdateResult::CHANGED; // persist the change
                }
            }
            return StateUpdateResult::UNCHANGED;
        },
        "local");

    // if the sensor exists and we're using HA, delete the old HA record
    if (found_sensor && Mqtt::ha_enabled()) {
        remove_ha_topic(id); // id is the GPIO
    }

    // we didn't find it, it's new, so create and store it
    if (!found_sensor) {
        EMSESP::webCustomizationService.update(
            [&](WebCustomization & settings) {
                AnalogCustomization newSensor = AnalogCustomization();
                newSensor.id                  = id;
                newSensor.name                = name;
                newSensor.offset              = offset;
                newSensor.factor              = factor;
                newSensor.uom                 = uom;
                newSensor.type                = type;
                settings.analogCustomizations.push_back(newSensor);
                LOG_DEBUG(F("Adding new customization for analog sensor ID %d"), id);
                return StateUpdateResult::CHANGED; // persist the change
            },
            "local");
    }

    // reloads the sensors in the customizations file into the sensors list
    reload();

    return true;
}

// check to see if values have been updated
bool AnalogSensor::updated_values() {
    if (changed_) {
        changed_ = false;
        return true;
    }
    return false;
}

// publish a single sensor to MQTT
void AnalogSensor::publish_sensor(Sensor sensor) {
    if (Mqtt::publish_single()) {
        char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
        snprintf(topic, sizeof(topic), "%s/%s", read_flash_string(F_(analogsensor)).c_str(), sensor.name().c_str());
        char payload[10];
        Mqtt::publish(topic, Helpers::render_value(payload, sensor.value(), 2)); // always publish as floats
    }
}

// send empty config topic to remove the entry from HA
void AnalogSensor::remove_ha_topic(const uint8_t id) {
    if (!Mqtt::ha_enabled()) {
        return;
    }
#ifdef EMSESP_DEBUG
    LOG_DEBUG(F("Removing HA config for analog sensor ID %d"), id);
#endif
    char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
    snprintf(topic, sizeof(topic), "sensor/%s/analogsensor_%d/config", Mqtt::base().c_str(), id);
    Mqtt::publish_ha(topic);
}

// send all sensor values as a JSON package to MQTT
void AnalogSensor::publish_values(const bool force) {
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
        if (sensor.type() != AnalogType::NOTUSED) {
            if (Mqtt::is_nested() || Mqtt::ha_enabled()) {
                // nested
                char       s[10];
                JsonObject dataSensor = doc.createNestedObject(Helpers::smallitoa(s, sensor.id()));
                dataSensor["name"]    = sensor.name();
                switch (sensor.type()) {
                case AnalogType::IOCOUNTER:
                    dataSensor["count"] = (uint16_t)sensor.value(); // convert to integer
                    break;
                case AnalogType::ADC:
                    dataSensor["value"] = (float)sensor.value(); // float
                    break;
                case AnalogType::READ:
                default:
                    dataSensor["value"] = (uint8_t)sensor.value(); // convert to char for 1 or 0
                    break;
                }

                // create HA config
                if (Mqtt::ha_enabled()) {
                    if (!sensor.ha_registered || force) {
                        LOG_DEBUG(F("Recreating HA config for analog sensor ID %d"), sensor.id());

                        StaticJsonDocument<EMSESP_JSON_SIZE_MEDIUM> config;

                        char stat_t[50];
                        snprintf(stat_t, sizeof(stat_t), "%s/analogsensor_data", Mqtt::base().c_str());
                        config["stat_t"] = stat_t;

                        char str[50];
                        snprintf(str, sizeof(str), "{{value_json['%d'].value}}", sensor.id());
                        config["val_tpl"] = str;

                        snprintf(str, sizeof(str), "Analog Sensor %s", sensor.name().c_str());
                        config["name"] = str;

                        snprintf(str, sizeof(str), "analogsensor_%d", sensor.id());
                        config["uniq_id"] = str;

                        JsonObject dev = config.createNestedObject("dev");
                        JsonArray  ids = dev.createNestedArray("ids");
                        ids.add("ems-esp");

                        char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
                        snprintf(topic, sizeof(topic), "sensor/%s/analogsensor_%d/config", Mqtt::base().c_str(), sensor.id());

                        Mqtt::publish_ha(topic, config.as<JsonObject>());

                        sensor.ha_registered = true;
                    }
                }

            } else {
                // not nested
                doc[sensor.name()] = sensor.value();
            }
        }

        Mqtt::publish(F("analogsensor_data"), doc.as<JsonObject>());
    }
}

// called from emsesp.cpp, similar to the emsdevice->get_value_info
// searches by name
bool AnalogSensor::get_value_info(JsonObject & output, const char * cmd, const int8_t id) {
    for (const auto & sensor : sensors_) {
        if (strcmp(cmd, sensor.name().c_str()) == 0) {
            output["id"]     = sensor.id();
            output["name"]   = sensor.name();
            output["type"]   = sensor.type();
            output["uom"]    = sensor.uom();
            output["offset"] = sensor.offset();
            output["factor"] = sensor.factor();
            output["value"]  = sensor.value();
            return true;
        }
    }
    return false;
}

// creates JSON doc from values
// returns false if there are no sensors
bool AnalogSensor::command_info(const char * value, const int8_t id, JsonObject & output) {
    if (sensors_.size() == 0) {
        return false;
    }

    for (const auto & sensor : sensors_) {
        if (id == -1) { // show number and id
            JsonObject dataSensor = output.createNestedObject(sensor.name());
            dataSensor["id"]      = sensor.id();
            dataSensor["name"]    = sensor.name();
            dataSensor["type"]    = sensor.type();
            dataSensor["uom"]     = sensor.uom();
            dataSensor["offset"]  = sensor.offset();
            dataSensor["factor"]  = sensor.factor();
            dataSensor["value"]   = sensor.value();
        } else {
            output[sensor.name()] = sensor.value();
        }
    }

    return (output.size() > 0);
}

// this creates the sensor, initializing everything
AnalogSensor::Sensor::Sensor(const uint8_t id, const std::string & name, const uint16_t offset, const float factor, const uint8_t uom, const int8_t type)
    : id_(id)
    , name_(name)
    , offset_(offset)
    , factor_(factor)
    , uom_(uom)
    , type_(type) {
    value_ = 0; // init value to 0 always
}

// returns name of the analog sensor or creates one if its empty
std::string AnalogSensor::Sensor::name() const {
    if (name_.empty()) {
        char name[50];
        snprintf(name, sizeof(name), "Analog Sensor GPIO%d", id_);
        return name;
    }
    return name_;
}

// hard coded tests
#ifdef EMSESP_DEBUG
void AnalogSensor::test() {
    // Sensor(const uint8_t id, const std::string & name, const uint16_t offset, const float factor, const uint8_t uom, const int8_t type);
    sensors_.emplace_back(36, "test12", 0, 0.1, 17, AnalogType::ADC);
    sensors_.back().set_value(12.4);

    sensors_.emplace_back(37, "test13", 0, 0, 0, AnalogType::READ);
    sensors_.back().set_value(13);
}
#endif

} // namespace emsesp
