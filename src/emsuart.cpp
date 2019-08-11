/*
 * emsuart.cpp
 *
 * The low level UART code for ESP8266 to read and write to the EMS bus via uart
 * Paul Derbyshire - https://github.com/proddy/EMS-ESP
 */

#include "emsuart.h"
#include "ems.h"
#include <user_interface.h>

_EMSRxBuf * pEMSRxBuf;
_EMSRxBuf * paEMSRxBuf[EMS_MAXBUFFERS];
uint8_t     emsRxBufIdx  = 0;
uint8_t     phantomBreak = 0;

os_event_t recvTaskQueue[EMSUART_recvTaskQueueLen]; // our Rx queue

//
// Main interrupt handler
// Important: do not use ICACHE_FLASH_ATTR !
//
static void emsuart_rx_intr_handler(void * para) {
    static uint8_t length;
    static uint8_t uart_buffer[EMS_MAXBUFFERSIZE + 2];

    // is a new buffer? if so init the thing for a new telegram
    if (EMS_Sys_Status.emsRxStatus == EMS_RX_STATUS_IDLE) {
        EMS_Sys_Status.emsRxStatus = EMS_RX_STATUS_BUSY; // status set to busy
        length                     = 0;
    }
    GPIO_H(RX_MARK_MASK);
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
    GPIO_L(RX_MARK_MASK);

    // BREAK detection = End of EMS data block
    if (USIS(EMSUART_UART) & ((1 << UIBD))) {
        ETS_UART_INTR_DISABLE();          // disable all interrupts and clear them
        USIC(EMSUART_UART) = (1 << UIBD); // INT clear the BREAK detect interrupt

        pEMSRxBuf->length = (length > EMS_MAXBUFFERSIZE) ? EMS_MAXBUFFERSIZE : length;
        os_memcpy((void *)pEMSRxBuf->buffer, (void *)&uart_buffer, pEMSRxBuf->length); // copy data into transfer buffer, including the BRK 0x00 at the end
        length                     = 0;
        EMS_Sys_Status.emsRxStatus = EMS_RX_STATUS_IDLE; // set the status flag stating BRK has been received and we can start a new package
        ETS_UART_INTR_ENABLE();                          // re-enable UART interrupts

        system_os_post(EMSUART_recvTaskPrio, 0, 0); // call emsuart_recvTask() at next opportunity
        RX_PULSE(EMSUART_BIT_TIME / 2);
    }
}

/*
 * system task triggered on BRK interrupt
 * incoming received messages are always asynchronous
 * The full buffer is sent to the ems_parseTelegram() function in ems.cpp.
 */
static void ICACHE_FLASH_ATTR emsuart_recvTask(os_event_t * events) {
    _EMSRxBuf * pCurrent = pEMSRxBuf;
    pEMSRxBuf            = paEMSRxBuf[++emsRxBufIdx % EMS_MAXBUFFERS]; // next free EMS Receive buffer
    uint8_t length       = pCurrent->length;                           // number of bytes including the BRK at the end
    pCurrent->length     = 0;

    if (phantomBreak) {
        phantomBreak = 0;
        length--; // remove phantom break from Rx buffer
    }

    if (length == 2) {
        RX_PULSE(20);
        // it's a poll or status code, single byte and ok to send on
        ems_parseTelegram((uint8_t *)pCurrent->buffer, 1);
    } else if ((length > 4) && (length <= EMS_MAXBUFFERSIZE + 1)) {
        // ignore double BRK at the end, possibly from the Tx loopback
        // also telegrams with no data value
        RX_PULSE(40);
        ems_parseTelegram((uint8_t *)pCurrent->buffer, length - 1); // transmit EMS buffer, excluding the BRK
    }
}

/*
 * flush everything left over in buffer, this clears both rx and tx FIFOs
 */
static inline void ICACHE_FLASH_ATTR emsuart_flush_fifos() {
    uint32_t tmp = ((1 << UCRXRST) | (1 << UCTXRST)); // bit mask
    USC0(EMSUART_UART) |= (tmp);                      // set bits
    USC0(EMSUART_UART) &= ~(tmp);                     // clear bits
}

/*
 * init UART0 driver
 */
void ICACHE_FLASH_ATTR emsuart_init() {
    ETS_UART_INTR_DISABLE();
    ETS_UART_INTR_ATTACH(nullptr, nullptr);

    // allocate and preset EMS Receive buffers
    for (int i = 0; i < EMS_MAXBUFFERS; i++) {
        _EMSRxBuf * p = (_EMSRxBuf *)malloc(sizeof(_EMSRxBuf));
        paEMSRxBuf[i] = p;
    }
    pEMSRxBuf = paEMSRxBuf[0]; // preset EMS Rx Buffer

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
}

