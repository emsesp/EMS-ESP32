/*
Copyright (c) 2022 Bert Melis. All rights reserved.

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)

#include "ClientSecureSync.h"
#include <lwip/sockets.h>  // socket options

namespace espMqttClientInternals {

ClientSecureSync::ClientSecureSync()
: client() {
  // empty
}

bool ClientSecureSync::connect(IPAddress ip, uint16_t port) {
  bool ret = client.connect(ip, port);  // implicit conversion of return code int --> bool
  if (ret) {
    #if defined(ARDUINO_ARCH_ESP8266)
    client.setNoDelay(true);
    #elif defined(ARDUINO_ARCH_ESP32)
    // Set TCP option directly to bypass lack of working setNoDelay for WiFiClientSecure
    int val = true;
    client.setSocketOption(IPPROTO_TCP, TCP_NODELAY, &val, sizeof(int));
    #endif
  }
  return ret;
}

bool ClientSecureSync::connect(const char* host, uint16_t port) {
  bool ret = client.connect(host, port);  // implicit conversion of return code int --> bool
  if (ret) {
    #if defined(ARDUINO_ARCH_ESP8266)
    client.setNoDelay(true);
    #elif defined(ARDUINO_ARCH_ESP32)
    // Set TCP option directly to bypass lack of working setNoDelay for WiFiClientSecure
    int val = true;
    client.setSocketOption(IPPROTO_TCP, TCP_NODELAY, &val, sizeof(int));
    #endif
  }
  return ret;
}

size_t ClientSecureSync::write(const uint8_t* buf, size_t size) {
  return client.write(buf, size);
}

int ClientSecureSync::read(uint8_t* buf, size_t size) {
  return client.read(buf, size);
}

void ClientSecureSync::stop() {
  client.stop();
}

bool ClientSecureSync::connected() {
  return client.connected();
}

bool ClientSecureSync::disconnected() {
  return !client.connected();
}

}  // namespace espMqttClientInternals

#endif
