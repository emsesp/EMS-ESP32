/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2026  emsesp.org
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
    // HRV176/HRV156 module, device_id 0x51
    register_telegram_type(0x56B, "VentilationMode", true, MAKE_PF_CB(process_ModeMessage), 1);
    register_telegram_type(0x589, "VentilationAutoLevel", true, MAKE_PF_CB(process_AutoLevelMessage), 2);
    register_telegram_type(0x575, "VentilationBlower", false, MAKE_PF_CB(process_BlowerMessage));
    register_telegram_type(0x583, "VentilationMonitor", false, MAKE_PF_CB(process_MonitorMessage));
    register_telegram_type(0x5D9, "Airquality", false, MAKE_PF_CB(process_VOCMessage));
    register_telegram_type(0x587, "Bypass", false, MAKE_PF_CB(process_BypassMessage));
    register_telegram_type(0x55C, "VentilationSet", true, MAKE_PF_CB(process_SetMessage), 2);
    register_telegram_type(0x556, "VentilationBalance", true, MAKE_PF_CB(process_BalanceMessage));
    register_telegram_type(0x561, "VentilationLevel", true, MAKE_PF_CB(process_LevelMessage));
    register_telegram_type(0x557, "VentilationAirflow", true, MAKE_PF_CB(process_AirflowMessage), 3);

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
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &ventOutRpm_, DeviceValueType::UINT16, FL_(ventOutRpm), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &ventInRpm_, DeviceValueType::UINT16, FL_(ventInRpm), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &ventOutVoltage_,
                          DeviceValueType::UINT8,
                          DeviceValueNumOp::DV_NUMOP_DIV10,
                          FL_(ventOutVoltage),
                          DeviceValueUOM::VOLTS);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &ventInVoltage_,
                          DeviceValueType::UINT8,
                          DeviceValueNumOp::DV_NUMOP_DIV10,
                          FL_(ventInVoltage),
                          DeviceValueUOM::VOLTS);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &ventBalance_,
                          DeviceValueType::UINT8,
                          FL_(ventBalance),
                          DeviceValueUOM::PERCENT,
                          MAKE_CF_CB(set_ventBalance),
                          90,
                          110);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &level1Pct_,
                          DeviceValueType::UINT8,
                          FL_(level1Pct),
                          DeviceValueUOM::PERCENT,
                          MAKE_CF_CB(set_level1Pct),
                          0,
                          150);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &level2Pct_,
                          DeviceValueType::UINT8,
                          FL_(level2Pct),
                          DeviceValueUOM::PERCENT,
                          MAKE_CF_CB(set_level2Pct),
                          0,
                          150);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &level4Pct_,
                          DeviceValueType::UINT8,
                          FL_(level4Pct),
                          DeviceValueUOM::PERCENT,
                          MAKE_CF_CB(set_level4Pct),
                          0,
                          150);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &bypassMinOutdoorTemp_,
                          DeviceValueType::UINT8,
                          FL_(bypassMinOutdoorTemp),
                          DeviceValueUOM::DEGREES,
                          MAKE_CF_CB(set_bypassMinOutdoorTemp),
                          12,
                          19);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &bypassMaxSupplyTemp_,
                          DeviceValueType::UINT8,
                          FL_(bypassMaxSupplyTemp),
                          DeviceValueUOM::DEGREES,
                          MAKE_CF_CB(set_bypassMaxSupplyTemp),
                          21,
                          30);
    // min/max limits taken from the device's ValueConfig (0xF9) responses
    register_device_value(
        DeviceValueTAG::TAG_DEVICE_DATA, &airflowNominal_, DeviceValueType::UINT16, FL_(airflowNom), DeviceValueUOM::NONE, MAKE_CF_CB(set_airflowNominal), 75, 450);
}

// message 0x55C (04 5C on the bus), data: 08 01 11 17
// 1: bypass on/off, 2: bypass minimum outdoor temperature, 3: bypass maximum supply air temperature
void Ventilation::process_SetMessage(const std::shared_ptr<const Telegram> & telegram) {
    has_update(telegram, bypass_, 1);
    has_update(telegram, bypassMinOutdoorTemp_, 2);
    has_update(telegram, bypassMaxSupplyTemp_, 3);
}

// message 0x0557, nominal airflow in m³/h as 16-bit value at offset 1, e.g. data: 00 DC (offset 1) = 220
void Ventilation::process_AirflowMessage(const std::shared_ptr<const Telegram> & telegram) {
    has_update(telegram, airflowNominal_, 1);
}

// message 0x0583
void Ventilation::process_MonitorMessage(const std::shared_ptr<const Telegram> & telegram) {
    has_update(telegram, outEx_, 0);       // Fortluft
    has_update(telegram, ventOutRpm_, 2);  // Abluft rpm
    has_update(telegram, inEx_, 7);        // Abluft
    has_update(telegram, outFresh_, 13);   // Außenluft
    has_update(telegram, inFresh_, 15);    // Zuluft
    has_update(telegram, ventInRpm_, 17);  // Zuluft rpm
}

