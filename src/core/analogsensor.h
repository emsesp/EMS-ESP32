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

#ifndef EMSESP_ANALOGSENSOR_H
#define EMSESP_ANALOGSENSOR_H

#include "helpers.h"
#include "mqtt.h"
#include "console.h"

#include <uuid/log.h>

namespace emsesp {

// names, same order as AnalogType
MAKE_ENUM_FIXED(AnalogTypeName, "disabled", "dig_in", "counter", "adc", "timer", "rate", "dig_out", "pwm0", "pwm1", "pwm2")

class AnalogSensor {
  public:
    class Sensor {
      public:
        Sensor(const uint8_t gpio, const std::string & name, const double offset, const double factor, const uint8_t uom, const int8_t type);
        ~Sensor() = default;

        void set_offset(const double offset) {
            offset_ = offset;
        }

        std::string name() const {
            return name_;
        }

        void set_name(const std::string & name) {
            name_ = name;
        }

        uint8_t gpio() const {
            return gpio_;
        }

        double value() const {
            return value_;
        }

        void set_value(const double value) {
            value_ = value;
        }

        double factor() const {
            return factor_;
        }

        void set_factor(const double factor) {
            factor_ = factor;
        }

        double offset() const {
            return offset_;
        }

        void set_uom(const uint8_t uom) {
            uom_ = uom;
        }

        uint8_t uom() const {
            return uom_;
        }

        int8_t type() const {
            return type_;
        }

        void set_type(const int8_t type) {
            type_ = type;
        }

        bool ha_registered = false;

        uint16_t analog_        = 0; // ADC - average value
        uint32_t sum_           = 0; // ADC - rolling sum
        uint16_t last_reading_  = 0; // IO COUNTER & ADC - last reading
        uint16_t count_         = 0; // counter raw counts
        uint32_t polltime_      = 0; // digital IO & COUNTER debounce time
        int      poll_          = 0;
        uint32_t last_polltime_ = 0; // for timer

      private:
        uint8_t     gpio_;
        std::string name_;
        double      offset_;
        double      factor_;
        uint8_t     uom_;
        double      value_; // double because of the factor is a double
        int8_t      type_;  // one of the AnalogType enum
    };

    AnalogSensor()  = default;
    ~AnalogSensor() = default;

    enum AnalogType : int8_t {
        NOTUSED     = 0, // 0 = disabled
        DIGITAL_IN  = 1,
        COUNTER     = 2,
        ADC         = 3,
        TIMER       = 4,
        RATE        = 5,
        DIGITAL_OUT = 6,
        PWM_0       = 7,
        PWM_1       = 8,
        PWM_2       = 9
    };

    void start();
    void loop();
    void publish_sensor(const Sensor & sensor) const;
    void publish_values(const bool force);
    void reload(bool get_nvs = false);
    bool updated_values();

    // return back reference to the sensor list, used by other classes
    std::vector<Sensor> sensors() const {
        return sensors_;
    }

    uint32_t reads() const {
        return sensorreads_;
    }

    uint32_t fails() const {
        return sensorfails_;
    }

    bool analog_enabled() const {
        return (analog_enabled_);
    }

    bool have_sensors() const {
        return (!sensors_.empty());
    }

    size_t count_entities(bool include_disabled = true) const {
        if (!include_disabled) {
            // count number of items in sensors_ where type is not set to disabled
            return std::count_if(sensors_.begin(), sensors_.end(), [](const Sensor & sensor) { return sensor.type() != AnalogSensor::AnalogType::NOTUSED; });
        }
        return sensors_.size();
    }

    bool update(uint8_t gpio, std::string & name, double offset, double factor, uint8_t uom, int8_t type, bool deleted = false);
    bool get_value_info(JsonObject output, const char * cmd, const int8_t id = -1);
    void store_counters();

  private:
    static constexpr uint8_t  MAX_SENSORS             = 20;
    static constexpr uint32_t MEASURE_ANALOG_INTERVAL = 500;

    static uuid::log::Logger logger_;

    void remove_ha_topic(const int8_t type, const uint8_t id) const;
    bool command_setvalue(const char * value, const int8_t gpio);
    void measure();
    void addSensorJson(JsonObject output, const Sensor & sensor);
    void get_value_json(JsonObject output, const Sensor & sensor);

    std::vector<Sensor> sensors_; // our list of sensors

    bool     analog_enabled_;
    bool     changed_     = false;
    uint32_t sensorfails_ = 0;
    uint32_t sensorreads_ = 0;
};

} // namespace emsesp

#endif