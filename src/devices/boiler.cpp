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

#include "boiler.h"

namespace emsesp {

REGISTER_FACTORY(Boiler, EMSdevice::DeviceType::BOILER)

uuid::log::Logger Boiler::logger_{F_(boiler), uuid::log::Facility::CONSOLE};

Boiler::Boiler(uint8_t device_type, int8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand)
    : EMSdevice(device_type, device_id, product_id, version, name, flags, brand) {
    // register values for master boiler/cascade module

    // the telegram handlers...
    // common for all boilers
    register_telegram_type(0xBF, "ErrorMessage", false, MAKE_PF_CB(process_ErrorMessage));
    register_telegram_type(0x10, "UBAErrorMessage1", false, MAKE_PF_CB(process_UBAErrorMessage));
    register_telegram_type(0x11, "UBAErrorMessage2", false, MAKE_PF_CB(process_UBAErrorMessage));
    register_telegram_type(0xC2, "UBAErrorMessage3", false, MAKE_PF_CB(process_UBAErrorMessage2));
    register_telegram_type(0x14, "UBATotalUptime", true, MAKE_PF_CB(process_UBATotalUptime));
    register_telegram_type(0x15, "UBAMaintenanceData", false, MAKE_PF_CB(process_UBAMaintenanceData));
    register_telegram_type(0x1C, "UBAMaintenanceStatus", false, MAKE_PF_CB(process_UBAMaintenanceStatus));

    // EMS1.0 and maybe EMS+?
    register_telegram_type(0x18, "UBAMonitorFast", false, MAKE_PF_CB(process_UBAMonitorFast));
    register_telegram_type(0x19, "UBAMonitorSlow", false, MAKE_PF_CB(process_UBAMonitorSlow));
    register_telegram_type(0x1A, "UBASetPoints", false, MAKE_PF_CB(process_UBASetPoints));
    register_telegram_type(0x35, "UBAFlags", false, MAKE_PF_CB(process_UBAFlags));

    // only EMS 1.0
    register_telegram_type(0x16, "UBAParameters", true, MAKE_PF_CB(process_UBAParameters));
    register_telegram_type(0x33, "UBAParameterWW", true, MAKE_PF_CB(process_UBAParameterWW));
    register_telegram_type(0x34, "UBAMonitorWW", false, MAKE_PF_CB(process_UBAMonitorWW));

    // not ems1.0, but HT3
    if (model() != EMSdevice::EMS_DEVICE_FLAG_EMS) {
        register_telegram_type(0x26, "UBASettingsWW", true, MAKE_PF_CB(process_UBASettingsWW));
        register_telegram_type(0x2A, "MC110Status", false, MAKE_PF_CB(process_MC110Status));
    }

    // only EMS+
    if (model() != EMSdevice::EMS_DEVICE_FLAG_EMS && model() != EMSdevice::EMS_DEVICE_FLAG_HT3 && model() != EMSdevice::EMS_DEVICE_FLAG_HYBRID) {
        register_telegram_type(0xD1, "UBAOutdoorTemp", false, MAKE_PF_CB(process_UBAOutdoorTemp));
        register_telegram_type(0xE3, "UBAMonitorSlowPlus2", false, MAKE_PF_CB(process_UBAMonitorSlowPlus2));
        register_telegram_type(0xE4, "UBAMonitorFastPlus", false, MAKE_PF_CB(process_UBAMonitorFastPlus));
        register_telegram_type(0xE5, "UBAMonitorSlowPlus", false, MAKE_PF_CB(process_UBAMonitorSlowPlus));
        register_telegram_type(0xE6, "UBAParametersPlus", true, MAKE_PF_CB(process_UBAParametersPlus));
        register_telegram_type(0xE9, "UBAMonitorWWPlus", false, MAKE_PF_CB(process_UBAMonitorWWPlus));
        register_telegram_type(0xEA, "UBAParameterWWPlus", true, MAKE_PF_CB(process_UBAParameterWWPlus));
        register_telegram_type(0x28, "WeatherComp", true, MAKE_PF_CB(process_WeatherComp));
    }

    if (model() == EMSdevice::EMS_DEVICE_FLAG_HEATPUMP) {
        register_telegram_type(0x494, "UBAEnergySupplied", false, MAKE_PF_CB(process_UBAEnergySupplied));
        register_telegram_type(0x495, "UBAInformation", false, MAKE_PF_CB(process_UBAInformation));
        register_telegram_type(0x48D, "HpPower", true, MAKE_PF_CB(process_HpPower));
        register_telegram_type(0x48F, "HpTemperatures", false, MAKE_PF_CB(process_HpTemperatures));
        register_telegram_type(0x48A, "HpPool", true, MAKE_PF_CB(process_HpPool));
        register_telegram_type(0x4A2, "HpInput", true, MAKE_PF_CB(process_HpInput));
        register_telegram_type(0x485, "HpCooling", true, MAKE_PF_CB(process_HpCooling));
        register_telegram_type(0x486, "HpInConfig", true, MAKE_PF_CB(process_HpInConfig));

        register_telegram_type(0x492, "HpHeaterConfig", true, MAKE_PF_CB(process_HpHeaterConfig));
        register_telegram_type(0x488, "HPValve", true, MAKE_PF_CB(process_HpValve));
        register_telegram_type(0x484, "HPSilentMode", true, MAKE_PF_CB(process_HpSilentMode));
        register_telegram_type(0x48B, "HPPumps", true, MAKE_PF_CB(process_HpPumps));
        register_telegram_type(0x491, "HPAdditionalHeater", true, MAKE_PF_CB(process_HpAdditionalHeater));
        register_telegram_type(0x499, "HPDhwSettings", true, MAKE_PF_CB(process_HpDhwSettings));
        register_telegram_type(0x49C, "HPSettings2", true, MAKE_PF_CB(process_HpSettings2));
        register_telegram_type(0x49D, "HPSettings3", true, MAKE_PF_CB(process_HpSettings3));
        register_telegram_type(0x4AE, "HPEnergy", true, MAKE_PF_CB(process_HpEnergy));
        register_telegram_type(0x4AF, "HPMeters", true, MAKE_PF_CB(process_HpMeters));
        register_telegram_type(0x2CC, "HPPressure", true, MAKE_PF_CB(process_HpPressure));
        register_telegram_type(0x4A5, "HPFan", true, MAKE_PF_CB(process_HpFan));
        register_telegram_type(0x4AA, "HPPower2", true, MAKE_PF_CB(process_HpPower2));
        register_telegram_type(0x4A7, "HPPowerLimit", true, MAKE_PF_CB(process_HpPowerLimit));
        register_telegram_type(0x2D6, "HPPump2", true, MAKE_PF_CB(process_HpPump2));
    }

    // some gas boilers, see #1701
    if (model() != EMSdevice::EMS_DEVICE_FLAG_HEATPUMP) {
        register_telegram_type(0x2E, "Meters", false, MAKE_PF_CB(process_Meters));
        register_telegram_type(0x3B, "Energy", false, MAKE_PF_CB(process_Energy));
    }

    if (model() == EMSdevice::EMS_DEVICE_FLAG_HIU) {
        register_telegram_type(0x772, "HIUSettings", false, MAKE_PF_CB(process_HIUSettings));
        register_telegram_type(0x779, "HIUMonitor", false, MAKE_PF_CB(process_HIUMonitor));

        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &netFlowTemp_,
                              DeviceValueType::UINT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(netFlowTemp),
                              DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &heatValve_, DeviceValueType::UINT8, FL_(heatValve), DeviceValueUOM::PERCENT);
        register_device_value(DeviceValueTAG::TAG_DHW1, &wwValve_, DeviceValueType::UINT8, FL_(wwValve), DeviceValueUOM::PERCENT);
        // register_device_value(DeviceValueTAG::TAG_DHW1, &wwCurFlow_, DeviceValueType::UINT8, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(wwCurFlow), DeviceValueUOM::LMIN);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &keepWarmTemp_,
                              DeviceValueType::UINT8,
                              FL_(keepWarmTemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_keepWarmTemp));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &setReturnTemp_,
                              DeviceValueType::UINT8,
                              FL_(setReturnTemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_returnTemp));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &burnGas_, DeviceValueType::BOOL, FL_(heatingOn), DeviceValueUOM::NONE);
    }

    /*
    * Hybrid heatpump with telegram 0xBB is readable and writeable in boiler and thermostat
    * thermostat always overwrites settings in boiler
    * enable settings here if no thermostat is used in system
    *
    if (model() == EMSdevice::EMS_DEVICE_FLAG_HYBRID) {
        register_telegram_type(0xBB, "HybridHp", true, MAKE_PF_CB(process_HybridHp));
    }
    */

    // reset is a command uses a dummy variable which is always zero, shown as blank, but provides command enum options
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &reset_, DeviceValueType::CMD, FL_(enum_reset), FL_(reset), DeviceValueUOM::NONE, MAKE_CF_CB(set_reset));
    has_update(reset_, 0);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &forceHeatingOff_,
                          DeviceValueType::BOOL,
                          FL_(forceHeatingOff),
                          DeviceValueUOM::NONE,
                          MAKE_CF_CB(set_forceHeatingOff));
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &heatingActive_, DeviceValueType::BOOL, FL_(heatingActive), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &tapwaterActive_, DeviceValueType::BOOL, FL_(tapwaterActive), DeviceValueUOM::NONE);
    register_device_value(
        DeviceValueTAG::TAG_DEVICE_DATA, &selFlowTemp_, DeviceValueType::UINT8, FL_(selFlowTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_flow_temp), 0, 90);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &heatingPumpMod_, DeviceValueType::UINT8, FL_(heatingPumpMod), DeviceValueUOM::PERCENT);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &outdoorTemp_,
                          DeviceValueType::INT16,
                          DeviceValueNumOp::DV_NUMOP_DIV10,
                          FL_(outdoorTemp),
                          DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &curFlowTemp_,
                          DeviceValueType::UINT16,
                          DeviceValueNumOp::DV_NUMOP_DIV10,
                          FL_(curFlowTemp),
                          DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &retTemp_, DeviceValueType::UINT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(retTemp), DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &switchTemp_,
                          DeviceValueType::UINT16,
                          DeviceValueNumOp::DV_NUMOP_DIV10,
                          FL_(switchTemp),
                          DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &sysPress_, DeviceValueType::UINT8, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(sysPress), DeviceValueUOM::BAR);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &boilTemp_,
                          DeviceValueType::UINT16,
                          DeviceValueNumOp::DV_NUMOP_DIV10,
                          FL_(boilTemp),
                          DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &headertemp_,
                          DeviceValueType::UINT16,
                          DeviceValueNumOp::DV_NUMOP_DIV10,
                          FL_(headertemp),
                          DeviceValueUOM::DEGREES);

    // exclude burner related entities from heatpump and HIU
    if (model() != EMSdevice::EMS_DEVICE_FLAG_HEATPUMP && model() != EMSdevice::EMS_DEVICE_FLAG_HIU) {
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &exhaustTemp_,
                              DeviceValueType::UINT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(exhaustTemp),
                              DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &burnGas_, DeviceValueType::BOOL, FL_(burnGas), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &burnGas2_, DeviceValueType::BOOL, FL_(burnGas2), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &flameCurr_,
                              DeviceValueType::UINT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(flameCurr),
                              DeviceValueUOM::UA);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &fanWork_, DeviceValueType::BOOL, FL_(fanWork), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &ignWork_, DeviceValueType::BOOL, FL_(ignWork), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &oilPreHeat_, DeviceValueType::BOOL, FL_(oilPreHeat), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &burnMinPower_,
                              DeviceValueType::UINT8,
                              FL_(burnMinPower),
                              DeviceValueUOM::PERCENT,
                              MAKE_CF_CB(set_min_power));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &burnMaxPower_,
                              DeviceValueType::UINT8,
                              FL_(burnMaxPower),
                              DeviceValueUOM::PERCENT,
                              MAKE_CF_CB(set_max_power),
                              0,
                              254);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &burnMinPeriod_,
                              DeviceValueType::UINT8,
                              FL_(burnMinPeriod),
                              DeviceValueUOM::MINUTES,
                              MAKE_CF_CB(set_burn_period),
                              0,
                              120);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &absBurnPow_, DeviceValueType::UINT8, FL_(absBurnPow), DeviceValueUOM::PERCENT);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &heatblock_,
                              DeviceValueType::UINT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(heatblock),
                              DeviceValueUOM::DEGREES);
        register_device_value(
            DeviceValueTAG::TAG_DEVICE_DATA, &boilHystOn_, DeviceValueType::INT8, FL_(boilHystOn), DeviceValueUOM::DEGREES_R, MAKE_CF_CB(set_hyst_on), -20, 0);
        register_device_value(
            DeviceValueTAG::TAG_DEVICE_DATA, &boilHystOff_, DeviceValueType::INT8, FL_(boilHystOff), DeviceValueUOM::DEGREES_R, MAKE_CF_CB(set_hyst_off), 0, 20);
        register_device_value(
            DeviceValueTAG::TAG_DEVICE_DATA, &boil2HystOn_, DeviceValueType::INT8, FL_(boil2HystOn), DeviceValueUOM::DEGREES_R, MAKE_CF_CB(set_hyst2_on), -20, 0);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &boil2HystOff_,
                              DeviceValueType::INT8,
                              FL_(boil2HystOff),
                              DeviceValueUOM::DEGREES_R,
                              MAKE_CF_CB(set_hyst2_off),
                              0,
                              20);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &curveOn_, DeviceValueType::BOOL, FL_(curveOn), DeviceValueUOM::NONE, MAKE_CF_CB(set_curveOn));
        register_device_value(
            DeviceValueTAG::TAG_DEVICE_DATA, &curveBase_, DeviceValueType::UINT8, FL_(curveBase), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_curveBase), 20, 90);
        register_device_value(
            DeviceValueTAG::TAG_DEVICE_DATA, &curveEnd_, DeviceValueType::UINT8, FL_(curveEnd), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_curveEnd), 20, 90);
        register_device_value(
            DeviceValueTAG::TAG_DEVICE_DATA, &summerTemp_, DeviceValueType::UINT8, FL_(summertemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_summerTemp), 0, 45);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &nofrost_, DeviceValueType::BOOL, FL_(nofrostmode), DeviceValueUOM::NONE, MAKE_CF_CB(set_nofrost));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &nofrostTemp_,
                              DeviceValueType::UINT8,
                              FL_(nofrosttemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_nofrostTemp),
                              0,
                              10);
    }
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &heatingActivated_,
                          DeviceValueType::BOOL,
                          FL_(heatingActivated),
                          DeviceValueUOM::NONE,
                          MAKE_CF_CB(set_heating_activated));
    register_device_value(
        DeviceValueTAG::TAG_DEVICE_DATA, &heatingTemp_, DeviceValueType::UINT8, FL_(heatingTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_heating_temp), 0, 90);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &heatingPump_, DeviceValueType::BOOL, FL_(heatingPump), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &pumpModMax_, DeviceValueType::UINT8, FL_(pumpModMax), DeviceValueUOM::PERCENT, MAKE_CF_CB(set_max_pump));
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &pumpModMin_, DeviceValueType::UINT8, FL_(pumpModMin), DeviceValueUOM::PERCENT, MAKE_CF_CB(set_min_pump));
    register_device_value(
        DeviceValueTAG::TAG_DEVICE_DATA, &pumpMode_, DeviceValueType::ENUM, FL_(enum_pumpMode), FL_(pumpMode), DeviceValueUOM::NONE, MAKE_CF_CB(set_pumpMode));
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &pumpCharacter_,
                          DeviceValueType::ENUM,
                          FL_(enum_pumpCharacter),
                          FL_(pumpCharacter),
                          DeviceValueUOM::NONE,
                          MAKE_CF_CB(set_pumpCharacter));
    register_device_value(
        DeviceValueTAG::TAG_DEVICE_DATA, &pumpDelay_, DeviceValueType::UINT8, FL_(pumpDelay), DeviceValueUOM::MINUTES, MAKE_CF_CB(set_pump_delay), 0, 60);
    register_device_value(
        DeviceValueTAG::TAG_DEVICE_DATA, &pumpOnTemp_, DeviceValueType::UINT8, FL_(pumpOnTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_pumpOnTemp), 0, 60);
    register_device_value(
        DeviceValueTAG::TAG_DEVICE_DATA, &selBurnPow_, DeviceValueType::UINT8, FL_(selBurnPow), DeviceValueUOM::PERCENT, MAKE_CF_CB(set_burn_power), 0, 254);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &curBurnPow_, DeviceValueType::UINT8, FL_(curBurnPow), DeviceValueUOM::PERCENT);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &burnStarts_, DeviceValueType::UINT24, FL_(burnStarts), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &burnWorkMin_, DeviceValueType::TIME, FL_(burnWorkMin), DeviceValueUOM::MINUTES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &burn2WorkMin_, DeviceValueType::TIME, FL_(burn2WorkMin), DeviceValueUOM::MINUTES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &heatWorkMin_, DeviceValueType::TIME, FL_(heatWorkMin), DeviceValueUOM::MINUTES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &heatStarts_, DeviceValueType::UINT24, FL_(heatStarts), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &UBAuptime_, DeviceValueType::TIME, FL_(UBAuptime), DeviceValueUOM::MINUTES);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &lastCode_, DeviceValueType::STRING, FL_(lastCode), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &serviceCode_, DeviceValueType::STRING, FL_(serviceCode), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &serviceCodeNumber_, DeviceValueType::UINT16, FL_(serviceCodeNumber), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &maintenanceMessage_, DeviceValueType::STRING, FL_(maintenanceMessage), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &maintenanceType_,
                          DeviceValueType::ENUM,
                          FL_(enum_off_time_date_manual),
                          FL_(maintenanceType),
                          DeviceValueUOM::NONE,
                          MAKE_CF_CB(set_maintenance));
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &maintenanceTime_,
                          DeviceValueType::UINT16,
                          FL_(maintenanceTime),
                          DeviceValueUOM::HOURS,
                          MAKE_CF_CB(set_maintenancetime));
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &maintenanceDate_,
                          DeviceValueType::STRING,
                          FL_(tpl_date),
                          FL_(maintenanceDate),
                          DeviceValueUOM::NONE,
                          MAKE_CF_CB(set_maintenancedate));
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &emergencyOps_,
                          DeviceValueType::BOOL,
                          FL_(emergencyOps),
                          DeviceValueUOM::NONE,
                          MAKE_CF_CB(set_emergency_ops));
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                          &emergencyTemp_,
                          DeviceValueType::UINT8,
                          FL_(emergencyTemp),
                          DeviceValueUOM::DEGREES,
                          MAKE_CF_CB(set_emergency_temp),
                          15,
                          70);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &pc0Flow_, DeviceValueType::INT16, FL_(pc0Flow), DeviceValueUOM::LH);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &pc1Flow_, DeviceValueType::INT16, FL_(pc1Flow), DeviceValueUOM::LH);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &pc1On_, DeviceValueType::BOOL, FL_(pc1On), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &pc1Rate_, DeviceValueType::UINT8, FL_(pc1Rate), DeviceValueUOM::PERCENT);

    /*
    * Hybrid heatpump with telegram 0xBB is readable and writeable in boiler and thermostat
    * thermostat always overwrites settings in boiler
    * enable settings here if no thermostat is used in system
    *
    // Hybrid Heatpump
    if (model() == EMSdevice::EMS_DEVICE_FLAG_HYBRID) {
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &hybridStrategy_,
                              DeviceValueType::ENUM,
                              FL_(enum_hybridStrategy),
                              FL_(hybridStrategy),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_hybridStrategy));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &switchOverTemp_,
                              DeviceValueType::INT8,
                              nullptr,
                              FL_(switchOverTemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_switchOverTemp),
                              -20,
                              20);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &energyCostRatio_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(energyCostRatio),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_energyCostRatio),
                              0,
                              20);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &fossileFactor_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(fossileFactor),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_fossileFactor),
                              0,
                              5);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &electricFactor_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(electricFactor),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_electricFactor),
                              0,
                              5);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &delayBoiler_,
                              DeviceValueType::UINT8,
                              nullptr,
                              FL_(delayBoiler),
                              DeviceValueUOM::MINUTES,
                              MAKE_CF_CB(set_delayBoiler),
                              5,
                              120);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &tempDiffBoiler_,
                              DeviceValueType::UINT8,
                              nullptr,
                              FL_(tempDiffBoiler),
                              DeviceValueUOM::DEGREES_R,
                              MAKE_CF_CB(set_tempDiffBoiler),
                              1,
                              99);
    }
    */
    if (model() != EMSdevice::EMS_DEVICE_FLAG_HEATPUMP) {
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &meterHeat_,
                              DeviceValueType::UINT24,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(meterHeat),
                              DeviceValueUOM::KWH);
        register_device_value(DeviceValueTAG::TAG_DHW1, &meterWw_, DeviceValueType::UINT24, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(meterWw), DeviceValueUOM::KWH);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &gasMeterHeat_,
                              DeviceValueType::UINT24,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(gasMeterHeat),
                              DeviceValueUOM::KWH);
        register_device_value(DeviceValueTAG::TAG_DHW1, &gasMeterWw_, DeviceValueType::UINT24, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(gasMeterWw), DeviceValueUOM::KWH);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &nrgHeat2_,
                              DeviceValueType::UINT24,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(nrgHeat2),
                              DeviceValueUOM::KWH);
        register_device_value(DeviceValueTAG::TAG_DHW1, &nrgWw2_, DeviceValueType::UINT24, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(nrgWw2), DeviceValueUOM::KWH);
    }

    // heatpump info
    if (model() == EMSdevice::EMS_DEVICE_FLAG_HEATPUMP) {
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &nrgTotal_,
                              DeviceValueType::UINT24,
                              DeviceValueNumOp::DV_NUMOP_DIV100,
                              FL_(nrgTotal),
                              DeviceValueUOM::KWH);
        register_device_value(DeviceValueTAG::TAG_DHW1, &nrgWw_, DeviceValueType::UINT24, DeviceValueNumOp::DV_NUMOP_DIV100, FL_(nrgWw), DeviceValueUOM::KWH);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &nrgHeat_,
                              DeviceValueType::UINT24,
                              DeviceValueNumOp::DV_NUMOP_DIV100,
                              FL_(nrgHeat),
                              DeviceValueUOM::KWH);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &nrgCool_,
                              DeviceValueType::UINT24,
                              DeviceValueNumOp::DV_NUMOP_DIV100,
                              FL_(nrgCool),
                              DeviceValueUOM::KWH);
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
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &meterCool_,
                              DeviceValueType::UINT24,
                              DeviceValueNumOp::DV_NUMOP_DIV100,
                              FL_(meterCool),
                              DeviceValueUOM::KWH);
        register_device_value(DeviceValueTAG::TAG_DHW1, &meterWw_, DeviceValueType::UINT24, DeviceValueNumOp::DV_NUMOP_DIV100, FL_(meterWw), DeviceValueUOM::KWH);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &upTimeTotal_,
                              DeviceValueType::TIME,
                              DeviceValueNumOp::DV_NUMOP_DIV60,
                              FL_(upTimeTotal),
                              DeviceValueUOM::MINUTES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &upTimeControl_,
                              DeviceValueType::TIME,
                              DeviceValueNumOp::DV_NUMOP_DIV60,
                              FL_(upTimeControl),
                              DeviceValueUOM::MINUTES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &upTimeCompHeating_,
                              DeviceValueType::TIME,
                              DeviceValueNumOp::DV_NUMOP_DIV60,
                              FL_(upTimeCompHeating),
                              DeviceValueUOM::MINUTES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &upTimeCompCooling_,
                              DeviceValueType::TIME,
                              DeviceValueNumOp::DV_NUMOP_DIV60,
                              FL_(upTimeCompCooling),
                              DeviceValueUOM::MINUTES);
        register_device_value(DeviceValueTAG::TAG_DHW1,
                              &upTimeCompWw_,
                              DeviceValueType::TIME,
                              DeviceValueNumOp::DV_NUMOP_DIV60,
                              FL_(upTimeCompWw),
                              DeviceValueUOM::MINUTES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &upTimeCompPool_,
                              DeviceValueType::TIME,
                              DeviceValueNumOp::DV_NUMOP_DIV60,
                              FL_(upTimeCompPool),
                              DeviceValueUOM::MINUTES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &totalCompStarts_, DeviceValueType::UINT24, FL_(totalCompStarts), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &heatingStarts_, DeviceValueType::UINT24, FL_(heatingStarts), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &coolingStarts_, DeviceValueType::UINT24, FL_(coolingStarts), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DHW1, &wwStartsHp_, DeviceValueType::UINT24, FL_(wwStartsHp), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &poolStarts_, DeviceValueType::UINT24, FL_(poolStarts), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &nrgConsTotal_, DeviceValueType::UINT24, FL_(nrgConsTotal), DeviceValueUOM::KWH);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &nrgConsCompTotal_, DeviceValueType::UINT24, FL_(nrgConsCompTotal), DeviceValueUOM::KWH);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &nrgConsCompHeating_, DeviceValueType::UINT24, FL_(nrgConsCompHeating), DeviceValueUOM::KWH);
        register_device_value(DeviceValueTAG::TAG_DHW1, &nrgConsCompWw_, DeviceValueType::UINT24, FL_(nrgConsCompWw), DeviceValueUOM::KWH);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &nrgConsCompCooling_, DeviceValueType::UINT24, FL_(nrgConsCompCooling), DeviceValueUOM::KWH);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &nrgConsCompPool_, DeviceValueType::UINT24, FL_(nrgConsCompPool), DeviceValueUOM::KWH);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &auxElecHeatNrgConsTotal_, DeviceValueType::UINT24, FL_(auxElecHeatNrgConsTotal), DeviceValueUOM::KWH);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &auxElecHeatNrgConsHeating_,
                              DeviceValueType::UINT24,
                              FL_(auxElecHeatNrgConsHeating),
                              DeviceValueUOM::KWH);
        register_device_value(DeviceValueTAG::TAG_DHW1, &auxElecHeatNrgConsWw_, DeviceValueType::UINT24, FL_(auxElecHeatNrgConsWw), DeviceValueUOM::KWH);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &auxElecHeatNrgConsPool_, DeviceValueType::UINT24, FL_(auxElecHeatNrgConsPool), DeviceValueUOM::KWH);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &nrgSuppTotal_, DeviceValueType::UINT24, FL_(nrgSuppTotal), DeviceValueUOM::KWH);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &nrgSuppHeating_, DeviceValueType::UINT24, FL_(nrgSuppHeating), DeviceValueUOM::KWH);
        register_device_value(DeviceValueTAG::TAG_DHW1, &nrgSuppWw_, DeviceValueType::UINT24, FL_(nrgSuppWw), DeviceValueUOM::KWH);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &nrgSuppCooling_, DeviceValueType::UINT24, FL_(nrgSuppCooling), DeviceValueUOM::KWH);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &nrgSuppPool_, DeviceValueType::UINT24, FL_(nrgSuppPool), DeviceValueUOM::KWH);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpPower_, DeviceValueType::UINT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(hpPower), DeviceValueUOM::KW);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &hpMaxPower_,
                              DeviceValueType::UINT8,
                              FL_(hpMaxPower),
                              DeviceValueUOM::PERCENT,
                              MAKE_CF_CB(set_hpMaxPower));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &pvMaxComp_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(pvMaxComp),
                              DeviceValueUOM::KW,
                              MAKE_CF_CB(set_pvMaxComp));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &powerReduction_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_MUL10,
                              FL_(powerReduction),
                              DeviceValueUOM::PERCENT,
                              MAKE_CF_CB(set_powerReduction),
                              30,
                              60);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &hpSetDiffPress_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_MUL50,
                              FL_(hpSetDiffPress),
                              DeviceValueUOM::MBAR,
                              MAKE_CF_CB(set_hpDiffPress),
                              150,
                              750);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpCompOn_, DeviceValueType::BOOL, FL_(hpCompOn), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpActivity_, DeviceValueType::ENUM, FL_(enum_hpactivity), FL_(hpActivity), DeviceValueUOM::NONE);
        // register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpHeatingOn_, DeviceValueType::BOOL, FL_(hpHeatingOn), DeviceValueUOM::NONE);
        // register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpCoolingOn_, DeviceValueType::BOOL, FL_(hpCoolingOn), DeviceValueUOM::NONE);
        // register_device_value(DeviceValueTAG::TAG_DHW1, &hpWwOn_, DeviceValueType::BOOL, FL_(hpWwOn), DeviceValueUOM::NONE);
        // register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpPoolOn_, DeviceValueType::BOOL, FL_(hpPoolOn), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpBrinePumpSpd_, DeviceValueType::UINT8, FL_(hpBrinePumpSpd), DeviceValueUOM::PERCENT);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpSwitchValve_, DeviceValueType::BOOL, FL_(hpSwitchValve), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpCompSpd_, DeviceValueType::UINT8, FL_(hpCompSpd), DeviceValueUOM::PERCENT);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpCircSpd_, DeviceValueType::UINT8, FL_(hpCircSpd), DeviceValueUOM::PERCENT);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &hpBrineIn_,
                              DeviceValueType::INT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(hpBrineIn),
                              DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &hpBrineOut_,
                              DeviceValueType::INT16,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(hpBrineOut),
                              DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpTc0_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(hpTc0), DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpTc1_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(hpTc1), DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpTc3_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(hpTc3), DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpTr1_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(hpTr1), DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpTr3_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(hpTr3), DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpTr4_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(hpTr4), DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpTr5_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(hpTr5), DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpTr6_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(hpTr6), DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpTr7_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(hpTr7), DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpTl2_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(hpTl2), DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpPl1_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(hpPl1), DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpPh1_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(hpPh1), DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpTa4_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(hpTa4), DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpTw1_, DeviceValueType::INT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(hpTw1), DeviceValueUOM::DEGREES);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &poolSetTemp_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV2,
                              FL_(poolSetTemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_pool_temp));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hp4wayValve_, DeviceValueType::ENUM, FL_(enum_4way), FL_(hp4wayValve), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpInput[0].state, DeviceValueType::BOOL, FL_(hpInput1), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &hpInput[0].option,
                              DeviceValueType::STRING,
                              FL_(tpl_input),
                              FL_(hpIn1Opt),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_HpIn1Logic));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpInput[1].state, DeviceValueType::BOOL, FL_(hpInput2), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &hpInput[1].option,
                              DeviceValueType::STRING,
                              FL_(tpl_input),
                              FL_(hpIn2Opt),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_HpIn2Logic));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpInput[2].state, DeviceValueType::BOOL, FL_(hpInput3), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &hpInput[2].option,
                              DeviceValueType::STRING,
                              FL_(tpl_input),
                              FL_(hpIn3Opt),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_HpIn3Logic));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpInput[3].state, DeviceValueType::BOOL, FL_(hpInput4), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &hpInput[3].option,
                              DeviceValueType::STRING,
                              FL_(tpl_input4),
                              FL_(hpIn4Opt),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_HpIn4Logic));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &maxHeatComp_,
                              DeviceValueType::ENUM,
                              FL_(enum_maxHeat),
                              FL_(maxHeatComp),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_maxHeatComp));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &maxHeatHeat_,
                              DeviceValueType::ENUM,
                              FL_(enum_maxHeat),
                              FL_(maxHeatHeat),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_maxHeatHeat));
        register_device_value(DeviceValueTAG::TAG_DHW1,
                              &maxHeatDhw_,
                              DeviceValueType::ENUM,
                              FL_(enum_maxHeat),
                              FL_(maxHeatDhw),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_maxHeatDhw));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &manDefrost_, DeviceValueType::BOOL, FL_(manDefrost), DeviceValueUOM::NONE, MAKE_CF_CB(set_manDefrost));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &pvCooling_, DeviceValueType::BOOL, FL_(pvCooling), DeviceValueUOM::NONE, MAKE_CF_CB(set_pvCooling));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &auxHeaterOnly_,
                              DeviceValueType::BOOL,
                              FL_(auxHeaterOnly),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_additionalHeaterOnly));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &auxHeaterOff_,
                              DeviceValueType::BOOL,
                              FL_(auxHeaterOff),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_additionalHeater));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &auxHeaterStatus_,
                              DeviceValueType::ENUM,
                              FL_(enum_hpactivity),
                              FL_(auxHeaterStatus),
                              DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &auxHeaterLevel_, DeviceValueType::UINT8, FL_(auxHeaterLevel), DeviceValueUOM::PERCENT);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &auxHeaterDelay_,
                              DeviceValueType::UINT16,
                              DeviceValueNumOp::DV_NUMOP_MUL10,
                              FL_(auxHeaterDelay),
                              DeviceValueUOM::KMIN,
                              MAKE_CF_CB(set_additionalHeaterDelay),
                              10,
                              1000);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &auxMaxLimit_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(auxMaxLimit),
                              DeviceValueUOM::K,
                              MAKE_CF_CB(set_auxMaxLimit),
                              0,
                              10);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &auxLimitStart_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(auxLimitStart),
                              DeviceValueUOM::K,
                              MAKE_CF_CB(set_auxLimitStart),
                              0,
                              10);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &auxHeatMode_,
                              DeviceValueType::ENUM,
                              FL_(enum_modetype),
                              FL_(auxHeatMode),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_auxHeatMode));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &hpHystHeat_,
                              DeviceValueType::UINT16,
                              DeviceValueNumOp::DV_NUMOP_MUL5,
                              FL_(hpHystHeat),
                              DeviceValueUOM::KMIN,
                              MAKE_CF_CB(set_hpHystHeat),
                              50,
                              1500);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &hpHystCool_,
                              DeviceValueType::UINT16,
                              DeviceValueNumOp::DV_NUMOP_MUL5,
                              FL_(hpHystCool),
                              DeviceValueUOM::KMIN,
                              MAKE_CF_CB(set_hpHystCool),
                              50,
                              1500);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &hpHystPool_,
                              DeviceValueType::UINT16,
                              DeviceValueNumOp::DV_NUMOP_MUL5,
                              FL_(hpHystPool),
                              DeviceValueUOM::KMIN,
                              MAKE_CF_CB(set_hpHystPool),
                              50,
                              1500);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &silentMode_,
                              DeviceValueType::ENUM,
                              FL_(enum_silentMode),
                              FL_(silentMode),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_silentMode));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &silentFrom_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_MUL15,
                              FL_(silentFrom),
                              DeviceValueUOM::MINUTES,
                              MAKE_CF_CB(set_silentFrom));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &silentTo_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_MUL15,
                              FL_(silentTo),
                              DeviceValueUOM::MINUTES,
                              MAKE_CF_CB(set_silentTo));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &minTempSilent_,
                              DeviceValueType::INT8,
                              FL_(minTempSilent),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_minTempSilent));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &tempParMode_,
                              DeviceValueType::INT8,
                              FL_(tempParMode),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_tempParMode));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &auxHeatMixValve_, DeviceValueType::INT8, FL_(auxHeatMixValve), DeviceValueUOM::PERCENT);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &tempDiffHeat_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(tempDiffHeat),
                              DeviceValueUOM::K,
                              MAKE_CF_CB(set_tempDiffHeat),
                              2,
                              10);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &tempDiffCool_,
                              DeviceValueType::UINT8,
                              DeviceValueNumOp::DV_NUMOP_DIV10,
                              FL_(tempDiffCool),
                              DeviceValueUOM::K,
                              MAKE_CF_CB(set_tempDiffCool),
                              2,
                              10);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &vp_cooling_, DeviceValueType::BOOL, FL_(vp_cooling), DeviceValueUOM::NONE, MAKE_CF_CB(set_vp_cooling));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &heatCable_, DeviceValueType::BOOL, FL_(heatCable), DeviceValueUOM::NONE, MAKE_CF_CB(set_heatCable));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &VC0valve_, DeviceValueType::BOOL, FL_(VC0valve), DeviceValueUOM::NONE, MAKE_CF_CB(set_VC0valve));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &primePump_, DeviceValueType::BOOL, FL_(primePump), DeviceValueUOM::NONE, MAKE_CF_CB(set_primePump));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &primePumpMod_,
                              DeviceValueType::UINT8,
                              FL_(primePumpMod),
                              DeviceValueUOM::PERCENT,
                              MAKE_CF_CB(set_primePumpMod));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &hp3wayValve_,
                              DeviceValueType::BOOL,
                              FL_(hp3wayValve),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_hp3wayValve));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &elHeatStep1_,
                              DeviceValueType::BOOL,
                              FL_(elHeatStep1),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_elHeatStep1));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &elHeatStep2_,
                              DeviceValueType::BOOL,
                              FL_(elHeatStep2),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_elHeatStep2));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &elHeatStep3_,
                              DeviceValueType::BOOL,
                              FL_(elHeatStep3),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_elHeatStep3));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpEA0_, DeviceValueType::BOOL, FL_(hpEA0), DeviceValueUOM::NONE);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &hpPumpMode_,
                              DeviceValueType::ENUM,
                              FL_(enum_hpPumpMode),
                              FL_(hpPumpMode),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_hpPumpMode));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &fan_, DeviceValueType::UINT8, FL_(hpFan), DeviceValueUOM::PERCENT, MAKE_CF_CB(set_fan), 20, 100);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &hpshutdown_,
                              DeviceValueType::CMD,
                              FL_(enum_modetype5),
                              FL_(hpShutdown),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_shutdown));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &hpCurrPower_, DeviceValueType::UINT16, FL_(hpCurrPower), DeviceValueUOM::W);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &hpPowerLimit_,
                              DeviceValueType::UINT16,
                              FL_(hpPowerLimit),
                              DeviceValueUOM::W,
                              MAKE_CF_CB(set_hpPowerLimit));

        // heatpump DHW settings
        register_device_value(DeviceValueTAG::TAG_DHW1,
                              &wwAlternatingOper_,
                              DeviceValueType::BOOL,
                              FL_(wwAlternatingOper),
                              DeviceValueUOM::NONE,
                              MAKE_CF_CB(set_wwAlternatingOper));
        register_device_value(DeviceValueTAG::TAG_DHW1,
                              &wwAltOpPrioHeat_,
                              DeviceValueType::UINT8,
                              FL_(wwAltOpPrioHeat),
                              DeviceValueUOM::MINUTES,
                              MAKE_CF_CB(set_wwAltOpPrioHeat),
                              20,
                              120);
        register_device_value(DeviceValueTAG::TAG_DHW1,
                              &wwAltOpPrioWw_,
                              DeviceValueType::UINT8,
                              FL_(wwAltOpPrioWw),
                              DeviceValueUOM::MINUTES,
                              MAKE_CF_CB(set_wwAltOpPrioWw),
                              30,
                              120);
        register_device_value(DeviceValueTAG::TAG_DHW1,
                              &wwComfOffTemp_,
                              DeviceValueType::UINT8,
                              FL_(wwComfOffTemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_wwComfOffTemp),
                              15,
                              65);
        register_device_value(
            DeviceValueTAG::TAG_DHW1, &wwEcoOffTemp_, DeviceValueType::UINT8, FL_(wwEcoOffTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_wwEcoOffTemp), 15, 65);
        register_device_value(DeviceValueTAG::TAG_DHW1,
                              &wwEcoPlusOffTemp_,
                              DeviceValueType::UINT8,
                              FL_(wwEcoPlusOffTemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_wwEcoPlusOffTemp),
                              48,
                              63);
        register_device_value(
            DeviceValueTAG::TAG_DHW1, &wwComfDiffTemp_, DeviceValueType::UINT8, FL_(wwComfDiffTemp), DeviceValueUOM::K, MAKE_CF_CB(set_wwComfDiffTemp), 6, 12);
        register_device_value(
            DeviceValueTAG::TAG_DHW1, &wwEcoDiffTemp_, DeviceValueType::UINT8, FL_(wwEcoDiffTemp), DeviceValueUOM::K, MAKE_CF_CB(set_wwEcoDiffTemp), 6, 12);
        register_device_value(DeviceValueTAG::TAG_DHW1,
                              &wwEcoPlusDiffTemp_,
                              DeviceValueType::UINT8,
                              FL_(wwEcoPlusDiffTemp),
                              DeviceValueUOM::K,
                              MAKE_CF_CB(set_wwEcoPlusDiffTemp),
                              6,
                              12);
        register_device_value(DeviceValueTAG::TAG_DHW1,
                              &wwComfStopTemp_,
                              DeviceValueType::UINT8,
                              FL_(wwComfStopTemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_wwComfStopTemp));
        register_device_value(DeviceValueTAG::TAG_DHW1,
                              &wwEcoStopTemp_,
                              DeviceValueType::UINT8,
                              FL_(wwEcoStopTemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_wwEcoStopTemp));
        register_device_value(DeviceValueTAG::TAG_DHW1,
                              &wwEcoPlusStopTemp_,
                              DeviceValueType::UINT8,
                              FL_(wwEcoPlusStopTemp),
                              DeviceValueUOM::DEGREES,
                              MAKE_CF_CB(set_wwEcoPlusStopTemp));
        register_device_value(DeviceValueTAG::TAG_DHW1, &hpCircPumpWw_, DeviceValueType::BOOL, FL_(hpCircPumpWw), DeviceValueUOM::NONE, MAKE_CF_CB(set_hpCircPumpWw));
    }

    // dhw - DEVICE_DATA_ww topic
    register_device_value(DeviceValueTAG::TAG_DHW1,
                          &wwTapActivated_,
                          DeviceValueType::BOOL,
                          FL_(wwtapactivated),
                          DeviceValueUOM::NONE,
                          MAKE_CF_CB(set_tapwarmwater_activated));
    register_device_value(DeviceValueTAG::TAG_DHW1, &wwSetTemp_, DeviceValueType::UINT8, FL_(wwSetTemp), DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DHW1, &wwSelTemp_, DeviceValueType::UINT8, FL_(wwSelTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_ww_temp));
    register_device_value(DeviceValueTAG::TAG_DHW1, &wwSelTempLow_, DeviceValueType::UINT8, FL_(wwSelTempLow), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_ww_temp_low));
    register_device_value(DeviceValueTAG::TAG_DHW1,
                          &wwSelTempEcoplus_,
                          DeviceValueType::UINT8,
                          FL_(wwSelTempEco),
                          DeviceValueUOM::DEGREES,
                          MAKE_CF_CB(set_ww_temp_eco));
    register_device_value(DeviceValueTAG::TAG_DHW1, &wwSelTempOff_, DeviceValueType::UINT8, FL_(wwSelTempOff), DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DHW1,
                          &wwSelTempSingle_,
                          DeviceValueType::UINT8,
                          FL_(wwSelTempSingle),
                          DeviceValueUOM::DEGREES,
                          MAKE_CF_CB(set_ww_temp_single));
    register_device_value(DeviceValueTAG::TAG_DHW1,
                          &wwSolarTemp_,
                          DeviceValueType::UINT16,
                          DeviceValueNumOp::DV_NUMOP_DIV10,
                          FL_(wwSolarTemp),
                          DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DHW1, &wwType_, DeviceValueType::ENUM, FL_(enum_flow), FL_(wwType), DeviceValueUOM::NONE);
    register_device_value(
        DeviceValueTAG::TAG_DHW1, &wwComfort_, DeviceValueType::ENUM, FL_(enum_comfort), FL_(wwComfort), DeviceValueUOM::NONE, MAKE_CF_CB(set_ww_mode));
    wwComfort2_ = EMS_VALUE_UINT8_NOTSET; // read separately, but published as wwComfort1_
    register_device_value(
        DeviceValueTAG::TAG_DHW1, &wwComfort1_, DeviceValueType::ENUM, FL_(enum_comfort1), FL_(wwComfort1), DeviceValueUOM::NONE, MAKE_CF_CB(set_ww_mode));
    register_device_value(DeviceValueTAG::TAG_DHW1,
                          &wwFlowTempOffset_,
                          DeviceValueType::UINT8,
                          FL_(wwFlowTempOffset),
                          DeviceValueUOM::DEGREES_R,
                          MAKE_CF_CB(set_ww_flowTempOffset),
                          0,
                          100);
    register_device_value(DeviceValueTAG::TAG_DHW1,
                          &wwChargeOptimization_,
                          DeviceValueType::BOOL,
                          FL_(wwChargeOptimization),
                          DeviceValueUOM::NONE,
                          MAKE_CF_CB(set_ww_chargeOptimization));
    register_device_value(
        DeviceValueTAG::TAG_DHW1, &wwMaxPower_, DeviceValueType::UINT8, FL_(wwMaxPower), DeviceValueUOM::PERCENT, MAKE_CF_CB(set_ww_maxpower), 0, 254);
    register_device_value(DeviceValueTAG::TAG_DHW1, &wwMaxTemp_, DeviceValueType::UINT8, FL_(wwMaxTemp), DeviceValueUOM::DEGREES, MAKE_CF_CB(set_ww_maxtemp), 0, 80);
    register_device_value(DeviceValueTAG::TAG_DHW1, &wwCircPump_, DeviceValueType::BOOL, FL_(wwCircPump), DeviceValueUOM::NONE, MAKE_CF_CB(set_ww_circulation_pump));
    register_device_value(DeviceValueTAG::TAG_DHW1, &wwChargeType_, DeviceValueType::ENUM, FL_(enum_charge), FL_(wwChargeType), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DHW1, &wwHystOn_, DeviceValueType::INT8, FL_(wwHystOn), DeviceValueUOM::DEGREES_R, MAKE_CF_CB(set_ww_hyst_on));
    register_device_value(DeviceValueTAG::TAG_DHW1, &wwHystOff_, DeviceValueType::INT8, FL_(wwHystOff), DeviceValueUOM::DEGREES_R, MAKE_CF_CB(set_ww_hyst_off));
    register_device_value(DeviceValueTAG::TAG_DHW1,
                          &wwDisinfectionTemp_,
                          DeviceValueType::UINT8,
                          FL_(wwDisinfectionTemp),
                          DeviceValueUOM::DEGREES,
                          MAKE_CF_CB(set_ww_disinfect_temp),
                          60,
                          80);
    register_device_value(DeviceValueTAG::TAG_DHW1,
                          &wwCircMode_,
                          DeviceValueType::ENUM,
                          FL_(enum_freq),
                          FL_(wwCircMode),
                          DeviceValueUOM::NONE,
                          MAKE_CF_CB(set_ww_circulation_mode));
    register_device_value(DeviceValueTAG::TAG_DHW1, &wwCirc_, DeviceValueType::BOOL, FL_(wwCirc), DeviceValueUOM::NONE, MAKE_CF_CB(set_ww_circulation));
    register_device_value(DeviceValueTAG::TAG_DHW1, &wwCurTemp_, DeviceValueType::UINT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(wwCurTemp), DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DHW1, &wwCurTemp2_, DeviceValueType::UINT16, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(wwCurTemp2), DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DHW1, &wwCurFlow_, DeviceValueType::UINT8, DeviceValueNumOp::DV_NUMOP_DIV10, FL_(wwCurFlow), DeviceValueUOM::LMIN);
    register_device_value(DeviceValueTAG::TAG_DHW1,
                          &wwStorageTemp1_,
                          DeviceValueType::UINT16,
                          DeviceValueNumOp::DV_NUMOP_DIV10,
                          FL_(wwStorageTemp1),
                          DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DHW1,
                          &wwStorageTemp2_,
                          DeviceValueType::UINT16,
                          DeviceValueNumOp::DV_NUMOP_DIV10,
                          FL_(wwStorageTemp2),
                          DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DHW1, &wwActivated_, DeviceValueType::BOOL, FL_(wwActivated), DeviceValueUOM::NONE, MAKE_CF_CB(set_ww_activated));
    register_device_value(DeviceValueTAG::TAG_DHW1, &wwOneTime_, DeviceValueType::BOOL, FL_(wwOneTime), DeviceValueUOM::NONE, MAKE_CF_CB(set_ww_onetime));
    register_device_value(DeviceValueTAG::TAG_DHW1, &wwDisinfect_, DeviceValueType::BOOL, FL_(wwDisinfecting), DeviceValueUOM::NONE, MAKE_CF_CB(set_ww_disinfect));
    register_device_value(DeviceValueTAG::TAG_DHW1, &wwCharging_, DeviceValueType::BOOL, FL_(wwCharging), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DHW1, &wwRecharging_, DeviceValueType::BOOL, FL_(wwRecharging), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DHW1, &wwTempOK_, DeviceValueType::BOOL, FL_(wwTempOK), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DHW1, &wwActive_, DeviceValueType::BOOL, FL_(wwActive), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DHW1, &ww3wayValve_, DeviceValueType::BOOL, FL_(ww3wayValve), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DHW1,
                          &wwMixerTemp_,
                          DeviceValueType::UINT16,
                          DeviceValueNumOp::DV_NUMOP_DIV10,
                          FL_(wwMixerTemp),
                          DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DHW1,
                          &wwCylMiddleTemp_,
                          DeviceValueType::UINT16,
                          DeviceValueNumOp::DV_NUMOP_DIV10,
                          FL_(wwCylMiddleTemp),
                          DeviceValueUOM::DEGREES);
    register_device_value(DeviceValueTAG::TAG_DHW1, &wwStarts_, DeviceValueType::UINT24, FL_(wwStarts), DeviceValueUOM::NONE);
    register_device_value(DeviceValueTAG::TAG_DHW1, &wwWorkM_, DeviceValueType::TIME, FL_(wwWorkM), DeviceValueUOM::MINUTES);

    // fetch some initial data
    EMSESP::send_read_request(0x10, device_id); // read last errorcode on start (only published on errors)
    EMSESP::send_read_request(0x11, device_id); // read last errorcode on start (only published on errors)
    EMSESP::send_read_request(0x15, device_id); // read maintenance data on start (only published on change)
    EMSESP::send_read_request(0x1C, device_id); // read maintenance status on start (only published on change)
    EMSESP::send_read_request(0xC2, device_id); // read last errorcode on start (only published on errors)


    if (model() != EMSdevice::EMS_DEVICE_FLAG_HEATPUMP && model() != EMSdevice::EMS_DEVICE_FLAG_HIU) {
        register_telegram_type(0x04, "UBAFactory", true, MAKE_PF_CB(process_UBAFactory));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA, &nomPower_, DeviceValueType::UINT8, FL_(nomPower), DeviceValueUOM::KW, MAKE_CF_CB(set_nomPower));
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &nrgTotal_,
                              DeviceValueType::UINT24,
                              DeviceValueNumOp::DV_NUMOP_DIV100,
                              FL_(nrgTotal),
                              DeviceValueUOM::KWH);
        register_device_value(DeviceValueTAG::TAG_DEVICE_DATA,
                              &nrgHeat_,
                              DeviceValueType::UINT24,
                              DeviceValueNumOp::DV_NUMOP_DIV100,
                              FL_(nrgHeat),
                              DeviceValueUOM::KWH,
                              MAKE_CF_CB(set_nrgHeat),
                              0,
                              10000000UL);
        register_device_value(DeviceValueTAG::TAG_DHW1,
                              &nrgWw_,
                              DeviceValueType::UINT24,
                              DeviceValueNumOp::DV_NUMOP_DIV100,
                              FL_(nrgWw),
                              DeviceValueUOM::KWH,
                              MAKE_CF_CB(set_nrgWw),
                              0,
                              10000000UL);

        nrgHeatF_ = EMSESP::nvs_.getDouble("nrgheat", 0);
        nrgWwF_   = EMSESP::nvs_.getDouble("nrgww", 0);
        nomPower_ = EMSESP::nvs_.getUChar("nompower", 0);
        if (nrgHeatF_ < 0 || nrgHeatF_ >= EMS_VALUE_UINT32_NOTSET) {
            nrgHeatF_ = 0;
        }
        if (nrgWwF_ < 0 || nrgWwF_ >= EMS_VALUE_UINT32_NOTSET) {
            nrgWwF_ = 0;
        }
        if (nomPower_ == EMS_VALUE_UINT8_NOTSET) {
            nomPower_ = 0;
        }
        store_energy();
        // update/publish the values
        has_update(nrgHeat_, (uint32_t)(nrgHeatF_ + 0.5));
        has_update(nrgWw_, (uint32_t)(nrgWwF_ + 0.5));
        has_update(&nomPower_);
    }
}

