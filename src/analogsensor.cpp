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

    if (!analog_enabled_) {
        return;
    }
    analogSetAttenuation(ADC_2_5db); // for all channels 1.5V

    LOG_INFO("Starting Analog sensor service");

    // Add API call for /info
    Command::add(
        EMSdevice::DeviceType::ANALOGSENSOR,
        F_(info),
        [&](const char * value, const int8_t id, JsonObject & output) { return command_info(value, id, output); },
        FL_(info_cmd));
    Command::add(
        EMSdevice::DeviceType::ANALOGSENSOR,
        F_(values),
        [&](const char * value, const int8_t id, JsonObject & output) { return command_info(value, 0, output); },
        nullptr,
        CommandFlag::HIDDEN); // this command is hidden
    Command::add(
        EMSdevice::DeviceType::ANALOGSENSOR,
        F_(setvalue),
        [&](const char * value, const int8_t id) { return command_setvalue(value, id); },
        FL_(setiovalue_cmd),
        CommandFlag::ADMIN_ONLY);
    Command::add(
        EMSdevice::DeviceType::ANALOGSENSOR,
        F_(commands),
        [&](const char * value, const int8_t id, JsonObject & output) { return command_commands(value, id, output); },
        FL_(commands_cmd));

    Mqtt::subscribe(EMSdevice::DeviceType::ANALOGSENSOR, "analogsensor/#", nullptr); // use empty function callback
}

