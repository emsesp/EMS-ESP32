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

#ifndef EMSESP_TEMPERATURESENSOR_H
#define EMSESP_TEMPERATURESENSOR_H

#include "helpers.h"
#include "mqtt.h"
#include "console.h"

#include <uuid/log.h>

#ifndef EMSESP_STANDALONE
#include <OneWire.h>
#endif

namespace emsesp {

class TemperatureSensor {
  public:
    class Sensor {
      public:
        Sensor(const uint8_t addr[]);
        ~Sensor() = default;

        uint64_t internal_id() const {
            return internal_id_;
        }

        std::string id() const {
            return id_;
        }

        int16_t offset() const {
            return offset_;
        }
        void set_offset(const int16_t offset) {
            offset_ = offset;
        }

        std::string name() const;
        void        set_name(const std::string & name) {
            name_ = name;
        }

        bool apply_customization();

        // initial values
        int16_t temperature_c = EMS_VALUE_INT16_NOTSET; // value is *10
        bool    read          = false;
        bool    ha_registered = false;

      private:
        uint64_t    internal_id_;
        std::string id_;
        std::string name_;
        int16_t     offset_;
    };

    TemperatureSensor()  = default;
    ~TemperatureSensor() = default;

    void start();
    void loop();
    void publish_sensor(const Sensor & sensor);
    void publish_values(const bool force);
    void reload();
    bool updated_values();
    bool get_value_info(JsonObject output, const char * cmd, const int8_t id = -1);

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

    bool sensor_enabled() const {
        return (dallas_gpio_ != 0);
    }

    bool have_sensors() const {
        return (!sensors_.empty());
    }

    size_t count_entities() const {
        return sensors_.size();
    }

    bool update(const std::string & id, const std::string & name, int16_t offset);

#if defined(EMSESP_TEST)
    void test();
#endif

  private:
    static constexpr uint8_t MAX_SENSORS = 20;

    enum class State { IDLE, READING, SCANNING };

    static constexpr size_t ADDR_LEN = 8;

    static constexpr size_t SCRATCHPAD_LEN      = 9;
    static constexpr size_t SCRATCHPAD_TEMP_MSB = 1;
    static constexpr size_t SCRATCHPAD_TEMP_LSB = 0;
    static constexpr size_t SCRATCHPAD_CONFIG   = 4;
    static constexpr size_t SCRATCHPAD_CNT_REM  = 6;

    // dallas chip types
    static constexpr uint8_t TYPE_DS18B20 = 0x28;
    static constexpr uint8_t TYPE_DS18S20 = 0x10;
    static constexpr uint8_t TYPE_DS1822  = 0x22;
    static constexpr uint8_t TYPE_DS1825  = 0x3B; // also DS1826

    static constexpr uint32_t READ_INTERVAL_MS = 5000; // 5 seconds
    static constexpr uint32_t CONVERSION_MS    = 1000; // 1 seconds
    static constexpr uint32_t READ_TIMEOUT_MS  = 2000; // 2 seconds
    static constexpr uint32_t SCAN_TIMEOUT_MS  = 3000; // 3 seconds

    static constexpr uint8_t CMD_CONVERT_TEMP    = 0x44;
    static constexpr uint8_t CMD_READ_SCRATCHPAD = 0xBE;

    static constexpr int8_t SCAN_START = -3;
    static constexpr int8_t SCAN_MAX   = 5;

    static uuid::log::Logger logger_;

    bool     temperature_convert_complete();
    int16_t  get_temperature_c(const uint8_t addr[]);
    uint64_t get_id(const uint8_t addr[]);
    void     get_value_json(JsonObject output, const Sensor & sensor);
    void     remove_ha_topic(const std::string & id);

    std::vector<Sensor> sensors_; // our list of active sensors

#ifndef EMSESP_STANDALONE
    OneWire  bus_;
    uint32_t last_activity_ = uuid::get_uptime();
    State    state_         = State::IDLE;
    int8_t   scancnt_       = SCAN_START;
    uint8_t  firstscan_     = 0;
    int8_t   scanretry_     = 0;
#endif

    uint8_t  dallas_gpio_ = 0;
    bool     parasite_    = false;
    bool     changed_     = false;
    uint32_t sensorfails_ = 0;
    uint32_t sensorreads_ = 0;
};

} // namespace emsesp

#endif