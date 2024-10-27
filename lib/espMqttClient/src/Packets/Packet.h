/*
Copyright (c) 2022 Bert Melis. All rights reserved.

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#pragma once

#include <stdint.h>
#include <stddef.h>

#include "Constants.h"
#include "../Config.h"
#include "../TypeDefs.h"
#include "../Helpers.h"
#include "../Logging.h"
#include "RemainingLength.h"
#include "StringUtil.h"

#if EMC_USE_MEMPOOL
  #include "MemoryPool/src/MemoryPool.h"
#endif

namespace espMqttClientInternals {

class Packet {
 public:
  ~Packet();
  size_t available(size_t index);
  const uint8_t* data(size_t index) const;

  size_t size() const;
  void setDup();
  uint16_t packetId() const;
  MQTTPacketType packetType() const;
  bool removable() const;

 protected:
  uint16_t _packetId;  // save as separate variable: will be accessed frequently
  uint8_t* _data;
  size_t _size;

  // variables for chunked payload handling
  size_t _payloadIndex;
  size_t _payloadStartIndex;
  size_t _payloadEndIndex;
  espMqttClientTypes::PayloadCallback _getPayload;

  struct SubscribeItem {
    const char* topic;
    uint8_t qos;
  };

 public:
  // CONNECT
  Packet(espMqttClientTypes::Error& error,  // NOLINT(runtime/references)
         bool cleanSession,
         const char* username,
         const char* password,
         const char* willTopic,
         bool willRetain,
         uint8_t willQos,
         const uint8_t* willPayload,
         uint16_t willPayloadLength,
         uint16_t keepAlive,
         const char* clientId);
  // PUBLISH
  Packet(espMqttClientTypes::Error& error,  // NOLINT(runtime/references)
         uint16_t packetId,
         const char* topic,
         const uint8_t* payload,
         size_t payloadLength,
         uint8_t qos,
         bool retain);
  Packet(espMqttClientTypes::Error& error,  // NOLINT(runtime/references)
         uint16_t packetId,
         const char* topic,
         espMqttClientTypes::PayloadCallback payloadCallback,
         size_t payloadLength,
         uint8_t qos,
         bool retain);
  // SUBSCRIBE
  Packet(espMqttClientTypes::Error& error,  // NOLINT(runtime/references)
         uint16_t packetId,
         const char* topic,
         uint8_t qos);
  template<typename ... Args>
  Packet(espMqttClientTypes::Error& error,  // NOLINT(runtime/references)
         uint16_t packetId,
         const char* topic1,
         uint8_t qos1,
         const char* topic2,
         uint8_t qos2,
         Args&& ... args)
  : _packetId(packetId)
  , _data(nullptr)
  , _size(0)
  , _payloadIndex(0)
  , _payloadStartIndex(0)
  , _payloadEndIndex(0)
  , _getPayload(nullptr) {
    static_assert(sizeof...(Args) % 2 == 0, "Subscribe should be in topic/qos pairs");
    size_t numberTopics = 2 + (sizeof...(Args) / 2);
    SubscribeItem list[numberTopics] = {topic1, qos1, topic2, qos2, args...};
    _createSubscribe(error, list, numberTopics);
  }
  // UNSUBSCRIBE
  Packet(espMqttClientTypes::Error& error,  // NOLINT(runtime/references)
         uint16_t packetId,
         const char* topic);
  template<typename ... Args>
  Packet(espMqttClientTypes::Error& error,  // NOLINT(runtime/references)
         uint16_t packetId,
         const char* topic1,
         const char* topic2,
         Args&& ... args)
  : _packetId(packetId)
  , _data(nullptr)
  , _size(0)
  , _payloadIndex(0)
  , _payloadStartIndex(0)
  , _payloadEndIndex(0)
  , _getPayload(nullptr) {
    size_t numberTopics = 2 + sizeof...(Args);
    const char* list[numberTopics] = {topic1, topic2, args...};
    _createUnsubscribe(error, list, numberTopics);
  }
  // PUBACK, PUBREC, PUBREL, PUBCOMP
  Packet(espMqttClientTypes::Error& error,  // NOLINT(runtime/references)
         MQTTPacketType type,
         uint16_t packetId);
  // PING, DISCONN
  explicit Packet(espMqttClientTypes::Error& error,  // NOLINT(runtime/references)
                  MQTTPacketType type);

 private:
  // pass remainingLength = total size - header - remainingLengthLength!
  bool _allocate(size_t remainingLength, bool check);

  // fills header and returns index of next available byte in buffer
  size_t _fillPublishHeader(uint16_t packetId,
                            const char* topic,
                            size_t remainingLength,
                            uint8_t qos,
                            bool retain);
  void _createSubscribe(espMqttClientTypes::Error& error,  // NOLINT(runtime/references)
                        SubscribeItem* list,
                        size_t numberTopics);
  void _createUnsubscribe(espMqttClientTypes::Error& error,  // NOLINT(runtime/references)
                          const char** list,
                          size_t numberTopics);

  size_t _chunkedAvailable(size_t index);
  const uint8_t* _chunkedData(size_t index) const;

  #if EMC_USE_MEMPOOL
  static MemoryPool::Variable<EMC_NUM_POOL_ELEMENTS, EMC_SIZE_POOL_ELEMENTS> _memPool;
  #endif
};

}  // end namespace espMqttClientInternals
