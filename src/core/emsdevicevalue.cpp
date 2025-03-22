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

#include "emsdevicevalue.h"

#include "emsesp.h"

namespace emsesp {

// constructor
DeviceValue::DeviceValue(uint8_t               device_type,
                         int8_t                tag,
                         void *                value_p,
                         uint8_t               type,
                         const char * const ** options,
                         const char * const *  options_single,
                         int8_t                numeric_operator,
                         const char * const    short_name,
                         const char * const *  fullname,
                         std::string &         custom_fullname,
                         uint8_t               uom,
                         bool                  has_cmd,
                         int16_t               min,
                         uint32_t              max,
                         uint8_t               state)
    : device_type(device_type)
    , tag(tag)
    , value_p(value_p)
    , type(type)
    , options(options)
    , options_single(options_single)
    , numeric_operator(numeric_operator)
    , short_name(short_name)
    , fullname(fullname)
    , custom_fullname(custom_fullname)
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

    // set the min/max
    set_custom_minmax();

    /*
#ifdef EMSESP_STANDALONE
    // only added for debugging
    Serial.print(COLOR_BRIGHT_RED_BACKGROUND);
    Serial.print(" registering entity: ");
    Serial.print((short_name));
    Serial.print("/");
    if (!custom_fullname.empty()) {
        Serial.print(COLOR_BRIGHT_CYAN);
        Serial.print(custom_fullname.c_str());
        Serial.print(COLOR_RESET);
    } else {
        Serial.print(Helpers::translated_word(fullname));
    }
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
            auto str = Helpers::translated_word(options[i]);
            Serial.print(str);
            i++;
        }
    } else if (options_single != nullptr) {
        Serial.print("option1:!");
        Serial.print((options_single[0]));
        Serial.print("!");
    }
    Serial.println(COLOR_RESET);
#endif
*/
}

// mapping of UOM, to match order in DeviceValueUOM enum emsdevicevalue.h
// also maps to DeviceValueUOM in interface/src/project/types.ts for the Web UI
// must be an int of 4 bytes, 32bit aligned
const char * DeviceValue::DeviceValueUOM_s[] = {

    F_(uom_blank), // 0
    F_(uom_degrees), F_(uom_degrees),    F_(uom_percent), F_(uom_lmin), F_(uom_kwh),   F_(uom_wh),   FL_(hours)[0],
    FL_(minutes)[0], F_(uom_ua),         F_(uom_bar),     F_(uom_kw),   F_(uom_w),     F_(uom_kb),   FL_(seconds)[0],
    F_(uom_dbm),     F_(uom_fahrenheit), F_(uom_mv),      F_(uom_sqm),  F_(uom_m3),    F_(uom_l),    F_(uom_kmin),
    F_(uom_k),       F_(uom_volts),      F_(uom_mbar),    F_(uom_lh),   F_(uom_ctkwh), F_(uom_blank)

};

// mapping of TAGs, to match order in DeviceValueTAG enum in emsdevicevalue.h
const char * const * DeviceValue::DeviceValueTAG_s[] = {

    FL_(tag_device_data), // ""
    FL_(tag_hc1),         // "hc1"
    FL_(tag_hc2),         // "hc2"
    FL_(tag_hc3),         // "hc3"
    FL_(tag_hc4),         // "hc4"
    FL_(tag_hc5),         // "hc5"
    FL_(tag_hc6),         // "hc6"
    FL_(tag_hc7),         // "hc7"
    FL_(tag_hc8),         // "hc8"
    FL_(tag_dhw1),        // "dhw"
    FL_(tag_dhw2),        // "dhw2"
    FL_(tag_dhw3),        // "dhw3"
    FL_(tag_dhw4),        // "dhw4"
    FL_(tag_dhw5),        // "dhw5"
    FL_(tag_dhw6),        // "dhw6"
    FL_(tag_dhw7),        // "dhw7"
    FL_(tag_dhw8),        // "dhw8"
    FL_(tag_dhw9),        // "dhw9"
    FL_(tag_dhw10),       // "dhw10"
    FL_(tag_ahs1),        // "ahs1"
    FL_(tag_hs1),         // "hs1"
    FL_(tag_hs2),         // "hs2"
    FL_(tag_hs3),         // "hs3"
    FL_(tag_hs4),         // "hs4"
    FL_(tag_hs5),         // "hs5"
    FL_(tag_hs6),         // "hs6"
    FL_(tag_hs7),         // "hs7"
    FL_(tag_hs8),         // "hs8"
    FL_(tag_hs9),         // "hs9"
    FL_(tag_hs10),        // "hs10"
    FL_(tag_hs11),        // "hs11"
    FL_(tag_hs12),        // "hs12"
    FL_(tag_hs13),        // "hs13"
    FL_(tag_hs14),        // "hs14"
    FL_(tag_hs15),        // "hs15"
    FL_(tag_hs16)         // "hs16"

};

