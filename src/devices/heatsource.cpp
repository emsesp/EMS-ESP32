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

#include "heatsource.h"

namespace emsesp {

REGISTER_FACTORY(Heatsource, EMSdevice::DeviceType::HEATSOURCE);

Heatsource::Heatsource(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    // AM200 alternative heatsource
    if (device_id == EMSdevice::EMS_DEVICE_ID_BOILER || (device_id >= EMSdevice::EMS_DEVICE_ID_AHS1 && device_id < EMSdevice::EMS_DEVICE_ID_HS1)) {
        int8_t tag = device_id == EMSdevice::EMS_DEVICE_ID_BOILER
                         ? DeviceValueTAG::TAG_DEVICE_DATA
                         : DeviceValueTAG::TAG_AHS1 + device_id - EMSdevice::EMS_DEVICE_ID_AHS1; // heating source id, count from 0
        register_telegram_type(0x54D, "AmTemperatures", false, MAKE_PF_CB(process_amTempMessage));
        register_telegram_type(0x54E, "AmStatus", false, MAKE_PF_CB(process_amStatusMessage));
        register_telegram_type(0x54F, "AmCommand", false, MAKE_PF_CB(process_amCommandMessage)); // not broadcasted, but actually not used
        register_telegram_type(0x550, "AmExtra", false, MAKE_PF_CB(process_amExtraMessage));
        register_telegram_type(0x54C, "AmSettings", true, MAKE_PF_CB(process_amSettingMessage)); // not broadcasted

        register_device_value(tag, &curFlowTemp_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(sysFlowTemp), DeviceValueUOM::DEGREES);
        register_device_value(tag, &retTemp_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(sysRetTemp), DeviceValueUOM::DEGREES);
        register_device_value(tag, &aFlowTemp_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(aFlowTemp), DeviceValueUOM::DEGREES);
        register_device_value(tag, &aRetTemp_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(aRetTemp), DeviceValueUOM::DEGREES);
        register_device_value(tag, &cylTopTemp_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(aCylTopTemp), DeviceValueUOM::DEGREES);
        register_device_value(tag, &cylCenterTemp_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(aCylCenterTemp), DeviceValueUOM::DEGREES);
        register_device_value(tag, &cylBottomTemp_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(aCylBottomTemp), DeviceValueUOM::DEGREES);
        register_device_value(tag, &flueGasTemp_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(flueGasTemp), DeviceValueUOM::DEGREES);
        // register_device_value(tag, &valveByPass_, DeviceValueType::BOOL, nullptr, FL_(valveByPass), DeviceValueUOM::NONE);
        register_device_value(tag, &valveBuffer_, DeviceValueType::UINT8, FL_(valveBuffer), DeviceValueUOM::PERCENT);
        register_device_value(tag, &valveReturn_, DeviceValueType::UINT8, FL_(valveReturn), DeviceValueUOM::PERCENT);
        register_device_value(tag, &aPumpMod_, DeviceValueType::UINT8, FL_(aPumpMod), DeviceValueUOM::PERCENT);
        // register_device_value(tag, &heatSource_, DeviceValueType::BOOL, nullptr, FL_(heatSource), DeviceValueUOM::NONE);
        // Settings:
        register_device_value(tag, &vr2Config_, DeviceValueType::ENUM, FL_(enum_vr2Config), FL_(vr2Config), DeviceValueUOM::NONE, MAKE_CF_CB(set_vr2Config));
        register_device_value(tag, &ahsActivated_, DeviceValueType::BOOL, FL_(ahsActivated), DeviceValueUOM::NONE, MAKE_CF_CB(set_ahsActivated));
        register_device_value(tag, &aPumpConfig_, DeviceValueType::BOOL, FL_(aPumpConfig), DeviceValueUOM::NONE, MAKE_CF_CB(set_aPumpConfig));
        register_device_value(tag, &aPumpSignal_, DeviceValueType::ENUM, FL_(enum_aPumpSignal), FL_(aPumpSignal), DeviceValueUOM::NONE, MAKE_CF_CB(set_aPumpSignal));
        register_device_value(tag, &aPumpMin_, DeviceValueType::UINT8, FL_(aPumpMin), DeviceValueUOM::PERCENT, MAKE_CF_CB(set_aPumpMin), 12, 50);
        register_device_value(tag, &tempRise_, DeviceValueType::BOOL, FL_(tempRise), DeviceValueUOM::NONE, MAKE_CF_CB(set_tempRise));
        register_device_value(tag, &setReturnTemp_, DeviceValueType::UINT8, FL_(setReturnTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_setReturnTemp), 40, 75);
        register_device_value(tag, &mixRuntime_, DeviceValueType::UINT16, FL_(mixRuntime), DeviceValueUOM::SECONDS, MAKE_CF_CB(set_mixRuntime), 0, 600);
        register_device_value(tag, &setFlowTemp_, DeviceValueType::UINT8, FL_(setFlowTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_setFlowTemp), 40, 75);
        register_device_value(tag, &bufBypass_, DeviceValueType::ENUM, FL_(enum_bufBypass), FL_(bufBypass), DeviceValueUOM::NONE, MAKE_CF_CB(set_bufBypass));
        register_device_value(tag, &bufMixRuntime_, DeviceValueType::UINT16, FL_(bufMixRuntime), DeviceValueUOM::SECONDS, MAKE_CF_CB(set_bufMixRuntime), 0, 600);
        register_device_value(tag, &bufConfig_, DeviceValueType::ENUM, FL_(enum_bufConfig), FL_(bufConfig), DeviceValueUOM::NONE, MAKE_CF_CB(set_bufConfig));
        register_device_value(tag, &blockMode_, DeviceValueType::ENUM, FL_(enum_blockMode), FL_(blockMode), DeviceValueUOM::NONE, MAKE_CF_CB(set_blockMode));
        register_device_value(tag, &blockTerm_, DeviceValueType::ENUM, FL_(enum_blockTerm), FL_(blockTerm), DeviceValueUOM::NONE, MAKE_CF_CB(set_blockTerm));
        register_device_value(tag, &blockHyst_, DeviceValueType::INT8, FL_(blockHyst), DeviceValueUOM::DEGREES_R, MAKE_CF_CB(set_blockHyst), 0, 50);
        register_device_value(tag, &releaseWait_, DeviceValueType::UINT8, FL_(releaseWait), DeviceValueUOM::MINUTES, MAKE_CF_CB(set_releaseWait), 0, 240);

        register_device_value(tag, &burner_, DeviceValueType::BOOL, FL_(burner), DeviceValueUOM::NONE);
        register_device_value(tag, &aPump_, DeviceValueType::BOOL, FL_(aPump), DeviceValueUOM::NONE);
        register_device_value(tag, &heatRequest_, DeviceValueType::UINT8, FL_(heatRequest), DeviceValueUOM::PERCENT);
        register_device_value(tag, &blockRemain_, DeviceValueType::UINT8, FL_(blockRemain), DeviceValueUOM::MINUTES);
        register_device_value(tag, &blockRemainWw_, DeviceValueType::UINT8, FL_(blockRemainWw), DeviceValueUOM::MINUTES);
    }

    // cascaded heating sources, only some values per individual heatsource (hs)
    if (device_id >= EMSdevice::EMS_DEVICE_ID_HS1) {
        int8_t tag = DeviceValueTAG::TAG_HS1 + device_id - EMSdevice::EMS_DEVICE_ID_HS1; // heating source id, count from 0
        // Runtime of each heatingsource in 0x06DC, ff
        register_telegram_type(0x6DC + device_id - EMSdevice::EMS_DEVICE_ID_HS1, "CascadeMessage", false, MAKE_PF_CB(process_CascadeMessage));
        register_device_value(tag, &burnWorkMin_, DeviceValueType::TIME, FL_(burnWorkMin), DeviceValueUOM::MINUTES);
        // selBurnpower in D2 and E4
        // register_telegram_type(0xD2, "CascadePowerMessage", false, MAKE_PF_CB(process_CascadePowerMessage));
        // individual Flowtemps and powervalues for each heatingsource in E4
        register_telegram_type(0xE4, "UBAMonitorFastPlus", false, MAKE_PF_CB(process_UBAMonitorFastPlus));
        register_device_value(tag, &setFlowTemp_, DeviceValueType::UINT8, FL_(setFlowTemp), DeviceValueUOM::DEGREES);
        register_device_value(tag, &selBurnPow_, DeviceValueType::UINT8, FL_(selBurnPow), DeviceValueUOM::PERCENT);
        register_device_value(tag, &curFlowTemp_, DeviceValueType::UINT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(curFlowTemp), DeviceValueUOM::DEGREES);
        register_device_value(tag, &curBurnPow_, DeviceValueType::UINT8, FL_(curBurnPow), DeviceValueUOM::PERCENT);
        return;
    }
}

/*
 * heatingsources (boilers)
 */

// 0x6DC, ff for cascaded heatsources (hs)
void Heatsource::process_CascadeMessage(std::shared_ptr<const Telegram> telegram) {
    telegram->read_value(burnWorkMin_, 3); // this is in seconds
    burnWorkMin_ /= 60;
    has_update(burnWorkMin_);
}

// UBAMonitorFastPlus - type 0xE4 - central heating monitor EMS+
void Heatsource::process_UBAMonitorFastPlus(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, setFlowTemp_, 6);
    has_update(telegram, curBurnPow_, 10);
    has_update(telegram, selBurnPow_, 9);
    has_update(telegram, curFlowTemp_, 7);
}

