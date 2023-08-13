/*
Copyright (c) 2022 Bert Melis. All rights reserved.

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)

#include "ClientAsync.h"

namespace espMqttClientInternals {

ClientAsync::ClientAsync()
: client()
, availableData(0)
, bufData(nullptr) {
  // empty
}

bool ClientAsync::connect(IPAddress ip, uint16_t port) {
  return client.connect(ip, port);
}

bool ClientAsync::connect(const char* host, uint16_t port) {
  return client.connect(host, port);
}

size_t ClientAsync::write(const uint8_t* buf, size_t size) {
  return client.write(reinterpret_cast<const char*>(buf), size);
}

int ClientAsync::read(uint8_t* buf, size_t size) {
  size_t willRead = std::min(size, availableData);
  memcpy(buf, bufData, std::min(size, availableData));
  if (availableData > size) {
    emc_log_w("Buffer is smaller than available data: %zu - %zu", size, availableData);
  }
  availableData = 0;
  return willRead;
}

void ClientAsync::stop() {
  client.close(false);
}

bool ClientAsync::connected() {
  return client.connected();
}

bool ClientAsync::disconnected() {
  return client.disconnected();
}

}  // namespace espMqttClientInternals

#endif
