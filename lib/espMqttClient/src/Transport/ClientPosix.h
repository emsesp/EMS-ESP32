/*
Copyright (c) 2022 Bert Melis. All rights reserved.

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#pragma once

#if defined(__linux__)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>

#include "Transport.h"  // includes IPAddress
#include "../Logging.h"

#ifndef EMC_POSIX_PEEK_SIZE
#define EMC_POSIX_PEEK_SIZE 1500
#endif

namespace espMqttClientInternals {

class ClientPosix : public Transport {
 public:
  ClientPosix();
  ~ClientPosix();
  bool connect(IPAddress ip, uint16_t port) override;
  bool connect(const char* host, uint16_t port) override;
  size_t write(const uint8_t* buf, size_t size) override;
  int read(uint8_t* buf, size_t size) override;
  void stop() override;
  bool connected() override;
  bool disconnected() override;

 protected:
  int _sockfd;
  struct sockaddr_in _host;
};

}  // namespace espMqttClientInternals

#endif
