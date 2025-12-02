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

uuid::log::Logger    AnalogSensor::logger_{F_(analogsensor), uuid::log::Facility::DAEMON};
std::vector<uint8_t> AnalogSensor::exclude_types_;

#ifndef EMSESP_STANDALONE
portMUX_TYPE  mux                     = portMUX_INITIALIZER_UNLOCKED;
unsigned long AnalogSensor::edge[]    = {0, 0, 0};
unsigned long AnalogSensor::edgecnt[] = {0, 0, 0};

void IRAM_ATTR AnalogSensor::freqIrq0() {
    portENTER_CRITICAL_ISR(&mux);
    if (micros() - edge[0] > 10) { // limit to 100kHz
        edgecnt[0]++;
        edge[0] = micros();
    }
    portEXIT_CRITICAL_ISR(&mux);
}
void IRAM_ATTR AnalogSensor::freqIrq1() {
    portENTER_CRITICAL_ISR(&mux);
    if (micros() - edge[1] > 10) { // limit to 100kHz
        edgecnt[1]++;
        edge[1] = micros();
    }
    portEXIT_CRITICAL_ISR(&mux);
}
void IRAM_ATTR AnalogSensor::freqIrq2() {
    portENTER_CRITICAL_ISR(&mux);
    if (micros() - edge[2] > 10) { // limit to 100kHz
        edgecnt[2]++;
        edge[2] = micros();
    }
    portEXIT_CRITICAL_ISR(&mux);
}
#endif

void AnalogSensor::start(const bool factory_settings) {
    // if (factory_settings && EMSESP::nvs_.getString("boot").equals("E32V2_2") && EMSESP::nvs_.getString("hwrevision").equals("3.0")) {
    if (factory_settings && analogReadMilliVolts(39) > 700) { // core voltage > 2.6V
        EMSESP::webCustomizationService.update([&](WebCustomization & settings) {
            auto newSensor = AnalogCustomization();

            newSensor.gpio      = 39;
            newSensor.name      = "core_voltage";
            newSensor.offset    = 0;
            newSensor.factor    = 0.00377136; // Divider 24k - 8,66k
            newSensor.uom       = DeviceValueUOM::VOLTS;
            newSensor.type      = AnalogType::ADC;
            newSensor.is_system = true;
            settings.analogCustomizations.push_back(newSensor);

            newSensor.gpio      = 36;
            newSensor.name      = "supply_voltage";
            newSensor.factor    = 0.017; // Divider 24k - 1,5k
            newSensor.is_system = true;
            settings.analogCustomizations.push_back(newSensor);

            return StateUpdateResult::CHANGED; // persist the change
        });
    }
    reload(true); // fetch the list of sensors from our customization service

    if (!analog_enabled_) {
        return;
    }

    analogSetAttenuation(ADC_11db); // for all channels 3.3V

    LOG_INFO("Starting Analog Sensor service");

    // Add API calls
    Command::add(
        EMSdevice::DeviceType::ANALOGSENSOR,
        F_(setvalue),
        [&](const char * value, const int8_t id) { return command_setvalue(value, id); },
        FL_(setiovalue_cmd),
        CommandFlag::ADMIN_ONLY);

    char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
    snprintf(topic, sizeof(topic), "%s/#", F_(analogsensor));
    Mqtt::subscribe(EMSdevice::DeviceType::ANALOGSENSOR, topic, nullptr); // use empty function callback
}

