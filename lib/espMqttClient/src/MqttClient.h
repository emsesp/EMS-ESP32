/*
Copyright (c) 2022 Bert Melis. All rights reserved.

API is based on the original work of Marvin Roger:
https://github.com/marvinroger/async-mqtt-client

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#pragma once

#include <atomic>
#include <utility>

#include "Helpers.h"
#include "Config.h"
#include "TypeDefs.h"
#include "Logging.h"
#include "Outbox.h"
#include "Packets/Packet.h"
#include "Packets/Parser.h"
#include "Transport/Transport.h"

class MqttClient {
 public:
  virtual ~MqttClient();
  bool connected() const;
  bool disconnected() const;
  bool connect();
  bool disconnect(bool force = false);
  template <typename... Args>
  uint16_t subscribe(const char* topic, uint8_t qos, Args&&... args) {
    uint16_t packetId = 0;
    if (_state != State::connected) {
      return packetId;
    } else {
      EMC_SEMAPHORE_TAKE();
      packetId = _getNextPacketId();
      if (!_addPacket(packetId, topic, qos, std::forward<Args>(args) ...)) {
        emc_log_e("Could not create SUBSCRIBE packet");
        packetId = 0;
      }
      EMC_SEMAPHORE_GIVE();
    }
    return packetId;
  }
  template <typename... Args>
  uint16_t unsubscribe(const char* topic, Args&&... args) {
    uint16_t packetId = 0;
    if (_state != State::connected) {
      return packetId;
    } else {
      EMC_SEMAPHORE_TAKE();
      packetId = _getNextPacketId();
      if (!_addPacket(packetId, topic, std::forward<Args>(args) ...)) {
        emc_log_e("Could not create UNSUBSCRIBE packet");
        packetId = 0;
      }
      EMC_SEMAPHORE_GIVE();
    }
    return packetId;
  }
  uint16_t publish(const char* topic, uint8_t qos, bool retain, const uint8_t* payload, size_t length);
  uint16_t publish(const char* topic, uint8_t qos, bool retain, const char* payload);
  uint16_t publish(const char* topic, uint8_t qos, bool retain, espMqttClientTypes::PayloadCallback callback, size_t length);
  void clearQueue(bool deleteSessionData = false);  // Not MQTT compliant and may cause unpredictable results when `deleteSessionData` = true!
  const char* getClientId() const;
  size_t queueSize();  // No const because of mutex
  void loop();

 protected:
  explicit MqttClient(espMqttClientTypes::UseInternalTask useInternalTask, uint8_t priority = 1, uint8_t core = 1);
  espMqttClientTypes::UseInternalTask _useInternalTask;
  espMqttClientInternals::Transport* _transport;

  espMqttClientTypes::OnConnectCallback _onConnectCallback;
  espMqttClientTypes::OnDisconnectCallback _onDisconnectCallback;
  espMqttClientTypes::OnSubscribeCallback _onSubscribeCallback;
  espMqttClientTypes::OnUnsubscribeCallback _onUnsubscribeCallback;
  espMqttClientTypes::OnMessageCallback _onMessageCallback;
  espMqttClientTypes::OnPublishCallback _onPublishCallback;
  espMqttClientTypes::OnErrorCallback _onErrorCallback;
  typedef void(*mqttClientHook)(void*);
  const char* _clientId;
  IPAddress _ip;
  const char* _host;
  uint16_t _port;
  bool _useIp;
  uint32_t _keepAlive;
  bool _cleanSession;
  const char* _username;
  const char* _password;
  const char* _willTopic;
  const uint8_t* _willPayload;
  uint16_t _willPayloadLength;
  uint8_t _willQos;
  bool _willRetain;
  uint32_t _timeout;

  // state is protected to allow state changes by the transport system, defined in child classes
  // eg. to allow AsyncTCP
  enum class State {
    disconnected =       0,
    connectingTcp1 =     1,
    connectingTcp2 =     2,
    connectingMqtt =     3,
    connected =          4,
    disconnectingMqtt1 = 5,
    disconnectingMqtt2 = 6,
    disconnectingTcp1 =  7,
    disconnectingTcp2 =  8
  };
  std::atomic<State> _state;
  inline void _setState(State newState);

 private:
  char _generatedClientId[EMC_CLIENTID_LENGTH];
  uint16_t _packetId;

#if defined(ARDUINO_ARCH_ESP32)
  SemaphoreHandle_t _xSemaphore;
  TaskHandle_t _taskHandle;
  static void _loop(MqttClient* c);
#elif defined(ARDUINO_ARCH_ESP8266) && EMC_ESP8266_MULTITHREADING
  std::atomic<bool> _xSemaphore = false;
#elif defined(__linux__)
  std::mutex mtx;
#endif

  uint8_t _rxBuffer[EMC_RX_BUFFER_SIZE];
  struct OutgoingPacket {
    uint32_t timeSent;
    espMqttClientInternals::Packet packet;
    template <typename... Args>
    OutgoingPacket(uint32_t t, espMqttClientTypes::Error& error, Args&&... args) :  // NOLINT(runtime/references)
      timeSent(t),
      packet(error, std::forward<Args>(args) ...) {}
  };
  espMqttClientInternals::Outbox<OutgoingPacket> _outbox;
  size_t _bytesSent;
  espMqttClientInternals::Parser _parser;
  uint32_t _lastClientActivity;
  uint32_t _lastServerActivity;
  bool _pingSent;
  espMqttClientTypes::DisconnectReason _disconnectReason;

  uint16_t _getNextPacketId();

  template <typename... Args>
  bool _addPacket(Args&&... args) {
    espMqttClientTypes::Error error(espMqttClientTypes::Error::SUCCESS);
    espMqttClientInternals::Outbox<OutgoingPacket>::Iterator it = _outbox.emplace(0, error, std::forward<Args>(args) ...);
    if (it && error == espMqttClientTypes::Error::SUCCESS) {
      return true;
    } else {
      if (it) _outbox.remove(it);
      return false;
    }
  }

  template <typename... Args>
  bool _addPacketFront(Args&&... args) {
    espMqttClientTypes::Error error(espMqttClientTypes::Error::SUCCESS);
    espMqttClientInternals::Outbox<OutgoingPacket>::Iterator it = _outbox.emplaceFront(0, error, std::forward<Args>(args) ...);
    if (it && error == espMqttClientTypes::Error::SUCCESS) {
      return true;
    } else {
      if (it) _outbox.remove(it);
      return false;
    }
  }

  void _checkOutbox();
  int _sendPacket();
  bool _advanceOutbox();
  void _checkIncoming();
  void _checkPing();
  void _checkTimeout();

  void _onConnack();
  void _onPublish();
  void _onPuback();
  void _onPubrec();
  void _onPubrel();
  void _onPubcomp();
  void _onSuback();
  void _onUnsuback();

  void _clearQueue(int clearData);  // 0: keep session,
                                    // 1: keep only PUBLISH qos > 0
                                    // 2: delete all
  void _onError(uint16_t packetId, espMqttClientTypes::Error error);

  #if defined(ARDUINO_ARCH_ESP32)
  #if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
  size_t _highWaterMark;
  #endif
  #endif
};