// tags used in MQTT topic names. Matches sequence from DeviceValueTAG_s
const char * const DeviceValue::DeviceValueTAG_mqtt[] = {

    FL_(tag_device_data)[0], // ""
    FL_(tag_hc1)[0],         // "hc1"
    FL_(tag_hc2)[0],         // "hc2"
    FL_(tag_hc3)[0],         // "hc3"
    FL_(tag_hc4)[0],         // "hc4"
    FL_(tag_hc5)[0],         // "hc5"
    FL_(tag_hc6)[0],         // "hc6"
    FL_(tag_hc7)[0],         // "hc7"
    FL_(tag_hc8)[0],         // "hc8"
    FL_(tag_dhw1)[0],        // "dhw"
    FL_(tag_dhw2)[0],        // "dhw2"
    FL_(tag_dhw3)[0],        // "dhw3"
    FL_(tag_dhw4)[0],        // "dhw4"
    FL_(tag_dhw5)[0],        // "dhw5"
    FL_(tag_dhw6)[0],        // "dhw6"
    FL_(tag_dhw7)[0],        // "dhw7"
    FL_(tag_dhw8)[0],        // "dhw8"
    FL_(tag_dhw9)[0],        // "dhw9"
    FL_(tag_dhw10)[0],       // "dhw10"
    FL_(tag_ahs1)[0],        // "ahs1"
    FL_(tag_hs1)[0],         // "hs1"
    FL_(tag_hs2)[0],         // "hs2"
    FL_(tag_hs3)[0],         // "hs3"
    FL_(tag_hs4)[0],         // "hs4"
    FL_(tag_hs5)[0],         // "hs5"
    FL_(tag_hs6)[0],         // "hs6"
    FL_(tag_hs7)[0],         // "hs7"
    FL_(tag_hs8)[0],         // "hs8"
    FL_(tag_hs9)[0],         // "hs9"
    FL_(tag_hs10)[0],        // "hs10"
    FL_(tag_hs11)[0],        // "hs11"
    FL_(tag_hs12)[0],        // "hs12"
    FL_(tag_hs13)[0],        // "hs13"
    FL_(tag_hs14)[0],        // "hs14"
    FL_(tag_hs15)[0],        // "hs15"
    FL_(tag_hs16)[0]         // "hs16"

};

// count #tags once at compile time
uint8_t DeviceValue::NUM_TAGS = sizeof(DeviceValue::DeviceValueTAG_s) / sizeof(char * const *);

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
    case DeviceValueType::INT8:
        has_value = Helpers::hasValue(*(int8_t *)(value_p));
        break;
    case DeviceValueType::UINT8:
        has_value = Helpers::hasValue(*(uint8_t *)(value_p));
        break;
    case DeviceValueType::INT16:
        has_value = Helpers::hasValue(*(int16_t *)(value_p));
        break;
    case DeviceValueType::UINT16:
        has_value = Helpers::hasValue(*(uint16_t *)(value_p));
        break;
    case DeviceValueType::UINT24:
    case DeviceValueType::TIME:
    case DeviceValueType::UINT32:
        has_value = Helpers::hasValue(*(uint32_t *)(value_p));
        break;
    case DeviceValueType::CMD:
        has_value = true; // we count command as an actual entity
        break;
    default:
        break;
    }

    return has_value;
}

// See if the device value has a tag and it's not empty
bool DeviceValue::has_tag() const {
    return ((tag < DeviceValue::NUM_TAGS) && (tag != TAG_NONE) && strlen(DeviceValueTAG_s[tag][0]));
}