// load settings from the customization file, sorts them and initializes the GPIOs
void AnalogSensor::reload(bool get_nvs) {
    exclude_types_.clear();
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
    changed_ = true;
    // load the list of analog sensors from the customization service
    // and store them locally and then activate them
    EMSESP::webCustomizationService.read([&](WebCustomization & settings) {
        auto it = sensors_.begin();
        for (auto & sensor_ : sensors_) {
            // update existing sensors
            bool found = false;
            for (const auto & sensor : settings.analogCustomizations) { // search customlist
                if (sensor_.gpio() == sensor.gpio) {
                    // for output sensors set value to new start-value
                    if (sensor.type >= AnalogType::DIGITAL_OUT && sensor.type <= AnalogType::PWM_2
                        && (sensor_.type() != sensor.type || sensor_.offset() != sensor.offset || sensor_.factor() != sensor.factor)) {
                        sensor_.set_value(sensor.offset);
                    }
                    if ((sensor.type == AnalogType::COUNTER || (sensor.type >= AnalogType::CNT_0 && sensor.type <= AnalogType::CNT_2))
                        && sensor_.offset() != sensor.offset && sensor.offset != EMSESP::nvs_.getDouble(sensor.name.c_str(), 0)) {
                        EMSESP::nvs_.putDouble(sensor.name.c_str(), sensor.offset);
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
                // it's new, we assume it's valid
                AnalogType type = static_cast<AnalogType>(sensor.type);
                sensors_.emplace_back(sensor.gpio, sensor.name, sensor.offset, sensor.factor, sensor.uom, type, sensor.is_system);

                sensors_.back().ha_registered = false; // this will trigger recreate of the HA config
                if (sensor.type == AnalogType::COUNTER || sensor.type >= AnalogType::DIGITAL_OUT
                    || (sensor.type >= AnalogType::CNT_0 && sensor.type <= AnalogType::CNT_2)) {
                    sensors_.back().set_value(sensor.offset);
                } else {
                    sensors_.back().set_value(0); // reset value only for new sensors
                }
            }

            // add the command to set the value of the sensor
            if (sensor.type == AnalogType::COUNTER || (sensor.type >= AnalogType::DIGITAL_OUT && sensor.type <= AnalogType::PWM_2)
                || sensor.type == AnalogType::RGB || sensor.type == AnalogType::PULSE || (sensor.type >= AnalogType::CNT_0 && sensor.type <= AnalogType::CNT_2)) {
                Command::add(
                    EMSdevice::DeviceType::ANALOGSENSOR,
                    sensor.name.c_str(),
                    [&](const char * value, const int8_t id) { return command_setvalue(value, sensor.gpio); },
                    sensor.type == AnalogType::COUNTER || (sensor.type >= AnalogType::CNT_0 && sensor.type <= AnalogType::CNT_2) ? FL_(counter)
                    : sensor.type == AnalogType::DIGITAL_OUT                                                                     ? FL_(digital_out)
                    : sensor.type == AnalogType::RGB                                                                             ? FL_(RGB)
                    : sensor.type == AnalogType::PULSE                                                                           ? FL_(pulse)
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
        if ((sensor.type() >= AnalogType::FREQ_0 && sensor.type() <= AnalogType::FREQ_2)) {
            exclude_types_.push_back(sensor.type());
            exclude_types_.push_back(sensor.type() + 3);
        }
        if ((sensor.type() >= AnalogType::CNT_0 && sensor.type() <= AnalogType::CNT_2)) {
            exclude_types_.push_back(sensor.type());
            exclude_types_.push_back(sensor.type() - 3);
        }
        if ((sensor.type() >= AnalogType::PWM_0 && sensor.type() <= AnalogType::PWM_2)) {
            exclude_types_.push_back(sensor.type());
        }

        if (sensor.type() == AnalogType::COUNTER || sensor.type() == AnalogType::DIGITAL_IN || sensor.type() == AnalogType::RATE
            || sensor.type() == AnalogType::TIMER || (sensor.type() >= AnalogType::FREQ_0 && sensor.type() <= AnalogType::CNT_2)) {
            // pullup is mapped to DAC, so set to 3.3V
#if CONFIG_IDF_TARGET_ESP32
            if (sensor.gpio() == 25 || sensor.gpio() == 26) {
                dacWrite(sensor.gpio(), 255);
            }
#elif CONFIG_IDF_TARGET_ESP32S2
            if (sensor.gpio() == 17 || sensor.gpio() == 18) {
                dacWrite(sensor.gpio(), 255);
            }
#endif
            pinMode(sensor.gpio(), INPUT_PULLUP);
        }
        if (sensor.type() == AnalogType::ADC) {
            LOG_DEBUG("ADC Sensor on GPIO %02d", sensor.gpio());
            // analogSetPinAttenuation does not work with analogReadMilliVolts
            sensor.analog_       = 0; // initialize
            sensor.last_reading_ = 0;
        } else if (sensor.type() == AnalogType::NTC) {
            LOG_DEBUG("NTC Sensor on GPIO %02d", sensor.gpio());
            // analogSetPinAttenuation(sensor.gpio(), ADC_11db); //does not work with analogReadMilliVolts
            sensor.set_uom(DeviceValueUOM::DEGREES);
        } else if (sensor.type() == AnalogType::COUNTER) {
            LOG_DEBUG("I/O Counter on GPIO %02d", sensor.gpio());
            sensor.polltime_ = 0;
            sensor.poll_     = digitalRead(sensor.gpio());
            if (double_t val = EMSESP::nvs_.getDouble(sensor.name().c_str(), 0)) {
                sensor.set_value(val);
            }
            publish_sensor(sensor);
        } else if (sensor.type() == AnalogType::TIMER || sensor.type() == AnalogType::RATE) {
            LOG_DEBUG("Timer/Rate on GPIO %02d", sensor.gpio());
            sensor.polltime_      = uuid::get_uptime();
            sensor.last_polltime_ = uuid::get_uptime();
            sensor.poll_          = digitalRead(sensor.gpio());
            sensor.set_offset(0);
            sensor.set_value(0);
            publish_sensor(sensor);
#ifndef EMSESP_STANDALONE
        } else if (sensor.type() >= AnalogType::FREQ_0 && sensor.type() <= AnalogType::FREQ_2) {
            auto index = sensor.type() - AnalogType::FREQ_0;
            LOG_DEBUG("Frequency %d on GPIO %02d", index, sensor.gpio());
            sensor.set_offset(0);
            sensor.set_value(0);
            publish_sensor(sensor);
            attachInterrupt(sensor.gpio(), index == 0 ? freqIrq0 : index == 1 ? freqIrq1 : freqIrq2, FALLING);
            lastedge[index] = edge[index] = micros();
            edgecnt[index]                = 0;
        } else if (sensor.type() >= AnalogType::CNT_0 && sensor.type() <= AnalogType::CNT_2) {
            auto index = sensor.type() - AnalogType::CNT_0;
            LOG_DEBUG("Counter %d on GPIO %02d", index, sensor.gpio());
            if (double_t val = EMSESP::nvs_.getDouble(sensor.name().c_str(), 0)) {
                sensor.set_value(val);
            }
            publish_sensor(sensor);
            attachInterrupt(sensor.gpio(), index == 0 ? freqIrq0 : index == 1 ? freqIrq1 : freqIrq2, FALLING);
            edgecnt[index] = 0;
#endif
        } else if (sensor.type() == AnalogType::DIGITAL_IN) {
            LOG_DEBUG("Digital Read on GPIO %02d", sensor.gpio());
            sensor.set_value(digitalRead(sensor.gpio())); // initial value
            sensor.set_uom(0);                            // no uom, just for safe measures
            sensor.polltime_ = 0;
            sensor.poll_     = digitalRead(sensor.gpio());
            publish_sensor(sensor);
        } else if (sensor.type() == AnalogType::RGB) {
            sensor.set_uom(0); // no uom
            LOG_DEBUG("RGB on GPIO %02d", sensor.gpio());
            uint32_t v = sensor.value();
            uint8_t  r = v / 10000;
            uint8_t  g = (v - r * 10000) / 100;
            uint8_t  b = v % 100;
#if ESP_ARDUINO_VERSION_MAJOR < 3
            neopixelWrite(sensor.gpio(), 2 * r, 2 * g, 2 * b);
#else
            rgbLedWrite(sensor.gpio(), 2 * r, 2 * g, 2 * b);
#endif
            LOG_DEBUG("RGB set to %d, %d, %d", r, g, b);
        } else if (sensor.type() == AnalogType::DIGITAL_OUT) {
            LOG_DEBUG("Digital Write on GPIO %02d", sensor.gpio());
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
            if (sensor.gpio() == 17 || sensor.gpio() == 18) {
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
                if (sensor.uom() == 0) { // set state from NVS
                    if (!get_nvs || EMSESP::nvs_.getChar(sensor.name().c_str(), -1) == -1) {
                        EMSESP::nvs_.putChar(sensor.name().c_str(), (int8_t)sensor.offset());
                    } else {
                        sensor.set_offset(EMSESP::nvs_.getChar(sensor.name().c_str()));
                    }
                } else if (sensor.uom() > 1) {
                    sensor.set_uom(2);
                }
                sensor.set_offset(sensor.offset() > 0 ? 1 : 0);
                digitalWrite(sensor.gpio(), (sensor.offset() == 0) ^ (sensor.factor() > 0));
                sensor.set_value(sensor.offset());
            }
            publish_sensor(sensor);
        } else if (sensor.type() == AnalogType::PULSE) {
            LOG_DEBUG("Pulse on GPIO %02d", sensor.gpio());
            pinMode(sensor.gpio(), OUTPUT);
            digitalWrite(sensor.gpio(), (sensor.offset() == 1) ^ (sensor.value() == 1));
            sensor.polltime_ = sensor.value() != 0 ? uuid::get_uptime() + (sensor.factor() * 1000) : 0;
        } else if (sensor.type() >= AnalogType::PWM_0 && sensor.type() <= AnalogType::PWM_2) {
            LOG_DEBUG("PWM output on GPIO %02d", sensor.gpio());
#if ESP_IDF_VERSION_MAJOR >= 5
            ledcAttach(sensor.gpio(), sensor.factor(), 13);
#else
            uint8_t channel = sensor.type() - AnalogType::PWM_0;
            ledcSetup(channel, sensor.factor(), 13);
            ledcAttachPin(sensor.gpio(), channel);
#endif
            if (sensor.offset() > 100) {
                sensor.set_offset(100);
            } else if (sensor.offset() < 0) {
                sensor.set_offset(0);
            }
#if ESP_IDF_VERSION_MAJOR >= 5
            ledcWrite(sensor.gpio(), (uint32_t)(sensor.offset() * 8191 / 100));
#else
            ledcWrite(channel, (uint32_t)(sensor.offset() * 8191 / 100));
#endif
            sensor.set_value(sensor.offset());
            sensor.set_uom(DeviceValueUOM::PERCENT);
            publish_sensor(sensor);
        }
    }
}

// measure input sensors and moving average adc
void AnalogSensor::measure() {
    static uint32_t measure_last_ = uuid::get_uptime() - MEASURE_ANALOG_INTERVAL;

    // measure interval 500ms for adc sensors
    if ((uuid::get_uptime() - measure_last_) >= MEASURE_ANALOG_INTERVAL) {
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
            } else if (sensor.type() == AnalogType::NTC) {
                auto a = analogReadMilliVolts(sensor.gpio());
                if (!sensor.analog_) { // init first time
                    sensor.analog_ = a;
                    sensor.sum_    = a * 16;
                } else { // simple moving average filter
                    sensor.sum_    = (sensor.sum_ * 15 + a * 16) / 16;
                    sensor.analog_ = sensor.sum_ / 16;
                }
                if (sensor.analog_ > 0 && sensor.analog_ < 3300 && (sensor.last_reading_ + 1 < sensor.analog_ || sensor.last_reading_ > sensor.analog_ + 1)) {
                    sensor.set_value(sensor.offset() + 1 / (1 / T25 + log((double)sensor.analog_ / (3300 - sensor.analog_) * (Rt / R0)) / Beta)
                                     - T0); // Temperature in Celsius
                    sensor.last_reading_ = sensor.analog_;
                    sensorreads_++;
                    changed_ = true;
                    publish_sensor(sensor);
                }
#ifndef EMSESP_STANDALONE
            } else if (sensor.type() >= AnalogType::FREQ_0 && sensor.type() <= AnalogType::FREQ_2) {
                auto index  = sensor.type() - AnalogType::FREQ_0;
                auto oldval = sensor.value();
                if (edge[index] != lastedge[index] && edgecnt[index] > 0) {
                    portENTER_CRITICAL_ISR(&mux);
                    auto t          = (edge[index] - lastedge[index]) / edgecnt[index];
                    lastedge[index] = edge[index];
                    edgecnt[index]  = 0;
                    portEXIT_CRITICAL_ISR(&mux);
                    sensor.set_value(sensor.factor() * 1000000.0 / t);
                } else if (micros() - edge[index] > 10000000ul && sensor.value() > 0) {
                    sensor.set_value(0);
                }
                if (sensor.value() != oldval) {
                    changed_ = true;
                    publish_sensor(sensor);
                }
            } else if (sensor.type() >= AnalogType::CNT_0 && sensor.type() <= AnalogType::CNT_2) {
                auto index  = sensor.type() - AnalogType::CNT_0;
                auto oldval = sensor.value();
                portENTER_CRITICAL_ISR(&mux);
                auto c = edgecnt[index];
                edgecnt[index] = 0;
                portEXIT_CRITICAL_ISR(&mux);
                sensor.set_value(oldval + sensor.factor() * c);
                if (sensor.value() != oldval) {
                    changed_ = true;
                    publish_sensor(sensor);
                }
#endif
            }
        }
    }

    // poll digital io every time with debounce
    // go through the list of digital sensors
    for (auto & sensor : sensors_) {
        auto old_value = sensor.value(); // remember current value before reading
        if (sensor.type() == AnalogType::DIGITAL_IN || sensor.type() == AnalogType::COUNTER || sensor.type() == AnalogType::TIMER
            || sensor.type() == AnalogType::RATE) {
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
                        // EMSESP::nvs_.putDouble(sensor.name().c_str(), sensor.value());
                    } else if (sensor.type() == AnalogType::RATE) { // default uom: Hz (1/sec) with factor 1
                        sensor.set_value(sensor.factor() * 1000 / (sensor.polltime_ - sensor.last_polltime_));
                    } else if (sensor.type() == AnalogType::TIMER) { // default seconds with factor 1
                        sensor.set_value(sensor.factor() * (sensor.polltime_ - sensor.last_polltime_) / 1000);
                    }
                    sensor.last_polltime_ = sensor.polltime_;
                }
            }
        }
        if (sensor.type() == AnalogType::PULSE && sensor.value() && sensor.polltime_ && sensor.polltime_ < uuid::get_uptime()) {
            sensor.set_value(0);
            digitalWrite(sensor.gpio(), sensor.offset());
            sensor.polltime_ = 0;
        }
        // see if there is a change and increment # reads
        if (old_value != sensor.value()) {
            sensorreads_++;
            changed_ = true;
            publish_sensor(sensor);
        }
    }

    // store counter-values only every hour to reduce flash wear
    static uint8_t lastSaveHour = 0;
    time_t         now          = time(nullptr);
    tm *           tm_          = localtime(&now);
    if (tm_->tm_hour != lastSaveHour) {
        lastSaveHour = tm_->tm_hour;
        store_counters();
    }
}

// store counters to NVS, called every hour, on restart and update
void AnalogSensor::store_counters() {
    for (auto & sensor : sensors_) {
        if (sensor.type() == AnalogType::COUNTER || (sensor.type() >= AnalogType::CNT_0 && sensor.type() <= AnalogType::CNT_2)) {
            if (sensor.value() != EMSESP::nvs_.getDouble(sensor.name().c_str())) {
                EMSESP::nvs_.putDouble(sensor.name().c_str(), sensor.value());
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

// update analog information name, offset, factor, uom, type, deleted, is_system
// a type value of -1 is used to delete the sensor
// the gpio is the key
bool AnalogSensor::update(uint8_t gpio, std::string & name, double offset, double factor, uint8_t uom, int8_t type, bool deleted, bool is_system) {
    // first see if we can find the sensor in our customization list
    bool found_sensor = false;
    EMSESP::webCustomizationService.update([&](WebCustomization & settings) {
        for (auto & AnalogCustomization : settings.analogCustomizations) {
            if (AnalogCustomization.type == AnalogType::COUNTER
                || (AnalogCustomization.type >= AnalogType::DIGITAL_OUT && AnalogCustomization.type <= AnalogType::PWM_2)
                || AnalogCustomization.type == AnalogType::RGB || AnalogCustomization.type == AnalogType::PULSE) {
                Command::erase_command(EMSdevice::DeviceType::ANALOGSENSOR, AnalogCustomization.name.c_str());
            }
            if (name.empty()) {
                char n[20];
                snprintf(n, sizeof(n), "%s_%02d", FL_(list_sensortype)[type], gpio);
                name = n;
            }
            if (AnalogCustomization.gpio == gpio) {
                found_sensor = true; // found the record
                // see if it's marked for deletion
                if (deleted) {
                    LOG_DEBUG("Removing analog sensor GPIO %02d", gpio);
                    EMSESP::system_.remove_gpio(gpio); // remove from used list only
                    EMSESP::nvs_.remove(AnalogCustomization.name.c_str());
                    settings.analogCustomizations.remove(AnalogCustomization);
                } else {
                    // update existing record
                    if (name != AnalogCustomization.name) {
                        EMSESP::nvs_.remove(AnalogCustomization.name.c_str());
                    }
                    AnalogCustomization.name      = name;
                    AnalogCustomization.offset    = offset;
                    AnalogCustomization.factor    = factor;
                    AnalogCustomization.uom       = uom;
                    AnalogCustomization.type      = type;
                    AnalogCustomization.is_system = is_system;
                    LOG_DEBUG("Customizing existing analog GPIO %02d", gpio);
                }
                return StateUpdateResult::CHANGED; // persist the change
            }
        }
        return StateUpdateResult::UNCHANGED;
    });

    // if the sensor exists and we're using HA, delete the old HA record
    if (found_sensor && Mqtt::ha_enabled()) {
        remove_ha_topic(type, gpio); // the GPIO
    }

    // we didn't find it, it's new, so create and store it in the customization list
    if (!found_sensor) {
        found_sensor = true;
        EMSESP::webCustomizationService.update([&](WebCustomization & settings) {
            auto newSensor      = AnalogCustomization();
            newSensor.gpio      = gpio;
            newSensor.name      = name;
            newSensor.offset    = offset;
            newSensor.factor    = factor;
            newSensor.uom       = uom;
            newSensor.type      = type;
            newSensor.is_system = is_system;
            settings.analogCustomizations.push_back(newSensor);
            // check the gpio again and add to used list
            if (EMSESP::system_.add_gpio(gpio, "Analog Sensor")) {
                LOG_DEBUG("Adding customization for analog sensor GPIO %02d", gpio);
                return StateUpdateResult::CHANGED; // persist the change
            } else {
                found_sensor = false;
                return StateUpdateResult::ERROR; // if we can't add the GPIO, return an error
            }
        });
    }

    // reloads the sensors in the customizations file into the sensors list
    if (found_sensor) {
        reload();
    }

    return found_sensor;
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
        char result[12];
        if (sensor.type() == AnalogType::DIGITAL_IN || sensor.type() == AnalogType::DIGITAL_OUT) {
            Helpers::render_boolean(result, sensor.value() != 0);
        } else {
            Helpers::render_value(result, sensor.value(), 2); // double
        }
        Mqtt::queue_publish(topic, result); // always publish as doubles
    }
    char cmd[COMMAND_MAX_LENGTH];
    snprintf(cmd, sizeof(cmd), "%s/%s", F_(analogsensor), sensor.name().c_str());
    EMSESP::webSchedulerService.onChange(cmd);
}

// send empty config topic to remove the entry from HA
void AnalogSensor::remove_ha_topic(const int8_t type, const uint8_t gpio) const {
    if (!Mqtt::ha_enabled()) {
        return;
    }

    LOG_DEBUG("Removing HA config for analog sensor GPIO %02d", gpio);
    char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];

#if CONFIG_IDF_TARGET_ESP32
    if (type == AnalogType::PULSE || (type == AnalogType::DIGITAL_OUT && gpio != 25 && gpio != 26)) {
#else
    if (type == AnalogType::PULSE || type == AnalogType::DIGITAL_OUT) {
#endif
        snprintf(topic, sizeof(topic), "switch/%s/%s_%02d/config", Mqtt::basename().c_str(), F_(analogsensor), gpio);
    } else if (type == AnalogType::DIGITAL_OUT) { // DAC
        snprintf(topic, sizeof(topic), "number/%s/%s_%02d/config", Mqtt::basename().c_str(), F_(analogsensor), gpio);
    } else if (type >= AnalogType::PWM_0 && type <= AnalogType::PWM_2) {
        snprintf(topic, sizeof(topic), "number/%s/%s_%02d/config", Mqtt::basename().c_str(), F_(analogsensor), gpio);
    } else if (type >= AnalogType::RGB) {
        snprintf(topic, sizeof(topic), "number/%s/%s_%02d/config", Mqtt::basename().c_str(), F_(analogsensor), gpio);
    } else if (type == AnalogType::DIGITAL_IN) {
        snprintf(topic, sizeof(topic), "binary_sensor/%s/%s_%02d/config", Mqtt::basename().c_str(), F_(analogsensor), gpio);
    } else {
        snprintf(topic, sizeof(topic), "sensor/%s/%s_%02d/config", Mqtt::basename().c_str(), F_(analogsensor), gpio);
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

    JsonDocument doc;

    for (auto & sensor : sensors_) {
        if (Mqtt::is_nested()) {
            char       s[10];
            JsonObject dataSensor = doc[Helpers::smallitoa(s, sensor.gpio())].to<JsonObject>();
            dataSensor["name"]    = sensor.name();
#if CONFIG_IDF_TARGET_ESP32
            if (sensor.type() == AnalogType::PULSE || (sensor.type() == AnalogType::DIGITAL_OUT && sensor.gpio() != 25 && sensor.gpio() != 26)) {
#else
            if (sensor.type() == AnalogType::PULSE || sensor.type() == AnalogType::DIGITAL_OUT) {
#endif
                if (EMSESP::system_.bool_format() == BOOL_FORMAT_TRUEFALSE) {
                    dataSensor["value"] = sensor.value() != 0;
                } else if (EMSESP::system_.bool_format() == BOOL_FORMAT_10) {
                    dataSensor["value"] = sensor.value() != 0 ? 1 : 0;
                } else {
                    char result[12];
                    dataSensor["value"] = Helpers::render_boolean(result, sensor.value() != 0);
                }
            } else {
                dataSensor["value"] = serialized(Helpers::render_value(s, sensor.value(), 2)); // double
            }
        } else if (sensor.type() == AnalogType::DIGITAL_IN || sensor.type() == AnalogType::DIGITAL_OUT || sensor.type() == AnalogType::PULSE) {
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

        // create HA config if hasn't already been done
        if (Mqtt::ha_enabled() && (!sensor.ha_registered || force)) {
            LOG_DEBUG("Recreating HA config for analog sensor GPIO %02d", sensor.gpio());

            JsonDocument config;
            config["~"]          = Mqtt::base();

            char stat_t[50];
            snprintf(stat_t, sizeof(stat_t), "~/%s_data", F_(analogsensor)); // use base path
            config["stat_t"] = stat_t;

            char val_obj[50];
            char val_cond[95];
            if (Mqtt::is_nested()) {
                snprintf(val_obj, sizeof(val_obj), "value_json['%02d']['value']", sensor.gpio());
                snprintf(val_cond, sizeof(val_cond), "value_json['%02d'] is defined and %s is defined", sensor.gpio(), val_obj);
            } else {
                snprintf(val_obj, sizeof(val_obj), "value_json['%s']", sensor.name().c_str());
                snprintf(val_cond, sizeof(val_cond), "%s is defined", val_obj);
            }
            char sample_val[12] = "0";
            if (sensor.type() == AnalogType::DIGITAL_IN || sensor.type() == AnalogType::DIGITAL_OUT || sensor.type() == AnalogType::PULSE) {
                Helpers::render_boolean(sample_val, false);
            }
            // don't bother with value template conditions if using Domoticz which doesn't fully support MQTT Discovery
            if (Mqtt::discovery_type() == Mqtt::discoveryType::HOMEASSISTANT) {
                config["val_tpl"] = (std::string) "{{" + val_obj + " if " + val_cond + "}}";
            } else {
                config["val_tpl"] = (std::string) "{{" + val_obj + "}}";
            }

            char uniq_s[70];
            if (Mqtt::entity_format() == Mqtt::entityFormat::MULTI_SHORT) {
                snprintf(uniq_s, sizeof(uniq_s), "%s_%s_%02d", Mqtt::basename().c_str(), F_(analogsensor), sensor.gpio());
            } else {
                snprintf(uniq_s, sizeof(uniq_s), "%s_%02d", F_(analogsensor), sensor.gpio());
            }

            config["~"] = Mqtt::base();
            config["uniq_id"] = uniq_s;

            char name[50];
            snprintf(name, sizeof(name), "%s", sensor.name().c_str());
            config["name"] = name;

            if (sensor.uom() != DeviceValueUOM::NONE && sensor.type() != AnalogType::DIGITAL_OUT) {
                config["unit_of_meas"] = EMSdevice::uom_to_string(sensor.uom());
            }

            char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];

            // Set commands for some analog types
            char command_topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
#if CONFIG_IDF_TARGET_ESP32
            if (sensor.type() == AnalogType::PULSE || (sensor.type() == AnalogType::DIGITAL_OUT && sensor.gpio() != 25 && sensor.gpio() != 26)) {
#else
            if (sensor.type() == AnalogType::PULSE || sensor.type() == AnalogType::DIGITAL_OUT) {
#endif
                snprintf(topic, sizeof(topic), "switch/%s/%s_%02d/config", Mqtt::basename().c_str(), F_(analogsensor), sensor.gpio());
                snprintf(command_topic, sizeof(command_topic), "%s/%s/%s", Mqtt::base().c_str(), F_(analogsensor), sensor.name().c_str());
                config["cmd_t"] = command_topic;
                Mqtt::add_ha_bool(config.as<JsonObject>());
            } else if (sensor.type() == AnalogType::DIGITAL_OUT) { // DAC
                snprintf(topic, sizeof(topic), "number/%s/%s_%02d/config", Mqtt::basename().c_str(), F_(analogsensor), sensor.gpio());
                snprintf(command_topic, sizeof(command_topic), "%s/%s/%s", Mqtt::base().c_str(), F_(analogsensor), sensor.name().c_str());
                config["cmd_t"] = command_topic;
                config["min"]   = 0;
                config["max"]   = 255;
                config["mode"]  = "box"; // auto, slider or box
                config["step"]  = 1;
            } else if (sensor.type() >= AnalogType::PWM_0 && sensor.type() <= AnalogType::PWM_2) {
                snprintf(topic, sizeof(topic), "number/%s/%s_%02d/config", Mqtt::basename().c_str(), F_(analogsensor), sensor.gpio());
                snprintf(command_topic, sizeof(command_topic), "%s/%s/%s", Mqtt::base().c_str(), F_(analogsensor), sensor.name().c_str());
                config["cmd_t"] = command_topic;
                config["min"]   = 0;
                config["max"]   = 100;
                config["mode"]  = "box"; // auto, slider or box
                config["step"]  = 0.1;
            } else if (sensor.type() == AnalogType::RGB) {
                snprintf(topic, sizeof(topic), "number/%s/%s_%02d/config", Mqtt::basename().c_str(), F_(analogsensor), sensor.gpio());
                snprintf(command_topic, sizeof(command_topic), "%s/%s/%s", Mqtt::base().c_str(), F_(analogsensor), sensor.name().c_str());
                config["cmd_t"] = command_topic;
                config["min"]   = 0;
                config["max"]   = 999999;
                config["mode"]  = "box"; // auto, slider or box
                config["step"]  = 1;
            } else if (sensor.type() == AnalogType::COUNTER || (sensor.type() >= AnalogType::CNT_0 && sensor.type() <= AnalogType::CNT_2)) {
                snprintf(topic, sizeof(topic), "sensor/%s/%s_%02d/config", Mqtt::basename().c_str(), F_(analogsensor), sensor.gpio());
                snprintf(command_topic, sizeof(command_topic), "%s/%s/%s", Mqtt::base().c_str(), F_(analogsensor), sensor.name().c_str());
                config["cmd_t"]    = command_topic;
                config["stat_cla"] = "total_increasing";
                // config["mode"]  = "box"; // auto, slider or box
                // config["step"]  = sensor.factor();
            } else if (sensor.type() == AnalogType::DIGITAL_IN) {
                snprintf(topic, sizeof(topic), "binary_sensor/%s/%s_%02d/config", Mqtt::basename().c_str(), F_(analogsensor), sensor.gpio());
                Mqtt::add_ha_bool(config.as<JsonObject>());
            } else {
                snprintf(topic, sizeof(topic), "sensor/%s/%s_%02d/config", Mqtt::basename().c_str(), F_(analogsensor), sensor.gpio());
                config["stat_cla"] = "measurement";
            }

            // see if we need to create the [devs] discovery section, as this needs only to be done once for all sensors
            bool is_ha_device_created = false;
            for (auto const & sensor : sensors_) {
                if (sensor.ha_registered) {
                    is_ha_device_created = true;
                    break;
                }
            }

            // add default_entity_id
            std::string topic_str(topic);
            doc["def_ent_id"] = topic_str.substr(0, topic_str.find("/")) + "." + uniq_s;

            Mqtt::add_ha_dev_section(config.as<JsonObject>(), "Analog Sensors", nullptr, nullptr, nullptr, false);
            Mqtt::add_ha_avail_section(config.as<JsonObject>(), stat_t, !is_ha_device_created, val_cond);

            sensor.ha_registered = Mqtt::queue_ha(topic, config.as<JsonObject>());
        }
    }

    char topic[Mqtt::MQTT_TOPIC_MAX_SIZE];
    snprintf(topic, sizeof(topic), "%s_data", F_(analogsensor));
    Mqtt::queue_publish(topic, doc.as<JsonObject>());
}

// called from emsesp.cpp for commands
// searches sensor by name
bool AnalogSensor::get_value_info(JsonObject output, const char * cmd, const int8_t id) {
    if (sensors_.empty()) {
        return true; // no sensors, return true
    }

    // return all values if its an info and values command
    if (!strcmp(cmd, F_(info)) || !strcmp(cmd, F_(values))) {
        for (const auto & sensor : sensors_) {
            output[sensor.name()] = sensor.value();
        }
        return true;
    }

    // show all entity details of the command is entities
    if (!strcmp(cmd, F_(entities))) {
        for (const auto & sensor : sensors_) {
            get_value_json(output[sensor.name()].to<JsonObject>(), sensor);
        }
        return true;
    }

    // this is for a specific sensor, return the value
    const char * attribute_s = Command::get_attribute(cmd);

    for (const auto & sensor : sensors_) {
        // match custom name or sensor GPIO
        if (cmd == Helpers::toLower(sensor.name()) || Helpers::atoint(cmd) == sensor.gpio()) {
            get_value_json(output, sensor);
            return Command::get_attribute(output, cmd, attribute_s);
        }
    }

    return false; // not found
}

// note we don't add the device and state classes here, as we do in the custom entity service
void AnalogSensor::get_value_json(JsonObject output, const Sensor & sensor) {
    output["name"]      = sensor.name();
    output["fullname"]  = sensor.name();
    output["gpio"]      = sensor.gpio();
    output["type"]      = F_(number);
    output["analog"]    = FL_(list_sensortype)[sensor.type()];
    output["value"]     = sensor.value();
    output["readable"]  = true;
    output["writeable"] = sensor.type() == AnalogType::COUNTER || sensor.type() == AnalogType::RGB || sensor.type() == AnalogType::PULSE
                          || (sensor.type() >= AnalogType::DIGITAL_OUT && sensor.type() <= AnalogType::PWM_2)|| (sensor.type() >= AnalogType::CNT_0 && sensor.type() <= AnalogType::CNT_2);
    output["visible"]   = true;
    output["is_system"] = sensor.is_system();
    if (sensor.type() == AnalogType::COUNTER|| (sensor.type() >= AnalogType::CNT_0 && sensor.type() <= AnalogType::CNT_2)) {
        output["min"]         = 0;
        output["max"]         = 4000000;
        output["start_value"] = sensor.offset();
        output["factor"]      = sensor.factor();
        output["uom"]         = EMSdevice::uom_to_string(sensor.uom());
    } else if (sensor.type() == AnalogType::ADC) {
        output["offset"] = sensor.offset();
        output["factor"] = sensor.factor();
        output["uom"]    = EMSdevice::uom_to_string(sensor.uom());
    } else if (sensor.type() == AnalogType::TIMER || sensor.type() == AnalogType::RATE) {
        output["factor"] = sensor.factor();
    } else if (sensor.type() >= AnalogType::PWM_0 && sensor.type() <= AnalogType::PWM_2) {
        output["frequency"] = sensor.factor();
        output["min"]       = 0;
        output["max"]       = 100;
        output["uom"]       = EMSdevice::uom_to_string(sensor.uom());
    } else if (sensor.type() == AnalogType::DIGITAL_OUT) {
        output["min"] = 0;
#if CONFIG_IDF_TARGET_ESP32
        output["max"] = sensor.gpio() == 25 || sensor.gpio() == 26 ? 255 : 1;
#else
        output["max"] = 1;
#endif
        output["start"] = sensor.uom() == 1 ? "0" : sensor.uom() == 2 ? "1" : "?";
    } else if (sensor.type() == AnalogType::PULSE) {
        output["min"] = 0;
        output["max"] = 1;
    }
}

// this creates the sensor, initializing everything
AnalogSensor::Sensor::Sensor(const uint8_t       gpio,
                             const std::string & name,
                             const double        offset,
                             const double        factor,
                             const uint8_t       uom,
                             const int8_t        type,
                             const bool          is_system)
    : gpio_(gpio)
    , name_(name)
    , offset_(offset)
    , factor_(factor)
    , uom_(uom)
    , type_(type)
    , is_system_(is_system) {
    value_ = 0; // init value to 0 always
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
            double oldoffset = sensor.offset();
            if (sensor.type() == AnalogType::COUNTER || (sensor.type() >= AnalogType::CNT_0 && sensor.type() <= AnalogType::CNT_2)) {
                if (val < 0 || value[0] == '+') { // sign corrects values
                    // sensor.set_offset(sensor.value() + val);
                    sensor.set_value(sensor.value() + val);
                } else { // positive values are set
                    // sensor.set_offset(val);
                    sensor.set_value(val);
                }
                sensor.set_offset(sensor.value());
                if (sensor.value() != EMSESP::nvs_.getDouble(sensor.name().c_str(), 0)) {
                    EMSESP::nvs_.putDouble(sensor.name().c_str(), sensor.value());
                }
            } else if (sensor.type() == AnalogType::ADC) {
                sensor.set_offset(val);
            } else if (sensor.type() == AnalogType::RGB) {
                uint32_t v = val;
                sensor.set_offset(v);
                sensor.set_value(v);
                uint8_t r = v / 10000;
                uint8_t g = (v - r * 10000) / 100;
                uint8_t b = v % 100;
#if ESP_ARDUINO_VERSION_MAJOR < 3
                neopixelWrite(sensor.gpio(), 2 * r, 2 * g, 2 * b);
#else
                rgbLedWrite(sensor.gpio(), 2 * r, 2 * g, 2 * b);
#endif
                LOG_DEBUG("RGB set to %d, %d, %d", r, g, b);
            } else if (sensor.type() == AnalogType::PULSE) {
                uint8_t v = val;
                sensor.set_value(v);
                pinMode(sensor.gpio(), OUTPUT);
                digitalWrite(sensor.gpio(), (sensor.offset() != 0) ^ (sensor.value() != 0));
                sensor.polltime_ = sensor.value() != 0 ? uuid::get_uptime() + (sensor.factor() * 1000) : 0;
            } else if (sensor.type() == AnalogType::DIGITAL_OUT) {
                uint8_t v = val;
#if CONFIG_IDF_TARGET_ESP32
                if ((sensor.gpio() == 25 || sensor.gpio() == 26) && v <= 255) {
                    sensor.set_offset(v);
                    sensor.set_value(v);
                    pinMode(sensor.gpio(), OUTPUT);
                    dacWrite(sensor.gpio(), sensor.offset());
                } else
#elif CONFIG_IDF_TARGET_ESP32S2
                if ((sensor.gpio() == 17 || sensor.gpio() == 18) && v <= 255) {
                    sensor.set_offset(v);
                    sensor.set_value(v);
                    pinMode(sensor.gpio(), OUTPUT);
                    dacWrite(sensor.gpio(), sensor.offset());
                } else
#endif
                    if (v == 0 || v == 1) {
                    sensor.set_offset(v);
                    sensor.set_value(v);
                    pinMode(sensor.gpio(), OUTPUT);
                    digitalWrite(sensor.gpio(), (sensor.offset() == 0) ^ (sensor.factor() > 0));
                    if (sensor.uom() == 0 && EMSESP::nvs_.getChar(sensor.name().c_str()) != (int8_t)sensor.offset()) {
                        EMSESP::nvs_.putChar(sensor.name().c_str(), (int8_t)sensor.offset());
                    }
                }
            } else if (sensor.type() >= AnalogType::PWM_0 && sensor.type() <= AnalogType::PWM_2) {
                if (val > 100) {
                    val = 100;
                } else if (val < 0) {
                    val = 0;
                }
                sensor.set_offset(val);
                sensor.set_value(val);
#if ESP_IDF_VERSION_MAJOR >= 5
                ledcWrite(sensor.gpio(), (uint32_t)(sensor.offset() * 8191 / 100));
#else
                uint8_t channel = sensor.type() - AnalogType::PWM_0;
                ledcWrite(channel, (uint32_t)(val * 8191 / 100));
#endif
            } else {
                return false;
            }
            if (oldoffset != sensor.offset()) {
                publish_sensor(sensor);
                changed_ = true;
            }
            return true;
        }
    }

    return false;
}

} // namespace emsesp