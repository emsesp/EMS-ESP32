/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2025  emsesp.org - proddy, MichaelDvP
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

#include "solar.h"

namespace emsesp {

REGISTER_FACTORY(Solar, EMSdevice::DeviceType::SOLAR);

uuid::log::Logger Solar::logger_{F_(solar), uuid::log::Facility::CONSOLE};

Solar::Solar(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    // telegram handlers
    if (flags == EMSdevice::EMS_DEVICE_FLAG_SM10) {
        register_telegram_type(0x97, "SM10Monitor", false, MAKE_PF_CB(process_SM10Monitor));
        register_telegram_type(0x96, "SM10Config", true, MAKE_PF_CB(process_SM10Config));
        EMSESP::send_read_request(0x97, device_id);
    }

    if (flags == EMSdevice::EMS_DEVICE_FLAG_SM100) {
        // F9 is not a telegram type, it's a flag for configure
        // register_telegram_type(0xF9, "ParamCfg", false, MAKE_PF_CB(process_SM100ParamCfg));
        register_telegram_type(0x0358, "SM100SystemConfig", true, MAKE_PF_CB(process_SM100SystemConfig));
        register_telegram_type(0x035A, "SM100CircuitConfig", true, MAKE_PF_CB(process_SM100CircuitConfig));
        register_telegram_type(0x035D, "SM100Circuit2Config", true, MAKE_PF_CB(process_SM100Circuit2Config));
        register_telegram_type(0x0362, "SM100Monitor", false, MAKE_PF_CB(process_SM100Monitor));
        register_telegram_type(0x0363, "SM100Monitor2", false, MAKE_PF_CB(process_SM100Monitor2));
        register_telegram_type(0x0366, "SM100Config", false, MAKE_PF_CB(process_SM100Config));
        register_telegram_type(0x0364, "SM100Status", false, MAKE_PF_CB(process_SM100Status));
        register_telegram_type(0x036A, "SM100Status2", false, MAKE_PF_CB(process_SM100Status2));
        register_telegram_type(0x0380, "SM100CollectorConfig", true, MAKE_PF_CB(process_SM100CollectorConfig));
        register_telegram_type(0x038E, "SM100Energy", true, MAKE_PF_CB(process_SM100Energy));
        register_telegram_type(0x0391, "SM100Time", true, MAKE_PF_CB(process_SM100Time));
        register_telegram_type(0x035F, "SM100Config1", true, MAKE_PF_CB(process_SM100Config1));
        register_telegram_type(0x035C, "SM100HeatAssist", true, MAKE_PF_CB(process_SM100HeatAssist));
        register_telegram_type(0x0361, "SM100Differential", true, MAKE_PF_CB(process_SM100Differential));
    }

    if (flags == EMSdevice::EMS_DEVICE_FLAG_ISM) {
        register_telegram_type(0x0103, "ISM1StatusMessage", true, MAKE_PF_CB(process_ISM1StatusMessage));
        register_telegram_type(0x0101, "ISM1Set", true, MAKE_PF_CB(process_ISM1Set));
        register_telegram_type(0x0104, "ISM2StatusMessage", false, MAKE_PF_CB(process_ISM2StatusMessage));
    }

    // device values...
    // common solar values for all modules (except dhw)
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &collectorTemp_,
                          DeviceValueType::INT16,
                          DeviceValueNumOp::DV_NUMOP_DIV10,
                          FL_(collectorTemp),
                          DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &cylBottomTemp_,
                          DeviceValueType::INT16,
                          DeviceValueNumOp::DV_NUMOP_DIV10,
                          FL_(cylBottomTemp),
                          DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &solarPump_, DeviceValueType::BOOL, FL_(solarPump), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &pumpWorkTime_, DeviceValueType::TIME, FL_(pumpWorkTime), DeviceValueUOM::MINUTES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &cylMaxTemp_, DeviceValueType::UINT8, FL_(cylMaxTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_cylMaxTemp));
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &collectorShutdown_, DeviceValueType::BOOL, FL_(collectorShutdown), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &cylHeated_, DeviceValueType::BOOL, FL_(cylHeated), DeviceValueUOM::NONE);

    // values per device flag
    if (flags == EMSdevice::EMS_DEVICE_FLAG_SM10) {
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &solarPumpMod_, DeviceValueType::UINT8, FL_(solarPumpMod), DeviceValueUOM::PERCENT);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &solarPumpMinMod_,
                              DeviceValueType::UINT8,
                              FL_(pumpMinMod),
                              DeviceValueUOM::PERCENT,
                              MAKE_CF_CB(set_PumpMinMod));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &solarPumpTurnonDiff_,
                              DeviceValueType::UINT8,
                              FL_(solarPumpTurnonDiff),
                              DeviceValueUOM::DEGREES_R,
                              MAKE_CF_CB(set_TurnonDiff));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &solarPumpTurnoffDiff_,
                              DeviceValueType::UINT8,
                              FL_(solarPumpTurnoffDiff),
                              DeviceValueUOM::DEGREES_R,
                              MAKE_CF_CB(set_TurnoffDiff));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &solarPower_, DeviceValueType::INT16, FL_(solarPower), DeviceValueUOM::W);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &energyLastHour_,
                              DeviceValueType::UINT24,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(energyLastHour),
                              DeviceValueUOM::WH);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &maxFlow_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(maxFlow),
                              DeviceValueUOM::LMIN,
                              MAKE_CF_CB(set_SM10MaxFlow));
        register_device_value(DeviceValueTAG::TAG_DHW1, &wwMinTemp_, DeviceValueType::UINT8, FL_(wwMinTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_wwMinTemp));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &solarIsEnabled_,
                              DeviceValueType::BOOL,
                              FL_(solarIsEnabled),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_solarEnabled));

        /*
        // unknown values for testing and logging. Used by MichaelDvP
        register_device_value(
            DeviceValueTAG::TAG_DEVICE_DATA, &setting3_, DeviceValueType::UINT8, FL_(setting3), DeviceValueUOM::NONE, MAKE_CF_CB(set_CollectorMaxTemp));
        register_device_value(
            DeviceValueTAG::TAG_DEVICE_DATA, &setting4_, DeviceValueType::UINT8, FL_(setting4), DeviceValueUOM::NONE, MAKE_CF_CB(set_CollectorMinTemp));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &data11_, DeviceValueType::UINT8, FL_(data11), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &data12_, DeviceValueType::UINT8, FL_(data12), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &data1_, DeviceValueType::UINT8, FL_(data1), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &data0_, DeviceValueType::UINT8, FL_(data0), DeviceValueUOM::NONE);
		*/
    }
    if (flags == EMSdevice::EMS_DEVICE_FLAG_ISM) {
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &cylMiddleTemp_,
                              DeviceValueType::INT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(cylMiddleTemp),
                              DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &retHeatAssist_,
                              DeviceValueType::INT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(retHeatAssist),
                              DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &m1Valve_, DeviceValueType::BOOL, FL_(m1Valve), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &energyLastHour_,
                              DeviceValueType::UINT24,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(energyLastHour),
                              DeviceValueUOM::WH);
    }

    if (flags == EMSdevice::EMS_DEVICE_FLAG_SM100) {
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &solarPumpMod_, DeviceValueType::UINT8, FL_(solarPumpMod), DeviceValueUOM::PERCENT);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &solarPumpMinMod_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_MUL5,
                              FL_(pumpMinMod),
                              DeviceValueUOM::PERCENT,
                              MAKE_CF_CB(set_PumpMinMod));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &solarPumpTurnonDiff_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(solarPumpTurnonDiff),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_TurnonDiff));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &solarPumpTurnoffDiff_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(solarPumpTurnoffDiff),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_TurnoffDiff));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &collector2Temp_,
                              DeviceValueType::INT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(collector2Temp),
                              DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &cylMiddleTemp_,
                              DeviceValueType::INT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(cylMiddleTemp),
                              DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &retHeatAssist_,
                              DeviceValueType::INT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(retHeatAssist),
                              DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &ts8_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(ts8), DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &ts16_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(ts16), DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &m1Valve_, DeviceValueType::BOOL, FL_(m1Valve), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &m1Power_, DeviceValueType::UINT8, FL_(m1Power), DeviceValueUOM::PERCENT);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &solarPump2_, DeviceValueType::BOOL, FL_(solarPump2), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &solarPump2Mod_, DeviceValueType::UINT8, FL_(solarPump2Mod), DeviceValueUOM::PERCENT);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &cylBottomTemp2_,
                              DeviceValueType::INT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(cyl2BottomTemp),
                              DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &cylBottomTemp3_,
                              DeviceValueType::INT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(cyl3BottomTemp),
                              DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &cylTopTemp_,
                              DeviceValueType::INT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(cylTopTemp),
                              DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &heatExchangerTemp_,
                              DeviceValueType::INT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(heatExchangerTemp),
                              DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &cylPumpMod_, DeviceValueType::UINT8, FL_(cylPumpMod), DeviceValueUOM::PERCENT);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &valveStatus_, DeviceValueType::BOOL, FL_(valveStatus), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &vs1Status_, DeviceValueType::BOOL, FL_(vs1Status), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &vs3Status_, DeviceValueType::BOOL, FL_(vs3Status), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &transferPump_, DeviceValueType::BOOL, FL_(transferPump), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &transferPumpMod_, DeviceValueType::UINT8, FL_(transferPumpMod), DeviceValueUOM::PERCENT);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &collectorMaxTemp_,
                              DeviceValueType::UINT8,
                              FL_(collectorMaxTemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_CollectorMaxTemp));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &collectorMinTemp_,
                              DeviceValueType::UINT8,
                              FL_(collectorMinTemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_CollectorMinTemp));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &energyLastHour_,
                              DeviceValueType::UINT24,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(energyLastHour),
                              DeviceValueUOM::WH);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &energyToday_, DeviceValueType::UINT24, FL_(energyToday), DeviceValueUOM::WH);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &energyTotal_,
                              DeviceValueType::UINT24,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(energyTotal),
                              DeviceValueUOM::KWH);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &pump2WorkTime_, DeviceValueType::TIME, FL_(pump2WorkTime), DeviceValueUOM::MINUTES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &m1WorkTime_, DeviceValueType::TIME, FL_(m1WorkTime), DeviceValueUOM::MINUTES);
        // register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &cyl2MaxTemp_, DeviceValueType::UINT8, nullptr, FL_(cyl2MaxTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_cyl2MaxTemp));

        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &heatTransferSystem_,
                              DeviceValueType::BOOL,
                              FL_(heatTransferSystem),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_heatTransferSystem));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &externalCyl_,
                              DeviceValueType::BOOL,
                              FL_(externalCyl),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_externalCyl));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &thermalDisinfect_,
                              DeviceValueType::BOOL,
                              FL_(thermalDisinfect),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_thermalDisinfect));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &heatMetering_,
                              DeviceValueType::BOOL,
                              FL_(heatMetering),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_heatMetering));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &solarIsEnabled_,
                              DeviceValueType::BOOL,
                              FL_(activated),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_solarEnabled));

        // telegram 0x035A
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &solarPumpMode_,
                              DeviceValueType::ENUM,
                              FL_(enum_solarmode),
                              FL_(solarPumpMode),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_solarMode));
        register_device_value( // pump kick for vacuum collector, 00=off
            DeviceValueTAG::TAG_DEVICE_DATA,
            &solarPumpKick_,
            DeviceValueType::BOOL,
            FL_(solarPumpKick),
            DeviceValueUOM::NONE,
            MAKE_CF_CB(set_solarPumpKick));
        register_device_value( // system does not use antifreeze, 00=off
            DeviceValueTAG::TAG_DEVICE_DATA,
            &plainWaterMode_,
            DeviceValueType::BOOL,
            FL_(plainWaterMode),
            DeviceValueUOM::NONE,
            MAKE_CF_CB(set_plainWaterMode));
        register_device_value( // double Match Flow, 00=off
            DeviceValueTAG::TAG_DEVICE_DATA,
            &doubleMatchFlow_,
            DeviceValueType::BOOL,
            FL_(doubleMatchFlow),
            DeviceValueUOM::NONE,
            MAKE_CF_CB(set_doubleMatchFlow));

        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &solarPump2MinMod_,
                              DeviceValueType::UINT8,
                              FL_(pump2MinMod),
                              DeviceValueUOM::PERCENT,
                              MAKE_CF_CB(set_Pump2MinMod));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &solarPump2TurnonDiff_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(solarPump2TurnonDiff),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_TurnonDiff2));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &solarPump2TurnoffDiff_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(solarPump2TurnoffDiff),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_TurnoffDiff2));
        register_device_value( // pump kick for vacuum collector, 00=off
            DeviceValueTAG::TAG_DEVICE_DATA,
            &solarPump2Kick_,
            DeviceValueType::BOOL,
            FL_(solarPump2Kick),
            DeviceValueUOM::NONE,
            MAKE_CF_CB(set_solarPump2Kick));

        // telegram 0x380
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &climateZone_,
                              DeviceValueType::UINT8,
                              FL_(climateZone),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_climateZone)); // climate zone identifier
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &collector1Area_,
                              DeviceValueType::UINT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(collector1Area),
                              DeviceValueUOM::SQM,
                              MAKE_CF_CB(set_collector1Area)); // Area of collector field 1
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &collector1Type_,
                              DeviceValueType::ENUM,
                              FL_(enum_collectortype),
                              FL_(collector1Type),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_collector1Type)); // Type of collector field 1, 01=flat, 02=vacuum
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &collector2Area_,
                              DeviceValueType::UINT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(collector2Area),
                              DeviceValueUOM::SQM,
                              MAKE_CF_CB(set_collector2Area)); // Area of collector field 2
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &collector2Type_,
                              DeviceValueType::ENUM,
                              FL_(enum_collectortype),
                              FL_(collector2Type),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_collector2Type)); // Type of collector field 2, 01=flat, 02=vacuum
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &cylPriority_,
                              DeviceValueType::ENUM,
                              FL_(enum_cylprio),
                              FL_(cylPriority),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_cylPriority));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &heatCntFlowTemp_,
                              DeviceValueType::UINT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(heatCntFlowTemp),
                              DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &heatCntRetTemp_,
                              DeviceValueType::UINT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(heatCntRetTemp),
                              DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &heatCnt_, DeviceValueType::UINT8, FL_(heatCnt), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &swapFlowTemp_,
                              DeviceValueType::UINT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(swapFlowTemp),
                              DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &swapRetTemp_,
                              DeviceValueType::UINT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(swapRetTemp),
                              DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &heatAssistOn_,
                              DeviceValueType::INT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(heatAssistOn),
                              DeviceValueUOM::K,
                              MAKE_CF_CB(set_solarHeatAssistOn));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &heatAssistOff_,
                              DeviceValueType::INT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(heatAssistOff),
                              DeviceValueUOM::K,
                              MAKE_CF_CB(set_solarHeatAssistOn));
    }
}