// load settings from the customization file, sorts them and initializes the GPIOs
void AnalogSensor::reload() {
    EMSESP::webSettingsService.read([&](WebSettings & settings) { analog_enabled_ = settings.analog_enabled; });

#if defined(EMSESP_STANDALONE)
    analog_enabled_ = true; // for local offline testing
#endif
    for (auto sensor : sensors_) {
        remove_ha_topic(sensor.type(), sensor.gpio());
        sensor.ha_registered = false;
    }
    if (!analog_enabled_) {
        sensors_.clear();
        return;
    }
    // load the list of analog sensors from the customization service
    // and store them locally and then activate them
    EMSESP::webCustomizationService.read([&](WebCustomization & settings) {
        auto it = sensors_.begin();
        for (auto & sensor_ : sensors_) {
            // update existing sensors
            bool found = false;
            for (const auto & sensor : settings.analogCustomizations) { //search customlist
                if (sensor_.gpio() == sensor.gpio) {
                    // for output sensors set value to new start-value
                    if ((sensor.type == AnalogType::COUNTER || sensor.type >= AnalogType::DIGITAL_OUT)
                        && (sensor_.type() != sensor.type || sensor_.offset() != sensor.offset || sensor_.factor() != sensor.factor)) {
                        sensor_.set_value(sensor.offset);
                    }
                    sensor_.set_name(sensor.name);
                    sensor_.set_type(sensor.type);
                    sensor_.set_offset(sensor.offset);
                    sensor_.set_factor(sensor.factor);
                    sensor_.set_uom(sensor.uom);
                    sensor_.ha_registered = false;
                    found                 = true;
                }
            }
            if (!found) {
                sensors_.erase(it);
            }
            it++;
        }

        // add new sensors from list
        for (const auto & sensor : settings.analogCustomizations) {
            bool found = false;
            for (const auto & sensor_ : sensors_) {
                if (sensor_.gpio() == sensor.gpio) {
                    found = true;
                }
            }
            if (!found) {
                sensors_.emplace_back(sensor.gpio, sensor.name, sensor.offset, sensor.factor, sensor.uom, sensor.type);
                sensors_.back().ha_registered = false; // this will trigger recrate of the HA config
                if (sensor.type == AnalogType::COUNTER || sensor.type >= AnalogType::DIGITAL_OUT) {
                    sensors_.back().set_value(sensor.offset);
                } else {
                    sensors_.back().set_value(0); // reset value only for new sensors
                }
            }
            if (sensor.type == AnalogType::COUNTER || sensor.type >= AnalogType::DIGITAL_OUT) {
                Command::add(
                    EMSdevice::DeviceType::ANALOGSENSOR,
                    sensor.name.c_str(),
                    [&](const char * value, const int8_t id) { return command_setvalue(value, sensor.gpio); },
                    sensor.type == AnalogType::COUNTER       ? FL_(counter)
                    : sensor.type == AnalogType::DIGITAL_OUT ? FL_(digital_out)
                                                             : FL_(pwm),
                    CommandFlag::ADMIN_ONLY);
            }
        }
        return true;
    });

    // sort the list based on GPIO (id)
    // std::sort(sensors_.begin(), sensors_.end(), [](const Sensor & a, const Sensor & b) { return a.id() < b.id(); });

    // activate each sensor
    for (auto & sensor : sensors_) {
        sensor.ha_registered = false; // force HA configs to be re-created
        if (sensor.type() == AnalogType::ADC) {
            LOG_DEBUG("Adding analog ADC sensor on GPIO %02d", sensor.gpio());
            // analogSetPinAttenuation does not work with analogReadMilliVolts
            sensor.analog_       = 0; // initialize
            sensor.last_reading_ = 0;
        } else if (sensor.type() == AnalogType::COUNTER) {
            LOG_DEBUG("Adding analog I/O Counter sensor on GPIO %02d", sensor.gpio());
            pinMode(sensor.gpio(), INPUT_PULLUP);
#if CONFIG_IDF_TARGET_ESP32
            if (sensor.gpio() == 25 || sensor.gpio() == 26) {
                dacWrite(sensor.gpio(), 255);
            }
#elif CONFIG_IDF_TARGET_ESP32S2
            if (sensor.gpio() == 23 || sensor.gpio() == 24) {
                dacWrite(sensor.gpio(), 255);
            }
#endif
            sensor.polltime_ = 0;
            sensor.poll_     = digitalRead(sensor.gpio());
            publish_sensor(sensor);
        } else if (sensor.type() == AnalogType::TIMER || sensor.type() == AnalogType::RATE) {
            LOG_DEBUG("Adding analog Timer/Rate sensor on GPIO %02d", sensor.gpio());
            pinMode(sensor.gpio(), INPUT_PULLUP);
            sensor.polltime_      = uuid::get_uptime();
            sensor.last_polltime_ = uuid::get_uptime();
            sensor.poll_          = digitalRead(sensor.gpio());
            sensor.set_offset(0);
            sensor.set_value(0);
            publish_sensor(sensor);
        } else if (sensor.type() == AnalogType::DIGITAL_IN) {
            LOG_DEBUG("Adding analog Read sensor on GPIO %02d", sensor.gpio());
            pinMode(sensor.gpio(), INPUT_PULLUP);
            sensor.set_value(digitalRead(sensor.gpio())); // initial value
            sensor.set_uom(0);                            // no uom, just for safe measures
            sensor.polltime_ = 0;
            sensor.poll_     = digitalRead(sensor.gpio());
            publish_sensor(sensor);
        } else if (sensor.type() == AnalogType::DIGITAL_OUT) {
            LOG_DEBUG("Adding analog Write sensor on GPIO %02d", sensor.gpio());
            pinMode(sensor.gpio(), OUTPUT);
#if CONFIG_IDF_TARGET_ESP32
            if (sensor.gpio() == 25 || sensor.gpio() == 26) {
                if (sensor.offset() > 255) {
                    sensor.set_offset(255);
                } else if (sensor.offset() < 0) {
                    sensor.set_offset(0);
                }
                dacWrite(sensor.gpio(), sensor.offset());
                sensor.set_value(sensor.offset());
            } else
#elif CONFIG_IDF_TARGET_ESP32S2
            if (sensor.gpio() == 23 || sensor.gpio() == 24) {
                if (sensor.offset() > 255) {
                    sensor.set_offset(255);
                } else if (sensor.offset() < 0) {
                    sensor.set_offset(0);
                }
                dacWrite(sensor.gpio(), sensor.offset());
                sensor.set_value(sensor.offset());
            } else
#endif
            {
                digitalWrite(sensor.gpio(), sensor.offset() > 0 ? 1 : 0);
                sensor.set_value(digitalRead(sensor.gpio()));
            }
            sensor.set_uom(0); // no uom, just for safe measures
            publish_sensor(sensor);
        } else if (sensor.type() >= AnalogType::PWM_0) {
            LOG_DEBUG("Adding PWM output sensor on GPIO %02d", sensor.gpio());
            uint channel = sensor.type() - AnalogType::PWM_0;
            ledcSetup(channel, sensor.factor(), 13);
            ledcAttachPin(sensor.gpio(), channel);
            if (sensor.offset() > 100) {
                sensor.set_offset(100);
            } else if (sensor.offset() < 0) {
                sensor.set_offset(0);
            }
            ledcWrite(channel, (uint32_t)(sensor.offset() * 8191 / 100));
            sensor.set_value(sensor.offset());
            sensor.set_uom(DeviceValueUOM::PERCENT);
            publish_sensor(sensor);
        }
    }
}

