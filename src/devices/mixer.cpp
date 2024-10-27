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

#include "mixer.h"

namespace emsesp {

REGISTER_FACTORY(Mixer, EMSdevice::DeviceType::MIXER);

uuid::log::Logger Mixer::logger_{F_(mixer), uuid::log::Facility::CONSOLE};

Mixer::Mixer(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    int8_t tag = DeviceValueTAG::TAG_HC1 + device_id - 0x20;
    // EMS+
    if (flags == EMSdevice::EMS_DEVICE_FLAG_MMPLUS) {
        register_telegram_type(device_id - 0x20 + 0x02D7, "MMPLUSStatusMessage", false, MAKE_PF_CB(process_MMPLUSStatusMessage_HC));
        // register_telegram_type(device_id - 0x20 + 0x02E1, "MMPLUSSetMessage", true, MAKE_PF_CB(process_MMPLUSSetMessage_HC));
        register_telegram_type(device_id - 0x20 + 0x02CD, "MMPLUSConfigMessage", true, MAKE_PF_CB(process_MMPLUSConfigMessage_HC));
        register_device_value(tag, &flowTempHc_, DeviceValueType::UINT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(flowTempHc), DeviceValueUOM::DEGREES);
        register_device_value(tag, &status_, DeviceValueType::UINT8, FL_(mixerStatus), DeviceValueUOM::PERCENT);
        register_device_value(tag, &flowSetTemp_, DeviceValueType::UINT8, FL_(flowSetTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_flowSetTemp));
        register_device_value(tag, &pumpStatus_, DeviceValueType::BOOL, FL_(pumpStatus), DeviceValueUOM::NONE, MAKE_CF_CB(set_pump));
        register_device_value(tag, &activated_, DeviceValueType::BOOL, FL_(activated), DeviceValueUOM::NONE, MAKE_CF_CB(set_activated));
        register_device_value(tag,
                              &setValveTime_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_MUL10,
                              FL_(mixerSetTime),
                              DeviceValueUOM::SECONDS,
                              MAKE_CF_CB(set_setValveTime),
                              10,
                              600);
        register_device_value(tag, &flowTempOffset_, DeviceValueType::UINT8, FL_(flowtempoffset), DeviceValueUOM::K, MAKE_CF_CB(set_flowTempOffset), 0, 20);
    }

    // EMS 1.0
    if (flags == EMSdevice::EMS_DEVICE_FLAG_MM10) {
        register_telegram_type(0x00AA, "MMConfigMessage", true, MAKE_PF_CB(process_MMConfigMessage));
        register_telegram_type(0x00AB, "MMStatusMessage", false, MAKE_PF_CB(process_MMStatusMessage));
        register_telegram_type(0x00AC, "MMSetMessage", false, MAKE_PF_CB(process_MMSetMessage));
        register_device_value(tag, &flowTempHc_, DeviceValueType::UINT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(flowTempHc), DeviceValueUOM::DEGREES);
        register_device_value(tag, &status_, DeviceValueType::INT8, FL_(mixerStatus), DeviceValueUOM::PERCENT);
        register_device_value(tag, &flowSetTemp_, DeviceValueType::UINT8, FL_(flowSetTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_flowSetTemp));
        register_device_value(tag, &pumpStatus_, DeviceValueType::BOOL, FL_(pumpStatus), DeviceValueUOM::NONE, MAKE_CF_CB(set_pump));
        register_device_value(tag, &activated_, DeviceValueType::BOOL, FL_(activated), DeviceValueUOM::NONE, MAKE_CF_CB(set_activated));
        register_device_value(tag,
                              &setValveTime_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_MUL10,
                              FL_(mixerSetTime),
                              DeviceValueUOM::SECONDS,
                              MAKE_CF_CB(set_setValveTime),
                              10,
                              600);
    }

    // HT3
    if (flags == EMSdevice::EMS_DEVICE_FLAG_IPM) {
        register_telegram_type(0x010C, "IPMStatusMessage", false, MAKE_PF_CB(process_IPMStatusMessage));
        register_telegram_type(0x011E, "IPMTempMessage", false, MAKE_PF_CB(process_IPMTempMessage));
        // register_telegram_type(0x0123, "IPMSetMessage", false, MAKE_PF_CB(process_IPMSetMessage));
        register_device_value(tag, &flowTempHc_, DeviceValueType::UINT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(flowTempHc), DeviceValueUOM::DEGREES);
        register_device_value(tag, &status_, DeviceValueType::UINT8, FL_(mixerStatus), DeviceValueUOM::PERCENT);
        register_device_value(tag, &flowSetTemp_, DeviceValueType::UINT8, FL_(flowSetTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_flowSetTemp));
        register_device_value(tag, &pumpStatus_, DeviceValueType::BOOL, FL_(pumpStatus), DeviceValueUOM::NONE, MAKE_CF_CB(set_pump));
        register_device_value(tag, &flowTempVf_, DeviceValueType::UINT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(flowTempVf), DeviceValueUOM::DEGREES);
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
    has_update(telegram, status_, 4); // valve status -100 to 100
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

// Mixer Config 0x2CD, ..
// mixer(0x20) -W-> Me(0x0B), ?(0x02CD), data: FF 0E 05 FF 1E 00
void Mixer::process_MMPLUSConfigMessage_HC(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, activated_, 0);      // on = 0xFF
    has_update(telegram, setValveTime_, 1);   // valve runtime in 10 sec, default 120 s, max 600 s
    has_update(telegram, flowTempOffset_, 2); // Mixer increase [0-20 K]
}

// Thermostat(0x10) -> Mixer(0x20), ?(0x2E1), data: 01 1C 64 00 01
// Thermostat(0x10) -> Mixing Module(0x20), (0x2E1), data: 01 00 00 00 01
// Thermostat(0x10) -> Mixing Module(0x20), (0x2EB), data: 00
// void Mixer::process_MMPLUSSetMessage_HC(std::shared_ptr<const Telegram> telegram) {
// pos 1: setpoint
// pos2: pump
// }

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
        // AC telegram can only be written with offset 0
        uint8_t dat[2] = {flowSetTemp_, b ? (uint8_t)0x64 : (uint8_t)0};
        write_command(0xAC, 0, dat, sizeof(dat), 0xAB);
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
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_MMPLUS) {
        uint8_t hc = device_id() - 0x20;
        write_command(0x2CD + hc, 0, b ? 0xFF : 0, 0x2CD + hc);
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
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_MMPLUS) {
        v          = (v + 5) / 10;
        uint8_t hc = device_id() - 0x20;
        write_command(0x2CD + hc, 1, v, 0x2CD + hc);
        return true;
    }
    return false;
}

bool Mixer::set_flowTempOffset(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_MMPLUS) {
        uint8_t hc = device_id() - 0x20;
        write_command(0x2CD + hc, 2, v, 0x2CD + hc);
        return true;
    }
    return false;
}

} // namespace emsesp