// SM10Monitor - type 0x96
// Solar(0x30) -> All(0x00), (0x96), data: FF 18 19 0A 02 5A 27 0A 05 2D 1E 0F 64 28 0A
void Solar::process_SM10Config(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, solarIsEnabled_, 0); // FF on
    has_update(telegram, setting3_, 3);
    has_update(telegram, setting4_, 4);
    /*
    uint8_t colmax = collectorMaxTemp_ / 10;
    telegram->read_value(colmax, 3);
    has_update(collectorMaxTemp_, colmax * 10);
    uint8_t colmin = collectorMinTemp_ / 10;
    telegram->read_value(colmin, 4);
    has_update(collectorMinTemp_, colmin * 10);
    */
    has_update(telegram, solarPumpMinMod_, 2);
    has_update(telegram, solarPumpTurnonDiff_, 7);
    has_update(telegram, solarPumpTurnoffDiff_, 8);
    has_update(telegram, cylMaxTemp_, 5);
    has_update(telegram, wwMinTemp_, 6);
}

// SM10Monitor - type 0x97
//  Solar(0x30) -> All(0x00), SM10Monitor(0x97), data: 00 00 00 22 00 00 D2 01 00 F6 2A 00 00
void Solar::process_SM10Monitor(std::shared_ptr<const Telegram> telegram) {
    uint8_t solarpumpmod = solarPumpMod_;

    has_update(telegram, data0_, 0);
    has_update(telegram, data1_, 1);
    has_update(telegram, data11_, 11);
    has_update(telegram, data12_, 12);

    has_bitupdate(telegram, collectorShutdown_, 0, 3); // collectorMaxTemp reached
    has_bitupdate(telegram, cylHeated_, 0, 2);         // cylMaxTemp reached
    has_update(telegram, collectorTemp_, 2);           // collector temp from SM10, is *10
    telegram->read_value(solarpumpmod, 4);             // modulation solar pump
    has_update(telegram, cylBottomTemp_, 5);           // cyl bottom temp from SM10, is *10
    has_bitupdate(telegram, solarPump_, 7, 1);         // pump onoff
    has_update(telegram, pumpWorkTime_, 8, 3);

    // mask out pump-boosts
    if (solarpumpmod == 100 && solarPumpMod_ == 0 && solarPumpMinMod_ > 0) {
        solarpumpmod = solarPumpMinMod_; // set to minimum
    }
    has_update(solarPumpMod_, solarpumpmod);

    if (!Helpers::hasValue(maxFlow_)) {
        EMSESP::webSettingsService.read([&](WebSettings & settings) { maxFlow_ = settings.solar_maxflow; });
        has_update(&maxFlow_);
    }

    // solar publishes every minute, do not count reads by other devices
    if (telegram->dest == 0) {
        // water 4.184 J/gK, glycol ~2.6-2.8 J/gK, no aceotrope
        // solarPower_ = (collectorTemp_ - cylBottomTemp_) * solarPumpModulation_ * maxFlow_ * 10 / 1434; // water
        solarPower_ = (collectorTemp_ - cylBottomTemp_) * solarpumpmod * maxFlow_ * 10 / 1665; //40% glycol@40°C
        if (energy.size() >= 60) {
            energy.pop_front();
        }
        energy.push_back(solarPower_);
        int32_t sum = 0;
        for (auto e : energy) {
            sum += e;
        }
        energyLastHour_ = sum > 0 ? sum / 6 : 0; // counts in 0.1 Wh
        has_update(&solarPower_);
        has_update(&energyLastHour_);
    }
}

