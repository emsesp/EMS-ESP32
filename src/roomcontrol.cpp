/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2024  Paul Derbyshire
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

// init statics
bool     Roomctrl::switch_off_[HCS] = {false, false, false, false};
uint32_t Roomctrl::rc_time_[HCS]    = {0, 0, 0, 0};
int16_t  Roomctrl::remotetemp_[HCS] = {EMS_VALUE_SHORT_NOTSET, EMS_VALUE_SHORT_NOTSET, EMS_VALUE_SHORT_NOTSET, EMS_VALUE_SHORT_NOTSET};
uint8_t  Roomctrl::remotehum_[HCS]  = {EMS_VALUE_UINT_NOTSET, EMS_VALUE_UINT_NOTSET, EMS_VALUE_UINT_NOTSET, EMS_VALUE_UINT_NOTSET};
uint8_t  Roomctrl::sendcnt[]        = {0, 0, 0, 0};
uint8_t  Roomctrl::type_            = RC20;

/**
 * set the temperature,
 */
void Roomctrl::set_remotetemp(const uint8_t type, const uint8_t hc, const int16_t temp) {
    if (hc >= HCS || (type != RC20 && type != FB10 && type != RC100H && type != SENSOR && type != RC200)) {
        return;
    }
    type_ = type;
    if (remotetemp_[hc] != EMS_VALUE_SHORT_NOTSET && temp == EMS_VALUE_SHORT_NOTSET) {
        switch_off_[hc] = true;
    }
    if (remotetemp_[hc] != temp) {
        rc_time_[hc] = uuid::get_uptime() - SEND_INTERVAL; // send now
        sendcnt[hc]  = 0;
    }
    remotetemp_[hc] = temp;
}

void Roomctrl::set_remotehum(const uint8_t type, const uint8_t hc, const int8_t hum) {
    if (hc >= HCS || (type != RC20 && type != FB10 && type != RC100H && type != SENSOR)) {
        return;
    }
    type_ = type;
    if (remotehum_[hc] != EMS_VALUE_UINT_NOTSET && (uint8_t)hum == EMS_VALUE_UINT_NOTSET) {
        switch_off_[hc] = true;
    }
    if (remotehum_[hc] != hum) {
        rc_time_[hc] = uuid::get_uptime() - SEND_INTERVAL; // send now
        sendcnt[hc]  = 1;
    }
    remotehum_[hc] = hum;
}

uint8_t Roomctrl::get_hc(uint8_t addr) {
    addr &= 0x7F;
    switch (type_) {
    case SENSOR:
        return addr - 0x40;
    case RC200:
    case RC100H:
        return addr - 0x38;
    case FB10:
    case RC20:
    default:
        return addr - 0x18;
    }
}

/**
 * if remote control is active send the temperature every minute
 */
void Roomctrl::send(uint8_t addr) {
    addr &= 0x7F;
    uint8_t hc = get_hc(addr);
    // check address, reply only on addresses 0x18..0x1B or 0x40..0x43
    if (hc >= HCS) {
        return;
    }
    // no reply if the temperature is not set
    if (!switch_off_[hc] && remotetemp_[hc] == EMS_VALUE_SHORT_NOTSET && remotehum_[hc] == EMS_VALUE_UINT_NOTSET) {
        return;
    }

    if (uuid::get_uptime() - rc_time_[hc] > SEND_INTERVAL) { // send every minute
        if (type_ == RC100H) {
            if (sendcnt[hc] == 1) { // second telegram for humidity
                if (switch_off_[hc]) {
                    remotehum_[hc] = EMS_VALUE_UINT_NOTSET;
                }
                rc_time_[hc] = uuid::get_uptime();
                humidity(addr, 0x10, hc);
                sendcnt[hc] = 0;
            } else { // temperature telegram
                if (remotehum_[hc] != EMS_VALUE_UINT_NOTSET) {
                    sendcnt[hc] = 1;
                } else {
                    rc_time_[hc] = uuid::get_uptime();
                }
                temperature(addr, 0x10, hc); // send to master-thermostat
            }
        } else if (type_ == RC200) {
            rc_time_[hc] = uuid::get_uptime();
            temperature(addr, 0x10, hc);
        } else if (type_ == FB10) {
            rc_time_[hc] = uuid::get_uptime();
            temperature(addr, 0x10, hc); // send to master-thermostat (https://github.com/emsesp/EMS-ESP32/issues/336)
        } else {                         // type==RC20 or SENSOR
            rc_time_[hc] = uuid::get_uptime();
            temperature(addr, 0x00, hc); // send to all
        }
        if (remotehum_[hc] == EMS_VALUE_UINT_NOTSET) {
            switch_off_[hc] = false;
        }
    } else {
        // acknowledge every poll, otherwise the master shows error A22-816
        EMSuart::send_poll(addr | EMSbus::ems_mask());
    }
}

/**
 * check if there is a message for the remote room controller
 */
