/*
 * emsuart.cpp
 *
 * The low level UART code for ESP8266 to read and write to the EMS bus via uart
 * Paul Derbyshire - https://github.com/proddy/EMS-ESP
 */

#include "emsuart.h"
#include "ems.h"
#include "ets_sys.h"
#include "osapi.h"

#include <Arduino.h>
#include <user_interface.h>

_EMSRxBuf * pEMSRxBuf;
_EMSRxBuf * paEMSRxBuf[EMS_MAXBUFFERS];
uint8_t     emsRxBufIdx = 0;
uint8_t     phantomBrk  = 0; // tells Rx about upcoming phantom break from Tx

os_event_t recvTaskQueue[EMSUART_recvTaskQueueLen]; // our Rx queue

//
// Main interrupt handler
// Important: do not use ICACHE_FLASH_ATTR !
//
static void emsuart_rx_intr_handler(void * para) {
    static uint8_t length;
    static uint8_t uart_buffer[EMS_MAXBUFFERSIZE];

    // is a new buffer? if so init the thing for a new telegram
    if (EMS_Sys_Status.emsRxStatus == EMS_RX_STATUS_IDLE) {
        EMS_Sys_Status.emsRxStatus = EMS_RX_STATUS_BUSY; // status set to busy
        length                     = 0;
    }

    // fill IRQ buffer, by emptying Rx FIFO
    if (U0IS & ((1 << UIFF) | (1 << UITO) | (1 << UIBD))) {
        while ((USS(EMSUART_UART) >> USRXC) & 0xFF) {
            uart_buffer[length++] = USF(EMSUART_UART);
        }

        // clear Rx FIFO full and Rx FIFO timeout interrupts
        U0IC = (1 << UIFF) | (1 << UITO);
    }

    // BREAK detection = End of EMS data block
    if (USIS(EMSUART_UART) & ((1 << UIBD))) {
        ETS_UART_INTR_DISABLE(); // disable all interrupts and clear them

        U0IC = (1 << UIBD); // INT clear the BREAK detect interrupt

        // we've send a BRK with loopback enabled, so we must compensate the phantom break
        if (phantomBrk) {
            length--;       // compensate buffer length
            phantomBrk = 0; // clear flag
        }

        // copy data into transfer buffer
        pEMSRxBuf->writePtr = length;
        os_memcpy((void *)pEMSRxBuf->buffer, (void *)&uart_buffer, length);

        // set the status flag stating BRK has been received and we can start a new package
        EMS_Sys_Status.emsRxStatus = EMS_RX_STATUS_IDLE;

        // call emsuart_recvTask() at next opportunity
        system_os_post(EMSUART_recvTaskPrio, 0, 0);

        // re-enable UART interrupts
        ETS_UART_INTR_ENABLE();
    }
}

/*
 * system task triggered on BRK interrupt
 * incoming received messages are always asynchronous
 * The full buffer is sent to the ems_parseTelegram() function in ems.cpp.
 */
