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

/*
 * ESP32 UART port by @ArwedL and improved by @MichaelDvP. See https://github.com/proddy/EMS-ESP/issues/380
 */

#if defined(ESP32)

#include "uart/emsuart_esp32.h"

#include "emsesp.h"

namespace emsesp {

static intr_handle_t   uart_handle;
static RingbufHandle_t buf_handle    = NULL;
static bool            drop_first_rx = true;
static uint8_t         tx_mode_      = 0xFF;

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

    if (EMS_UART.int_st.brk_det) {
        EMS_UART.int_clr.brk_det = 1; // clear flag
        EMS_UART.conf0.txd_brk   = 0; // if it was break from sending, clear bit
        length = 0;
        while (EMS_UART.status.rxfifo_cnt) {
            uint8_t rx = EMS_UART.fifo.rw_byte; // read all bytes into buffer
            if (length < EMS_MAXBUFFERSIZE) {
                rxbuf[length++] = rx;
            }
        }
        if ((!drop_first_rx) && ((length == 2) || ((length > 4)))) {
            int baseType = 0;
            xRingbufferSendFromISR(buf_handle, rxbuf, length - 1, &baseType);
        }
        drop_first_rx = false;
    }
}
/*
 * init UART driver
 */
void EMSuart::start(uint8_t tx_mode) {
    if (tx_mode_ != 0xFF) { // uart already initialized
        restart();
        return;
    }
    tx_mode_ =  tx_mode;
    uart_config_t uart_config = {
        .baud_rate = EMSUART_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    
    ESP_ERROR_CHECK(uart_param_config(EMSUART_UART, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(EMSUART_UART, EMSUART_TXPIN, EMSUART_RXPIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    EMS_UART.int_ena.val          = 0; // disable all intr.
    EMS_UART.int_clr.val          = 0xFFFFFFFF; // clear all intr. flags
    EMS_UART.idle_conf.tx_brk_num = 11; // breaklength 11 bit
    drop_first_rx                 = true;
    buf_handle                    = xRingbufferCreate(128, RINGBUF_TYPE_NOSPLIT);
    ESP_ERROR_CHECK(uart_isr_register(EMSUART_UART, emsuart_rx_intr_handler, NULL, ESP_INTR_FLAG_IRAM, &uart_handle));
    xTaskCreate(emsuart_recvTask, "emsuart_recvTask", 2048, NULL, 12, NULL);
    EMS_UART.int_ena.brk_det = 1;          // activate only break   
}

/*
 * Stop, disables interrupt
 */
void EMSuart::stop() {
    EMS_UART.int_ena.val = 0; // disable all intr.
};

/*
 * Restart Interrupt
 */
void EMSuart::restart() {
    if (EMS_UART.int_raw.brk_det) { 
        EMS_UART.int_clr.brk_det = 1; // clear flag
        drop_first_rx = true; // and drop first frame 
    }
    EMS_UART.int_ena.brk_det = 1;          // activate only break
};

/*
 * Sends a 1-byte poll, ending with a <BRK>
 */
void EMSuart::send_poll(uint8_t data) {
    EMS_UART.conf0.txd_brk        = 0; // just to make sure the bit is cleared
    EMS_UART.fifo.rw_byte         = data;
    //EMS_UART.idle_conf.tx_brk_num = 11; // breaklength 11 bit
    EMS_UART.conf0.txd_brk        = 1;  // sending ends in a break
}

/*
 * Send data to Tx line, ending with a <BRK>
 * buf contains the CRC and len is #bytes including the CRC
 * returns code, 1=success
 */
EMSUART_STATUS EMSuart::transmit(uint8_t * buf, uint8_t len) {
    if (len > 0) {
        EMS_UART.conf0.txd_brk = 0; // just to make sure the bit is cleared
        for (uint8_t i = 0; i < len; i++) {
            EMS_UART.fifo.rw_byte = buf[i];
        }
        //uart_tx_chars(EMSUART_UART, (const char *)buf, len);
        //EMS_UART.idle_conf.tx_brk_num = 11; // breaklength 11 bit
        EMS_UART.conf0.txd_brk        = 1;  // sending ends in a break
    }
    return EMS_TX_STATUS_OK;
}

} // namespace emsesp

#endif
