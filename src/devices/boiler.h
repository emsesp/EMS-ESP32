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

#ifndef EMSESP_BOILER_H
#define EMSESP_BOILER_H

#include "emsesp.h"

namespace emsesp {

class Boiler : public EMSdevice {
  public:
    Boiler(uint8_t device_type, int8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand);

  private:
    static uuid::log::Logger logger_;

    // specific boiler characteristics, stripping the top 4 bits
    inline uint8_t model() const {
        return (flags() & 0x0F);
    }

    void check_active();
    void store_energy();

    uint8_t boilerState_ = EMS_VALUE_UINT8_NOTSET; // Boiler state flag - FOR INTERNAL USE

    static constexpr uint8_t  EMS_TYPE_UBASettingsWW      = 0x26;
    static constexpr uint8_t  EMS_TYPE_UBAParameterWW     = 0x33;
    static constexpr uint8_t  EMS_TYPE_UBAFunctionTest    = 0x1D;
    static constexpr uint8_t  EMS_TYPE_UBAFlags           = 0x35;
    static constexpr uint8_t  EMS_TYPE_UBASetPoints       = 0x1A;
    static constexpr uint8_t  EMS_TYPE_UBAParameters      = 0x16;
    static constexpr uint8_t  EMS_TYPE_UBAParametersPlus  = 0xE6;
    static constexpr uint8_t  EMS_TYPE_UBAParameterWWPlus = 0xEA;
    static constexpr uint16_t EMS_TYPE_UBAInformation     = 0x495;
    static constexpr uint16_t EMS_TYPE_UBAEnergySupplied  = 0x494;

    // ww
    uint8_t  wwSetTemp_;            // DHW set temperature
    uint8_t  wwSelTemp_;            // DHW selected temperature (comfort)
    uint8_t  wwSelTempLow_;         // DHW lower selected temperature (eco)
    uint8_t  wwSelTempOff_;         // DHW selected temperature for off position
    uint8_t  wwSelTempSingle_;      // DHW single charge temperature
    uint8_t  wwSelTempEcoplus_;     // DHW ECO+ temperature
    uint8_t  wwType_;               // 0-off, 1-flow, 2-flowbuffer, 3-buffer, 4-layered buffer
    uint8_t  wwComfort_;            // WW comfort mode
    uint8_t  wwComfort1_;           // WW comfort mode RC310, 0xEA offset 13
    uint8_t  wwComfort2_;           // WW comfort mode emsplus 0xEA, offset 26
    uint8_t  wwCircPump_;           // DHW circulation pump available
    uint8_t  wwChargeType_;         // DHW charge type (pump or 3-way-valve)
    uint8_t  wwChargeOptimization_; // DHW charge optimization
    uint8_t  wwDisinfectionTemp_;   // DHW disinfection temperature to prevent infection
    uint8_t  wwCircMode_;           // DHW circulation pump mode
    uint8_t  wwCirc_;               // DHW circulation on/off
    uint16_t wwCurTemp_;            // DHW current temperature
    uint16_t wwCurTemp2_;           // DHW current temperature storage
    uint8_t  wwCurFlow_;            // DHW current flow in l/min
    uint16_t wwStorageTemp1_;       // DHW storage temp 1
    uint16_t wwStorageTemp2_;       // DHW storage temp 2
    uint8_t  wwActivated_;          // DHW activated
    uint8_t  wwOneTime_;            // DHW one time function on/off
    uint8_t  wwDisinfect_;          // DHW disinfection on/off
    uint8_t  wwCharging_;           // DHW charging on/off
    uint8_t  wwRecharging_;         // DHW recharge on/off
    uint8_t  wwTempOK_;             // DHW temperature ok on/off
    uint8_t  wwActive_;             //
    uint8_t  ww3wayValve_;          // 3-way valve on WW
    uint8_t  wwFlowTempOffset_;     // Boiler offset for ww heating
    uint8_t  wwMaxPower_;           // DHW maximum power
    uint8_t  wwMaxTemp_;            // DHW maximum temperature
    uint32_t wwStarts_;             // DHW starts
    uint32_t wwStartsHp_;           // DHW starts Heatpump
    uint32_t wwWorkM_;              // DHW minutes
    int8_t   wwHystOn_;
    int8_t   wwHystOff_;
    uint16_t wwMixerTemp_;     // mixing temperature
    uint16_t wwCylMiddleTemp_; // Cyl middle temperature (TS3)
    uint16_t wwSolarTemp_;
    uint8_t  wwAlternatingOper_; // alternating operation on/off
    uint8_t  wwAltOpPrioHeat_;   // alternating operation, prioritize heat time
    uint8_t  wwAltOpPrioWw_;     // alternating operation, prioritize dhw time

