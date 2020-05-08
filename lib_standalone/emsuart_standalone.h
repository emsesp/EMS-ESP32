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

#ifndef EMSESP_EMSUART_H
#define EMSESP_EMSUART_H

#include <Arduino.h>

namespace emsesp {

typedef enum {
    EMS_TX_STATUS_OK = 1,
    EMS_TX_WTD_TIMEOUT, // watchdog timeout during send
    EMS_TX_BRK_DETECT,  // incoming BRK during Tx
} EMSUART_STATUS;

class EMSuart {
  public:
    EMSuart()  = default;
    ~EMSuart() = default;

    static void           start(uint8_t tx_mode);
    static void           stop();
    static void           restart();
    static void           send_poll(uint8_t data);
    static EMSUART_STATUS transmit(uint8_t * buf, uint8_t len);

  private:
    static char *  hextoa(char * result, const uint8_t value);
};

} // namespace emsesp

#endif