void Roomctrl::check(uint8_t addr, const uint8_t * data, const uint8_t length) {
    if (type_ == SENSOR) {
        return;
    }

    uint8_t hc = get_hc(addr);
    // check address, reply only on addresses 0x18..0x1B
    if (hc >= HCS || length < 5) {
        return;
    }
    // no reply if the temperature is not set
    if (remotetemp_[hc] == EMS_VALUE_SHORT_NOTSET) {
        return;
    }
    // reply to writes with write nack byte
    if ((addr & 0x80) == 0) { // it's a write to us
        ack_write();          // accept writes, don't care.
        return;
    }
    addr &= 0x7F;
    // reads: for now we only reply to version and remote temperature
    // empty message back if temperature not set or unknown message type
    if (data[2] == EMSdevice::EMS_TYPE_VERSION) {
        version(addr, data[0]);
    } else if (length == 6 && remotetemp_[hc] == EMS_VALUE_SHORT_NOTSET) {
        unknown(addr, data[0], data[2], data[3]);
    } else if (length == 8 && remotetemp_[hc] == EMS_VALUE_SHORT_NOTSET) {
        unknown(addr, data[0], data[3], data[5], data[6]);
    } else if (data[2] == 0xAF && data[3] == 0) {
        temperature(addr, data[0], hc);
    } else if (length == 8 && data[2] == 0xFF && data[3] == 0 && data[5] == 0 && data[6] == 0x23) { // Junkers
        temperature(addr, data[0], hc);
    } else if (length == 8 && data[2] == 0xFF && data[3] == 0 && data[5] == 3 && data[6] == 0x2B + hc) { // EMS+ temperature
        temperature(addr, data[0], hc);
    } else if (length == 8 && data[2] == 0xFF && data[3] == 0 && data[5] == 3 && data[6] == 0x7B + hc && remotehum_[hc] != EMS_VALUE_UINT_NOTSET) { // EMS+ humidity
        humidity(addr, data[0], hc);
    } else if (length == 6) { // ems query
        unknown(addr, data[0], data[2], data[3]);
    } else if (length == 8 && data[2] == 0xFF) { // ems+ query
        unknown(addr, data[0], data[3], data[5], data[6]);
    }
}

/**
 * send version info RC20 (Prod. 113, Ver. 02.01) or RC20RF (Prod. 93, Ver. 02.00)
 */
void Roomctrl::version(uint8_t addr, uint8_t dst) {
    uint8_t data[20];
    data[0] = addr | EMSbus::ems_mask();
    data[1] = dst & 0x7F;
    data[2] = 0x02;
    data[3] = 0;
    data[4] = type_; // set RC20 id 113, Ver 02.01 or Junkers FB10 id 109, Ver 16.05, RC100H id 200 ver 40.04
    if (type_ == RC20) {
        data[5] = 2; // version 2.01
        data[6] = 1;
        data[7] = EMSbus::calculate_crc(data, 7); // apppend CRC
        EMSuart::transmit(data, 8);
        return;
    } else if (type_ == FB10) {
        data[5]  = 16; // version 16.05
        data[6]  = 5;
        data[7]  = 0;
        data[8]  = 0;
        data[9]  = 0;
        data[10] = 0;
        data[11] = 0;
        data[12] = 0;
        data[13] = 0;
        data[14] = EMSbus::calculate_crc(data, 14); // apppend CRC
        EMSuart::transmit(data, 15);
        return;
    } else if (type_ == RC200) {
        data[5]  = 32; // version 32.02 see #1611
        data[6]  = 2;
        data[7]  = 0;
        data[8]  = 0xFF;
        data[9]  = 0;
        data[10] = 0;
        data[11] = 0;
        data[12] = 0;
        data[13] = 0;
        data[14] = EMSbus::calculate_crc(data, 14); // apppend CRC
        EMSuart::transmit(data, 15);
        return;
    } else if (type_ == RC100H) {
        data[5] = 40; // version 40.04
        data[6] = 4;
        data[7] = 0;
        data[8] = 0xFF;
        data[9] = EMSbus::calculate_crc(data, 9); // apppend CRC
        EMSuart::transmit(data, 10);
        return;
    }
}

/**
 * unknown message id, we reply with empty message
 */
void Roomctrl::unknown(uint8_t addr, uint8_t dst, uint8_t type, uint8_t offset) {
    uint8_t data[10];
    data[0] = addr | EMSbus::ems_mask();
    data[1] = dst & 0x7F;
    data[2] = type;
    data[3] = offset;
    data[4] = EMSbus::calculate_crc(data, 4); // apppend CRC
    EMSuart::transmit(data, 5);
}

void Roomctrl::unknown(uint8_t addr, uint8_t dst, uint8_t offset, uint8_t typeh, uint8_t typel) {
    uint8_t data[10];
    data[0] = addr | EMSbus::ems_mask();
    data[1] = dst & 0x7F;
    data[2] = 0xFF;
    data[3] = offset;
    data[4] = typeh;
    data[5] = typel;
    data[6] = EMSbus::calculate_crc(data, 6); // apppend CRC
    EMSuart::transmit(data, 7);
}

