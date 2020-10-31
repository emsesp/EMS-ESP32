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

#if defined(ESP8266)

#include "uart/emsuart_esp8266.h"

#include "emsesp.h"

namespace emsesp {

os_event_t            recvTaskQueue[EMSUART_recvTaskQueueLen]; // our Rx queue
EMSuart::EMSRxBuf_t * pEMSRxBuf;
EMSuart::EMSRxBuf_t * paEMSRxBuf[EMS_MAXBUFFERS];
uint8_t               emsRxBufIdx  = 0;
uint8_t               tx_mode_     = 0xFF;
bool                  drop_next_rx = true;
uint8_t               emsTxBuf[EMS_MAXBUFFERSIZE];
uint8_t               emsTxBufIdx;
uint8_t               emsTxBufLen;
uint32_t              emsTxWait;
bool                  EMSuart::sending_ = false;

//
// Main interrupt handler
// Important: must not use ICACHE_FLASH_ATTR
//
void ICACHE_RAM_ATTR EMSuart::emsuart_rx_intr_handler(void * para) {
    static uint8_t length = 0;
    static uint8_t uart_buffer[EMS_MAXBUFFERSIZE + 2];

    if (USIS(EMSUART_UART) & ((1 << UIBD))) { // BREAK detection = End of EMS data block
        USC0(EMSUART_UART) &= ~(1 << UCBRK);  // reset tx-brk
        if (sending_) {                       // irq tx_mode is interrupted by <brk>, should never happen
            drop_next_rx = true;              // we have trash in buffer
        }
        USIC(EMSUART_UART) = (1 << UIBD); // INT clear the BREAK detect interrupt
        length             = 0;
        while ((USS(EMSUART_UART) >> USRXC) & 0x0FF) { // read fifo into buffer
            uint8_t rx = USF(EMSUART_UART);
            if (length < EMS_MAXBUFFERSIZE) {
                if (length || rx) { // skip a leading zero
                    uart_buffer[length++] = rx;
                }
            } else {
                drop_next_rx = true;
            }
        }
        if (!drop_next_rx) {
            if (uart_buffer[length - 1]) { // check if last byte is break
                length++;
            }
            pEMSRxBuf->length = length;
            os_memcpy((void *)pEMSRxBuf->buffer, (void *)&uart_buffer, pEMSRxBuf->length); // copy data into transfer buffer, including the BRK 0x00 at the end
            system_os_post(EMSUART_recvTaskPrio, 0, 0);                                    // call emsuart_recvTask() at next opportunity
        }
        drop_next_rx = false;
        sending_     = false;
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

    // Ignore telegrams with no data value, then transmit EMS buffer, excluding the BRK
    if (length > 4 || length == 2) {
        EMSESP::incoming_telegram((uint8_t *)pCurrent->buffer, length - 1);
    }
}

// ISR to Fire when Timer is triggered
void ICACHE_RAM_ATTR EMSuart::emsuart_tx_timer_intr_handler() {
    if (!sending_) {
        return;
    }
    emsTxBufIdx++;
    if (emsTxBufIdx < emsTxBufLen) {
        USF(EMSUART_UART) = emsTxBuf[emsTxBufIdx];
        timer1_write(emsTxWait);
    } else if (emsTxBufIdx == emsTxBufLen) {
        USC0(EMSUART_UART) |= (1 << UCBRK); // set <BRK>
        timer1_write(EMSUART_TX_BRK_TIMER);
    } else {
        USC0(EMSUART_UART) &= ~(1 << UCBRK); // reset <BRK>
        sending_ = false;
    }
}

/*
 * flush everything left over in buffer, this clears both rx and tx FIFOs
 */
void ICACHE_FLASH_ATTR EMSuart::emsuart_flush_fifos() {
    USC0(EMSUART_UART) |= ((1 << UCRXRST) | (1 << UCTXRST));  // set bits
    USC0(EMSUART_UART) &= ~((1 << UCRXRST) | (1 << UCTXRST)); // clear bits
}

/*
 * init UART0 driver
 */
void ICACHE_FLASH_ATTR EMSuart::start(const uint8_t tx_mode, const uint8_t rx_gpio, const uint8_t tx_gpio) {
    if (tx_mode_ != 0xFF) { // it's a restart no need to configure uart
        tx_mode_ = tx_mode;
        restart();
        return;
    }
    tx_mode_ = tx_mode;
    // allocate and preset EMS Receive buffers
    for (int i = 0; i < EMS_MAXBUFFERS; i++) {
        EMSRxBuf_t * p = (EMSRxBuf_t *)malloc(sizeof(EMSRxBuf_t));
        p->length      = 0;
        paEMSRxBuf[i]  = p;
    }
    pEMSRxBuf = paEMSRxBuf[0]; // reset EMS Rx Buffer

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
    // change: don't care, we do not use these interrupts
    USC1(EMSUART_UART) = 0; // reset config
    // USC1(EMSUART_UART) = (0x7F << UCFFT) | (0x01 << UCTOT) | (1 << UCTOE); // enable interupts

    // set interrupts for triggers
    USIC(EMSUART_UART) = 0xFFFF; // clear all interupts
    USIE(EMSUART_UART) = 0;      // disable all interrupts

    // set up interrupt callbacks for Rx
    system_os_task(emsuart_recvTask, EMSUART_recvTaskPrio, recvTaskQueue, EMSUART_recvTaskQueueLen);

    // disable esp debug which will go to Tx and mess up the line - see https://github.com/espruino/Espruino/issues/655
    system_set_os_print(0);

    if (tx_gpio == 1 && rx_gpio == 3) {
        system_uart_de_swap();
    } else if (tx_gpio == 15 && rx_gpio == 13) {
        system_uart_swap(); // swap Rx and Tx pins to use GPIO13 (D7) and GPIO15 (D8) respectively
    }

    ETS_UART_INTR_ATTACH(emsuart_rx_intr_handler, nullptr);
    drop_next_rx = true;

    // for sending with large delay in EMS+ mode we use a timer interrupt
    timer1_attachInterrupt(emsuart_tx_timer_intr_handler);

    restart();
}

/*
 * stop UART0 driver
 * This is called prior to an OTA upload and also before a save to the filesystem to prevent conflicts
 */
void ICACHE_FLASH_ATTR EMSuart::stop() {
    USIE(EMSUART_UART) = 0;
    USC0(EMSUART_UART) &= ~(1 << UCBRK); // clear BRK bit
    timer1_disable();
    sending_ = false;
}

/*
 * re-start UART0 driver
 */
void ICACHE_FLASH_ATTR EMSuart::restart() {
    if (USIR(EMSUART_UART) & ((1 << UIBD))) {
        USIC(EMSUART_UART) = (1 << UIBD); // INT clear the <brk> detect interrupt
        drop_next_rx       = true;
    }
    if (tx_mode_ > 100) {
        emsTxWait = 5 * EMSUART_TX_BIT_TIME * (tx_mode_ - 90);
    } else {
        emsTxWait = 5 * EMSUART_TX_BIT_TIME * (tx_mode_ + 10); // bittimes wait to next bytes
    }
    emsTxBufIdx = 0;
    emsTxBufLen = 0;
    timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
    USIE(EMSUART_UART) = (1 << UIBD); // enable <brk> interrupt
}

/*
 * Sends a 1-byte poll, ending with a <BRK>
 */
void ICACHE_FLASH_ATTR EMSuart::send_poll(uint8_t data) {
    transmit(&data, 1);
}

/*
 * Send data to Tx line, ending with a <BRK>
 * buf contains the CRC and len is #bytes including the CRC
 * returns code, 0=success, 1=brk error, 2=watchdog timeout
 */
uint16_t ICACHE_FLASH_ATTR EMSuart::transmit(uint8_t * buf, uint8_t len) {
    if (len == 0 || len >= EMS_MAXBUFFERSIZE) {
        return EMS_TX_STATUS_ERR; // nothing or to much to send
    }

    // timer controlled modes with extra delay
    if (tx_mode_ >= 5) {
        sending_ = true;
        for (uint8_t i = 0; i < len; i++) {
            emsTxBuf[i] = buf[i];
        }
        USF(EMSUART_UART) = buf[0]; // send first byte
        emsTxBufIdx       = 0;
        emsTxBufLen       = len;
        if (tx_mode_ > 100 && len > 1) {
            timer1_write(EMSUART_TX_WAIT_REPLY); // large delay after first byte
        } else {
            timer1_write(emsTxWait);
        }
        return EMS_TX_STATUS_OK;
    }

    // new code from Michael. See https://github.com/proddy/EMS-ESP/issues/380
    if (tx_mode_ == EMS_TXMODE_NEW) { // tx_mode 4
        for (uint8_t i = 0; i < len; i++) {
            USF(EMSUART_UART) = buf[i];
        }
        USC0(EMSUART_UART) |= (1 << UCBRK); // send <BRK> at the end, clear by interrupt
        return EMS_TX_STATUS_OK;
    }

    // EMS+ https://github.com/proddy/EMS-ESP/issues/23#
    if (tx_mode_ == EMS_TXMODE_EMSPLUS) { // tx_mode 2, With extra tx delay for EMS+
        for (uint8_t i = 0; i < len; i++) {
            USF(EMSUART_UART) = buf[i];
            delayMicroseconds(EMSUART_TX_WAIT_PLUS); // 2070
        }
        USC0(EMSUART_UART) |= (1 << UCBRK); // set break
        delayMicroseconds(EMSUART_TX_BRK_PLUS);
        USC0(EMSUART_UART) &= ~(1 << UCBRK);
        return EMS_TX_STATUS_OK;
    }

    // Junkers logic by @philrich, tx_mode 3
    if (tx_mode_ == EMS_TXMODE_HT3) {
        for (uint8_t i = 0; i < len; i++) {
            USF(EMSUART_UART) = buf[i];
            // just to be safe wait for tx fifo empty (still needed?)
            while (((USS(EMSUART_UART) >> USTXC) & 0xff)) {
            }
            // wait until bits are sent on wire
            delayMicroseconds(EMSUART_TX_WAIT_HT3);
        }
        USC0(EMSUART_UART) |= (1 << UCBRK); // set break bit
        delayMicroseconds(EMSUART_TX_BRK_HT3);
        USC0(EMSUART_UART) &= ~(1 << UCBRK);
        return EMS_TX_STATUS_OK;
    }

    /*
     * Logic for tx_mode of 1
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
     * Tx attempt. The timeout should be 20x EMSUART_TX_BIT_TIME plus
     * some smart guess for processing time on targeted EMS device.
     * We set Status to EMS_TX_WTD_TIMEOUT and return
     * 
     * Case 2. is handled via a BRK chk during transmission.
     * We set Status to EMS_TX_BRK_DETECT and return
     * 
     */

    // clear Rx status register, resetting the Rx FIFO and flush it
    emsuart_flush_fifos();

    // send the bytes along the serial line
    for (uint8_t i = 0; i < len; i++) {
        volatile uint8_t _usrxc     = (USS(EMSUART_UART) >> USRXC) & 0xFF;
        uint16_t         timeoutcnt = EMSUART_TX_TIMEOUT;
        USF(EMSUART_UART)           = buf[i]; // send each Tx byte
        // wait for echo
        while ((((USS(EMSUART_UART) >> USRXC) & 0xFF) == _usrxc) && (--timeoutcnt > 0)) {
            delayMicroseconds(EMSUART_TX_BUSY_WAIT); // burn CPU cycles...
        }
    }
    USC0(EMSUART_UART) |= (1 << UCBRK); // snd break
    delayMicroseconds(EMSUART_TX_BRK_EMS);
    USC0(EMSUART_UART) &= ~(1 << UCBRK);
    return EMS_TX_STATUS_OK; // send the Tx ok status back
}

} // namespace emsesp

#endif