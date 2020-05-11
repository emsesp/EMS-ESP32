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
uint8_t               emsRxBufIdx  = 0;
uint8_t               phantomBreak = 0;
uint8_t               tx_mode_     = EMS_TXMODE_DEFAULT;

//
// Main interrupt handler
// Important: must not use ICACHE_FLASH_ATTR
//
void ICACHE_RAM_ATTR EMSuart::emsuart_rx_intr_handler(void * para) {
    static uint8_t length   = 0;
    static bool    rx_idle_ = true;
    static uint8_t uart_buffer[EMS_MAXBUFFERSIZE + 2];

    // is a new buffer? if so init the thing for a new telegram
    if (rx_idle_) {
        rx_idle_ = false; // status set to busy
        length   = 0;
    }

    // fill IRQ buffer, by emptying Rx FIFO
    if (USIS(EMSUART_UART) & ((1 << UIFF) | (1 << UITO) | (1 << UIBD))) {
        while ((USS(EMSUART_UART) >> USRXC) & 0xFF) {
            uint8_t rx = USF(EMSUART_UART);
            if (length < EMS_MAXBUFFERSIZE)
                uart_buffer[length++] = rx;
        }

        // clear Rx FIFO full and Rx FIFO timeout interrupts
        USIC(EMSUART_UART) = (1 << UIFF) | (1 << UITO);
    }

    // BREAK detection = End of EMS data block
    if (USIS(EMSUART_UART) & ((1 << UIBD))) {
        ETS_UART_INTR_DISABLE();          // disable all interrupts and clear them
        USIC(EMSUART_UART) = (1 << UIBD); // INT clear the BREAK detect interrupt

        pEMSRxBuf->length = (length > EMS_MAXBUFFERSIZE) ? EMS_MAXBUFFERSIZE : length;
        os_memcpy((void *)pEMSRxBuf->buffer, (void *)&uart_buffer, pEMSRxBuf->length); // copy data into transfer buffer, including the BRK 0x00 at the end
        rx_idle_ = true;        // check set the status flag stating BRK has been received and we can start a new package
        ETS_UART_INTR_ENABLE(); // re-enable UART interrupts

        system_os_post(EMSUART_recvTaskPrio, 0, 0); // call emsuart_recvTask() at next opportunity
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

    if (phantomBreak) {
        phantomBreak = 0;
        length--; // remove phantom break from Rx buffer
    }

    // it's a poll or status code, single byte and ok to send on, then quit
    if (length == 2) {
        EMSESP::incoming_telegram((uint8_t *)pCurrent->buffer, 1);
        return;
    }

    // ignore double BRK at the end, possibly from the Tx loopback
    // also telegrams with no data value
    // then transmit EMS buffer, excluding the BRK
    if ((length > 4) && (length <= EMS_MAXBUFFERSIZE + 1)) {
        EMSESP::incoming_telegram((uint8_t *)pCurrent->buffer, length - 1);
    }
}

/*
 * flush everything left over in buffer, this clears both rx and tx FIFOs
 */
void ICACHE_FLASH_ATTR EMSuart::emsuart_flush_fifos() {
    uint32_t tmp = ((1 << UCRXRST) | (1 << UCTXRST)); // bit mask
    USC0(EMSUART_UART) |= (tmp);                      // set bits
    USC0(EMSUART_UART) &= ~(tmp);                     // clear bits
}

/*
 * init UART0 driver
 */
void ICACHE_FLASH_ATTR EMSuart::start(uint8_t tx_mode) {
    tx_mode_ = tx_mode;

    // allocate and preset EMS Receive buffers
    for (int i = 0; i < EMS_MAXBUFFERS; i++) {
        EMSRxBuf_t * p = (EMSRxBuf_t *)malloc(sizeof(EMSRxBuf_t));
        paEMSRxBuf[i]  = p;
    }
    pEMSRxBuf = paEMSRxBuf[0]; // reset EMS Rx Buffer

    ETS_UART_INTR_DISABLE();
    ETS_UART_INTR_ATTACH(nullptr, nullptr);

    // pin settings
    PIN_PULLUP_DIS(PERIPHS_IO_MUX_U0TXD_U);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_U0TXD);
    PIN_PULLUP_DIS(PERIPHS_IO_MUX_U0RXD_U);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_U0RXD);

    // set 9600, 8 bits, no parity check, 1 stop bit
    USD(EMSUART_UART)  = (UART_CLK_FREQ / EMSUART_BAUD);
    USC0(EMSUART_UART) = EMSUART_CONFIG; // 8N1

    emsuart_flush_fifos();

    // conf1 params
    // UCTOE = RX TimeOut enable (default is 1)
    // UCTOT = RX TimeOut Threshold (7 bit) = want this when no more data after 1 characters (default is 2)
    // UCFFT = RX FIFO Full Threshold (7 bit) = want this to be 31 for 32 bytes of buffer (default was 127)
    // see https://www.espressif.com/sites/default/files/documentation/esp8266-technical_reference_en.pdf
    //
    // change: we set UCFFT to 1 to get an immediate indicator about incoming traffic.
    //         Otherwise, we're only noticed by UCTOT or RxBRK!
    USC1(EMSUART_UART) = 0;                                                // reset config first
    USC1(EMSUART_UART) = (0x01 << UCFFT) | (0x01 << UCTOT) | (0 << UCTOE); // enable interupts

    // set interrupts for triggers
    USIC(EMSUART_UART) = 0xFFFF; // clear all interupts
    USIE(EMSUART_UART) = 0;      // disable all interrupts

    // enable rx break, fifo full and timeout.
    // but not frame error UIFR (because they are too frequent) or overflow UIOF because our buffer is only max 32 bytes
    // change: we don't care about Rx Timeout - it may lead to wrong readouts
    USIE(EMSUART_UART) = (1 << UIBD) | (1 << UIFF) | (0 << UITO);

    // set up interrupt callbacks for Rx
    system_os_task(emsuart_recvTask, EMSUART_recvTaskPrio, recvTaskQueue, EMSUART_recvTaskQueueLen);

    // disable esp debug which will go to Tx and mess up the line - see https://github.com/espruino/Espruino/issues/655
    system_set_os_print(0);

    // swap Rx and Tx pins to use GPIO13 (D7) and GPIO15 (D8) respectively
    system_uart_swap();

    ETS_UART_INTR_ATTACH(emsuart_rx_intr_handler, nullptr);
    ETS_UART_INTR_ENABLE();

    // logger_.info(F("UART service for Rx/Tx started"));
}