void Boiler::store_energy() {
    // only write if something is changed
    if (nrgHeatF_ != EMSESP::nvs_.getDouble("nrgheat")) {
        EMSESP::nvs_.putDouble("nrgheat", nrgHeatF_);
    }
    if (nrgWwF_ != EMSESP::nvs_.getDouble("nrgww")) {
        EMSESP::nvs_.putDouble("nrgww", nrgWwF_);
    }
    if (nomPower_ != EMSESP::nvs_.getUChar("nompower")) {
        EMSESP::nvs_.putUChar("nompower", nomPower_);
    }
    // LOG_DEBUG("energy values stored");
}

// Check if hot tap water or heating is active
// Values will always be posted first time as heatingActive_ and tapwaterActive_ will have values EMS_VALUE_BOOL_NOTSET
void Boiler::check_active() {
    if (!Helpers::hasValue(boilerState_)) {
        return;
    }

    bool    b;
    uint8_t val;

    // check if heating is active, bits 2 and 4 must be set
    b   = ((boilerState_ & 0x09) == 0x09);
    val = b ? EMS_VALUE_BOOL_ON : EMS_VALUE_BOOL_OFF;
    if (heatingActive_ != val) {
        has_update(heatingActive_, val);
        char s[12];
        Mqtt::queue_publish(F_(heating_active), Helpers::render_boolean(s, b));
    }

    // check if we can use tapactivated in flow systems
    if ((wwType_ == 1) && !Helpers::hasValue(wwTapActivated_, EMS_VALUE_BOOL)) {
        wwTapActivated_ = 1;
    }

    // check if tap water is active, bits 1 and 4 must be set
    // also check if there is a flowsensor and flow-type
    static bool flowsensor = false;
    if (Helpers::hasValue(wwCurFlow_) && (wwCurFlow_ > 0) && (wwType_ == 1)) {
        flowsensor = true;
    }
    if (flowsensor) {
        b = ((wwCurFlow_ > 0) && ((boilerState_ & 0x0A) == 0x0A));
    } else {
        b = ((boilerState_ & 0x0A) == 0x0A);
    }

    val = b ? EMS_VALUE_BOOL_ON : EMS_VALUE_BOOL_OFF;
    if (tapwaterActive_ != val) {
        has_update(tapwaterActive_, val);
        char s[12];
        Mqtt::queue_publish(F_(tapwater_active), Helpers::render_boolean(s, b));
        // if (flowsensor) {
        EMSESP::tap_water_active(b); // let EMS-ESP know, used in the Shower class
        // }
    }

    // check forceheatingoff option
    if (!Helpers::hasValue(forceHeatingOff_, EMS_VALUE_BOOL)) {
        EMSESP::webSettingsService.read([&](WebSettings & settings) { forceHeatingOff_ = settings.boiler_heatingoff ? EMS_VALUE_BOOL_ON : 0; });
        has_update(&forceHeatingOff_);
    }
    static uint32_t lastSendHeatingOff = 0;
    if (forceHeatingOff_ == EMS_VALUE_BOOL_ON && (uuid::get_uptime_sec() - lastSendHeatingOff) >= 60) {
        lastSendHeatingOff = uuid::get_uptime_sec();
        uint8_t data[]     = {0, 0, 0, 0};
        write_command(EMS_TYPE_UBASetPoints, 0, data, sizeof(data), 0);
    }

    // calculate energy for boiler 0x08 from stored modulation an time in units of 0.01 Wh
    if (model() != EMSdevice::EMS_DEVICE_FLAG_HEATPUMP && model() != EMSdevice::EMS_DEVICE_FLAG_HIU) {
        // remember values from last call
        static uint32_t powLastReadTime_ = uuid::get_uptime();
        static uint8_t  heatBurnPow      = 0;
        static uint8_t  wwBurnPow        = 0;
        static uint8_t  lastSaveHour     = 0;
        // store in units of 0.01 kWh, resolution needed: 0.01 Wh = 0.01 Ws / 3600  = (% * kW * ms) / 3600
        nrgHeatF_ += ((double)((uint32_t)heatBurnPow * nomPower_ * (uuid::get_uptime() - powLastReadTime_)) / 3600) / 1000UL;
        nrgWwF_ += ((double)((uint32_t)wwBurnPow * nomPower_ * (uuid::get_uptime() - powLastReadTime_)) / 3600) / 1000UL;
        has_update(nrgHeat_, (uint32_t)(nrgHeatF_ + 0.5));
        has_update(nrgWw_, (uint32_t)(nrgWwF_ + 0.5));
        has_update(nrgTotal_, (uint32_t)(nrgHeatF_ + nrgWwF_ + 0.5));
        // check for store values
        time_t now = time(nullptr);
        tm *   tm_ = localtime(&now);
        if (tm_->tm_hour != lastSaveHour) {
            lastSaveHour = tm_->tm_hour;
            store_energy();
        } else if (curBurnPow_ == 0 && (heatBurnPow + wwBurnPow) > 0) { // on burner switch off
            store_energy();
        }
        // store new modulation and time
        heatBurnPow      = heatingActive_ && !tapwaterActive_ ? curBurnPow_ : 0;
        wwBurnPow        = tapwaterActive_ ? curBurnPow_ : 0;
        powLastReadTime_ = uuid::get_uptime();
    }
}

