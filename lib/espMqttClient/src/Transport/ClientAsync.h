/*
Copyright (c) 2022 Bert Melis. All rights reserved.

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)

#pragma once

#if defined(ARDUINO_ARCH_ESP32)
  #include "freertos/FreeRTOS.h"
  #include <AsyncTCP.h>
#elif defined(ARDUINO_ARCH_ESP8266)
  #include <ESPAsyncTCP.h>
#endif

#include "Transport.h"
#include "../Config.h"
#include "../Logging.h"

namespace espMqttClientInternals {

class ClientAsync : public Transport {
 public:
  ClientAsync();
  bool connect(IPAddress ip, uint16_t port) override;
  bool connect(const char* host, uint16_t port) override;
  size_t write(const uint8_t* buf, size_t size) override;
  int read(uint8_t* buf, size_t size) override;
  void stop() override;
  bool connected() override;
  bool disconnected() override;

  AsyncClient client;
  size_t availableData;
  uint8_t* bufData;
};

}  // namespace espMqttClientInternals

#endif