    // special function
    uint8_t forceHeatingOff_;
    uint8_t wwTapActivated_; // maintenance-mode to switch DHW off

    // main
    uint8_t  reset_;          // for reset command
    uint8_t  heatingActive_;  // Central heating is on/off
    uint8_t  tapwaterActive_; // Hot tap water is on/off
    uint8_t  selFlowTemp_;    // Selected flow temperature
    uint8_t  selBurnPow_;     // Burner max power % (can be > 100%)
    uint8_t  absBurnPow_;     // absolute burner power in % of rating plate
    uint8_t  heatingPumpMod_; // Pump modulation %
    int16_t  outdoorTemp_;    // Outside temperature
    uint16_t curFlowTemp_;    // Current flow temperature
    uint16_t retTemp_;        // Return temperature
    uint16_t switchTemp_;     // Switch temperature
    uint8_t  sysPress_;       // System pressure
    uint16_t boilTemp_;       // Boiler temperature
    uint16_t exhaustTemp_;    // Exhaust temperature published
    // read second value from E4 and initialize it
    uint16_t exhaustTemp1_ = EMS_VALUE_UINT16_NOTSET;
    uint8_t  burnGas_;          // Gas on/off
    uint8_t  burnGas2_;         // Gas stage 2 on/off
    uint16_t flameCurr_;        // Flame current in micro amps
    uint8_t  heatingPump_;      // Boiler heating pump on/off
    uint8_t  fanWork_;          // Fan on/off
    uint8_t  ignWork_;          // Ignition on/off
    uint8_t  oilPreHeat_;       // oil preheating on
    uint8_t  heatingActivated_; // Heating activated on the boiler
    uint8_t  heatingTemp_;      // Heating temperature setting on the boiler
    uint8_t  pumpModMax_;       // Boiler circuit pump modulation max. power %
    uint8_t  pumpModMin_;       // Boiler circuit pump modulation min. power
    uint8_t  pumpMode_;         // pump setting proportional/deltaP
    uint8_t  pumpCharacter_;    // pump setting proportional/deltaP
    uint8_t  pumpDelay_;
    uint8_t  pumpOnTemp_;
    uint8_t  burnMinPeriod_;
    uint8_t  burnMinPower_;
    uint8_t  burnMaxPower_;
    int8_t   boilHystOn_;
    int8_t   boilHystOff_;
    int8_t   boil2HystOn_;
    int8_t   boil2HystOff_;
    uint8_t  curBurnPow_;        // Burner current power %
    uint32_t burnStarts_;        // burner restarts
    uint32_t heatStarts_;        // burner starts for heating
    uint32_t burnWorkMin_;       // Total burner operating time
    uint32_t burn2WorkMin_;      // burner stage 2 operating time
    uint32_t heatWorkMin_;       // Total heat operating time
    uint32_t UBAuptime_;         // Total UBA working hours
    char     lastCode_[55];      // last error code
    char     serviceCode_[4];    // 3 character status/service code
    uint16_t serviceCodeNumber_; // error/service code
    uint8_t  emergencyOps_;
    uint8_t  emergencyTemp_;
    uint16_t headertemp_; // see #1317
    uint16_t heatblock_;  // see #1317

    // weather compensation, see #1642
    uint8_t curveOn_;
    uint8_t curveBase_;
    uint8_t curveEnd_;
    uint8_t summerTemp_;
    uint8_t nofrost_;
    uint8_t nofrostTemp_;