// 0x04
//  boiler(0x08) -W-> Me(0x0B), ?(0x04), data: 13 96 09 81 00 64 64 35 05 64 5A 22 00 00 00 00 00 00 00 00 B7
// offset 4 - nominal Power kW, could be zero, 5 - min. Burner, 6 - max. Burner
void Boiler::process_UBAFactory(std::shared_ptr<const Telegram> telegram) {
    // check for all wanted info in telegram
    if (telegram->offset > 4 || telegram->offset + telegram->message_length < 7) {
        return;
    }
    toggle_fetch(telegram->type_id, false); // only read once
    uint8_t min      = 0;
    uint8_t max      = 0;
    uint8_t nomPower = 0;
    telegram->read_value(nomPower, 4);
    telegram->read_value(min, 5);
    telegram->read_value(max, 6);
    // set the value only if no nvs-value is set
    if (nomPower > 0 && nomPower_ == 0) {
        has_update(nomPower_, nomPower);
    }
    if (min <= max) {
        set_minmax(&burnMinPower_, 0, max);
        set_minmax(&burnMaxPower_, min, max);
        set_minmax(&wwMaxPower_, min, max);
        set_minmax(&selBurnPow_, 0, max);
    }
}

// 0x18
void Boiler::process_UBAMonitorFast(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, selFlowTemp_, 0);
    has_update(telegram, curFlowTemp_, 1);
    // has_update(telegram, selBurnPow_, 3); // burn power max setting
    // has_update(telegram, curBurnPow_, 4);
    has_update(telegram, boilerState_, 5); // bits 0-heat, 1-dhw, 2-service, 3-flame, 4-preheat, 5-lock-Err, 6-block-err, 7-maint
    has_bitupdate(telegram, burnGas_, 7, 0);
    has_bitupdate(telegram, burnGas2_, 7, 1);
    has_bitupdate(telegram, fanWork_, 7, 2);
    has_bitupdate(telegram, ignWork_, 7, 3);
    has_bitupdate(telegram, oilPreHeat_, 7, 4);
    has_bitupdate(telegram, heatingPump_, 7, 5);
    has_bitupdate(telegram, ww3wayValve_, 7, 6);
    has_bitupdate(telegram, wwCirc_, 7, 7);

    // dhw storage sensors (if present)
    // wwStorageTemp2 is also used by some brands as the boiler temperature - see https://github.com/emsesp/EMS-ESP/issues/206
    has_update(telegram, wwStorageTemp1_, 9);  // 0x8300 if not available
    has_update(telegram, wwStorageTemp2_, 11); // 0x8000 if not available - this is boiler temp

    has_update(telegram, retTemp_, 13);
    has_update(telegram, flameCurr_, 15);

    // system pressure. FF means missing
    has_update(telegram, sysPress_, 17); // is *10

    // read the service code / installation status as appears on the display
    if ((telegram->message_length > 18) && (telegram->offset == 0)) {
        char serviceCode[4];
        telegram->read_value(serviceCode[0], 18);
        serviceCode[0] = (serviceCode[0] == (char)0xF0) ? '~' : serviceCode[0];
        telegram->read_value(serviceCode[1], 19);
        serviceCode[2] = '\0'; // null terminate string
        has_update(serviceCode_, serviceCode, sizeof(serviceCode_));
    }

    has_update(telegram, serviceCodeNumber_, 20);

    if (telegram->offset <= 3 && telegram->offset + telegram->message_length > 7) {
        // some boiler only switch burnGas and have no other burner values, https://github.com/emsesp/EMS-ESP32/discussions/1483
        uint8_t selBurnPow = selBurnPow_;
        uint8_t curBurnPow = curBurnPow_;
        telegram->read_value(selBurnPow, 3);
        telegram->read_value(curBurnPow, 4);
        if (burnGas_ && selBurnPow == 0 && curBurnPow == 0) {
            boilerState_ |= 0x08; // set flame signal
            curBurnPow = 100;
            selBurnPow = 100;
        }
        has_update(selBurnPow_, selBurnPow);
        has_update(curBurnPow_, curBurnPow);

        check_active(); // do a quick check to see if the hot water or heating is active
    }
}

