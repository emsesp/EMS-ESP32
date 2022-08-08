
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

#ifndef EMSESP_EMSDEVICEVALUE_H_
#define EMSESP_EMSDEVICEVALUE_H_

#include <Arduino.h>
#include <ArduinoJson.h>

#include "helpers.h"          // for conversions
#include "default_settings.h" // for enum types
#include <uuid/common.h>      // for read_flash_string

namespace emsesp {

// DeviceValue holds the information for a device entity
class DeviceValue {
  public:
    enum DeviceValueType : uint8_t {
        BOOL,
        INT,
        UINT,
        SHORT,
        USHORT,
        ULONG,
        TIME, // same as ULONG (32 bits)
        ENUM,
        STRING,
        CMD // special for commands only
    };

    // Unit Of Measurement mapping - maps to DeviceValueUOM_s in emsdevice.cpp. Sequence is important!!
    // also used with HA as uom
    enum DeviceValueUOM : uint8_t {
        NONE = 0,   // 0
        DEGREES,    // 1
        DEGREES_R,  // 2
        PERCENT,    // 3
        LMIN,       // 4
        KWH,        // 5
        WH,         // 6
        HOURS,      // 7
        MINUTES,    // 8
        UA,         // 9
        BAR,        // 10
        KW,         // 11
        W,          // 12
        KB,         // 13
        SECONDS,    // 14
        DBM,        // 15
        FAHRENHEIT, // 16
        MV,         // 17
        SQM,        // 18 squaremeter
        M3,         // 19 cubic meter
        L           // 20
    };

    // TAG mapping - maps to DeviceValueTAG_s in emsdevice.cpp
    enum DeviceValueTAG : uint8_t {
        TAG_NONE = 0, // wild card
        TAG_HEARTBEAT,
        TAG_BOILER_DATA_WW,
        TAG_DEVICE_DATA,
        TAG_DEVICE_DATA_WW,
        TAG_HC1,
        TAG_HC2,
        TAG_HC3,
        TAG_HC4,
        TAG_HC5,
        TAG_HC6,
        TAG_HC7,
        TAG_HC8,
        TAG_WWC1,
        TAG_WWC2,
        TAG_WWC3,
        TAG_WWC4,
        TAG_WWC5,
        TAG_WWC6,
        TAG_WWC7,
        TAG_WWC8,
        TAG_WWC9,
        TAG_WWC10,
        TAG_AHS,
        TAG_HS1,
        TAG_HS2,
        TAG_HS3,
        TAG_HS4,
        TAG_HS5,
        TAG_HS6,
        TAG_HS7,
        TAG_HS8,
        TAG_HS9,
        TAG_HS10,
        TAG_HS11,
        TAG_HS12,
        TAG_HS13,
        TAG_HS14,
        TAG_HS15,
        TAG_HS16
    };

    // states of a device value
    enum DeviceValueState : uint8_t {
        // low nibble active state of the device value
        DV_DEFAULT           = 0,        // 0 - does not yet have a value
        DV_ACTIVE            = (1 << 0), // 1 - has a validated real value
        DV_HA_CONFIG_CREATED = (1 << 1), // 2 - set if the HA config topic has been created
        DV_HA_CLIMATE_NO_RT  = (1 << 2), // 4 - climate created without roomTemp

        // high nibble as mask for exclusions & special functions
        DV_WEB_EXCLUDE      = (1 << 4), // 16 - not shown on web
        DV_API_MQTT_EXCLUDE = (1 << 5), // 32 - not shown on mqtt, API
        DV_READONLY         = (1 << 6), // 64 - read only
        DV_FAVORITE         = (1 << 7)  // 128 - sort to front
    };

    uint8_t                             device_type;  // EMSdevice::DeviceType
    uint8_t                             tag;          // DeviceValueTAG::*
    void *                              value_p;      // pointer to variable of any type
    uint8_t                             type;         // DeviceValueType::*
    const __FlashStringHelper * const * options;      // options as a flash char array
    uint8_t                             options_size; // number of options in the char array, calculated
    const __FlashStringHelper *         short_name;   // used in MQTT
    const __FlashStringHelper *         full_name;    // used in Web and Console
    uint8_t                             uom;          // DeviceValueUOM::*
    uint8_t                             ha;           // DeviceValueHA::
    bool                                has_cmd;      // true if there is a Console/MQTT command which matches the short_name
    int16_t                             min;          // min range
    uint16_t                            max;          // max range
    uint8_t                             state;        // DeviceValueState::*

    DeviceValue(uint8_t                             device_type,
                uint8_t                             tag,
                void *                              value_p,
                uint8_t                             type,
                const __FlashStringHelper * const * options,
                uint8_t                             options_size,
                const __FlashStringHelper *         short_name,
                const __FlashStringHelper *         full_name,
                uint8_t                             uom,
                uint8_t                             ha,
                bool                                has_cmd,
                int16_t                             min,
                uint16_t                            max,
                uint8_t                             state)
        : device_type(device_type)
        , tag(tag)
        , value_p(value_p)
        , type(type)
        , options(options)
        , options_size(options_size)
        , short_name(short_name)
        , full_name(full_name)
        , uom(uom)
        , ha(ha)
        , has_cmd(has_cmd)
        , min(min)
        , max(max)
        , state(state) {
    }

    bool hasValue() const;
    bool get_min_max(int16_t & dv_set_min, int16_t & dv_set_max);

    // state flags
    void add_state(uint8_t s) {
        state |= s;
    }
    bool has_state(uint8_t s) const {
        return (state & s) == s;
    }
    void remove_state(uint8_t s) {
        state &= ~s;
    }
    uint8_t get_state() const {
        return state;
    }

    static const __FlashStringHelper *       DeviceValueUOM_s[];
    static const __FlashStringHelper * const DeviceValueTAG_s[];
    static const __FlashStringHelper * const DeviceValueTAG_mqtt[];
    static size_t                            tag_count; // # tags
};

}; // namespace emsesp

#endif