    // info
    uint32_t upTimeTotal_;               // Operating time
    uint32_t upTimeControl_;             // Operating time control
    uint32_t upTimeCompHeating_;         // Operating time compressor heating
    uint32_t upTimeCompCooling_;         // Operating time compressor cooling
    uint32_t upTimeCompWw_;              // Operating time compressor DHW
    uint32_t upTimeCompPool_;            // Operating time compressor pool
    uint32_t totalCompStarts_;           // Total Commpressor control starts
    uint32_t heatingStarts_;             // Heating control starts
    uint32_t coolingStarts_;             // Cooling control starts
    uint32_t poolStarts_;                // DHW control starts
    uint32_t nrgConsTotal_;              // Energy consumption total
    uint32_t nrgConsCompTotal_;          // Energy consumption compressor total
    uint32_t nrgConsCompHeating_;        // Energy consumption compressor heating
    uint32_t nrgConsCompWw_;             // Energy consumption compressor DHW
    uint32_t nrgConsCompCooling_;        // Energy consumption compressor cooling
    uint32_t nrgConsCompPool_;           // Energy consumption compressor pool
    uint32_t nrgSuppTotal_;              // Energy supplied total
    uint32_t nrgSuppHeating_;            // Energy supplied heating
    uint32_t nrgSuppWw_;                 // Energy supplied DHW
    uint32_t nrgSuppCooling_;            // Energy supplied cooling
    uint32_t nrgSuppPool_;               // Energy supplied pool
    uint32_t auxElecHeatNrgConsTotal_;   // Auxiliary electrical heater energy consumption total
    uint32_t auxElecHeatNrgConsHeating_; // Auxiliary electrical heater energy consumption heating
    uint32_t auxElecHeatNrgConsWw_;      // Auxiliary electrical heater energy consumption DHW
    uint32_t auxElecHeatNrgConsPool_;    // Auxiliary electrical heater energy consumption Pool
    char     maintenanceMessage_[4];
    char     maintenanceDate_[12];
    uint8_t  maintenanceType_;
    uint16_t maintenanceTime_;

    // heatpump
    uint16_t hpPower_;
    uint8_t  hpCompOn_;
    uint8_t  hpBrinePumpSpd_;
    uint8_t  hpCompSpd_;
    uint8_t  hpCircSpd_;
    uint16_t hpBrineIn_;
    uint16_t hpBrineOut_;
    uint8_t  hpSwitchValve_;
    uint8_t  hpActivity_;
    // uint8_t  hpHeatingOn_;
    // uint8_t  hpCoolingOn_;
    // uint8_t  hpWwOn_;
    // uint8_t  hpPoolOn_;
    int16_t  hpTc0_;
    int16_t  hpTc1_;
    int16_t  hpTc3_;
    int16_t  hpTr1_;
    int16_t  hpTr3_;
    int16_t  hpTr4_;
    int16_t  hpTr5_;
    int16_t  hpTr6_;
    int16_t  hpTr7_;
    int16_t  hpTl2_;
    int16_t  hpPl1_;
    int16_t  hpPh1_;
    int16_t  hpTa4_;
    int16_t  hpTw1_;
    uint32_t nrgTotal_;
    uint32_t nrgWw_;
    uint32_t nrgHeat_;
    uint32_t nrgWw2_;
    uint32_t nrgHeat2_;
    uint32_t nrgCool_;
    uint32_t meterTotal_;
    uint32_t meterComp_;
    uint32_t meterEHeat_;
    uint32_t meterHeat_;
    uint32_t meterWw_;
    uint32_t meterCool_;
    uint32_t gasMeterHeat_;
    uint32_t gasMeterWw_;
    uint8_t  hpEA0_;
    uint8_t  hpPumpMode_;
    uint8_t  hpSetDiffPress_;
    uint8_t  fan_;
    uint8_t  hpshutdown_;

    // Pool unit
    int8_t poolSetTemp_;

    // Inputs
    struct {
        uint8_t state;
        char option[16]; // logic, block_comp, block_dhw, block_heat, block_cool, overheat_protect, evu_blocktime1,2,3, block_heater, Solar, brine lowpressure, brine pump modulation
    } hpInput[4];

    // Heater limits
    uint8_t maxHeatComp_;
    uint8_t maxHeatHeat_;
    uint8_t maxHeatDhw_;
    uint8_t hpMaxPower_;
    uint8_t pvMaxComp_;
    uint8_t powerReduction_;

