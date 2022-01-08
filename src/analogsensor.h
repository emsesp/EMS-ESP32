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

#ifndef EMSESP_ANALOGSENSOR_H
#define EMSESP_ANALOGSENSOR_H

#include "helpers.h"
#include "mqtt.h"
#include "console.h"

#ifndef EMSESP_STANDALONE
#include "driver/adc.h"
#include <esp_bt.h>
#endif

#include <uuid/log.h>

namespace emsesp {

class AnalogSensor {
  public:
    class Sensor {
      public:
        Sensor(const uint8_t id, const std::string & name, const uint16_t offset, const float factor, const uint8_t uom, const int8_t type);
        ~Sensor() = default;

        void set_offset(const uint16_t offset) {
            offset_ = offset;
        }

        std::string name() const;
        void        set_name(const std::string & name) {
            name_ = name;
        }

        uint8_t id() const {
            return id_;
        }

        float value() const {
            return value_;
        }

        void set_value(float value) {
            value_ = value;
        }

        float factor() const {
            return factor_;
        }

        void set_factor(float factor) {
            factor_ = factor;
        }

        uint16_t offset() const {
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

        void set_type(int8_t type) {
            type_ = type;
        }

        bool ha_registered = false;

        uint16_t analog_       = 0; // ADC - average value
        uint32_t sum_          = 0; // ADC - rolling sum
        uint16_t last_reading_ = 0; // IO COUNTER & ADC - last reading
        uint32_t polltime_     = 0; // digital IO & COUNTER debounce time
        int      poll_         = 0;

      private:
        uint8_t     id_;
        std::string name_;
        uint16_t    offset_;
        float       factor_;
        uint8_t     uom_;
        float       value_; // float because of the factor is a float
        int8_t      type_;
    };

    AnalogSensor()  = default;
    ~AnalogSensor() = default;

    enum AnalogType : int8_t {
        MARK_DELETED = -1, // mark for deletion
        NOTUSED,           // 0 - disabled
        DIGITAL_IN,        // 1
        COUNTER,           // 2
        ADC                // 3
    };

    void start();
    void loop();
    void publish_sensor(Sensor sensor);
    void publish_values(const bool force);
    void reload();
    bool updated_values();

    // return back reference to the sensor list, used by other classes
    const std::vector<Sensor> sensors() const {
        return sensors_;
    }

    uint32_t reads() {
        return sensorreads_;
    }

    uint32_t fails() {
        return sensorfails_;
    }

    bool analog_enabled() {
        return (analog_enabled_);
    }

    bool have_sensors() {
        return (sensors_.size() > 0);
    }

    size_t no_sensors() {
        return sensors_.size();
    }

    bool update(uint8_t id, const std::string & name, uint16_t offset, float factor, uint8_t uom, int8_t type);
    bool get_value_info(JsonObject & output, const char * cmd, const int8_t id);

#ifdef EMSESP_DEBUG
    void test();
#endif

  private:
    static constexpr uint8_t  MAX_SENSORS             = 20;
    static constexpr uint32_t MEASURE_ANALOG_INTERVAL = 500;

    static uuid::log::Logger logger_;

    void remove_ha_topic(const uint8_t id);
    bool command_counter(const char * value, const int8_t id);
    void measure();
    bool command_info(const char * value, const int8_t id, JsonObject & output);

    std::vector<Sensor> sensors_; // our list of sensors

    bool     analog_enabled_;
    bool     changed_     = false;
    uint32_t sensorfails_ = 0; // this is always zero for now
    uint32_t sensorreads_ = 0;
};

} // namespace emsesp

#endif