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

#include "alert.h"

namespace emsesp {

REGISTER_FACTORY(Alert, EMSdevice::DeviceType::ALERT);

Alert::Alert(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    register_telegram_type(0x937, "EM100TempMessage", false, MAKE_PF_CB(process_EM100TempMessage));
    register_telegram_type(0x93A, "EM100SetMessage", false, MAKE_PF_CB(process_EM100SetMessage));
    register_telegram_type(0x936, "EM100MonitorMessage", false, MAKE_PF_CB(process_EM100MonitorMessage));

    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &headerTemp_,
                          DeviceValueType::SHORT,
                          DeviceValueNumOp::DV_NUMOP_DIV10,
                          FL_(flowTempVf),
                          DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &dip_, DeviceValueType::UINT, FL_(mode), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &outPower_, DeviceValueType::UINT, FL_(power), DeviceValueUOM::PERCENT);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &errorState_, DeviceValueType::BOOL, FL_(error), DeviceValueUOM::NONE);
}


// alert(0x15) -B-> All(0x00), ?(0x093A), data: 00 00 00 00 00 00 00 00 00 03 01
void Alert::process_EM100SetMessage(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, dip_, 9);
}
// one of 0x936, 0x937, 0x938, 0x939, 0x93A
void Alert::process_EM100MonitorMessage(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, errorState_, 0); // OE1
    has_update(telegram, errorPump_, 0);  // IE0
    has_update(telegram, outPower_, 0);   // IO1 percent
    has_update(telegram, input_, 0);      //IO1
}
// alert(0x15) -B-> All(0x00), ?(0x0937), data: 80 00
void Alert::process_EM100TempMessage(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, headerTemp_, 0);
}

} // namespace emsesp