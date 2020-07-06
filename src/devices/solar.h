/*
 * EMS-ESP - https://github.com/proddy/EMS-ESP
 * Copyright 2019  Paul Derbyshire
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
#include "telegram.h"
#include "helpers.h"
#include "mqtt.h"

namespace emsesp {

class Solar : public EMSdevice {
  public:
    Solar(uint8_t device_type, uint8_t device_id, uint8_t product_id, const std::string & version, const std::string & name, uint8_t flags, uint8_t brand);

    virtual void show_values(uuid::console::Shell & shell);
    virtual void publish_values();
    virtual bool updated_values();
    virtual void add_context_menu();

  private:
    static uuid::log::Logger logger_;

    void console_commands();

    int16_t  collectorTemp_          = EMS_VALUE_SHORT_NOTSET; // collector temp (TS1)
    int16_t  bottomTemp_             = EMS_VALUE_SHORT_NOTSET; // bottom temp (TS2)
    int16_t  bottomTemp2_            = EMS_VALUE_SHORT_NOTSET; // bottom temp cylinder 2 (TS5)
    uint8_t  pumpModulation_         = EMS_VALUE_UINT_NOTSET;  // modulation solar pump
    uint8_t  pump_                   = EMS_VALUE_BOOL_NOTSET;  // pump active
    uint8_t  valveStatus_            = EMS_VALUE_BOOL_NOTSET;  // valve status (VS2)
    int16_t  setpoint_maxBottomTemp_ = EMS_VALUE_SHORT_NOTSET; // setpoint for maximum collector temp
    uint32_t energyLastHour_         = EMS_VALUE_ULONG_NOTSET;
    uint32_t energyToday_            = EMS_VALUE_ULONG_NOTSET;
    uint32_t energyTotal_            = EMS_VALUE_ULONG_NOTSET;
    uint32_t pumpWorkMin_            = EMS_VALUE_ULONG_NOTSET; // Total solar pump operating time
    uint8_t  tankHeated_             = EMS_VALUE_BOOL_NOTSET;
    uint8_t  collectorOnOff_         = EMS_VALUE_BOOL_NOTSET;

    uint8_t availabilityFlag_ = EMS_VALUE_BOOL_NOTSET;
    uint8_t configFlag_       = EMS_VALUE_BOOL_NOTSET;
    uint8_t userFlag_         = EMS_VALUE_BOOL_NOTSET;

    void process_SM10Monitor(std::shared_ptr<const Telegram> telegram);
    void process_SM100Monitor(std::shared_ptr<const Telegram> telegram);
    void process_SM100Monitor2(std::shared_ptr<const Telegram> telegram);

    void process_SM100Config(std::shared_ptr<const Telegram> telegram);

    void process_SM100Status(std::shared_ptr<const Telegram> telegram);
    void process_SM100Status2(std::shared_ptr<const Telegram> telegram);
    void process_SM100Energy(std::shared_ptr<const Telegram> telegram);

    void process_ISM1StatusMessage(std::shared_ptr<const Telegram> telegram);
    void process_ISM1Set(std::shared_ptr<const Telegram> telegram);
};

} // namespace emsesp

#endif
