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

#ifndef EMSESP_ROOMCONTROL_H
#define EMSESP_ROOMCONTROL_H

#include "emsesp.h"

namespace emsesp {

class Roomctrl {
  public:
    static void send(const uint8_t addr);
    static void check(const uint8_t addr, const uint8_t * data);
    static void set_remotetemp(const uint8_t hc, const int16_t temp);

  private:
    static constexpr uint8_t  ADDR          = 0x18;
    static constexpr uint32_t SEND_INTERVAL = 60000; // 1 minute
    static constexpr uint8_t  HCS           = 4;     // max 4 heating circuits

    static void version(uint8_t addr, uint8_t dst);
    static void unknown(uint8_t addr, uint8_t dst, uint8_t type, uint8_t offset);
    static void temperature(uint8_t addr, uint8_t dst);
    static void nack_write();

    static bool     switch_off_[HCS];
    static uint32_t rc_time_[HCS];
    static int16_t  remotetemp_[HCS];
};

} // namespace emsesp

#endif
