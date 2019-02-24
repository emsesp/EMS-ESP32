/*
 * emsuart.h
 * 
 * Header file for emsuart.cpp
 * 
 * Paul Derbyshire - https://github.com/proddy/EMS-ESP
 */
#pragma once

#include <Arduino.h>

#define EMSUART_UART 0      // UART 0
#define EMSUART_CONFIG 0x1c // 8N1 (8 bits, no stop bits, 1 parity)
#define EMSUART_BAUD 9600   // uart baud rate for the EMS circuit

#define EMS_MAXBUFFERS 4      // 4 buffers for circular filling to avoid collisions
#define EMS_MAXBUFFERSIZE 128 // max size of the buffer. packets are max 32 bytes

// this is how long we drop the Tx signal to create a 11-bit Break of zeros
// At 9600 baud, 11 bits will be 1144 microseconds
// the BRK from Boiler is roughly 1.039ms, so accounting for hardware lag using around 2078 (for half-duplex) - 8 (lag)
#define EMS_TX_BRK_WAIT 2070

#define EMSUART_recvTaskPrio 1
#define EMSUART_recvTaskQueueLen 64

typedef struct {
    int16_t writePtr;
    uint8_t buffer[EMS_MAXBUFFERSIZE];
} _EMSRxBuf;

void ICACHE_FLASH_ATTR emsuart_init();
void ICACHE_FLASH_ATTR emsuart_stop();
void ICACHE_FLASH_ATTR emsuart_tx_buffer(uint8_t * buf, uint8_t len);
void ICACHE_FLASH_ATTR emsaurt_tx_poll();
void ICACHE_FLASH_ATTR emsuart_tx_brk();
