/*
Copyright (c) 2022 Bert Melis. All rights reserved.

Parts are based on the original work of Marvin Roger:
https://github.com/marvinroger/async-mqtt-client

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <functional>

namespace espMqttClientTypes {

enum class DisconnectReason : uint8_t {
  USER_OK = 0,
  MQTT_UNACCEPTABLE_PROTOCOL_VERSION = 1,
  MQTT_IDENTIFIER_REJECTED = 2,
  MQTT_SERVER_UNAVAILABLE = 3,
  MQTT_MALFORMED_CREDENTIALS = 4,
  MQTT_NOT_AUTHORIZED = 5,
  TLS_BAD_FINGERPRINT = 6,
  TCP_DISCONNECTED = 7
};

const char* disconnectReasonToString(DisconnectReason reason);

enum class SubscribeReturncode : uint8_t {
  QOS0 = 0x00,
  QOS1 = 0x01,
  QOS2 = 0x02,
  FAIL = 0X80
};

const char* subscribeReturncodeToString(SubscribeReturncode returnCode);

enum class Error : uint8_t {
  SUCCESS = 0,
  OUT_OF_MEMORY = 1,
  MAX_RETRIES = 2,
  MALFORMED_PARAMETER = 3,
  MISC_ERROR = 4
};

const char* errorToString(Error error);

struct MessageProperties {
  uint8_t qos;
  bool dup;
  bool retain;
  uint16_t packetId;
};

typedef std::function<void(bool sessionPresent)> OnConnectCallback;
typedef std::function<void(DisconnectReason reason)> OnDisconnectCallback;
typedef std::function<void(uint16_t packetId, const SubscribeReturncode* returncodes, size_t len)> OnSubscribeCallback;
typedef std::function<void(uint16_t packetId)> OnUnsubscribeCallback;
typedef std::function<void(const MessageProperties& properties, const char* topic, const uint8_t* payload, size_t len, size_t index, size_t total)> OnMessageCallback;
typedef std::function<void(uint16_t packetId)> OnPublishCallback;
typedef std::function<size_t(uint8_t* data, size_t maxSize, size_t index)> PayloadCallback;
typedef std::function<void(uint16_t packetId, Error error)> OnErrorCallback;

enum class UseInternalTask {
  NO = 0,
  YES = 1,
};

}  // end namespace espMqttClientTypes
