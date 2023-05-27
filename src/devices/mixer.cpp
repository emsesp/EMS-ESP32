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

#include "mixer.h"

namespace emsesp {

REGISTER_FACTORY(Mixer, EMSdevice::DeviceType::MIXER);

uuid::log::Logger Mixer::logger_{F_(mixer), uuid::log::Facility::CONSOLE};

Mixer::Mixer(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    // Pool module
    if (flags == EMSdevice::EMS_DEVICE_FLAG_MP) {
        register_telegram_type(0x5BA, "HpPoolStatus", true, MAKE_PF_CB(process_HpPoolStatus));
        type_ = Type::MP;
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &poolTemp_,
                              DeviceValueType::SHORT,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(poolTemp),
                              DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &poolShuntStatus_, DeviceValueType::ENUM, FL_(enum_shunt), FL_(poolShuntStatus), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &poolShunt_, DeviceValueType::UINT, FL_(poolShunt), DeviceValueUOM::PERCENT);
    }

    // EMS+
    if (flags == EMSdevice::EMS_DEVICE_FLAG_MMPLUS) {
        if (device_id >= 0x20 && device_id <= 0x27) {
            register_telegram_type(device_id - 0x20 + 0x02D7, "MMPLUSStatusMessage_HC", false, MAKE_PF_CB(process_MMPLUSStatusMessage_HC));
            // register_telegram_type(device_id - 0x20 + 0x02E1, "MMPLUSStetMessage_HC", true, MAKE_PF_CB(process_MMPLUSSetMessage_HC));
            type_       = Type::HC;
            hc_         = device_id - 0x20 + 1;
            uint8_t tag = DeviceValueTAG::TAG_HC1 + hc_ - 1;
            register_device_value(tag, &flowTempHc_, DeviceValueType::USHORT, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(flowTempHc), DeviceValueUOM::DEGREES);
            register_device_value(tag, &status_, DeviceValueType::INT, FL_(mixerStatus), DeviceValueUOM::PERCENT);
            register_device_value(tag, &flowSetTemp_, DeviceValueType::UINT, FL_(flowSetTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_flowSetTemp));
            register_device_value(tag, &pumpStatus_, DeviceValueType::BOOL, FL_(pumpStatus), DeviceValueUOM::NONE, MAKE_CF_CB(set_pump));
        } else if (device_id >= 0x28 && device_id <= 0x29) {
            register_telegram_type(device_id - 0x28 + 0x0331, "MMPLUSStatusMessage_WWC", false, MAKE_PF_CB(process_MMPLUSStatusMessage_WWC));
            register_telegram_type(device_id - 0x28 + 0x0313, "MMPLUSConfigMessage_WWC", true, MAKE_PF_CB(process_MMPLUSConfigMessage_WWC));
            // register_telegram_type(device_id - 0x28 + 0x033B, "MMPLUSSetMessage_WWC", true, MAKE_PF_CB(process_MMPLUSSetMessage_WWC));
            type_       = Type::WWC;
            hc_         = device_id - 0x28 + 1;
            uint8_t tag = DeviceValueTAG::TAG_WWC1 + hc_ - 1;
            register_device_value(tag, &flowTempHc_, DeviceValueType::USHORT, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(wwTemp), DeviceValueUOM::DEGREES);
            register_device_value(tag, &pumpStatus_, DeviceValueType::BOOL, FL_(wwPumpStatus), DeviceValueUOM::NONE);
            register_device_value(tag, &status_, DeviceValueType::INT, FL_(wwTempStatus), DeviceValueUOM::NONE);

            register_device_value(tag, &wwMaxTemp_, DeviceValueType::UINT, FL_(wwMaxTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_wwMaxTemp));
            register_device_value(tag, &wwDiffTemp_, DeviceValueType::INT, FL_(wwDiffTemp), DeviceValueUOM::DEGREES_R, MAKE_CF_CB(set_wwDiffTemp));
            register_device_value(tag,
                                  &wwDisinfectionTemp_,
                                  DeviceValueType::UINT,
                                  FL_(wwDisinfectionTemp),
                                  DeviceValueUOM::DEGREES,
                                  MAKE_CF_CB(set_wwDisinfectionTemp));
            register_device_value(tag, &wwReducedTemp_, DeviceValueType::UINT, FL_(wwRedTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_wwReducedTemp));
            register_device_value(tag, &wwRequiredTemp_, DeviceValueType::UINT, FL_(wwRequiredTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_wwRequiredTemp));
            register_device_value(DeviceValueTAG::TAG_DEVICE_DATA_WW,
                                  &wwCircPump_,
                                  DeviceValueType::BOOL,
                                  FL_(wwCircPump),
                                  DeviceValueUOM::NONE,
                                  MAKE_CF_CB(set_wwCircPump));
            register_device_value(tag, &wwCircMode_, DeviceValueType::ENUM, FL_(enum_wwCircMode), FL_(wwCircMode), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwCircMode));
        }
    }

    // EMS 1.0
    if (flags == EMSdevice::EMS_DEVICE_FLAG_MM10) {
        register_telegram_type(0x00AA, "MMConfigMessage", true, MAKE_PF_CB(process_MMConfigMessage));
        register_telegram_type(0x00AB, "MMStatusMessage", false, MAKE_PF_CB(process_MMStatusMessage));
        register_telegram_type(0x00AC, "MMSetMessage", false, MAKE_PF_CB(process_MMSetMessage));
        type_       = Type::HC;
        hc_         = device_id - 0x20 + 1;
        uint8_t tag = DeviceValueTAG::TAG_HC1 + hc_ - 1;
        register_device_value(tag, &flowTempHc_, DeviceValueType::USHORT, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(flowTempHc), DeviceValueUOM::DEGREES);
        register_device_value(tag, &status_, DeviceValueType::INT, FL_(mixerStatus), DeviceValueUOM::PERCENT);
        register_device_value(tag, &flowSetTemp_, DeviceValueType::UINT, FL_(flowSetTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_flowSetTemp));
        register_device_value(tag, &pumpStatus_, DeviceValueType::BOOL, FL_(pumpStatus), DeviceValueUOM::NONE, MAKE_CF_CB(set_pump));
        register_device_value(tag, &activated_, DeviceValueType::BOOL, FL_(activated), DeviceValueUOM::NONE, MAKE_CF_CB(set_activated));
        register_device_value(tag,
                              &setValveTime_,
                              DeviceValueType::UINT,
                              DeviceValueNumOp::DV_NUMOP_MUL10,
                              FL_(mixerSetTime),
                              DeviceValueUOM::SECONDS,
                              MAKE_CF_CB(set_setValveTime),
                              10,
                              120);
    }

    // HT3
    if (flags == EMSdevice::EMS_DEVICE_FLAG_IPM) {
        if (device_id >= 0x40) { // special DHW pos 10
            register_telegram_type(0x34, "UBAMonitorWW", false, MAKE_PF_CB(process_IPMMonitorWW));
            register_telegram_type(0x1E, "HydrTemp", false, MAKE_PF_CB(process_IPMHydrTemp));
            register_telegram_type(0x33, "UBAParameterWW", true, MAKE_PF_CB(process_IPMParameterWW));
            // register_telegram_type(0x10D, "wwNTCStatus", false, MAKE_PF_CB(process_wwNTCStatus));
            type_       = Type::WWC;
            hc_         = device_id - 0x40 + 1;
            uint8_t tag = DeviceValueTAG::TAG_WWC9 + hc_ - 1;
            register_device_value(tag, &wwSelTemp_, DeviceValueType::UINT, FL_(wwSelTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_wwSelTemp));
            register_device_value(tag, &wwCurTemp_1_, DeviceValueType::USHORT, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(wwCurTemp), DeviceValueUOM::DEGREES);
            register_device_value(tag, &wwCurTemp_2_, DeviceValueType::USHORT, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(wwCurTemp2), DeviceValueUOM::DEGREES);
            register_device_value(tag, &HydrTemp_, DeviceValueType::USHORT, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(hydrTemp), DeviceValueUOM::DEGREES);
            register_device_value(tag, &pumpStatus_, DeviceValueType::BOOL, FL_(pumpStatus), DeviceValueUOM::NONE);
            register_device_value(tag, &wwFlowTempOffset_, DeviceValueType::UINT, FL_(wwFlowTempOffset), DeviceValueUOM::DEGREES_R, MAKE_CF_CB(set_wwFlowTempOffset));
            register_device_value(tag, &wwHystOn_, DeviceValueType::INT, FL_(wwHystOn), DeviceValueUOM::DEGREES_R, MAKE_CF_CB(set_wwHystOn));
            register_device_value(tag, &wwHystOff_, DeviceValueType::INT, FL_(wwHystOff), DeviceValueUOM::DEGREES_R, MAKE_CF_CB(set_wwHystOff));
            register_device_value(tag,
                                  &wwDisinfectionTemp_,
                                  DeviceValueType::UINT,
                                  FL_(wwDisinfectionTemp),
                                  DeviceValueUOM::DEGREES,
                                  MAKE_CF_CB(set_wwDisinfectionTemp));
            register_device_value(DeviceValueTAG::TAG_DEVICE_DATA_WW,
                                  &wwCircPump_,
                                  DeviceValueType::BOOL,
                                  FL_(wwCircPump),
                                  DeviceValueUOM::NONE,
                                  MAKE_CF_CB(set_wwCircPump));
            register_device_value(tag, &wwCircMode_, DeviceValueType::ENUM, FL_(enum_wwCircMode), FL_(wwCircMode), DeviceValueUOM::NONE, MAKE_CF_CB(set_wwCircMode));
        } else {
            register_telegram_type(0x010C, "IPMStatusMessage", false, MAKE_PF_CB(process_IPMStatusMessage));
            register_telegram_type(0x011E, "IPMTempMessage", false, MAKE_PF_CB(process_IPMTempMessage));
            // register_telegram_type(0x0123, "IPMSetMessage", false, MAKE_PF_CB(process_IPMSetMessage));
            type_       = Type::HC;
            hc_         = device_id - 0x20 + 1;
            uint8_t tag = DeviceValueTAG::TAG_HC1 + hc_ - 1;
            register_device_value(tag, &flowTempHc_, DeviceValueType::USHORT, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(flowTempHc), DeviceValueUOM::DEGREES);
            register_device_value(tag, &status_, DeviceValueType::INT, FL_(mixerStatus), DeviceValueUOM::PERCENT);
            register_device_value(tag, &flowSetTemp_, DeviceValueType::UINT, FL_(flowSetTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_flowSetTemp));
            register_device_value(tag, &pumpStatus_, DeviceValueType::BOOL, FL_(pumpStatus), DeviceValueUOM::NONE, MAKE_CF_CB(set_pump));
            register_device_value(tag, &flowTempVf_, DeviceValueType::USHORT, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(flowTempVf), DeviceValueUOM::DEGREES);
        }
    }
}