/*
 * alternative heatingsource AM200
 */
// 0x054D AM200 temperatures
// Rx: 60 00 FF 00 04 4D 0103 0108 8000 00C6 0127 0205 8000 0200 0000 8000 6C
//                        TB4  TR2       TA1  TR1  TB1  TB2* TB3
void Heatsource::process_amTempMessage(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, curFlowTemp_, 0); // TB4
    has_update(telegram, retTemp_, 2);     // TR2
    has_update(telegram, flueGasTemp_, 4);
    has_update(telegram, aFlowTemp_, 6);
    has_update(telegram, aRetTemp_, 8);
    has_update(telegram, cylTopTemp_, 10);
    has_update(telegram, cylCenterTemp_, 12);
    has_update(telegram, cylBottomTemp_, 14);
}

// 0x054E AM200 status (6 bytes long)
// Rx: 60 00 FF 00 04 4E 00 00 00 00 00 00 86
void Heatsource::process_amStatusMessage(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, aPumpMod_, 0); // PR1
    // offset 1: bitfield 01-pump on, 02-VR1 opening, 04-VR1 closing, 08-VB1 opening, 10-VB1 closing
    // actually we dont know the offset of VR2
    // has_update(telegram, valveBypass_, ?); // VR2
    has_bitupdate(telegram, burner_, 1, 5);
    has_bitupdate(telegram, aPump_, 1, 0);
    has_update(telegram, heatRequest_, 2); // percent
    has_update(telegram, valveReturn_, 4); // VR1, percent
    has_update(telegram, valveBuffer_, 5); // VB1, percent
}

