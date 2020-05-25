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

#include <user_interface.h>

#define EMSUART_UART 0      // UART 0
#define EMSUART_CONFIG 0x1C // 8N1 (8 bits, no stop bits, 1 parity)
#define EMSUART_BAUD 9600   // uart baud rate for the EMS circuit

#define EMS_MAXBUFFERS 3     // buffers for circular filling to avoid collisions
#define EMS_MAXBUFFERSIZE 33 // max size of the buffer. EMS packets are max 32 bytes, plus extra 2 for BRK

#define EMSUART_recvTaskPrio 1      // 0, 1 or 2. 0 being the lowest
#define EMSUART_recvTaskQueueLen 10 // number of queued'd Rx triggers

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

    static void ICACHE_FLASH_ATTR           start(uint8_t tx_mode);
    static void ICACHE_FLASH_ATTR           stop();
    static void ICACHE_FLASH_ATTR           restart();
    static void ICACHE_FLASH_ATTR           send_poll(uint8_t data);
    static EMSUART_STATUS ICACHE_FLASH_ATTR transmit(uint8_t * buf, uint8_t len);

    typedef struct {
        uint8_t length;
        uint8_t buffer[EMS_MAXBUFFERSIZE];
    } EMSRxBuf_t;

  private:
    static void ICACHE_RAM_ATTR emsuart_rx_intr_handler(void * para);
    static void ICACHE_FLASH_ATTR emsuart_recvTask(os_event_t * events);
};

} // namespace emsesp

#endif
