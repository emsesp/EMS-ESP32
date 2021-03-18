/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
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

/*
 * ESP32 UART port by @ArwedL and improved by @MichaelDvP. See https://github.com/emsesp/EMS-ESP/issues/380
 */

#if defined(ESP32)

#include "uart/emsuart_esp32.h"

#include "emsesp.h"

namespace emsesp {

RingbufHandle_t buf_handle_   = NULL;
portMUX_TYPE    mux_          = portMUX_INITIALIZER_UNLOCKED;
bool            drop_next_rx_ = true;
uint8_t         tx_mode_      = 0xFF;

/*
* Task to handle the incoming data
*/
void EMSuart::emsuart_recvTask(void * para) {
    while (1) {
        size_t    item_size;
        uint8_t * telegram     = (uint8_t *)xRingbufferReceive(buf_handle_, &item_size, portMAX_DELAY);
        uint8_t   telegramSize = item_size;
        if (telegram) {
            EMSESP::incoming_telegram(telegram, telegramSize);
            vRingbufferReturnItem(buf_handle_, (void *)telegram);
        }
    }
}

/*
 * UART interrupt, on break read the fifo and put the whole telegram to ringbuffer
 */
void IRAM_ATTR EMSuart::emsuart_rx_intr_handler(void * para) {
    portENTER_CRITICAL(&mux_);
    if (EMS_UART.int_st.brk_det) {
        EMS_UART.int_clr.brk_det = 1; // clear flag
        uint8_t rxbuf[EMS_MAXBUFFERSIZE];
        uint8_t length = 0;
        while (EMS_UART.status.rxfifo_cnt) {
            uint8_t rx = EMS_UART.fifo.rw_byte; // read all bytes from fifo
            if (length < EMS_MAXBUFFERSIZE) {
                if (length || rx) { // skip leading zero
                    rxbuf[length++] = rx;
                }
            } else {
                drop_next_rx_ = true; // we have a overflow
            }
        }
        if (rxbuf[length - 1]) { // check if last byte is break
            length++;
        }
        if ((!drop_next_rx_) && ((length == 2) || (length > 4))) {
            int baseType = 0;
            xRingbufferSendFromISR(buf_handle_, rxbuf, length - 1, &baseType);
        }
        drop_next_rx_ = false;
    }
    portEXIT_CRITICAL(&mux_);
}

/*
 * init UART driver
 */
void EMSuart::start(const uint8_t tx_mode, const uint8_t rx_gpio, const uint8_t tx_gpio) {
    if (tx_mode_ != 0xFF) { // uart already initialized
        tx_mode_ = tx_mode;
        restart();
        return;
    }
    tx_mode_ = tx_mode;
    portENTER_CRITICAL(&mux_);
    uart_config_t uart_config = {
        .baud_rate = EMSUART_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    uart_param_config(EMSUART_UART, &uart_config);
    uart_set_pin(EMSUART_UART, tx_gpio, rx_gpio, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    EMS_UART.int_ena.val             = 0;          // disable all intr.
    EMS_UART.int_clr.val             = 0xFFFFFFFF; // clear all intr. flags
    EMS_UART.idle_conf.tx_brk_num    = 10;         // breaklength 10 bit
    drop_next_rx_                    = true;
    // EMS_UART.idle_conf.rx_idle_thrhd = 256;
    // EMS_UART.auto_baud.glitch_filt   = 192;
#if (EMSUART_UART != UART_NUM_2)
    EMS_UART.conf0.rxfifo_rst = 1; // flush fifos, remove for UART2
    EMS_UART.conf0.txfifo_rst = 1;
#endif
    buf_handle_ = xRingbufferCreate(128, RINGBUF_TYPE_NOSPLIT);
    uart_isr_register(EMSUART_UART, emsuart_rx_intr_handler, NULL, ESP_INTR_FLAG_IRAM, NULL);
    xTaskCreate(emsuart_recvTask, "emsuart_recvTask", 2048, NULL, configMAX_PRIORITIES - 3, NULL);
    portEXIT_CRITICAL(&mux_);
    restart();
}

/*
 * Stop, disable interrupt
 */
void EMSuart::stop() {
    portENTER_CRITICAL(&mux_);
    EMS_UART.int_ena.val = 0; // disable all intr.
    portEXIT_CRITICAL(&mux_);
};

/*
 * Restart uart and make mode dependent configs.
 */
void EMSuart::restart() {
    portENTER_CRITICAL(&mux_);
    if (EMS_UART.int_raw.brk_det) {      // we received a break in the meantime
        EMS_UART.int_clr.brk_det = 1;    // clear flag
        drop_next_rx_            = true; // and drop first frame
    }
    EMS_UART.int_ena.brk_det = 1; // activate only break
    EMS_UART.conf0.txd_brk   = (tx_mode_ == EMS_TXMODE_HW) ? 1 : 0;
    portEXIT_CRITICAL(&mux_);

}

/*
 * Sends a 1-byte poll, ending with a <BRK>
 */
void EMSuart::send_poll(const uint8_t data) {
    transmit(&data, 1);
}

/*
 * Send data to Tx line, ending with a <BRK>
 * buf contains the CRC and len is #bytes including the CRC
 * returns code, 1=success
 */
uint16_t EMSuart::transmit(const uint8_t * buf, const uint8_t len) {
    if (len == 0 || len >= EMS_MAXBUFFERSIZE) {
        return EMS_TX_STATUS_ERR;
    }

    if (tx_mode_ == 0) {
        return EMS_TX_STATUS_OK;
    }

    if (tx_mode_ == EMS_TXMODE_HW) { // hardware controlled mode
        for (uint8_t i = 0; i < len; i++) {
            EMS_UART.fifo.rw_byte = buf[i];
        }
        return EMS_TX_STATUS_OK;
    }

    if (tx_mode_ == EMS_TXMODE_EMSPLUS) { // EMS+ with long delay
        for (uint8_t i = 0; i < len; i++) {
            EMS_UART.fifo.rw_byte = buf[i];
            delayMicroseconds(EMSUART_TX_WAIT_PLUS);
        }
        EMS_UART.conf0.txd_inv = 1; // send <brk>
        delayMicroseconds(EMSUART_TX_BRK_PLUS);
        EMS_UART.conf0.txd_inv = 0;
        return EMS_TX_STATUS_OK;
    }

    if (tx_mode_ == EMS_TXMODE_HT3) { // HT3 with 7 bittimes delay
        for (uint8_t i = 0; i < len; i++) {
            EMS_UART.fifo.rw_byte = buf[i];
            delayMicroseconds(EMSUART_TX_WAIT_HT3);
        }
        EMS_UART.conf0.txd_inv = 1; // send <brk>
        delayMicroseconds(EMSUART_TX_BRK_HT3);
        EMS_UART.conf0.txd_inv = 0;
        return EMS_TX_STATUS_OK;
    }

    // mode 1: wait for echo after each byte
    // flush fifos -- not supported in ESP32 uart #2!
    // EMS_UART.conf0.rxfifo_rst = 1;
    // EMS_UART.conf0.txfifo_rst = 1;
    for (uint8_t i = 0; i < len; i++) {
        volatile uint8_t _usrxc = EMS_UART.status.rxfifo_cnt;
        EMS_UART.fifo.rw_byte   = buf[i]; // send each Tx byte
        uint16_t timeoutcnt     = EMSUART_TX_TIMEOUT;
        while ((EMS_UART.status.rxfifo_cnt == _usrxc) && (--timeoutcnt > 0)) {
            delayMicroseconds(EMSUART_TX_BUSY_WAIT); // burn CPU cycles...
        }
    }
    EMS_UART.conf0.txd_inv = 1;
    delayMicroseconds(EMSUART_TX_BRK_EMS);
    EMS_UART.conf0.txd_inv = 0;
    return EMS_TX_STATUS_OK;
}

} // namespace emsesp

#endif