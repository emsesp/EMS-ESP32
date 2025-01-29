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

#include "heatpump.h"

namespace emsesp {

REGISTER_FACTORY(Heatpump, EMSdevice::DeviceType::HEATPUMP);

Heatpump::Heatpump(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    // telegram handlers
    // register_telegram_type(0x042B, "HP1", false, MAKE_PF_CB(process_HPMonitor1));
    register_telegram_type(0x047B, "HP2", false, MAKE_PF_CB(process_HPMonitor2));

    register_telegram_type(0x998, "HPSettings", true, MAKE_PF_CB(process_HPSettings));
    register_telegram_type(0x999, "HPFunctionTest", true, MAKE_PF_CB(process_HPFunctionTest));
    register_telegram_type(0x9A0, "HPTemperature", false, MAKE_PF_CB(process_HPTemperature));
    register_telegram_type(0x99B, "HPFlowTemp", false, MAKE_PF_CB(process_HPFlowTemp));
    register_telegram_type(0x99C, "HPComp", false, MAKE_PF_CB(process_HPComp));
    register_telegram_type(0x4AE, "HPEnergy", true, MAKE_PF_CB(process_HpEnergy));
    register_telegram_type(0x4AF, "HPMeters", true, MAKE_PF_CB(process_HpMeters));
    register_telegram_type(0x99A, "HPStarts", false, MAKE_PF_CB(process_HpStarts));
    register_telegram_type(0x12E, "HPEnergy1", false, MAKE_PF_CB(process_HpEnergy1));
    register_telegram_type(0x13B, "HPEnergy2", false, MAKE_PF_CB(process_HpEnergy2));

    // device values
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &airHumidity_, DeviceValueType::UINT8, FL_(airHumidity), DeviceValueUOM::PERCENT);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &dewTemperature_, DeviceValueType::UINT8, FL_(dewTemperature), DeviceValueUOM::DEGREES);

    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &flowTemp_,
                          DeviceValueType::INT16,
                          DeviceValueNumOp::DV_NUMOP_DIV10,
                          FL_(curFlowTemp),
                          DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &retTemp_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(retTemp), DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &sysRetTemp_,
                          DeviceValueType::INT16,
                          DeviceValueNumOp::DV_NUMOP_DIV10,
                          FL_(sysRetTemp),
                          DeviceValueUOM::DEGREES);

    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpTa4_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(hpTa4), DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpTr1_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(hpTr1), DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpTr3_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(hpTr3), DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpTr4_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(hpTr4), DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpTr5_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(hpTr5), DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpTr6_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(hpTr6), DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpTl2_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(hpTl2), DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpJr0_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(hpPl1), DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpJr1_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(hpPh1), DeviceValueUOM::DEGREES);

    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &heatingPumpMod_, DeviceValueType::UINT8, FL_(heatingPumpMod), DeviceValueUOM::PERCENT);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpCompSpd_, DeviceValueType::UINT8, FL_(hpCompSpd), DeviceValueUOM::PERCENT);

    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &controlStrategy_,
                          DeviceValueType::ENUM,
                          FL_(enum_hybridStrategy1),
                          FL_(hybridStrategy),
                          DeviceValueUOM::NONE,
                          MAKE_CF_CB(set_controlStrategy));
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &lowNoiseMode_,
                          DeviceValueType::ENUM,
                          FL_(enum_lowNoiseMode),
                          FL_(lowNoiseMode),
                          DeviceValueUOM::NONE,
                          MAKE_CF_CB(set_lowNoiseMode));
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &lowNoiseStart_,
                          DeviceValueType::UINT8,
                          FL_(lowNoiseStart),
                          DeviceValueUOM::HOURS,
                          MAKE_CF_CB(set_lowNoiseStart),
                          0,
                          23);
    register_device_value(
        DeviceValueTAG::TAG_DEVICE_DATA, &lowNoiseStop_, DeviceValueType::UINT8, FL_(lowNoiseStop), DeviceValueUOM::HOURS, MAKE_CF_CB(set_lowNoiseStop), 0, 23);
    register_device_value(
        DeviceValueTAG::TAG_DHW1, &hybridDHW_, DeviceValueType::ENUM, FL_(enum_comfort2), FL_(hybridDHW), DeviceValueUOM::NONE, MAKE_CF_CB(set_hybridDHW));
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &energyPriceGas_,
                          DeviceValueType::UINT8,
                          FL_(energyPriceGas),
                          DeviceValueUOM::CTKWH,
                          MAKE_CF_CB(set_energyPriceGas));
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &energyPriceEl_,
                          DeviceValueType::UINT8,
                          FL_(energyPriceEl),
                          DeviceValueUOM::CTKWH,
                          MAKE_CF_CB(set_energyPriceEl));
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &energyPricePV_,
                          DeviceValueType::UINT8,
                          FL_(energyPricePV),
                          DeviceValueUOM::CTKWH,
                          MAKE_CF_CB(set_energyPricePV));
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &switchOverTemp_,
                          DeviceValueType::INT8,
                          FL_(switchOverTemp),
                          DeviceValueUOM::DEGREES,
                          MAKE_CF_CB(set_switchOverTemp));
    // Function test
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &airPurgeMode_,
                          DeviceValueType::BOOL,
                          FL_(airPurgeMode),
                          DeviceValueUOM::NONE,
                          MAKE_CF_CB(set_airPurgeMode));
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &heatPumpOutput_,
                          DeviceValueType::UINT8,
                          FL_(heatPumpOutput),
                          DeviceValueUOM::PERCENT,
                          MAKE_CF_CB(set_heatPumpOutput));
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &coolingCircuit_,
                          DeviceValueType::BOOL,
                          FL_(coolingCircuit),
                          DeviceValueUOM::NONE,
                          MAKE_CF_CB(set_coolingCircuit));
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &compStartMod_,
                          DeviceValueType::UINT8,
                          FL_(compStartMod),
                          DeviceValueUOM::PERCENT,
                          MAKE_CF_CB(set_compStartMod));
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &heatDrainPan_,
                          DeviceValueType::BOOL,
                          FL_(heatDrainPan),
                          DeviceValueUOM::NONE,
                          MAKE_CF_CB(set_heatDrainPan));
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &heatCable_, DeviceValueType::BOOL, FL_(heatCable), DeviceValueUOM::NONE, MAKE_CF_CB(set_heatCable));
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &nrgTotal_, DeviceValueType::UINT24, DeviceValueNumOp::DV_NUMOP_DIV100, FL_(nrgTotal), DeviceValueUOM::KWH);
    register_device_value(DeviceValueTAG::TAG_DHW1, &nrgWw_, DeviceValueType::UINT24, DeviceValueNumOp::DV_NUMOP_DIV100, FL_(nrgWw), DeviceValueUOM::KWH);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &nrgHeat_, DeviceValueType::UINT24, DeviceValueNumOp::DV_NUMOP_DIV100, FL_(nrgHeat), DeviceValueUOM::KWH);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &meterTotal_,
                          DeviceValueType::UINT24,
                          DeviceValueNumOp::DV_NUMOP_DIV100,
                          FL_(meterTotal),
                          DeviceValueUOM::KWH);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &meterComp_,
                          DeviceValueType::UINT24,
                          DeviceValueNumOp::DV_NUMOP_DIV100,
                          FL_(meterComp),
                          DeviceValueUOM::KWH);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &meterEHeat_,
                          DeviceValueType::UINT24,
                          DeviceValueNumOp::DV_NUMOP_DIV100,
                          FL_(meterEHeat),
                          DeviceValueUOM::KWH);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &meterHeat_,
                          DeviceValueType::UINT24,
                          DeviceValueNumOp::DV_NUMOP_DIV100,
                          FL_(meterHeat),
                          DeviceValueUOM::KWH);
    register_device_value(DeviceValueTAG::TAG_DHW1, &meterWw_, DeviceValueType::UINT24, DeviceValueNumOp::DV_NUMOP_DIV100, FL_(meterWw), DeviceValueUOM::KWH);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &heatStartsHp_, DeviceValueType::UINT24, FL_(heatingStarts), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DHW1, &wwStartsHp_, DeviceValueType::UINT24, FL_(wwStartsHp), DeviceValueUOM::NONE);

    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &fuelHeat_, DeviceValueType::UINT32, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(fuelHeat), DeviceValueUOM::KWH);
    register_device_value(DeviceValueTAG::TAG_DHW1, &fuelDhw_, DeviceValueType::UINT32, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(fuelDhw), DeviceValueUOM::KWH);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &elHeat_, DeviceValueType::UINT32, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(elHeat), DeviceValueUOM::KWH);
    register_device_value(DeviceValueTAG::TAG_DHW1, &elDhw_, DeviceValueType::UINT32, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(elDhw), DeviceValueUOM::KWH);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &elGenHeat_,
                          DeviceValueType::UINT32,
                          DeviceValueNumOp::DV_NUMOP_DIV10,
                          FL_(elGenHeat),
                          DeviceValueUOM::KWH);
    register_device_value(DeviceValueTAG::TAG_DHW1, &elGenDhw_, DeviceValueType::UINT32, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(elGenDhw), DeviceValueUOM::KWH);
}