/*
 * process_SM100SystemConfig - type 0x0358 EMS+ - for MS/SM100 and MS/SM200
 * e.g. B0 0B FF 00 02 58 FF 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 FF 00 FF 01 00 00
 * SM100SystemConfig(0x358), data: FF 00 FF 00 FF 00 00 00 00 00 00 FF 00 00 FF 00 00 00 00 FF 00 FF 01 01 00
 * SM100SystemConfig(0x358), data: 00 00 00 00 00 00 00 (offset 25)
 */
void Solar::process_SM100SystemConfig(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, heatTransferSystem_, 5, 1);
    has_update(telegram, externalCyl_, 9, 1);
    has_update(telegram, thermalDisinfect_, 10, 1);
    has_update(telegram, heatMetering_, 14, 1);
    has_update(telegram, solarIsEnabled_, 19, 1);
}

/*
 * process_SM100SolarCircuitConfig - type 0x035A EMS+ - for MS/SM100 and MS/SM200
 * e.g. B0 0B FF 00 02 5A 64 05 00 58 14 01 01 32 64 00 00 00 5A 0C
 */
void Solar::process_SM100CircuitConfig(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, collectorMaxTemp_, 0);
    has_update(telegram, cylMaxTemp_, 3);
    has_update(telegram, collectorMinTemp_, 4);
    has_update(telegram, solarPumpMode_, 5);
    has_update(telegram, solarPumpMinMod_, 6);      // is / 5
    has_update(telegram, solarPumpTurnoffDiff_, 7); // is * 10
    has_update(telegram, solarPumpTurnonDiff_, 8);  // is * 10
    has_update(telegram, solarPumpKick_, 9);
    has_update(telegram, plainWaterMode_, 10);
    has_update(telegram, doubleMatchFlow_, 11);
}

