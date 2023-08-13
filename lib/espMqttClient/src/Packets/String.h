/*
Copyright (c) 2022 Bert Melis. All rights reserved.

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#pragma once

#include <stdint.h>
#include <cstring>  // memcpy

#include "../Logging.h"

namespace espMqttClientInternals {

// encodes the given source string into destination and returns number of bytes used
// destination is expected to be large enough to hold the number of bytes needed
size_t encodeString(const char* source, uint8_t* dest);

}  // namespace espMqttClientInternals
