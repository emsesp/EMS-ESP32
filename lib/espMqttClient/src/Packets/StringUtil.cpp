/*
Copyright (c) 2022 Bert Melis. All rights reserved.

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#include "StringUtil.h"

namespace espMqttClientInternals {

size_t encodeString(const char* source, uint8_t* dest) {
  size_t length = strlen(source);
  if (length > 65535) {
    emc_log_e("String length error");
    return 0;
  }

  dest[0] = static_cast<uint16_t>(length) >> 8;
  dest[1] = static_cast<uint16_t>(length) & 0xFF;
  memcpy(&dest[2], source, length);
  return 2 + length;
}

}  // namespace espMqttClientInternals
