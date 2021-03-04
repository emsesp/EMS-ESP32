/*
 * EMS-ESP - https://github.com/proddy/EMS-ESP
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

    void check_active(const bool force = false);

    uint8_t boilerState_ = EMS_VALUE_UINT_NOTSET; // Boiler state flag - FOR INTERNAL USE

    static constexpr uint8_t  EMS_TYPE_UBAParameterWW     = 0x33;
    static constexpr uint8_t  EMS_TYPE_UBAFunctionTest    = 0x1D;
    static constexpr uint8_t  EMS_TYPE_UBAFlags           = 0x35;
    static constexpr uint8_t  EMS_TYPE_UBASetPoints       = 0x1A;
    static constexpr uint8_t  EMS_TYPE_UBAParameters      = 0x16;
    static constexpr uint8_t  EMS_TYPE_UBAParametersPlus  = 0xE6;
    static constexpr uint8_t  EMS_TYPE_UBAParameterWWPlus = 0xEA;
    static constexpr uint16_t EMS_TYPE_UBAInfomration     = 0x495;
    static constexpr uint16_t EMS_TYPE_UBAEnergySupplied  = 0x494;

    static constexpr uint8_t EMS_BOILER_SELFLOWTEMP_HEATING = 20; // was originally 70, changed to 30 for issue #193, then to 20 with issue #344

    // ww
    uint8_t  wWSetTemp_;          // Warm Water set temperature
    uint8_t  wWSelTemp_;          // Warm Water selected temperature
    uint8_t  wWType_;             // 0-off, 1-flow, 2-flowbuffer, 3-buffer, 4-layered buffer
    uint8_t  wWComfort_;          // WW comfort mode
    uint8_t  wWCircPump_;         // Warm Water circulation pump available
    uint8_t  wWChargeType_;       // Warm Water charge type (pump or 3-way-valve)
    uint8_t  wWDisinfectionTemp_; // Warm Water disinfection temperature to prevent infection
    uint8_t  wWCircPumpMode_;     // Warm Water circulation pump mode
    uint8_t  wWCirc_;             // Circulation on/off
    uint16_t wWCurTemp_;          // Warm Water current temperature
    uint16_t wWCurTemp2_;         // Warm Water current temperature storage
    uint8_t  wWCurFlow_;          // Warm Water current flow temp in l/min
    uint16_t wWStorageTemp1_;     // warm water storage temp 1
    uint16_t wWStorageTemp2_;     // warm water storage temp 2
    uint8_t  wWActivated_;        // Warm Water activated
    uint8_t  wWOneTime_;          // Warm Water one time function on/off
    uint8_t  wWDisinfecting_;     // Warm Water disinfection on/off
    uint8_t  wWCharging_;         // Warm Water charging on/off
    uint8_t  wWRecharging_;       // Warm Water recharge on/off
    uint8_t  wWTempOK_;           // Warm Water temperature ok on/off
    uint8_t  wWActive_;           //
    uint8_t  wWHeat_;             // 3-way valve on WW
    uint8_t  wWSetPumpPower_;     // ww pump speed/power?
    uint32_t wWStarts_;           // Warm Water # starts
    uint32_t wWStarts2_;          // Warm water starts (control)
    uint32_t wWWorkM_;            // Warm Water # minutes

    uint16_t mixerTemp_;      // mixing temperature
    uint16_t tankMiddleTemp_; // Tank middle temperature (TS3)

    // main
    uint8_t  id_;               // product id
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
    uint32_t heatingStarts_;             // Heating starts (control)
    uint32_t coolingStarts_;             // Cooling  starts (control)
    uint32_t nrgConsTotal_;              // Energy consumption total
    uint32_t nrgConsCompTotal_;          // Energy consumption compressor total
    uint32_t nrgConsCompHeating_;        // Energy consumption compressor heating
    uint32_t nrgConsCompWw_;             // Energy consumption compressor warm water
    uint32_t nrgConsCompCooling_;        // Energy consumption compressor cooling
    uint32_t nrgSuppTotal_;              // Energy supplied total
    uint32_t nrgSuppHeating_;            // Energy supplied heating
    uint32_t nrgSuppWw_;                 // Energy supplied warm water
    uint32_t nrgSuppCooling_;            // Energy supplied cooling
    uint32_t auxElecHeatNrgConsTotal_;   // Auxiliary electrical heater energy consumption total
    uint32_t auxElecHeatNrgConsHeating_; // Auxiliary electrical heater energy consumption heating
    uint32_t auxElecHeatNrgConsDHW_;     // Auxiliary electrical heater energy consumption DHW
    char     maintenanceMessage_[4];
    char     maintenanceDate_[12];
    uint8_t  maintenanceType_;
    uint16_t maintenanceTime_;

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
    void process_MC10Status(std::shared_ptr<const Telegram> telegram);
    void process_UBAMaintenanceStatus(std::shared_ptr<const Telegram> telegram);
    void process_UBAMaintenanceData(std::shared_ptr<const Telegram> telegram);
    void process_UBAErrorMessage(std::shared_ptr<const Telegram> telegram);
    void process_UBADHWStatus(std::shared_ptr<const Telegram> telegram);
    void process_UBAInformation(std::shared_ptr<const Telegram> telegram);
    void process_UBAEnergySupplied(std::shared_ptr<const Telegram> telegram);

    // commands - none of these use the additional id parameter
    bool set_warmwater_mode(const char * value, const int8_t id);
    bool set_warmwater_activated(const char * value, const int8_t id);
    bool set_tapwarmwater_activated(const char * value, const int8_t id);
    bool set_warmwater_onetime(const char * value, const int8_t id);
    bool set_warmwater_circulation(const char * value, const int8_t id);
    bool set_warmwater_circulation_pump(const char * value, const int8_t id);
    bool set_warmwater_circulation_mode(const char * value, const int8_t id);
    bool set_warmwater_temp(const char * value, const int8_t id);
    bool set_flow_temp(const char * value, const int8_t id);
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
};

} // namespace emsesp

#endif
