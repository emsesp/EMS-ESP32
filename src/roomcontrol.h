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
#ifndef EMSESP_ROOMCONTROL_H
#define EMSESP_ROOMCONTROL_H

#include "emsesp.h"

namespace emsesp {

class Roomctrl {
  public:
    // Product-Id of the remote
    enum RemoteType : uint8_t { NONE = 0, RC20 = 113, FB10 = 109, RC100H = 200, SENSOR = 0x40, RC200 = 157, RC100 = 165, RT800 = 3 };

    static void send(uint8_t addr);
    static void check(uint8_t addr, const uint8_t * data, const uint8_t length);
    static void set_remotetemp(const uint8_t type, const uint8_t hc, const int16_t temp);
    static void set_remotehum(const uint8_t type, const uint8_t hc, const int8_t hum);
    static bool is_remote(const uint8_t hc) {
        return (hc < 4 && remotetemp_[hc] != EMS_VALUE_INT16_NOTSET);
    }
    static void set_timeout(uint8_t t);

  private:
    static constexpr uint32_t SEND_INTERVAL = 15000; // 15 sec
    static constexpr uint8_t  HCS           = 4;     // max 4 heating circuits
    enum SendType : uint8_t { TEMP, HUMI };

    static uint8_t get_hc(const uint8_t addr);
    static void    version(uint8_t addr, uint8_t dst, uint8_t hc);
    static void    unknown(uint8_t addr, uint8_t dst, uint8_t type, uint8_t offset);
    static void    unknown(uint8_t addr, uint8_t dst, uint8_t offset, uint8_t typeh, uint8_t typel);
    static void    temperature(uint8_t addr, uint8_t dst, uint8_t hc);
    static void    humidity(uint8_t addr, uint8_t dst, uint8_t hc);
    static void    nack_write();
    static void    ack_write();
    static void    replyF7(uint8_t addr, uint8_t dst, uint8_t offset, uint8_t typehh, uint8_t typeh, uint8_t typel, uint8_t hc);
    static int16_t calc_dew(int16_t temp, uint8_t hum);

    static bool     switch_off_[HCS];
    static uint32_t send_time_[HCS];
    static uint32_t receive_time_[HCS];
    static int16_t  remotetemp_[HCS];
    static uint8_t  remotehum_[HCS];
    static uint8_t  sendtype_[HCS];
    static uint8_t  type_[HCS]; // type is product-id 113 for RC20 or 109 for Junkers FB10
    static uint32_t timeout_;
};

} // namespace emsesp

#endif
