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

#include "water.h"

namespace emsesp {

REGISTER_FACTORY(Water, EMSdevice::DeviceType::WATER);

uuid::log::Logger Water::logger_{F_(water), uuid::log::Facility::CONSOLE};

Water::Water(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    uint8_t tag = DeviceValueTAG::TAG_WWC1 + device_id - EMSdevice::EMS_DEVICE_ID_DHW1;
    if (device_id == 0x2A) { // SM100, DHW3
        wwc_ = 2;
        // telegram handlers
        register_telegram_type(0x07D6, "SM100wwTemperature", false, MAKE_PF_CB(process_SM100wwTemperature));
        register_telegram_type(0x07AA, "SM100wwStatus", false, MAKE_PF_CB(process_SM100wwStatus));
        register_telegram_type(0x07AB, "SM100wwCommand", false, MAKE_PF_CB(process_SM100wwCommand));
        register_telegram_type(0x07A5, "SM100wwCirc", true, MAKE_PF_CB(process_SM100wwCirc));
        register_telegram_type(0x07A6, "SM100wwParam", true, MAKE_PF_CB(process_SM100wwParam));
        register_telegram_type(0x07AE, "SM100wwKeepWarm", true, MAKE_PF_CB(process_SM100wwKeepWarm));
        register_telegram_type(0x07E0, "SM100wwStatus2", true, MAKE_PF_CB(process_SM100wwStatus2));
        // device values...
        register_device_value(tag, &wwTemp_1_, DeviceValueType::USHORT, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(wwTemp1), DeviceValueUOM::DEGREES);
        // register_device_value(tag, &wwTemp_2_, DeviceValueType::USHORT, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(wwTemp2), DeviceValueUOM::DEGREES);
        register_device_value(tag, &wwTemp_3_, DeviceValueType::USHORT, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(wwTemp3), DeviceValueUOM::DEGREES);
        register_device_value(tag, &wwTemp_4_, DeviceValueType::USHORT, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(wwTemp4), DeviceValueUOM::DEGREES);
        register_device_value(tag, &wwTemp_5_, DeviceValueType::USHORT, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(wwTemp5), DeviceValueUOM::DEGREES);
        // register_device_value(tag, &wwTemp_6_, DeviceValueType::USHORT, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(wwTemp6), DeviceValueUOM::DEGREES);
        register_device_value(tag, &wwTemp_7_, DeviceValueType::USHORT, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(wwTemp7), DeviceValueUOM::DEGREES);
        register_device_value(tag, &wwPump_, DeviceValueType::BOOL, FL_(wwPump), DeviceValueUOM::NONE);
        register_device_value(tag, &wwMaxTemp_, DeviceValueType::UINT, FL_(wwMaxTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_wwMaxTemp));
        register_device_value(tag, &wwSelTemp_, DeviceValueType::UINT, FL_(wwSelTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_wwSelTemp));
        register_device_value(tag, &wwRedTemp_, DeviceValueType::UINT, FL_(wwRedTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_wwRedTemp));
        register_device_value(tag, &wwDailyTemp_, DeviceValueType::UINT, FL_(wwDailyTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_wwDailyTemp));
        register_device_value(tag, &wwDisinfectionTemp_, DeviceValueType::UINT, FL_(wwDisinfectionTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_wwDisinfectionTemp));
        register_device_value(tag, &wwCirc_, DeviceValueType::BOOL, FL_(wwCirc), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwCirc));
        register_device_value(tag, &wwCircMode_, DeviceValueType::ENUM, FL_(enum_freq), FL_(wwCircMode), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwCircMode));
        register_device_value(tag, &wwKeepWarm_, DeviceValueType::BOOL, FL_(wwKeepWarm), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwKeepWarm));
        register_device_value(tag, &wwStatus2_, DeviceValueType::ENUM, FL_(enum_wwStatus2), FL_(wwStatus2), DeviceValueUOM::NONE);
        register_device_value(tag, &wwPumpMod_, DeviceValueType::UINT, FL_(wwPumpMod), DeviceValueUOM::PERCENT);
        register_device_value(tag, &wwFlow_, DeviceValueType::UINT, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(wwFlow), DeviceValueUOM::LMIN);

    } else if (device_id >= EMSdevice::EMS_DEVICE_ID_DHW1 && device_id <= EMSdevice::EMS_DEVICE_ID_DHW2) {
        wwc_ = device_id - EMSdevice::EMS_DEVICE_ID_DHW1;
        register_telegram_type(0x331 + wwc_, "MMPLUSStatusMessage_WWC", false, MAKE_PF_CB(process_MMPLUSStatusMessage_WWC));
        register_telegram_type(0x313 + wwc_, "MMPLUSConfigMessage_WWC", true, MAKE_PF_CB(process_MMPLUSConfigMessage_WWC));
        // register_telegram_type(0x33B + type_offset, "MMPLUSSetMessage_WWC", true, MAKE_PF_CB(process_MMPLUSSetMessage_WWC));
        // device values...
        register_device_value(tag, &wwFlowTemp_, DeviceValueType::USHORT, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(wwTemp), DeviceValueUOM::DEGREES);
        register_device_value(tag, &wwStatus_, DeviceValueType::INT, FL_(wwTempStatus), DeviceValueUOM::NONE);
        register_device_value(tag, &wwPump_, DeviceValueType::BOOL, FL_(wwPump), DeviceValueUOM::NONE);
        register_device_value(tag, &wwMaxTemp_, DeviceValueType::UINT, FL_(wwMaxTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_wwMaxTemp));
        register_device_value(tag, &wwDiffTemp_, DeviceValueType::INT, FL_(wwDiffTemp), DeviceValueUOM::DEGREES_R, MAKE_CF_CB(set_wwDiffTemp));
        register_device_value(tag, &wwDisinfectionTemp_, DeviceValueType::UINT, FL_(wwDisinfectionTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_wwDisinfectionTemp));
        register_device_value(tag, &wwRedTemp_, DeviceValueType::UINT, FL_(wwRedTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_wwRedTemp));
        register_device_value(tag, &wwRequiredTemp_, DeviceValueType::UINT, FL_(wwRequiredTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_wwRequiredTemp));
        register_device_value(tag, &wwCirc_, DeviceValueType::BOOL, FL_(wwCirc), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwCirc));
        register_device_value(tag, &wwCircMode_, DeviceValueType::ENUM, FL_(enum_wwCircMode), FL_(wwCircMode), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwCircMode));
    } else if (device_id == 0x40) { // flags == EMSdevice::EMS_DEVICE_FLAG_IPM, special DHW pos 10
        wwc_ = 0;
        tag  = DeviceValueTAG::TAG_WWC1;
        register_telegram_type(0x34, "UBAMonitorWW", false, MAKE_PF_CB(process_IPMMonitorWW));
        register_telegram_type(0x1E, "HydrTemp", false, MAKE_PF_CB(process_IPMHydrTemp));
        register_telegram_type(0x33, "UBAParameterWW", true, MAKE_PF_CB(process_IPMParameterWW));
        // register_telegram_type(0x10D, "wwNTCStatus", false, MAKE_PF_CB(process_wwNTCStatus));
        // device values...
        register_device_value(tag, &wwSelTemp_, DeviceValueType::UINT, FL_(wwSelTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_wwSelTemp));
        register_device_value(tag, &wwTemp_1_, DeviceValueType::USHORT, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(wwTemp1), DeviceValueUOM::DEGREES);
        register_device_value(tag, &wwTemp_2_, DeviceValueType::USHORT, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(wwTemp2), DeviceValueUOM::DEGREES);
        register_device_value(tag, &HydrTemp_, DeviceValueType::USHORT, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(hydrTemp), DeviceValueUOM::DEGREES);
        register_device_value(tag, &wwPump_, DeviceValueType::BOOL, FL_(wwPump), DeviceValueUOM::NONE);
        register_device_value(tag, &wwFlowTempOffset_, DeviceValueType::UINT, FL_(wwFlowTempOffset), DeviceValueUOM::DEGREES_R, MAKE_CF_CB(set_wwFlowTempOffset));
        register_device_value(tag, &wwHystOn_, DeviceValueType::INT, FL_(wwHystOn), DeviceValueUOM::DEGREES_R, MAKE_CF_CB(set_wwHystOn));
        register_device_value(tag, &wwHystOff_, DeviceValueType::INT, FL_(wwHystOff), DeviceValueUOM::DEGREES_R, MAKE_CF_CB(set_wwHystOff));
        register_device_value(tag, &wwDisinfectionTemp_, DeviceValueType::UINT, FL_(wwDisinfectionTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_wwDisinfectionTemp));
        register_device_value(tag, &wwCirc_, DeviceValueType::BOOL, FL_(wwCirc), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwCirc));
        register_device_value(tag, &wwCircMode_, DeviceValueType::ENUM, FL_(enum_wwCircMode), FL_(wwCircMode), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwCircMode));
    }
}

