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
static RingbufHandle_t buf_handle = NULL;
static uint8_t         rxbuf[UART_FIFO_LEN];
static uint8_t         rxlen;
bool                   drop_first_rx = true;

/*
* Task to handle the incoming data
*/
void EMSuart::emsuart_recvTask(void * param) {
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
static void IRAM_ATTR uart_intr_handle(void * arg) {
    if (EMS_UART.int_st.brk_det) {
        uint8_t rx_fifo_len = EMS_UART.status.rxfifo_cnt;
        for (rxlen = 0; rxlen < rx_fifo_len; rxlen++) {
            rxbuf[rxlen] = EMS_UART.fifo.rw_byte; // read all bytes into buffer
        }
        if (!drop_first_rx && (rxlen == 2) || ((rxlen > 4) && (rxlen <= EMS_MAXBUFFERSIZE))) {
            int baseType = 0;
            xRingbufferSendFromISR(buf_handle, rxbuf, rxlen - 1, &baseType);
        }
        drop_first_rx = false;
        EMS_UART.int_clr.brk_det = 1; // clear flag
        EMS_UART.conf0.txd_brk   = 0; // if it was break from sending, clear bit
    }
}
/*
 * init UART driver
 */
void EMSuart::start(uint8_t tx_mode) {
    uart_config_t uart_config = {
        .baud_rate = EMSUART_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    
    ESP_ERROR_CHECK(uart_param_config(EMSUART_UART, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(EMSUART_UART, EMSUART_TXPIN, EMSUART_RXPIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    //EMS_UART.conf1.rxfifo_full_thrhd = 127;        // enough to hold the incoming telegram, should never reached
    //EMS_UART.idle_conf.tx_brk_num    = 12;         // breaklength 12 bit
    EMS_UART.int_ena.val     = 0;          // disable all intr.
    EMS_UART.int_clr.val     = 0xFFFFFFFF; // clear all intr. flags
    buf_handle               = xRingbufferCreate(128, RINGBUF_TYPE_NOSPLIT);
    ESP_ERROR_CHECK(uart_isr_register(EMSUART_UART, uart_intr_handle, NULL, ESP_INTR_FLAG_IRAM, &uart_handle));
    xTaskCreate(emsuart_recvTask, "emsuart_recvTask", 2048, NULL, 12, NULL);
    drop_first_rx = true;
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
    if (EMS_UART.int_st.brk_det) { 
        EMS_UART.int.clr.brk_det = 1; // clear break if happend 
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
    EMS_UART.idle_conf.tx_brk_num = 12; // breaklength 12 bit
    EMS_UART.conf0.txd_brk        = 1;  // sending ends in a break
}

/*
 * Send data to Tx line, ending with a <BRK>
 * buf contains the CRC and len is #bytes including the CRC
 * returns code, 1=success
 */
EMSUART_STATUS EMSuart::transmit(uint8_t * buf, uint8_t len) {
    if (len > 0) {
        if (EMS_UART.status.txfifo_cnt > 0) { // fifo not empty
            return EMS_TX_WTD_TIMEOUT;
        }
        EMS_UART.conf0.txd_brk = 0; // just to make sure the bit is cleared
        for (uint8_t i = 0; i < len; i++) {
            EMS_UART.fifo.rw_byte = buf[i];
        }
        //uart_tx_chars(EMSUART_UART, (const char *)buf, len);
        EMS_UART.idle_conf.tx_brk_num = 12; // breaklength 12 bit
        EMS_UART.conf0.txd_brk        = 1;  // sending ends in a break
    }
    return EMS_TX_STATUS_OK;
}

} // namespace emsesp

#endif
