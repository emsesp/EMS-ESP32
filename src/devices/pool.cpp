/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2023  Paul Derbyshire
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

#include "pool.h"

namespace emsesp {

REGISTER_FACTORY(Pool, EMSdevice::DeviceType::POOL);

uuid::log::Logger Pool::logger_{F_(pool), uuid::log::Facility::CONSOLE};

Pool::Pool(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    // Pool module
    register_telegram_type(0x5BA, "HpPoolStatus", true, MAKE_PF_CB(process_HpPoolStatus), 4);
    register_telegram_type(0x5C4, "HpPoolShunt", false, MAKE_PF_CB(process_HpPoolShunt));
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &poolTemp_,
                          DeviceValueType::INT16,
                          DeviceValueNumOp::DV_NUMOP_DIV10,
                          FL_(poolTemp),
                          DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &poolShuntStatus_, DeviceValueType::ENUM, FL_(enum_shunt), FL_(poolShuntStatus), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &poolShunt_, DeviceValueType::UINT8, FL_(poolShunt), DeviceValueUOM::PERCENT);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &poolShuntSet_, DeviceValueType::UINT8, FL_(poolShuntSet), DeviceValueUOM::PERCENT);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &poolPump_, DeviceValueType::BOOL, FL_(poolPump), DeviceValueUOM::NONE);
}

// Mixer MP100 for pools - 0x5BA
// MP100(0x11) -> Boiler(0x08): 11 08 FF 00 04 BA 01 45 02 00 02
// offset 4 (pool circulation pump, 0 = running, 2 = off) is only in the telegram to the boiler,
// the answer to a read request is one byte shorter
void Pool::process_HpPoolStatus(const std::shared_ptr<const Telegram> & telegram) {
    uint8_t pump = EMS_VALUE_UINT8_NOTSET;
    if (telegram->read_value(pump, 4)) {
        has_update(poolPump_, pump == 0 ? EMS_VALUE_BOOL_ON : EMS_VALUE_BOOL_OFF);
    }
    has_update(telegram, poolTemp_, 0);
    has_update(telegram, poolShunt_, 3); // 0-100% how much is the shunt open?
    telegram->read_value(poolShuntStatus__, 2);
    uint8_t pss = poolShunt_ == 100 ? 3 : (poolShunt_ == 0 ? 4 : poolShuntStatus__);
    has_update(poolShuntStatus_, pss);
}

// Boiler(0x08) -> MP100(0x11), 0x5C4: 08 11 FF 00 04 C4 64
// shunt setpoint from the heatpump, 0% = pool / 100% = heat, poolshunt follows within ~2 min
void Pool::process_HpPoolShunt(const std::shared_ptr<const Telegram> & telegram) {
    has_update(telegram, poolShuntSet_, 0);
}

} // namespace emsesp