    uint8_t  pvCooling_;
    uint8_t  manDefrost_;
    uint8_t  auxHeatMode_;
    uint8_t  auxMaxLimit_;
    uint8_t  auxLimitStart_;
    uint8_t  auxHeaterOnly_;
    uint8_t  auxHeaterOff_;
    uint8_t  auxHeaterStatus_;
    uint8_t  auxHeaterLevel_;
    uint16_t auxHeaterDelay_;
    uint8_t  silentMode_;
    int8_t   minTempSilent_;
    uint8_t  silentFrom_;
    uint8_t  silentTo_;
    int8_t   tempParMode_;
    int8_t   auxHeatMixValve_;
    uint16_t hpHystHeat_;
    uint16_t hpHystCool_;
    uint16_t hpHystPool_;
    uint8_t  tempDiffHeat_;
    uint8_t  tempDiffCool_;
    uint8_t  hpCircPumpWw_;

    uint8_t wwComfOffTemp_;
    uint8_t wwEcoOffTemp_;
    uint8_t wwEcoPlusOffTemp_;
    uint8_t wwComfDiffTemp_;
    uint8_t wwEcoDiffTemp_;
    uint8_t wwEcoPlusDiffTemp_;
    uint8_t wwComfStopTemp_;
    uint8_t wwEcoStopTemp_;
    uint8_t wwEcoPlusStopTemp_;

    uint8_t  vp_cooling_;
    uint8_t  heatCable_;
    uint8_t  VC0valve_;
    uint8_t  primePump_;
    uint8_t  primePumpMod_;
    uint8_t  hp3wayValve_;
    uint8_t  hp4wayValve_;
    uint8_t  elHeatStep1_;
    uint8_t  elHeatStep2_;
    uint8_t  elHeatStep3_;
    uint16_t hpPowerLimit_;
    uint16_t hpCurrPower_;
    int16_t  pc0Flow_;
    int16_t  pc1Flow_;
    uint8_t  pc1Rate_;
    uint8_t  pc1On_;

    // HIU
    // uint16_t cwFlowRate_;  // cold water flow rate *10
    uint16_t netFlowTemp_; // heat network flow temperature *10
    uint8_t  keepWarmTemp_;
    uint8_t  setReturnTemp_;
    uint8_t  heatValve_;
    uint8_t  wwValve_;

    // special
    double  nrgHeatF_; // double calculate for nrgHeat
    double  nrgWwF_;   // double calculate for nrgWw
    uint8_t nomPower_;

