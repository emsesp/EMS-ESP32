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

#ifndef EMSESP_SOLAR_H
#define EMSESP_SOLAR_H

#include "emsesp.h"

namespace emsesp {

class Solar : public EMSdevice {
  public:
    Solar(uint8_t device_type, uint8_t device_id, uint8_t product_id, const std::string & version, const std::string & name, uint8_t flags, uint8_t brand);

    virtual bool publish_ha_config();

  private:
    static uuid::log::Logger logger_;

    int16_t  collectorTemp_;          // TS1: Temperature sensor for collector array 1
    int16_t  tankBottomTemp_;         // TS2: Temperature sensor 1 cylinder, bottom tank (solar thermal system)
    int16_t  tankBottomTemp2_;        // TS5: Temperature sensor 2 cylinder, bottom tank, or swimming pool (solar thermal system)
    int16_t  heatExchangerTemp_;      // TS6: Heat exchanger temperature sensor
    uint8_t  solarPumpModulation_;    // PS1: modulation solar pump
    uint8_t  cylinderPumpModulation_; // PS5: modulation cylinder pump
    uint8_t  solarPump_;              // PS1: solar pump active
    uint8_t  valveStatus_;            // VS2: status 3-way valve for cylinder 2 (solar thermal system) with valve
    int16_t  setpoint_maxBottomTemp_; // setpoint for maximum collector temp
    uint32_t energyLastHour_;
    uint32_t energyToday_;
    uint32_t energyTotal_;
    uint32_t pumpWorkTime_; // Total solar pump operating time
    uint8_t  tankHeated_;
    uint8_t  collectorShutdown_; // Collector shutdown on/off

    uint8_t availabilityFlag_;
    uint8_t configFlag_;
    uint8_t userFlag_;

    // telegram 0x0358
    uint8_t heatTransferSystem_; // Umladesystem, 00=no
    uint8_t externalTank_;       // Heat exchanger, 00=no
    uint8_t thermalDisinfect_;   // Daily heatup for disinfection, 00=no
    uint8_t heatMetering_;       // Wärmemengenzählung, 00=no
    uint8_t solarIsEnabled_;     // System enable, 00=no

    // telegram 0x035A
    uint8_t collectorMaxTemp_;     // maximum allowed collectorTemp array 1
    uint8_t tankBottomMaxTemp_;    // Current value for max tank temp
    uint8_t collectorMinTemp_;     // minimum allowed collectorTemp array 1
    uint8_t solarPumpMode_;        // 00=off, 01=PWM, 02=10V
    uint8_t solarPumpMinRPM_;      // minimum RPM setting, *5 %
    uint8_t solarPumpTurnoffDiff_; // solar pump turnoff collector/tank diff
    uint8_t solarPumpTurnonDiff_;  // solar pump turnon collector/tank diff
    uint8_t solarPumpKick_;        // pump kick for vacuum collector, 00=off
    uint8_t plainWaterMode_;       // system does not use antifreeze, 00=off
    uint8_t doubleMatchFlow_;      // double Match Flow, 00=off

    // telegram 0x380
    uint8_t  climateZone_;    // climate zone identifier
    uint16_t collector1Area_; // Area of collector field 1
    uint8_t  collector1Type_; // Type of collector field 1, 01=flat, 02=vacuum

    char    type_[20]; // Solar of WWC
    uint8_t id_;

    void process_SM10Monitor(std::shared_ptr<const Telegram> telegram);
    void process_SM100SystemConfig(std::shared_ptr<const Telegram> telegram);
    void process_SM100SolarCircuitConfig(std::shared_ptr<const Telegram> telegram);
    void process_SM100ParamCfg(std::shared_ptr<const Telegram> telegram);
    void process_SM100Monitor(std::shared_ptr<const Telegram> telegram);
    void process_SM100Monitor2(std::shared_ptr<const Telegram> telegram);

    void process_SM100Config(std::shared_ptr<const Telegram> telegram);

    void process_SM100Status(std::shared_ptr<const Telegram> telegram);
    void process_SM100Status2(std::shared_ptr<const Telegram> telegram);
    void process_SM100CollectorConfig(std::shared_ptr<const Telegram> telegram);
    void process_SM100Energy(std::shared_ptr<const Telegram> telegram);
    void process_SM100Time(std::shared_ptr<const Telegram> telegram);

    void process_SM100wwTemperature(std::shared_ptr<const Telegram> telegram);
    void process_SM100wwStatus(std::shared_ptr<const Telegram> telegram);
    void process_SM100wwCommand(std::shared_ptr<const Telegram> telegram);

    void process_ISM1StatusMessage(std::shared_ptr<const Telegram> telegram);
    void process_ISM1Set(std::shared_ptr<const Telegram> telegram);


    bool set_SM100TankBottomMaxTemp(const char * value, const int8_t id);
};

} // namespace emsesp

#endif