// message 0x575 10 bytes
// data: 02 02 46 46 00 00 FF 80 00 01
// 0-level out, 1-level in, 2-mod out, 3-mod in, 9-mode:1-manual/2-auto/3-prog
// in demand mode the modulation is set freely (e.g. 43%), it does not follow the fixed L1-L4 percentages
void Ventilation::process_BlowerMessage(const std::shared_ptr<const Telegram> & telegram) {
    has_update(telegram, ventOutSpeed_, 2);
    has_update(telegram, ventInSpeed_, 3);
}

// message 0x05D9, data: 03 9C FF
void Ventilation::process_VOCMessage(const std::shared_ptr<const Telegram> & telegram) {
    has_update(telegram, voc_, 0);
    has_update(telegram, humidity_, 2);
}

// message 0x56B
// level 0=0, 1=1, 2=2, 3=3, 4= 4, Auto 0xFF, demand 5, sleep 6, intense 7, bypass-8, party 9, fireplace 0A
void Ventilation::process_ModeMessage(const std::shared_ptr<const Telegram> & telegram) {
    telegram->read_enumvalue(rawMode_, 0, -1); // 0xFF (auto) -> 0
    update_mode();
}

// message 0x0589, data: 01 FF
// auto level: 00=L0, 01=L1, 02=L2, 03=L3, 04=L4, 05=demand (second byte always FF)
void Ventilation::process_AutoLevelMessage(const std::shared_ptr<const Telegram> & telegram) {
    telegram->read_value(autoLevel_, 0);
    update_mode();
}

// combine the mode (0x56B) and the auto level (0x589) into the single ventmode value:
// in auto mode show autoL0..autodemand, otherwise the plain mode
void Ventilation::update_mode() {
    if (rawMode_ == EMS_VALUE_UINT8_NOTSET) {
        return;
    }

    uint8_t m;
    if (rawMode_ == 0) { // auto
        m = (autoLevel_ <= 5) ? (VENTMODE_AUTOL0 + autoLevel_) : 0; // plain auto until the level is known
    } else {
        m = rawMode_;
    }

    has_update(mode_, m);
}

// message 0x0587, data: 00 00 64 00 64 0A 00 01 54 01 00 01 00 00 00 46 00 00 00 02 00 A3 00 A3
// offset 2/4: exhaust/supply fan control voltage (0.1 V), confirmed with multimeter
void Ventilation::process_BypassMessage(const std::shared_ptr<const Telegram> & telegram) {
    has_update(telegram, ventOutVoltage_, 2);
    has_update(telegram, ventInVoltage_, 4);
}

// message 0x0556 — volume flow balance (do not write offset 7: forces L3)
void Ventilation::process_BalanceMessage(const std::shared_ptr<const Telegram> & telegram) {
    has_update(telegram, ventBalance_, 8);
}

// message 0x0561 — level percentages (L3 is fixed at 100 %)
void Ventilation::process_LevelMessage(const std::shared_ptr<const Telegram> & telegram) {
    has_update(telegram, level4Pct_, 13);
    has_update(telegram, level1Pct_, 14);
    has_update(telegram, level2Pct_, 15);
}

bool Ventilation::set_ventMode(const char * value, const int8_t id) {
    uint8_t v;
    if (!Helpers::value2enum(value, v, FL_(enum_ventMode))) {
        return false;
    }

    // autoL0..autodemand: select the auto level via 0x0589, switching to auto mode first if needed
    if (v >= VENTMODE_AUTOL0) {
        if (rawMode_ != 0) {
            write_command(0x56B, 0, 0xFF, 0x56B);
        }
        uint8_t data[2] = {static_cast<uint8_t>(v - VENTMODE_AUTOL0), 0xFF};
        write_command(0x589, 0, data, 2, 0x589);
        return true;
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

bool Ventilation::set_ventBalance(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }

    write_command(0x556, 8, v, 0x556);
    return true;
}

bool Ventilation::set_level1Pct(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }

    write_command(0x561, 14, v, 0x561);
    return true;
}

bool Ventilation::set_level2Pct(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }

    write_command(0x561, 15, v, 0x561);
    return true;
}

bool Ventilation::set_level4Pct(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }

    write_command(0x561, 13, v, 0x561);
    return true;
}


// sets the minimum outdoor temperature for bypass at offset 2 of 0x55C
bool Ventilation::set_bypassMinOutdoorTemp(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }

    write_command(0x55C, 2, static_cast<uint8_t>(v), 0x55C);
    return true;
}

// sets the maximum supply air temperature for bypass at offset 3 of 0x55C
bool Ventilation::set_bypassMaxSupplyTemp(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }

    write_command(0x55C, 3, static_cast<uint8_t>(v), 0x55C);
    return true;
}

// sets the nominal airflow in m³/h, 16-bit value at offset 1 of 0x0557
bool Ventilation::set_airflowNominal(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }

    uint8_t data[2] = {static_cast<uint8_t>(v >> 8), static_cast<uint8_t>(v & 0xFF)};
    write_command(0x557, 1, data, 2, 0x557);
    return true;
}


} // namespace emsesp
