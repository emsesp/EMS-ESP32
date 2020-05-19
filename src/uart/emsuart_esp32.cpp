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

/* ESP32 UART port by Arwed Richert @ArwedL */

#if defined(ESP32)

#include "uart/emsuart_esp32.h"

#include "emsesp.h"

namespace emsesp {

static RingbufHandle_t  buf_handle = NULL;
static QueueHandle_t    uart_queue = NULL;
static volatile uint8_t sending    = 0; // If a telegram is send we ++, in receiving we ignore if != 0 and --
uint8_t                 tx_mode_   = EMS_TXMODE_DEFAULT;

// Main interrupt handler
void EMSuart::emsuart_parseTask(void * param) {
    for (;;) {
        size_t    item_size;
        uint8_t * telegram     = (uint8_t *)xRingbufferReceive(buf_handle, &item_size, pdMS_TO_TICKS(1000));
        uint8_t   telegramSize = item_size; // can't be more than EMS_MAXBUFFERSIZE size (checked in sending task)

        // Did we had a timeout?
        if (telegram) {
            EMSESP::incoming_telegram(telegram, telegramSize);
            vRingbufferReturnItem(buf_handle, (void *)telegram);
        }
    }
}

/*
 * system task triggered on BRK interrupt
 * incoming received messages are always asynchronous
 * The full buffer is sent to the ems_parseTelegram() function in ems.cpp.
 */
void EMSuart::emsuart_recvTask(void * param) {
    uart_event_t event;
    for (;;) {
        if (xQueueReceive(uart_queue, (void *)&event, (portTickType)portMAX_DELAY)) {
            // We are only interested in UART_BREAK event and ignore all others
            if (event.type == UART_BREAK) {
                // If we didn't send it previously it is a newly received telegram
                if (sending == 0) {
                    // Read all bytes which are already in the buffer
                    uint8_t data[EMSUART_RXBUFSIZE];
                    int     length = 0;
                    ESP_ERROR_CHECK(uart_get_buffered_data_len(EMSUART_UART, (size_t *)&length));
                    if (length > EMSUART_RXBUFSIZE) // pedantic check - should never be the case because the receive buffer is of size EMSUART_RXBUFSIZE
                    {
                        length = EMSUART_RXBUFSIZE;
                    }
                    length = uart_read_bytes(EMSUART_UART, data, length, 0); // 0 --> we don't want to wait
                    // we will have runtime performance penalty with impact because of parseTelegram
                    // --> send the data to a 2nd task --> use a ringbuffer to exchange data between both tasks --> no need for pEMSRxBuf or alike
                    // this 2nd tasks processes the message with parseTelegram --> our receiving task is still responsiveness
                    // already do some validty checking here before informing the 2nd task
                    if ((length == 2) || ((length > 4) && (length <= EMS_MAXBUFFERSIZE + 1))) {
                        xRingbufferSend(buf_handle, data, length - 1, 0); // we are not waiting if ringbuffer is full --> just loose telegrams pdMS_TO_TICKS(1000)
                    }
                } else {
                    --sending;
                }
            }
        }
    }
}

/*
 * init UART0 driver
 */
void EMSuart::start(uint8_t tx_mode) {

    tx_mode_ = tx_mode;

    // Configure UART parameters
    // clang-format off
	uart_config_t uart_config = {
		.baud_rate = EMSUART_BAUD,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		//.rx_flow_ctrl_thresh = 0, // UART HW RTS threshold should not be important
	};
    // clang-format on

    ESP_ERROR_CHECK(uart_param_config(EMSUART_UART, &uart_config));

    // Set UART pins(TX: IO16 (UART2 default), RX: IO17 (UART2 default), RTS: IO18, CTS: IO19)
    ESP_ERROR_CHECK(uart_set_pin(EMSUART_UART, EMSUART_TXPIN, EMSUART_RXPIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // Setup UART buffered IO with event queue
    //	const int uart_buffer_size = (1024 * 2);
    //	QueueHandle_t uart_queue;
    // Install UART driver using an event queue here
    //	ESP_ERROR_CHECK(uart_driver_install(UART2, uart_buffer_size,
    //		uart_buffer_size, 10, &uart_queue, 0));
    // do not use user defined interrupt handlers --> we are waiting for break event
    // Decision was against written buffered I/O (seems to be fit better to ESP8266 approach), Queue is needed for receiving the break event, queue size wasn't clear
    // 1 should be enough - also 10 should be fine
    ESP_ERROR_CHECK(uart_driver_install(EMSUART_UART, EMSUART_RXBUFSIZE, 0, 10, &uart_queue, 0));

    // Create a ringbuffer for communication between recvTask and parseTask (as max message size is ~32 bytes) 512 bytes has capacity to hold more than 16 telegrams
    buf_handle = xRingbufferCreate(512, RINGBUF_TYPE_NOSPLIT);

    // start recvTaskQueue stacksize choosen any value 4069 bytes, Priority choosen 2 above normal to be able to fastly react on received signals
    // xTaskCreate parameters taken from https://github.com/espressif/esp-idf/blob/ce2a99dc23533f40369e4ab0d17ffd40f4b0dd72/examples/peripherals/uart/uart_events/main/uart_events_example_main.c
    xTaskCreate(emsuart_recvTask, "EMS_recv", 2048, NULL, 12, NULL);

    // create 2 tasks because assumption is that parseTelegram needs some time (even on ESP32) which would block receiving task
    xTaskCreate(emsuart_parseTask, "EMS_parse", 8000, NULL, 2, NULL);
}

/*
 * Sends a 1-byte poll, ending with a <BRK>
 * It's a bit dirty. there is no special wait logic per tx_mode type, fifo flushes or error checking
 */
void EMSuart::send_poll(uint8_t data) {
    char buf[1];
    buf[0] = data;
    uart_write_bytes_with_break(EMSUART_UART, (const char *)buf, 1, EMSUART_BREAKBITS);
}

/*
 * Send data to Tx line, ending with a <BRK>
 * buf contains the CRC and len is #bytes including the CRC
 * returns code, 0=success, 1=brk error, 2=watchdog timeout
 */
EMSUART_STATUS EMSuart::transmit(uint8_t * buf, uint8_t len) {
    if (len == 0) {
        return EMS_TX_STATUS_OK; // nothing to send
    }

    if (len && buf) {
        ++sending;
        uart_write_bytes_with_break(EMSUART_UART, (const char *)buf, len, EMSUART_BREAKBITS);
    }

    return EMS_TX_STATUS_OK;
}

} // namespace emsesp

#endif
