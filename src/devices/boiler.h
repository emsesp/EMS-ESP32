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
    Boiler(uint8_t device_type, int8_t device_id, uint8_t product_id, const std::string & version, const std::string & name, uint8_t flags, uint8_t brand);

    virtual bool publish_ha_config();

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

    static constexpr uint8_t EMS_BOILER_SELFLOWTEMP_HEATING = 20; // was originally 70, changed to 30 for issue #193, then to 20 with issue #344

    // ww
    uint8_t  wwSetTemp_;          // Warm Water set temperature
    uint8_t  wwSelTemp_;          // Warm Water selected temperature
    uint8_t  wwSelTempLow_;       // Warm Water lower selected temperature
    uint8_t  wwSelTempOff_;       // Warm Water selected temperature for off position
    uint8_t  wwSelTempSingle_;    // Warm Water single charge temperature
    uint8_t  wwType_;             // 0-off, 1-flow, 2-flowbuffer, 3-buffer, 4-layered buffer
    uint8_t  wwComfort_;          // WW comfort mode
    uint8_t  wwCircPump_;         // Warm Water circulation pump available
    uint8_t  wWChargeType_;       // Warm Water charge type (pump or 3-way-valve)
    uint8_t  wwDisinfectionTemp_; // Warm Water disinfection temperature to prevent infection
    uint8_t  wwCircMode_;         // Warm Water circulation pump mode
    uint8_t  wwCirc_;             // Circulation on/off
    uint16_t wwCurTemp_;          // Warm Water current temperature
    uint16_t wwCurTemp2_;         // Warm Water current temperature storage
    uint8_t  wwCurFlow_;          // Warm Water current flow temp in l/min
    uint16_t wwStorageTemp1_;     // warm water storage temp 1
    uint16_t wwStorageTemp2_;     // warm water storage temp 2
    uint8_t  wwActivated_;        // Warm Water activated
    uint8_t  wwOneTime_;          // Warm Water one time function on/off
    uint8_t  wwDisinfect_;        // Warm Water disinfection on/off
    uint8_t  wwCharging_;         // Warm Water charging on/off
    uint8_t  wwRecharging_;       // Warm Water recharge on/off
    uint8_t  wwTempOK_;           // Warm Water temperature ok on/off
    uint8_t  wwActive_;           //
    uint8_t  wwHeat_;             // 3-way valve on WW
    uint8_t  wwSetPumpPower_;     // ww pump speed/power?
    uint8_t  wwFlowTempOffset_;   // Boiler offset for ww heating
    uint8_t  wwMaxPower_;         // Warm Water maximum power
    uint32_t wWStarts_;           // Warm Water # starts
    uint32_t wwStarts2_;          // Warm water control starts
    uint32_t wwWorkM_;            // Warm Water # minutes
    int8_t   wwHystOn_;
    int8_t   wwHystOff_;
    uint8_t  wwTapActivated_;   // maintenance-mode to switch DHW off
    uint16_t wwMixerTemp_;      // mixing temperature
    uint16_t wwTankMiddleTemp_; // Tank middle temperature (TS3)

    // main
    uint8_t  id_;               // product id
    uint8_t  dummy8u_;          // for commands with no output
    uint8_t  dummybool_;        // for commands with no output
    uint8_t  heatingActive_;    // Central heating is on/off
    uint8_t  tapwaterActive_;   // Hot tap water is on/off
    uint8_t  selFlowTemp_;      // Selected flow temperature
    uint8_t  selBurnPow_;       // Burner max power %
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
    uint16_t flameCurr_;        // Flame current in micro amps
    uint8_t  heatingPump_;      // Boiler heating pump on/off
    uint8_t  fanWork_;          // Fan on/off
    uint8_t  ignWork_;          // Ignition on/off
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
    uint32_t burnStarts_;        // # burner restarts
    uint32_t burnWorkMin_;       // Total burner operating time
    uint32_t heatWorkMin_;       // Total heat operating time
    uint32_t UBAuptime_;         // Total UBA working hours
    char     lastCode_[30];      // last error code
    char     serviceCode_[4];    // 3 character status/service code
    uint16_t serviceCodeNumber_; // error/service code

    // info
    uint32_t upTimeControl_;             // Operating time control
    uint32_t upTimeCompHeating_;         // Operating time compressor heating
    uint32_t upTimeCompCooling_;         // Operating time compressor cooling
    uint32_t upTimeCompWw_;              // Operating time compressor warm water
    uint32_t upTimeCompPool_;            // Operating time compressor pool
    uint32_t totalCompStarts_;           // Total Commpressor control starts
    uint32_t heatingStarts_;             // Heating control starts
    uint32_t coolingStarts_;             // Cooling control starts
    uint32_t poolStarts_;                // Warm water control starts
    uint32_t nrgConsTotal_;              // Energy consumption total
    uint32_t nrgConsCompTotal_;          // Energy consumption compressor total
    uint32_t nrgConsCompHeating_;        // Energy consumption compressor heating
    uint32_t nrgConsCompWw_;             // Energy consumption compressor warm water
    uint32_t nrgConsCompCooling_;        // Energy consumption compressor cooling
    uint32_t nrgConsCompPool_;           // Energy consumption compressor pool
    uint32_t nrgSuppTotal_;              // Energy supplied total
    uint32_t nrgSuppHeating_;            // Energy supplied heating
    uint32_t nrgSuppWw_;                 // Energy supplied warm water
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
    uint8_t  hpHeatingOn;
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
    int8_t  poolSetTemp_;
    uint8_t poolShunt_;

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
    void process_HpPoolStatus(std::shared_ptr<const Telegram> telegram);


    // commands - none of these use the additional id parameter
    bool set_warmwater_mode(const char * value, const int8_t id);
    bool set_warmwater_activated(const char * value, const int8_t id);
    bool set_tapwarmwater_activated(const char * value, const int8_t id);
    bool set_warmwater_onetime(const char * value, const int8_t id);
    bool set_warmwater_disinfect(const char * value, const int8_t id);
    bool set_warmwater_circulation(const char * value, const int8_t id);
    bool set_warmwater_circulation_pump(const char * value, const int8_t id);
    bool set_warmwater_circulation_mode(const char * value, const int8_t id);
    bool set_warmwater_temp(const char * value, const int8_t id);
    bool set_warmwater_temp_low(const char * value, const int8_t id);
    bool set_warmwater_temp_single(const char * value, const int8_t id);
    bool set_disinfect_temp(const char * value, const int8_t id);
    bool set_warmwater_maxpower(const char * value, const int8_t id);
    bool set_wWFlowTempOffset(const char * value, const int8_t id);
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
    bool set_pool_pump(const char * value, const int8_t id);

};

} // namespace emsesp

#endif