/*
 * Type 0x47B - HeatPump Monitor 2
 * e.g. "38 10 FF 00 03 7B 08 24 00 4B"
 */
void Heatpump::process_HPMonitor2(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, dewTemperature_, 0);
    has_update(telegram, airHumidity_, 1);
}

/*
 * Type 0x42B- HeatPump Monitor 1
 * e.g. "38 10 FF 00 03 2B 00 D1 08 2A 01"
 */
void Heatpump::process_HPMonitor1(std::shared_ptr<const Telegram> telegram) {
    // still to implement
}

// 0x09A0
// Heatpump(0x53) -> All(0x00), ?(0x09A0), data: 02 23 01 3E 01 39 00 5D 01 DE 01 38 00 40 00 5E 00 58 00 3F 01 34 00 02
void Heatpump::process_HPTemperature(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, hpTc3_, 2);  // condenser temp.
    has_update(telegram, hpTr1_, 8);  // compressor temp.
    has_update(telegram, hpTr3_, 10); // cond. temp. heating
    has_update(telegram, hpTr4_, 12); // cond. temp. clg
    has_update(telegram, hpTr5_, 14); // suction line temp.
    has_update(telegram, hpTr6_, 0);  // hot gas temp.
    has_update(telegram, hpTl2_, 6);  // inlet air temperature
    has_update(telegram, hpTa4_, 16); // drain pan temp.
    has_update(telegram, hpJr0_, 18); // low pressure sensor
    has_update(telegram, hpJr1_, 20); // high pressure sensor
}

