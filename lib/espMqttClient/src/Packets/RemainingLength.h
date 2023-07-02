/*
Copyright (c) 2022 Bert Melis. All rights reserved.

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#pragma once

#include <stdint.h>

#include "../Logging.h"

namespace espMqttClientInternals {

// Calculations are based on non normative comment in section 2.2.3 Remaining Length of the MQTT specification

// returns decoded length based on input stream
// stream is expected to contain full encoded remaining length
// return -1 on error.
int32_t decodeRemainingLength(const uint8_t* stream);


// returns the number of bytes needed to encode the remaining length
uint8_t remainingLengthLength(uint32_t remainingLength);

// encodes the given remaining length to destination and returns number of bytes used
// destination is expected to be large enough to hold the number of bytes needed
uint8_t encodeRemainingLength(uint32_t remainingLength, uint8_t* destination);

}  // namespace espMqttClientInternals