/*
 * process_SM100Solar2CircuitConfig - type 0x035D EMS+ - for MS/SM100 and MS/SM200
 */
void Solar::process_SM100Circuit2Config(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, solarPump2Kick_, 0);
    //has_update(telegram, solar2PumpTurnoffDiff_, 3); // is * 10
    has_update(telegram, solarPump2TurnonDiff_, 4); // is * 10
    /*
    has_update(telegram, collector2MaxTemp_, 0);
    has_update(telegram, cylMaxTemp2_, 3);
    has_update(telegram, collector2MinTemp_, 4);
    has_update(telegram, solar2PumpMode_, 5);
    has_update(telegram, solar2PumpMinMod_, 6);
    has_update(telegram, plainWaterMode2_, 10);
    has_update(telegram, doubleMatchFlow2_, 11);
    */
}

// type 0x35C Heat assistance
void Solar::process_SM100HeatAssist(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, heatAssistOn_, 0);  // is *10
    has_update(telegram, heatAssistOff_, 1); // is *10
}

// type 0x361 differential control
void Solar::process_SM100Differential(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, diffControl_, 0); // is *10
}

// process_SM100ParamCfg - type 0xF9 EMS 1.0
// This telegram is used to inquire the min, default, max, and current values of a value that is usually read and written with another telegram ID
// The CS200 uses this method extensively to find out which values may be set in the SM100
// e.g. B0 10 F9 00 FF 02 5A 03 17 00 00 00 14 00 00 00 3C 00 00 00 5A 00 00 00 59 29 - requested with 90 B0 F9 00 11 FF 02 5A 03 AF
// byte 0 = 0xFF
// byte 1-2 = telegram ID used to write this value
// byte 3 = offset in telegram used to write this value
// byte 4 = unknown
// bytes 5..8 = minimum value
// bytes 9..12 = default value
// bytes 13..16 = maximum value
// bytes 17..20 = current value
// e.g. B0 0B F9 00 00 02 5A 00 00 6E
void Solar::process_SM100ParamCfg(std::shared_ptr<const Telegram> telegram) {
    uint16_t t_id = EMS_VALUE_UINT16_NOTSET;
    uint8_t  of   = EMS_VALUE_UINT8_NOTSET;
    int32_t  min  = EMS_VALUE_UINT16_NOTSET;
    int32_t  def  = EMS_VALUE_UINT16_NOTSET;
    int32_t  max  = EMS_VALUE_UINT16_NOTSET;
    int32_t  cur  = EMS_VALUE_UINT16_NOTSET;
    telegram->read_value(t_id, 1);
    telegram->read_value(of, 3);
    telegram->read_value(min, 5);
    telegram->read_value(def, 9);
    telegram->read_value(max, 13);
    telegram->read_value(cur, 17);

    // LOG_DEBUG("SM100ParamCfg param=0x%04X, offset=%d, min=%d, default=%d, max=%d, current=%d", t_id, of, min, def, max, cur));
}

