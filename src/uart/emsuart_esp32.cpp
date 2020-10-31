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

/*
 * ESP32 UART port by @ArwedL and improved by @MichaelDvP. See https://github.com/proddy/EMS-ESP/issues/380
 */

#if defined(ESP32)

#include "uart/emsuart_esp32.h"

#include "emsesp.h"

namespace emsesp {

static RingbufHandle_t buf_handle   = NULL;
portMUX_TYPE           mux          = portMUX_INITIALIZER_UNLOCKED;
static hw_timer_t *    timer        = NULL;
bool                   drop_next_rx = true;
uint8_t                tx_mode_     = 0xFF;
uint8_t                emsTxBuf[EMS_MAXBUFFERSIZE];
uint8_t                emsTxBufIdx = 0;
uint8_t                emsTxBufLen = 0;
uint32_t               emsTxWait;

/*
* Task to handle the incoming data
*/
void EMSuart::emsuart_recvTask(void * para) {
    while (1) {
        size_t    item_size;
        uint8_t * telegram     = (uint8_t *)xRingbufferReceive(buf_handle, &item_size, portMAX_DELAY);
        uint8_t   telegramSize = item_size;
        if (telegram) {
            EMSESP::incoming_telegram(telegram, telegramSize);
            vRingbufferReturnItem(buf_handle, (void *)telegram);
        }
    }
}

/*
 * UART interrupt, on break read the fifo and put the whole telegram to ringbuffer
 */
void IRAM_ATTR EMSuart::emsuart_rx_intr_handler(void * para) {
    static uint8_t rxbuf[EMS_MAXBUFFERSIZE];
    static uint8_t length;
    portENTER_CRITICAL(&mux);
    if (EMS_UART.int_st.brk_det) {
        EMS_UART.int_clr.brk_det = 1; // clear flag
        length                   = 0;
        while (EMS_UART.status.rxfifo_cnt) {
            uint8_t rx = EMS_UART.fifo.rw_byte; // read all bytes from fifo
            if (length < EMS_MAXBUFFERSIZE) {
                rxbuf[length++] = rx;
            } else {
                drop_next_rx = true; // we have a overflow
            }
        }
        if ((!drop_next_rx) && ((length == 2) || (length > 4))) {
            int baseType = 0;
            xRingbufferSendFromISR(buf_handle, rxbuf, length - 1, &baseType);
        }
        drop_next_rx = false;
    }
    portEXIT_CRITICAL(&mux);
}

void IRAM_ATTR EMSuart::emsuart_tx_timer_intr_handler() {
    if (emsTxBufLen == 0) {
        return;
    }
    portENTER_CRITICAL(&mux);
    if (emsTxBufIdx < emsTxBufLen) {
        EMS_UART.fifo.rw_byte = emsTxBuf[emsTxBufIdx];
        if (emsTxBufIdx == 1) {
            timerAlarmWrite(timer, emsTxWait, true);
        }
    } else if (emsTxBufIdx == emsTxBufLen) {
        EMS_UART.conf0.txd_inv = 1;
        timerAlarmWrite(timer, EMSUART_TX_BRK_TIMER, true);
    } else if (emsTxBufIdx == emsTxBufLen + 1) {
        EMS_UART.conf0.txd_inv = 0;
        emsTxBufLen            = 0;
        timerAlarmDisable(timer);
    }
    emsTxBufIdx++;
    portEXIT_CRITICAL(&mux);
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
    EMS_UART.idle_conf.rx_idle_thrhd = 256;
    drop_next_rx                     = true;
    buf_handle                       = xRingbufferCreate(128, RINGBUF_TYPE_NOSPLIT);
    uart_isr_register(EMSUART_UART, emsuart_rx_intr_handler, NULL, ESP_INTR_FLAG_IRAM, NULL);
    xTaskCreate(emsuart_recvTask, "emsuart_recvTask", 2048, NULL, configMAX_PRIORITIES - 1, NULL);

    timer = timerBegin(0, 80, true);                                   // timer prescale to 1 us, countup
    timerAttachInterrupt(timer, &emsuart_tx_timer_intr_handler, true); // Timer with edge interrupt
    restart();
}

/*
 * Stop, disable interrupt
 */
void EMSuart::stop() {
    EMS_UART.int_ena.val   = 0; // disable all intr.
    EMS_UART.conf0.txd_inv = 0; // stop break
    if (emsTxBufLen > 0) {
        timerAlarmDisable(timer);
    }
};

/*
 * Restart uart and make mode dependent configs.
 */
void EMSuart::restart() {
    if (EMS_UART.int_raw.brk_det) {      // we received a break in the meantime
        EMS_UART.int_clr.brk_det = 1;    // clear flag
        drop_next_rx             = true; // and drop first frame
    }
    EMS_UART.int_ena.brk_det = 1; // activate only break
    emsTxBufIdx              = 0;
    emsTxBufLen              = 0;
    if (tx_mode_ > 100) {
        emsTxWait = EMSUART_TX_BIT_TIME * (tx_mode_ - 90);
    } else {
        emsTxWait = EMSUART_TX_BIT_TIME * (tx_mode_ + 10);
    }
    if (tx_mode_ == EMS_TXMODE_NEW) {
        EMS_UART.conf0.txd_brk = 1;
    } else {
        EMS_UART.conf0.txd_brk = 0;
    }
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

    if (tx_mode_ > 5) { // timer controlled modes
        for (uint8_t i = 0; i < len; i++) {
            emsTxBuf[i] = buf[i];
        }
        emsTxBufIdx = 0;
        emsTxBufLen = len;
        if (tx_mode_ > 100 && len > 1) {
            timerAlarmWrite(timer, EMSUART_TX_WAIT_REPLY, true);
        } else {
            timerAlarmWrite(timer, emsTxWait, true); // start with autoreload
        }
        timerAlarmEnable(timer);
        return EMS_TX_STATUS_OK;
    }

    if (tx_mode_ == EMS_TXMODE_NEW) { // hardware controlled modes
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