/*
Copyright (c) 2022 Bert Melis. All rights reserved.

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#pragma once

#include <stddef.h>  // size_t

#include "ClientPosixIPAddress.h"

namespace espMqttClientInternals {

class Transport {
 public:
  virtual bool connect(IPAddress ip, uint16_t port) = 0;
  virtual bool connect(const char* host, uint16_t port) = 0;
  virtual size_t write(const uint8_t* buf, size_t size) = 0;
  virtual int read(uint8_t* buf, size_t size) = 0;
  virtual void stop() = 0;
  virtual bool connected() = 0;
  virtual bool disconnected() = 0;
};

}  // namespace espMqttClientInternals
