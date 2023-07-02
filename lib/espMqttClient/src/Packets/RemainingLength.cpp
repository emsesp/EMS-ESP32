/*
Copyright (c) 2022 Bert Melis. All rights reserved.

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#include "RemainingLength.h"

namespace espMqttClientInternals {

int32_t decodeRemainingLength(const uint8_t* stream) {
  uint32_t multiplier = 1;
  int32_t remainingLength = 0;
  uint8_t currentByte = 0;
  uint8_t encodedByte;

  do {
    encodedByte = stream[currentByte++];
    remainingLength += (encodedByte & 127) * multiplier;
    if (multiplier > 128 * 128 * 128) {
      emc_log_e("Malformed Remaining Length");
      return -1;
    }
    multiplier *= 128;
  } while ((encodedByte & 128) != 0);

  return remainingLength;
}

uint8_t remainingLengthLength(uint32_t remainingLength) {
  if (remainingLength < 128) return 1;
  if (remainingLength < 16384) return 2;
  if (remainingLength < 2097152) return 3;
  if (remainingLength > 268435455) return 0;
  return 4;
}

uint8_t encodeRemainingLength(uint32_t remainingLength, uint8_t* destination) {
  uint8_t currentByte = 0;
  uint8_t bytesNeeded = 0;

  do {
    uint8_t encodedByte = remainingLength % 128;
    remainingLength /= 128;
    if (remainingLength > 0) {
      encodedByte = encodedByte | 128;
    }
    destination[currentByte++] = encodedByte;
    bytesNeeded++;
  } while (remainingLength > 0);

  return bytesNeeded;
}

}  // namespace espMqttClientInternals