/*
 * stop UART0 driver
 * This is called prior to an OTA upload and also before a save to SPIFFS to prevent conflicts
 */
void ICACHE_FLASH_ATTR emsuart_stop() {
    ETS_UART_INTR_DISABLE();
}

/*
 * re-start UART0 driver
 */
void ICACHE_FLASH_ATTR emsuart_start() {
    ETS_UART_INTR_ENABLE();
}

/*
 * Send to Tx, ending with a <BRK>
 */
_EMS_TX_STATUS ICACHE_FLASH_ATTR emsuart_tx_buffer(uint8_t * buf, uint8_t len) {
    _EMS_TX_STATUS result = EMS_TX_STATUS_OK;
    ems_dumpBuffer("emsuart_tx_buffer: ", buf, len); // validate and transmit the EMS buffer, excluding the BRK
    if (len) {
        LA_PULSE(50);
        /* 
        *
        * based on code from https://github.com/proddy/EMS-ESP/issues/103 by @susisstrolch
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
        * We set EMS_Sys_Status.emsTxStatus to EMS_TX_WTD_TIMEOUT and return
        * 
        * Case 2. is handled via a BRK chk during transmission.
        * We set EMS_Sys_Status.emsTxStatus to EMS_TX_BRK_DETECT and return
        * 
        */

// shorter busy poll...
#define EMSUART_BUSY_WAIT (EMSUART_BIT_TIME / 8)
#define EMS_TX_TO_CHARS (2 + 20)
#define EMS_TX_TO_COUNT ((EMS_TX_TO_CHARS)*10 * 8)
        uint16_t wdc = EMS_TX_TO_COUNT;
        ETS_UART_INTR_DISABLE(); // disable rx interrupt

        // clear Rx status register
        USC0(EMSUART_UART) |= (1 << UCRXRST); // reset uart rx fifo
        emsuart_flush_fifos();

        // throw out the telegram...
        for (uint8_t i = 0; i < len && result == EMS_TX_STATUS_OK;) {
            GPIO_H(TX_MARK_MASK);

            wdc                     = EMS_TX_TO_COUNT;
            volatile uint8_t _usrxc = (USS(EMSUART_UART) >> USRXC) & 0xFF;
            USF(EMSUART_UART)       = buf[i++]; // send each Tx byte
            // wait for echo from busmaster
            GPIO_L(TX_MARK_MASK);
            while (((USS(EMSUART_UART) >> USRXC) & 0xFF) == _usrxc) {
                delayMicroseconds(EMSUART_BUSY_WAIT); // burn CPU cycles...
                if (--wdc == 0) {
                    EMS_Sys_Status.emsTxStatus = result = EMS_TX_WTD_TIMEOUT;
                    break;
                }
                if (USIR(EMSUART_UART) & (1 << UIBD)) {
                    USIC(EMSUART_UART)         = (1 << UIBD); // clear BRK detect IRQ
                    EMS_Sys_Status.emsTxStatus = result = EMS_TX_BRK_DETECT;
                }
            }
        }

        // we got the whole telegram in the Rx buffer
        // on Rx-BRK (bus collision), we simply enable Rx and leave it
        // otherwise we send the final Tx-BRK in the loopback and re=enable Rx-INT.
        // worst case, we'll see an additional Rx-BRK...
        if (result != EMS_TX_STATUS_OK) {
            LA_PULSE(200); // mark Tx error
        } else {
            // neither bus collision nor timeout - send terminating BRK signal
            GPIO_H(TX_MARK_MASK);
            if (!(USIS(EMSUART_UART) & (1 << UIBD))) {
                // no bus collision - send terminating BRK signal
                USC0(EMSUART_UART) |= (1 << UCLBE) | (1 << UCBRK); // enable loopback & set <BRK>

                // wait until BRK detected...
                while (!(USIR(EMSUART_UART) & (1 << UIBD))) {
                    // delayMicroseconds(EMSUART_BUSY_WAIT);
                    delayMicroseconds(EMSUART_BIT_TIME);
                }

                USC0(EMSUART_UART) &= ~((1 << UCBRK) | (1 << UCLBE)); // disable loopback & clear <BRK>
                USIC(EMSUART_UART) = (1 << UIBD);                     // clear BRK detect IRQ
                phantomBreak       = 1;
            }
            GPIO_L(TX_MARK_MASK);
        }
        ETS_UART_INTR_ENABLE(); // receive anything from FIFO...
    }
    return result;
}
