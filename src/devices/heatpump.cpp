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

#include "heatpump.h"

namespace emsesp {

REGISTER_FACTORY(Heatpump, EMSdevice::DeviceType::HEATPUMP);

Heatpump::Heatpump(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const std::string & name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    // telegram handlers
    register_telegram_type(0x042B, F("HP1"), true, MAKE_PF_CB(process_HPMonitor1));
    register_telegram_type(0x047B, F("HP2"), true, MAKE_PF_CB(process_HPMonitor2));

    // device values
    register_device_value(DeviceValueTAG::TAG_NONE, &airHumidity_, DeviceValueType::UINT, FL_(div2), FL_(airHumidity), DeviceValueUOM::PERCENT);
    register_device_value(DeviceValueTAG::TAG_NONE, &dewTemperature_, DeviceValueType::UINT, nullptr, FL_(dewTemperature), DeviceValueUOM::DEGREES);
}

/*
 * Type 0x47B - HeatPump Monitor 2
 * e.g. "38 10 FF 00 03 7B 08 24 00 4B"
 */
void Heatpump::process_HPMonitor2(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, dewTemperature_, 0);
    has_update(telegram, airHumidity_, 1);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/*
 * Type 0x42B- HeatPump Monitor 1
 * e.g. "38 10 FF 00 03 2B 00 D1 08 2A 01"
 */
void Heatpump::process_HPMonitor1(std::shared_ptr<const Telegram> telegram) {
    // still to implement
}

#pragma GCC diagnostic pop

} // namespace emsesp
