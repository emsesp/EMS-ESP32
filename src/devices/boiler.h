/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020  Paul Derbyshire
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
    Boiler(uint8_t device_type, int8_t device_id, uint8_t product_id, const char * version, const std::string & name, uint8_t flags, uint8_t brand);

  private:
    static uuid::log::Logger logger_;

    // specific boiler characteristics, stripping the top 4 bits
    inline uint8_t model() const {
        return (flags() & 0x0F);
    }

    void check_active(const bool force = false);

    uint8_t boilerState_ = EMS_VALUE_UINT_NOTSET; // Boiler state flag - FOR INTERNAL USE

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
    uint8_t  wwSelTemp_;            // DHW selected temperature
    uint8_t  wwSelTempLow_;         // DHW lower selected temperature
    uint8_t  wwSelTempOff_;         // DHW selected temperature for off position
    uint8_t  wwSelTempSingle_;      // DHW single charge temperature
    uint8_t  wwType_;               // 0-off, 1-flow, 2-flowbuffer, 3-buffer, 4-layered buffer
    uint8_t  wwComfort_;            // WW comfort mode
    uint8_t  wwComfort1_;           // WW comfort mode RC310
    uint8_t  wwCircPump_;           // DHW circulation pump available
    uint8_t  wwChargeType_;         // DHW charge type (pump or 3-way-valve)
    uint8_t  wwChargeOptimization_; // DHW charge optimization
    uint8_t  wwDisinfectionTemp_;   // DHW disinfection temperature to prevent infection
    uint8_t  wwCircMode_;           // DHW circulation pump mode
    uint8_t  wwCirc_;               // Circulation on/off
    uint16_t wwCurTemp_;            // DHW current temperature
    uint16_t wwCurTemp2_;           // DHW current temperature storage
    uint8_t  wwCurFlow_;            // DHW current flow temp in l/min
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
    uint8_t  wwSetPumpPower_;       // ww pump speed/power?
    uint8_t  wwFlowTempOffset_;     // Boiler offset for ww heating
    uint8_t  wwMaxPower_;           // DHW maximum power
    uint8_t  wwMaxTemp_;            // DHW maximum temperature
    uint32_t wwStarts_;             // DHW starts
    uint32_t wwStarts2_;            // DHW control starts
    uint32_t wwWorkM_;              // DHW minutes
    int8_t   wwHystOn_;
    int8_t   wwHystOff_;
    uint8_t  wwTapActivated_;  // maintenance-mode to switch DHW off
    uint16_t wwMixerTemp_;     // mixing temperature
    uint16_t wwCylMiddleTemp_; // Cyl middle temperature (TS3)

    // main
    uint8_t  reset_;            // for reset command
    uint8_t  heatingActive_;    // Central heating is on/off
    uint8_t  tapwaterActive_;   // Hot tap water is on/off
    uint8_t  selFlowTemp_;      // Selected flow temperature
    uint8_t  selBurnPow_;       // Burner max power % (can be > 100%)
    uint8_t  heatingPump2Mod_;  // heatpump modulation from 0xE3 (heatpumps)
    uint8_t  heatingPumpMod_;   // Pump modulation %
    int16_t  outdoorTemp_;      // Outside temperature
    uint16_t curFlowTemp_;      // Current flow temperature
    uint16_t retTemp_;          // Return temperature
    uint16_t switchTemp_;       // Switch temperature
    uint8_t  sysPress_;         // System pressure
    uint16_t boilTemp_;         // Boiler temperature
    uint16_t exhaustTemp_;      // Exhaust temperature
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
    uint8_t  pumpDelay_;
    uint8_t  burnMinPeriod_;
    uint8_t  burnMinPower_;
    uint8_t  burnMaxPower_;
    int8_t   boilHystOn_;
    int8_t   boilHystOff_;
    uint8_t  setFlowTemp_;       // boiler setpoint temp
    uint8_t  curBurnPow_;        // Burner current power %
    uint8_t  setBurnPow_;        // max output power in %
    uint32_t burnStarts_;        // burner restarts
    uint32_t burnWorkMin_;       // Total burner operating time
    uint32_t burn2WorkMin_;      // burner stage 2 operating time
    uint32_t heatWorkMin_;       // Total heat operating time
    uint32_t UBAuptime_;         // Total UBA working hours
    char     lastCode_[55];      // last error code
    char     serviceCode_[4];    // 3 character status/service code
    uint16_t serviceCodeNumber_; // error/service code
    uint8_t  emergencyOps_;
    uint8_t  emergencyTemp_;

    // info
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
    uint32_t auxElecHeatNrgConsWW_;      // Auxiliary electrical heater energy consumption DHW
    uint32_t auxElecHeatNrgConsPool_;    // Auxiliary electrical heater energy consumption Pool
    char     maintenanceMessage_[4];
    char     maintenanceDate_[12];
    uint8_t  maintenanceType_;
    uint16_t maintenanceTime_;

    // heatpump
    uint8_t  hpPower_;
    uint8_t  hpCompOn_;
    uint8_t  hpBrinePumpSpd_;
    uint8_t  hpCompSpd_;
    uint8_t  hpCircSpd_;
    uint16_t hpBrineIn_;
    uint16_t hpBrineOut_;
    uint16_t hpSuctionGas_;
    uint16_t hpHotGas_;
    uint8_t  hpSwitchValve_;
    uint8_t  hpActivity_;
    uint8_t  hpHeatingOn_;
    uint8_t  hpCoolingOn_;
    uint8_t  hpWwOn_;
    uint8_t  hpPoolOn_;
    int16_t  hpTc0_;
    int16_t  hpTc1_;
    int16_t  hpTc3_;
    int16_t  hpTr3_;
    int16_t  hpTr4_;
    int16_t  hpTr5_;
    int16_t  hpTr6_;
    int16_t  hpTr7_;
    int16_t  hpTl2_;
    int16_t  hpPl1_;
    int16_t  hpPh1_;

    // Pool unit
    int8_t poolSetTemp_;

    // Alternative Heatsource AM200
    int16_t cylTopTemp_;    // TB1
    int16_t cylCenterTemp_; // TB2
    int16_t cylBottomTemp_; // TB3
    int16_t aFlowTemp_;     // TA1
    int16_t aRetTemp_;      // TR1
    uint8_t aPumpMod_;      // PR1 - percent
    // uint8_t valveByPass_;    // VR2
    uint8_t valveBuffer_; // VB1
    uint8_t valveReturn_; // VR1
    // uint8_t heatSource_;     // OEV
    // Settings:
    uint8_t  vr2Config_;     // pos 12: off(00)/Keelbypass(01)/(hc1pump(02) only standalone)
    uint8_t  ahsActivated_;  // pos 00: Alternate heat source activation: No(00),Yes(01)
    uint8_t  aPumpConfig_;   // pos 04: Buffer primary pump->Config pump: No(00),Yes(01)
    uint8_t  aPumpSignal_;   // pos 03: Output for PR1 pump: On/Off(00),PWM(01),PWM invers(02)
    uint8_t  aPumpMin_;      // pos 21: Min output pump PR1 (%)
    uint8_t  tempRise_;      // pos 01: AHS return temp rise: No(00),Yes(01) (mixer VR1)
    uint8_t  setReturnTemp_; // pos 06: Set temp return (°C) (VR1)
    uint16_t mixRuntime_;    // pos 10/11?: Mixer run time (s) (VR1)
    // uint8_t  setFlowTemp_;   // pos 07: Set flow temp AHS (°C) (Buffer)
    uint8_t  bufBypass_;     // pos 02: Puffer bypass: No(00), Mischer(01), Ventil(02) (Buffer)
    uint16_t bufMixRuntime_; // pos 8/9: Bypass mixer run time: [time] (s) (Buffer)
    uint8_t  bufConfig_;     // pos 20: Konfig WW-Speicher Monovalent(01), Bivalent(02) (buffer)
    uint8_t  blockMode_;     // pos 16: Config htg. blocking mode: No(00),Automatic(01),Always block02) (blocking)
    uint8_t  blockTerm_;     // pos 17: Config of block terminal: NO(00), NC(01)
    int8_t   blockHyst_;     // pos 14?: Hyst. for bolier block (K)
    uint8_t  releaseWait_;   // pos 15: Boiler release wait time (min)

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
    void process_HpOutdoor(std::shared_ptr<const Telegram> telegram);
    void process_HpPool(std::shared_ptr<const Telegram> telegram);
    void process_HybridHp(std::shared_ptr<const Telegram> telegram);
    void process_amTempMessage(std::shared_ptr<const Telegram> telegram);
    void process_amStatusMessage(std::shared_ptr<const Telegram> telegram);
    void process_amSettingMessage(std::shared_ptr<const Telegram> telegram);
    void process_amCommandMessage(std::shared_ptr<const Telegram> telegram);
    void process_amExtraMessage(std::shared_ptr<const Telegram> telegram);

    // commands - none of these use the additional id parameter
    bool set_ww_mode(const char * value, const int8_t id);
    bool set_ww_activated(const char * value, const int8_t id);
    bool set_tapwarmwater_activated(const char * value, const int8_t id);
    bool set_ww_onetime(const char * value, const int8_t id);
    bool set_ww_disinfect(const char * value, const int8_t id);
    bool set_ww_circulation(const char * value, const int8_t id);
    bool set_ww_circulation_pump(const char * value, const int8_t id);
    bool set_ww_circulation_mode(const char * value, const int8_t id);
    bool set_ww_temp(const char * value, const int8_t id);
    bool set_ww_temp_low(const char * value, const int8_t id);
    bool set_ww_temp_single(const char * value, const int8_t id);
    bool set_ww_disinfect_temp(const char * value, const int8_t id);
    bool set_ww_maxpower(const char * value, const int8_t id);
    bool set_ww_maxtemp(const char * value, const int8_t id);
    bool set_ww_flowTempOffset(const char * value, const int8_t id);
    bool set_ww_chargeOptimization(const char * value, const int8_t id);
    bool set_flow_temp(const char * value, const int8_t id);
    bool set_burn_power(const char * value, const int8_t id);
    bool set_heating_activated(const char * value, const int8_t id);
    bool set_heating_temp(const char * value, const int8_t id);
    bool set_min_power(const char * value, const int8_t id);
    bool set_max_power(const char * value, const int8_t id);
    bool set_min_pump(const char * value, const int8_t id);
    bool set_max_pump(const char * value, const int8_t id);
    bool set_hyst_on(const char * value, const int8_t id);
    bool set_hyst_off(const char * value, const int8_t id);
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

    bool set_vr2Config(const char * value, const int8_t id);     // pos 12: off(00)/Keelbypass(01)/(hc1pump(02) only standalone)
    bool set_ahsActivated(const char * value, const int8_t id);  // pos 00: Alternate heat source activation: No(00),Yes(01)
    bool set_aPumpConfig(const char * value, const int8_t id);   // pos 04: Buffer primary pump->Config pump: No(00),Yes(01)
    bool set_aPumpSignal(const char * value, const int8_t id);   // pos 03: Output for PR1 pump: On/Off(00),PWM(01),PWM invers(02)
    bool set_aPumpMin(const char * value, const int8_t id);      // pos 21: Min output pump PR1 (%)
    bool set_tempRise(const char * value, const int8_t id);      // pos 01: AHS return temp rise: No(00),Yes(01) (mixer VR1)
    bool set_setReturnTemp(const char * value, const int8_t id); // pos 06: Set temp return (°C) (VR1)
    bool set_mixRuntime(const char * value, const int8_t id);    // pos 10/11?: Mixer run time (s) (VR1)
    bool set_setFlowTemp(const char * value, const int8_t id);   // pos 07: Set flow temp AHS (°C) (Buffer)
    bool set_bufBypass(const char * value, const int8_t id);     // pos 02: Puffer bypass: No(00), Mischer(01), Ventil(02) (Buffer)
    bool set_bufMixRuntime(const char * value, const int8_t id); // pos 8/9: Bypass mixer run time: [time] (s) (Buffer)
    bool set_bufConfig(const char * value, const int8_t id);     // pos 20: Konfig WW-Speicher Monovalent(01), Bivalent(02) (buffer)
    bool set_blockMode(const char * value, const int8_t id);     // pos 16: Config htg. blocking mode: No(00),Automatic(01),Always block02) (blocking)
    bool set_blockTerm(const char * value, const int8_t id);     // pos 17: Config of block terminal: NO(00), NC(01)
    bool set_blockHyst(const char * value, const int8_t id);     // pos 14?: Hyst. for bolier block (K)
    bool set_releaseWait(const char * value, const int8_t id);   // pos 15: Boiler release wait time (min)

    /*
    bool set_hybridStrategy(const char * value, const int8_t id);
    bool set_switchOverTemp(const char * value, const int8_t id);
    bool set_energyCostRatio(const char * value, const int8_t id);
    bool set_fossileFactor(const char * value, const int8_t id);
    bool set_electricFactor(const char * value, const int8_t id);
    bool set_delayBoiler(const char * value, const int8_t id);
    bool set_tempDiffBoiler(const char * value, const int8_t id);
    */
};

} // namespace emsesp

#endif
