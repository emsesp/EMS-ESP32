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

#include "roomcontrol.h"

namespace emsesp {

// init statics
bool     Roomctrl::switch_off_[HCS]   = {false, false, false, false};
uint32_t Roomctrl::send_time_[HCS]    = {0, 0, 0, 0};
uint32_t Roomctrl::receive_time_[HCS] = {0, 0, 0, 0};
int16_t  Roomctrl::remotetemp_[HCS]   = {EMS_VALUE_INT16_NOTSET, EMS_VALUE_INT16_NOTSET, EMS_VALUE_INT16_NOTSET, EMS_VALUE_INT16_NOTSET};
uint8_t  Roomctrl::remotehum_[HCS]    = {EMS_VALUE_UINT8_NOTSET, EMS_VALUE_UINT8_NOTSET, EMS_VALUE_UINT8_NOTSET, EMS_VALUE_UINT8_NOTSET};
uint8_t  Roomctrl::sendtype_[HCS]     = {SendType::TEMP, SendType::TEMP, SendType::TEMP, SendType::TEMP};
uint8_t  Roomctrl::type_[HCS]         = {RemoteType::NONE, RemoteType::NONE, RemoteType::NONE, RemoteType::NONE};
uint32_t Roomctrl::timeout_           = 0;

/**
 * set the temperature,
 */
void Roomctrl::set_timeout(uint8_t t) {
    timeout_ = t * 3600000; // ms
}
void Roomctrl::set_remotetemp(const uint8_t type, const uint8_t hc, const int16_t temp) {
    if (!type_[hc] && !type) {
        return;
    }
    if (remotetemp_[hc] != EMS_VALUE_INT16_NOTSET && temp == EMS_VALUE_INT16_NOTSET) { // switch remote off
        remotetemp_[hc] = EMS_VALUE_INT16_NOTSET;
        switch_off_[hc] = true;
        send_time_[hc]  = uuid::get_uptime() - SEND_INTERVAL; // send now
        sendtype_[hc]   = SendType::TEMP;
        return;
    }
    if (hc >= HCS || !type) {
        return;
    }
    if (remotetemp_[hc] != temp) {
        send_time_[hc] = uuid::get_uptime() - SEND_INTERVAL; // send now
        sendtype_[hc]  = SendType::TEMP;
    }
    type_[hc]         = type;
    remotetemp_[hc]   = temp;
    receive_time_[hc] = uuid::get_uptime();
}

// set humidity for RC100H emulation
void Roomctrl::set_remotehum(const uint8_t type, const uint8_t hc, const int8_t hum) {
    if (hc >= HCS || type != type_[hc]) {
        return;
    }
    if (remotehum_[hc] != hum) {
        send_time_[hc] = uuid::get_uptime() - SEND_INTERVAL; // send now
        sendtype_[hc]  = SendType::HUMI;
    }
    remotehum_[hc] = hum;
}

uint8_t Roomctrl::get_hc(uint8_t addr) {
    addr &= 0x7F;
    if (addr >= 0x40 && addr <= 0x44 && type_[addr - 0x40] == SENSOR) {
        return addr - 0x40; // SENSOR
    } else if (addr >= 0x38 && addr <= 0x3B
               && (type_[addr - 0x38] == RC100H || type_[addr - 0x38] == RC200 || type_[addr - 0x38] == RC100 || type_[addr - 0x38] == RT800)) {
        return addr - 0x38; // RC100H, RC200
    } else if (addr >= 0x18 && addr <= 0x1B && (type_[addr - 0x18] == RC20 || type_[addr - 0x18] == FB10)) {
        return addr - 0x18; // RC20, FB10
    }
    return 0xFF; // invalid
}

/**
 * if remote control is active send the temperature every 15 seconds
 */
void Roomctrl::send(uint8_t addr) {
    if (addr & 0x80) {
        return;
    }
    uint8_t hc = get_hc(addr);
    // check address, reply only on addresses 0x18..0x1B or 0x40..0x43
    if (hc >= HCS) {
        return;
    }
    // no reply if the temperature is not set
    if (!switch_off_[hc] && remotetemp_[hc] == EMS_VALUE_INT16_NOTSET && remotehum_[hc] == EMS_VALUE_UINT8_NOTSET) {
        return;
    }

    if (!switch_off_[hc] && timeout_ && (uuid::get_uptime() - receive_time_[hc]) > timeout_) {
        remotetemp_[hc] = EMS_VALUE_INT16_NOTSET;
        switch_off_[hc] = true;
        sendtype_[hc]   = SendType::TEMP;
        EMSESP::logger().warning("remotetemp timeout hc%d, stop sending roomtemperature to thermostat", hc);
    }
    if (switch_off_[hc] || (uuid::get_uptime() - send_time_[hc]) > SEND_INTERVAL) { // check interval
        if (type_[hc] == RC100H || type_[hc] == RT800) {
            if (sendtype_[hc] == SendType::HUMI) { // send humidity
                if (switch_off_[hc]) {
                    remotehum_[hc] = EMS_VALUE_UINT8_NOTSET;
                }
                send_time_[hc] = uuid::get_uptime();
                humidity(addr, 0x10, hc);
                sendtype_[hc] = SendType::TEMP;
            } else { // temperature telegram
                if (remotehum_[hc] != EMS_VALUE_UINT8_NOTSET) {
                    sendtype_[hc] = SendType::HUMI;
                } else {
                    send_time_[hc] = uuid::get_uptime();
                }
                temperature(addr, 0x10, hc); // send to master-thermostat
            }
        } else if (type_[hc] == RC200 || type_[hc] == RC100 || type_[hc] == FB10) {
            send_time_[hc] = uuid::get_uptime();
            temperature(addr, 0x10, hc); // send to master-thermostat (https://github.com/emsesp/EMS-ESP32/issues/336)
        } else {                         // type==RC20 or SENSOR
            send_time_[hc] = uuid::get_uptime();
            temperature(addr, 0x00, hc); // send to all
        }
        if (remotehum_[hc] == EMS_VALUE_UINT8_NOTSET && switch_off_[hc]) {
            switch_off_[hc] = false;
            type_[hc]       = RemoteType::NONE;
        }
    } else {
        // acknowledge every poll
        EMSuart::send_poll(addr | EMSbus::ems_mask());
    }
}

/**
 * check if there is a message for the remote room controller
 */
void Roomctrl::check(uint8_t addr, const uint8_t * data, const uint8_t length) {
    uint8_t hc = get_hc(addr);
    if (hc >= HCS || length < 5) {
        return;
    }
    if (type_[hc] == SENSOR) {
        return;
    }
    // no reply if the temperature is not set
    if (remotetemp_[hc] == EMS_VALUE_INT16_NOTSET) {
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
        version(addr, data[0], hc);
    } else if (data[2] == 0xAF && data[3] == 0) {
        temperature(addr, data[0], hc);
    } else if (length == 6) { // all other ems queries
        unknown(addr, data[0], data[2], data[3]);
    } else if (length == 8 && data[2] == 0xFF && data[3] == 0 && data[5] == 0 && data[6] == 0x23) { // Junkers
        temperature(addr, data[0], hc);
    } else if (length == 8 && data[2] == 0xFF && data[3] == 0 && data[5] == 3 && data[6] == 0x2B + hc) { // EMS+ temperature
        temperature(addr, data[0], hc);
    } else if (length == 8 && data[2] == 0xFF && data[3] == 0 && data[5] == 3 && data[6] == 0x7B + hc && remotehum_[hc] != EMS_VALUE_UINT8_NOTSET) { // EMS+ humidity
        humidity(addr, data[0], hc);
    } else if (length == 8 && data[2] == 0xFF) { // ems+ query
        unknown(addr, data[0], data[3], data[5], data[6]);
    } else if (data[2] == 0xF7) { // ems+ query with 3 bytes type src dst 7F offset len=FF FF HIGH LOW
        replyF7(addr, data[0], data[3], data[5], data[6], data[7], hc);
    } else if (length == 8) {
        unknown(addr, data[0], data[3], data[5], data[6]);
    }
}

/**
 * send version info
 */
void Roomctrl::version(uint8_t addr, uint8_t dst, uint8_t hc) {
    uint8_t data[20];
    data[0] = addr | EMSbus::ems_mask();
    data[1] = dst & 0x7F;
    data[2] = 0x02;
    data[3] = 0;
    data[4] = type_[hc]; // set RC20 id 113, Ver 02.01 or Junkers FB10 id 109, Ver 16.05, RC100H id 200 ver 40.04
    if (type_[hc] == RC20) {
        data[5] = 2; // version 2.01
        data[6] = 1;
        data[7] = EMSbus::calculate_crc(data, 7); // append CRC
        EMSuart::transmit(data, 8);
        return;
    } else if (type_[hc] == FB10) {
        data[5]  = 16; // version 16.05
        data[6]  = 5;
        data[7]  = 0;
        data[8]  = 0;
        data[9]  = 0;
        data[10] = 0;
        data[11] = 0;
        data[12] = 0;
        data[13] = 0;
        data[14] = EMSbus::calculate_crc(data, 14); // append CRC
        EMSuart::transmit(data, 15);
        return;
    } else if (type_[hc] == RC200) {
        data[5]  = 32; // version 32.02 see #1611
        data[6]  = 2;
        data[7]  = 0;
        data[8]  = 0xFF;
        data[9]  = 0;
        data[10] = 0;
        data[11] = 0;
        data[12] = 0;
        data[13] = 0;
        data[14] = EMSbus::calculate_crc(data, 14); // append CRC
        EMSuart::transmit(data, 15);
        return;
    } else if (type_[hc] == RC100H) {
        data[5] = 40; // version 40.04
        data[6] = 4;
        data[7] = 0;
        data[8] = 0xFF;
        data[9] = EMSbus::calculate_crc(data, 9); // append CRC
        EMSuart::transmit(data, 10);
        return;
    } else if (type_[hc] == RC100) {
        data[5] = 40; // version 40.03
        data[6] = 3;
        data[7] = 0;
        data[8] = 0xFF;
        data[9] = EMSbus::calculate_crc(data, 9); // append CRC
        EMSuart::transmit(data, 10);
        return;
    } else if (type_[hc] == RT800) {
        data[5] = 21; // version 21.03
        data[6] = 3;
        data[7] = EMSbus::calculate_crc(data, 7); // append CRC
        EMSuart::transmit(data, 8);
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
    data[4] = EMSbus::calculate_crc(data, 4); // append CRC
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
    data[6] = EMSbus::calculate_crc(data, 6); // append CRC
    EMSuart::transmit(data, 7);
}

/**
 * send the room temperature in message 0xAF
 */
void Roomctrl::temperature(uint8_t addr, uint8_t dst, uint8_t hc) {
    uint8_t data[14];
    data[0] = addr | EMSbus::ems_mask();
    data[1] = dst & 0x7F;
    if (type_[hc] == RC20) { // RC20, telegram 0xAF
        data[2] = 0xAF;
        data[3] = 0;
        data[4] = (uint8_t)(remotetemp_[hc] >> 8);
        data[5] = (uint8_t)(remotetemp_[hc] & 0xFF);
        data[6] = 0;
        data[7] = EMSbus::calculate_crc(data, 7); // append CRC
        EMSuart::transmit(data, 8);
    } else if (type_[hc] == FB10) { // Junkers FB10, telegram 0x0123
        data[2] = 0xFF;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0x23; //  fixed for all hc
        data[6] = (uint8_t)(remotetemp_[hc] >> 8);
        data[7] = (uint8_t)(remotetemp_[hc] & 0xFF);
        data[8] = EMSbus::calculate_crc(data, 8); // append CRC
        EMSuart::transmit(data, 9);
    } else if (type_[hc] == RC200) { // RC200, telegram 42B, ff
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
        data[11]    = EMSbus::calculate_crc(data, 11); // append CRC
        EMSuart::transmit(data, 12);
    } else if (type_[hc] == RC100H || type_[hc] == RC100) { // RC100H, telegram 42B, ff
        data[2] = 0xFF;
        data[3] = 0;
        data[4] = 3;
        data[5] = 0x2B + hc;
        data[6] = (uint8_t)(remotetemp_[hc] >> 8);
        data[7] = (uint8_t)(remotetemp_[hc] & 0xFF);
        data[8] = EMSbus::calculate_crc(data, 8); // append CRC
        EMSuart::transmit(data, 9);
    } else if (type_[hc] == SENSOR) { // wireless sensor, broadcast id 435
        data[2] = 0xFF;
        data[3] = 0;
        data[4] = 3;
        data[5] = 0x35 + hc;
        data[6] = (uint8_t)(remotetemp_[hc] >> 8);
        data[7] = (uint8_t)(remotetemp_[hc] & 0xFF);
        data[8] = EMSbus::calculate_crc(data, 8); // append CRC
        EMSuart::transmit(data, 9);
    } else if (type_[hc] == RT800) { // RC200, telegram 42B, ff
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
        data[11]    = 9;                               // not sure what this is and if we need it, maybe mode?
        data[12]    = EMSbus::calculate_crc(data, 12); // append CRC
        EMSuart::transmit(data, 13);
    }
}

// send telegram 0x047B only for RC100H
void Roomctrl::humidity(uint8_t addr, uint8_t dst, uint8_t hc) {
    uint8_t data[11];
    data[0]      = addr | EMSbus::ems_mask();
    data[1]      = dst & 0x7F;
    uint16_t dew = calc_dew(remotetemp_[hc], remotehum_[hc]);
    data[2]      = 0xFF;
    data[3]      = 0;
    data[4]      = 3;
    data[5]      = 0x7B + hc;
    data[6]      = dew == EMS_VALUE_INT16_NOTSET ? EMS_VALUE_INT8_NOTSET : (uint8_t)((dew + 5) / 10);
    data[7]      = remotehum_[hc];
    data[8]      = (uint8_t)(dew << 8);
    data[9]      = (uint8_t)(dew & 0xFF);
    data[10]     = EMSbus::calculate_crc(data, 10); // append CRC
    EMSuart::transmit(data, 11);
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

void Roomctrl::replyF7(uint8_t addr, uint8_t dst, uint8_t offset, uint8_t typehh, uint8_t typeh, uint8_t typel, uint8_t hc) {
    uint8_t data[12];
    data[0] = addr | EMSbus::ems_mask();
    data[1] = dst & 0x7F;
    data[2] = 0xF7;
    data[3] = offset;
    data[4] = typehh;
    data[5] = typeh;
    data[6] = typel;
    if (typehh == 0x02) {
        if (type_[hc] == RC200 || type_[hc] == FB10) {
            data[7] = 0xFF;
            data[8] = 0x01;
        } else {
            data[7] = 0x0F;
            data[8] = 0x00;
        }
    } else {
        data[7] = 0;
        data[8] = 0;
    }
    data[9] = EMSbus::calculate_crc(data, 9); // append CRC
    EMSuart::transmit(data, 10);
}

int16_t Roomctrl::calc_dew(int16_t temp, uint8_t humi) {
    if (humi == EMS_VALUE_UINT8_NOTSET || temp == EMS_VALUE_INT16_NOTSET) {
        return EMS_VALUE_INT16_NOTSET;
    }
    const float k2 = 17.62;
    const float k3 = 243.12;
    const float t  = (float)temp / 10;
    const float h  = (float)humi / 100;
    int16_t     dt = (10 * k3 * (((k2 * t) / (k3 + t)) + log(h)) / (((k2 * k3) / (k3 + t)) - log(h)));
    return dt;
}


} // namespace emsesp