// SM100wwTemperature - 0x07D6
// Solar Module(0x2A) -> (0x00), (0x7D6), data: 01 C1 00 00 02 5B 01 AF 01 AD 80 00 01 90
void Water::process_SM100wwTemperature(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, wwTemp_1_, 0);  // is *10
    has_update(telegram, wwTemp_2_, 2);  // is *10 always zero
    has_update(telegram, wwTemp_3_, 4);  // is *10 TS21
    has_update(telegram, wwTemp_4_, 6);  // is *10 cold water
    has_update(telegram, wwTemp_5_, 8);  // is *10
    has_update(telegram, wwTemp_6_, 10); // is *10 always unset 8000
    has_update(telegram, wwTemp_7_, 12); // is *10
    has_update(telegram, wwTemp_8_, 14); // is *10, return temp TS22
}

// SM100wwStatus - 0x07AA
// Solar Module(0x2A) -> (0x00), (0x7AA), data: 64 00 04 00 03 00 28 01 0F
void Water::process_SM100wwStatus(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, wwPump_, 0);
}

// SM100wwParam - 0x07A6, Solar Module(0x2A) -> (0x00)
// data: FF 05 0F 5F 00 01 3C 3C 3C 3C 28 12 46 01 3C 1E 03 07 3C 00 0F 00 05
void Water::process_SM100wwParam(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, wwMaxTemp_, 8);
    has_update(telegram, wwSelTemp_, 9);
    has_update(telegram, wwRedTemp_, 10);
    has_update(telegram, wwDailyTemp_, 6);
    has_update(telegram, wwDisinfectionTemp_, 12);
    // (daily heating time thermostat 2F5, offset 9, offset 8 on/off)
}

