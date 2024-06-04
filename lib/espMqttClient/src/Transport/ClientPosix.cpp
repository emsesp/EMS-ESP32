/*
Copyright (c) 2022 Bert Melis. All rights reserved.

This work is licensed under the terms of the MIT license.  
For a copy, see <https://opensource.org/licenses/MIT> or
the LICENSE file.
*/

#include "ClientPosix.h"

#if defined(__linux__)

namespace espMqttClientInternals {

ClientPosix::ClientPosix()
: _sockfd(-1)
, _host() {
  // empty
}

ClientPosix::~ClientPosix() {
  ClientPosix::stop();
}

bool ClientPosix::connect(IPAddress ip, uint16_t port) {
  if (connected()) stop();

  _sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
  if (_sockfd < 0) {
    emc_log_e("Error %d opening socket", errno);
  }

  int flag = 1;
  if (setsockopt(_sockfd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(int)) < 0) {
    emc_log_e("Error %d disabling nagle", errno);
  }

  memset(&_host, 0, sizeof(_host));
  _host.sin_family = AF_INET;
  _host.sin_addr.s_addr = htonl(uint32_t(ip));
  _host.sin_port = ::htons(port);

  int ret = ::connect(_sockfd, reinterpret_cast<sockaddr*>(&_host), sizeof(_host));

  if (ret < 0) {
    emc_log_e("Error connecting: %d - (%d) %s", ret, errno, strerror(errno));
    return false;
  }

  emc_log_i("Connected");
  return true;
}

bool ClientPosix::connect(const char* host, uint16_t port) {
  // tbi
  (void) host;
  (void) port;
  return false;
}

size_t ClientPosix::write(const uint8_t* buf, size_t size) {
  return ::send(_sockfd, buf, size, 0);
}

int ClientPosix::read(uint8_t* buf, size_t size) {
  int ret = ::recv(_sockfd, buf, size, MSG_DONTWAIT);
  /*
  if (ret < 0) {
    emc_log_e("Error reading: %s", strerror(errno));
  }
  */
  return ret;
}

void ClientPosix::stop() {
  if (_sockfd >= 0) {
    ::close(_sockfd);
    _sockfd = -1;
  }
}

bool ClientPosix::connected() {
  return _sockfd >= 0;
}

bool ClientPosix::disconnected() {
  return _sockfd < 0;
}

}  // namespace espMqttClientInternals

#endif
