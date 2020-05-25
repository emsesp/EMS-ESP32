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

#if defined(ESP8266)

#include "uart/emsuart_esp8266.h"

#include "emsesp.h"

namespace emsesp {

os_event_t recvTaskQueue[EMSUART_recvTaskQueueLen]; // our Rx queue

EMSuart::EMSRxBuf_t * pEMSRxBuf;
EMSuart::EMSRxBuf_t * paEMSRxBuf[EMS_MAXBUFFERS];
uint8_t emsRxBufIdx = 0;

//
// Main interrupt handler
// Important: must not use ICACHE_FLASH_ATTR
//
void ICACHE_RAM_ATTR EMSuart::emsuart_rx_intr_handler(void * para) {
    static uint8_t length = 0;
    static uint8_t uart_buffer[128];

    // BREAK detection = End of EMS data block
    if (USIS(EMSUART_UART) & ((1 << UIBD))) {
        uint8_t rxlen = (USS(EMSUART_UART) & 0xFF); // length of buffer
        for (length = 0; length < rxlen; length++) {
            uart_buffer[length] = USF(EMSUART_UART);
        }
        USIE(EMSUART_UART) = 0;              // disable all interrupts and clear them
        USC0(EMSUART_UART) &= ~(1 << UCBRK); // reset <BRK> from sending
        if (length < EMS_MAXBUFFERSIZE) {    // only a valid telegram
            pEMSRxBuf->length = length;
            os_memcpy((void *)pEMSRxBuf->buffer, (void *)&uart_buffer, pEMSRxBuf->length); // copy data into transfer buffer, including the BRK 0x00 at the end
            system_os_post(EMSUART_recvTaskPrio, 0, 0); // call emsuart_recvTask() at next opportunity
        }
        USIC(EMSUART_UART) = (1 << UIBD);    // INT clear the BREAK detect interrupt
        USIE(EMSUART_UART) = (1 << UIBD);     // enable only rx break
    }
}

/*
 * system task triggered on BRK interrupt
 * incoming received messages are always asynchronous
 * The full buffer is sent to EMSESP::incoming_telegram()
 */
void ICACHE_FLASH_ATTR EMSuart::emsuart_recvTask(os_event_t * events) {
    EMSRxBuf_t * pCurrent = pEMSRxBuf;
    pEMSRxBuf             = paEMSRxBuf[++emsRxBufIdx % EMS_MAXBUFFERS]; // next free EMS Receive buffer
    uint8_t length        = pCurrent->length;                           // number of bytes including the BRK at the end
    pCurrent->length      = 0;

    // it's a poll or status code, single byte and ok to send on, then quit
    if (length == 2) {
        EMSESP::incoming_telegram((uint8_t *)pCurrent->buffer, 1);
        return;
    }

    // also telegrams with no data value
    // then transmit EMS buffer, excluding the BRK, lenght is checked by irq
    if (length > 4) {
        EMSESP::incoming_telegram((uint8_t *)pCurrent->buffer, length - 1);
    }
}

/*
 * init UART0 driver
 */
void ICACHE_FLASH_ATTR EMSuart::start(uint8_t tx_mode) {

    // allocate and preset EMS Receive buffers
    for (int i = 0; i < EMS_MAXBUFFERS; i++) {
        EMSRxBuf_t * p = (EMSRxBuf_t *)malloc(sizeof(EMSRxBuf_t));
        paEMSRxBuf[i]  = p;
    }
    pEMSRxBuf = paEMSRxBuf[0]; // reset EMS Rx Buffer

    //ETS_UART_INTR_DISABLE();
    //ETS_UART_INTR_ATTACH(nullptr, nullptr);

    // pin settings
    PIN_PULLUP_DIS(PERIPHS_IO_MUX_U0TXD_U);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_U0TXD);
    PIN_PULLUP_DIS(PERIPHS_IO_MUX_U0RXD_U);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_U0RXD);

    // set 9600, 8 bits, no parity check, 1 stop bit
    USD(EMSUART_UART)  = (UART_CLK_FREQ / EMSUART_BAUD);
    USC0(EMSUART_UART) = EMSUART_CONFIG; // 8N1

    USC0(EMSUART_UART) |= ((1 << UCRXRST) | (1 << UCTXRST));  // set bits
    USC0(EMSUART_UART) &= ~((1 << UCRXRST) | (1 << UCTXRST)); // clear bits

    // UCFFT = RX FIFO Full Threshold (7 bit) = want this to be more than 32)
    USC1(EMSUART_UART) = (0x7F << UCFFT); // rx buffer full
    USIE(EMSUART_UART) = 0;               // disable all interrupts
    USIC(EMSUART_UART) = 0xFFFF;          // clear all interupts
    // set up interrupt callbacks for Rx
    system_os_task(emsuart_recvTask, EMSUART_recvTaskPrio, recvTaskQueue, EMSUART_recvTaskQueueLen);
    // disable esp debug which will go to Tx and mess up the line - see https://github.com/espruino/Espruino/issues/655
    system_set_os_print(0);
    // swap Rx and Tx pins to use GPIO13 (D7) and GPIO15 (D8) respectively
    system_uart_swap();
    ETS_UART_INTR_ATTACH(emsuart_rx_intr_handler, nullptr);
    USIE(EMSUART_UART) = (1 << UIBD); // enable only rx break interrupt
    //ETS_UART_INTR_ENABLE();
}

/*
 * stop UART0 driver
 * This is called prior to an OTA upload and also before a save to the filesystem to prevent conflicts
 */
void ICACHE_FLASH_ATTR EMSuart::stop() {
    USIE(EMSUART_UART) = 0;     // disable interrup
}

/*
 * re-start UART0 driver
 */
void ICACHE_FLASH_ATTR EMSuart::restart() {
    USIE(EMSUART_UART) = (1 << UIBD);     // enable only rx break
}

void EMSuart::send_poll(uint8_t data) {
    USC0(EMSUART_UART) &= ~(1 << UCBRK); // clear <BRK> bit
    USF(EMSUART_UART) = data;
    USC0(EMSUART_UART) |= (1 << UCBRK); // send <BRK> at the end
}

/*
 * Send data to Tx line, ending with a <BRK>
 * buf contains the CRC and len is #bytes including the CRC
 */
EMSUART_STATUS ICACHE_FLASH_ATTR EMSuart::transmit(uint8_t * buf, uint8_t len) {
    if (len) {
        USC0(EMSUART_UART) &= ~(1 << UCBRK); // clear <BRK> bit
        for (uint8_t i = 0; i < len ; i++) {
            USF(EMSUART_UART) = buf[i];
        }
        USC0(EMSUART_UART) |= (1 << UCBRK); // send <BRK> at the end
    }
    return EMS_TX_STATUS_OK;
} 
} // namespace emsesp

#endif