// SM100wwCirc - 0x07A5
// Solar Module(0x2A) -> (0x00), (0x7A5), data:
void Water::process_SM100wwCirc(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, wwCirc_, 0);
    has_update(telegram, wwCircMode_, 3);
}

// SM100wwKeepWarm - 0x7AE, keepWarm
// Thermostat(0x10) -> Solar(0x2A), ?(0x7AE), data: FF
void Water::process_SM100wwKeepWarm(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, wwKeepWarm_, 0);
}

/*
// SM100ww? - 0x7E0, some kind of status
// data: 00 00 46 00 00 01 06 0E 06 0E 00 00 00 00 00 03 03 03 03
// publishes single values offset 1/2(16bit), offset 5, offset 6, offset 7, offset 8, offset 9,
// status2 = 03:"no heat", 06:"heat request", 08:"disinfecting", 09:"hold"
*/
void Water::process_SM100wwStatus2(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, wwFlow_, 7);
    has_update(telegram, wwStatus2_, 8);
    has_update(telegram, wwPumpMod_, 9);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
// SM100wwCommand - 0x07AB
// Thermostat(0x10) -> Solar Module(0x2A), (0x7AB), data: 01 00 01
void Water::process_SM100wwCommand(std::shared_ptr<const Telegram> telegram) {
    // not implemented yet
}
#pragma GCC diagnostic pop

/*
 * MM100 messages
 */

