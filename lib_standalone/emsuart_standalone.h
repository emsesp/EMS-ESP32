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

#ifndef EMSESP_EMSUART_H
#define EMSESP_EMSUART_H

#include "Arduino.h"

namespace emsesp {

#define EMS_TX_STATUS_ERR 0
#define EMS_TX_STATUS_OK 1

class EMSuart {
  public:
    EMSuart()  = default;
    ~EMSuart() = default;

    static void     start(uint8_t tx_mode, uint8_t rx_gpio, uint8_t tx_gpio);
    static void     stop();
    static void     restart();
    static void     send_poll(uint8_t data);
    static uint16_t transmit(uint8_t * buf, uint8_t len);
    static uint8_t  last_tx_src() {
        return 0;
    }

  private:
    static char * hextoa(char * result, const uint8_t value);
};

} // namespace emsesp

#endif
