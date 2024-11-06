/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2024  emsesp.org - proddy, MichaelDvP
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

#ifndef EMSESP_STANDALONE

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "soc/uart_reg.h"
#include "uart/emsuart_esp32.h"
#include "emsesp.h"

namespace emsesp {
uint8_t EMSuart::last_tx_src_ = 0;

static QueueHandle_t uart_queue;
uint8_t              tx_mode_     = 0xFF;
uint32_t             inverse_mask = 0;

/*
* receive task, wait for break and call incoming_telegram
*/
void EMSuart::uart_event_task(void * pvParameters) {
    uart_event_t event;
    uint8_t      telegram[EMS_MAXBUFFERSIZE];
    uint8_t      length = 0;

    while (1) {
        //Waiting for UART event.
        if (xQueueReceive(uart_queue, (void *)&event, portMAX_DELAY)) {
            if (event.type == UART_DATA) {
                length += event.size;
            } else if (event.type == UART_BREAK) {
                if (length == 2 || (length >= 6 && length <= EMS_MAXBUFFERSIZE)) {
                    uart_read_bytes(EMSUART_NUM, telegram, length, portMAX_DELAY);
                    EMSESP::incoming_telegram(telegram, (uint8_t)(length - 1));
                } else { // flush buffer up to break
                    uint8_t buf[length];
                    uart_read_bytes(EMSUART_NUM, buf, length, portMAX_DELAY);
                }
                length = 0;
            } else if (event.type == UART_BUFFER_FULL) {
                uart_flush_input(EMSUART_NUM);
                length = 0;
            }
        }
    }
    vTaskDelete(NULL);
}

/*
 * init UART driver
 */
void EMSuart::start(const uint8_t tx_mode, const uint8_t rx_gpio, const uint8_t tx_gpio) {
    if (tx_mode_ == 0xFF) {
        uart_config_t uart_config = {
            .baud_rate           = EMSUART_BAUD,
            .data_bits           = UART_DATA_8_BITS,
            .parity              = UART_PARITY_DISABLE,
            .stop_bits           = UART_STOP_BITS_1,
            .flow_ctrl           = UART_HW_FLOWCTRL_DISABLE,
            .rx_flow_ctrl_thresh = 0, // not used - https://docs.espressif.com/projects/esp-idf/en/v3.3.6/api-reference/peripherals/uart.html
            .source_clk          = UART_SCLK_APB,
        };
#if defined(EMSUART_RX_INVERT)
        inverse_mask |= UART_SIGNAL_RXD_INV;
#endif
#if defined(EMSUART_TX_INVERT)
        inverse_mask |= UART_SIGNAL_TXD_INV;
#endif
        uart_param_config(EMSUART_NUM, &uart_config);
        uart_set_pin(EMSUART_NUM, tx_gpio, rx_gpio, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
        uart_set_line_inverse(EMSUART_NUM, inverse_mask);
        uart_driver_install(EMSUART_NUM, 129, 0, (EMS_MAXBUFFERSIZE + 1) * 2, &uart_queue, 0); // buffer must be > fifo
        uart_set_rx_full_threshold(EMSUART_NUM, 1);
        uart_set_rx_timeout(EMSUART_NUM, 0); // disable

        // note esp32s3 crashes with 2k stacksize, stack overflow here sometimes wipes settingsfiles.
        xTaskCreate(uart_event_task, "uart_event_task", 2560, NULL, configMAX_PRIORITIES - 1, NULL);
    }
    tx_mode_ = tx_mode;
    uart_enable_intr_mask(EMSUART_NUM, UART_BRK_DET_INT_ENA | UART_RXFIFO_FULL_INT_ENA);
}

/*
 * Stop, disable interrupt
 */
void EMSuart::stop() {
    if (tx_mode_ != 0xFF) { // only call after driver initialisation
        uart_disable_intr_mask(EMSUART_NUM, UART_BRK_DET_INT_ENA | UART_RXFIFO_FULL_INT_ENA);
        // TODO should we xTaskSuspend() the event task here?
    }
};

/*
 * generate <BRK> by inverting tx
 */
void IRAM_ATTR EMSuart::uart_gen_break(uint32_t length_us) {
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&mux);
    uart_set_line_inverse(EMSUART_NUM, UART_SIGNAL_TXD_INV ^ inverse_mask);
    delayMicroseconds(length_us);
    uart_set_line_inverse(EMSUART_NUM, inverse_mask);
    portEXIT_CRITICAL(&mux);
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

    last_tx_src_ = len < 4 ? 0 : buf[0];

    if (tx_mode_ == EMS_TXMODE_HW) { // hardware controlled mode
        uart_write_bytes_with_break(EMSUART_NUM, buf, len, 10);
        return EMS_TX_STATUS_OK;
    }

    if (tx_mode_ == EMS_TXMODE_EMSPLUS) { // EMS+ with long delay
        for (uint8_t i = 0; i < len; i++) {
            uart_write_bytes(EMSUART_NUM, &buf[i], 1);
            delayMicroseconds(EMSUART_TX_WAIT_PLUS);
        }
        uart_gen_break(EMSUART_TX_BRK_PLUS);
        return EMS_TX_STATUS_OK;
    }

    if (tx_mode_ == EMS_TXMODE_HT3) { // HT3 with 7 bittimes delay
        for (uint8_t i = 0; i < len; i++) {
            uart_write_bytes(EMSUART_NUM, &buf[i], 1);
            delayMicroseconds(EMSUART_TX_WAIT_HT3);
        }
        uart_gen_break(EMSUART_TX_BRK_HT3);
        return EMS_TX_STATUS_OK;
    }

    // mode 1: wait for echo after each byte
    for (uint8_t i = 0; i < len; i++) {
        size_t rx0, rx1;
        uart_get_buffered_data_len(EMSUART_NUM, &rx0);
        uart_write_bytes(EMSUART_NUM, &buf[i], 1);
        uint16_t timeoutcnt = EMSUART_TX_TIMEOUT;
        do {
            delayMicroseconds(EMSUART_TX_BUSY_WAIT); // burn CPU cycles...
            uart_get_buffered_data_len(EMSUART_NUM, &rx1);
        } while ((rx1 == rx0) && (--timeoutcnt));
    }
    uart_gen_break(EMSUART_TX_BRK_EMS);
    return EMS_TX_STATUS_OK;
}

} // namespace emsesp

#endif