static void ICACHE_FLASH_ATTR emsuart_recvTask(os_event_t * events) {
    _EMSRxBuf * pCurrent = pEMSRxBuf;
    ems_parseTelegram((uint8_t *)pCurrent->buffer, (pCurrent->writePtr) - 1); // transmit EMS buffer, excluding the BRK
    pEMSRxBuf = paEMSRxBuf[++emsRxBufIdx % EMS_MAXBUFFERS];                   // next free EMS Receive buffer
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
    ETS_UART_INTR_ATTACH(NULL, NULL);

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
    // UCTOT = RX TimeOut Threshold (7bit) = want this when no more data after 2 characters. (default is 2)
    // UCFFT = RX FIFO Full Threshold (7 bit) = want this to be 31 for 32 bytes of buffer. (default was 127).
    // see https://www.espressif.com/sites/default/files/documentation/esp8266-technical_reference_en.pdf
    USC1(EMSUART_UART) = 0;                                                                   // reset config first
    USC1(EMSUART_UART) = (EMS_MAX_TELEGRAM_LENGTH << UCFFT) | (0x02 << UCTOT) | (1 << UCTOE); // enable interupts

    // set interrupts for triggers
    USIC(EMSUART_UART) = 0xFFFF; // clear all interupts
    USIE(EMSUART_UART) = 0;      // disable all interrupts

    // enable rx break, fifo full and timeout.
    // not frame error UIFR (because they are too frequent) or overflow UIOF because our buffer is only max 32 bytes
    USIE(EMSUART_UART) = (1 << UIBD) | (1 << UIFF) | (1 << UITO);

    // set up interrupt callbacks for Rx
    system_os_task(emsuart_recvTask, EMSUART_recvTaskPrio, recvTaskQueue, EMSUART_recvTaskQueueLen);

    // disable esp debug which will go to Tx and mess up the line
    system_set_os_print(0); // https://github.com/espruino/Espruino/issues/655

// swap Rx and Tx pins to use GPIO13 (D7) and GPIO15 (D8) respectively
#ifndef NO_UART_SWAP
    system_uart_swap();
#endif

    ETS_UART_INTR_ATTACH(emsuart_rx_intr_handler, NULL);
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
 * Send a BRK signal
 * Which is a 11-bit set of zero's (11 cycles)
 */
void ICACHE_FLASH_ATTR emsuart_tx_brk() {
    uint32_t tmp;

    // must make sure Tx FIFO is empty
    while (((USS(EMSUART_UART) >> USTXC) & 0xFF) != 0)
        ;

    tmp = ((1 << UCRXRST) | (1 << UCTXRST)); // bit mask
    USC0(EMSUART_UART) |= (tmp);             // set bits
    USC0(EMSUART_UART) &= ~(tmp);            // clear bits

    // To create a 11-bit <BRK> we set TXD_BRK bit so the break signal will
    // automatically be sent when the tx fifo is empty
    tmp = (1 << UCBRK);
    USC0(EMSUART_UART) |= (tmp); // set bit

    if (EMS_Sys_Status.emsTxMode <= 1) { // classic mode and ems+ (0, 1)
        delayMicroseconds(EMSUART_TX_BRK_WAIT);
    } else if (EMS_Sys_Status.emsTxMode == 3) {                  // junkers mode
        delayMicroseconds(EMSUART_TX_WAIT_BRK - EMSUART_TX_LAG); // 1144 (11 Bits)
    }

    USC0(EMSUART_UART) &= ~(tmp); // clear bit
}

/*
 * set loopback mode and clear Tx/Rx FIFO
 */
static inline void ICACHE_FLASH_ATTR emsuart_loopback(bool enable) {
    if (enable)
        USC0(EMSUART_UART) |= (1 << UCLBE); // enable loopback
    else
        USC0(EMSUART_UART) &= ~(1 << UCLBE); // disable loopback
}

/*
 * Send to Tx, ending with a <BRK>
 */
void ICACHE_FLASH_ATTR emsuart_tx_buffer(uint8_t * buf, uint8_t len) {
    if (EMS_Sys_Status.emsTxMode == 0) { // classic mode logic
        for (uint8_t i = 0; i < len; i++) {
            USF(EMSUART_UART) = buf[i];
        }
        emsuart_tx_brk();                       // send <BRK>
    } else if (EMS_Sys_Status.emsTxMode == 1) { // With extra tx delay for EMS+
        for (uint8_t i = 0; i < len; i++) {
            USF(EMSUART_UART) = buf[i];
            delayMicroseconds(EMSUART_TX_BRK_WAIT); // https://github.com/proddy/EMS-ESP/issues/23#
        }
        emsuart_tx_brk();                       // send <BRK>
    } else if (EMS_Sys_Status.emsTxMode == 3) { // Junkers logic by @philrich
        for (uint8_t i = 0; i < len; i++) {
            USF(EMSUART_UART) = buf[i];

            // just to be safe wait for tx fifo empty (needed?)
            while (((USS(EMSUART_UART) >> USTXC) & 0xff) != 0)
                ;

            // wait until bits are sent on wire
            delayMicroseconds(EMSUART_TX_WAIT_BYTE - EMSUART_TX_LAG + EMSUART_TX_WAIT_GAP);
        }
        emsuart_tx_brk();                       // send <BRK>
    } else if (EMS_Sys_Status.emsTxMode == 2) { // smart Tx - take two - https://github.com/proddy/EMS-ESP/issues/103 by @susisstrolch

        /*
        * we emit the whole telegram, with Rx interrupt disabled, collecting busmaster response in FIFO.
        * after sending the last char we poll the Rx status until either
        * - size(Rx FIFO) == size(Tx-Telegram)
        * - <BRK> is detected
        * At end of receive we re-enable Rx-INT and send a Tx-BRK in loopback mode.
        */
        ETS_UART_INTR_DISABLE(); // disable rx interrupt

        // clear Rx status register
        USC0(EMSUART_UART) |= (1 << UCRXRST); // reset uart rx fifo
        emsuart_flush_fifos();

        // throw out the telegram...
        for (uint8_t i = 0; i < len;) {
            USF(EMSUART_UART) = buf[i++]; // send each Tx byte
            // wait for echo from busmaster
            while ((((USS(EMSUART_UART) >> USRXC) & 0xFF) < i || (USIS(EMSUART_UART) & (1 << UIBD)))) {
                delayMicroseconds(EMSUART_BIT_TIME); // burn CPU cycles...
            }
        }

        // we got the whole telegram in the Rx buffer
        // on Rx-BRK (bus collision), we simply enable Rx and leave it
        // otherwise we send the final Tx-BRK in the loopback and re=enable Rx-INT.
        // worst case, we'll see an additional Rx-BRK...
        if (!(USIS(EMSUART_UART) & (1 << UIBD))) {
            phantomBrk = 1; // tell Rx to expect a phantom BRK

            // no bus collision - send terminating BRK signal
            emsuart_loopback(true);
            USC0(EMSUART_UART) |= (1 << UCBRK); // set <BRK>

            // wait until BRK detected...
            while (!(USIS(EMSUART_UART) & (1 << UIBD))) {
                delayMicroseconds(EMSUART_BIT_TIME);
            }
            USC0(EMSUART_UART) &= ~(1 << UCBRK); // clear <BRK>

            USIC(EMSUART_UART) = (1 << UIBD); // clear BRK detect IRQ
            emsuart_loopback(false);          // disable loopback mode
        }

        // w/o flushing the Rx FIFO we will get a trailing \0 from our loopback BRK
        ETS_UART_INTR_ENABLE(); // receive anything from FIFO...
    }
}

/*
 * Send the Poll (our own ID) to Tx as a single byte and end with a <BRK>
 */
void ICACHE_FLASH_ATTR emsuart_tx_poll() {
    static uint8_t buf[1];
    if (EMS_Sys_Status.emsReverse) {
        buf[0] = {EMS_ID_ME | 0x80};
    } else {
        buf[0] = {EMS_ID_ME};
    }
    emsuart_tx_buffer(buf, 1);
}
