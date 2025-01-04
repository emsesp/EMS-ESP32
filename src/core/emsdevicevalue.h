
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

#ifndef EMSESP_EMSDEVICEVALUE_H_
#define EMSESP_EMSDEVICEVALUE_H_

#include <Arduino.h>
#include <ArduinoJson.h>

#include "helpers.h"          // for conversions
#include "default_settings.h" // for enum types

namespace emsesp {

// DeviceValue holds the information for a device entity
class DeviceValue {
  public:
    enum DeviceValueType : uint8_t {
        BOOL,
        INT8,
        UINT8,
        INT16,
        UINT16,
        UINT24,
        TIME, // same as ULONG (32 bits)
        UINT32,
        ENUM,
        STRING,
        CMD // special for commands only
    };

    // Unit Of Measurement mapping - maps to DeviceValueUOM_s in emsdevicevalue.cpp. Sequence is important!!
    // also used with HA as uom
    enum DeviceValueUOM : uint8_t {
        NONE = 0,    // 0
        DEGREES,     // 1 - °C
        DEGREES_R,   // 2 - °C (relative temperature)
        PERCENT,     // 3 - %
        LMIN,        // 4 - l/min
        KWH,         // 5 - kWh
        WH,          // 6 - Wh
        HOURS,       // 7 - h
        MINUTES,     // 8 - m
        UA,          // 9 - µA
        BAR,         // 10 - bar
        KW,          // 11 - kW
        W,           // 12 - W
        KB,          // 13 - kB
        SECONDS,     // 14 - s
        DBM,         // 15 - dBm
        FAHRENHEIT,  // 16 - °F
        MV,          // 17 - mV
        SQM,         // 18 - m²
        M3,          // 19 - m³
        L,           // 20 - L
        KMIN,        // 21 - K*min
        K,           // 22 - K
        VOLTS,       // 23 - V
        MBAR,        // 24 - mbar
        LH,          // 25 - l/h
        CTKWH,       // 26 - ct/kWh
        CONNECTIVITY // 27 - used in HA
    };

    // TAG mapping - maps to DeviceValueTAG_s in emsdevicevalue.cpp
    enum DeviceValueTAG : int8_t {
        TAG_NONE        = -1, // wild card
        TAG_DEVICE_DATA = 0,
        TAG_HC1,
        TAG_HC2,
        TAG_HC3,
        TAG_HC4,
        TAG_HC5,
        TAG_HC6,
        TAG_HC7,
        TAG_HC8,
        TAG_DHW1,
        TAG_DHW2,
        TAG_DHW3,
        TAG_DHW4,
        TAG_DHW5,
        TAG_DHW6,
        TAG_DHW7,
        TAG_DHW8,
        TAG_DHW9,
        TAG_DHW10,
        TAG_AHS1,
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
        DV_FAVORITE         = (1 << 7)  // 128 - marked as a favorite
    };

    // numeric operators
    // negative numbers used for multipliers
    enum DeviceValueNumOp : int8_t {
        DV_NUMOP_NONE   = 0, // default
        DV_NUMOP_DIV2   = 2,
        DV_NUMOP_DIV10  = 10,
        DV_NUMOP_DIV60  = 60,
        DV_NUMOP_DIV100 = 100,
        DV_NUMOP_MUL5   = -5,
        DV_NUMOP_MUL10  = -10,
        DV_NUMOP_MUL15  = -15,
        DV_NUMOP_MUL50  = -50
    };

    uint8_t               device_type;    // EMSdevice::DeviceType
    int8_t                tag;            // DeviceValueTAG::*
    void *                value_p;        // pointer to variable of any type
    uint8_t               type;           // DeviceValueType::*
    const char * const ** options;        // options as a flash char array
    const char * const *  options_single; // options are not translated
    int8_t                numeric_operator;
    const char * const    short_name;      // used in MQTT and API
    const char * const *  fullname;        // used in Web and Console, is translated
    std::string           custom_fullname; // optional, from customization
    uint8_t               uom;             // DeviceValueUOM::*
    bool                  has_cmd;         // true if there is a Console/MQTT command which matches the short_name
    int16_t               min;             // min range
    uint32_t              max;             // max range
    uint8_t               state;           // DeviceValueState::*

    uint8_t options_size; // number of options in the char array, calculated at class initialization

    DeviceValue(uint8_t               device_type,    // EMSdevice::DeviceType
                int8_t                tag,            // DeviceValueTAG::*
                void *                value_p,        // pointer to variable of any type
                uint8_t               type,           // DeviceValueType::*
                const char * const ** options,        // options as a flash char array
                const char * const *  options_single, // options are not translated
                int8_t                numeric_operator,
                const char * const    short_name,      // used in MQTT and API
                const char * const *  fullname,        // used in Web and Console, is translated
                std::string &         custom_fullname, // optional, from customization
                uint8_t               uom,             // DeviceValueUOM::*
                bool                  has_cmd,         // true if there is a Console/MQTT command which matches the short_name
                int16_t               min,             // min range
                uint32_t              max,             // max range
                uint8_t               state            // DeviceValueState::* (also known as the mask)
    );

    bool hasValue() const;
    bool has_tag() const;
    bool get_min_max(int16_t & dv_set_min, uint32_t & dv_set_max);

    void               set_custom_minmax();
    bool               get_custom_min(int16_t & val);
    bool               get_custom_max(uint32_t & val);
    std::string        get_custom_fullname() const;
    std::string        get_fullname() const;
    static std::string get_name(const std::string & entity);

    // dv state flags
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

    static const char *         DeviceValueUOM_s[];
    static const char * const * DeviceValueTAG_s[];
    static const char * const   DeviceValueTAG_mqtt[];
    static uint8_t              NUM_TAGS; // # tags
};

}; // namespace emsesp

#endif