// heating circuits 0x02D7, 0x02D8 etc...
// e.g.  A0 00 FF 00 01 D7 00 00 00 80 00 00 00 00 03 C5
//       A0 0B FF 00 01 D7 00 00 00 80 00 00 00 00 03 80
void Mixer::process_MMPLUSStatusMessage_HC(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, flowTempHc_, 3); // is * 10
    has_update(telegram, flowSetTemp_, 5);
    has_bitupdate(telegram, pumpStatus_, 0, 0);
    has_update(telegram, status_, 2); // valve status
}

// Mixer warm water loading/DHW - 0x0331, 0x0332
// e.g. A9 00 FF 00 02 32 02 6C 00 3C 00 3C 3C 46 02 03 03 00 3C // on 0x28
//      A8 00 FF 00 02 31 02 35 00 3C 00 3C 3C 46 02 03 03 00 3C // in 0x29
void Mixer::process_MMPLUSStatusMessage_WWC(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, flowTempHc_, 0); // is * 10
    has_bitupdate(telegram, pumpStatus_, 2, 0);
    has_update(telegram, status_, 11);    // temp status
}

// Mixer IPM - 0x010C
// e.g.  A0 00 FF 00 00 0C 01 00 00 00 00 00 54
//       A1 00 FF 00 00 0C 02 04 00 01 1D 00 82
void Mixer::process_IPMStatusMessage(std::shared_ptr<const Telegram> telegram) {
    // check if circuit is active, 0-off, 1-unmixed, 2-mixed
    uint8_t ismixed = 0;
    telegram->read_value(ismixed, 0);
    if (ismixed == 0) {
        return;
    }

    // do we have a mixed circuit
    if (ismixed == 2) {
        has_update(telegram, flowTempHc_, 3); // is * 10
        has_update(telegram, status_, 2);     // valve status
    }

    has_bitupdate(telegram, pumpStatus_, 1, 0); // pump is also in unmixed circuits
    has_update(telegram, flowSetTemp_, 5);      // flowSettemp is also in unmixed circuits, see #711
}