/*
 * stop UART0 driver
 * This is called prior to an OTA upload and also before a save to the filesystem to prevent conflicts
 */
void ICACHE_FLASH_ATTR EMSuart::stop() {
    ETS_UART_INTR_DISABLE();
}

/*
 * re-start UART0 driver
 */
void ICACHE_FLASH_ATTR EMSuart::restart() {
    ETS_UART_INTR_ENABLE();
}

/*
 * Send a BRK signal
 * Which is a 11-bit set of zero's (11 cycles)
 */
void ICACHE_FLASH_ATTR EMSuart::tx_brk() {
    uint32_t tmp;

    // must make sure Tx FIFO is empty
    while (((USS(EMSUART_UART) >> USTXC) & 0xFF))
        ;

    tmp = ((1 << UCRXRST) | (1 << UCTXRST)); // bit mask
    USC0(EMSUART_UART) |= (tmp);             // set bits
    USC0(EMSUART_UART) &= ~(tmp);            // clear bits

    // To create a 11-bit <BRK> we set TXD_BRK bit so the break signal will
    // automatically be sent when the tx fifo is empty
    tmp = (1 << UCBRK);
    USC0(EMSUART_UART) |= (tmp); // set bit

    if (tx_mode_ == EMS_TX_WTD_TIMEOUT) { // EMS+ mode
        delayMicroseconds(EMSUART_TX_BRK_WAIT);
    } else if (tx_mode_ == EMS_TXMODE_HT3) {                     // junkers mode
        delayMicroseconds(EMSUART_TX_WAIT_BRK - EMSUART_TX_LAG); // 1144 (11 Bits)
    }

    USC0(EMSUART_UART) &= ~(tmp); // clear bit
}

/*
 * Sends a 1-byte poll, ending with a <BRK>
 * It's a bit dirty. there is no special wait logic per tx_mode type, fifo flushes or error checking
 */
void EMSuart::send_poll(uint8_t data) {
    USF(EMSUART_UART) = data;
    delayMicroseconds(EMSUART_TX_BRK_WAIT);
    tx_brk(); // send <BRK>
}

/*
 * Send data to Tx line, ending with a <BRK>
 * buf contains the CRC and len is #bytes including the CRC
 * returns code, 0=success, 1=brk error, 2=watchdog timeout
 */
