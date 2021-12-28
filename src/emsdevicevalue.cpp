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

namespace emsesp {

// checks whether the device value has an actual value
// returns true if its valid
// state is stored in the dv object
bool DeviceValue::hasvalue() {
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
        has_value = true;
        break;
    default:
        break;
    }

#if defined(EMSESP_DEBUG)
    // https://github.com/emsesp/EMS-ESP32/issues/196
    // if (has_state(DeviceValueState::DV_ACTIVE) && !has_value) {
    //     emsesp::EMSESP::logger().warning(F("[DEBUG] Lost device value %s"), short_name);
    // }
#endif

    return has_value;
}

} // namespace emsesp