// 0x099B
// Heatpump(0x53) -> All(0x00), ?(0x099B), data: 80 00 80 00 01 3C 01 38 80 00 80 00 80 00 01 37 00 00 00 00 64
void Heatpump::process_HPFlowTemp(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, flowTemp_, 4);
    has_update(telegram, retTemp_, 6);
    has_update(telegram, sysRetTemp_, 14);
    has_update(telegram, heatingPumpMod_, 19);
}

// 0x0998 HPSettings
// [emsesp] Heatpump(0x53) -> Me(0x0B), ?(0x0998), data: 00 00 0B 00 00 1F 01 00 01 01 16 06 00 04 02 FF 00 01 7C 01
void Heatpump::process_HPSettings(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, controlStrategy_, 0);
    has_update(telegram, hybridDHW_, 1);
    has_update(telegram, energyPriceGas_, 2);
    has_update(telegram, energyPriceEl_, 5);
    has_update(telegram, energyPricePV_, 6);
    has_update(telegram, lowNoiseMode_, 9);
    has_update(telegram, lowNoiseStart_, 10);
    has_update(telegram, lowNoiseStop_, 11);
    has_update(telegram, switchOverTemp_, 14);
}

// 0x099C HPComp
// Broadcast (0x099C), data: 00 04 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 02 76 00 00
//                     data: 00 2B 00 03 04 13 00 00 00 00 00 02 02 02 (offset 24)
void Heatpump::process_HPComp(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, hpCompSpd_, 15);
}