// measure input sensors and moving average adc
void AnalogSensor::measure() {
    static uint32_t measure_last_ = 0;

    // measure interval 500ms for adc sensors
    if (!measure_last_ || (uuid::get_uptime() - measure_last_) >= MEASURE_ANALOG_INTERVAL) {
        measure_last_ = uuid::get_uptime();
        // go through the list of adc sensors
        for (auto & sensor : sensors_) {
            if (sensor.type() == AnalogType::ADC) {
                uint16_t a = analogReadMilliVolts(sensor.gpio()); // e.g. ADC1_CHANNEL_0_GPIO_NUM
                if (!sensor.analog_) {                            // init first time
                    sensor.analog_ = a;
                    sensor.sum_    = a * 512;
                } else { // simple moving average filter
                    sensor.sum_    = (sensor.sum_ * 511) / 512 + a;
                    sensor.analog_ = sensor.sum_ / 512;
                }
                // detect change with little hysteresis on raw mV value
                if (sensor.last_reading_ + 1 < sensor.analog_ || sensor.last_reading_ > sensor.analog_ + 1) {
                    sensor.set_value(((int32_t)sensor.analog_ - sensor.offset()) * sensor.factor());
                    sensor.last_reading_ = sensor.analog_;
                    sensorreads_++;
                    changed_ = true;
                    publish_sensor(sensor);
                }
            }
        }
    }
    // poll digital io every time with debounce
    // go through the list of digital sensors
    for (auto & sensor : sensors_) {
        if (sensor.type() == AnalogType::DIGITAL_IN || sensor.type() == AnalogType::COUNTER || sensor.type() == AnalogType::TIMER
            || sensor.type() == AnalogType::RATE) {
            auto old_value       = sensor.value(); // remember current value before reading
            auto current_reading = digitalRead(sensor.gpio());
            if (sensor.poll_ != current_reading) {     // check for pinchange
                sensor.polltime_ = uuid::get_uptime(); // remember time of pinchange
                sensor.poll_     = current_reading;
            }
            // debounce and check for real pinchange
            if (uuid::get_uptime() - sensor.polltime_ >= 15 && sensor.poll_ != sensor.last_reading_) {
                sensor.last_reading_ = sensor.poll_;
                if (sensor.type() == AnalogType::DIGITAL_IN) {
                    sensor.set_value(sensor.poll_);
                } else if (!sensor.poll_) { // falling edge
                    if (sensor.type() == AnalogType::COUNTER) {
                        sensor.set_value(old_value + sensor.factor());
                    } else if (sensor.type() == AnalogType::RATE) { // dafault uom: Hz (1/sec) with factor 1
                        sensor.set_value(sensor.factor() * 1000 / (sensor.polltime_ - sensor.last_polltime_));
                    } else if (sensor.type() == AnalogType::TIMER) { // default seconds with factor 1
                        sensor.set_value(sensor.factor() * (sensor.polltime_ - sensor.last_polltime_) / 1000);
                    }
                    sensor.last_polltime_ = sensor.polltime_;
                }
            }
            // see if there is a change and increment # reads
            if (old_value != sensor.value()) {
                sensorreads_++;
                changed_ = true;
                publish_sensor(sensor);
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
bool AnalogSensor::update(uint8_t gpio, const std::string & name, double offset, double factor, uint8_t uom, int8_t type) {
    boolean found_sensor = false; // see if we can find the sensor in our customization list

    EMSESP::webCustomizationService.update(
        [&](WebCustomization & settings) {
            for (auto & AnalogCustomization : settings.analogCustomizations) {
                if (AnalogCustomization.type == AnalogType::COUNTER || AnalogCustomization.type >= AnalogType::DIGITAL_OUT) {
                    Command::erase_command(EMSdevice::DeviceType::ANALOGSENSOR, AnalogCustomization.name.c_str());
                }
                if (AnalogCustomization.gpio == gpio) {
                    found_sensor = true; // found the record
                    // see if it's marked for deletion
                    if (type == AnalogType::MARK_DELETED) {
                        LOG_DEBUG("Removing analog sensor GPIO %02d", gpio);
                        settings.analogCustomizations.remove(AnalogCustomization);
                    } else {
                        // update existing record
                        AnalogCustomization.name   = name;
                        AnalogCustomization.offset = offset;
                        AnalogCustomization.factor = factor;
                        AnalogCustomization.uom    = uom;
                        AnalogCustomization.type   = type;
                        LOG_DEBUG("Customizing existing analog GPIO %02d", gpio);
                    }
                    return StateUpdateResult::CHANGED; // persist the change
                }
            }
            return StateUpdateResult::UNCHANGED;
        },
        "local");

    // if the sensor exists and we're using HA, delete the old HA record
    if (found_sensor && Mqtt::ha_enabled()) {
        remove_ha_topic(type, gpio); // the GPIO
    }

    // we didn't find it, it's new, so create and store it
    if (!found_sensor) {
        EMSESP::webCustomizationService.update(
            [&](WebCustomization & settings) {
                auto newSensor   = AnalogCustomization();
                newSensor.gpio   = gpio;
                newSensor.name   = name;
                newSensor.offset = offset;
                newSensor.factor = factor;
                newSensor.uom    = uom;
                newSensor.type   = type;
                settings.analogCustomizations.push_back(newSensor);
                LOG_DEBUG("Adding new customization for analog sensor GPIO %02d", gpio);
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
void AnalogSensor::publish_sensor(const Sensor & sensor) const {
    if (Mqtt::publish_single()) {
        char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
        if (Mqtt::publish_single2cmd()) {
            snprintf(topic, sizeof(topic), "%s/%s", F_(analogsensor), sensor.name().c_str());
        } else {
            snprintf(topic, sizeof(topic), "%s%s/%s", F_(analogsensor), "_data", sensor.name().c_str());
        }
        char payload[10];
        Mqtt::queue_publish(topic, Helpers::render_value(payload, sensor.value(), 2)); // always publish as doubles
    }
}

// send empty config topic to remove the entry from HA
void AnalogSensor::remove_ha_topic(const int8_t type, const uint8_t gpio) const {
    if (!Mqtt::ha_enabled()) {
        return;
    }

    LOG_DEBUG("Removing HA config for analog sensor GPIO %02d", gpio);
    char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];

#if CONFIG_IDF_TARGET_ESP32
    if (type == AnalogType::DIGITAL_OUT && gpio != 25 && gpio != 26) {
#else
    if (type == AnalogType::DIGITAL_OUT) {
#endif
        snprintf(topic, sizeof(topic), "switch/%s/analogsensor_%02d/config", Mqtt::basename().c_str(), gpio);
    } else if (type == AnalogType::DIGITAL_OUT) { // DAC
        snprintf(topic, sizeof(topic), "number/%s/analogsensor_%02d/config", Mqtt::basename().c_str(), gpio);
    } else if (type >= AnalogType::PWM_0) {
        snprintf(topic, sizeof(topic), "number/%s/analogsensor_%02d/config", Mqtt::basename().c_str(), gpio);
    } else if (type == AnalogType::DIGITAL_IN) {
        snprintf(topic, sizeof(topic), "binary_sensor/%s/analogsensor_%02d/config", Mqtt::basename().c_str(), gpio);
    } else {
        snprintf(topic, sizeof(topic), "sensor/%s/analogsensor_%02d/config", Mqtt::basename().c_str(), gpio);
    }
    Mqtt::queue_remove_topic(topic);
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
    }

    DynamicJsonDocument doc(120 * num_sensors);

    for (auto & sensor : sensors_) {
        if (sensor.type() != AnalogType::NOTUSED) {
            if (Mqtt::is_nested()) {
                char       s[10];
                JsonObject dataSensor = doc.createNestedObject(Helpers::smallitoa(s, sensor.gpio()));
                dataSensor["name"]    = sensor.name();
                switch (sensor.type()) {
                case AnalogType::COUNTER:
                case AnalogType::TIMER:
                case AnalogType::RATE:
                case AnalogType::ADC:
                case AnalogType::PWM_0:
                case AnalogType::PWM_1:
                case AnalogType::PWM_2:
                    dataSensor["value"] = serialized(Helpers::render_value(s, sensor.value(), 2)); // double
                    break;
                case AnalogType::DIGITAL_IN:
                case AnalogType::DIGITAL_OUT:
                    if (EMSESP::system_.bool_format() == BOOL_FORMAT_TRUEFALSE) {
                        dataSensor["value"] = sensor.value() != 0;
                    } else if (EMSESP::system_.bool_format() == BOOL_FORMAT_10) {
                        dataSensor["value"] = sensor.value() != 0 ? 1 : 0;
                    } else {
                        char result[12];
                        dataSensor["value"] = Helpers::render_boolean(result, sensor.value() != 0);
                    }
                    break;
                default:
                    break;
                }
            } else if (sensor.type() == AnalogType::DIGITAL_IN || sensor.type() == AnalogType::DIGITAL_OUT) {
                if (EMSESP::system_.bool_format() == BOOL_FORMAT_TRUEFALSE) {
                    doc[sensor.name()] = sensor.value() != 0;
                } else if (EMSESP::system_.bool_format() == BOOL_FORMAT_10) {
                    doc[sensor.name()] = sensor.value() != 0 ? 1 : 0;
                } else {
                    char result[12];
                    doc[sensor.name()] = Helpers::render_boolean(result, sensor.value() != 0);
                }
            } else {
                char s[10];
                doc[sensor.name()] = serialized(Helpers::render_value(s, sensor.value(), 2));
            }

            // create HA config
            if (Mqtt::ha_enabled() && (!sensor.ha_registered || force)) {
                LOG_DEBUG("Recreating HA config for analog sensor GPIO %02d", sensor.gpio());

                StaticJsonDocument<EMSESP_JSON_SIZE_MEDIUM> config;

                char stat_t[50];
                snprintf(stat_t, sizeof(stat_t), "%s/analogsensor_data", Mqtt::base().c_str()); // use base path
                config["stat_t"] = stat_t;

                char val_obj[50];
                char val_cond[95];
                if (Mqtt::is_nested()) {
                    snprintf(val_obj, sizeof(val_obj), "value_json['%02d'].value", sensor.gpio());
                    snprintf(val_cond, sizeof(val_cond), "value_json['%02d'] is defined and %s is defined", sensor.gpio(), val_obj);
                } else {
                    snprintf(val_obj, sizeof(val_obj), "value_json['%s']", sensor.name().c_str());
                    snprintf(val_cond, sizeof(val_cond), "%s is defined", val_obj);
                }
                char sample_val[12] = "0";
                if (sensor.type() == AnalogType::DIGITAL_IN || sensor.type() == AnalogType::DIGITAL_OUT) {
                    Helpers::render_boolean(sample_val, false);
                }
                config["val_tpl"] = (std::string) "{{" + val_obj + " if " + val_cond + " else " + sample_val + "}}";

                char uniq_s[70];
                if (Mqtt::entity_format() == 2) {
                    snprintf(uniq_s, sizeof(uniq_s), "%s_analogsensor_%02d", Mqtt::basename().c_str(), sensor.gpio());
                } else {
                    snprintf(uniq_s, sizeof(uniq_s), "analogsensor_%02d", sensor.gpio());
                }

                config["obj_id"]  = uniq_s;
                config["uniq_id"] = uniq_s; // same as object_id

                char name[50];
                snprintf(name, sizeof(name), "%s", sensor.name().c_str());
                config["name"] = name;

                if (sensor.uom() != DeviceValueUOM::NONE) {
                    config["unit_of_meas"] = EMSdevice::uom_to_string(sensor.uom());
                }

                char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];

                // Set commands for some analog types
                char command_topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
#if CONFIG_IDF_TARGET_ESP32
                if (sensor.type() == AnalogType::DIGITAL_OUT && sensor.gpio() != 25 && sensor.gpio() != 26) {
#else
                if (sensor.type() == AnalogType::DIGITAL_OUT) {
#endif
                    snprintf(topic, sizeof(topic), "switch/%s/analogsensor_%02d/config", Mqtt::basename().c_str(), sensor.gpio());
                    snprintf(command_topic, sizeof(command_topic), "%s/analogsensor/%s", Mqtt::basename().c_str(), sensor.name().c_str());
                    config["cmd_t"] = command_topic;
                    if (EMSESP::system_.bool_format() == BOOL_FORMAT_TRUEFALSE) {
                        config["pl_on"]  = true;
                        config["pl_off"] = false;
                    } else if (EMSESP::system_.bool_format() == BOOL_FORMAT_10) {
                        config["pl_on"]  = 1;
                        config["pl_off"] = 0;
                    } else {
                        char result[12];
                        config["pl_on"]  = Helpers::render_boolean(result, true);
                        config["pl_off"] = Helpers::render_boolean(result, false);
                    }
                } else if (sensor.type() == AnalogType::DIGITAL_OUT) { // DAC
                    snprintf(topic, sizeof(topic), "number/%s/analogsensor_%02d/config", Mqtt::basename().c_str(), sensor.gpio());
                    snprintf(command_topic, sizeof(command_topic), "%s/analogsensor/%s", Mqtt::basename().c_str(), sensor.name().c_str());
                    config["cmd_t"] = command_topic;
                    config["min"]   = 0;
                    config["max"]   = 255;
                    config["mode"]  = "box"; // auto, slider or box
                    config["step"]  = 1;
                } else if (sensor.type() >= AnalogType::PWM_0) {
                    snprintf(topic, sizeof(topic), "number/%s/analogsensor_%02d/config", Mqtt::basename().c_str(), sensor.gpio());
                    snprintf(command_topic, sizeof(command_topic), "%s/analogsensor/%s", Mqtt::basename().c_str(), sensor.name().c_str());
                    config["cmd_t"] = command_topic;
                    config["min"]   = 0;
                    config["max"]   = 100;
                    config["mode"]  = "box"; // auto, slider or box
                    config["step"]  = 0.1;
                } else if (sensor.type() == AnalogType::COUNTER) {
                    snprintf(topic, sizeof(topic), "sensor/%s/analogsensor_%02d/config", Mqtt::basename().c_str(), sensor.gpio());
                    snprintf(command_topic, sizeof(command_topic), "%s/analogsensor/%s", Mqtt::basename().c_str(), sensor.name().c_str());
                    config["cmd_t"]    = command_topic;
                    config["stat_cla"] = "total_increasing";
                    // config["mode"]  = "box"; // auto, slider or box
                    // config["step"]  = sensor.factor();
                } else if (sensor.type() == AnalogType::DIGITAL_IN) {
                    snprintf(topic, sizeof(topic), "binary_sensor/%s/analogsensor_%02d/config", Mqtt::basename().c_str(), sensor.gpio());
                } else {
                    snprintf(topic, sizeof(topic), "sensor/%s/analogsensor_%02d/config", Mqtt::basename().c_str(), sensor.gpio());
                    config["stat_cla"] = "measurement";
                }

                JsonObject dev = config.createNestedObject("dev");
                JsonArray  ids = dev.createNestedArray("ids");
                ids.add("ems-esp");

                // add "availability" section
                Mqtt::add_avty_to_doc(stat_t, config.as<JsonObject>(), val_cond);

                Mqtt::queue_ha(topic, config.as<JsonObject>());

                sensor.ha_registered = true;
            }
        }
    }

    Mqtt::queue_publish("analogsensor_data", doc.as<JsonObject>());
}

// called from emsesp.cpp, similar to the emsdevice->get_value_info
// searches by name
bool AnalogSensor::get_value_info(JsonObject & output, const char * cmd, const int8_t id) const {
    if (sensors_.empty()) {
        return false;
    }
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
        if (Helpers::toLower(command_s) == Helpers::toLower(sensor.name().c_str()) || Helpers::atoint(command_s) == sensor.gpio()) {
            output["gpio"]      = sensor.gpio();
            output["name"]      = sensor.name();
            output["type"]      = F_(number);
            output["analog"]    = FL_(list_sensortype)[sensor.type()];
            output["uom"]       = EMSdevice::uom_to_string(sensor.uom());
            output["offset"]    = sensor.offset();
            output["factor"]    = sensor.factor();
            output["value"]     = sensor.value();
            output["writeable"] = sensor.type() == AnalogType::COUNTER || sensor.type() >= AnalogType::DIGITAL_OUT;
            // min/max for writeable analogs
            if (sensor.type() == AnalogType::COUNTER) {
                output["min"] = 0;
                output["max"] = 4000000;
            } else if (sensor.type() == AnalogType::DIGITAL_OUT) {
                output["min"] = 0;
                output["max"] = sensor.gpio() == 25 || sensor.gpio() == 26 ? 255 : 1;
            } else if (sensor.type() >= AnalogType::PWM_0) {
                output["min"] = 0;
                output["max"] = 100;
            }
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

// creates JSON doc from values
// returns false if there are no sensors
bool AnalogSensor::command_info(const char * value, const int8_t id, JsonObject & output) const {
    if (sensors_.empty()) {
        return false;
    }

    for (const auto & sensor : sensors_) {
        if (id == -1) { // show number and id for info command
            JsonObject dataSensor = output.createNestedObject(sensor.name());
            dataSensor["gpio"]    = sensor.gpio();
            dataSensor["type"]    = F_(number);
            dataSensor["analog"]  = FL_(list_sensortype)[sensor.type()];
            if (sensor.type() == AnalogType::ADC) {
                dataSensor["uom"]    = EMSdevice::uom_to_string(sensor.uom());
                dataSensor["offset"] = sensor.offset();
                dataSensor["factor"] = sensor.factor();
            } else if (sensor.type() == AnalogType::COUNTER) {
                dataSensor["uom"]         = EMSdevice::uom_to_string(sensor.uom());
                dataSensor["start_value"] = sensor.offset();
                dataSensor["factor"]      = sensor.factor();
            } else if (sensor.type() == AnalogType::TIMER || sensor.type() == AnalogType::RATE) {
                dataSensor["factor"] = sensor.factor();
            } else if (sensor.type() >= AnalogType::PWM_0) {
                dataSensor["uom"]       = EMSdevice::uom_to_string(sensor.uom());
                dataSensor["frequency"] = sensor.factor();
            }
            dataSensor["value"] = sensor.value();
        } else if (id == 0) { // output values command
            output[sensor.name()] = sensor.value();
        } else { // if someone wants gpio numbers
            char gpio_str[9];
            snprintf(gpio_str, sizeof(gpio_str), "gpio_%02d", sensor.gpio());
            output[gpio_str] = sensor.value();
        }
    }

    return (output.size() > 0);
}

// this creates the sensor, initializing everything
AnalogSensor::Sensor::Sensor(const uint8_t gpio, const std::string & name, const double offset, const double factor, const uint8_t uom, const int8_t type)
    : gpio_(gpio)
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
        snprintf(name, sizeof(name), "Analog Sensor GPIO %02d", gpio_);
        return name;
    }
    return name_;
}

// set the dig_out/counter/DAC/PWM value, id is gpio-no
bool AnalogSensor::command_setvalue(const char * value, const int8_t gpio) {
    float val;
    if (!Helpers::value2float(value, val)) {
        bool b;
        if (!Helpers::value2bool(value, b)) {
            return false;
        }
        val = b ? 1 : 0;
    }
    for (auto & sensor : sensors_) {
        if (sensor.gpio() == gpio) {
            if (sensor.type() == AnalogType::COUNTER) {
                if (val < 0 || value[0] == '+') { // sign corrects values
                    sensor.set_offset(sensor.value() + val);
                    sensor.set_value(sensor.value() + val);
                } else { // positive values are set
                    sensor.set_offset(val);
                    sensor.set_value(val);
                }
                publish_sensor(sensor);
                return true;
            } else if (sensor.type() == AnalogType::ADC) {
                sensor.set_offset(val);
                return true;
            } else if (sensor.type() == AnalogType::DIGITAL_OUT) {
                uint8_t v = val;
#if CONFIG_IDF_TARGET_ESP32
                if ((sensor.gpio() == 25 || sensor.gpio() == 26) && v <= 255) {
                    sensor.set_offset(v);
                    sensor.set_value(v);
                    pinMode(sensor.gpio(), OUTPUT);
                    dacWrite(sensor.gpio(), sensor.offset());
                    publish_sensor(sensor);
                    return true;
                } else
#elif CONFIG_IDF_TARGET_ESP32S2
                if ((sensor.gpio() == 23 || sensor.gpio() == 24) && v <= 255) {
                    sensor.set_offset(v);
                    sensor.set_value(v);
                    pinMode(sensor.gpio(), OUTPUT);
                    dacWrite(sensor.gpio(), sensor.offset());
                    publish_sensor(sensor);
                    return true;
                } else
#endif
                    if (v == 0 || v == 1) {
                    sensor.set_offset(v);
                    sensor.set_value(v);
                    pinMode(sensor.gpio(), OUTPUT);
                    digitalWrite(sensor.gpio(), sensor.offset() > 0 ? 1 : 0);
                    publish_sensor(sensor);
                    return true;
                }
            } else if (sensor.type() >= AnalogType::PWM_0) {
                uint8_t channel = sensor.type() - AnalogType::PWM_0;
                if (val > 100) {
                    val = 100;
                } else if (val < 0) {
                    val = 0;
                }
                sensor.set_offset(val);
                sensor.set_value(val);
                ledcWrite(channel, (uint32_t)(val * 8191 / 100));
                publish_sensor(sensor);
                return true;
            }
        }
    }
    return false;
}

// list commands
bool AnalogSensor::command_commands(const char * value, const int8_t id, JsonObject & output) {
    return Command::list(EMSdevice::DeviceType::ANALOGSENSOR, output);
}

// hard coded tests
#ifdef EMSESP_TEST
void AnalogSensor::test() {
    sensors_.emplace_back(36, "test12", 0, 0.1, 17, AnalogType::ADC);
    sensors_.back().set_value(12.4);

    sensors_.emplace_back(37, "test13", 0, 0, 0, AnalogType::DIGITAL_IN);
    sensors_.back().set_value(13);
}
#endif

} // namespace emsesp