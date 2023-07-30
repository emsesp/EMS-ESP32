/*
Copyright (c) 2022 Bert Melis. All rights reserved.

API is based on the original work of Marvin Roger:
https://github.com/marvinroger/async-mqtt-client

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#pragma once

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)

#include "Transport/ClientAsync.h"

#include "MqttClientSetup.h"

class espMqttClientAsync : public MqttClientSetup<espMqttClientAsync> {
 public:
  espMqttClientAsync();
  bool connect();

 protected:
  espMqttClientInternals::ClientAsync _clientAsync;
  static void _setupClient(espMqttClientAsync* c);
  static void _disconnectClient(espMqttClientAsync* c);

  static void onConnectCb(void* a, AsyncClient* c);
  static void onDataCb(void* a, AsyncClient* c, void* data, size_t len);
  static void onDisconnectCb(void* a, AsyncClient* c);
  static void onPollCb(void* a, AsyncClient* c);
};

#endif