// Mixer IPM - 0x001E Temperature Message in unmixed circuits
// in unmixed circuits FlowTemp in 10C is zero, this is the measured flowtemp in header
void Mixer::process_IPMTempMessage(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, flowTempVf_, 0); // TC1, is * 10
}

// Mixer MP100 for pools - 0x5BA
void Mixer::process_HpPoolStatus(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, poolTemp_, 0);
    has_update(telegram, poolShunt_, 3); // 0-100% how much is the shunt open?
    telegram->read_value(poolShuntStatus__, 2);
    uint8_t pss = poolShunt_ == 100 ? 3 : (poolShunt_ == 0 ? 4 : poolShuntStatus__);
    has_update(poolShuntStatus_, pss);
}

// Mixer on a MM10 - 0xAB
// e.g. Mixer Module -> All, type 0xAB, telegram: 21 00 AB 00 2D 01 BE 64 04 01 00 (CRC=15) #data=7
// see also https://github.com/emsesp/EMS-ESP/issues/386
void Mixer::process_MMStatusMessage(std::shared_ptr<const Telegram> telegram) {
    // the heating circuit is determine by which device_id it is, 0x20 - 0x23
    // 0x21 is position 2. 0x20 is typically reserved for the WM10 switch module
    // see https://github.com/emsesp/EMS-ESP/issues/270 and https://github.com/emsesp/EMS-ESP/issues/386#issuecomment-629610918

    has_update(telegram, flowTempHc_, 1);       // is * 10
    has_bitupdate(telegram, pumpStatus_, 3, 2); // is 0 or 0x64 (100%), check only bit 2
    has_update(telegram, flowSetTemp_, 0);
    has_update(telegram, status_, 4);           // valve status -100 to 100
}