/*
 * SM100Monitor - type 0x0362 EMS+ - for MS/SM100 and MS/SM200
 * e.g. B0 0B FF 00 02 62 00 77 01 D4 80 00 80 00 80 00 80 00 80 00 80 00 80 00 80 00 00 F9 80 00 80 9E - for heat exchanger temp
 * e.g, 30 00 FF 00 02 62 01 AC
 *      30 00 FF 18 02 62 80 00
 *      30 00 FF 00 02 62 01 A1 - for cyl bottom temps
 * bytes 0+1 = TS1 Temperature sensor for collector
 * bytes 2+3 = TS2 Temperature sensor 1 cylinder, bottom
 * bytes 16+17 = TS5 Temperature sensor 2 cylinder, bottom, or swimming pool
 * bytes 20+21 = TS6 Temperature sensor external heat exchanger
 */
void Solar::process_SM100Monitor(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, collectorTemp_, 0);      // is *10 - TS1: Temperature sensor for collector array 1
    has_update(telegram, cylBottomTemp_, 2);      // is *10 - TS2: Temperature sensor 1 cylinder, bottom
    has_update(telegram, cylBottomTemp2_, 16);    // is *10 - TS5: Temperature sensor 2 cylinder, bottom, or swimming pool
    has_update(telegram, heatExchangerTemp_, 20); // is *10 - TS6: Heat exchanger temperature sensor

    has_update(telegram, collector2Temp_, 6);  // is *10 - TS7: Temperature sensor for collector array 2
    has_update(telegram, cylMiddleTemp_, 8);   // is *10 - TS14: cylinder middle temperature
    has_update(telegram, retHeatAssist_, 10);  // is *10 - TS15: return temperature heating assistance
    has_update(telegram, ts8_, 22);            // is *10 - TS8: ?
    has_update(telegram, cylBottomTemp3_, 24); // is *10 - TS5: Temperature sensor cylinder 3, bottom
}

// SM100Monitor2 - 0x0363 Heatcounter
// e.g. B0 00 FF 00 02 63 80 00 80 00 00 00 80 00 80 00 80 00 00 80 00 5A
// Solar(0x30) -> All(0x00), SM100Monitor2(0x363), data: 01 E1 01 6B 00 00 01 5D 02 8E 80 00 0F 80 00
void Solar::process_SM100Monitor2(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, heatCntFlowTemp_, 0); // is *10
    has_update(telegram, heatCntRetTemp_, 2);  // is *10
    has_update(telegram, heatCnt_, 12);
    has_update(telegram, swapRetTemp_, 6);  // is *10
    has_update(telegram, swapFlowTemp_, 8); // is *10
    has_update(telegram, cylTopTemp_, 10);  // is *10 - TS10: cylinder top temperature
    has_update(telegram, ts16_, 13);        // is *10 - TS16, see https://github.com/emsesp/EMS-ESP32/issues/2690
}

// SM100Config - 0x0366
// e.g. B0 00 FF 00 02 66     01 62 00 13 40 14
void Solar::process_SM100Config(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, availabilityFlag_, 0);
    has_update(telegram, configFlag_, 1);
    has_update(telegram, userFlag_, 2);
}

// SM100Config1 - 0x035F
// e.g. Solar(0x30) -> Me(0x0B), ?(0x35F), data: 00 00 41 01 1E 0A 0C 19 00 3C 19
void Solar::process_SM100Config1(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, cylPriority_, 3);
}

/*
 * SM100Status - type 0x0364 EMS+ for pump modulations - for MS/SM100 and MS/SM200
 - PS1: Solar circuit pump for collector array 1
 - PS5: Cylinder primary pump when using an external heat exchanger
 * e.g. 30 00 FF 09 02 64 64 = 100%
 * Solar(0x30) -> All(0x00), (0x364), data: 00 64 05 24 00 00 FF 00 00 05 00 14 3C 64 00 00 00 00
 */
