/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2026  emsesp.org
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EMSESP_HTTPCLIENT_H
#define EMSESP_HTTPCLIENT_H

#include <ArduinoJson.h>

#include <string>

namespace emsesp {

class HttpClient {
  public:
    static int request(std::string url, const std::string & method, const std::string & value, JsonObjectConst headers, std::string & result);

    // ESP_SSLClient (BearSSL) settings, shared by every TLS client in the project.
    // The RX buffer must hold one full TLS record: servers such as GitHub's CDN send up-to-16 KB
    // records and don't negotiate max_fragment_length, so a smaller buffer can't decode the body
    static constexpr size_t   TLS_RX_BUFFER_SIZE    = 16384;
    static constexpr size_t   TLS_TX_BUFFER_SIZE    = 1024;
    static constexpr uint32_t TLS_SESSION_TIMEOUT_S = 120; // in seconds, the library minimum
    static constexpr uint32_t TLS_READ_TIMEOUT_S    = 5;   // in seconds, not ms like the socket timeouts

  private:
    static constexpr uint32_t CONNECT_TIMEOUT_MS    = 5000;  // TCP connect, the core defaults to 3s
    static constexpr uint32_t FIRST_BYTE_TIMEOUT_MS = 8000;  // how long the server may take to start replying
    static constexpr uint32_t IDLE_TIMEOUT_MS       = 500;   // gap in the stream that marks the end of a response
    static constexpr uint32_t TOTAL_TIMEOUT_MS      = 10000; // ceiling for the whole read

    // ceiling on the response we'll buffer. Without it a chatty or hijacked endpoint can grow the
    // result string until the heap runs out, which aborts the firmware since it's built -fno-exceptions
    static constexpr size_t MAX_RESPONSE_BYTES = 32 * 1024;

    static constexpr size_t MAX_HOSTNAME_LENGTH = 63; // ESP_SSLClient stores the host in a char[64]
};

} // namespace emsesp

#endif
