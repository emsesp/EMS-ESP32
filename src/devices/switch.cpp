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

#include "switch.h"

namespace emsesp {

REGISTER_FACTORY(Switch, EMSdevice::DeviceType::SWITCH);

Switch::Switch(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    // WM10 module, device_id 0x11
    register_telegram_type(0x9C, "WM10MonitorMessage", false, MAKE_PF_CB(process_WM10MonitorMessage));
    register_telegram_type(0x9D, "WM10SetMessage", false, MAKE_PF_CB(process_WM10SetMessage));
    register_telegram_type(0x1E, "WM10TempMessage", false, MAKE_PF_CB(process_WM10TempMessage));

    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &activated_, DeviceValueType::BOOL, FL_(activated), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &flowTempHc_,
                          DeviceValueType::UINT16,
                          DeviceValueNumOp::DV_NUMOP_DIV10,
                          FL_(flowTempHc),
                          DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &status_, DeviceValueType::INT8, FL_(status), DeviceValueUOM::NONE);
}

// message 0x9D switch on/off
// Thermostat(0x10) -> Switch(0x11), ?(0x9D), data: 00
void Switch::process_WM10SetMessage(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, activated_, 0);
}

// message 0x9C holds flowtemp and unknown status value
// Switch(0x11) -> All(0x00), ?(0x9C), data: 01 BA 00 01 00
void Switch::process_WM10MonitorMessage(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, flowTempHc_, 0); // is * 10
    has_update(telegram, status_, 2);
    // has_update(telegram, status2_, 3)); // unknown
}

// message 0x1E flow temperature, same as in 9C, published often, republished also by boiler UBAFast 0x18
// Switch(0x11) -> Boiler(0x08), ?(0x1E), data: 01 BA
void Switch::process_WM10TempMessage(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, flowTempHc_, 0); // is * 10
}

} // namespace emsesp