/*
 * UBATotalUptime - type 0x14 - total uptime
 * received only after requested (not broadcasted)
 */
void Boiler::process_UBATotalUptime(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, UBAuptime_, 0, 3); // force to 3 bytes
    // if broadcasted there is no need to fetch
    if (telegram->dest == 0) {
        toggle_fetch(0x14, false);
    }
}

/*
 * UBAParameters - type 0x16
 * data: FF 5A 64 00 0A FA 0F 02 06 64 64 02 08 F8 0F 0F 0F 0F 1E 05 04 09 09 00 28 00 3C
 */
void Boiler::process_UBAParameters(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, heatingActivated_, 0);
    has_update(telegram, heatingTemp_, 1);
    has_update(telegram, burnMaxPower_, 2);
    has_update(telegram, burnMinPower_, 3);
    has_update(telegram, boilHystOff_, 4);
    has_update(telegram, boilHystOn_, 5);
    has_update(telegram, burnMinPeriod_, 6);
    //has_update(telegram, pumpType_, 7); // 0=off, 02=?
    has_update(telegram, pumpDelay_, 8);
    has_update(telegram, pumpModMax_, 9);
    has_update(telegram, pumpModMin_, 10);
    has_update(telegram, pumpMode_, 11);
    has_update(telegram, boil2HystOff_, 12);
    has_update(telegram, boil2HystOn_, 13);
    has_update(telegram, pumpOnTemp_, 23); // https://github.com/emsesp/EMS-ESP32/issues/2088
}

/*
 * UBASettingsWW - type 0x26 - max power on offset 7, https://github.com/emsesp/EMS-ESP/issues/740
 * Boiler(0x08) -> Me(0x0B), ?(0x26), data: 01 05 00 0F 00 1E 58 5A
 */
void Boiler::process_UBASettingsWW(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, wwMaxPower_, 7);
}

// 0x33
//  Boiler(0x08) -> Me(0x0B), UBAParameterWW(0x33), data: 08 FF 30 FB FF 28 FF 07 46 00 00
void Boiler::process_UBAParameterWW(std::shared_ptr<const Telegram> telegram) {
    // has_bitupdate(telegram, wwEquipt_,0,3);  //  8=boiler has ww
    has_update(telegram, wwActivated_, 1); // 0xFF means on
    has_update(telegram, wwSelTemp_, 2);
    has_update(telegram, wwHystOn_, 3);         // Hyst on (default -5)
    has_update(telegram, wwHystOff_, 4);        // Hyst off (default -1)
    has_update(telegram, wwFlowTempOffset_, 5); // default 40
    has_update(telegram, wwCircPump_, 6);       // 0xFF means on
    has_update(telegram, wwCircMode_, 7);       // 0=off, 1=1x3min 6=6x3min 7=continuous
    has_update(telegram, wwDisinfectionTemp_, 8);
    has_bitupdate(telegram, wwChargeType_, 10, 0); // 0 = charge pump, 0xff = 3-way valve

    uint8_t wwComfort = EMS_VALUE_UINT8_NOTSET;
    if (telegram->read_value(wwComfort, 9)) {
        if (wwComfort == 0) {
            wwComfort = 0; // Hot
        } else if (wwComfort == 0xD8) {
            wwComfort = 1; // Eco
        } else if (wwComfort == 0xEC) {
            wwComfort = 2; // Intelligent
        } else {
            wwComfort = EMS_VALUE_UINT8_NOTSET;
        }
        has_update(wwComfort_, wwComfort);
    }
}

/*
 * UBAMonitorWW - type 0x34 - dhw monitor. 19 bytes long
 * received every 10 seconds
 * Boiler(0x08) -> Me(0x0B), UBAMonitorWW(0x34), data: 30 01 BA 7D 00 21 00 00 03 00 01 22 2B 00 19 5B
*/
void Boiler::process_UBAMonitorWW(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, wwSetTemp_, 0);
    has_update(telegram, wwCurTemp_, 1);
    has_update(telegram, wwCurTemp2_, 3);

    has_update(telegram, wwType_, 8);
    has_update(telegram, wwCurFlow_, 9);
    has_update(telegram, wwSolarTemp_, 17);
    has_update(telegram, wwWorkM_, 10, 3);  // force to 3 bytes
    has_update(telegram, wwStarts_, 13, 3); // force to 3 bytes

    has_bitupdate(telegram, wwOneTime_, 5, 1);
    has_bitupdate(telegram, wwDisinfect_, 5, 2);
    has_bitupdate(telegram, wwCharging_, 5, 3);
    has_bitupdate(telegram, wwRecharging_, 5, 4);
    has_bitupdate(telegram, wwTempOK_, 5, 5);
    has_bitupdate(telegram, wwActive_, 5, 6);
}

/*
 * UBAMonitorFastPlus - type 0xE4 - central heating monitor EMS+
 * temperatures at 7 and 23 always identical
+ * Bosch Logamax Plus GB122: https://github.com/emsesp/EMS-ESP/issues/620
+ * 88 00 E4 00 00 2D 2D 00 00 C9 34 02 21 64 3D 05 02 01 DE 00 00 00 00 03 62 14 00 02 21 00 00 00 00 00 00 00 2B 2B 83
+ * GB125/Logamatic MC110: issue #650: add retTemp & sysPress
+ * 08 00 E4 00 10 20 2D 48 00 C8 38 02 37 3C 27 03 00 00 00 00 00 01 7B 01 8F 11 00 02 37 80 00 02 1B 80 00 7F FF 80 00
 */
void Boiler::process_UBAMonitorFastPlus(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, selFlowTemp_, 6);
    has_bitupdate(telegram, burnGas_, 11, 0);
    //has_bitupdate(telegram, heatingPump_, 11, 1); // heating active? see SlowPlus
    has_bitupdate(telegram, ww3wayValve_, 11, 2);
    has_update(telegram, curBurnPow_, 10);
    has_update(telegram, selBurnPow_, 9);
    has_update(telegram, curFlowTemp_, 7);
    has_update(telegram, flameCurr_, 19);
    uint16_t rettemp = retTemp_;
    telegram->read_value(rettemp, 17); // 0 means no sensor, HIU read it in 0x779
    if (rettemp != 0 && rettemp != 0x8000) {
        has_update(retTemp_, rettemp);
    }

    uint8_t syspress = sysPress_;
    telegram->read_value(syspress, 21); // 0 means no sensor
    if (syspress == 0) {
        syspress = EMS_VALUE_UINT8_NOTSET;
    }
    has_update(sysPress_, syspress);

    //has_update(telegram, temperatur_, 13); // unknown temperature
    has_update(telegram, heatblock_, 23);  // see #1317
    has_update(telegram, headertemp_, 25); // see #1317
    //has_update(telegram, temperatur_, 27); // unknown temperature
    telegram->read_value(exhaustTemp1_, 31);
    if (Helpers::hasValue(exhaustTemp1_)) {
        has_update(exhaustTemp_, exhaustTemp1_);
    }
    has_update(telegram, pc0Flow_, 36); // see https://github.com/emsesp/EMS-ESP32/issues/2001

    // read 3 char service code / installation status as appears on the display
    if ((telegram->message_length > 3) && (telegram->offset == 0)) {
        char serviceCode[4] = {0};
        telegram->read_value(serviceCode[0], 1);
        serviceCode[0] = (serviceCode[0] == (char)0xF0) ? '~' : serviceCode[0];
        telegram->read_value(serviceCode[1], 2);
        serviceCode[1] = (serviceCode[1] == (char)0xF0) ? '~' : serviceCode[1];
        telegram->read_value(serviceCode[2], 3);
        serviceCode[3] = '\0';
        has_update(serviceCode_, serviceCode, sizeof(serviceCode_));
    }

    has_update(telegram, serviceCodeNumber_, 4);

    // at this point do a quick check to see if the hot water or heating is active
    uint8_t state = EMS_VALUE_UINT8_NOTSET;
    if (telegram->read_value(state, 11) && model() != EMSdevice::EMS_DEVICE_FLAG_HIU && model() != EMSdevice::EMS_DEVICE_FLAG_HEATPUMP) {
        boilerState_ = state & 0x01 ? 0x08 : 0;  // burnGas
        boilerState_ |= state & 0x02 ? 0x01 : 0; // heatingPump
        boilerState_ |= state & 0x04 ? 0x02 : 0; // 3-way-valve
    }

    if (telegram->offset <= 10 && telegram->offset + telegram->message_length > 11) {
        check_active(); // do a quick check to see if the hot water or heating is active
    }
}

/*
 * UBAMonitorSlow - type 0x19 - central heating monitor part 2 (27 bytes long)
 * received every 60 seconds
 * e.g. 08 00 19 00 80 00 02 41 80 00 00 00 00 00 03 91 7B 05 B8 40 00 00 00 04 92 AD 00 5E EE 80 00
 *      08 0B 19 00 FF EA 02 47 80 00 00 00 00 62 03 CA 24 2C D6 23 00 00 00 27 4A B6 03 6E 43
 *                  00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 17 19 20 21 22 23 24
 */
void Boiler::process_UBAMonitorSlow(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, outdoorTemp_, 0);
    has_update(telegram, boilTemp_, 2);
    has_update(telegram, exhaustTemp_, 4);
    has_update(telegram, switchTemp_, 25); // only if there is a mixer module present
    has_update(telegram, heatingPumpMod_, 9);
    has_update(telegram, burnStarts_, 10, 3);   // force to 3 bytes
    has_update(telegram, burnWorkMin_, 13, 3);  // force to 3 bytes
    has_update(telegram, burn2WorkMin_, 16, 3); // force to 3 bytes
    has_update(telegram, heatWorkMin_, 19, 3);  // force to 3 bytes
    has_update(telegram, heatStarts_, 22, 3);   // force to 3 bytes
}

/*
 * UBAMonitorSlowPlus2 - type 0xE3
 * 88 00 E3 00 04 00 00 00 00 01 00 00 00 00 00 02 22 2B 64 46 01 00 00 61
 * https://github.com/Th3M3/buderus_ems-wiki/blob/master/Quelle_08.md
 * https://github.com/emsesp/EMS-ESP32/issues/908
 */
void Boiler::process_UBAMonitorSlowPlus2(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, absBurnPow_, 13); // current burner absolute power (percent of rating plate power)
    if (model() == EMSdevice::EMS_DEVICE_FLAG_HIU) {
        uint8_t state = EMS_VALUE_UINT8_NOTSET;
        boilerState_  = 0;
        if (telegram->read_value(state, 2)) {
            boilerState_ |= state == 1 ? 0x09 : 0; // heating 0/1
        }
        state = EMS_VALUE_UINT8_NOTSET;
        if (telegram->read_value(state, 5)) {
            boilerState_ |= state == 1 ? 0x0A : 0; // dhw 0/1
        }
        check_active(); // do a quick check to see if the hot water or heating is active
    }
}

