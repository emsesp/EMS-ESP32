/*
Copyright (c) 2022 Bert Melis. All rights reserved.

API is based on the original work of Marvin Roger:
https://github.com/marvinroger/async-mqtt-client

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#pragma once

#if EMC_MULTIPLE_CALLBACKS
#include <list>
#include <utility>
#endif

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

  T& onConnect(espMqttClientTypes::OnConnectCallback callback, uint32_t id = 0) {
    #if EMC_MULTIPLE_CALLBACKS
    _onConnectCallbacks.emplace_back(callback, id);
    #else
    (void) id;
    _onConnectCallback = callback;
    #endif
    return static_cast<T&>(*this);
  }

  T& onDisconnect(espMqttClientTypes::OnDisconnectCallback callback, uint32_t id = 0) {
    #if EMC_MULTIPLE_CALLBACKS
    _onDisconnectCallbacks.emplace_back(callback, id);
    #else
    (void) id;
    _onDisconnectCallback = callback;
    #endif
    return static_cast<T&>(*this);
  }

  T& onSubscribe(espMqttClientTypes::OnSubscribeCallback callback, uint32_t id = 0) {
    #if EMC_MULTIPLE_CALLBACKS
    _onSubscribeCallbacks.emplace_back(callback, id);
    #else
    (void) id;
    _onSubscribeCallback = callback;
    #endif
    return static_cast<T&>(*this);
  }

  T& onUnsubscribe(espMqttClientTypes::OnUnsubscribeCallback callback, uint32_t id = 0) {
    #if EMC_MULTIPLE_CALLBACKS
    _onUnsubscribeCallbacks.emplace_back(callback, id);
    #else
    (void) id;
    _onUnsubscribeCallback = callback;
    #endif
    return static_cast<T&>(*this);
  }

  T& onMessage(espMqttClientTypes::OnMessageCallback callback, uint32_t id = 0) {
    #if EMC_MULTIPLE_CALLBACKS
    _onMessageCallbacks.emplace_back(callback, id);
    #else
    (void) id;
    _onMessageCallback = callback;
    #endif
    return static_cast<T&>(*this);
  }

  T& onPublish(espMqttClientTypes::OnPublishCallback callback, uint32_t id = 0) {
    #if EMC_MULTIPLE_CALLBACKS
    _onPublishCallbacks.emplace_back(callback, id);
    #else
    (void) id;
    _onPublishCallback = callback;
    #endif
    return static_cast<T&>(*this);
  }

  #if EMC_MULTIPLE_CALLBACKS
  T& removeOnConnect(uint32_t id) {
    for (auto it = _onConnectCallbacks.begin(); it != _onConnectCallbacks.end(); ++it) {
      if (it->second == id) {
        _onConnectCallbacks.erase(it);
        break;
      }
    }
    return static_cast<T&>(*this);
  }

  T& removeOnDisconnect(uint32_t id) {
    for (auto it = _onDisconnectCallbacks.begin(); it != _onDisconnectCallbacks.end(); ++it) {
      if (it->second == id) {
        _onDisconnectCallbacks.erase(it);
        break;
      }
    }
    return static_cast<T&>(*this);
  }

  T& removeOnSubscribe(uint32_t id) {
    for (auto it = _onSubscribeCallbacks.begin(); it != _onSubscribeCallbacks.end(); ++it) {
      if (it->second == id) {
        _onSubscribeCallbacks.erase(it);
        break;
      }
    }
    return static_cast<T&>(*this);
  }

  T& removeOnUnsubscribe(uint32_t id) {
    for (auto it = _onUnsubscribeCallbacks.begin(); it != _onUnsubscribeCallbacks.end(); ++it) {
      if (it->second == id) {
        _onUnsubscribeCallbacks.erase(it);
        break;
      }
    }
    return static_cast<T&>(*this);
  }

  T& removeOnMessage(uint32_t id) {
    for (auto it = _onMessageCallbacks.begin(); it != _onMessageCallbacks.end(); ++it) {
      if (it->second == id) {
        _onMessageCallbacks.erase(it);
        break;
      }
    }
    return static_cast<T&>(*this);
  }

  T& removeOnPublish(uint32_t id) {
    for (auto it = _onPublishCallbacks.begin(); it != _onPublishCallbacks.end(); ++it) {
      if (it->second == id) {
        _onPublishCallbacks.erase(it);
        break;
      }
    }
    return static_cast<T&>(*this);
  }
  #endif

  /*
  T& onError(espMqttClientTypes::OnErrorCallback callback) {
    _onErrorCallback = callback;
    return static_cast<T&>(*this);
  }
  */

 protected:
  explicit MqttClientSetup(espMqttClientTypes::UseInternalTask useInternalTask, uint8_t priority = 1, uint8_t core = 1)
  : MqttClient(useInternalTask, priority, core) {
    #if EMC_MULTIPLE_CALLBACKS
    _onConnectCallback = [this](bool sessionPresent) {
      for (auto callback : _onConnectCallbacks) if (callback.first) callback.first(sessionPresent);
    };
    _onDisconnectCallback = [this](espMqttClientTypes::DisconnectReason reason) {
      for (auto callback : _onDisconnectCallbacks) if (callback.first) callback.first(reason);
    };
    _onSubscribeCallback = [this](uint16_t packetId, const espMqttClientTypes::SubscribeReturncode* returncodes, size_t len) {
      for (auto callback : _onSubscribeCallbacks) if (callback.first) callback.first(packetId, returncodes, len);
    };
    _onUnsubscribeCallback = [this](int16_t packetId) {
      for (auto callback : _onUnsubscribeCallbacks) if (callback.first) callback.first(packetId);
    };
    _onMessageCallback = [this](const espMqttClientTypes::MessageProperties& properties, const char* topic, const uint8_t* payload, size_t len, size_t index, size_t total) {
      for (auto callback : _onMessageCallbacks) if (callback.first) callback.first(properties, topic, payload, len, index, total);
    };
    _onPublishCallback = [this](uint16_t packetId) {
      for (auto callback : _onPublishCallbacks) if (callback.first) callback.first(packetId);
    };
    #else
    // empty
    #endif
  }

  #if EMC_MULTIPLE_CALLBACKS
  std::list<std::pair<espMqttClientTypes::OnConnectCallback, uint32_t>> _onConnectCallbacks;
  std::list<std::pair<espMqttClientTypes::OnDisconnectCallback, uint32_t>> _onDisconnectCallbacks;
  std::list<std::pair<espMqttClientTypes::OnSubscribeCallback, uint32_t>> _onSubscribeCallbacks;
  std::list<std::pair<espMqttClientTypes::OnUnsubscribeCallback, uint32_t>> _onUnsubscribeCallbacks;
  std::list<std::pair<espMqttClientTypes::OnMessageCallback, uint32_t>> _onMessageCallbacks;
  std::list<std::pair<espMqttClientTypes::OnPublishCallback, uint32_t>> _onPublishCallbacks;
  #endif
};
