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

#define EMS_MAXBUFFERSIZE 33                  // max size of the buffer. EMS packets are max 32 bytes, plus BRK
#define EMSUART_UART UART_NUM_2               // UART 0 --> Changed to 2 for ESP32 // To do: Adapt
#define EMS_UART UART2                        // for intr setting
//#define EMSUART_RXPIN 17                    // To do: Adapt seems to be IO17 for ESP32 UART2 RX pin
//#define EMSUART_TXPIN 16                    // To do: Adapt seems to be IO16 for ESP32 UART2 TX pin
#define EMSUART_RXPIN 23                      // Wemos D1 ESP32 UART2 RX pin for compatibility
#define EMSUART_TXPIN 5                       // Wemos D1 ESP32 UART2 TX pin for compatibility
#define EMSUART_BAUD 9600                     // uart baud rate for the EMS circuit

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
    static void           stop();
    static void           restart();
    static EMSUART_STATUS transmit(uint8_t * buf, uint8_t len);

  private:
    static void emsuart_recvTask(void * param);
};

} // namespace emsesp

#endif