/*
 * UBAMonitorSlowPlus - type 0xE5 - central heating monitor EMS+
 * Boiler(0x08) -> Me(0x0B), UBAMonitorSlowPlus(0xE5),
 * data: 01 00 20 00 00 78 00 00 00 00 00 1E EB 00 9D 3E 00 00 00 00 6B 5E 00 06 4C 64 00 00 00 00 8A A3
 */
void Boiler::process_UBAMonitorSlowPlus(std::shared_ptr<const Telegram> telegram) {
    has_bitupdate(telegram, fanWork_, 2, 2);
    has_bitupdate(telegram, ignWork_, 2, 3);
    has_bitupdate(telegram, heatingPump_, 2, 5);
    has_bitupdate(telegram, wwCirc_, 2, 7);
    // temperature measurements at offset 4 unknown, see https://github.com/emsesp/EMS-ESP/issues/620
    // exhaust is in E4/31, but not always published, but this goes to zero if burner stops
    if (!Helpers::hasValue(exhaustTemp1_)) {
        has_update(telegram, exhaustTemp_, 6);
    }
    has_update(telegram, burnStarts_, 10, 3);   // force to 3 bytes
    has_update(telegram, burnWorkMin_, 13, 3);  // force to 3 bytes
    has_update(telegram, burn2WorkMin_, 16, 3); // force to 3 bytes
    has_update(telegram, heatWorkMin_, 19, 3);  // force to 3 bytes
    has_update(telegram, heatStarts_, 22, 3);   // force to 3 bytes
    has_update(telegram, heatingPumpMod_, 25);
}

/*
 * UBAParametersPlus - type 0xE6
 * parameters originally taken from
 * https://github.com/Th3M3/buderus_ems-wiki/blob/master/Einstellungen%20des%20Regelger%C3%A4ts%20MC110.md
 * 88 0B E6 00 01 46 00 00 46 0A 00 01 06 FA 0A 01 02 64 01 00 00 1E 00 3C 01 00 00 00 01 00 9A
 * from: issue #732
 *       data: 01 50 1E 5A 46 12 64 00 06 FA 3C 03 05 64 00 00 00 28 00 41 03 00 00 00 00 00 00 00 00 00
 */
void Boiler::process_UBAParametersPlus(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, heatingActivated_, 0);
    has_update(telegram, heatingTemp_, 1);
    has_update(telegram, burnMaxPower_, 4);
    has_update(telegram, burnMinPower_, 5);
    has_update(telegram, boilHystOff_, 8);
    has_update(telegram, boilHystOn_, 9);
    has_update(telegram, burnMinPeriod_, 10);
    has_update(telegram, emergencyOps_, 18);
    has_update(telegram, emergencyTemp_, 19);

    // has_update(telegram, pumpType_, 11);   // guess, RC300 manual: power controlled, pressure controlled 1-4?
    // has_update(telegram, pumpDelay_, 12);  // guess
    has_update(telegram, pumpModMax_, 13);
    has_update(telegram, pumpModMin_, 14);
    has_update(telegram, pumpCharacter_, 15);
}

// 0xEA
// Boiler(0x08) -> (0x0B), (0xEA), data: 00 00 00 00 00 00 3C FB 00 28 00 02 46 00 00 00 3C 3C 28
void Boiler::process_UBAParameterWWPlus(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, wwSelTempOff_, 0); // confusing description in #96
    has_update(telegram, wwActivated_, 5);  // 0x01 means on
    has_update(telegram, wwSelTemp_, 6);    // setting here
    has_update(telegram, wwHystOn_, 7);
    has_update(telegram, wwHystOff_, 8);
    has_update(telegram, wwFlowTempOffset_, 9);
    has_update(telegram, wwCircPump_, 10);         // 0x01 means yes
    has_update(telegram, wwCircMode_, 11);         // 0=off, 1=1x3min... 6=6x3min, 7=continuous
    has_update(telegram, wwDisinfectionTemp_, 12); // setting here, status in E9
    has_update(telegram, wwAlternatingOper_, 14);  // 0x01 means enabled
    has_update(telegram, wwSelTempSingle_, 16);
    has_update(telegram, wwSelTempLow_, 18);
    has_update(telegram, wwMaxTemp_, 20);
    has_update(telegram, wwChargeOptimization_, 25);
    has_update(telegram, wwSelTempEcoplus_, 27);

    telegram->read_value(wwComfort2_, 26);
    uint8_t wwComfort1 = EMS_VALUE_UINT8_NOTSET;
    if (Helpers::hasValue(wwComfort2_)) {
        has_update(wwComfort1_, wwComfort2_);
    } else if (telegram->read_value(wwComfort1, 13)) {
        if (wwComfort1 == 0) {
            wwComfort1 = 0; // High_Comfort
        } else if (wwComfort1 == 0xD8) {
            wwComfort1 = 1; // Eco
        } else {
            wwComfort1 = EMS_VALUE_UINT8_NOTSET;
        }
        has_update(wwComfort1_, wwComfort1);
    }
}

// 0xE9 - WW monitor ems+
// e.g. 08 00 E9 00 37 01 F6 01 ED 00 00 00 00 41 3C 00 00 00 00 00 00 00 00 00 00 00 00 37 00 00 00 (CRC=77) #data=27
void Boiler::process_UBAMonitorWWPlus(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, wwSetTemp_, 0);
    has_update(telegram, wwCurTemp_, 1);
    has_update(telegram, wwCurTemp2_, 3);
    if (!is_received(0x779)) { // HIUMonitor
        has_update(telegram, wwCurFlow_, 11);
    }

    has_update(telegram, wwWorkM_, 14, 3);  // force to 3 bytes
    has_update(telegram, wwStarts_, 17, 3); // force to 3 bytes

    has_bitupdate(telegram, wwOneTime_, 12, 2);
    has_bitupdate(telegram, wwDisinfect_, 12, 3);
    has_bitupdate(telegram, wwCharging_, 12, 4);
    has_bitupdate(telegram, wwRecharging_, 13, 4);
    has_bitupdate(telegram, wwTempOK_, 13, 5);
    has_bitupdate(telegram, wwCirc_, 13, 2);

    // has_update(telegram, wwActivated_, 20); // Activated is in 0xEA, this is something other 0/100%
    // has_update(telegram, wwSelTemp_, 10);   // this is wrong, see #96
    // has_update(telegram, wwDisinfectionTemp_, 9);
}

/*
 * UBAInformation - type 0x495
 * all values 32 bit
 * 08 00 FF 00 03 95 00 0F 8E C2 00 08 39 C8 00 00 18 7A 00 07 3C 80 00 00 00 00 00 00 00 E5 F6 00
 * 08 00 FF 18 03 95 00 00 00 A1 00 00 00 00 00 00 00 44 00 00 00 00 00 00 00 0A 00 00 00 0A BD 00
 * 08 00 FF 30 03 95 00 00 00 00 00 00 00 00 00 00 02 10 00 00 00 00 00 00 02 1A 00 00 00 02 66 00
 * 08 00 FF 48 03 95 00 00 01 15 00 00 00 00 00 00 00 F9 29 00
 *
 */
void Boiler::process_UBAInformation(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, upTimeControl_, 0);
    has_update(telegram, upTimeCompHeating_, 8);
    has_update(telegram, upTimeCompCooling_, 16);
    has_update(telegram, upTimeCompWw_, 4);
    has_update(telegram, upTimeCompPool_, 12);

    has_update(telegram, totalCompStarts_, 20);
    has_update(telegram, heatingStarts_, 28);
    has_update(telegram, coolingStarts_, 36);
    has_update(telegram, wwStartsHp_, 24);
    has_update(telegram, poolStarts_, 32);

    has_update(telegram, nrgConsTotal_, 64);

    has_update(telegram, auxElecHeatNrgConsTotal_, 40);
    has_update(telegram, auxElecHeatNrgConsHeating_, 48);
    has_update(telegram, auxElecHeatNrgConsWw_, 44);
    has_update(telegram, auxElecHeatNrgConsPool_, 52);

    has_update(telegram, nrgConsCompTotal_, 56);
    has_update(telegram, nrgConsCompHeating_, 68);
    has_update(telegram, nrgConsCompWw_, 72);
    has_update(telegram, nrgConsCompCooling_, 76);
    has_update(telegram, nrgConsCompPool_, 80);
}

/*
 * UBAEnergy - type 0x494
 * Energy-values all 32bit
 * 08 00 FF 00 03 94 03 31 21 59 00 00 7C 70 00 00 15 B8 00 00 40 E3 00 00 27 23 FF FF FF FF EA
 * 08 00 FF 18 03 94 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 00 00 00 00 00 00 00 00 00 7E
 * 08 00 FF 31 03 94 00 00 00 00 00 00 00 38
 */
void Boiler::process_UBAEnergySupplied(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, upTimeTotal_, 0);
    has_update(telegram, nrgSuppTotal_, 4);
    has_update(telegram, nrgSuppHeating_, 12);
    has_update(telegram, nrgSuppWw_, 8);
    has_update(telegram, nrgSuppCooling_, 16);
    has_update(telegram, nrgSuppPool_, 20);
}

// Heatpump power - type 0x48D
//08 00 FF 00 03 8D 03 00 10 30 10 60 00 04 00 00 00 17 00 00 00 3C 38 0E 64 00 00 0C 33 C7 00
//XR1A050001   A05 Pump Heat circuit (1.0 ) 1 >> 1 & 0x01 ?
//XR1A040001   A04 Pump Cold circuit (1.0 ) 1 & 0x1 ?
void Boiler::process_HpPower(std::shared_ptr<const Telegram> telegram) {
    has_bitupdate(telegram, VC0valve_, 0, 7);
    has_bitupdate(telegram, hp3wayValve_, 0, 6);
    // has_bitupdate(telegram, heating_, 0, 0); // heating on? https://github.com/emsesp/EMS-ESP32/discussions/1898
    has_bitupdate(telegram, hpSwitchValve_, 0, 4);

    has_bitupdate(telegram, elHeatStep1_, 3, 0);
    has_bitupdate(telegram, elHeatStep2_, 3, 1);
    has_bitupdate(telegram, elHeatStep3_, 3, 2);
    has_bitupdate(telegram, hpCompOn_, 3, 4);
    has_bitupdate(telegram, hpEA0_, 3, 6);
    has_update(telegram, hpCircSpd_, 4);
    has_update(telegram, hpBrinePumpSpd_, 5);
    has_update(telegram, auxHeaterLevel_, 6);
    has_update(telegram, hpActivity_, 7);
    has_update(telegram, auxHeaterStatus_, 8);
    has_update(telegram, hpPower_, 10);
    has_update(telegram, hpCompSpd_, 17);

    has_bitupdate(telegram, hpInput[0].state, 1, 4);
    has_bitupdate(telegram, hpInput[1].state, 1, 5);
    has_bitupdate(telegram, hpInput[2].state, 1, 6);
    has_bitupdate(telegram, hpInput[3].state, 1, 7);

    // has_update(hpHeatingOn_, hpActivity_ == 1 ? 0xFF : 0);
    // has_update(hpCoolingOn_, hpActivity_ == 2 ? 0xFF : 0);
    // has_update(hpWwOn_, hpActivity_ == 3 ? 0xFF : 0);
    // has_update(hpPoolOn_, hpActivity_ == 4 ? 0xFF : 0);
    boilerState_ = hpActivity_ == 1 ? 0x09 : hpActivity_ == 3 ? 0x0A : 0;
    check_active(); // do a quick check to see if the hot water or heating is active
}

// Heatpump temperatures - type 0x48F
void Boiler::process_HpTemperatures(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, hpTc0_, 6);
    has_update(telegram, hpTc1_, 4);
    has_update(telegram, hpTc3_, 2);
    has_update(telegram, hpTr1_, 14);
    has_update(telegram, hpTr3_, 16);
    has_update(telegram, hpTr4_, 18);
    has_update(telegram, hpTr5_, 20);
    has_update(telegram, hpTr6_, 0);
    has_update(telegram, hpTr7_, 30);
    has_update(telegram, hpTl2_, 12);
    has_update(telegram, hpTa4_, 22);
    has_update(telegram, hpTw1_, 24);
    has_update(telegram, hpPl1_, 26);
    has_update(telegram, hpPh1_, 28);
    has_update(telegram, hpBrineIn_, 8);
    has_update(telegram, hpBrineOut_, 10);
}

// Heatpump pool unit - type 0x48A
// 08 00 FF 00 03 8A 01 4C 01 0C 00 00 0A 00 1E 00 00 01 00 04 4A 00
void Boiler::process_HpPool(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, poolSetTemp_, 1);
}

// Heatpump inputs - type 0x4A2
// Boiler(0x08) -> All(0x00), ?(0x04A2), data: 02 01 01 00 01 00
// Boiler(0x08) -W-> Me(0x0B), HpInput(0x04A2), data: 20 07 06 01 00 (from #802)
// fix states 1/3 see https://github.com/emsesp/EMS-ESP32/issues/1388
void Boiler::process_HpInput(std::shared_ptr<const Telegram> telegram) {
    has_bitupdate(telegram, hp4wayValve_, 0, 7);
    // has_update(telegram, hpInput[0].state, 2);
    // has_bitupdate(telegram, hpInput[1].state, 0, 1);
    // has_update(telegram, hpInput[2].state, 3);
    // has_bitupdate(telegram, hpInput[3].state, 4, 0);
}

// Heatpump inputs settings- type 0x486 (https://github.com/emsesp/EMS-ESP32/issues/600)
// Boiler(0x08) -> All(0x00), ?(0x0486), data: 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
// Boiler(0x08) -> All(0x00), ?(0x0486), data: 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 01 01 00 00 00 00 00 (offset 25)
// Boiler(0x08) -> All(0x00), ?(0x0486), data: 00 00 (offset 51)
void Boiler::process_HpInConfig(std::shared_ptr<const Telegram> telegram) {
    char option[16];
    // inputs 1,2,3 <inv>[<evu1><evu2><evu3><comp><aux><cool><heat><dhw><pv><prot><pres><mod>]
    uint8_t index[] = {0, 3, 6, 9, 12, 15, 18, 21, 24, 39, 36, 30, 27};
    for (uint8_t i = 0; i < 3; i++) {
        for (uint8_t j = 0; j < 12; j++) {
            option[j] = hpInput[i].option[j] - '0';
            telegram->read_value(option[j], index[j] + i);
            option[j] = option[j] == 1 ? '1' : '0';
        }
        option[12] = atoi(&hpInput[i].option[12]);
        telegram->read_value(option[12], 27 + i); // modulation
        Helpers::smallitoa(&option[12], (uint16_t)option[12]);
        has_update(hpInput[i].option, option, 16);
    }
    // input 4 <inv>[<comp><aux><cool><heat><dhw><pv><prot><pres><mod>]
    uint8_t index4[] = {42, 43, 44, 45, 46, 47, 52, 50, 49, 48};
    for (uint8_t j = 0; j < 9; j++) {
        option[j] = hpInput[3].option[j] - '0';
        telegram->read_value(option[j], index4[j]);
        option[j] = option[j] == 1 ? '1' : '0';
    }
    option[9] = atoi(&hpInput[3].option[9]);
    telegram->read_value(option[9], 48); // modulation
    Helpers::smallitoa(&option[9], (uint16_t)option[9]);
    has_update(hpInput[3].option, option, 13);
}

// Boiler(0x08) -W-> Me(0x0B), HpHeaterConfig(0x0485)
void Boiler::process_HpCooling(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, pvCooling_, 21);
}

// Boiler(0x08) -W-> Me(0x0B), HpHeaterConfig(0x0492), data: 03 00 00 04 00
void Boiler::process_HpHeaterConfig(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, maxHeatComp_, 2);
    has_update(telegram, maxHeatHeat_, 3);
    has_update(telegram, maxHeatDhw_, 4);
}

// 0x2A - MC110Status
// e.g. 88 00 2A 00 00 00 00 00 00 00 00 00 D2 00 00 80 00 00 01 08 80 00 02 47 00
// see https://github.com/emsesp/EMS-ESP/issues/397
void Boiler::process_MC110Status(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, wwMixerTemp_, 14);
    has_update(telegram, wwCylMiddleTemp_, 18);
}

/*
 * UBAOutdoorTemp - type 0xD1 - external temperature EMS+
 */
void Boiler::process_UBAOutdoorTemp(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, outdoorTemp_, 0);
}

// UBASetPoint 0x1A
void Boiler::process_UBASetPoints(std::shared_ptr<const Telegram> telegram) {
    uint8_t setFlowTemp_  = 0;
    uint8_t setBurnPow_   = 0;
    uint8_t wwSetBurnPow_ = 0;
    telegram->read_value(setFlowTemp_, 0);
    telegram->read_value(setBurnPow_, 1);
    telegram->read_value(wwSetBurnPow_, 2);

    // overwrite other settings on receive?
    if (forceHeatingOff_ == EMS_VALUE_BOOL_ON && telegram->dest == 0x08 && (setFlowTemp_ + setBurnPow_ + wwSetBurnPow_) != 0) {
        uint8_t data[] = {0, 0, 0, 0};
        write_command(EMS_TYPE_UBASetPoints, 0, data, sizeof(data), 0);
    }
}

// 0x35 - not yet implemented, not readable, only for settings
void Boiler::process_UBAFlags(std::shared_ptr<const Telegram> telegram) {
}

// 0x1C
// 08 00 1C 94 0B 0A 1D 31 08 00 80 00 00 00 -> message for 29.11.2020
// 08 00 1C 94 0B 0A 1D 31 00 00 00 00 00 00 -> message reset
void Boiler::process_UBAMaintenanceStatus(std::shared_ptr<const Telegram> telegram) {
    // 5. byte: Maintenance due (0 = no, 3 = yes, due to operating hours, 8 = yes, due to date)
    uint8_t message_code = maintenanceMessage_[2] - '0';
    telegram->read_value(message_code, 5);

    if (Helpers::hasValue(message_code)) {
        char message[5];
        snprintf(message, sizeof(message), "H%02d", message_code);
        has_update(maintenanceMessage_, message, sizeof(maintenanceMessage_));
    }
}