// 0x999 HPFunctionTest
// HPFunctionTest(0x0999), data: 00 00 00 32 00 00 00 00 00 00 00
void Heatpump::process_HPFunctionTest(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, airPurgeMode_, 0);
    has_update(telegram, heatPumpOutput_, 2);
    has_update(telegram, coolingCircuit_, 6);
    has_update(telegram, compStartMod_, 7);
    has_update(telegram, heatDrainPan_, 9);
    has_update(telegram, heatCable_, 10);
}

// boiler(0x08) -W-> Me(0x0B), ?(0x04AE), data: 00 00 BD C4 00 00 5B 6A 00 00 00 24 00 00 62 59 00 00 00 00 00 00 00 00
// boiler(0x08) -W-> Me(0x0B), ?(0x04AE), data: 00 00 00 00 00 00 00 00 (offset 24)
void Heatpump::process_HpEnergy(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, nrgTotal_, 0);
    has_update(telegram, nrgHeat_, 4);
    has_update(telegram, nrgWw_, 12);
}

// boiler(0x08) -W-> Me(0x0B), ?(0x04AF), data: 00 00 48 B2 00 00 48 55 00 00 00 5D 00 00 01 78 00 00 00 00 00 00 07 61
// boiler(0x08) -W-> Me(0x0B), ?(0x04AF), data: 00 00 24 B0 00 00 00 12 00 00 23 A5 00 00 00 4B 00 00 00 00 00 00 00 00 (offset 24)
// boiler(0x08) -W-> Me(0x0B), ?(0x04AF), data: 00 00 00 00 00 00 00 00 (offset 48)
void Heatpump::process_HpMeters(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, meterTotal_, 0);
    has_update(telegram, meterComp_, 4);
    has_update(telegram, meterEHeat_, 8);
    has_update(telegram, meterHeat_, 24);
    has_update(telegram, meterWw_, 32);
}

// Broadcast (0x099A), data: 05 00 00 00 00 00 00 37 00 00 1D 00 00 52 00 00 13 01 00 01 7C
void Heatpump::process_HpStarts(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, heatStartsHp_, 11, 3);
    has_update(telegram, wwStartsHp_, 14, 3);
}

// 0x0112E energy consumption
void Heatpump::process_HpEnergy1(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, fuelHeat_, 3);
    has_update(telegram, fuelDhw_, 7);
    has_update(telegram, elHeat_, 11);
    has_update(telegram, elDhw_, 15);
}

// 0x013B energy generated
void Heatpump::process_HpEnergy2(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, elGenHeat_, 3);
    has_update(telegram, elGenDhw_, 7);
}