// 0x054C AM200 not broadcasted message, 23 bytes long
// data: 00 01 01 00 01 00 41 4B 00 5A 00 5A 00 01 05 3C 00 00 5A 00 01 23 00
void Heatsource::process_amSettingMessage(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, vr2Config_, 12);     // pos 12: off(00)/bypass(01)
    has_update(telegram, ahsActivated_, 0);   // pos 00: Alternate heat source activation: No(00),Yes(01)
    has_update(telegram, aPumpConfig_, 4);    // pos 04: Buffer primary pump->Config pump: No(00),Yes(01)
    has_update(telegram, aPumpSignal_, 3);    // pos 03: Output for PR1 pump: On/Off(00),PWM(01),PWM invers(02)
    has_update(telegram, aPumpMin_, 21);      // pos 21: Min output pump PR1 (%)
    has_update(telegram, tempRise_, 1);       // pos 01: AHS return temp rise: No(00),Yes(01) (mixer VR1)
    has_update(telegram, setReturnTemp_, 6);  // pos 06: Set temp return (°C) (VR1)
    has_update(telegram, mixRuntime_, 8);     // pos 8/9: Mixer run time (s) (VR1)
    has_update(telegram, setFlowTemp_, 7);    // pos 07: Set flow temp AHS (°C) (Buffer)
    has_update(telegram, bufBypass_, 2);      // pos 02: Puffer bypass: No(00), Mischer(01), Ventil(02) (Buffer)
    has_update(telegram, bufMixRuntime_, 10); // pos 10/11: Bypass mixer run time: [time] (s) (Buffer)
    has_update(telegram, bufConfig_, 20);     // pos 20: Konfig WW-Speicher Monovalent(01), Bivalent(02) (buffer)
    has_update(telegram, blockMode_, 16);     // pos 16: Config htg. blocking mode: No(00),Automatic(01),Always block02) (blocking)
    has_update(telegram, blockTerm_, 17);     // pos 17: Config of block terminal: NO(00), NC(01)
    has_update(telegram, blockHyst_, 14);     // pos 14?: Hyst. for bolier block (K)
    has_update(telegram, releaseWait_, 15);   // pos 15: Boiler release wait time (min)
}