void Solar::process_SM100Status(std::shared_ptr<const Telegram> telegram) {
    uint8_t solarpumpmod    = solarPumpMod_;
    uint8_t cylinderpumpmod = cylPumpMod_;
    telegram->read_value(cylinderpumpmod, 8);
    telegram->read_value(solarpumpmod, 9);

    // mask out boosts
    if (solarpumpmod == 100 && solarPumpMod_ == 0 && solarPumpMinMod_ > 0) {
        solarpumpmod = solarPumpMinMod_ * 5;
    }
    has_update(solarPumpMod_, solarpumpmod);
    if (cylinderpumpmod == 100 && cylPumpMod_ == 0 && solarPumpMinMod_ > 0) {
        cylinderpumpmod = solarPumpMinMod_ * 5;
    }
    has_update(cylPumpMod_, cylinderpumpmod);

    has_bitupdate(telegram, cylHeated_, 3, 1);         // issue #422
    has_bitupdate(telegram, collectorShutdown_, 3, 0); // collector shutdown
    has_update(telegram, m1Power_, 13);

    solarpumpmod = solarPump2Mod_;
    telegram->read_value(solarpumpmod, 4);
    // mask out boost
    if (solarpumpmod == 100 && solarPump2Mod_ == 0 && solarPumpMinMod_ > 0) {
        solarpumpmod = solarPumpMinMod_ * 5; // set to minimum
    }
    has_update(solarPump2Mod_, solarpumpmod);

    has_update(telegram, transferPumpMod_, 14);
}

/*
 * SM100Status2 - type 0x036A EMS+ for pump on/off at offset 0x0A - for SM100 and SM200
 * e.g. B0 00 FF 00 02 6A 03 03 03 03 01 03 03 03 03 03 01 03
 * byte 4 = VS2 3-way valve for cylinder 2 : test=01, on=04 and off=03
 * byte 10 = PS1 Solar circuit pump for collector array 1: test=b0001(1), on=b0100(4) and off=b0011(3)
 */
void Solar::process_SM100Status2(std::shared_ptr<const Telegram> telegram) {
    has_bitupdate(telegram, vs1Status_, 0, 2);     // on if bit 2 set
    has_bitupdate(telegram, valveStatus_, 4, 2);   // on if bit 2 set
    has_bitupdate(telegram, solarPump_, 10, 2);    // on if bit 2 set
    has_bitupdate(telegram, solarPump2_, 1, 2);    // on if bit 2 set
    has_bitupdate(telegram, m1Valve_, 7, 2);       // values 8/4 seen
    has_bitupdate(telegram, transferPump_, 11, 2); // #2212
}

/*
 * SM100CollectorConfig - type 0x0380 EMS+  - for SM100 and SM200
 * e.g. B0 0B FF 00 02 80 50 64 00 00 29 01 00 00 01
 * SM100CollectorConfig(0x380), data: 5A 3B 00 00 41 02 00 2D 02 (with 2 collectors)
 */
void Solar::process_SM100CollectorConfig(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, climateZone_, 0);
    has_update(telegram, collector1Area_, 3);
    // has_enumupdate(telegram, collector1Type_, 5, 1);
    // has_update(telegram, collector2Area_, 6);
    // do not show collector 2 if area is zero
    telegram->read_value(collector2Area_, 6);
    telegram->read_enumvalue(collector2Type_, 8, 1);
    if (collector2Area_ == 0) {
        collector2Area_ = EMS_VALUE_UINT16_NOTSET;
        collector2Type_ = EMS_VALUE_UINT8_NOTSET;
    }
    // has_enumupdate(telegram, collector2Type_, 8, 1);
}

/*
 * SM100Energy - type 0x038E EMS+ for energy readings
 * e.g. 30 00 FF 00 02 8E 00 00 00 00 00 00 06 C5 00 00 76 35
 * SM100Energy(0x38E), data: 00 00 01 79 00 00 22 3D 00 00 09 31 (with 2 collectors)
 */
void Solar::process_SM100Energy(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, energyLastHour_, 0); // last hour / 10 in Wh
    has_update(telegram, energyToday_, 4);    // todays in Wh
    has_update(telegram, energyTotal_, 8);    // total / 10 in kWh
}

/*
 * SM100Time - type 0x0391 EMS+ for pump working time
 * SM100Time(0x391), data: 00 00 2A 13 00 00 00 00 00 00 70 13 00 00 00 00 00 00 24 7E 00 00 00 00 00
 * SM100Time(0x391), data: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 12 4A 00 (offset 24)
 */
void Solar::process_SM100Time(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, pumpWorkTime_, 1, 3);
    // has_update(telegram, pumpXWorkTime_, 9, 3);
    has_update(telegram, pump2WorkTime_, 17, 3);
    has_update(telegram, m1WorkTime_, 45, 3);
}

/*
 * Junkers ISM1 Solar Module - type 0x0103 EMS+ for energy readings
 *  e.g. B0 00 FF 00 00 03 32 00 00 00 00 13 00 D6 00 00 00 FB D0 F0
 */
void Solar::process_ISM1StatusMessage(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, collectorTemp_, 4); // Collector Temperature
    has_update(telegram, cylBottomTemp_, 6); // Temperature Bottom of Solar Boiler cyl
    uint16_t Wh = energyLastHour_ / 10;
    telegram->read_value(Wh, 2); // Solar Energy produced in last hour only ushort, is not * 10
    if (energyLastHour_ != Wh * 10) {
        energyLastHour_ = Wh * 10;
        has_update(&energyLastHour_);
    }

    has_bitupdate(telegram, solarPump_, 8, 0);         // PS1 Solar pump on (1) or off (0)
    has_update(telegram, pumpWorkTime_, 10, 3);        // force to 3 bytes
    has_bitupdate(telegram, collectorShutdown_, 9, 0); // collector shutdown on/off
    has_bitupdate(telegram, cylHeated_, 9, 2);         // cyl full
}

/*
 * Junkers ISM12 Solar Module - type 0x0104 EMS+ for heat assist
 * ?(0x103), data: 00 00 00 00 00 7A 01 15 00 00 05 37 F0
 * ?(0x104), data: 01 A9 01 22 27 0F 27 0F 27 0F 27 0F 27 0F 27 0F
 * ?(0x104), data: 01 01 00 00 00 00 00 27 0F 27 0F (offset 16)
 */