// Mixer warm water loading/DHW - 0x0331, 0x0332
// e.g. A9 00 FF 00 02 32 02 6C 00 3C 00 3C 3C 46 02 03 03 00 3C // on 0x28
//      A8 00 FF 00 02 31 02 35 00 3C 00 3C 3C 46 02 03 03 00 3C // in 0x29
void Water::process_MMPLUSStatusMessage_WWC(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, wwFlowTemp_, 0); // is * 10
    has_bitupdate(telegram, wwPump_, 2, 0);
    has_update(telegram, wwStatus_, 11); // temp status
}

// Config message 0x313, has to be fetched
void Water::process_MMPLUSConfigMessage_WWC(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, wwRequiredTemp_, 4);
    has_update(telegram, wwRedTemp_, 5);
    has_update(telegram, wwDiffTemp_, 7);
    has_update(telegram, wwDisinfectionTemp_, 9);
    has_update(telegram, wwMaxTemp_, 10);
}

// unknown, 2 examples from older threads
// Thermostat(0x10) -> Mixer(0x28), ?(0x33B), data: 01 01 00
// Thermostat -> Mixing Module, type 0x023B, telegram: 90 28 FF 00 02 3B 00 02 00 (CRC=68)
void Water::process_MMPLUSSetMessage_WWC(std::shared_ptr<const Telegram> telegram) {
}

// MMPLUS telegram 0x345 unknown
// Solar Module -> Mixing Module, type 0x0245, telegram: B0 28 FF 00 02 45 64 01 01 (CRC=36)
// ?

/*
 * IPM messages
 */

// 0x34 only 8 bytes long
// Mixer(0x41) -> All(0x00), UBAMonitorWW(0x34), data: 37 02 1E 02 1E 00 00 00 00
void Water::process_IPMMonitorWW(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, wwSelTemp_, 0);
    has_update(telegram, wwTemp_1_, 1);
    has_update(telegram, wwTemp_2_, 3);
    has_bitupdate(telegram, wwPump_, 5, 3);
}

// Mixer(0x41) -> Me(0x0B), UBAParameterWW(0x33), data: 08 FF 46 FB FF 28 FF 07 46 00 FF 00
void Water::process_IPMParameterWW(std::shared_ptr<const Telegram> telegram) {
    // has_update(telegram, wwActivated_, 1); // 0xFF means on
    // has_update(telegram, wwSelTemp_, 2);
    has_update(telegram, wwHystOn_, 3);         // Hyst on (default -5)
    has_update(telegram, wwHystOff_, 4);        // Hyst off (default -1)
    has_update(telegram, wwFlowTempOffset_, 5); // default 40
    has_update(telegram, wwCirc_, 6);           // 0xFF means on
    has_update(telegram, wwCircMode_, 7);       // 1=1x3min 6=6x3min 7=continuous
    has_update(telegram, wwDisinfectionTemp_, 8);
    // has_bitupdate(telegram, wwChargeType_, 10, 0); // 0 = charge pump, 0xff = 3-way valve
}


// 0x1E, only16 bit temperature
// Mixer(0x41) -> Boiler(0x08), HydrTemp(0x1E), data: 01 D8
void Water::process_IPMHydrTemp(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, HydrTemp_, 0);
}

/*
 * Settings
 */

bool Water::set_wwSelTemp(const char * value, const int8_t id) {
    int temperature;
    if (!Helpers::value2temperature(value, temperature)) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_IPM) {
        write_command(0x35, 3, (uint8_t)temperature, 0x34);
    } else if (flags() == EMSdevice::EMS_DEVICE_FLAG_MMPLUS) {
        return false;
    } else { // SM100
        write_command(0x7A6, 9, (uint8_t)temperature, 0x7A6);
    }
    return true;
}

bool Water::set_wwMaxTemp(const char * value, const int8_t id) {
    int temperature;
    if (!Helpers::value2temperature(value, temperature)) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_IPM) {
        return false;
    } else if (flags() == EMSdevice::EMS_DEVICE_FLAG_MMPLUS) {
        write_command(0x313 + wwc_, 10, (uint8_t)temperature, 0x313 + wwc_);
    } else { // SM100
        write_command(0x7A6, 8, (uint8_t)temperature, 0x7A6);
    }
    return true;
}

