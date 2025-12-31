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

#ifndef EMSESP_SOLAR_H
#define EMSESP_SOLAR_H

#include "emsesp.h"

namespace emsesp {

class Solar : public EMSdevice {
  public:
    Solar(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand);

  private:
    static uuid::log::Logger logger_;

    int16_t collectorTemp_;     // TS1: Temperature sensor for collector array 1
    int16_t cylBottomTemp_;     // TS2: Temperature sensor 1 cylinder, bottom cyl (solar thermal system)
    int16_t cylBottomTemp2_;    // TS5: Temperature sensor 2 cylinder, bottom cyl, or swimming pool (solar thermal system)
    int16_t cylBottomTemp3_;    // TS11: Temperature sensor 3. cylinder
    int16_t cylTopTemp_;        // TS10: Temperature sensor 1 cylinder, Top
    int16_t heatExchangerTemp_; // TS6: Heat exchanger temperature sensor
    int16_t collector2Temp_;    // TS7: Temperature sensor for collector array 2
    int16_t ts8_;               // TS8: Temperature sensor for ?
    int16_t ts16_;              // TS16: Temperature sensor for ?
    int16_t cylMiddleTemp_;     // TS14: Cylinder middle temp
    int16_t retHeatAssist_;     // TS15: return temperature heating assistance
    uint8_t solarPumpMod_;      // PS1: modulation solar pump
    uint8_t cylPumpMod_;        // PS5: modulation cylinder pump
    uint8_t solarPump_;         // PS1: solar pump active
    uint8_t valveStatus_;       // VS2: status 3-way valve for cylinder 2 (solar thermal system) with valve
    uint8_t solarPump2_;        // PS4: solar pump 2 active
    uint8_t solarPump2Mod_;     // PS4: modulation solar pump
    uint8_t m1Valve_;           // M1:  heat assistance valve
    uint8_t m1Power_;           // M1:  heat assistance valve
    uint8_t vs1Status_;         // VS1: status
    uint8_t vs3Status_;         // VS3: status
    uint8_t transferPump_;
    uint8_t transferPumpMod_;

    // 0x363 heat counter
    uint16_t heatCntFlowTemp_;
    uint16_t heatCntRetTemp_;
    uint8_t  heatCnt_;
    uint16_t swapFlowTemp_;
    uint16_t swapRetTemp_;

    // 0x38E
    uint32_t energyLastHour_;
    uint32_t energyToday_;
    uint32_t energyTotal_;
    uint32_t pumpWorkTime_;  // Total solar pump operating time
    uint32_t pump2WorkTime_; // Total solar pump 2 operating time
    uint32_t m1WorkTime_;    // differential control work time
    uint8_t  cylHeated_;
    uint8_t  collectorShutdown_; // Collector shutdown on/off

    uint8_t availabilityFlag_;
    uint8_t configFlag_;
    uint8_t userFlag_;

    // telegram 0x0358
    uint8_t heatTransferSystem_; // Umladesystem, 00=no
    uint8_t externalCyl_;        // Heat exchanger, 00=no
    uint8_t thermalDisinfect_;   // Daily heatup for disinfection, 00=no
    uint8_t heatMetering_;       // Heat quantity metering, 00=no
    uint8_t solarIsEnabled_;     // System enable, 00=no

    // telegram 0x035A
    uint8_t collectorMaxTemp_; // maximum allowed collectorTemp array 1
    uint8_t cylMaxTemp_;       // Current value for max cyl temp
    // uint8_t cyl2MaxTemp_;           // Current value for max cyl temp
    uint8_t collectorMinTemp_;     // minimum allowed collectorTemp array 1
    uint8_t solarPumpMode_;        // 00=off, 01=PWM, 02=10V
    uint8_t solarPumpMinMod_;      // minimum modulation setting
    uint8_t solarPumpTurnoffDiff_; // solar pump turnoff collector/cyl diff
    uint8_t solarPumpTurnonDiff_;  // solar pump turnon collector/cyl diff
    uint8_t solarPumpKick_;        // pump kick for vacuum collector, 00=off
    uint8_t plainWaterMode_;       // system does not use antifreeze, 00=off
    uint8_t doubleMatchFlow_;      // double Match Flow, 00=off

    // telegram 0x035D
    uint8_t collector2MaxTemp_;     // maximum allowed collectorTemp array 1
    uint8_t collector2MinTemp_;     // minimum allowed collectorTemp array 1
    uint8_t solarPump2MinMod_;      // minimum modulation setting
    uint8_t solarPump2TurnoffDiff_; // solar pump turnoff collector/cyl diff
    uint8_t solarPump2TurnonDiff_;  // solar pump turnon collector/cyl diff
    uint8_t solarPump2Kick_;        // pump kick for vacuum collector, 00=off
    uint8_t solarPump2Mode_;        // 00=off, 01=PWM, 02=10V