void Solar::process_ISM2StatusMessage(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, cylMiddleTemp_, 0);  // Temperature Middle of Solar Boiler cyl
    has_update(telegram, retHeatAssist_, 2);  // return temperature from heating T4
    has_bitupdate(telegram, m1Valve_, 17, 0); // return valve DUW1 (also 16,0)
}

/*
 * Junkers ISM1 Solar Module - type 0x0101 EMS+ for setting values
 */
void Solar::process_ISM1Set(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, cylMaxTemp_, 6);
}

/*
 * Settings
 */

// collector shutdown temperature
bool Solar::set_CollectorMaxTemp(const char * value, const int8_t id) {
    int temperature;
    if (!Helpers::value2temperature(value, temperature)) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_SM10) {
        write_command(0x96, 3, (uint8_t)temperature, 0x96);
    } else {
        write_command(0x35A, 0, (uint8_t)temperature, 0x35A);
    }
    return true;
}

// collector shutdown temperature
bool Solar::set_CollectorMinTemp(const char * value, const int8_t id) {
    int temperature;
    if (!Helpers::value2temperature(value, temperature)) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_SM10) {
        write_command(0x96, 4, (uint8_t)temperature, 0x96);
    } else {
        write_command(0x35A, 4, (uint8_t)temperature, 0x35A);
    }
    return true;
}

// cylinder max temperature
bool Solar::set_cylMaxTemp(const char * value, const int8_t id) {
    int temperature;
    if (!Helpers::value2temperature(value, temperature)) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_SM10) {
        write_command(0x96, 5, (uint8_t)temperature, 0x96);
    } else if (flags() == EMSdevice::EMS_DEVICE_FLAG_ISM) {
        write_command(0x101, 6, (uint8_t)temperature, 0x101);
    } else {
        write_command(0x35A, 3, (uint8_t)temperature, 0x35A);
    }
    return true;
}

// solar pump minimum modulation
bool Solar::set_PumpMinMod(const char * value, const int8_t id) {
    int modulation;
    if (!Helpers::value2number(value, modulation)) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_SM10) {
        write_command(0x96, 2, (uint8_t)modulation, 0x96);
    } else {
        write_command(0x35A, 6, (uint8_t)(modulation + 2) / 5, 0x35A);
    }
    return true;
}

// solar pump 2 minimum modulation
bool Solar::set_Pump2MinMod(const char * value, const int8_t id) {
    int modulation;
    if (!Helpers::value2number(value, modulation)) {
        return false;
    }
    // write_command(0x35D, x, (uint8_t)(modulation), 0x35D);
    return true;
}

// warm water minimum temperature
bool Solar::set_wwMinTemp(const char * value, const int8_t id) {
    int temperature;
    if (!Helpers::value2temperature(value, temperature)) {
        return false;
    }
    write_command(0x96, 6, (uint8_t)temperature, 0x96);
    return true;
}

// turn on difference for solar pump
bool Solar::set_TurnoffDiff(const char * value, const int8_t id) {
    float temperature;
    if (!Helpers::value2temperature(value, temperature, true)) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_SM10) {
        write_command(0x96, 8, (uint8_t)temperature, 0x96);
    } else {
        write_command(0x35A, 7, (uint8_t)(temperature * 10), 0x35A);
    }
    return true;
}

// turn off difference for solar pump
bool Solar::set_TurnonDiff(const char * value, const int8_t id) {
    float temperature;
    if (!Helpers::value2temperature(value, temperature, true)) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_SM10) {
        write_command(0x96, 7, (uint8_t)temperature, 0x96);
    } else {
        write_command(0x35A, 8, (uint8_t)(temperature * 10), 0x35A);
    }
    return true;
}

// turn on difference for solar pump 2
bool Solar::set_TurnoffDiff2(const char * value, const int8_t id) {
    float temperature;
    if (!Helpers::value2temperature(value, temperature, true)) {
        return false;
    }
    write_command(0x361, 3, (uint8_t)(temperature * 10), 0x361);
    return true;
}

// turn off difference for solar pump 2
bool Solar::set_TurnonDiff2(const char * value, const int8_t id) {
    float temperature;
    if (!Helpers::value2temperature(value, temperature, true)) {
        return false;
    }
    write_command(0x361, 4, (uint8_t)(temperature * 10), 0x361);
    return true;
}

// external value to calculate energy
bool Solar::set_SM10MaxFlow(const char * value, const int8_t id) {
    float flow;
    if (!Helpers::value2float(value, flow)) {
        return false;
    }
    maxFlow_ = (flow * 10);
    EMSESP::webSettingsService.update([&](WebSettings & settings) {
        settings.solar_maxflow = maxFlow_;
        return StateUpdateResult::CHANGED;
    });
    return true;
}

// switch heat transfer system on/off
bool Solar::set_heatTransferSystem(const char * value, const int8_t id) {
    bool b;
    if (!Helpers::value2bool(value, b)) {
        return false;
    }
    write_command(0x358, 5, b ? 0x01 : 0x00, 0x358);
    return true;
}

// switch external cylinder on/off
bool Solar::set_externalCyl(const char * value, const int8_t id) {
    bool b;
    if (!Helpers::value2bool(value, b)) {
        return false;
    }
    write_command(0x358, 9, b ? 0x01 : 0x00, 0x358);
    return true;
}

