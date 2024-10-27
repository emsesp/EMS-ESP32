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
    register_telegram_type(0x5BA, "HpPoolStatus", true, MAKE_PF_CB(process_HpPoolStatus));
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &poolTemp_,
                          DeviceValueType::INT16,
                          DeviceValueNumOp::DV_NUMOP_DIV10,
                          FL_(poolTemp),
                          DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &poolShuntStatus_, DeviceValueType::ENUM, FL_(enum_shunt), FL_(poolShuntStatus), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &poolShunt_, DeviceValueType::UINT8, FL_(poolShunt), DeviceValueUOM::PERCENT);
}

// Mixer MP100 for pools - 0x5BA
void Pool::process_HpPoolStatus(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, poolTemp_, 0);
    has_update(telegram, poolShunt_, 3); // 0-100% how much is the shunt open?
    telegram->read_value(poolShuntStatus__, 2);
    uint8_t pss = poolShunt_ == 100 ? 3 : (poolShunt_ == 0 ? 4 : poolShuntStatus__);
    has_update(poolShuntStatus_, pss);
}

} // namespace emsesp
