/*
Copyright (c) 2022 Bert Melis. All rights reserved.

API is based on the original work of Marvin Roger:
https://github.com/marvinroger/async-mqtt-client

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#pragma once

#include "MqttClient.h"

template <typename T>
class MqttClientSetup : public MqttClient {
 public:
  T& setKeepAlive(uint16_t keepAlive) {
    _keepAlive = keepAlive * 1000;  // s to ms conversion, will also do 16 to 32 bit conversion
    return static_cast<T&>(*this);
  }

  T& setClientId(const char* clientId) {
    _clientId = clientId;
    return static_cast<T&>(*this);
  }

  T& setCleanSession(bool cleanSession) {
    _cleanSession = cleanSession;
    return static_cast<T&>(*this);
  }

  T& setCredentials(const char* username, const char* password) {
    _username = username;
    _password = password;
    return static_cast<T&>(*this);
  }

  T& setWill(const char* topic, uint8_t qos, bool retain, const uint8_t* payload, size_t length) {
    _willTopic = topic;
    _willQos = qos;
    _willRetain = retain;
    _willPayload = payload;
    if (!_willPayload) {
      _willPayloadLength = 0;
    } else {
      _willPayloadLength = length;
    }
    return static_cast<T&>(*this);
  }

  T& setWill(const char* topic, uint8_t qos, bool retain, const char* payload) {
    return setWill(topic, qos, retain, reinterpret_cast<const uint8_t*>(payload), strlen(payload));
  }

  T& setServer(IPAddress ip, uint16_t port) {
    _ip = ip;
    _port = port;
    _useIp = true;
    return static_cast<T&>(*this);
  }

  T& setServer(const char* host, uint16_t port) {
    _host = host;
    _port = port;
    _useIp = false;
    return static_cast<T&>(*this);
  }

  T& setTimeout(uint16_t timeout) {
    _timeout = timeout * 1000;  // s to ms conversion, will also do 16 to 32 bit conversion
    return static_cast<T&>(*this);
  }

  T& onConnect(espMqttClientTypes::OnConnectCallback callback) {
    _onConnectCallback = callback;
    return static_cast<T&>(*this);
  }

  T& onDisconnect(espMqttClientTypes::OnDisconnectCallback callback) {
    _onDisconnectCallback = callback;
    return static_cast<T&>(*this);
  }

  T& onSubscribe(espMqttClientTypes::OnSubscribeCallback callback) {
    _onSubscribeCallback = callback;
    return static_cast<T&>(*this);
  }

  T& onUnsubscribe(espMqttClientTypes::OnUnsubscribeCallback callback) {
    _onUnsubscribeCallback = callback;
    return static_cast<T&>(*this);
  }

  T& onMessage(espMqttClientTypes::OnMessageCallback callback) {
    _onMessageCallback = callback;
    return static_cast<T&>(*this);
  }

  T& onPublish(espMqttClientTypes::OnPublishCallback callback) {
    _onPublishCallback = callback;
    return static_cast<T&>(*this);
  }

  /*
  T& onError(espMqttClientTypes::OnErrorCallback callback) {
    _onErrorCallback = callback;
    return static_cast<T&>(*this);
  }
  */

 protected:
  explicit MqttClientSetup(espMqttClientTypes::UseInternalTask useInternalTask, uint8_t priority = 1, uint8_t core = 1)
  : MqttClient(useInternalTask, priority, core) {}
};
