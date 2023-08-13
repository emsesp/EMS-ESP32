/*
Copyright (c) 2022 Bert Melis. All rights reserved.

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <algorithm>

#include "../Config.h"
#include "Constants.h"
#include "../Logging.h"
#include "RemainingLength.h"

namespace espMqttClientInternals {

struct IncomingPacket {
  struct __attribute__((__packed__)) {
    MQTTPacketType packetType;
    union {
      size_t remainingLength;
      uint8_t remainingLengthRaw[4];
    } remainingLength;
  } fixedHeader;
  struct __attribute__((__packed__)) {
    uint16_t topicLength;
    char topic[EMC_MAX_TOPIC_LENGTH + 1];  // + 1 for c-string delimiter
    union {
      struct {
        uint8_t sessionPresent;
        uint8_t returnCode;
      } connackVarHeader;
      uint16_t packetId;
    } fixed;
  } variableHeader;
  struct {
    const uint8_t* data;
    size_t length;
    size_t index;
    size_t total;
  } payload;

  uint8_t qos() const;
  bool retain() const;
  bool dup() const;
  void reset();
};

enum class ParserResult : uint8_t {
  awaitData,
  packet,
  protocolError
};

class Parser;
typedef ParserResult(*ParserFunc)(Parser*);

class Parser {
 public:
  Parser();
  ParserResult parse(const uint8_t* data, size_t len, size_t* bytesRead);
  const IncomingPacket& getPacket() const;
  void reset();

 private:
  // keep data variables in class to avoid copying on every iteration of the parser
  const uint8_t* _data;
  size_t _len;
  size_t _bytesRead;
  size_t _bytePos;
  ParserFunc _parse;
  IncomingPacket _packet;
  uint8_t _payloadBuffer[EMC_PAYLOAD_BUFFER_SIZE];

  static ParserResult _fixedHeader(Parser* p);
  static ParserResult _remainingLengthFixed(Parser* p);
  static ParserResult _remainingLengthNone(Parser* p);
  static ParserResult _remainingLengthVariable(Parser* p);


  static ParserResult _varHeaderConnack1(Parser* p);
  static ParserResult _varHeaderConnack2(Parser* p);

  static ParserResult _varHeaderPacketId1(Parser* p);
  static ParserResult _varHeaderPacketId2(Parser* p);

  static ParserResult _varHeaderTopicLength1(Parser* p);
  static ParserResult _varHeaderTopicLength2(Parser* p);
  static ParserResult _varHeaderTopic(Parser* p);

  static ParserResult _payloadSuback(Parser* p);
  static ParserResult _payloadPublish(Parser* p);
};

}  // end namespace espMqttClientInternals
