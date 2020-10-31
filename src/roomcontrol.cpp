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

#include "roomcontrol.h"

namespace emsesp {

static uint32_t rc_time_      = 0;
static int16_t  remotetemp[4] = {EMS_VALUE_SHORT_NOTSET, EMS_VALUE_SHORT_NOTSET, EMS_VALUE_SHORT_NOTSET, EMS_VALUE_SHORT_NOTSET};

/**
 * set the temperature,
 */
void Roomctrl::set_remotetemp(const uint8_t hc, const int16_t temp) {
    if (hc > 3) {
        return;
    }
    remotetemp[hc] = temp;
}

/**
 * if remote control is active send the temperature every minute
 */
void Roomctrl::send(const uint8_t addr) {
    uint8_t hc_ = addr - ADDR;
    // check address, reply only on addresses 0x18..0x1B
    if (hc_ > 3) {
        return;
    }
    // no reply if the temperature is not set
    if (remotetemp[hc_] == EMS_VALUE_SHORT_NOTSET) {
        return;
    }
    if (uuid::get_uptime() - rc_time_ > SEND_INTERVAL) { // send every minute
        rc_time_ = uuid::get_uptime();                   // use EMS-ESP's millis() to prevent overhead
        temperature(addr, 0x00);                         // send to all
    } else {
        // acknowledge every poll, otherwise the master shows error A22-816
        EMSuart::send_poll(addr);
    }
}

/**
 * check if there is a message for the remote room controller
 */
void Roomctrl::check(const uint8_t addr, const uint8_t * data) {
    uint8_t hc_ = (addr & 0x7F) - ADDR;

    // check address, reply only on addresses 0x18..0x1B
    if (hc_ > 3) {
        return;
    }
    // no reply if the temperature is not set
    if (remotetemp[hc_] == EMS_VALUE_SHORT_NOTSET) {
        return;
    }
    // reply to writes with write nack byte
    if (addr & 0x80) { // it's a write to us
        nack_write();  // we don't accept writes.
        return;
    }
    // reads: for now we only reply to version and remote temperature
    // empty message back if temperature not set or unknown message type
    if (data[2] == 0x02) {
        version(addr, data[0]);
    } else if (remotetemp[hc_] == EMS_VALUE_SHORT_NOTSET) {
        unknown(addr, data[0], data[2], data[3]);
    } else if (data[2] == 0xAF && data[3] == 0) {
        temperature(addr, data[0]);
    } else {
        unknown(addr, data[0], data[2], data[3]);
    }
}

/**
 * send version info RC20 (Prod. 113, Ver. 02.01) or RC20RF (Prod. 93, Ver. 02.00)
 */
void Roomctrl::version(uint8_t addr, uint8_t dst) {
    uint8_t data[10];
    data[0] = addr;
    data[1] = dst;
    data[2] = 0x02;
    data[3] = 0;
    data[4] = 113; // set RC20 id 113, Ver 02.01
    data[5] = 0x02;
    data[6] = 0x01;
    data[7] = EMSbus::calculate_crc(data, 7); // apppend CRC
    EMSuart::transmit(data, 8);
}

/**
 * unknown message id, we reply with empty message
 */
void Roomctrl::unknown(uint8_t addr, uint8_t dst, uint8_t type, uint8_t offset) {
    uint8_t data[10];
    data[0] = addr;
    data[1] = dst;
    data[2] = type;
    data[3] = offset;
    data[4] = EMSbus::calculate_crc(data, 4); // apppend CRC
    EMSuart::transmit(data, 5);
}

/**
 * send the room temperature in message 0xAF
 */
void Roomctrl::temperature(uint8_t addr, uint8_t dst) {
    uint8_t data[10];
    uint8_t hc_ = addr - ADDR;
    data[0]     = addr;
    data[1]     = dst;
    data[2]     = 0xAF;
    data[3]     = 0;
    data[4]     = (uint8_t)(remotetemp[hc_] >> 8);
    data[5]     = (uint8_t)(remotetemp[hc_] & 0xFF);
    data[6]     = 0;
    data[7]     = EMSbus::calculate_crc(data, 7); // apppend CRC
    EMSuart::transmit(data, 8);
}
/**
 * send a nack if someone want to write to us.
 */
void Roomctrl::nack_write() {
    uint8_t data[1];
    data[0] = TxService::TX_WRITE_FAIL;
    EMSuart::transmit(data, 1);
}

} // namespace emsesp