/*
* The set-messages are not broadcasted and send from thermostat to mixer,
* we have to fetch for processing
*/

// Mixer on a MM10 - 0xAA
// e.g. Thermostat -> Mixer Module, type 0xAA, telegram: 10 21 AA 00 FF 0C 0A 11 0A 32 xx
void Mixer::process_MMConfigMessage(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, activated_, 0);    // on = 0xFF
    has_update(telegram, setValveTime_, 1); // valve runtime in 10 sec, max 120 s
}

// Config message 0x313, has to be fetched
void Mixer::process_MMPLUSConfigMessage_WWC(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, wwRequiredTemp_, 4);
    has_update(telegram, wwReducedTemp_, 5);
    has_update(telegram, wwDiffTemp_, 7);
    has_update(telegram, wwDisinfectionTemp_, 9);
    has_update(telegram, wwMaxTemp_, 10);
}

// 0x34 only 8 bytes long
// Mixer(0x41) -> All(0x00), UBAMonitorWW(0x34), data: 37 02 1E 02 1E 00 00 00 00
void Mixer::process_IPMMonitorWW(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, wwSelTemp_, 0);
    has_update(telegram, wwCurTemp_1_, 1);
    has_update(telegram, wwCurTemp_2_, 3);
    has_bitupdate(telegram, pumpStatus_, 5, 3);
}