/*
 * Broadcast (0x099A), data: 05 00 00 00 00 00 00 37 00 00 1D 00 00 52 00 00 13 01 00 01 7C
 * Broadcast (0x099B), data: 80 00 80 00 01 3C 01 38 80 00 80 00 80 00 01 37 00 00 00 00 64
 * Broadcast (0x099C), data: 00 04 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 02 76 00 00
                       data: 00 2B 00 03 04 13 00 00 00 00 00 02 02 02 (offset 24)
 * Broadcast (0x099D), data: 02 02 00 00 02 00 01 38 00 00 63 03 E8 00 00 00 00
 * Broadcast (0x099E), data: 03
 * Broadcast (0x099F), data 05 4E 20 00 00 00 00 02 00 01 34 00 00 3F 00 00 5C 00 01 DD 00 01 37 00
                       data 00 3F 00 00 5D 00 00 57 00 02 23 00 01 3D 02 80 00 5A 01 86 01 68 00 00 C0 (offset 24)
                       data 00 00 00 00 (offset 49)
 * Broadcast (0x09A0), data: 02 23 01 3E 01 39 00 5D 01 DE 01 38 00 40 00 5E 00 58 00 3F 01 34 00 02
 * Broadcast (0x09A2), data: 00 00 00 00
 * Broadcast (0x09A3), data: 01 01 01 00 0A 01 0A 00 00 00 00 00 00 01 01 00 00 (offset 2)
 * Broadcast (0x09A4), data: 00 35 00 00 01 42 00 00 00 00 30 01 00 00 00 00 00 00 00 00 00 00 00 00 00
                       data: 02 (offset 25)
 * Broadcast (0x09A5), data: 00 00 00 00 00 00 00 00 0A 01 00 01 03 03 00 5A 00 00 00 00 00 00 00 00 14
                       data: 00 00 00 (offset 25)
 * Broadcast (0x09A6), data: 05 01 00 01 F4 1F 01 1F 01 01 03 00 3F 00 00 00 19 64 00 3C 00 01 01 19
                       data: 02 EC 01 19 00 (offset 24)
 * Broadcast (0x09A8), data: 01 18 01 00 01 17 00 06 00 00 86 06 00 2E 07 D0 00 00 00 45 00 04
 
 * Broadcast (0x04AA), data: 00 00
 
*/

bool Heatpump::set_controlStrategy(const char * value, const int8_t id) {
    uint8_t v;
    if (!Helpers::value2enum(value, v, FL_(enum_hybridStrategy1))) {
        return false;
    }
    write_command(0x998, 0, v, 0x998);
    return true;
}
bool Heatpump::set_lowNoiseMode(const char * value, const int8_t id) {
    uint8_t v;
    if (!Helpers::value2enum(value, v, FL_(enum_lowNoiseMode))) {
        return false;
    }
    write_command(0x998, 9, v, 0x998);
    return true;
}
bool Heatpump::set_lowNoiseStart(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v, 0, 23)) {
        return false;
    }
    write_command(0x998, 10, v, 0x998);
    return true;
}
bool Heatpump::set_lowNoiseStop(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v, 0, 23)) {
        return false;
    }
    write_command(0x998, 11, v, 0x998);
    return true;
}
bool Heatpump::set_hybridDHW(const char * value, const int8_t id) {
    uint8_t v;
    if (!Helpers::value2enum(value, v, FL_(enum_comfort2))) {
        return false;
    }
    write_command(0x998, 1, v, 0x998);
    return true;
}
bool Heatpump::set_energyPriceGas(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }
    write_command(0x998, 2, v, 0x998);
    return true;
}
bool Heatpump::set_energyPriceEl(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }
    write_command(0x998, 5, v, 0x998);
    return true;
}
bool Heatpump::set_energyPricePV(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }
    write_command(0x998, 6, v, 0x998);
    return true;
}
bool Heatpump::set_switchOverTemp(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }
    write_command(0x998, 14, v, 0x998);
    return true;
}
// Function tests:
bool Heatpump::set_airPurgeMode(const char * value, const int8_t id) {
    bool v;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }
    write_command(0x999, 0, v, 0x999);
    return true;
}
bool Heatpump::set_heatPumpOutput(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v, 0, 100)) {
        return false;
    }
    write_command(0x999, 2, v, 0x999);
    return true;
}
bool Heatpump::set_coolingCircuit(const char * value, const int8_t id) {
    bool v;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }
    write_command(0x999, 6, v, 0x999);
    return true;
}
bool Heatpump::set_compStartMod(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v, 0, 100)) {
        return false;
    }
    write_command(0x999, 7, v, 0x999);
    return true;
}
bool Heatpump::set_heatDrainPan(const char * value, const int8_t id) {
    bool v;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }
    write_command(0x999, 9, v, 0x999);
    return true;
}
bool Heatpump::set_heatCable(const char * value, const int8_t id) {
    bool v;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }
    write_command(0x999, 10, v, 0x999);
    return true;
}

} // namespace emsesp
