/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2026  emsesp.org
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

#ifndef EMSESP_CONNECT_H
#define EMSESP_CONNECT_H

#include "emsesp.h"

namespace emsesp {

class Connect : public EMSdevice {
  public:
    Connect(uint8_t device_type, uint8_t device_id, uint8_t product_id, const char * version, const char * name, uint8_t flags, uint8_t brand);

    class RoomCircuit {
      public:
        RoomCircuit(const uint8_t num)
            : room_(num) {
        }
        ~RoomCircuit() = default;
        int16_t temp_;
        uint8_t humidity_;
        uint8_t seltemp_;
        uint8_t mode_;
        char    name_[51];
        int16_t dewtemp_;
        uint8_t childlock_;
        uint8_t icon_;
        uint8_t coolmode_;
        // uint8_t tempautotemp_;
        // uint8_t manualtemp_;
        // uint8_t coolautotemp_;
        // uint8_t cooltemp_;

        uint8_t room() {
            return room_;
        }

      private:
        uint8_t room_; // dhw circuit number 0..10
    };

  private:
    std::shared_ptr<Connect::RoomCircuit> room_circuit(const uint8_t num, const bool create = false);

    void register_device_values_room(std::shared_ptr<Connect::RoomCircuit> room);
    void process_roomThermostat(const std::shared_ptr<const Telegram> & telegram);
    void process_roomThermostatName(const std::shared_ptr<const Telegram> & telegram);
    void process_roomThermostatSettings(const std::shared_ptr<const Telegram> & telegram);
    void process_roomThermostatParam(const std::shared_ptr<const Telegram> & telegram);
    void process_roomThermostatData(const std::shared_ptr<const Telegram> & telegram);
    void process_roomSchedule(const std::shared_ptr<const Telegram> & telegram);
    void process_roomConfig(const std::shared_ptr<const Telegram> & telegram);
    bool set_mode(const char * value, const int8_t id);
    bool set_seltemp(const char * value, const int8_t id);
    bool set_name(const char * value, const int8_t id);
    bool set_childlock(const char * value, const int8_t id);
    bool set_icon(const char * value, const int8_t id);

    std::vector<std::shared_ptr<Connect::RoomCircuit>, AllocatorPSRAM<std::shared_ptr<Connect::RoomCircuit>>> room_circuits_;

    void    process_OutdoorTemp(const std::shared_ptr<const Telegram> & telegram);
    void    process_RCTime(const std::shared_ptr<const Telegram> & telegram);
    int16_t outdoorTemp_;
    char    dateTime_[25];  // date and time stamp
    uint8_t schedule_[126]; // telegram copy
};

} // namespace emsesp

#endif