// Mixer(0x41) -> Me(0x0B), UBAParameterWW(0x33), data: 08 FF 46 FB FF 28 FF 07 46 00 FF 00
void Mixer::process_IPMParameterWW(std::shared_ptr<const Telegram> telegram) {
    // has_update(telegram, wwActivated_, 1); // 0xFF means on
    // has_update(telegram, wwSelTemp_, 2);
    has_update(telegram, wwHystOn_, 3);         // Hyst on (default -5)
    has_update(telegram, wwHystOff_, 4);        // Hyst off (default -1)
    has_update(telegram, wwFlowTempOffset_, 5); // default 40
    has_update(telegram, wwCircPump_, 6);       // 0xFF means on
    has_update(telegram, wwCircMode_, 7);       // 1=1x3min 6=6x3min 7=continuous
    has_update(telegram, wwDisinfectionTemp_, 8);
    // has_bitupdate(telegram, wwChargeType_, 10, 0); // 0 = charge pump, 0xff = 3-way valve
}


// 0x1E, only16 bit temperature
// Mixer(0x41) -> Boiler(0x08), HydrTemp(0x1E), data: 01 D8
void Mixer::process_IPMHydrTemp(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, HydrTemp_, 0);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

// Thermostat(0x10) -> Mixer(0x20), ?(0x2E1), data: 01 1C 64 00 01
// Thermostat(0x10) -> Mixing Module(0x20), (0x2E1), data: 01 00 00 00 01
// Thermostat(0x10) -> Mixing Module(0x20), (0x2EB), data: 00
void Mixer::process_MMPLUSSetMessage_HC(std::shared_ptr<const Telegram> telegram) {
    // pos 1: setpoint
    // pos2: pump
}

// unknown, 2 examples from older threads
// Thermostat(0x10) -> Mixer(0x28), ?(0x33B), data: 01 01 00
// Thermostat -> Mixing Module, type 0x023B, telegram: 90 28 FF 00 02 3B 00 02 00 (CRC=68)
void Mixer::process_MMPLUSSetMessage_WWC(std::shared_ptr<const Telegram> telegram) {
}

// MMPLUS telegram 0x345 unknown
// Solar Module -> Mixing Module, type 0x0245, telegram: B0 28 FF 00 02 45 64 01 01 (CRC=36)
// ?

// Mixer on a MM10 - 0xAC
// e.g. Thermostat -> Mixer Module, type 0xAC, telegram: 10 21 AC 00 1E 64 01 AB
void Mixer::process_MMSetMessage(std::shared_ptr<const Telegram> telegram) {
    // pos 0: flowtemp setpoint 1E = 30°C
    // pos 1: pump in %
    // pos 2 flags (mostly 01)
    // LOG_INFO("MM10 SetMessage received");
}

// Thermostat(0x10) -> Mixer(0x21), ?(0x23), data: 1A 64 00 90 21 23 00 1A 64 00 89
void Mixer::process_IPMSetMessage(std::shared_ptr<const Telegram> telegram) {
    // pos 0: flowtemp setpoint 1A = 26°C
    // pos 1: pump in %?
}

#pragma GCC diagnostic pop

bool Mixer::set_wwSelTemp(const char * value, const int8_t id) {
    int temperature;
    if (!Helpers::value2temperature(value, temperature)) {
        return false;
    }
    write_command(0x35, 3, (uint8_t)temperature, 0x34);
    return true;
}

bool Mixer::set_flowSetTemp(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_MM10) {
        write_command(0xAC, 0, v, 0xAB);
        return true;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_IPM) {
        write_command(0x123, 0, v, 0x10C);
        return true;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_MMPLUS) {
        uint8_t hc = device_id() - 0x20;
        write_command(0x2E1 + hc, 1, v, 0x2D7 + hc);
        return true;
    }
    return false;
}

