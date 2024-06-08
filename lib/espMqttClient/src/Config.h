/*
Copyright (c) 2022 Bert Melis. All rights reserved.

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#pragma once

#ifndef EMC_TX_TIMEOUT
#define EMC_TX_TIMEOUT 10000
#endif

#ifndef EMC_RX_BUFFER_SIZE
#define EMC_RX_BUFFER_SIZE 1440
#endif

#ifndef EMC_TX_BUFFER_SIZE
#define EMC_TX_BUFFER_SIZE 1440
#endif

#ifndef EMC_MAX_TOPIC_LENGTH
#define EMC_MAX_TOPIC_LENGTH 128
#endif

#ifndef EMC_PAYLOAD_BUFFER_SIZE
#define EMC_PAYLOAD_BUFFER_SIZE 32
#endif

#ifndef EMC_MIN_FREE_MEMORY
#define EMC_MIN_FREE_MEMORY 16384
#endif

#ifndef EMC_ESP8266_MULTITHREADING
#define EMC_ESP8266_MULTITHREADING 0
#endif

#ifndef EMC_ALLOW_NOT_CONNECTED_PUBLISH
#define EMC_ALLOW_NOT_CONNECTED_PUBLISH 1
#endif

#ifndef EMC_WAIT_FOR_CONNACK
#define EMC_WAIT_FOR_CONNACK 1
#endif

#ifndef EMC_CLIENTID_LENGTH
// esp8266abc123 and esp32abcdef123456
#define EMC_CLIENTID_LENGTH 23 + 1
#endif

#ifndef EMC_TASK_STACK_SIZE
#define EMC_TASK_STACK_SIZE 5120
#endif

#ifndef EMC_MULTIPLE_CALLBACKS
#define EMC_MULTIPLE_CALLBACKS 0
#endif

#ifndef EMC_USE_WATCHDOG
#define EMC_USE_WATCHDOG 0
#endif

#ifndef EMC_USE_MEMPOOL
#define EMC_USE_MEMPOOL 0
#endif

#if EMC_USE_MEMPOOL
  #ifndef EMC_NUM_POOL_ELEMENTS
    #define EMC_NUM_POOL_ELEMENTS 32
  #endif
  #ifndef EMC_SIZE_POOL_ELEMENTS
    #define EMC_SIZE_POOL_ELEMENTS 128
  #endif
#endif