// 0xBF
void Boiler::process_ErrorMessage(std::shared_ptr<const Telegram> telegram) {
    EMSESP::send_read_request(0xC2, device_id()); // read last errorcode
}

// 0x10, 0x11
void Boiler::process_UBAErrorMessage(std::shared_ptr<const Telegram> telegram) {
    if (telegram->offset > 0 || telegram->message_length < 11) {
        return;
    }
    // data: displaycode(2), errornumber(2), year, month, hour, day, minute, duration(2), src-addr
    static uint32_t lastCodeDate_ = 0; // last code date
    uint8_t         code[3]       = {telegram->message_data[0], telegram->message_data[1], 0};
    uint16_t        codeNo        = telegram->message_data[2] * 256 + telegram->message_data[3];
    uint16_t        year          = (telegram->message_data[4] & 0x7F) + 2000;
    uint8_t         month         = telegram->message_data[5];
    uint8_t         day           = telegram->message_data[7];
    uint8_t         hour          = telegram->message_data[6];
    uint8_t         min           = telegram->message_data[8];
    uint16_t        duration      = telegram->message_data[9] * 256 + telegram->message_data[10];
    uint32_t        date          = (year - 2000) * 535680UL + month * 44640UL + day * 1440UL + hour * 60 + min + duration;
    // check valid https://github.com/emsesp/EMS-ESP32/issues/2189
    if (day == 0 || day > 31 || month == 0 || month > 12 || !std::isprint(code[0]) || !std::isprint(code[1])) {
        if (!lastCodeDate_) {
            char newCode[sizeof(lastCode_)];
            snprintf(newCode, sizeof(lastCode_), "%s(%d)", code, codeNo);
            has_update(lastCode_, newCode, sizeof(lastCode_));
        }
        return;
    }
    // store only the newest code from telegrams 10 and 11
    if (date > lastCodeDate_) {
        lastCodeDate_ = date;
        char newCode[sizeof(lastCode_)];
        snprintf(newCode, sizeof(lastCode_), "%s(%d) %02d.%02d.%d %02d:%02d (%d min)", code, codeNo, day, month, year, hour, min, duration);
        has_update(lastCode_, newCode, sizeof(lastCode_));
    }
}

// 0xC2, without clock in system it stores 3 bytes uptime in 11 and 16, with clock date in 10-14, and 15-19
// date is marked with 0x80 to year-field
// also C6, C7 https://github.com/emsesp/EMS-ESP32/issues/938#issuecomment-1425813815
void Boiler::process_UBAErrorMessage2(std::shared_ptr<const Telegram> telegram) {
    if (telegram->offset > 0 || telegram->message_length < 20) {
        return;
    }

    static uint32_t lastCodeDate_           = 0; // last code date
    uint32_t        date                    = 0;
    char            code[sizeof(lastCode_)] = {0};
    uint16_t        codeNo                  = EMS_VALUE_INT16_NOTSET;
    code[0]                                 = telegram->message_data[5];
    code[1]                                 = telegram->message_data[6];
    code[2]                                 = telegram->message_data[7];
    code[3]                                 = 0;
    telegram->read_value(codeNo, 8);
    if (!std::isprint(code[0]) || !std::isprint(code[1]) || !std::isprint(code[2])) {
        return;
    }

    // check for valid date, https://github.com/emsesp/EMS-ESP32/issues/204
    if (telegram->message_data[10] & 0x80) {
        uint16_t start_year  = (telegram->message_data[10] & 0x7F) + 2000;
        uint8_t  start_month = telegram->message_data[11];
        uint8_t  start_day   = telegram->message_data[13];
        uint8_t  start_hour  = telegram->message_data[12];
        uint8_t  start_min   = telegram->message_data[14];
        uint16_t end_year    = (telegram->message_data[15] & 0x7F) + 2000;
        uint8_t  end_month   = telegram->message_data[16];
        uint8_t  end_day     = telegram->message_data[18];
        uint8_t  end_hour    = telegram->message_data[17];
        uint8_t  end_min     = telegram->message_data[19];

        if (telegram->message_data[15] & 0x80) { //valid end date
            date = (end_year - 2000) * 535680UL + end_month * 44640UL + end_day * 1440UL + end_hour * 60 + end_min;
            snprintf(&code[3],
                     sizeof(code) - 3,
                     "(%d) %02d.%02d.%04d %02d:%02d - %02d.%02d.%04d %02d:%02d",
                     codeNo,
                     start_day,
                     start_month,
                     start_year,
                     start_hour,
                     start_min,
                     end_day,
                     end_month,
                     end_year,
                     end_hour,
                     end_min);
        } else { // no valid end date means error still persists
            date = (start_year - 2000) * 535680UL + start_month * 44640UL + start_day * 1440UL + start_hour * 60 + start_min;
            snprintf(&code[3], sizeof(code) - 3, "(%d) %02d.%02d.%04d %02d:%02d - now", codeNo, start_day, start_month, start_year, start_hour, start_min);
        }
    } else { // no clock, the uptime is stored https://github.com/emsesp/EMS-ESP32/issues/121
        uint32_t starttime = 0;
        uint32_t endtime   = 0;
        telegram->read_value(starttime, 11, 3);
        telegram->read_value(endtime, 16, 3);
        snprintf(&code[3], sizeof(code) - 3, "(%d) @uptime %lu - %lu min", codeNo, starttime, endtime);
        date = starttime;
    }
    if (date > lastCodeDate_) {
        lastCodeDate_ = date;
        has_update(lastCode_, code, sizeof(lastCode_));
    }
}

// 0x15 maintenance data
void Boiler::process_UBAMaintenanceData(std::shared_ptr<const Telegram> telegram) {
    if (telegram->offset > 0 || telegram->message_length < 5) {
        return;
    }

    // added additional type 3 (for Nefit TrendLine HRC 30/CW5)
    has_update(telegram, maintenanceType_, 0); // 0 = off, 1 = by operating hours, 2 = by date, 3 = manual

    uint8_t time = (maintenanceTime_ == EMS_VALUE_UINT16_NOTSET) ? EMS_VALUE_UINT8_NOTSET : maintenanceTime_ / 100;
    telegram->read_value(time, 1);
    if (Helpers::hasValue(time)) {
        if (time * 100 != maintenanceTime_) {
            maintenanceTime_ = time * 100;
            has_update(&maintenanceTime_);
        }
    }

    // date only
    uint8_t day   = telegram->message_data[2];
    uint8_t month = telegram->message_data[3];
    uint8_t year  = telegram->message_data[4];
    if (day > 0 && month > 0) {
        char date[20];
        snprintf(date, sizeof(date), "%02d.%02d.%04d", day, month, year + 2000);
        has_update(maintenanceDate_, date, sizeof(maintenanceDate_));
    }
}

// Boiler(0x08) -> All(0x00), ?(0x0484), data: 00 00 14 28 0D 50 00 00 00 02 02 07 28 01 00 02 05 19 0A 0A 03 0D 07 00 0A
// Boiler(0x08) -> All(0x00), ?(0x0484), data: 01 90 00 F6 28 14 64 00 00 E1 00 1E 00 1E 01 64 01 64 54 20 00 00 (offset 25)
void Boiler::process_HpSilentMode(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, wwAltOpPrioHeat_, 2); // range 20-120 minutes on Buderus WSW196i
    has_update(telegram, wwAltOpPrioWw_, 3);   // range 30-120 minutes on Buderus WSW196i
    has_update(telegram, silentMode_, 10);     // enum off-auto-on
    has_update(telegram, minTempSilent_, 11);
    has_update(telegram, hpHystHeat_, 37); // is / 5
    has_update(telegram, hpHystCool_, 35); // is / 5, maybe offset swapped with pool
    has_update(telegram, hpHystPool_, 33); // is / 5
    has_update(telegram, hpCircPumpWw_, 46);
    has_update(telegram, hpMaxPower_, 31);
    has_update(telegram, silentFrom_, 52);     // in steps of 15 min
    has_update(telegram, silentTo_, 53);       // in steps of 15 min
    has_update(telegram, pvMaxComp_, 54);      // #2062
    has_update(telegram, hpshutdown_, 58);     // 1 powers off
    has_update(telegram, powerReduction_, 64); // 3..6 -> is *10
}

// Boiler(0x08) -B-> All(0x00), ?(0x0488), data: 8E 00 00 00 00 00 01 03
void Boiler::process_HpValve(std::shared_ptr<const Telegram> telegram) {
    // has_bitupdate(telegram, auxHeaterStatus_, 0, 2);
    has_update(telegram, auxHeatMixValve_, 7);
    has_update(telegram, pc1Rate_, 13); // percent
}

// Boiler(0x08) -B-> All(0x00), ?(0x048B), data: 00 00 0A 1E 4E 00 1E 01 2C 00 01 64 55 05 12 50 50 50 00 00 1E 01 2C 00
// Boiler(0x08) -B-> All(0x00), ?(0x048B), data: 00 1E 00 96 00 1E (offset 24)
void Boiler::process_HpPumps(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, tempDiffHeat_, 4); // is * 10
    has_update(telegram, tempDiffCool_, 3); // is * 10
    has_update(telegram, hpPumpMode_, 18);
}

// 0x02D6, https://github.com/emsesp/EMS-ESP32/issues/2001
void Boiler::process_HpPump2(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, pc1On_, 0);
    has_update(telegram, pc1Flow_, 9);
}

// Boiler(0x08) -> All(0x00), ?(0x0491), data: 03 01 00 00 00 02 64 00 00 14 01 2C 00 0A 00 1E 00 1E 00 00 1E 0A 1E 05 05
void Boiler::process_HpAdditionalHeater(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, manDefrost_, 0); // off/on
    has_update(telegram, auxHeaterOnly_, 1);
    has_update(telegram, auxHeaterOff_, 2);
    has_update(telegram, auxHeatMode_, 4); // eco/comfort
    has_update(telegram, tempParMode_, 5);
    has_update(telegram, auxMaxLimit_, 14);    // is * 10
    has_update(telegram, auxLimitStart_, 15);  // is * 10
    has_update(telegram, auxHeaterDelay_, 16); // is / 10
}

// DHW 0x499
// Boiler(0x08) -B-> All(0x00), ?(0x0499), data: 31 33 3F 3B 01
void Boiler::process_HpDhwSettings(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, wwComfOffTemp_, 1);
    has_update(telegram, wwEcoOffTemp_, 0);
    has_update(telegram, wwEcoPlusOffTemp_, 5);
    // https://github.com/emsesp/EMS-ESP32/issues/1597
    has_update(telegram, wwComfDiffTemp_, 12);
    has_update(telegram, wwEcoDiffTemp_, 13);
    has_update(telegram, wwEcoPlusDiffTemp_, 14);
    //https://github.com/emsesp/EMS-ESP32/issues/1624
    has_update(telegram, wwComfStopTemp_, 8);
    has_update(telegram, wwEcoStopTemp_, 9);
    has_update(telegram, wwEcoPlusStopTemp_, 10);
}

// 0x49C:
// Boiler(0x08) -B-> All(0x00), ?(0x049C), data: 00 00 00 00
void Boiler::process_HpSettings2(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, vp_cooling_, 3);
}

// 0x49D
// Boiler(0x08) -B-> All(0x00), ?(0x049D), data: 00 00 00 00 00 00 00 00 00 00 00 00
void Boiler::process_HpSettings3(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, heatCable_, 2);
    // has_update(telegram, VC0valve_, 3); // read in 48D
    has_update(telegram, primePump_, 4);
    has_update(telegram, primePumpMod_, 5);
    // has_update(telegram, hp3wayValve_, 6); // read in 48D
    // has_update(telegram, elHeatStep1_, 7);
    // has_update(telegram, elHeatStep2_, 8);
    // has_update(telegram, elHeatStep3_, 9);
}

// boiler(0x08) -W-> Me(0x0B), ?(0x04AE), data: 00 00 BD C4 00 00 5B 6A 00 00 00 24 00 00 62 59 00 00 00 00 00 00 00 00
// boiler(0x08) -W-> Me(0x0B), ?(0x04AE), data: 00 00 00 00 00 00 00 00 (offset 24)
void Boiler::process_HpEnergy(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, nrgTotal_, 0);
    has_update(telegram, nrgHeat_, 4);
    has_update(telegram, nrgWw_, 12);
    has_update(telegram, nrgCool_, 20);
}

// boiler(0x08) -W-> Me(0x0B), ?(0x04AF), data: 00 00 48 B2 00 00 48 55 00 00 00 5D 00 00 01 78 00 00 00 00 00 00 07 61
// boiler(0x08) -W-> Me(0x0B), ?(0x04AF), data: 00 00 24 B0 00 00 00 12 00 00 23 A5 00 00 00 4B 00 00 00 00 00 00 00 00 (offset 24)
// boiler(0x08) -W-> Me(0x0B), ?(0x04AF), data: 00 00 00 00 00 00 00 00 (offset 48)
void Boiler::process_HpMeters(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, meterTotal_, 0);
    has_update(telegram, meterComp_, 4);
    has_update(telegram, meterEHeat_, 8);
    has_update(telegram, meterHeat_, 24);
    has_update(telegram, meterWw_, 32);
    has_update(telegram, meterCool_, 40);
}

void Boiler::process_HpPressure(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, hpSetDiffPress_, 9);
}

// boiler(0x08) -W-> Me(0x0B), ?(0x04A5), data: 00 00 3C 1D 09 0A 0A 01 00 28 0A 00 01 00 00
void Boiler::process_HpFan(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, fan_, 9);
}

// 0x4AA
void Boiler::process_HpPower2(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, hpCurrPower_, 0);
}

// 0x4A7
void Boiler::process_HpPowerLimit(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, hpPowerLimit_, 0);
}

// Boiler(0x08) -B-> All(0x00), ?(0x2E), data: 00 00 1C CE 00 00 05 E8 00 00 00 18 00 00 00 02
void Boiler::process_Meters(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, gasMeterHeat_, 0);
    has_update(telegram, gasMeterWw_, 4);
    has_update(telegram, meterHeat_, 8);
    has_update(telegram, meterWw_, 12);
}

// boiler(0x08) -B-> All(0x00), ?(0x3B), data: 00 00 1B D1 00 00 05 7F
void Boiler::process_Energy(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, nrgHeat2_, 0);
    has_update(telegram, nrgWw2_, 4);
}

// HIU unit

// boiler(0x08) -B-> All(0x00), ?(0x0779), data: 06 05 01 01 AD 02 EF FF FF 00 00 7F FF
void Boiler::process_HIUMonitor(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, retTemp_, 3);     // is * 10
    has_update(telegram, netFlowTemp_, 5); // is * 10
    has_update(telegram, heatValve_, 7);   // is %
    has_update(telegram, wwValve_, 8);     // is %
    has_update(telegram, wwCurFlow_, 10);  // is * 10 (for HIU 16 bit at offset 9, use 8 bit for compatibility to other boilers)
}

// Boiler(0x08) -W-> ME(0x0x), ?(0x0772), data: 00 00 00 00 00
void Boiler::process_HIUSettings(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, keepWarmTemp_, 1);
    has_update(telegram, setReturnTemp_, 2);
}

// Weather compensation, #1642
// boiler(0x08) -W-> Me(0x0B), ?(0x28), data: 00 3C 32 10 00 05
void Boiler::process_WeatherComp(std::shared_ptr<const Telegram> telegram) {
    has_update(telegram, curveOn_, 0);
    has_update(telegram, curveEnd_, 1);
    has_update(telegram, curveBase_, 2);
    has_update(telegram, summerTemp_, 3);
    has_update(telegram, nofrost_, 4);
    has_update(telegram, nofrostTemp_, 5);
}

// HIU Settings
bool Boiler::set_keepWarmTemp(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2temperature(value, v)) {
        return false;
    }
    write_command(0x772, 1, v, 0x772);
    return true;
}

bool Boiler::set_returnTemp(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2temperature(value, v)) {
        return false;
    }
    write_command(0x772, 2, v, 0x772);
    return true;
}

/*
 * Hybrid heatpump with telegram 0xBB is readable and writeable in boiler and thermostat
 * thermostat always overwrites settings in boiler
 * enable settings here if no thermostat is used in system
 *
// 0xBB Heatpump optimization
// Boiler(0x08) -> Me(0x0B), ?(0xBB), data: 00 00 00 00 00 00 00 00 00 00 00 FF 02 0F 1E 0B 1A 00 14 03
void Boiler::process_HybridHp(std::shared_ptr<const Telegram> telegram) {
    has_enumupdate(telegram, hybridStrategy_, 12, 1); // cost = 2, temperature = 3, mix = 4
    has_update(telegram, switchOverTemp_, 13);      // full degrees
    has_update(telegram, energyCostRatio_, 14);       // is *10
    has_update(telegram, fossileFactor_, 15);       // is * 10
    has_update(telegram, electricFactor_, 16);      // is * 10
    has_update(telegram, delayBoiler_, 18);          // minutes
    has_update(telegram, tempDiffBoiler_, 19);      // relative degrees
}
*/
/*
 * Settings
 */
/*
bool Boiler::set_hybridStrategy(const char * value, const int8_t id) {
    uint8_t v;
    if (!Helpers::value2enum(value, v, FL_(enum_hybridStrategy))) {
        return false;
    }
    write_command(0xBB, 12, v + 1, 0xBB);
    return true;
}

bool Boiler::set_switchOverTemp(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2temperature(value, v)) {
        return false;
    }
    write_command(0xBB, 13, v, 0xBB);
    return true;
}

bool Boiler::set_energyCostRatio(const char * value, const int8_t id) {
    float v;
    if (!Helpers::value2float(value, v)) {
        return false;
    }
    write_command(0xBB, 14, (uint8_t)(v * 10), 0xBB);
    return true;
}

bool Boiler::set_fossileFactor(const char * value, const int8_t id) {
    float v;
    if (!Helpers::value2float(value, v)) {
        return false;
    }
    write_command(0xBB, 15, (uint8_t)(v * 10), 0xBB);
    return true;
}

bool Boiler::set_electricFactor(const char * value, const int8_t id) {
    float v;
    if (!Helpers::value2float(value, v)) {
        return false;
    }
    write_command(0xBB, 16, (uint8_t)(v * 10), 0xBB);
    return true;
}

bool Boiler::set_delayBoiler(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }
    write_command(0xBB, 18, v, 0xBB);
    return true;
}

bool Boiler::set_tempDiffBoiler(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2temperature(value, v, true)) {
        return false;
    }
    write_command(0xBB, 19, v, 0xBB);
    return true;
}
*/

