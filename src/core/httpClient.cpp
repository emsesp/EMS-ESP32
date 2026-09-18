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

#include "emsesp.h"

#include "httpClient.h"

#include <WiFiClient.h>
#include <ESP_SSLClient.h>

namespace emsesp {

int HttpClient::request(std::string url, const std::string & method, const std::string & value, JsonObjectConst headers, std::string & result) {
    int        httpResult = 0;
    const bool is_post    = value.length() || Helpers::toLower(method) == "post";
    const auto lower_url  = Helpers::toLower(url.c_str());

    const bool is_https = lower_url.starts_with("https://");
    if (!is_https && !lower_url.starts_with("http://")) {
        return 0; // unsupported scheme
    }

    // split the host from the path before anything is allocated, so a bad URL costs nothing
    url.replace(0, is_https ? 8 : 7, "");
    std::string host  = url;
    auto        index = url.find_first_of('/');
    if (index != std::string::npos) {
        host = url.substr(0, index);
        url.replace(0, index, "");
    } else {
        url = "/";
    }

    if (host.empty()) {
        return 0;
    }

    // ESP_SSLClient keeps the host in a fixed char[64] and strcpy()s into it without a length
    // check, so a longer name would corrupt whatever follows it and crash somewhere unrelated
    if (host.length() > MAX_HOSTNAME_LENGTH) {
        EMSESP::logger().warning("%s hostname is too long (%u characters, max is %u)",
                                 is_https ? "HTTPS" : "HTTP",
                                 (unsigned)host.length(),
                                 (unsigned)MAX_HOSTNAME_LENGTH);
        return 0;
    }

    WiFiClient *    basic_client = new WiFiClient;
    ESP_SSLClient * ssl_client   = new ESP_SSLClient;
    if (is_https) {
        ssl_client->setInsecure();
        // Notes: with root CA we should set here: ssl_client->setCACert(rootCACert);
        // 1 KB RX buffer is fine for small JSON-style endpoints used by the scheduler/shunting-yard but it is NOT enough for servers that send full-size TLS records (>1 KB)
        ssl_client->setBufferSizes(16384, 1024);
        ssl_client->setSessionTimeout(120); // Set the timeout in seconds (>=120 seconds)
    }
    // WiFiClient is NetworkClient, which declares no setTimeout() of its own - calling it binds to
    // Stream::setTimeout() and only affects readBytes(), leaving the socket on the core's 3s default
    basic_client->setConnectionTimeout(CONNECT_TIMEOUT_MS);
    ssl_client->setTimeout(5);                     // seconds, drives BearSSL only - unused on the plain HTTP path
    ssl_client->setClient(basic_client, is_https); // enableSSL = false for plain HTTP

    const uint16_t port = is_https ? 443 : 80;
    if (ssl_client->connect(host.c_str(), port)) {
        bool content_set = false;
        bool agent_set   = false;

        // assemble the request in one buffer. Sent as a dozen small writes it is left to Nagle to
        // dribble them out an ACK at a time, which on a slow link can cost more than the read budget
        std::string req = (is_post ? "POST " : "GET ") + url + " HTTP/1.1\r\nHost: " + host + "\r\n";
        for (JsonPairConst p : headers) {
            const auto key = Helpers::toLower(p.key().c_str());
            content_set |= (key == "content-type");
            agent_set |= (key == "user-agent");
            req += std::string(p.key().c_str()) + ": " + p.value().as<std::string>() + "\r\n";
        }
        if (!agent_set) {
            req += "User-Agent: EMS-ESP\r\n"; // CDNs are more likely to stall on a request without one
        }
        if (is_post) {
            if (!content_set) {
                req += "Content-Type: ";
                req += value.starts_with('{') ? asyncsrv::T_application_json : asyncsrv::T_text_plain;
                req += "\r\n";
            }
            req += "Content-Length: " + std::to_string(value.length()) + "\r\n";
        }
        req += "Connection: close\r\n\r\n"; // the blank line terminates the headers - without it the server never responds

        ssl_client->print(req.c_str());
        if (is_post && value.length()) {
            ssl_client->print(value.c_str()); // sent separately so the body isn't copied into req
        }

        // available() drops to zero between TCP segments, so stopping at the first gap truncates
        // any response that doesn't arrive in a single packet. Keep reading until the peer closes,
        // the stream goes idle, or the overall budget runs out
        const uint32_t started   = millis();
        uint32_t       last_data = started;
        bool           truncated = false;
        while (millis() - started < TOTAL_TIMEOUT_MS) {
            const int avail = ssl_client->available();
            if (avail > 0) {
                if (result.length() >= MAX_RESPONSE_BYTES) {
                    truncated = true;
                    break;
                }
                uint8_t      buf[128];
                const size_t want = (avail < (int)sizeof(buf)) ? (size_t)avail : sizeof(buf);
                const int    len  = ssl_client->read(buf, want);
                if (len > 0) {
                    result.append(reinterpret_cast<const char *>(buf), len);
                    last_data = millis();
                    continue;
                }
                // available() reported bytes the client won't hand over. Yielding here keeps a
                // stale count from busy-spinning the whole read budget on this task's core
                delay(1);
                continue;
            }
            if (!ssl_client->connected()) {
                break; // closed, with nothing left buffered
            }
            // the server may take a while to start replying
            if (millis() - last_data > (result.empty() ? FIRST_BYTE_TIMEOUT_MS : IDLE_TIMEOUT_MS)) {
                break;
            }
            delay(1);
        }
        ssl_client->stop();

        const auto received = result.length();
        if (truncated) {
            EMSESP::logger().warning("%s response from %s truncated at %u bytes", is_https ? "HTTPS" : "HTTP", host.c_str(), (unsigned)received);
        }

        // parse the status line "HTTP/1.x <code> <reason>". stoi() would abort rather than throw on
        // a malformed response, since the firmware is built with -fno-exceptions
        if (result.starts_with("HTTP/")) {
            index = result.find_first_of(' ');
            if (index != std::string::npos) {
                httpResult = Helpers::atoint(result.c_str() + index + 1);
            }
        }

        index = result.find("\r\n\r\n");
        if (index != std::string::npos) {
            result.replace(0, index + 4, "");
        }

        if (httpResult == 0) {
            // the TCP connect worked but nothing usable came back
            EMSESP::logger().warning("%s no valid response from %s (%u bytes)", is_https ? "HTTPS" : "HTTP", host.c_str(), (unsigned)received);
        }
    } else {
        EMSESP::logger().warning("%s connection to %s failed", is_https ? "HTTPS" : "HTTP", host.c_str());
    }
    delete ssl_client;
    delete basic_client;

    return httpResult;
}

} // namespace emsesp
