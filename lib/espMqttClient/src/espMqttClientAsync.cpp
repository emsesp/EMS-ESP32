/*
Copyright (c) 2022 Bert Melis. All rights reserved.

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)

#include "espMqttClientAsync.h"

espMqttClientAsync::espMqttClientAsync()
: MqttClientSetup(espMqttClientTypes::UseInternalTask::NO)
, _clientAsync() {
  _transport = &_clientAsync;
  _clientAsync.client.onConnect(onConnectCb, this);
  _clientAsync.client.onDisconnect(onDisconnectCb, this);
  _clientAsync.client.onData(onDataCb, this);
  _clientAsync.client.onPoll(onPollCb, this);
}

bool espMqttClientAsync::connect() {
  bool ret = MqttClient::connect();
  loop();
  return ret;
}

void espMqttClientAsync::_setupClient(espMqttClientAsync* c) {
  (void)c;
}

void espMqttClientAsync::onConnectCb(void* a, AsyncClient* c) {
  c->setNoDelay(true);
  espMqttClientAsync* client = reinterpret_cast<espMqttClientAsync*>(a);
  client->_state = MqttClient::State::connectingTcp2;
  client->loop();
}

void espMqttClientAsync::onDataCb(void* a, AsyncClient* c, void* data, size_t len) {
  (void)c;
  espMqttClientAsync* client = reinterpret_cast<espMqttClientAsync*>(a);
  client->_clientAsync.bufData = reinterpret_cast<uint8_t*>(data);
  client->_clientAsync.availableData = len;
  client->loop();
}

void espMqttClientAsync::onDisconnectCb(void* a, AsyncClient* c) {
  (void)c;
  espMqttClientAsync* client = reinterpret_cast<espMqttClientAsync*>(a);
  client->_state = MqttClient::State::disconnectingTcp2;
  client->loop();
}

void espMqttClientAsync::onPollCb(void* a, AsyncClient* c) {
  (void)c;
  espMqttClientAsync* client = reinterpret_cast<espMqttClientAsync*>(a);
  client->loop();
}

#endif