    /*
  // Hybrid heatpump with telegram 0xBB is readable and writeable in boiler and thermostat
  // thermostat always overwrites settings in boiler
  //enable settings here if no thermostat is used in system
  // HybridHP
    uint8_t hybridStrategy_;  // cost = 2, temperature = 3, mix = 4
    int8_t  switchOverTemp_;  // degrees
    uint8_t energyCostRatio_; // is *10
    uint8_t fossileFactor_;   // is * 10
    uint8_t electricFactor_;  // is * 10
    uint8_t delayBoiler_;     // minutes
    uint8_t tempDiffBoiler_;  // relative temperature degrees
  */
    void process_UBAFactory(std::shared_ptr<const Telegram> telegram);
    void process_UBAParameterWW(std::shared_ptr<const Telegram> telegram);
    void process_UBAMonitorFast(std::shared_ptr<const Telegram> telegram);
    void process_UBATotalUptime(std::shared_ptr<const Telegram> telegram);
    void process_UBAParameters(std::shared_ptr<const Telegram> telegram);
    void process_UBAMonitorWW(std::shared_ptr<const Telegram> telegram);
    void process_UBAMonitorFastPlus(std::shared_ptr<const Telegram> telegram);
    void process_UBAMonitorSlow(std::shared_ptr<const Telegram> telegram);
    void process_UBAMonitorSlowPlus(std::shared_ptr<const Telegram> telegram);
    void process_UBAMonitorSlowPlus2(std::shared_ptr<const Telegram> telegram);
    void process_UBAParametersPlus(std::shared_ptr<const Telegram> telegram);
    void process_UBAParameterWWPlus(std::shared_ptr<const Telegram> telegram);
    void process_UBAOutdoorTemp(std::shared_ptr<const Telegram> telegram);
    void process_UBASetPoints(std::shared_ptr<const Telegram> telegram);
    void process_UBAFlags(std::shared_ptr<const Telegram> telegram);
    void process_MC110Status(std::shared_ptr<const Telegram> telegram);
    void process_UBAMaintenanceStatus(std::shared_ptr<const Telegram> telegram);
    void process_UBAMaintenanceData(std::shared_ptr<const Telegram> telegram);
    void process_ErrorMessage(std::shared_ptr<const Telegram> telegram);
    void process_UBAErrorMessage(std::shared_ptr<const Telegram> telegram);
    void process_UBAErrorMessage2(std::shared_ptr<const Telegram> telegram);
    void process_UBAMonitorWWPlus(std::shared_ptr<const Telegram> telegram);
    void process_UBAInformation(std::shared_ptr<const Telegram> telegram);
    void process_UBAEnergySupplied(std::shared_ptr<const Telegram> telegram);
    void process_CascadeMessage(std::shared_ptr<const Telegram> telegram);
    void process_UBASettingsWW(std::shared_ptr<const Telegram> telegram);
    void process_HpPower(std::shared_ptr<const Telegram> telegram);
    void process_HpTemperatures(std::shared_ptr<const Telegram> telegram);
    void process_HpPool(std::shared_ptr<const Telegram> telegram);
    void process_HpInput(std::shared_ptr<const Telegram> telegram);
    void process_HpInConfig(std::shared_ptr<const Telegram> telegram);
    void process_HpPressure(std::shared_ptr<const Telegram> telegram);
    void process_HpCooling(std::shared_ptr<const Telegram> telegram);
    void process_HpHeaterConfig(std::shared_ptr<const Telegram> telegram);
    void process_HybridHp(std::shared_ptr<const Telegram> telegram);
    void process_HpSilentMode(std::shared_ptr<const Telegram> telegram);
    void process_HpAdditionalHeater(std::shared_ptr<const Telegram> telegram);
    void process_HpValve(std::shared_ptr<const Telegram> telegram);
    void process_HpPumps(std::shared_ptr<const Telegram> telegram);
    void process_HpPump2(std::shared_ptr<const Telegram> telegram);
    void process_HpDhwSettings(std::shared_ptr<const Telegram> telegram);
    void process_HpSettings2(std::shared_ptr<const Telegram> telegram);
    void process_HpSettings3(std::shared_ptr<const Telegram> telegram);
    void process_HpEnergy(std::shared_ptr<const Telegram> telegram);
    void process_HpMeters(std::shared_ptr<const Telegram> telegram);
    void process_WeatherComp(std::shared_ptr<const Telegram> telegram);
    void process_HpFan(std::shared_ptr<const Telegram> telegram);
    void process_HpPower2(std::shared_ptr<const Telegram> telegram);
    void process_HpPowerLimit(std::shared_ptr<const Telegram> telegram);

    void process_Meters(std::shared_ptr<const Telegram> telegram);
    void process_Energy(std::shared_ptr<const Telegram> telegram);

    // HIU
    void process_HIUSettings(std::shared_ptr<const Telegram> telegram);
    void process_HIUMonitor(std::shared_ptr<const Telegram> telegram);

    bool set_keepWarmTemp(const char * value, const int8_t id);
    bool set_returnTemp(const char * value, const int8_t id);

