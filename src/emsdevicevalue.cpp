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

#include "emsdevicevalue.h"

#include "emsesp.h"

namespace emsesp {

// constructor
DeviceValue::DeviceValue(uint8_t                              device_type,
                         uint8_t                              tag,
                         void *                               value_p,
                         uint8_t                              type,
                         const __FlashStringHelper * const ** options,
                         const __FlashStringHelper * const *  options_single,
                         int8_t                               numeric_operator,
                         const __FlashStringHelper * const    short_name,
                         const __FlashStringHelper * const *  fullname,
                         uint8_t                              uom,
                         bool                                 has_cmd,
                         int16_t                              min,
                         uint16_t                             max,
                         uint8_t                              state)
    : device_type(device_type)
    , tag(tag)
    , value_p(value_p)
    , type(type)
    , options(options)
    , options_single(options_single)
    , numeric_operator(numeric_operator)
    , short_name(short_name)
    , fullname(fullname)
    , uom(uom)
    , has_cmd(has_cmd)
    , min(min)
    , max(max)
    , state(state) {
    // calculate #options in options list
    if (options_single) {
        options_size = 1;
    } else {
        options_size = Helpers::count_items(options);
    }

#ifdef EMSESP_STANDALONE
    // only added for debugging
    Serial.print("registering entity: ");
    Serial.print(read_flash_string(short_name).c_str());
    Serial.print("/");
    auto trans_fullname = Helpers::translated_word(fullname);
    Serial.print(trans_fullname.c_str());
    Serial.print(" (#options=");
    Serial.print(options_size);
    Serial.print(",numop=");
    Serial.print(numeric_operator);
    Serial.print(") ");
    if (options != nullptr) {
        uint8_t i = 0;
        while (i < options_size) {
            Serial.print(" option");
            Serial.print(i + 1);
            Serial.print(":");
            auto str = Helpers::translated_fword(options[i]);
            Serial.print(read_flash_string(str).c_str());
            i++;
        }
    } else if (options_single != nullptr) {
        Serial.print("option1:!");
        Serial.print(read_flash_string(options_single[0]).c_str());
        Serial.print("!");
    }
    Serial.println("");
#endif
}

// mapping of UOM, to match order in DeviceValueUOM enum emsdevice.h
// also maps to DeviceValueUOM in interface/src/project/types.ts for the Web UI
// must be an int of 4 bytes, 32bit aligned
const __FlashStringHelper * DeviceValue::DeviceValueUOM_s[] __attribute__((__aligned__(sizeof(uint32_t)))) PROGMEM = {

    F_(uom_blank),
    F_(uom_degrees),
    F_(uom_degrees),
    F_(uom_percent),
    F_(uom_lmin),
    F_(uom_kwh),
    F_(uom_wh),
    F_(uom_hours),
    F_(uom_minutes),
    F_(uom_ua),
    F_(uom_bar),
    F_(uom_kw),
    F_(uom_w),
    F_(uom_kb),
    F_(uom_seconds),
    F_(uom_dbm),
    F_(uom_fahrenheit),
    F_(uom_mv),
    F_(uom_sqm)

};

// mapping of TAGs, to match order in DeviceValueTAG enum in emsdevice.h
// must be an int of 4 bytes, 32bit aligned
const __FlashStringHelper * const DeviceValue::DeviceValueTAG_s[] PROGMEM = {

    F_(tag_none),           // ""
    F_(tag_heartbeat),      // ""
    F_(tag_boiler_data_ww), // "dhw"
    F_(tag_device_data),    // ""
    F_(tag_device_data_ww), // "dhw"
    F_(tag_hc1),            // "hc1"
    F_(tag_hc2),            // "hc2"
    F_(tag_hc3),            // "hc3"
    F_(tag_hc4),            // "hc4"
    F_(tag_hc5),            // "hc5"
    F_(tag_hc6),            // "hc6"
    F_(tag_hc7),            // "hc7"
    F_(tag_hc8),            // "hc8"
    F_(tag_wwc1),           // "wwc1"
    F_(tag_wwc2),           // "Wwc2"
    F_(tag_wwc3),           // "wwc3"
    F_(tag_wwc4),           // "wwc4"
    F_(tag_wwc5),           // "wwc5"
    F_(tag_wwc6),           // "wwc6"
    F_(tag_wwc7),           // "wwc7"
    F_(tag_wwc8),           // "wwc8"
    F_(tag_wwc9),           // "wwc9"
    F_(tag_wwc10),          // "wwc10"
    F_(tag_ahs),            // "ahs"
    F_(tag_hs1),            // "hs1"
    F_(tag_hs2),            // "hs2"
    F_(tag_hs3),            // "hs3"
    F_(tag_hs4),            // "hs4"
    F_(tag_hs5),            // "hs5"
    F_(tag_hs6),            // "hs6"
    F_(tag_hs7),            // "hs7"
    F_(tag_hs8),            // "hs8"
    F_(tag_hs9),            // "hs9"
    F_(tag_hs10),           // "hs10"
    F_(tag_hs11),           // "hs11"
    F_(tag_hs12),           // "hs12"
    F_(tag_hs13),           // "hs13"
    F_(tag_hs14),           // "hs14"
    F_(tag_hs15),           // "hs15"
    F_(tag_hs16)            // "hs16"

};

// MQTT topics derived from tags
const __FlashStringHelper * const DeviceValue::DeviceValueTAG_mqtt[] PROGMEM = {

    F_(tag_none),                // ""
    F_(heartbeat),               // "heartbeat"
    F_(tag_boiler_data_ww_mqtt), // "ww"
    F_(tag_device_data),         // ""
    F_(tag_device_data_ww_mqtt), // ""
    F_(tag_hc1),                 // "hc1"
    F_(tag_hc2),                 // "hc2"
    F_(tag_hc3),                 // "hc3"
    F_(tag_hc4),                 // "hc4"
    F_(tag_hc5),                 // "hc5"
    F_(tag_hc6),                 // "hc6"
    F_(tag_hc7),                 // "hc7"
    F_(tag_hc8),                 // "hc8"
    F_(tag_wwc1),                // "wwc1"
    F_(tag_wwc2),                // "Wwc2"
    F_(tag_wwc3),                // "wwc3"
    F_(tag_wwc4),                // "wwc4"
    F_(tag_wwc5),                // "wwc5"
    F_(tag_wwc6),                // "wwc6"
    F_(tag_wwc7),                // "wwc7"
    F_(tag_wwc8),                // "wwc8"
    F_(tag_wwc9),                // "wwc9"
    F_(tag_wwc10),               // "wwc10"
    F_(tag_ahs),                 // "ahs"
    F_(tag_hs1),                 // "hs1"
    F_(tag_hs2),                 // "hs2"
    F_(tag_hs3),                 // "hs3"
    F_(tag_hs4),                 // "hs4"
    F_(tag_hs5),                 // "hs5"
    F_(tag_hs6),                 // "hs6"
    F_(tag_hs7),                 // "hs7"
    F_(tag_hs8),                 // "hs8"
    F_(tag_hs9),                 // "hs9"
    F_(tag_hs10),                // "hs10"
    F_(tag_hs11),                // "hs11"
    F_(tag_hs12),                // "hs12"
    F_(tag_hs13),                // "hs13"
    F_(tag_hs14),                // "hs14"
    F_(tag_hs15),                // "hs15"
    F_(tag_hs16)                 // "hs16"

};

// count #tags once at compile time
size_t DeviceValue::tag_count = sizeof(DeviceValue::DeviceValueTAG_s) / sizeof(__FlashStringHelper *);

// checks whether the device value has an actual value
// returns true if its valid
// state is stored in the dv object
bool DeviceValue::hasValue() const {
    bool has_value = false;
    switch (type) {
    case DeviceValueType::BOOL:
        has_value = Helpers::hasValue(*(uint8_t *)(value_p), EMS_VALUE_BOOL);
        break;
    case DeviceValueType::STRING:
        has_value = Helpers::hasValue((char *)(value_p));
        break;
    case DeviceValueType::ENUM:
        has_value = Helpers::hasValue(*(uint8_t *)(value_p));
        break;
    case DeviceValueType::INT:
        has_value = Helpers::hasValue(*(int8_t *)(value_p));
        break;
    case DeviceValueType::UINT:
        has_value = Helpers::hasValue(*(uint8_t *)(value_p));
        break;
    case DeviceValueType::SHORT:
        has_value = Helpers::hasValue(*(int16_t *)(value_p));
        break;
    case DeviceValueType::USHORT:
        has_value = Helpers::hasValue(*(uint16_t *)(value_p));
        break;
    case DeviceValueType::ULONG:
        has_value = Helpers::hasValue(*(uint32_t *)(value_p));
        break;
    case DeviceValueType::TIME:
        has_value = Helpers::hasValue(*(uint32_t *)(value_p));
        break;
    case DeviceValueType::CMD:
        has_value = false; // commands don't have values!
        break;
    default:
        break;
    }

    return has_value;
}

// set the min and max value for a device value
// converts to signed int, which means rounding to an whole integer
// returns false if there is no min/max needed
// Types BOOL, ENUM, STRING and CMD are not used
bool DeviceValue::get_min_max(int16_t & dv_set_min, int16_t & dv_set_max) {
    uint8_t fahrenheit = !EMSESP::system_.fahrenheit() ? 0 : (uom == DeviceValueUOM::DEGREES) ? 2 : (uom == DeviceValueUOM::DEGREES_R) ? 1 : 0;

    // if we have individual limits set already, just do the conversion
    // limits are not scaled with num operator and temperatures are °C
    if (min != 0 || max != 0) {
        dv_set_min = Helpers::transformNumFloat(min, 0, fahrenheit);
        dv_set_max = Helpers::transformNumFloat(max, 0, fahrenheit);
        return true;
    }

    // init default values to 0 and 0
    dv_set_min = 0;
    dv_set_max = 0;

    if (type == DeviceValueType::USHORT) {
        dv_set_min = Helpers::transformNumFloat(0, numeric_operator, fahrenheit);
        dv_set_max = Helpers::transformNumFloat(EMS_VALUE_USHORT_NOTSET, numeric_operator, fahrenheit);
        return true;
    }

    if (type == DeviceValueType::SHORT) {
        dv_set_min = Helpers::transformNumFloat(-EMS_VALUE_SHORT_NOTSET, numeric_operator, fahrenheit);
        dv_set_max = Helpers::transformNumFloat(EMS_VALUE_SHORT_NOTSET, numeric_operator, fahrenheit);
        return true;
    }

    if (type == DeviceValueType::UINT) {
        if (uom == DeviceValueUOM::PERCENT) {
            dv_set_max = 100;
        } else {
            dv_set_max = Helpers::transformNumFloat(EMS_VALUE_UINT_NOTSET, numeric_operator, fahrenheit);
        }
        return true;
    }

    if (type == DeviceValueType::INT) {
        if (uom == DeviceValueUOM::PERCENT) {
            dv_set_min = -100;
            dv_set_max = 100;
        } else {
            dv_set_min = Helpers::transformNumFloat(-EMS_VALUE_INT_NOTSET, numeric_operator, fahrenheit);
            dv_set_max = Helpers::transformNumFloat(EMS_VALUE_INT_NOTSET, numeric_operator, fahrenheit);
        }
        return true;
    }

    if (type == DeviceValueType::ULONG) {
        dv_set_max = Helpers::transformNumFloat(EMS_VALUE_ULONG_NOTSET, numeric_operator);
        return true;
    }

    if (type == DeviceValueType::TIME) {
        dv_set_max = Helpers::transformNumFloat(EMS_VALUE_ULONG_NOTSET, numeric_operator);
        return true;
    }

    return false; // nothing changed, not supported
}

} // namespace emsesp
