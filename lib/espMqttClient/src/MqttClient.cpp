/*
Copyright (c) 2022 Bert Melis. All rights reserved.

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#include "MqttClient.h"

using espMqttClientInternals::Packet;
using espMqttClientInternals::PacketType;
using espMqttClientTypes::DisconnectReason;
using espMqttClientTypes::Error;

MqttClient::MqttClient(espMqttClientTypes::UseInternalTask useInternalTask, uint8_t priority, uint8_t core)
: _useInternalTask(useInternalTask)
, _transport(nullptr)
, _onConnectCallback(nullptr)
, _onDisconnectCallback(nullptr)
, _onSubscribeCallback(nullptr)
, _onUnsubscribeCallback(nullptr)
, _onMessageCallback(nullptr)
, _onPublishCallback(nullptr)
, _onErrorCallback(nullptr)
, _clientId(nullptr)
, _ip()
, _host(nullptr)
, _port(1883)
, _useIp(false)
, _keepAlive(15000)
, _cleanSession(true)
, _username(nullptr)
, _password(nullptr)
, _willTopic(nullptr)
, _willPayload(nullptr)
, _willPayloadLength(0)
, _willQos(0)
, _willRetain(false)
, _timeout(EMC_TX_TIMEOUT)
, _state(State::disconnected)
, _generatedClientId{0}
, _packetId(0)
#if defined(ARDUINO_ARCH_ESP32)
, _xSemaphore(nullptr)
, _taskHandle(nullptr)
#endif
, _rxBuffer{0}
, _outbox()
, _bytesSent(0)
, _parser()
, _lastClientActivity(0)
, _lastServerActivity(0)
, _pingSent(false)
, _disconnectReason(DisconnectReason::TCP_DISCONNECTED)
#if defined(ARDUINO_ARCH_ESP32) && ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
, _highWaterMark(4294967295)
#endif
  {
  EMC_GENERATE_CLIENTID(_generatedClientId);
#if defined(ARDUINO_ARCH_ESP32)
  _xSemaphore = xSemaphoreCreateMutex();
  EMC_SEMAPHORE_GIVE();  // release before first use
  if (_useInternalTask == espMqttClientTypes::UseInternalTask::YES) {
    xTaskCreatePinnedToCore((TaskFunction_t)_loop, "mqttclient", EMC_TASK_STACK_SIZE, this, priority, &_taskHandle, core);
  }
#else
  (void) useInternalTask;
  (void) priority;
  (void) core;
#endif
  _clientId = _generatedClientId;
}

MqttClient::~MqttClient() {
  disconnect(true);
  _clearQueue(2);
#if defined(ARDUINO_ARCH_ESP32)
  vSemaphoreDelete(_xSemaphore);
  if (_useInternalTask == espMqttClientTypes::UseInternalTask::YES) {
    #if EMC_USE_WATCHDOG
    esp_task_wdt_delete(_taskHandle);  // not sure if this is really needed
    #endif
    vTaskDelete(_taskHandle);
  }
#endif
}

bool MqttClient::connected() const {
  if (_state == State::connected) return true;
  return false;
}

bool MqttClient::disconnected() const {
  if (_state == State::disconnected) return true;
  return false;
}

bool MqttClient::connect() {
  bool result = false;
  if (_state == State::disconnected) {
    EMC_SEMAPHORE_TAKE();
    if (_addPacketFront(_cleanSession,
                        _username,
                        _password,
                        _willTopic,
                        _willRetain,
                        _willQos,
                        _willPayload,
                        _willPayloadLength,
                        (uint16_t)(_keepAlive / 1000),  // 32b to 16b doesn't overflow because it comes from 16b orignally
                        _clientId)) {
      result = true;
      _setState(State::connectingTcp1);
      #if defined(ARDUINO_ARCH_ESP32)
      if (_useInternalTask == espMqttClientTypes::UseInternalTask::YES) {
        vTaskResume(_taskHandle);
      }
      #endif
    } else {
      emc_log_e("Could not create CONNECT packet");
      EMC_SEMAPHORE_GIVE();
      _onError(0, Error::OUT_OF_MEMORY);
      EMC_SEMAPHORE_TAKE();
    }
    EMC_SEMAPHORE_GIVE();
  }
  return result;
}

bool MqttClient::disconnect(bool force) {
  if (force && _state != State::disconnected && _state != State::disconnectingTcp1 && _state != State::disconnectingTcp2) {
    _setState(State::disconnectingTcp1);
    return true;
  }
  if (!force && _state == State::connected) {
    _setState(State::disconnectingMqtt1);
    return true;
  }
  return false;
}

uint16_t MqttClient::publish(const char* topic, uint8_t qos, bool retain, const uint8_t* payload, size_t length) {
  #if !EMC_ALLOW_NOT_CONNECTED_PUBLISH
  if (_state != State::connected) {
  #else
  if (_state > State::connected) {
  #endif
    return 0;
  }
  EMC_SEMAPHORE_TAKE();
  uint16_t packetId = (qos > 0) ? _getNextPacketId() : 1;
  if (!_addPacket(packetId, topic, payload, length, qos, retain)) {
    emc_log_e("Could not create PUBLISH packet");
    EMC_SEMAPHORE_GIVE();
    _onError(packetId, Error::OUT_OF_MEMORY);
    EMC_SEMAPHORE_TAKE();
    packetId = 0;
  }
  EMC_SEMAPHORE_GIVE();
  return packetId;
}

uint16_t MqttClient::publish(const char* topic, uint8_t qos, bool retain, const char* payload) {
  size_t len = strlen(payload);
  return publish(topic, qos, retain, reinterpret_cast<const uint8_t*>(payload), len);
}

uint16_t MqttClient::publish(const char* topic, uint8_t qos, bool retain, espMqttClientTypes::PayloadCallback callback, size_t length) {
  #if !EMC_ALLOW_NOT_CONNECTED_PUBLISH
  if (_state != State::connected) {
  #else
  if (_state > State::connected) {
  #endif
    return 0;
  }
  EMC_SEMAPHORE_TAKE();
  uint16_t packetId = (qos > 0) ? _getNextPacketId() : 1;
  if (!_addPacket(packetId, topic, callback, length, qos, retain)) {
    emc_log_e("Could not create PUBLISH packet");
    EMC_SEMAPHORE_GIVE();
    _onError(packetId, Error::OUT_OF_MEMORY);
    EMC_SEMAPHORE_TAKE();
    packetId = 0;
  }
  EMC_SEMAPHORE_GIVE();
  return packetId;
}

void MqttClient::clearQueue(bool deleteSessionData) {
  EMC_SEMAPHORE_TAKE();
  _clearQueue(deleteSessionData ? 2 : 0);
  EMC_SEMAPHORE_GIVE();
}

const char* MqttClient::getClientId() const {
  return _clientId;
}

size_t MqttClient::queueSize() {
  size_t ret = 0;
  EMC_SEMAPHORE_TAKE();
  ret = _outbox.size();
  EMC_SEMAPHORE_GIVE();
  return ret;
}

void MqttClient::loop() {
  switch (_state) {
    case State::disconnected:
      #if defined(ARDUINO_ARCH_ESP32)
      if (_useInternalTask == espMqttClientTypes::UseInternalTask::YES) {
        vTaskSuspend(_taskHandle);
      }
      #endif
      break;
    case State::connectingTcp1:
      if (_useIp ? _transport->connect(_ip, _port) : _transport->connect(_host, _port)) {
        _setState(State::connectingTcp2);
      } else {
        _setState(State::disconnectingTcp1);
        _disconnectReason = DisconnectReason::TCP_DISCONNECTED;
        break;
      }
      // Falling through to speed up connecting on blocking transport 'connect' implementations
      [[fallthrough]];
    case State::connectingTcp2:
      if (_transport->connected()) {
        _parser.reset();
        _lastClientActivity = _lastServerActivity = millis();
        _setState(State::connectingMqtt);
      }  else if (_transport->disconnected()) {  // sync: implemented as "not connected"; async: depending on state of pcb in underlying lib
        _setState(State::disconnectingTcp1);
        _disconnectReason = DisconnectReason::TCP_DISCONNECTED;
      }
      break;
    case State::connectingMqtt:
      #if EMC_WAIT_FOR_CONNACK
      if (_transport->connected()) {
        EMC_SEMAPHORE_TAKE();
        _sendPacket();
        _checkIncoming();
        _checkPing();
        EMC_SEMAPHORE_GIVE();
      } else {
        _setState(State::disconnectingTcp1);
        _disconnectReason = DisconnectReason::TCP_DISCONNECTED;
      }
      break;
      #else
      // receipt of CONNACK packet will set state to CONNECTED
      // client however is allowed to send packets before CONNACK is received
      // so we fall through to 'connected'
      [[fallthrough]];
      #endif
    case State::connected:
      [[fallthrough]];
    case State::disconnectingMqtt2:
      if (_transport->connected()) {
        // CONNECT packet is first in the queue
        EMC_SEMAPHORE_TAKE();
        _checkOutbox();
        _checkIncoming();
        _checkPing();
        _checkTimeout();
        EMC_SEMAPHORE_GIVE();
      } else {
        _setState(State::disconnectingTcp1);
        _disconnectReason = DisconnectReason::TCP_DISCONNECTED;
      }
      break;
    case State::disconnectingMqtt1:
      EMC_SEMAPHORE_TAKE();
      if (_outbox.empty()) {
        if (!_addPacket(PacketType.DISCONNECT)) {
          EMC_SEMAPHORE_GIVE();
          emc_log_e("Could not create DISCONNECT packet");
          _onError(0, Error::OUT_OF_MEMORY);
          EMC_SEMAPHORE_TAKE();
        } else {
          _setState(State::disconnectingMqtt2);
        }
      }
      _checkOutbox();
      _checkIncoming();
      _checkPing();
      _checkTimeout();
      EMC_SEMAPHORE_GIVE();
      break;
    case State::disconnectingTcp1:
      _transport->stop();
      _setState(State::disconnectingTcp2);
      break;  // keep break to accomodate async clients
    case State::disconnectingTcp2:
      if (_transport->disconnected()) {
        EMC_SEMAPHORE_TAKE();
        _clearQueue(0);
        EMC_SEMAPHORE_GIVE();
        _bytesSent = 0;
        _setState(State::disconnected);
        if (_onDisconnectCallback) {
          _onDisconnectCallback(_disconnectReason);
        }
      }
      break;
    // all cases covered, no default case
  }
  EMC_YIELD();
  #if defined(ARDUINO_ARCH_ESP32) && ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
  size_t waterMark = uxTaskGetStackHighWaterMark(NULL);
  if (waterMark < _highWaterMark) {
    _highWaterMark = waterMark;
    emc_log_i("Stack usage: %zu/%i", EMC_TASK_STACK_SIZE - _highWaterMark, EMC_TASK_STACK_SIZE);
  }
  #endif
}

#if defined(ARDUINO_ARCH_ESP32)
void MqttClient::_loop(MqttClient* c) {
  #if EMC_USE_WATCHDOG
  if (esp_task_wdt_add(NULL) != ESP_OK) {
    emc_log_e("Failed to add async task to WDT");
  }
  #endif
  for (;;) {
    c->loop();
    #if EMC_USE_WATCHDOG
    esp_task_wdt_reset();
    #endif
  }
}
#endif

inline void MqttClient::_setState(State newState) {
  emc_log_i("state %i --> %i", static_cast<std::underlying_type<State>::type>(_state.load()), static_cast<std::underlying_type<State>::type>(newState));
  _state = newState;
}

uint16_t MqttClient::_getNextPacketId() {
  ++_packetId;
  if (_packetId == 0) ++_packetId;
  return _packetId;
}

void MqttClient::_checkOutbox() {
  while (_sendPacket() > 0) {
    if (!_advanceOutbox()) {
      break;
    }
  }
}

int MqttClient::_sendPacket() {
  OutgoingPacket* packet = _outbox.getCurrent();

  size_t written = 0;
  if (packet) {
    size_t wantToWrite = packet->packet.available(_bytesSent);
    if (wantToWrite == 0) {
      return 0;
    }
    written = _transport->write(packet->packet.data(_bytesSent), wantToWrite);
    packet->timeSent = millis();
    _lastClientActivity = millis();
    _bytesSent += written;
    emc_log_i("tx %zu/%zu (%02x)", _bytesSent, packet->packet.size(), packet->packet.packetType());
  }
  return written;
}

bool MqttClient::_advanceOutbox() {
  OutgoingPacket* packet = _outbox.getCurrent();
  if (packet && _bytesSent == packet->packet.size()) {
    if ((packet->packet.packetType()) == PacketType.DISCONNECT) {
      _setState(State::disconnectingTcp1);
      _disconnectReason = DisconnectReason::USER_OK;
    }
    if (packet->packet.removable()) {
      _outbox.removeCurrent();
    } else {
      // we already set 'dup' here, in case we have to retry
      if ((packet->packet.packetType()) == PacketType.PUBLISH) packet->packet.setDup();
      _outbox.next();
    }
    packet = _outbox.getCurrent();
    _bytesSent = 0;
  }
  return packet;
}

void MqttClient::_checkIncoming() {
  int32_t remainingBufferLength = _transport->read(_rxBuffer, EMC_RX_BUFFER_SIZE);
  if (remainingBufferLength > 0) {
    _lastServerActivity = millis();
    emc_log_i("rx len %i", remainingBufferLength);
    size_t bytesParsed = 0;
    size_t index = 0;
    while (remainingBufferLength > 0) {
      espMqttClientInternals::ParserResult result = _parser.parse(&_rxBuffer[index], remainingBufferLength, &bytesParsed);
      if (result == espMqttClientInternals::ParserResult::packet) {
        espMqttClientInternals::MQTTPacketType packetType = _parser.getPacket().fixedHeader.packetType & 0xF0;
        if (_state == State::connectingMqtt && packetType != PacketType.CONNACK) {
          emc_log_w("Disconnecting, expected CONNACK - protocol error");
          _setState(State::disconnectingTcp1);
          return;
        }
        switch (packetType) {
          case PacketType.CONNACK:
            _onConnack();
            if (_state != State::connected) {
              return;
            }
            break;
          case PacketType.PUBLISH:
            if (_state >= State::disconnectingMqtt1) break;  // stop processing incoming once user has called disconnect
            _onPublish();
            break;
          case PacketType.PUBACK:
            _onPuback();
            break;
          case PacketType.PUBREC:
            _onPubrec();
            break;
          case PacketType.PUBREL:
            _onPubrel();
            break;
          case PacketType.PUBCOMP:
            _onPubcomp();
            break;
          case PacketType.SUBACK:
            _onSuback();
            break;
          case PacketType.UNSUBACK:
            _onUnsuback();
            break;
          case PacketType.PINGRESP:
            _pingSent = false;
            break;
        }
      } else if (result ==  espMqttClientInternals::ParserResult::protocolError) {
        emc_log_w("Disconnecting, protocol error");
        _setState(State::disconnectingTcp1);
        _disconnectReason = DisconnectReason::TCP_DISCONNECTED;
        return;
      }
      remainingBufferLength -= bytesParsed;
      index += bytesParsed;
      emc_log_i("Parsed %zu - remaining %i", bytesParsed, remainingBufferLength);
      bytesParsed = 0;
    }
  }
}

void MqttClient::_checkPing() {
  if (_keepAlive == 0) return;  // keepalive is disabled

  uint32_t currentMillis = millis();

  // disconnect when server was inactive for twice the keepalive time
  if (currentMillis - _lastServerActivity > 2 * _keepAlive) {
    emc_log_w("Disconnecting, server exceeded keepalive");
    _setState(State::disconnectingTcp1);
    _disconnectReason = DisconnectReason::TCP_DISCONNECTED;
    return;
  }

  // send ping when client was inactive during the keepalive time
  // or when server hasn't responded within keepalive time (typically due to QOS 0)
  if (!_pingSent &&
      ((currentMillis - _lastClientActivity > _keepAlive) ||
       (currentMillis - _lastServerActivity > _keepAlive))) {
    if (!_addPacket(PacketType.PINGREQ)) {
      emc_log_e("Could not create PING packet");
      return;
    }
    _pingSent = true;
  }
}

void MqttClient::_checkTimeout() {
  espMqttClientInternals::Outbox<OutgoingPacket>::Iterator it = _outbox.front();
  // check that we're not busy sending
  // don't check when first item hasn't been sent yet
  if (it && _bytesSent == 0 && it.get() != _outbox.getCurrent()) {
    if (millis() - it.get()->timeSent > _timeout) {
      emc_log_w("Packet ack timeout, retrying");
      _outbox.resetCurrent();
    }
  }
}

void MqttClient::_onConnack() {
  if (_parser.getPacket().variableHeader.fixed.connackVarHeader.returnCode == 0x00) {
    _pingSent = false;  // reset after keepalive timeout disconnect
    _setState(State::connected);
    _advanceOutbox();
    if (_parser.getPacket().variableHeader.fixed.connackVarHeader.sessionPresent == 0) {
      _clearQueue(1);
    }
    if (_onConnectCallback) {
      EMC_SEMAPHORE_GIVE();
      _onConnectCallback(_parser.getPacket().variableHeader.fixed.connackVarHeader.sessionPresent);
      EMC_SEMAPHORE_TAKE();
    }
  } else {
    _setState(State::disconnectingTcp1);
    // cast is safe because the parser already checked for a valid return code
    _disconnectReason = static_cast<DisconnectReason>(_parser.getPacket().variableHeader.fixed.connackVarHeader.returnCode);
  }
}

void MqttClient::_onPublish() {
  const espMqttClientInternals::IncomingPacket& p = _parser.getPacket();
  uint8_t qos = p.qos();
  bool retain = p.retain();
  bool dup = p.dup();
  uint16_t packetId = p.variableHeader.fixed.packetId;
  bool callback = true;
  if (qos == 1) {
    if (p.payload.index + p.payload.length == p.payload.total) {
      if (!_addPacket(PacketType.PUBACK, packetId)) {
        emc_log_e("Could not create PUBACK packet");
      }
    }
  } else if (qos == 2) {
    espMqttClientInternals::Outbox<OutgoingPacket>::Iterator it = _outbox.front();
    while (it) {
      if ((it.get()->packet.packetType()) == PacketType.PUBREC && it.get()->packet.packetId() == packetId) {
        callback = false;
        emc_log_e("QoS2 packet previously delivered");
        break;
      }
      ++it;
    }
    if (p.payload.index + p.payload.length == p.payload.total) {
      if (!_addPacket(PacketType.PUBREC, packetId)) {
        emc_log_e("Could not create PUBREC packet");
      }
    }
  }
  if (callback && _onMessageCallback) {
    EMC_SEMAPHORE_GIVE();
    _onMessageCallback({qos, dup, retain, packetId},
                       p.variableHeader.topic,
                       p.payload.data,
                       p.payload.length,
                       p.payload.index,
                       p.payload.total);
    EMC_SEMAPHORE_TAKE();
  }
}

void MqttClient::_onPuback() {
  bool callback = false;
  uint16_t idToMatch = _parser.getPacket().variableHeader.fixed.packetId;
  espMqttClientInternals::Outbox<OutgoingPacket>::Iterator it = _outbox.front();
  while (it) {
    // PUBACKs come in the order PUBs are sent. So we only check the first PUB packet in outbox
    // if it doesn't match the ID, return
    if ((it.get()->packet.packetType()) == PacketType.PUBLISH) {
      if (it.get()->packet.packetId() == idToMatch) {
        callback = true;
        _outbox.remove(it);
        break;
      }
      emc_log_w("Received out of order PUBACK");
      break;
    }
    ++it;
  }
  if (callback) {
    if (_onPublishCallback) {
      EMC_SEMAPHORE_GIVE();
      _onPublishCallback(idToMatch);
      EMC_SEMAPHORE_TAKE();
    }
  } else {
    emc_log_w("No matching PUBLISH packet found");
  }
}

void MqttClient::_onPubrec() {
  bool success = false;
  uint16_t idToMatch = _parser.getPacket().variableHeader.fixed.packetId;
  espMqttClientInternals::Outbox<OutgoingPacket>::Iterator it = _outbox.front();
  while (it) {
    // PUBRECs come in the order PUBs are sent. So we only check the first PUB packet in outbox
    // if it doesn't match the ID, return
    if ((it.get()->packet.packetType()) == PacketType.PUBLISH) {
      if (it.get()->packet.packetId() == idToMatch) {
        if (!_addPacket(PacketType.PUBREL, idToMatch)) {
          emc_log_e("Could not create PUBREL packet");
        }
        _outbox.remove(it);
        success = true;
        break;
      }
      emc_log_w("Received out of order PUBREC");
      break;
    }
    ++it;
  }
  if (!success) {
    emc_log_w("No matching PUBLISH packet found");
  }
}

void MqttClient::_onPubrel() {
  bool success = false;
  uint16_t idToMatch = _parser.getPacket().variableHeader.fixed.packetId;
  espMqttClientInternals::Outbox<OutgoingPacket>::Iterator it = _outbox.front();
  while (it) {
    // PUBRELs come in the order PUBRECs are sent. So we only check the first PUBREC packet in outbox
    // if it doesn't match the ID, return
    if ((it.get()->packet.packetType()) == PacketType.PUBREC) {
      if (it.get()->packet.packetId() == idToMatch) {
        if (!_addPacket(PacketType.PUBCOMP, idToMatch)) {
          emc_log_e("Could not create PUBCOMP packet");
        }
        _outbox.remove(it);
        success = true;
        break;
      }
      emc_log_w("Received out of order PUBREL");
      break;
    }
    ++it;
  }
  if (!success) {
    emc_log_w("No matching PUBREC packet found");
  }
}

void MqttClient::_onPubcomp() {
  bool callback = false;
  espMqttClientInternals::Outbox<OutgoingPacket>::Iterator it = _outbox.front();
  uint16_t idToMatch = _parser.getPacket().variableHeader.fixed.packetId;
  while (it) {
    // PUBCOMPs come in the order PUBRELs are sent. So we only check the first PUBREL packet in outbox
    // if it doesn't match the ID, return
    if ((it.get()->packet.packetType()) == PacketType.PUBREL) {
      if (it.get()->packet.packetId() == idToMatch) {
        callback = true;
        _outbox.remove(it);
        break;
      }
      emc_log_w("Received out of order PUBCOMP");
      break;
    }
    ++it;
  }
  if (callback) {
    if (_onPublishCallback) {
      EMC_SEMAPHORE_GIVE();
      _onPublishCallback(idToMatch);
      EMC_SEMAPHORE_TAKE();
    }
  } else {
    emc_log_w("No matching PUBREL packet found");
  }
}

void MqttClient::_onSuback() {
  bool callback = false;
  uint16_t idToMatch = _parser.getPacket().variableHeader.fixed.packetId;
  espMqttClientInternals::Outbox<OutgoingPacket>::Iterator it = _outbox.front();
  while (it) {
    if (((it.get()->packet.packetType()) == PacketType.SUBSCRIBE) && it.get()->packet.packetId() == idToMatch) {
      callback = true;
      _outbox.remove(it);
      break;
    }
    ++it;
  }
  if (callback) {
    if (_onSubscribeCallback) {
      EMC_SEMAPHORE_GIVE();
      _onSubscribeCallback(idToMatch, reinterpret_cast<const espMqttClientTypes::SubscribeReturncode*>(_parser.getPacket().payload.data), _parser.getPacket().payload.total);
      EMC_SEMAPHORE_TAKE();
    }
  } else {
    emc_log_w("received SUBACK without SUB");
  }
}

void MqttClient::_onUnsuback() {
  bool callback = false;
  espMqttClientInternals::Outbox<OutgoingPacket>::Iterator it = _outbox.front();
  uint16_t idToMatch = _parser.getPacket().variableHeader.fixed.packetId;
  while (it) {
    if (it.get()->packet.packetId() == idToMatch) {
      callback = true;
      _outbox.remove(it);
      break;
    }
    ++it;
  }
  if (callback) {
    if (_onUnsubscribeCallback) {
      EMC_SEMAPHORE_GIVE();
      _onUnsubscribeCallback(idToMatch);
      EMC_SEMAPHORE_TAKE();
    }
  } else {
    emc_log_w("received UNSUBACK without UNSUB");
  }
}

void MqttClient::_clearQueue(int clearData) {
  emc_log_i("clearing queue (clear session: %d)", clearData);
  espMqttClientInternals::Outbox<OutgoingPacket>::Iterator it = _outbox.front();
  if (clearData == 0) {
    // keep PUB (qos > 0, aka packetID != 0), PUBREC and PUBREL
    // Spec only mentions PUB and PUBREL but this lib implements method B from point 4.3.3 (Fig. 4.3)
    // and stores the packet id in the PUBREC packet. So we also must keep PUBREC.
    while (it) {
      espMqttClientInternals::MQTTPacketType type = it.get()->packet.packetType();
      if (type == PacketType.PUBREC ||
          type == PacketType.PUBREL ||
         (type == PacketType.PUBLISH && it.get()->packet.packetId() != 0)) {
        ++it;
      } else {
        _outbox.remove(it);
      }
    }
  } else if (clearData == 1) {
    // keep PUB
    while (it) {
      if (it.get()->packet.packetType() == PacketType.PUBLISH) {
        ++it;
      } else {
        _outbox.remove(it);
      }
    }
  } else {  // clearData == 2
    while (it) {
      _outbox.remove(it);
    }
  }
}

void MqttClient::_onError(uint16_t packetId, espMqttClientTypes::Error error) {
  if (_onErrorCallback) {
    _onErrorCallback(packetId, error);
  }
}