/**
 * send the room temperature in message 0xAF
 */
void Roomctrl::temperature(uint8_t addr, uint8_t dst, uint8_t hc) {
    uint8_t data[12];
    data[0] = addr | EMSbus::ems_mask();
    data[1] = dst & 0x7F;
    if (type_ == RC20) { // RC20, telegram 0xAF
        data[2] = 0xAF;
        data[3] = 0;
        data[4] = (uint8_t)(remotetemp_[hc] >> 8);
        data[5] = (uint8_t)(remotetemp_[hc] & 0xFF);
        data[6] = 0;
        data[7] = EMSbus::calculate_crc(data, 7); // apppend CRC
        EMSuart::transmit(data, 8);
    } else if (type_ == FB10) { // Junkers FB10, telegram 0x0123
        data[2] = 0xFF;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0x23; //  fixed for all hc
        data[6] = (uint8_t)(remotetemp_[hc] >> 8);
        data[7] = (uint8_t)(remotetemp_[hc] & 0xFF);
        data[8] = EMSbus::calculate_crc(data, 8); // apppend CRC
        EMSuart::transmit(data, 9);
    } else if (type_ == RC200) { // RC200, telegram 42B, ff
        data[2]     = 0xFF;
        data[3]     = 0;
        data[4]     = 3;
        data[5]     = 0x2B + hc;
        data[6]     = (uint8_t)(remotetemp_[hc] >> 8);
        data[7]     = (uint8_t)(remotetemp_[hc] & 0xFF);
        uint16_t t1 = remotetemp_[hc] * 10 + 3;
        data[8]     = (uint8_t)(t1 >> 8);
        data[9]     = (uint8_t)(t1 & 0xFF);
        data[10]    = 1;                               // not sure what this is and if we need it, maybe mode?
        data[11]    = EMSbus::calculate_crc(data, 11); // apppend CRC
        EMSuart::transmit(data, 12);
    } else if (type_ == RC100H) { // RC100H, telegram 42B, ff
        data[2] = 0xFF;
        data[3] = 0;
        data[4] = 3;
        data[5] = 0x2B + hc;
        data[6] = (uint8_t)(remotetemp_[hc] >> 8);
        data[7] = (uint8_t)(remotetemp_[hc] & 0xFF);
        data[8] = EMSbus::calculate_crc(data, 8); // apppend CRC
        EMSuart::transmit(data, 9);
    } else if (type_ == SENSOR) { // wireless sensor, broadcast id 435
        data[2] = 0xFF;
        data[3] = 0;
        data[4] = 3;
        data[5] = 0x35 + hc;
        data[6] = (uint8_t)(remotetemp_[hc] >> 8);
        data[7] = (uint8_t)(remotetemp_[hc] & 0xFF);
        data[8] = EMSbus::calculate_crc(data, 8); // apppend CRC
        EMSuart::transmit(data, 9);
    }
}

// send telegram 0x047B only for RC100H
void Roomctrl::humidity(uint8_t addr, uint8_t dst, uint8_t hc) {
    uint8_t data[11];
    data[0]      = addr | EMSbus::ems_mask();
    data[1]      = dst & 0x7F;
    uint16_t dew = calc_dew(remotetemp_[hc], remotehum_[hc]);
    if (type_ == RC100H) { // RC100H, telegram 47B
        data[2]  = 0xFF;
        data[3]  = 0;
        data[4]  = 3;
        data[5]  = 0x7B + hc;
        data[6]  = dew == EMS_VALUE_SHORT_NOTSET ? EMS_VALUE_INT_NOTSET : (uint8_t)((dew + 5) / 10);
        data[7]  = remotehum_[hc];
        data[8]  = (uint8_t)(dew << 8);
        data[9]  = (uint8_t)(dew & 0xFF);
        data[10] = EMSbus::calculate_crc(data, 10); // apppend CRC
        EMSuart::transmit(data, 11);
    }
}

/**
 * send a nack if someone want to write to us.
 */
void Roomctrl::nack_write() {
    uint8_t data[1];
    data[0] = TxService::TX_WRITE_FAIL;
    EMSuart::transmit(data, 1);
}

/**
 * send a ack if someone want to write to us.
 */
void Roomctrl::ack_write() {
    uint8_t data[1];
    data[0] = TxService::TX_WRITE_SUCCESS;
    EMSuart::transmit(data, 1);
}

int16_t Roomctrl::calc_dew(int16_t temp, uint8_t humi) {
    if (humi == EMS_VALUE_UINT_NOTSET || temp == EMS_VALUE_SHORT_NOTSET) {
        return EMS_VALUE_SHORT_NOTSET;
    }
    const float k2 = 17.62;
    const float k3 = 243.12;
    const float t  = (float)temp / 10;
    const float h  = (float)humi / 100;
    int16_t     dt = (10 * k3 * (((k2 * t) / (k3 + t)) + log(h)) / (((k2 * k3) / (k3 + t)) - log(h)));
    return dt;
}


} // namespace emsesp
