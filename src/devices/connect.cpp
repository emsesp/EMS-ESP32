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

#include "connect.h"

namespace emsesp {

REGISTER_FACTORY(Connect, EMSdevice::DeviceType::CONNECT);

Connect::Connect(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    if (device_id == 0x50) { // RF Base
        register_telegram_type(0xD1, "RFOutdoorTemp", false, MAKE_PF_CB(process_OutdoorTemp));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &outdoorTemp_,
                              DeviceValueType::INT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(outdoorTemp),
                              DeviceValueUOM::DEGREES);
    }
}
/*
 * OutdoorTemp - type 0xD1 - external temperature
 */
void Connect::process_OutdoorTemp(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, outdoorTemp_, 0);
}

/* other values from 0x50 RF base
(0x087F), data: 00 00
(0x0880), data: 01 04
(0x0889), data: 00 80 80 01
*/


} // namespace emsesp