    // telegram 0x35C Heat assistance
    int8_t heatAssistOn_;  // is *10
    int8_t heatAssistOff_; // is *10

    // telegram 0x035F
    uint8_t cylPriority_; // 0 or 1

    // telegram 0x361 Differential control
    uint8_t diffControl_; // is *10


    // telegram 0x380
    uint8_t  climateZone_;    // climate zone identifier
    uint16_t collector1Area_; // Area of collector field 1
    uint8_t  collector1Type_; // Type of collector field 1, 01=flat, 02=vacuum
    uint16_t collector2Area_; // Area of collector field 2
    uint8_t  collector2Type_; // Type of collector field 2, 01=flat, 02=vacuum

    // SM10Config - 0x96
    uint8_t wwMinTemp_;
    uint8_t maxFlow_;    // set this to calculate power
    int16_t solarPower_; // calculated from maxFlow

    uint8_t data0_;
    uint8_t data1_;
    uint8_t data11_;
    uint8_t data12_;
    uint8_t setting3_;
    uint8_t setting4_;

    std::deque<int16_t> energy;

    void process_SM10Monitor(std::shared_ptr<const Telegram> telegram);
    void process_SM10Config(std::shared_ptr<const Telegram> telegram);
    void process_SM100SystemConfig(std::shared_ptr<const Telegram> telegram);
    void process_SM100CircuitConfig(std::shared_ptr<const Telegram> telegram);
    void process_SM100Circuit2Config(std::shared_ptr<const Telegram> telegram);
    void process_SM100ParamCfg(std::shared_ptr<const Telegram> telegram);
    void process_SM100Monitor(std::shared_ptr<const Telegram> telegram);
    void process_SM100Monitor2(std::shared_ptr<const Telegram> telegram);

    void process_SM100Config(std::shared_ptr<const Telegram> telegram);
    void process_SM100Config1(std::shared_ptr<const Telegram> telegram);

    void process_SM100Status(std::shared_ptr<const Telegram> telegram);
    void process_SM100Status2(std::shared_ptr<const Telegram> telegram);
    void process_SM100CollectorConfig(std::shared_ptr<const Telegram> telegram);
    void process_SM100Energy(std::shared_ptr<const Telegram> telegram);
    void process_SM100Time(std::shared_ptr<const Telegram> telegram);

    void process_SM100HeatAssist(std::shared_ptr<const Telegram> telegram);
    void process_SM100Differential(std::shared_ptr<const Telegram> telegram);

    void process_ISM1StatusMessage(std::shared_ptr<const Telegram> telegram);
    void process_ISM1Set(std::shared_ptr<const Telegram> telegram);
    void process_ISM2StatusMessage(std::shared_ptr<const Telegram> telegram);

    // settings
    bool set_CollectorMaxTemp(const char * value, const int8_t id);
    bool set_CollectorMinTemp(const char * value, const int8_t id);
    bool set_cylMaxTemp(const char * value, const int8_t id);
    bool set_PumpMinMod(const char * value, const int8_t id);
    bool set_wwMinTemp(const char * value, const int8_t id);
    bool set_TurnonDiff(const char * value, const int8_t id);
    bool set_TurnoffDiff(const char * value, const int8_t id);

    bool set_Collector2MaxTemp(const char * value, const int8_t id);
    bool set_Collector2MinTemp(const char * value, const int8_t id);
    bool set_Pump2MinMod(const char * value, const int8_t id);
    bool set_TurnonDiff2(const char * value, const int8_t id);
    bool set_TurnoffDiff2(const char * value, const int8_t id);

    bool set_SM10MaxFlow(const char * value, const int8_t id);
    // SM100
    bool set_heatTransferSystem(const char * value, const int8_t id);
    bool set_externalCyl(const char * value, const int8_t id);
    bool set_thermalDisinfect(const char * value, const int8_t id);
    bool set_heatMetering(const char * value, const int8_t id);
    bool set_solarEnabled(const char * value, const int8_t id);
    bool set_solarMode(const char * value, const int8_t id);
    bool set_solarPumpKick(const char * value, const int8_t id);
    bool set_solarPump2Kick(const char * value, const int8_t id);
    bool set_plainWaterMode(const char * value, const int8_t id);
    bool set_doubleMatchFlow(const char * value, const int8_t id);
    bool set_climateZone(const char * value, const int8_t id);
    bool set_collector1Area(const char * value, const int8_t id);
    bool set_collector1Type(const char * value, const int8_t id);
    bool set_collector2Area(const char * value, const int8_t id);
    bool set_collector2Type(const char * value, const int8_t id);
    bool set_cylPriority(const char * value, const int8_t id);
    bool set_heatAssist(const char * value, const int8_t id);
    bool set_diffControl(const char * value, const int8_t id);
    bool set_solarHeatAssistOn(const char * value, const int8_t id);
    bool set_solarHeatAssistOff(const char * value, const int8_t id);
};

} // namespace emsesp

#endif
