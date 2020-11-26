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

#include <Arduino.h>
#include <ArduinoJson.h>

#include <uuid/log.h>

#include "emsdevice.h"
#include "emsesp.h"
#include "telegram.h"
#include "helpers.h"
#include "mqtt.h"

namespace emsesp {

class Solar : public EMSdevice {
  public:
    Solar(uint8_t device_type, uint8_t device_id, uint8_t product_id, const std::string & version, const std::string & name, uint8_t flags, uint8_t brand);

    virtual void publish_values(JsonObject & json, bool force);
    virtual bool export_values(JsonObject & json);
    virtual void device_info_web(JsonArray & root);
    virtual bool updated_values();

  private:
    static uuid::log::Logger logger_;
    void                     register_mqtt_ha_config();

    int16_t  collectorTemp_          = EMS_VALUE_SHORT_NOTSET; // TS1: Temperature sensor for collector array 1
    int16_t  tankBottomTemp_         = EMS_VALUE_SHORT_NOTSET; // TS2: Temperature sensor 1 cylinder, bottom (solar thermal system)
    int16_t  tankBottomTemp2_        = EMS_VALUE_SHORT_NOTSET; // TS5: Temperature sensor 2 cylinder, bottom, or swimming pool (solar thermal system)
    int16_t  heatExchangerTemp_      = EMS_VALUE_SHORT_NOTSET; // TS6: Heat exchanger temperature sensor
    uint8_t  solarPumpModulation_    = EMS_VALUE_UINT_NOTSET;  // PS1: modulation solar pump
    uint8_t  cylinderPumpModulation_ = EMS_VALUE_UINT_NOTSET;  // PS5: modulation cylinder pump
    uint8_t  solarPump_              = EMS_VALUE_BOOL_NOTSET;  // PS1: solar pump active
    uint8_t  valveStatus_            = EMS_VALUE_BOOL_NOTSET;  // VS2: status 3-way valve for cylinder 2 (solar thermal system) with valve
    int16_t  setpoint_maxBottomTemp_ = EMS_VALUE_SHORT_NOTSET; // setpoint for maximum collector temp
    uint32_t energyLastHour_         = EMS_VALUE_ULONG_NOTSET;
    uint32_t energyToday_            = EMS_VALUE_ULONG_NOTSET;
    uint32_t energyTotal_            = EMS_VALUE_ULONG_NOTSET;
    uint32_t pumpWorkMin_            = EMS_VALUE_ULONG_NOTSET; // Total solar pump operating time
    uint8_t  tankHeated_             = EMS_VALUE_BOOL_NOTSET;
    uint8_t  collectorShutdown_      = EMS_VALUE_BOOL_NOTSET; // Collector shutdown on/off

    uint8_t availabilityFlag_ = EMS_VALUE_BOOL_NOTSET;
    uint8_t configFlag_       = EMS_VALUE_BOOL_NOTSET;
    uint8_t userFlag_         = EMS_VALUE_BOOL_NOTSET;

    // telegram 0x0358
    uint8_t heatTransferSystem_ = EMS_VALUE_UINT_NOTSET; // Umladesystem, 00=no
    uint8_t externalTank_       = EMS_VALUE_UINT_NOTSET; // Heat exchanger, 00=no
    uint8_t thermalDisinfect_   = EMS_VALUE_UINT_NOTSET; // Daily heatup for disinfection, 00=no
    uint8_t heatMetering_       = EMS_VALUE_UINT_NOTSET; // Wärmemengenzählung, 00=no
    uint8_t solarIsEnabled_     = EMS_VALUE_UINT_NOTSET; // System enable, 00=no

    // telegram 0x035A
    uint8_t collectorTempMax_     = EMS_VALUE_UINT_NOTSET; // maximum allowable temperature for collector
    uint8_t tank1MaxTempCurrent_  = EMS_VALUE_UINT_NOTSET; // Current value for max tank temp
    uint8_t collectorTempMin_     = EMS_VALUE_UINT_NOTSET; // minimum allowable temperature for collector
    uint8_t solarPumpMode_        = EMS_VALUE_UINT_NOTSET; // 00=off, 01=PWM, 02=10V
    uint8_t solarPumpMinRPM_      = EMS_VALUE_UINT_NOTSET; // minimum RPM setting, *5 %
    uint8_t solarPumpTurnoffDiff_ = EMS_VALUE_UINT_NOTSET; // solar pump turnoff collector/tank diff
    uint8_t solarPumpTurnonDiff_  = EMS_VALUE_UINT_NOTSET; // solar pump turnon collector/tank diff
    uint8_t solarPumpKick_        = EMS_VALUE_UINT_NOTSET; // pump kick for vacuum collector, 00=off
    uint8_t plainWaterMode_       = EMS_VALUE_UINT_NOTSET; // system does not use antifreeze, 00=off
    uint8_t doubleMatchFlow_      = EMS_VALUE_UINT_NOTSET; // double Match Flow, 00=off

    // telegram 0x380
    uint8_t  climateZone_    = EMS_VALUE_UINT_NOTSET;   // climate zone identifier
    uint16_t collector1Area_ = EMS_VALUE_USHORT_NOTSET; // Area of collector field 1
    uint8_t  collector1Type_ = EMS_VALUE_UINT_NOTSET;   // Type of collector field 1, 01=flat, 02=vacuum

    bool changed_        = false;
    bool mqtt_ha_config_ = false; // for HA MQTT Discovery

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


    bool set_SM100Tank1MaxTemp(const char * value, const int8_t id);
};

} // namespace emsesp

#endif