// Set the dhw temperature 0x33/0x35 or 0xEA
bool Boiler::set_ww_temp(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2temperature(value, v)) {
        return false;
    }

    if (is_received(EMS_TYPE_UBAParameterWWPlus)) {
        // write_command(EMS_TYPE_UBAFlags, 3, v, EMS_TYPE_UBAParameterWWPlus); // test for #96
        write_command(EMS_TYPE_UBAParameterWWPlus, 6, v, EMS_TYPE_UBAParameterWWPlus);
    } else {
        // some boiler have it in UBAParametersWW 0x33/0xEA, some in 0x35
        write_command(EMS_TYPE_UBAFlags, 3, v, EMS_TYPE_UBAParameterWW);       // for i9000, see #397
        write_command(EMS_TYPE_UBAParameterWW, 2, v, EMS_TYPE_UBAParameterWW); // read seltemp back
    }

    return true;
}


// Set the lower dhw temperature 0xEA
bool Boiler::set_ww_temp_low(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2temperature(value, v)) {
        return false;
    }

    write_command(EMS_TYPE_UBAParameterWWPlus, 18, v, EMS_TYPE_UBAParameterWWPlus);
    return true;
}

// Set the eco+ dhw temperature 0xEA
bool Boiler::set_ww_temp_eco(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2temperature(value, v)) {
        return false;
    }

    write_command(EMS_TYPE_UBAParameterWWPlus, 27, v, EMS_TYPE_UBAParameterWWPlus);
    return true;
}

// Set the dhw single charge temperature 0xEA
bool Boiler::set_ww_temp_single(const char * value, const int8_t id) {
    int v = 0;
    if (!Helpers::value2temperature(value, v)) {
        return false;
    }

    write_command(EMS_TYPE_UBAParameterWWPlus, 16, v, EMS_TYPE_UBAParameterWWPlus);
    return true;
}

// Set the dhw disinfection temperature
bool Boiler::set_ww_disinfect_temp(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2temperature(value, v)) {
        return false;
    }

    if (is_received(EMS_TYPE_UBAParameterWWPlus)) {
        write_command(EMS_TYPE_UBAParameterWWPlus, 12, v, EMS_TYPE_UBAParameterWWPlus);
    } else {
        write_command(EMS_TYPE_UBAParameterWW, 8, v, EMS_TYPE_UBAParameterWW);
    }

    return true;
}

// flow temp
bool Boiler::set_flow_temp(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2temperature(value, v)) {
        return false;
    }

    // no verify if value is unchanged, put it  to end of tx-queue, no priority
    // see https://github.com/emsesp/EMS-ESP32/issues/654, https://github.com/emsesp/EMS-ESP32/issues/954
    if (v == selFlowTemp_) {
        EMSESP::txservice_.add(Telegram::Operation::TX_WRITE, device_id(), EMS_TYPE_UBASetPoints, 0, (uint8_t *)&v, 1, 0, false);
        return true;
    }

    if (has_telegram_id(0xE4)) {
        write_command(EMS_TYPE_UBASetPoints, 0, v, 0xE4);
    } else {
        write_command(EMS_TYPE_UBASetPoints, 0, v, 0x18);
    }
    return true;
}

// set selected burner power
bool Boiler::set_burn_power(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }

    write_command(EMS_TYPE_UBASetPoints, 1, v, EMS_TYPE_UBASetPoints);

    return true;
}

// Set the dhw flow temperature offset 0x33
bool Boiler::set_ww_flowTempOffset(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2temperature(value, v, true)) {
        return false;
    }

    if (is_received(EMS_TYPE_UBAParameterWWPlus)) {
        write_command(EMS_TYPE_UBAParameterWWPlus, 9, v, EMS_TYPE_UBAParameterWWPlus);
    } else {
        write_command(EMS_TYPE_UBAParameterWW, 5, v, EMS_TYPE_UBAParameterWW);
    }

    return true;
}

// set heating activated
bool Boiler::set_heating_activated(const char * value, const int8_t id) {
    bool v;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }

    if (is_received(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParametersPlus, 0, v ? 0x01 : 0, EMS_TYPE_UBAParametersPlus);
    } else {
        write_command(EMS_TYPE_UBAParameters, 0, v ? 0xFF : 0, EMS_TYPE_UBAParameters);
    }

    return true;
}

// set heating maximum temperature
bool Boiler::set_heating_temp(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2temperature(value, v)) {
        return false;
    }

    if (is_received(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParametersPlus, 1, v, EMS_TYPE_UBAParametersPlus);
    } else {
        write_command(EMS_TYPE_UBAParameters, 1, v, EMS_TYPE_UBAParameters);
    }

    return true;
}

// set min boiler output
bool Boiler::set_min_power(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }

    if (is_received(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParametersPlus, 5, v, EMS_TYPE_UBAParametersPlus);
    } else {
        write_command(EMS_TYPE_UBAParameters, 3, v, EMS_TYPE_UBAParameters);
    }

    return true;
}

// set max boiler output
bool Boiler::set_max_power(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }

    if (is_received(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParametersPlus, 4, v, EMS_TYPE_UBAParametersPlus);
    } else {
        write_command(EMS_TYPE_UBAParameters, 2, v, EMS_TYPE_UBAParameters);
    }

    return true;
}

// set ww on hysteresis
bool Boiler::set_ww_hyst_on(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2temperature(value, v, true)) {
        return false;
    }

    if (is_received(EMS_TYPE_UBAParameterWWPlus)) {
        write_command(EMS_TYPE_UBAParameterWWPlus, 7, v, EMS_TYPE_UBAParameterWWPlus);
    } else {
        write_command(EMS_TYPE_UBAParameterWW, 3, v, EMS_TYPE_UBAParameterWW);
    }

    return true;
}

// set ww off hysteresis
bool Boiler::set_ww_hyst_off(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2temperature(value, v, true)) {
        return false;
    }

    if (is_received(EMS_TYPE_UBAParameterWWPlus)) {
        write_command(EMS_TYPE_UBAParameterWWPlus, 8, v, EMS_TYPE_UBAParameterWWPlus);
    } else {
        write_command(EMS_TYPE_UBAParameterWW, 4, v, EMS_TYPE_UBAParameterWW);
    }

    return true;
}

// set ww charge optimization
bool Boiler::set_ww_chargeOptimization(const char * value, const int8_t id) {
    bool v;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }

    if (is_received(EMS_TYPE_UBAParameterWWPlus)) {
        write_command(EMS_TYPE_UBAParameterWWPlus, 25, v ? 1 : 0, EMS_TYPE_UBAParameterWWPlus);
    }

    return true;
}

// set dhw max power
bool Boiler::set_ww_maxpower(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }

    write_command(EMS_TYPE_UBASettingsWW, 7, v, EMS_TYPE_UBASettingsWW);

    return true;
}

// set dhw maximum temperature
bool Boiler::set_ww_maxtemp(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }

    write_command(EMS_TYPE_UBAParameterWWPlus, 20, v, EMS_TYPE_UBAParameterWWPlus);

    return true;
}

// set min pump modulation
bool Boiler::set_min_pump(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }

    if (is_received(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParametersPlus, 14, v, EMS_TYPE_UBAParametersPlus);
    } else {
        write_command(EMS_TYPE_UBAParameters, 10, v, EMS_TYPE_UBAParameters);
    }

    return true;
}

// set max pump modulation
bool Boiler::set_max_pump(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }

    if (is_received(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParametersPlus, 13, v, EMS_TYPE_UBAParametersPlus);
    } else {
        write_command(EMS_TYPE_UBAParameters, 9, v, EMS_TYPE_UBAParameters);
    }

    return true;
}

bool Boiler::set_pumpMode(const char * value, const int8_t id) {
    uint8_t v;
    if (is_received(EMS_TYPE_UBAParametersPlus)) {
        if (Helpers::value2enum(value, v, FL_(enum_pumpCharacter))) {
            write_command(EMS_TYPE_UBAParametersPlus, 15, v, EMS_TYPE_UBAParametersPlus);
            return true;
        }
    } else {
        if (Helpers::value2enum(value, v, FL_(enum_pumpMode))) {
            write_command(EMS_TYPE_UBAParameters, 11, v, EMS_TYPE_UBAParameters);
            return true;
        }
    }
    return false;
}

bool Boiler::set_pumpCharacter(const char * value, const int8_t id) {
    uint8_t v;
    if (Helpers::value2enum(value, v, FL_(enum_pumpCharacter))) {
        write_command(EMS_TYPE_UBAParametersPlus, 15, v, EMS_TYPE_UBAParametersPlus);
        return true;
    }
    return false;
}

// set boiler on hysteresis
bool Boiler::set_hyst_on(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2temperature(value, v, true)) {
        return false;
    }

    if (is_received(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParametersPlus, 9, v, EMS_TYPE_UBAParametersPlus);
    } else {
        write_command(EMS_TYPE_UBAParameters, id == 2 ? 13 : 5, v, EMS_TYPE_UBAParameters);
    }

    return true;
}

// set boiler off hysteresis
bool Boiler::set_hyst_off(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2temperature(value, v, true)) {
        return false;
    }

    if (is_received(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParametersPlus, 8, v, EMS_TYPE_UBAParametersPlus);
    } else {
        write_command(EMS_TYPE_UBAParameters, id == 2 ? 12 : 4, v, EMS_TYPE_UBAParameters);
    }

    return true;
}

// set min burner period
bool Boiler::set_burn_period(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }

    if (is_received(EMS_TYPE_UBAParametersPlus)) {
        write_command(EMS_TYPE_UBAParametersPlus, 10, v, EMS_TYPE_UBAParametersPlus);
    } else {
        write_command(EMS_TYPE_UBAParameters, 6, v, EMS_TYPE_UBAParameters);
    }

    return true;
}

// set pump delay
bool Boiler::set_pump_delay(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }

    if (is_received(EMS_TYPE_UBAParameters)) {
        write_command(EMS_TYPE_UBAParameters, 8, v, EMS_TYPE_UBAParameters);
        return true;
    }

    return false;
}

// set pump logic temperature
bool Boiler::set_pumpOnTemp(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2temperature(value, v)) {
        return false;
    }

    write_command(EMS_TYPE_UBAParameters, 23, v, EMS_TYPE_UBAParameters);
    return true;
}

// note some boilers do not have this setting, than it's done by thermostat
// on a RC35 it's by EMSESP::send_write_request(0x37, 0x10, 2, &set, 1, 0); (set is 1,2,3) 1=hot, 2=eco, 3=intelligent
// on a RC310 it's 1=high, 2=eco
// eco key on controller: tele.05, offset 70: 0xAA =eco, 0x55= comfort, https://github.com/emsesp/EMS-ESP32/issues/908
bool Boiler::set_ww_mode(const char * value, const int8_t id) {
    uint8_t set;
    uint8_t comfort[] = {0x00, 0xD8, 0xEC}; // heat, eco, intelligent

    if (Helpers::hasValue(wwComfort2_)) {
        if (Helpers::value2enum(value, set, FL_(enum_comfort1))) {
            write_command(EMS_TYPE_UBAParameterWWPlus, 26, set, EMS_TYPE_UBAParameterWWPlus);
            return true;
        }
    } else if (is_received(EMS_TYPE_UBAParameterWWPlus)) {
        if (Helpers::value2enum(value, set, FL_(enum_comfort1))) {
            write_command(EMS_TYPE_UBAParameterWWPlus, 13, comfort[set], EMS_TYPE_UBAParameterWWPlus);
            write_command(0x05, 70, set == 1 ? 0xAA : 0x55); //
            return true;
        }
    } else {
        if (Helpers::value2enum(value, set, FL_(enum_comfort))) {
            write_command(EMS_TYPE_UBAParameterWW, 9, comfort[set], EMS_TYPE_UBAParameterWW);
            return true;
        }
    }
    return false;
}

// turn on/off dhw
bool Boiler::set_ww_activated(const char * value, const int8_t id) {
    bool v;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }

    // https://github.com/emsesp/EMS-ESP/issues/268
    // 08 for HT3 seems to be wrong, see https://github.com/emsesp/EMS-ESP32/issues/89
    if (is_received(EMS_TYPE_UBAParameterWWPlus)) {
        write_command(EMS_TYPE_UBAParameterWWPlus, 5, v ? 1 : 0, EMS_TYPE_UBAParameterWWPlus);
    } else {
        write_command(EMS_TYPE_UBAParameterWW, 1, v ? 0xFF : 0, EMS_TYPE_UBAParameterWW);
    }

    return true;
}

// Activate / De-activate the Warm Tap Water
// Note: Using the type 0x1D to put the boiler into Test mode. This may be shown on the boiler with a flashing 'T'
bool Boiler::set_tapwarmwater_activated(const char * value, const int8_t id) {
    // as it's a command it may not initially exist
    if (!Helpers::hasValue(wwTapActivated_, EMS_VALUE_BOOL)) {
        LOG_WARNING("Coldshot not possible in dhw-buffer systems");
        return false;
    }

    bool v;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }

    uint8_t message_data[EMS_MAX_TELEGRAM_MESSAGE_LENGTH];
    for (uint8_t i = 0; i < sizeof(message_data); i++) {
        message_data[i] = 0x00;
    }

    // we use the special test mode 0x1D for this. Setting the first data to 5A puts the system into test mode and
    // a setting of 0x00 puts it back into normal operating mode
    // when in test mode we're able to mess around with the 3-way valve settings
    if (!v) {
        // DHW off
        message_data[0] = 0x5A; // test mode on
        message_data[1] = 0x00; // burner output 0%
        message_data[3] = 0x64; // boiler pump capacity 100%
        message_data[4] = 0xFF; // 3-way valve hot water only
        wwTapActivated_ = 0;
    } else {
        // get out of test mode. Send all zeros.
        // telegram: 0B 08 1D 00 00
        wwTapActivated_ = 1;
    }

    // LOG_INFO("calling EMS_TYPE_UBAFunctionTest with %d", v);
    write_command(EMS_TYPE_UBAFunctionTest, 0, message_data, sizeof(message_data), 0);

    return true;
}

// Activate / De-activate One Time dhw 0x35
// true = on, false = off
// See also https://github.com/emsesp/EMS-ESP/issues/341#issuecomment-596245458 for Junkers
bool Boiler::set_ww_onetime(const char * value, const int8_t id) {
    bool v;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }

    if (is_received(EMS_TYPE_UBAParameterWWPlus)) {
        write_command(EMS_TYPE_UBAFlags, 0, (v ? 0x22 : 0x02), 0xE9); // not sure if this is in flags
    } else {
        write_command(EMS_TYPE_UBAFlags, 0, (v ? 0x23 : 0x03), 0x34);
    }

    return true;
}

// starting dhw disinfect, set to off seems not working
bool Boiler::set_ww_disinfect(const char * value, const int8_t id) {
    bool v;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }

    if (is_received(EMS_TYPE_UBAParameterWWPlus)) {
        write_command(EMS_TYPE_UBAFlags, 0, (v ? 0x44 : 0x04), 0xE9); // not sure if this is in flags
    } else {
        write_command(EMS_TYPE_UBAFlags, 0, (v ? 0x44 : 0x04), 0x34);
    }

    return true;
}

// Activate / De-activate circulation of dhw 0x35
// true = on, false = off
bool Boiler::set_ww_circulation(const char * value, const int8_t id) {
    bool v;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }

    if (is_received(EMS_TYPE_UBAParameterWWPlus)) {
        write_command(EMS_TYPE_UBAFlags, 1, (v ? 0x22 : 0x02), 0xE9); // not sure if this is in flags
    } else {
        write_command(EMS_TYPE_UBAFlags, 1, (v ? 0x22 : 0x02), 0x34);
    }

    return true;
}

// configuration of dhw circulation pump
bool Boiler::set_ww_circulation_pump(const char * value, const int8_t id) {
    bool v;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }

    if (is_received(EMS_TYPE_UBAParameterWWPlus)) {
        write_command(EMS_TYPE_UBAParameterWWPlus, 10, v ? 0x01 : 0x00, EMS_TYPE_UBAParameterWWPlus);
    } else {
        write_command(EMS_TYPE_UBAParameterWW, 6, v ? 0xFF : 0x00, EMS_TYPE_UBAParameterWW);
    }

#ifdef EMSESP_TEST
    // set wwCircPump_ , for test "api4" in test.cpp
    wwCircPump_ = v ? EMS_VALUE_BOOL_ON : EMS_VALUE_BOOL_OFF;
#endif

    return true;
}

// Set the mode of circulation, 1x3min, ... 6x3min, continuous
// if 0-off, switching is not possible, if 1-7 switching to off is not possible
bool Boiler::set_ww_circulation_mode(const char * value, const int8_t id) {
    uint8_t v;
    if (!Helpers::value2enum(value, v, FL_(enum_freq))) {
        return false;
    }

    if (is_received(EMS_TYPE_UBAParameterWWPlus)) {
        write_command(EMS_TYPE_UBAParameterWWPlus, 11, v, EMS_TYPE_UBAParameterWWPlus);
    } else {
        write_command(EMS_TYPE_UBAParameterWW, 7, v, EMS_TYPE_UBAParameterWW);
    }

    return true;
}

// Reset command
// 0 & 1        Reset-Mode (Manual, others)
// 8            reset maintenance message Hxx
// 12 & 13      Reset the Error-memory
bool Boiler::set_reset(const char * value, const int8_t id) {
    uint8_t num;
    if (!Helpers::value2enum(value, num, FL_(enum_reset))) {
        return false;
    }

    if (num == 0) {
        return true; // dash
    } else if (num == 1) {
        // LOG_INFO("Reset boiler maintenance message");
        write_command(0x05, 0x08, 0xFF, 0x1C);
        return true;
    } else if (num == 2) {
        // LOG_INFO("Reset boiler error message");
        write_command(0x05, 0x00, 0x5A); // error reset
        return true;
    } else if (num == 3) {
        // LOG_INFO("Reset boiler history");
        write_command(0x05, 42, 0x01); // clear history
        return true;
    } else if (num == 4) {
        // LOG_INFO("Reset boiler message");
        write_command(0x05, 0x08, 0xFF); // same as maintenance
        return true;
    }
    return false;
}

