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

#ifndef EMSESP_DALLASSENSOR_H
#define EMSESP_DALLASSENSOR_H

#include <string>
#include <vector>

#include "helpers.h"
#include "mqtt.h"
#include "console.h"

#include <uuid/log.h>

#ifndef EMSESP_STANDALONE
#include <OneWire.h>
#endif

namespace emsesp {

enum Dallas_Format : uint8_t { SENSORID = 1, NUMBER, NAME };

class DallasSensor {
  public:
    class Sensor {
      public:
        Sensor(const uint8_t addr[]);
        ~Sensor() = default;

        uint64_t    id() const;
        std::string id_string() const;
        std::string to_string(const bool name = false) const;
        int16_t     offset() const;

        int16_t temperature_c = EMS_VALUE_SHORT_NOTSET;
        bool    read          = false;

      private:
        const uint64_t id_;
    };

    DallasSensor()  = default;
    ~DallasSensor() = default;

    void start();
    void loop();
    void publish_values(const bool force);
    void reload();
    bool updated_values();

    const std::vector<Sensor> sensors() const;

    uint32_t reads() {
        return sensorreads_;
    }

    uint32_t fails() {
        return sensorfails_;
    }

    bool dallas_enabled() {
        return (dallas_gpio_ != 0);
    }

    uint8_t dallas_format() {
        return dallas_format_;
    }

    void dallas_format(uint8_t dallas_format) {
        dallas_format_ = dallas_format;
    }

    bool update(const char * idstr, const char * name, int16_t offset);

  private:
    static constexpr uint8_t MAX_SENSORS = 20;

    enum class State { IDLE, READING, SCANNING };

    static constexpr size_t ADDR_LEN = 8;

    static constexpr size_t SCRATCHPAD_LEN      = 9;
    static constexpr size_t SCRATCHPAD_TEMP_MSB = 1;
    static constexpr size_t SCRATCHPAD_TEMP_LSB = 0;
    static constexpr size_t SCRATCHPAD_CONFIG   = 4;
    static constexpr size_t SCRATCHPAD_CNT_REM  = 6;

    // dallas chips
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

#ifndef EMSESP_STANDALONE
    OneWire bus_;
#endif

    bool     temperature_convert_complete();
    int16_t  get_temperature_c(const uint8_t addr[]);
    uint64_t get_id(const uint8_t addr[]);

    bool command_info(const char * value, const int8_t id, JsonObject & output);
    bool command_commands(const char * value, const int8_t id, JsonObject & output);

    void delete_ha_config(uint8_t index, const char * name);

    uint32_t            last_activity_ = uuid::get_uptime();
    State               state_         = State::IDLE;
    std::vector<Sensor> sensors_;

    bool registered_ha_[MAX_SENSORS];

    int8_t   scancnt_       = SCAN_START;
    uint8_t  firstscan_     = 0;
    uint8_t  dallas_gpio_   = 0;
    bool     parasite_      = false;
    bool     changed_       = false;
    uint32_t sensorfails_   = 0;
    uint32_t sensorreads_   = 0;
    int8_t   scanretry_     = 0;
    uint8_t  dallas_format_ = 0;
};

} // namespace emsesp

#endif