EMSUART_STATUS ICACHE_FLASH_ATTR EMSuart::transmit(uint8_t * buf, uint8_t len) {
    if (len == 0) {
        return EMS_TX_STATUS_OK; // nothing to send
    }

    // EMS+ https://github.com/proddy/EMS-ESP/issues/23#
    if (tx_mode_ == EMS_TXMODE_EMSPLUS) { // With extra tx delay for EMS+
        for (uint8_t i = 0; i < len; i++) {
            USF(EMSUART_UART) = buf[i];
            delayMicroseconds(EMSUART_TX_BRK_WAIT); // 2070
        }
        tx_brk(); // send <BRK>
        return EMS_TX_STATUS_OK;
    }

    // Junkers logic by @philrich
    if (tx_mode_ == EMS_TXMODE_HT3) {
        for (uint8_t i = 0; i < len; i++) {
            USF(EMSUART_UART) = buf[i];

            // just to be safe wait for tx fifo empty (still needed?)
            while (((USS(EMSUART_UART) >> USTXC) & 0xff))
                ;

            // wait until bits are sent on wire
            delayMicroseconds(EMSUART_TX_WAIT_BYTE - EMSUART_TX_LAG + EMSUART_TX_WAIT_GAP); // 1760
        }
        tx_brk(); // send <BRK>
        return EMS_TX_STATUS_OK;
    }

    /*
     * Logic for tx_mode of 0 (EMS_TXMODE_DEFAULT)
     * based on code from https://github.com/proddy/EMS-ESP/issues/103 by @susisstrolch
     * 
     * Logic:
     * we emit the whole telegram, with Rx interrupt disabled, collecting busmaster response in FIFO.
     * after sending the last char we poll the Rx status until either
     * - size(Rx FIFO) == size(Tx-Telegram)
     * - <BRK> is detected
     * At end of receive we re-enable Rx-INT and send a Tx-BRK in loopback mode.
     * 
     * EMS-Bus error handling
     * 1. Busmaster stops echoing on Tx w/o permission
     * 2. Busmaster cancel telegram by sending a BRK
     * 
     * Case 1. is handled by a watchdog counter which is reset on each
     * Tx attempt. The timeout should be 20x EMSUART_BIT_TIME plus 
     * some smart guess for processing time on targeted EMS device.
     * We set Status to EMS_TX_WTD_TIMEOUT and return
     * 
     * Case 2. is handled via a BRK chk during transmission.
     * We set Status to EMS_TX_BRK_DETECT and return
     * 
     */

    EMSUART_STATUS result = EMS_TX_STATUS_OK;

    // disable rx interrupt
    // clear Rx status register, resetting the Rx FIFO and flush it
    ETS_UART_INTR_DISABLE();
    USC0(EMSUART_UART) |= (1 << UCRXRST);
    emsuart_flush_fifos();

    // send the bytes along the serial line
    for (uint8_t i = 0; i < len; i++) {
        uint16_t         wdc    = EMS_TX_TO_COUNT; // 1760
        volatile uint8_t _usrxc = (USS(EMSUART_UART) >> USRXC) & 0xFF;
        USF(EMSUART_UART)       = buf[i]; // send each Tx byte
        // wait for echo from the busmaster
        while (((USS(EMSUART_UART) >> USRXC) & 0xFF) == _usrxc) {
            delayMicroseconds(EMSUART_BUSY_WAIT); // burn CPU cycles...
            if (--wdc == 0) {
                ETS_UART_INTR_ENABLE();
                return EMS_TX_WTD_TIMEOUT;
            }
            if (USIR(EMSUART_UART) & (1 << UIBD)) {
                USIC(EMSUART_UART) = (1 << UIBD); // clear BRK detect IRQ
                ETS_UART_INTR_ENABLE();
                return EMS_TX_BRK_DETECT;
            }
        }
    }

    // we got the whole telegram in the Rx buffer
    // on Rx-BRK (bus collision), we simply enable Rx and leave it
    // otherwise we send the final Tx-BRK in the loopback and re=enable Rx-INT.
    // worst case, we'll see an additional Rx-BRK...
    if (result == EMS_TX_STATUS_OK) {
        // neither bus collision nor timeout - send terminating BRK signal
        if (!(USIS(EMSUART_UART) & (1 << UIBD))) {
            // no bus collision - send terminating BRK signal
            USC0(EMSUART_UART) |= (1 << UCLBE) | (1 << UCBRK); // enable loopback & set <BRK>

            // wait until BRK detected...
            while (!(USIR(EMSUART_UART) & (1 << UIBD))) {
                delayMicroseconds(EMSUART_BIT_TIME);
            }

            USC0(EMSUART_UART) &= ~((1 << UCBRK) | (1 << UCLBE)); // disable loopback & clear <BRK>
            USIC(EMSUART_UART) = (1 << UIBD);                     // clear BRK detect IRQ
            phantomBreak       = 1;
        }
    }

    ETS_UART_INTR_ENABLE(); // open up the FIFO again to start receiving

    return result; // send the Tx status back
}

} // namespace emsesp

#endif