// maintenance
bool Boiler::set_maintenance(const char * value, const int8_t id) {
    if (value == nullptr) {
        return false;
    }

    std::string s;
    if (Helpers::value2string(value, s)) {
        if (s == std::string(Helpers::translated_word(FL_(reset)))) {
            // LOG_INFO("Reset boiler maintenance message");
            write_command(0x05, 0x08, 0xFF, 0x1C);
            return true;
        }
    }

    if (strlen(value) == 10) { // date
        uint8_t day   = (value[0] - '0') * 10 + (value[1] - '0');
        uint8_t month = (value[3] - '0') * 10 + (value[4] - '0');
        uint8_t year  = (uint8_t)(Helpers::atoint(&value[6]) - 2000);
        if (day > 0 && day < 32 && month > 0 && month < 13) {
            LOG_INFO("Setting maintenance date to %02d.%02d.%04d", day, month, year + 2000);
            uint8_t data[5] = {2, (uint8_t)(maintenanceTime_ / 100), day, month, year};
            write_command(0x15, 0, data, 5, 0x15);
        } else {
            LOG_WARNING("Setting maintenance: wrong format %d.%d.%d", day, month, year + 2000);
            return false;
        }
        return true;
    }

    int hrs;
    if (Helpers::value2number(value, hrs)) {
        if (hrs > 99 && hrs < 25600) {
            LOG_INFO("Setting maintenance time %d hours", hrs);
            uint8_t data[2] = {1, (uint8_t)(hrs / 100)};
            write_command(0x15, 0, data, 2, 0x15);
            return true;
        }
    }

    uint8_t num;
    if (Helpers::value2enum(value, num, FL_(enum_off_time_date_manual))) {
        LOG_INFO("Setting maintenance type to %s", value);
        write_command(0x15, 0, num, 0x15);
        return true;
    }

    LOG_WARNING("Setting maintenance: wrong format");
    return false;
}
//maintenance
bool Boiler::set_maintenancetime(const char * value, const int8_t id) {
    int hrs;
    if (Helpers::value2number(value, hrs)) {
        if (hrs > 99 && hrs < 25600) {
            LOG_INFO("Setting maintenance time %d hours", hrs);
            uint8_t data[2] = {1, (uint8_t)(hrs / 100)};
            write_command(0x15, 0, data, 2, 0x15);
            return true;
        }
    }
    LOG_WARNING("Setting maintenance: wrong format");
    return false;
}

//maintenance
bool Boiler::set_maintenancedate(const char * value, const int8_t id) {
    if ((value != nullptr) && strlen(value) == 10) { // date
        uint8_t day   = (value[0] - '0') * 10 + (value[1] - '0');
        uint8_t month = (value[3] - '0') * 10 + (value[4] - '0');
        uint8_t year  = (uint8_t)(Helpers::atoint(&value[6]) - 2000);
        if (day > 0 && day < 32 && month > 0 && month < 13) {
            LOG_INFO("Setting maintenance date to %02d.%02d.%04d", day, month, year + 2000);
            uint8_t data[5] = {2, (uint8_t)(maintenanceTime_ / 100), day, month, year};
            write_command(0x15, 0, data, 5, 0x15);
        } else {
            LOG_WARNING("Setting maintenance: wrong format %d.%d.%d", day, month, year + 2000);
            return false;
        }
        return true;
    }

    LOG_WARNING("Setting maintenance: wrong format");
    return false;
}

// Set the pool temperature 0x48A
bool Boiler::set_pool_temp(const char * value, const int8_t id) {
    float v;
    if (!Helpers::value2temperature(value, v)) {
        return false;
    }
    uint8_t v2 = ((v * 2) + 0.5);
    // LOG_INFO("Setting pool temperature to %d.%d C", v2 >> 1, (v2 & 0x01) * 5);
    write_command(0x48A, 1, v2, 0x48A);

    return true;
}

bool Boiler::set_emergency_temp(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2temperature(value, v)) {
        return false;
    }

    write_command(EMS_TYPE_UBAParametersPlus, 19, v, EMS_TYPE_UBAParametersPlus);

    return true;
}

bool Boiler::set_emergency_ops(const char * value, const int8_t id) {
    bool v;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }
    write_command(EMS_TYPE_UBAParametersPlus, 18, v ? 0x01 : 0x00, EMS_TYPE_UBAParametersPlus);
    return true;
}

bool Boiler::set_HpInLogic(const char * value, const int8_t id) {
    if (id == 0 || id > 4 || strlen(value) > 15) {
        return false;
    }
    bool v;
    if (strlen(value) == 1 && Helpers::value2bool(value, v)) {
        write_command(0x486, id == 4 ? 42 : id - 1, v ? 1 : 0, 0x486);
        return true;
    }
    char option[] = {"xxxxxxxxxxxxxxx"};
    strncpy(option, value, strlen(option)); // copy without termination
    // inputs 1,2,3 <inv>[<evu1><evu2><evu3><comp><aux><cool><heat><dhw><pv><prot><pres><mod>]
    if (id < 4) {
        uint8_t index[] = {0, 3, 6, 9, 12, 15, 18, 21, 24, 39, 36, 30};
        for (uint8_t i = 0; i < 12; i++) {
            if (option[i] == '0' || option[i] == '1') {
                write_command(0x486, index[i] + id - 1, option[i] - '0');
            }
        }
        if (option[12] >= '0' && option[12] <= '9') {
            write_command(0x486, 27, (uint8_t)atoi(&option[12]));
        }
        return true;
    }

    // input 4: <inv>[<comp><aux><cool><heat><dhw><pv><prot><pres><mod>]
    uint8_t index4[] = {42, 43, 44, 45, 46, 47, 52, 50, 49};
    for (uint8_t i = 0; i < 9; i++) {
        if (option[i] == '0' || option[i] == '1') {
            write_command(0x486, index4[i], option[i] - '0');
        }
    }
    if (option[9] >= '0' && option[9] <= '9') {
        write_command(0x486, 48, (uint8_t)atoi(&option[9]));
    }
    return true;
}

bool Boiler::set_maxHeat(const char * value, const int8_t id) {
    uint8_t v;
    if (!Helpers::value2enum(value, v, FL_(enum_maxHeat))) {
        return false;
    }
    write_command(0x492, id, v, 0x492);
    return true;
}

bool Boiler::set_silentMode(const char * value, const int8_t id) {
    uint8_t v;
    if (!Helpers::value2enum(value, v, FL_(enum_silentMode))) {
        return false;
    }
    write_command(0x484, 10, v, 0x484);
    return true;
}

bool Boiler::set_silentFrom(const char * value, const int8_t id) {
    if (value == nullptr || value[0] < '0' || value[0] > '9') {
        return false;
    }
    auto v = Helpers::string2minutes(value);
    write_command(0x484, 52, v / 15, 0x484);
    return true;
}

bool Boiler::set_silentTo(const char * value, const int8_t id) {
    if (value == nullptr || value[0] < '0' || value[0] > '9') {
        return false;
    }
    auto v = Helpers::string2minutes(value);
    write_command(0x484, 53, v / 15, 0x484);
    return true;
}

bool Boiler::set_minTempSilent(const char * value, const int8_t id) {
    int v;
    if (Helpers::value2temperature(value, v)) {
        write_command(0x484, 11, v, 0x484);
        return true;
    }
    return false;
}

bool Boiler::set_additionalHeaterOnly(const char * value, const int8_t id) {
    bool v;
    if (Helpers::value2bool(value, v)) {
        write_command(0x491, 1, v ? 1 : 0, 0x491);
        return true;
    }
    return false;
}

bool Boiler::set_additionalHeater(const char * value, const int8_t id) {
    bool v;
    if (Helpers::value2bool(value, v)) {
        write_command(0x491, 2, v ? 1 : 0, 0x491);
        return true;
    }
    return false;
}

bool Boiler::set_tempParMode(const char * value, const int8_t id) {
    int v;
    if (Helpers::value2temperature(value, v)) {
        write_command(0x491, 5, v, 0x491);
        return true;
    }
    return false;
}

bool Boiler::set_additionalHeaterDelay(const char * value, const int8_t id) {
    int v;
    if (Helpers::value2number(value, v)) {
        v /= 10;
        uint8_t data[2] = {(uint8_t)(v >> 8), (uint8_t)v};
        write_command(0x491, 16, data, 2, 0x491);
        return true;
    }
    return false;
}

bool Boiler::set_auxHeatMode(const char * value, const int8_t id) {
    uint8_t v;
    if (Helpers::value2enum(value, v, FL_(enum_modetype))) {
        write_command(0x491, 4, v, 0x491);
        return true;
    }
    return false;
}

bool Boiler::set_auxLimit(const char * value, const int8_t id) {
    float v;
    if (Helpers::value2float(value, v)) {
        write_command(0x491, id, (uint8_t)(v * 10), 0x491);
        return true;
    }
    return false;
}

bool Boiler::set_manDefrost(const char * value, const int8_t id) {
    bool v;
    if (Helpers::value2bool(value, v)) {
        write_command(0x491, 0, v ? 1 : 0, 0x491);
        return true;
    }
    return false;
}

bool Boiler::set_pvCooling(const char * value, const int8_t id) {
    bool v;
    if (Helpers::value2bool(value, v)) {
        write_command(0x485, 21, v ? 1 : 0, 0x485);
        return true;
    }
    return false;
}

bool Boiler::set_hpHyst(const char * value, const int8_t id) {
    int v;
    if (Helpers::value2number(value, v)) {
        v /= 5;
        uint8_t data[2] = {(uint8_t)(v >> 8), (uint8_t)v};
        write_command(0x484, id, data, 2, 0x484);
        return true;
    }
    return false;
}

bool Boiler::set_tempDiff(const char * value, const int8_t id) {
    float v;
    if (Helpers::value2float(value, v)) {
        write_command(0x48B, id, (uint8_t)(v * 10), 0x48B);
        return true;
    }
    return false;
}

// also used for stopTemp with different index
bool Boiler::set_wwOffTemp(const char * value, const int8_t id) {
    int v;
    if (Helpers::value2temperature(value, v)) {
        write_command(0x499, id, v, 0x499);
        return true;
    }
    return false;
}

bool Boiler::set_wwDiffTemp(const char * value, const int8_t id) {
    int v;
    if (Helpers::value2number(value, v)) {
        write_command(0x499, id, v, 0x499);
        return true;
    }
    return false;
}

bool Boiler::set_hpCircPumpWw(const char * value, const int8_t id) {
    bool v;
    if (Helpers::value2bool(value, v)) {
        write_command(0x484, 46, v ? 1 : 0, 0x484);
        return true;
    }
    return false;
}

bool Boiler::set_hpPumpMode(const char * value, const int8_t id) {
    uint8_t v;
    if (Helpers::value2enum(value, v, FL_(enum_hpPumpMode))) {
        write_command(0x48B, 18, v, 0x48B);
        return true;
    }
    return false;
}

bool Boiler::set_hpMaxPower(const char * value, const int8_t id) {
    int v;
    if (Helpers::value2number(value, v)) {
        write_command(0x484, 31, v, 0x484);
        return true;
    }
    return false;
}

bool Boiler::set_pvMaxComp(const char * value, const int8_t id) {
    float v;
    if (Helpers::value2float(value, v)) {
        write_command(0x484, 54, (uint8_t)(v * 10), 0x484);
        return true;
    }
    return false;
}

bool Boiler::set_hpDiffPress(const char * value, const int8_t id) {
    int v;
    if (Helpers::value2number(value, v)) {
        write_command(0x2CC, 9, (uint8_t)(v / 50), 0x2CC);
        return true;
    }
    return false;
}

bool Boiler::set_hpPowerLimit(const char * value, const int8_t id) {
    int v;
    if (Helpers::value2number(value, v)) {
        uint8_t data[2] = {(uint8_t)(v >> 8), (uint8_t)v};
        write_command(0x4A7, 0, data, 2, 0x4A7);
        return true;
    }
    return false;
}

bool Boiler::set_powerReduction(const char * value, const int8_t id) {
    int v;
    if (Helpers::value2number(value, v)) {
        write_command(0x484, 64, v / 10, 0x484);
        return true;
    }
    return false;
}

bool Boiler::set_vp_cooling(const char * value, const int8_t id) {
    bool v;
    if (Helpers::value2bool(value, v)) {
        write_command(0x49C, 3, v ? 1 : 0, 0x49C);
        return true;
    }
    return false;
}

bool Boiler::set_heatCable(const char * value, const int8_t id) {
    bool v;
    if (Helpers::value2bool(value, v)) {
        write_command(0x49D, 2, v ? 1 : 0, 0x49D);
        return true;
    }
    return false;
}

bool Boiler::set_VC0valve(const char * value, const int8_t id) {
    bool v;
    if (Helpers::value2bool(value, v)) {
        write_command(0x49D, 3, v ? 1 : 0, 0x49D);
        return true;
    }
    return false;
}

bool Boiler::set_primePump(const char * value, const int8_t id) {
    bool v;
    if (Helpers::value2bool(value, v)) {
        write_command(0x49D, 4, v ? 1 : 0, 0x49D);
        return true;
    }
    return false;
}

bool Boiler::set_primePumpMod(const char * value, const int8_t id) {
    int v;
    if (Helpers::value2number(value, v)) {
        write_command(0x49D, 5, v, 0x49D);
        return true;
    }
    return false;
}

bool Boiler::set_hp3wayValve(const char * value, const int8_t id) {
    bool v;
    if (Helpers::value2bool(value, v)) {
        write_command(0x49D, 6, v ? 1 : 0, 0x49D);
        return true;
    }
    return false;
}

bool Boiler::set_elHeatStep(const char * value, const int8_t id) {
    bool v;
    if (Helpers::value2bool(value, v)) {
        write_command(0x49D, 6 + id, v ? 1 : 0, 0x49D);
        return true;
    }
    return false;
}

// dhw alternating operation, turn on/off; heatpumps only?
bool Boiler::set_wwAlternatingOper(const char * value, const int8_t id) {
    bool v;
    if (!Helpers::value2bool(value, v)) {
        return false;
    }
    write_command(EMS_TYPE_UBAParameterWWPlus, 14, v ? 1 : 0, EMS_TYPE_UBAParameterWWPlus);
    return true;
}

// dhw alternating operation, set prioitise heating (id=2) or dhw (id=3) time; heatpumps only?
bool Boiler::set_wwAltOpPrio(const char * value, const int8_t id) {
    int v;
    if (Helpers::value2number(value, v)) {
        write_command(0x484, id, v, 0x484);
        return true;
    }
    return false;
}

bool Boiler::set_forceHeatingOff(const char * value, const int8_t id) {
    bool v;
    if (Helpers::value2bool(value, v)) {
        has_update(forceHeatingOff_, v);
        if (!v && Helpers::hasValue(heatingTemp_)) {
            uint8_t data[] = {heatingTemp_,
                              (Helpers::hasValue(burnMaxPower_) ? burnMaxPower_ : (uint8_t)100),
                              (Helpers::hasValue(pumpModMax_) ? pumpModMax_ : (uint8_t)0),
                              0};
            write_command(EMS_TYPE_UBASetPoints, 0, data, sizeof(data), 0);
        }
        return true;
    }
    return false;
}

// energy counters. Setting an invalid value does not update, but trigger a store.
bool Boiler::set_nrgHeat(const char * value, const int8_t id) {
    float v;
    if (!Helpers::value2float(value, v)) {
        return false;
    }
    if (v >= 0) {
        nrgHeatF_ = v * 100;
        has_update(nrgHeat_, (uint32_t)(nrgHeatF_ + 0.5));
    }
    store_energy();
    return true;
}

bool Boiler::set_nrgWw(const char * value, const int8_t id) {
    float v;
    if (!Helpers::value2float(value, v)) {
        return false;
    }
    if (v >= 0) {
        nrgWwF_ = v * 100;
        has_update(nrgWw_, (uint32_t)(nrgWwF_ + 0.5));
    }
    store_energy();
    return true;
}

bool Boiler::set_nomPower(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2number(value, v)) {
        return false;
    }
    if (v > 0 && v < EMS_VALUE_UINT8_NOTSET) {
        has_update(nomPower_, (uint8_t)v);
    }
    store_energy();
    return true;
}

bool Boiler::set_curveOn(const char * value, const int8_t id) {
    bool v;
    if (Helpers::value2bool(value, v)) {
        write_command(0x28, 0, v ? 0xFF : 0);
        return true;
    }
    return false;
}

bool Boiler::set_curveBase(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2temperature(value, v)) {
        return false;
    }
    write_command(0x28, 2, v);
    return true;
}

bool Boiler::set_curveEnd(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2temperature(value, v)) {
        return false;
    }
    write_command(0x28, 1, v);
    return true;
}

bool Boiler::set_summerTemp(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2temperature(value, v)) {
        return false;
    }
    write_command(0x28, 3, v);
    return true;
}

bool Boiler::set_nofrost(const char * value, const int8_t id) {
    bool v;
    if (Helpers::value2bool(value, v)) {
        write_command(0x28, 4, v ? 0xFF : 0);
        return true;
    }
    return false;
}

bool Boiler::set_nofrostTemp(const char * value, const int8_t id) {
    int v;
    if (!Helpers::value2temperature(value, v)) {
        return false;
    }
    write_command(0x28, 5, v);
    return true;
}

bool Boiler::set_fan(const char * value, const int8_t id) {
    int v;
    if (Helpers::value2number(value, v)) {
        write_command(0x4A5, 9, (uint8_t)v, 0x4A5);
        return true;
    }
    return false;
}

bool Boiler::set_shutdown(const char * value, const int8_t id) {
    bool b;
    if (Helpers::value2bool(value, b)) {
        write_command(0x484, 58, b ? 1 : 0, 0x484);
        return true;
    }
    return false;
}

} // namespace emsesp