    // commands - none of these use the additional id parameter
    bool        set_ww_mode(const char * value, const int8_t id);
    bool        set_ww_activated(const char * value, const int8_t id);
    bool        set_tapwarmwater_activated(const char * value, const int8_t id);
    bool        set_ww_onetime(const char * value, const int8_t id);
    bool        set_ww_disinfect(const char * value, const int8_t id);
    bool        set_ww_circulation(const char * value, const int8_t id);
    bool        set_ww_circulation_pump(const char * value, const int8_t id);
    bool        set_ww_circulation_mode(const char * value, const int8_t id);
    bool        set_ww_temp(const char * value, const int8_t id);
    bool        set_ww_temp_low(const char * value, const int8_t id);
    bool        set_ww_temp_eco(const char * value, const int8_t id);
    bool        set_ww_temp_single(const char * value, const int8_t id);
    bool        set_ww_disinfect_temp(const char * value, const int8_t id);
    bool        set_ww_maxpower(const char * value, const int8_t id);
    bool        set_ww_maxtemp(const char * value, const int8_t id);
    bool        set_ww_flowTempOffset(const char * value, const int8_t id);
    bool        set_ww_chargeOptimization(const char * value, const int8_t id);
    bool        set_flow_temp(const char * value, const int8_t id);
    bool        set_burn_power(const char * value, const int8_t id);
    bool        set_heating_activated(const char * value, const int8_t id);
    bool        set_heating_temp(const char * value, const int8_t id);
    bool        set_min_power(const char * value, const int8_t id);
    bool        set_max_power(const char * value, const int8_t id);
    bool        set_min_pump(const char * value, const int8_t id);
    bool        set_max_pump(const char * value, const int8_t id);
    bool        set_pumpMode(const char * value, const int8_t id);
    bool        set_pumpCharacter(const char * value, const int8_t id);
    bool        set_pumpOnTemp(const char * value, const int8_t id);
    bool        set_hyst_on(const char * value, const int8_t id);
    bool        set_hyst_off(const char * value, const int8_t id);
    inline bool set_hyst2_on(const char * value, const int8_t id) {
        return set_hyst_on(value, 2);
    }
    inline bool set_hyst2_off(const char * value, const int8_t id) {
        return set_hyst_off(value, 2);
    }
    bool set_burn_period(const char * value, const int8_t id);
    bool set_pump_delay(const char * value, const int8_t id);
    bool set_reset(const char * value, const int8_t id);
    bool set_maintenance(const char * value, const int8_t id);
    bool set_maintenancetime(const char * value, const int8_t id);
    bool set_maintenancedate(const char * value, const int8_t id);
    bool set_ww_hyst_on(const char * value, const int8_t id);
    bool set_ww_hyst_off(const char * value, const int8_t id);
    bool set_pool_temp(const char * value, const int8_t id);
    bool set_emergency_temp(const char * value, const int8_t id);
    bool set_emergency_ops(const char * value, const int8_t id);

    bool        set_HpInLogic(const char * value, const int8_t id);
    inline bool set_HpIn1Logic(const char * value, const int8_t id) {
        return set_HpInLogic(value, 1);
    }
    inline bool set_HpIn2Logic(const char * value, const int8_t id) {
        return set_HpInLogic(value, 2);
    }
    inline bool set_HpIn3Logic(const char * value, const int8_t id) {
        return set_HpInLogic(value, 3);
    }
    inline bool set_HpIn4Logic(const char * value, const int8_t id) {
        return set_HpInLogic(value, 4);
    }
    bool        set_maxHeat(const char * value, const int8_t id);
    inline bool set_maxHeatComp(const char * value, const int8_t id) {
        return set_maxHeat(value, 2);
    }
    inline bool set_maxHeatHeat(const char * value, const int8_t id) {
        return set_maxHeat(value, 3);
    }
    inline bool set_maxHeatDhw(const char * value, const int8_t id) {
        return set_maxHeat(value, 4);
    }
    bool set_silentMode(const char * value, const int8_t id);
    bool set_minTempSilent(const char * value, const int8_t id);
    bool set_silentFrom(const char * value, const int8_t id);
    bool set_silentTo(const char * value, const int8_t id);
    bool set_additionalHeaterOnly(const char * value, const int8_t id);
    bool set_additionalHeater(const char * value, const int8_t id);
    bool set_additionalHeaterDelay(const char * value, const int8_t id);
    bool set_tempParMode(const char * value, const int8_t id);
    bool set_auxHeatMode(const char * value, const int8_t id);
    bool set_manDefrost(const char * value, const int8_t id);
    bool set_pvCooling(const char * value, const int8_t id);
    bool set_hpCircPumpWw(const char * value, const int8_t id);
    bool set_hpPumpMode(const char * value, const int8_t id);
    bool set_hpMaxPower(const char * value, const int8_t id);
    bool set_pvMaxComp(const char * value, const int8_t id);
    bool set_hpDiffPress(const char * value, const int8_t id);
    bool set_hpPowerLimit(const char * value, const int8_t id);
    bool set_powerReduction(const char * value, const int8_t id);

