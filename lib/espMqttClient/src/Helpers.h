/*
Copyright (c) 2022 Bert Melis. All rights reserved.

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#pragma once

#if defined(ARDUINO_ARCH_ESP32)
  #include <Arduino.h>  // millis(), ESP.getFreeHeap();
  #include "freertos/FreeRTOS.h"
  #include "freertos/task.h"
  #include "esp_task_wdt.h"
  #define EMC_SEMAPHORE_TAKE() xSemaphoreTake(_xSemaphore, portMAX_DELAY)
  #define EMC_SEMAPHORE_GIVE() xSemaphoreGive(_xSemaphore)
  #define EMC_GET_FREE_MEMORY() std::max(ESP.getMaxAllocHeap(), ESP.getMaxAllocPsram())
  #define EMC_YIELD() vTaskDelay(1)
  #define EMC_GENERATE_CLIENTID(x) snprintf(x, EMC_CLIENTID_LENGTH, "esp32%06llx", ESP.getEfuseMac());
#elif defined(ARDUINO_ARCH_ESP8266)
  #include <Arduino.h>  // millis(), ESP.getFreeHeap();
  #if EMC_ESP8266_MULTITHREADING
    // This lib doesn't run use multithreading on ESP8266
    // _xSemaphore defined as std::atomic<bool>
    #define EMC_SEMAPHORE_TAKE() while (_xSemaphore) { /*ESP.wdtFeed();*/ } _xSemaphore = true
    #define EMC_SEMAPHORE_GIVE() _xSemaphore = false
  #else
    #define EMC_SEMAPHORE_TAKE()
    #define EMC_SEMAPHORE_GIVE()
  #endif
  #define EMC_GET_FREE_MEMORY() ESP.getMaxFreeBlockSize()
  // no need to yield for ESP8266, the Arduino framework does this internally
  // yielding in async is forbidden (will crash)
  #define EMC_YIELD()
  #define EMC_GENERATE_CLIENTID(x) snprintf(x, EMC_CLIENTID_LENGTH, "esp8266%06x", ESP.getChipId());
#elif defined(__linux__)
  #include <chrono>  // NOLINT [build/c++11]
  #include <thread>  // NOLINT [build/c++11] for yield()
  #define millis() std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count()
  #define EMC_GET_FREE_MEMORY() 1000000000
  #define EMC_YIELD() std::this_thread::yield()
  #define EMC_GENERATE_CLIENTID(x) snprintf(x, EMC_CLIENTID_LENGTH, "Client%04d%04d%04d", rand()%10000, rand()%10000, rand()%10000)
  #include <mutex>  // NOLINT [build/c++11]
    #define EMC_SEMAPHORE_TAKE() mtx.lock();
    #define EMC_SEMAPHORE_GIVE() mtx.unlock();
#else
  #error Target platform not supported
#endif