// 0x054F AM200 not broadcasted message, 7 bytes long
// Boiler(0x60) -> Me(0x0B), amCommand(0x054F), data: 00 00 00 00 00 00 00
void Heatsource::process_amCommandMessage(std::shared_ptr<const Telegram> telegram) {
    // pos 0: return pump in percent
    // pos 3: setValveBuffer VB1 0-off, 1-open, 2-close
    // pos 2: setValveReturn VR1 0-off, 1-open, 2-close
    // pos 6: boiler blocking 0-off, 1-on
}

// 0x0550 AM200 broadcasted message, all 27 bytes unkown
// Rx: 60 00 FF 00 04 50 00 FF 00 FF FF 00 0D 00 01 00 00 00 00 01 03 01 00 03 00 2D 19 C8 02 94 00 4A
// Rx: 60 00 FF 19 04 50 00 FF FF 39
void Heatsource::process_amExtraMessage(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, blockRemain_, 24);   // minutes
    has_update(telegram, blockRemainWw_, 25); // minutes
}

// Settings AM200

// pos 12: off(00)/Keelbypass(01)/(hc1pump(02) only standalone)
bool Heatsource::set_vr2Config(const char * value, const int8_t id) {
    uint8_t v;
    if (!Helpers::value2enum(value, v, FL_(enum_vr2Config))) {
        return false;
    }
    write_command(0x54C, 12, v, 0x54C);
    return true;
}

// pos 00: Alternate heat source activation: No(00),Yes(01)
bool Heatsource::set_ahsActivated(const char * value, const int8_t id) {
    bool v;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }
    write_command(0x54C, 0, v, 0x54C);
    return true;
}

// pos 04: Buffer primary pump->Config pump: No(00),Yes(01)
bool Heatsource::set_aPumpConfig(const char * value, const int8_t id) {
    bool v;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }
    write_command(0x54C, 4, v, 0x54C);
    return true;
}

// pos 03: Output for PR1 pump: On/Off(00),PWM(01),PWM invers(02)
bool Heatsource::set_aPumpSignal(const char * value, const int8_t id) {
    uint8_t v;
    if (!Helpers::value2enum(value, v, FL_(enum_aPumpSignal))) {
        return false;
    }
    write_command(0x54C, 3, v, 0x54C);
    return true;
}