    bool        set_auxLimit(const char * value, const int8_t id);
    inline bool set_auxMaxLimit(const char * value, const int8_t id) {
        return set_auxLimit(value, 14);
    }
    inline bool set_auxLimitStart(const char * value, const int8_t id) {
        return set_auxLimit(value, 15);
    }
    bool        set_hpHyst(const char * value, const int8_t id);
    inline bool set_hpHystHeat(const char * value, const int8_t id) {
        return set_hpHyst(value, 37);
    }
    inline bool set_hpHystCool(const char * value, const int8_t id) {
        return set_hpHyst(value, 35);
    }
    inline bool set_hpHystPool(const char * value, const int8_t id) {
        return set_hpHyst(value, 33);
    }
    bool        set_tempDiff(const char * value, const int8_t id);
    inline bool set_tempDiffHeat(const char * value, const int8_t id) {
        return set_tempDiff(value, 4);
    }
    inline bool set_tempDiffCool(const char * value, const int8_t id) {
        return set_tempDiff(value, 3);
    }
    bool        set_wwOffTemp(const char * value, const int8_t id);
    inline bool set_wwComfOffTemp(const char * value, const int8_t id) {
        return set_wwOffTemp(value, 1);
    }
    inline bool set_wwEcoOffTemp(const char * value, const int8_t id) {
        return set_wwOffTemp(value, 0);
    }
    inline bool set_wwEcoPlusOffTemp(const char * value, const int8_t id) {
        return set_wwOffTemp(value, 5);
    }
    bool        set_wwDiffTemp(const char * value, const int8_t id);
    inline bool set_wwComfDiffTemp(const char * value, const int8_t id) {
        return set_wwDiffTemp(value, 12);
    }
    inline bool set_wwEcoDiffTemp(const char * value, const int8_t id) {
        return set_wwDiffTemp(value, 13);
    }
    inline bool set_wwEcoPlusDiffTemp(const char * value, const int8_t id) {
        return set_wwDiffTemp(value, 14);
    }
    inline bool set_wwComfStopTemp(const char * value, const int8_t id) {
        return set_wwOffTemp(value, 8);
    }
    inline bool set_wwEcoStopTemp(const char * value, const int8_t id) {
        return set_wwOffTemp(value, 9);
    }
    inline bool set_wwEcoPlusStopTemp(const char * value, const int8_t id) {
        return set_wwOffTemp(value, 10);
    }
    bool        set_vp_cooling(const char * value, const int8_t id);
    bool        set_heatCable(const char * value, const int8_t id);
    bool        set_VC0valve(const char * value, const int8_t id);
    bool        set_primePump(const char * value, const int8_t id);
    bool        set_primePumpMod(const char * value, const int8_t id);
    bool        set_hp3wayValve(const char * value, const int8_t id);
    bool        set_elHeatStep(const char * value, const int8_t id);
    inline bool set_elHeatStep1(const char * value, const int8_t id) {
        return set_elHeatStep(value, 1);
    }
    inline bool set_elHeatStep2(const char * value, const int8_t id) {
        return set_elHeatStep(value, 2);
    }
    inline bool set_elHeatStep3(const char * value, const int8_t id) {
        return set_elHeatStep(value, 3);
    }
    bool        set_wwAlternatingOper(const char * value, const int8_t id);
    bool        set_wwAltOpPrio(const char * value, const int8_t id);
    inline bool set_wwAltOpPrioHeat(const char * value, const int8_t id) {
        return set_wwAltOpPrio(value, 2);
    }
    inline bool set_wwAltOpPrioWw(const char * value, const int8_t id) {
        return set_wwAltOpPrio(value, 3);
    }
    bool set_forceHeatingOff(const char * value, const int8_t id);

    /*
    bool set_hybridStrategy(const char * value, const int8_t id);
    bool set_switchOverTemp(const char * value, const int8_t id);
    bool set_energyCostRatio(const char * value, const int8_t id);
    bool set_fossileFactor(const char * value, const int8_t id);
    bool set_electricFactor(const char * value, const int8_t id);
    bool set_delayBoiler(const char * value, const int8_t id);
    bool set_tempDiffBoiler(const char * value, const int8_t id);
    */
    bool set_curveOn(const char * value, const int8_t id);
    bool set_curveBase(const char * value, const int8_t id);
    bool set_curveEnd(const char * value, const int8_t id);
    bool set_summerTemp(const char * value, const int8_t id);
    bool set_nofrost(const char * value, const int8_t id);
    bool set_nofrostTemp(const char * value, const int8_t id);
    bool set_fan(const char * value, const int8_t id);
    bool set_shutdown(const char * value, const int8_t id);

    bool set_nrgHeat(const char * value, const int8_t id);
    bool set_nrgWw(const char * value, const int8_t id);
    bool set_nomPower(const char * value, const int8_t id);
};

} // namespace emsesp

#endif