bool Water::set_wwRedTemp(const char * value, const int8_t id) {
    int temperature;
    if (!Helpers::value2temperature(value, temperature)) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_IPM) {
        return false;
    } else if (flags() == EMSdevice::EMS_DEVICE_FLAG_MMPLUS) {
        write_command(0x313 + wwc_, 5, (uint8_t)temperature, 0x313 + wwc_);
    } else { // SM100
        write_command(0x7A6, 10, (uint8_t)temperature, 0x7A6);
    }
    return true;
}

bool Water::set_wwDailyTemp(const char * value, const int8_t id) {
    int temperature;
    if (!Helpers::value2temperature(value, temperature)) {
        return false;
    }
    write_command(0x7A6, 6, (uint8_t)temperature, 0x7A6);
    return true;
}

bool Water::set_wwDisinfectionTemp(const char * value, const int8_t id) {
    int temperature;
    if (!Helpers::value2temperature(value, temperature)) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_IPM) {
        write_command(0x33, 8, (uint8_t)temperature, 0x33);
    } else if (flags() == EMSdevice::EMS_DEVICE_FLAG_MMPLUS) {
        write_command(0x313 + wwc_, 9, (uint8_t)temperature, 0x313 + wwc_);
    } else { // SM100
        write_command(0x7A6, 12, (uint8_t)temperature, 0x7A6);
    }
    return true;
}

bool Water::set_wwCirc(const char * value, const int8_t id) {
    bool b;
    if (!Helpers::value2bool(value, b)) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_IPM) {
        write_command(0x33, 6, b ? 0xFF : 0x00, 0x33);
    } else if (flags() == EMSdevice::EMS_DEVICE_FLAG_MMPLUS) {
        write_command(0x33B + wwc_, 0, b ? 0x01 : 0x00, 0x33B + wwc_);
    } else { // SM100
        write_command(0x7A5, 0, b ? 0xFF : 0x00, 0x7A5);
    }
    return true;
}

bool Water::set_wwCircMode(const char * value, const int8_t id) {
    uint8_t num;
    if (!Helpers::value2enum(value, num, FL_(enum_wwCircMode))) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_IPM) {
        write_command(0x33, 7, num, 0x33);
    } else if (flags() == EMSdevice::EMS_DEVICE_FLAG_MMPLUS) {
        write_command(0x313 + wwc_, 0, num, 0x313 + wwc_);
    } else { // SM100
        write_command(0x7A5, 3, num, 0x7A5);
    }
    return true;
}

bool Water::set_wwKeepWarm(const char * value, const int8_t id) {
    bool b;
    if (!Helpers::value2bool(value, b)) {
        return false;
    }
    write_command(0x7AE, 0, b ? 0xFF : 0x00, 0x7AE);
    return true;
}

bool Water::set_wwDiffTemp(const char * value, const int8_t id) {
    uint8_t wwc = device_id() - 0x28;
    float   v;
    if (!Helpers::value2temperature(value, v)) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_MMPLUS) {
        write_command(0x313 + wwc, 7, (int8_t)(v * 10), 0x313 + wwc);
        return true;
    }
    return false;
}

bool Water::set_wwRequiredTemp(const char * value, const int8_t id) {
    uint8_t wwc = device_id() - 0x28;
    float   v;
    if (!Helpers::value2temperature(value, v)) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_MMPLUS) {
        write_command(0x313 + wwc, 4, (uint8_t)v, 0x313 + wwc);
        return true;
    }
    return false;
}


bool Water::set_wwFlowTempOffset(const char * value, const int8_t id) {
    int n;
    if (!Helpers::value2number(value, n)) {
        return false;
    }
    write_command(0x33, 5, n, 0x33);
    return true;
}

bool Water::set_wwHystOn(const char * value, const int8_t id) {
    int n;
    if (!Helpers::value2number(value, n)) {
        return false;
    }
    write_command(0x33, 3, n, 0x33);
    return true;
}

bool Water::set_wwHystOff(const char * value, const int8_t id) {
    int n;
    if (!Helpers::value2number(value, n)) {
        return false;
    }
    write_command(0x33, 4, n, 0x33);
    return true;
}
} // namespace emsesp
