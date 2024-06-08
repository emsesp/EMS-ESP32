/*
Copyright (c) 2022 Bert Melis. All rights reserved.

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#pragma once

#if defined(ARDUINO)
  #include <IPAddress.h>
#else

#include <stdint.h>

class IPAddress {
 public:
  IPAddress();
  IPAddress(uint8_t p0, uint8_t p1, uint8_t p2, uint8_t p3);
  explicit IPAddress(uint32_t address);
  operator uint32_t();

 protected:
  uint32_t _address;
};

#endif