// set the min and max value for a device value
// converts to signed int, which means rounding to an whole integer
// returns false if there is no min/max needed
// Types BOOL, ENUM, STRING and CMD are not used
bool DeviceValue::get_min_max(int16_t & dv_set_min, uint32_t & dv_set_max) {
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

    if (type == DeviceValueType::UINT16) {
        dv_set_min = Helpers::transformNumFloat(0, numeric_operator, fahrenheit);
        dv_set_max = Helpers::transformNumFloat(EMS_VALUE_UINT16_NOTSET - 1, numeric_operator, fahrenheit);
        return true;
    }

    if (type == DeviceValueType::INT16) {
        dv_set_min = Helpers::transformNumFloat(-EMS_VALUE_INT16_NOTSET + 1, numeric_operator, fahrenheit);
        dv_set_max = Helpers::transformNumFloat(EMS_VALUE_INT16_NOTSET - 1, numeric_operator, fahrenheit);
        return true;
    }

    if (type == DeviceValueType::UINT8) {
        if (uom == DeviceValueUOM::PERCENT) {
            dv_set_max = 100;
        } else {
            dv_set_max = Helpers::transformNumFloat(EMS_VALUE_UINT8_NOTSET - 1, numeric_operator, fahrenheit);
        }
        return true;
    }

    if (type == DeviceValueType::INT8) {
        if (uom == DeviceValueUOM::PERCENT) {
            dv_set_min = -100;
            dv_set_max = 100;
        } else {
            dv_set_min = Helpers::transformNumFloat(-EMS_VALUE_INT8_NOTSET + 1, numeric_operator, fahrenheit);
            dv_set_max = Helpers::transformNumFloat(EMS_VALUE_INT8_NOTSET - 1, numeric_operator, fahrenheit);
        }
        return true;
    }

    if (type == DeviceValueType::UINT24 || type == DeviceValueType::TIME || type == DeviceValueType::UINT32) {
        dv_set_max = Helpers::transformNumFloat(EMS_VALUE_UINT24_NOTSET - 1, numeric_operator);
        return true;
    }

    return false; // nothing changed, not supported
}

// extract custom min from custom_fullname
bool DeviceValue::get_custom_min(int16_t & val) {
    auto    min_pos    = custom_fullname.find('>');
    bool    has_min    = (min_pos != std::string::npos);
    uint8_t fahrenheit = !EMSESP::system_.fahrenheit() ? 0 : (uom == DeviceValueUOM::DEGREES) ? 2 : (uom == DeviceValueUOM::DEGREES_R) ? 1 : 0;
    if (has_min) {
        int32_t v = Helpers::atoint(custom_fullname.substr(min_pos + 1).c_str());
        if (fahrenheit) {
            v = (v - (32 * (fahrenheit - 1))) / 1.8; // reset to °C
        }
        if (max > 0 && v > 0 && (uint32_t)v > max) {
            return false;
        }
        val = v;
    }
    return has_min;
}

// extract custom max from custom_fullname
bool DeviceValue::get_custom_max(uint32_t & val) {
    auto    max_pos    = custom_fullname.find('<');
    bool    has_max    = (max_pos != std::string::npos);
    uint8_t fahrenheit = !EMSESP::system_.fahrenheit() ? 0 : (uom == DeviceValueUOM::DEGREES) ? 2 : (uom == DeviceValueUOM::DEGREES_R) ? 1 : 0;
    if (has_max) {
        int32_t v = Helpers::atoint(custom_fullname.substr(max_pos + 1).c_str());
        if (fahrenheit) {
            v = (v - (32 * (fahrenheit - 1))) / 1.8; // reset to °C
        }
        if (v < 0 || v < (int32_t)min) {
            return false;
        }
        val = v;
    }
    return has_max;
}

// sets min max to stored custom values (if set)
void DeviceValue::set_custom_minmax() {
    get_custom_min(min);
    get_custom_max(max);
}

std::string DeviceValue::get_custom_fullname() const {
    auto min_pos    = custom_fullname.find('>');
    auto max_pos    = custom_fullname.find('<');
    auto minmax_pos = min_pos < max_pos ? min_pos : max_pos;
    if (minmax_pos != std::string::npos) {
        return custom_fullname.substr(0, minmax_pos);
    }
    return custom_fullname;
}

// returns the translated fullname or the custom fullname (if provided)
// always returns a std::string
std::string DeviceValue::get_fullname() const {
    std::string customname = get_custom_fullname();
    if (customname.empty()) {
        return Helpers::translated_word(fullname);
    }
    return customname;
}

// returns any custom name defined in the entity_id
std::string DeviceValue::get_name(const std::string & entity) {
    auto pos = entity.find('|');
    if (pos != std::string::npos) {
        return entity.substr(2, pos - 2);
    }
    return entity.substr(2);
}

} // namespace emsesp