// pos 21: Min output pump PR1 (%)
bool Heatsource::set_aPumpMin(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }
    write_command(0x54C, 21, v, 0x54C);
    return true;
}

// pos 01: AHS return temp rise: No(00),Yes(01) (mixer VR1)
bool Heatsource::set_tempRise(const char * value, const int8_t id) {
    bool v;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }
    write_command(0x54C, 1, v, 0x54C);
    return true;
}

// pos 06: Set temp return (°C) (VR1)
bool Heatsource::set_setReturnTemp(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2temperature(value, v)) {
        return false;
    }
    write_command(0x54C, 6, v, 0x54C);
    return true;
}

// pos 10/11?: Mixer run time (s) (VR1)
bool Heatsource::set_mixRuntime(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }
    uint8_t data[2] = {(uint8_t)(v >> 8), (uint8_t)v};
    write_command(0x54C, 8, data, 2, 0x54C);
    return true;
}

// pos 07: Set flow temp AHS (°C) (Buffer)
bool Heatsource::set_setFlowTemp(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }
    write_command(0x54C, 7, v, 0x54C);
    return true;
}

// pos 02: Puffer bypass: No(00), Mischer(01), Ventil(02) (Buffer)
bool Heatsource::set_bufBypass(const char * value, const int8_t id) {
    uint8_t v;
    if (!Helpers::value2enum(value, v, FL_(enum_bufBypass))) {
        return false;
    }
    write_command(0x54C, 2, v, 0x54C);
    return true;
}

// pos 8/9: Bypass mixer run time: [time] (s) (Buffer)
bool Heatsource::set_bufMixRuntime(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }
    uint8_t data[2] = {(uint8_t)(v >> 8), (uint8_t)v};
    write_command(0x54C, 10, data, 2, 0x54C);
    return true;
}

// pos 20: Konfig WW-Speicher Monovalent(01), Bivalent(02) (buffer)
bool Heatsource::set_bufConfig(const char * value, const int8_t id) {
    uint8_t v;
    if (!Helpers::value2enum(value, v, FL_(enum_bufConfig))) {
        return false;
    }
    write_command(0x54C, 20, v, 0x54C);
    return true;
}

// pos 16: Config htg. blocking mode: No(00),Automatic(01),Always block02) (blocking)
bool Heatsource::set_blockMode(const char * value, const int8_t id) {
    uint8_t v;
    if (!Helpers::value2enum(value, v, FL_(enum_blockMode))) {
        return false;
    }
    write_command(0x54C, 16, v, 0x54C);
    return true;
}

// pos 17: Config of block terminal: NO(00), NC(01)
bool Heatsource::set_blockTerm(const char * value, const int8_t id) {
    uint8_t v;
    if (!Helpers::value2enum(value, v, FL_(enum_blockTerm))) {
        return false;
    }
    write_command(0x54C, 17, v, 0x54C);
    return true;
}

// pos 14?: Hyst. for bolier block (K)
bool Heatsource::set_blockHyst(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2temperature(value, v, true)) {
        return false;
    }
    write_command(0x54C, 14, v, 0x54C);
    return true;
}

// pos 15: Boiler release wait time (min)
bool Heatsource::set_releaseWait(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }
    write_command(0x54C, 15, v, 0x54C);
    return true;
}

/*
bool Heatsource::set_valveBuffer(const char * value, const int8_t id) {
    uint8_t v;
    if (!Helpers::value2enum(value, v, FL_(enum_am200valve))) {
        return false;
    }
    write_command(0x54F, 3, v, 0x54F);
    return true;
}

bool Heatsource::set_valveReturn(const char * value, const int8_t id) {
    uint8_t v;
    if (!Helpers::value2enum(value, v, FL_(enum_am200valve))) {
        return false;
    }
    write_command(0x54F, 2, v, 0x54F);
    return true;
}
*/

} // namespace emsesp