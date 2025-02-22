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

#include "ventilation.h"

namespace emsesp {

REGISTER_FACTORY(Ventilation, EMSdevice::DeviceType::VENTILATION);

Ventilation::Ventilation(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    // HRV176 module, device_id 0x51
    register_telegram_type(0x56B, "VentilationMode", true, MAKE_PF_CB(process_ModeMessage));
    register_telegram_type(0x585, "Blowerspeed", false, MAKE_PF_CB(process_BlowerMessage));
    register_telegram_type(0x583, "VentilationMonitor", false, MAKE_PF_CB(process_MonitorMessage));
    register_telegram_type(0x5D9, "Airquality", false, MAKE_PF_CB(process_VOCMessage));
    register_telegram_type(0x587, "Bypass", false, MAKE_PF_CB(process_BypassMessage));
    // register_telegram_type(0x5, "VentilationSet", true, MAKE_PF_CB(process_SetMessage));

    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &outFresh_,
                          DeviceValueType::INT16,
                          DeviceValueNumOp::DV_NUMOP_DIV10,
                          FL_(outFresh),
                          DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &inFresh_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(inFresh), DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &outEx_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(outEx), DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &inEx_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(inEx), DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &ventInSpeed_, DeviceValueType::UINT8, FL_(ventInSpeed), DeviceValueUOM::PERCENT);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &ventOutSpeed_, DeviceValueType::UINT8, FL_(ventOutSpeed), DeviceValueUOM::PERCENT);
    register_device_value(
        DeviceValueTAG::TAG_DEVICE_DATA, &mode_, DeviceValueType::ENUM, FL_(enum_ventMode), FL_(ventMode), DeviceValueUOM::NONE, MAKE_CF_CB(set_ventMode));
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &voc_, DeviceValueType::UINT16, FL_(airquality), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &humidity_, DeviceValueType::UINT8, FL_(airHumidity), DeviceValueUOM::PERCENT);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &bypass_, DeviceValueType::BOOL, FL_(airbypass), DeviceValueUOM::NONE, MAKE_CF_CB(set_bypass));
}

// message
void Ventilation::process_SetMessage(std::shared_ptr<const Telegram> telegram) {
}

// message 583
void Ventilation::process_MonitorMessage(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, outEx_, 0);     // Fortluft
    has_update(telegram, inEx_, 7);      // Abluft
    has_update(telegram, outFresh_, 13); // Außenluft
    has_update(telegram, inFresh_, 15);  // Zuluft
}

// message 575 10 bytes
// data: 02 02 46 46 00 00 FF 80 00 01
// 0-level out, 1-level in, 2-mod out, 3-mod in,  9-mode:1-manual/2-auto/3-prog

// message 585 26 bytes long
// Data: 46 46 00 00 00 77 00 03 F4 09 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
void Ventilation::process_BlowerMessage(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, ventOutSpeed_, 0);
    has_update(telegram, ventInSpeed_, 1);
}

// message 0x05D9, data: 03 9C FF
void Ventilation::process_VOCMessage(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, voc_, 0);
    has_update(telegram, humidity_, 2);
}

// message 0x56B
// level 0=0, 1=1, 2=2, 3=3, 4= 4, Auto 0xFF, demand 5, sleep 6, intense 7, bypass-8, party 9, fireplace 0A
void Ventilation::process_ModeMessage(std::shared_ptr<const Telegram> telegram) {
    has_enumupdate(telegram, mode_, 0, -1);
}

// message 0x0587, data: 01 00
void Ventilation::process_BypassMessage(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, bypass_, 1);
}

bool Ventilation::set_ventMode(const char * value, const int8_t id) {
    uint8_t v;
    if (!Helpers::value2enum(value, v, FL_(enum_ventMode))) {
        return false;
    }
    write_command(0x56B, 0, v - 1, 0x56B);
    return true;
}

bool Ventilation::set_filter(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }
    // write_command(0x5xx, 0, v, 0x5xx);
    return true;
}

bool Ventilation::set_bypass(const char * value, const int8_t id) {
    bool b;
    if (!Helpers::value2bool(value, b)) {
        return false;
    }
    write_command(0x55C, 1, b ? 1 : 0, 0x587);
    return true;
}

} // namespace emsesp