// switch thermal disinfection on/off
bool Solar::set_thermalDisinfect(const char * value, const int8_t id) {
    bool b;
    if (!Helpers::value2bool(value, b)) {
        return false;
    }
    write_command(0x358, 10, b ? 0x01 : 0x00, 0x358);
    return true;
}

// switch heat metering on/off
bool Solar::set_heatMetering(const char * value, const int8_t id) {
    bool b;
    if (!Helpers::value2bool(value, b)) {
        return false;
    }
    write_command(0x358, 14, b ? 0x01 : 0x00, 0x358);
    return true;
}

// switch solar system on/off
bool Solar::set_solarEnabled(const char * value, const int8_t id) {
    bool b;
    if (!Helpers::value2bool(value, b)) {
        return false;
    }
    if (flags() == EMSdevice::EMS_DEVICE_FLAG_SM10) {
        write_command(0x96, 0, b ? 0xFF : 0x00, 0x96);
    } else {
        write_command(0x358, 19, b ? 0x01 : 0x00, 0x358);
    }
    return true;
}

// pump mode: constant, pwm or analog
bool Solar::set_solarMode(const char * value, const int8_t id) {
    uint8_t num;
    if (!Helpers::value2enum(value, num, FL_(enum_solarmode))) {
        return false;
    }
    write_command(0x35A, 5, num, 0x35A);
    return true;
}

/*/ pump mode: constant, pwm or analog
bool Solar::set_solarMode2(const char * value, const int8_t id) {
    uint8_t num;
    if (!Helpers::value2enum(value, num, FL_(enum_solarmode))) {
        return false;
    }
    write_command(0x35D, x, num, 0x35D);
    return true;
}
*/

// switch pumpkick on/off
bool Solar::set_solarPumpKick(const char * value, const int8_t id) {
    bool b;
    if (!Helpers::value2bool(value, b)) {
        return false;
    }
    write_command(0x35A, 9, b ? 0x01 : 0x00, 0x35A);
    return true;
}

// switch pump2kick on/off
bool Solar::set_solarPump2Kick(const char * value, const int8_t id) {
    bool b;
    if (!Helpers::value2bool(value, b)) {
        return false;
    }
    write_command(0x35D, 0, b ? 0x01 : 0x00, 0x35D);
    return true;
}

// switch plain water mode on/off
bool Solar::set_plainWaterMode(const char * value, const int8_t id) {
    bool b;
    if (!Helpers::value2bool(value, b)) {
        return false;
    }
    write_command(0x35A, 10, b ? 0x01 : 0x00, 0x35A);
    return true;
}

// switch double match flow on/off
bool Solar::set_doubleMatchFlow(const char * value, const int8_t id) {
    bool b;
    if (!Helpers::value2bool(value, b)) {
        return false;
    }
    write_command(0x35A, 11, b ? 0x01 : 0x00, 0x35A);
    return true;
}

// set climate zone number
bool Solar::set_climateZone(const char * value, const int8_t id) {
    int zone;
    if (!Helpers::value2number(value, zone)) {
        return false;
    }
    write_command(0x380, 0, zone, 0x380);
    return true;
}

// collector area in squaremeters
bool Solar::set_collector1Area(const char * value, const int8_t id) {
    float area;
    if (!Helpers::value2float(value, area)) {
        return false;
    }
    write_command(0x380, 3, (uint16_t)(area * 10), 0x380);
    return true;
}

// collector area in squaremeters
bool Solar::set_collector2Area(const char * value, const int8_t id) {
    float area;
    if (!Helpers::value2float(value, area)) {
        return false;
    }
    write_command(0x380, 6, (uint16_t)(area * 10), 0x380);
    return true;
}

// collector type flat/vacuum
bool Solar::set_collector1Type(const char * value, const int8_t id) {
    uint8_t num;
    if (!Helpers::value2enum(value, num, FL_(enum_collectortype))) {
        return false;
    }
    write_command(0x380, 5, num + 1, 0x380);
    return true;
}

// collector type flat/vacuum
bool Solar::set_collector2Type(const char * value, const int8_t id) {
    uint8_t num;
    if (!Helpers::value2enum(value, num, FL_(enum_collectortype))) {
        return false;
    }
    write_command(0x380, 8, num + 1, 0x380);
    return true;
}

// priority of cylinders if there are 2
bool Solar::set_cylPriority(const char * value, const int8_t id) {
    uint8_t num;
    if (!Helpers::value2enum(value, num, FL_(enum_cylprio))) {
        return false;
    }
    write_command(0x35F, 3, num, 0x35F);
    return true;
}

bool Solar::set_heatAssist(const char * value, const int8_t id) {
    float temperature;
    if (!Helpers::value2temperature(value, temperature)) {
        return false;
    }
    write_command(0x35C, 0, (uint8_t)(temperature * 10), 0x35C);
    return true;
}

bool Solar::set_diffControl(const char * value, const int8_t id) {
    float temperature;
    if (!Helpers::value2temperature(value, temperature)) {
        return false;
    }
    write_command(0x361, 4, (uint8_t)(temperature * 10), 0x361);
    return true;
}

bool Solar::set_solarHeatAssistOn(const char * value, const int8_t id) {
    float t;
    if (!Helpers::value2float(value, t)) {
        return false;
    }
    write_command(0x35C, 0, (uint8_t)(t * 10), 0x35C);
    return true;
}

bool Solar::set_solarHeatAssistOff(const char * value, const int8_t id) {
    float t;
    if (!Helpers::value2float(value, t)) {
        return false;
    }
    write_command(0x35C, 1, (uint8_t)(t * 10), 0x35C);
    return true;
}

} // namespace emsesp
