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

namespace espMqttClientInternals {

constexpr const char PROTOCOL[] = "MQTT";
constexpr const uint8_t PROTOCOL_LEVEL = 0b00000100;

typedef uint8_t MQTTPacketType;

constexpr struct {
  const uint8_t RESERVED1   = 0;
  const uint8_t CONNECT     = 1 << 4;
  const uint8_t CONNACK     = 2 << 4;
  const uint8_t PUBLISH     = 3 << 4;
  const uint8_t PUBACK      = 4 << 4;
  const uint8_t PUBREC      = 5 << 4;
  const uint8_t PUBREL      = 6 << 4;
  const uint8_t PUBCOMP     = 7 << 4;
  const uint8_t SUBSCRIBE   = 8 << 4;
  const uint8_t SUBACK      = 9 << 4;
  const uint8_t UNSUBSCRIBE = 10 << 4;
  const uint8_t UNSUBACK    = 11 << 4;
  const uint8_t PINGREQ     = 12 << 4;
  const uint8_t PINGRESP    = 13 << 4;
  const uint8_t DISCONNECT  = 14 << 4;
  const uint8_t RESERVED2   = 1 << 4;
} PacketType;

constexpr struct {
  const uint8_t CONNECT_RESERVED     = 0x00;
  const uint8_t CONNACK_RESERVED     = 0x00;
  const uint8_t PUBLISH_DUP          = 0x08;
  const uint8_t PUBLISH_QOS0         = 0x00;
  const uint8_t PUBLISH_QOS1         = 0x02;
  const uint8_t PUBLISH_QOS2         = 0x04;
  const uint8_t PUBLISH_QOSRESERVED  = 0x06;
  const uint8_t PUBLISH_RETAIN       = 0x01;
  const uint8_t PUBACK_RESERVED      = 0x00;
  const uint8_t PUBREC_RESERVED      = 0x00;
  const uint8_t PUBREL_RESERVED      = 0x02;
  const uint8_t PUBCOMP_RESERVED     = 0x00;
  const uint8_t SUBSCRIBE_RESERVED   = 0x02;
  const uint8_t SUBACK_RESERVED      = 0x00;
  const uint8_t UNSUBSCRIBE_RESERVED = 0x02;
  const uint8_t UNSUBACK_RESERVED    = 0x00;
  const uint8_t PINGREQ_RESERVED     = 0x00;
  const uint8_t PINGRESP_RESERVED    = 0x00;
  const uint8_t DISCONNECT_RESERVED  = 0x00;
  const uint8_t RESERVED2_RESERVED   = 0x00;
} HeaderFlag;

constexpr struct {
  const uint8_t USERNAME      = 0x80;
  const uint8_t PASSWORD      = 0x40;
  const uint8_t WILL_RETAIN   = 0x20;
  const uint8_t WILL_QOS0     = 0x00;
  const uint8_t WILL_QOS1     = 0x08;
  const uint8_t WILL_QOS2     = 0x10;
  const uint8_t WILL          = 0x04;
  const uint8_t CLEAN_SESSION = 0x02;
  const uint8_t RESERVED      = 0x00;
} ConnectFlag;

}  // end namespace espMqttClientInternals
