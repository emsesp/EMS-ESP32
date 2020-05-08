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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include "freertos/queue.h"
#include <driver/uart.h>

#define EMS_MAXBUFFERSIZE 34 // max size of the buffer. EMS packets are max 32 bytes, plus extra 2 for BRKs

#define EMSUART_UART UART_NUM_2 // UART 0 --> Changed to 2 for ESP32 // To do: Adapt
#define EMSUART_RXPIN 17        // To do: Adapt seems to be IO17 for ESP32 UART2 RX pin
#define EMSUART_TXPIN 16        // To do: Adapt seems to be IO16 for ESP32 UART2 TX pin
//#define EMSUART_CONFIG 0x1C // 8N1 (8 bits, no parity, 1 stopbit)
#define EMSUART_BAUD 9600 // uart baud rate for the EMS circuit
#define EMSUART_RXBUFSIZE (2 * UART_FIFO_LEN)
#define EMSUART_BREAKBITS 11 // 11 bits break signal

#define EMS_TXMODE_DEFAULT 1
#define EMS_TXMODE_EMSPLUS 2
#define EMS_TXMODE_HT3 3

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
    static void           send_poll(uint8_t data);
    static EMSUART_STATUS transmit(uint8_t * buf, uint8_t len);

    static void stop(){};    // not used with ESP32
    static void restart(){}; // not used with ESP32

  private:
    static void emsuart_recvTask(void * param);
    static void emsuart_parseTask(void * param);
};

} // namespace emsesp

#endif
