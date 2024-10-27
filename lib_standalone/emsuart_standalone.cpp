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

#ifdef EMSESP_STANDALONE

#include "emsuart_standalone.h"

namespace emsesp {

/*
 * init UART0 driver
 */
void EMSuart::start(uint8_t tx_mode, uint8_t rx_gpio, uint8_t tx_gpio) {
}

/*
 * stop UART0 driver
 * This is called prior to an OTA upload and also before a save to SPIFFS to prevent conflicts
 */
void EMSuart::stop() {
}

/*
 * re-start UART0 driver
 */
void EMSuart::restart() {
}

/*
 * Sends a 1-byte poll, ending with a <BRK>
 * It's a bit dirty. there is no special wait logic per tx_mode type, fifo flushes or error checking
 */
void EMSuart::send_poll(uint8_t data) {
}

/*
 * Send data to Tx line, ending with a <BRK>
 * buf contains the CRC and len is #bytes including the CRC
 * returns code, 0=success, 1=brk error, 2=watchdog timeout
 */
uint16_t EMSuart::transmit(uint8_t * buf, uint8_t len) {
    if (len == 0) {
        return EMS_TX_STATUS_OK; // nothing to send
    }

    // Code for when running EMS-ESP standalone without a connected ESP8266 microcontroller
    // For debugging offline
    Serial.print("UART SENDING: ");
    char s[5];
    for (uint8_t i = 0; i < len; i++) {
        Serial.print(hextoa(s, buf[i]));
        Serial.print(" ");
    }
    Serial.println();

    return EMS_TX_STATUS_OK;
}

// like itoa but for hex, and quicker
char * EMSuart::hextoa(char * result, const uint8_t value) {
    char *  p    = result;
    uint8_t nib1 = (value >> 4) & 0x0F;
    uint8_t nib2 = (value >> 0) & 0x0F;
    *p++         = nib1 < 0xA ? '0' + nib1 : 'A' + nib1 - 0xA;
    *p++         = nib2 < 0xA ? '0' + nib2 : 'A' + nib2 - 0xA;
    *p           = '\0'; // null terminate just in case
    return result;
}

} // namespace emsesp

#endif