bool Mixer::set_pump(const char * value, const int8_t id) {
    bool b;
    if (!Helpers::value2bool(value, b)) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_MM10) {
        write_command(0xAC, 1, b ? 0x64 : 0, 0xAB);
        return true;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_IPM) {
        write_command(0x123, 1, b ? 0x64 : 0, 0x10C);
        return true;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_MMPLUS) {
        uint8_t hc = device_id() - 0x20;
        write_command(0x2E1 + hc, 2, b ? 0x64 : 0, 0x2D7 + hc);
        return true;
    }
    return false;
}

bool Mixer::set_activated(const char * value, const int8_t id) {
    bool b;
    if (!Helpers::value2bool(value, b)) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_MM10) {
        write_command(0xAA, 0, b ? 0xFF : 0, 0xAA);
        return true;
    }
    return false;
}

bool Mixer::set_setValveTime(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_MM10) {
        v = (v + 5) / 10;
        write_command(0xAA, 1, v, 0xAA);
        return true;
    }
    return false;
}

bool Mixer::set_wwMaxTemp(const char * value, const int8_t id) {
    uint8_t wwc = device_id() - 0x28;
    float   v;
    if (!Helpers::value2temperature(value, v)) {
        return false;
    }
    write_command(0x313 + wwc, 10, (uint8_t)v, 0x313 + wwc);
    return true;
}

bool Mixer::set_wwDiffTemp(const char * value, const int8_t id) {
    uint8_t wwc = device_id() - 0x28;
    float   v;
    if (!Helpers::value2temperature(value, v)) {
        return false;
    }
    write_command(0x313 + wwc, 7, (int8_t)(v * 10), 0x313 + wwc);
    return true;
}

bool Mixer::set_wwReducedTemp(const char * value, const int8_t id) {
    uint8_t wwc = device_id() - 0x28;
    float   v;
    if (!Helpers::value2temperature(value, v)) {
        return false;
    }
    write_command(0x313 + wwc, 5, (uint8_t)v, 0x313 + wwc);
    return true;
}

bool Mixer::set_wwRequiredTemp(const char * value, const int8_t id) {
    uint8_t wwc = device_id() - 0x28;
    float   v;
    if (!Helpers::value2temperature(value, v)) {
        return false;
    }
    write_command(0x313 + wwc, 4, (uint8_t)v, 0x313 + wwc);
    return true;
}

bool Mixer::set_wwDisinfectionTemp(const char * value, const int8_t id) {
    float v;
    if (!Helpers::value2temperature(value, v)) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_IPM) {
        write_command(0x33, 8, (uint8_t)v, 0x33);
    } else {
        uint8_t wwc = device_id() - 0x28;
        write_command(0x313 + wwc, 9, (uint8_t)v, 0x313 + wwc);
    }
    return true;
}

bool Mixer::set_wwCircPump(const char * value, const int8_t id) {
    bool v;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_IPM) {
        write_command(0x33, 6, v ? 0xFF : 0x00, 0x33);
    } else {
        uint8_t wwc = device_id() - 0x28;
        write_command(0x33B + wwc, 0, v ? 0x01 : 0x00, 0x33B + wwc);
    }
    return true;
}

bool Mixer::set_wwCircMode(const char * value, const int8_t id) {
    uint8_t n;
    if (!Helpers::value2enum(value, n, FL_(enum_wwCircMode))) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_IPM) {
        write_command(0x33, 7, n, 0x33);
    } else {
        uint8_t wwc = device_id() - 0x28;
        write_command(0x313 + wwc, 0, n, 0x313 + wwc);
    }
    return true;
}

bool Mixer::set_wwFlowTempOffset(const char * value, const int8_t id) {
    int n;
    if (!Helpers::value2number(value, n)) {
        return false;
    }
    write_command(0x33, 5, n, 0x33);
    return true;
}

bool Mixer::set_wwHystOn(const char * value, const int8_t id) {
    int n;
    if (!Helpers::value2number(value, n)) {
        return false;
    }
    write_command(0x33, 3, n, 0x33);
    return true;
}

bool Mixer::set_wwHystOff(const char * value, const int8_t id) {
    int n;
    if (!Helpers::value2number(value, n)) {
        return false;
    }
    write_command(0x33, 4, n, 0x33);
    return true;
}

} // namespace emsesp
