/*
 * emsuart.h
 * 
 * Header file for emsuart.cpp
 * 
 * Paul Derbyshire - https://github.com/proddy/EMS-ESP
 */
#pragma once

#include <ems.h>

#define EMSUART_UART 0      // UART 0
#define EMSUART_CONFIG 0x1C // 8N1 (8 bits, no stop bits, 1 parity)
#define EMSUART_BAUD 9600   // uart baud rate for the EMS circuit

#define EMS_MAXBUFFERS 5                                // buffers for circular filling to avoid collisions
#define EMS_MAXBUFFERSIZE (EMS_MAX_TELEGRAM_LENGTH + 2) // max size of the buffer. EMS packets are max 32 bytes, plus extra 2 for BRKs

#define EMSUART_BIT_TIME 104 // bit time @9600 baud

#define EMSUART_TX_BRK_WAIT 2070 // the BRK from Boiler master is roughly 1.039ms, so accounting for hardware lag using around 2078 (for half-duplex) - 8 (lag)
#define EMSUART_TX_WAIT_BYTE (EMSUART_BIT_TIME * 10) // Time to send one Byte (8 Bits, 1 Start Bit, 1 Stop Bit)
#define EMSUART_TX_WAIT_BRK (EMSUART_BIT_TIME * 11)  // Time to send a BRK Signal (11 Bit)
#define EMSUART_TX_WAIT_GAP (EMSUART_BIT_TIME * 7)   // Gap between to Bytes
#define EMSUART_TX_LAG 8

#define EMSUART_BUSY_WAIT (EMSUART_BIT_TIME / 8)
#define EMS_TX_TO_CHARS (2 + 20)
#define EMS_TX_TO_COUNT ((EMS_TX_TO_CHARS)*10 * 8)

#define EMSUART_recvTaskPrio 1
#define EMSUART_recvTaskQueueLen 64

typedef struct {
    uint8_t length;
    uint8_t buffer[EMS_MAXBUFFERSIZE];
} _EMSRxBuf;

void ICACHE_FLASH_ATTR emsuart_init();
void ICACHE_FLASH_ATTR emsuart_stop();
void ICACHE_FLASH_ATTR emsuart_start();
_EMS_TX_STATUS ICACHE_FLASH_ATTR emsuart_tx_buffer(uint8_t * buf, uint8_t len);
