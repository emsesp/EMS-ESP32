/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2025  emsesp.org - proddy, MichaelDvP
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

#include <esp32-psram.h>

namespace emsesp {

class AnalogSensor {
  public:
    class Sensor {
      public:
        Sensor(const uint8_t gpio, const char * name, const double offset, const double factor, const uint8_t uom, const int8_t type, const bool is_system);
        ~Sensor() = default;

        void set_offset(const double offset) {
            offset_ = offset;
        }

        const char * name() const {
            return name_;
        }

        void set_name(const char * name) {
            strlcpy(name_, name, sizeof(name_));
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

        bool is_system() const {
            return is_system_;
        }
        void set_is_system(const bool is_system) {
            is_system_ = is_system;
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
        uint8_t  gpio_;
        char     name_[20];
        double   offset_;
        double   factor_;
        uint8_t  uom_;
        double   value_;     // double because of the factor is a double
        int8_t   type_;      // one of the AnalogType enum
        bool     is_system_; // if true, the sensor is a system sensor
    };

    AnalogSensor()  = default;
    ~AnalogSensor() = default;

    enum AnalogType : int8_t {
        DIGITAL_IN  = 1,
        COUNTER     = 2,
        ADC         = 3,
        TIMER       = 4,
        RATE        = 5,
        DIGITAL_OUT = 6,
        PWM_0       = 7,
        PWM_1       = 8,
        PWM_2       = 9,
        NTC         = 10,
        RGB         = 11,
        PULSE       = 12,
        FREQ_0      = 13,
        FREQ_1      = 14,
        FREQ_2      = 15,
        CNT_0       = 16,
        CNT_1       = 17,
        CNT_2       = 18
    };

    void start(const bool factory_settings = false);
    void loop();
    void publish_sensor(const Sensor & sensor) const;
    void publish_values(const bool force);
    void reload(bool get_nvs = false);
    bool updated_values();

    // return back reference to the sensor list, used by other classes
    std::vector<Sensor, AllocatorPSRAM<Sensor>> sensors() const {
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

    // count number of items in sensors_ where type is not set to disabled and not a system sensor
    size_t count_entities(bool exclude_disabled_system = false) const {
        if (exclude_disabled_system) {
            return std::count_if(sensors_.begin(), sensors_.end(), [](const Sensor & sensor) { return !sensor.is_system(); });
        }
        return sensors_.size();
    }

    bool                        update(uint8_t gpio, const char * name, double offset, double factor, uint8_t uom, int8_t type, bool deleted, bool is_system);
    bool                        get_value_info(JsonObject output, const char * cmd, const int8_t id = -1);
    void                        store_counters();
    static std::vector<uint8_t> exclude_types() {
        return exclude_types_;
    }

  private:
    static constexpr double   Beta                    = 4260;
    static constexpr double   T0                      = 273.15;
    static constexpr double   T25                     = 298.15;
    static constexpr double   R0                      = 100000;
    static constexpr double   Rt                      = 60000;
    static constexpr uint8_t  MAX_SENSORS             = 20;
    static constexpr uint32_t MEASURE_ANALOG_INTERVAL = 500;

    static uuid::log::Logger logger_;
    void                     remove_ha_topic(const int8_t type, const uint8_t id) const;
    bool                     command_setvalue(const char * value, const int8_t gpio);
    void                     measure();
    void                     addSensorJson(JsonObject output, const Sensor & sensor);
    void                     get_value_json(JsonObject output, const Sensor & sensor);

    std::vector<Sensor, AllocatorPSRAM<Sensor>> sensors_; // our list of sensors
    static std::vector<uint8_t>                 exclude_types_;

    bool     analog_enabled_;
    bool     changed_     = true; // this will force a publish of all sensors when initialising
    uint32_t sensorfails_ = 0;
    uint32_t sensorreads_ = 0;

#ifndef EMSESP_STANDALONE
    static void IRAM_ATTR freqIrq0();
    static void IRAM_ATTR freqIrq1();
    static void IRAM_ATTR freqIrq2();
    static unsigned long  edge[3];
    static unsigned long  edgecnt[3];
    unsigned long         lastedge[3] = {0, 0, 0};
#endif
};

} // namespace emsesp

#endif