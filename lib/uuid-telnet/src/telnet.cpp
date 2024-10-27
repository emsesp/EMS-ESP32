/*
 * uuid-telnet - Telnet service
 * Copyright 2019  Simon Arlott
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

#include "uuid/telnet.h"

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#include <algorithm>
#include <list>
#include <memory>
#include <string>

#include <uuid/common.h>
#include <uuid/log.h>

#ifndef UUID_TELNET_HAVE_WIFICLIENT_REMOTE
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
#define UUID_TELNET_HAVE_WIFICLIENT_REMOTE 1
#else
#define UUID_TELNET_HAVE_WIFICLIENT_REMOTE 0
#endif
#endif

#ifndef UUID_TELNET_HAVE_WIFICLIENT_NODELAY
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
#define UUID_TELNET_HAVE_WIFICLIENT_NODELAY 1
#else
#define UUID_TELNET_HAVE_WIFICLIENT_NODELAY 0
#endif
#endif

#ifndef UUID_TELNET_HAVE_WIFICLIENT_KEEPALIVE
#if defined(ARDUINO_ARCH_ESP8266)
#define UUID_TELNET_HAVE_WIFICLIENT_KEEPALIVE 1
#else
#define UUID_TELNET_HAVE_WIFICLIENT_KEEPALIVE 0
#endif
#endif

namespace uuid {

namespace telnet {

uuid::log::Logger TelnetService::logger_{"telnet", uuid::log::Facility::DAEMON};

TelnetService::TelnetService(std::shared_ptr<uuid::console::Commands> commands, unsigned int context, unsigned int flags)
    : TelnetService(DEFAULT_PORT, commands, context, flags) {
}

TelnetService::TelnetService(uint16_t port, std::shared_ptr<uuid::console::Commands> commands, unsigned int context, unsigned int flags)
    : TelnetService(port,
                    [commands, context, flags](Stream & stream, const IPAddress & addr __attribute__((unused)), uint16_t port __attribute__((unused)))
                        -> std::shared_ptr<uuid::console::Shell> { return std::make_shared<uuid::console::Shell>(stream, commands, context, flags); }) {
}

TelnetService::TelnetService(shell_factory_function shell_factory)
    : TelnetService(DEFAULT_PORT, shell_factory) {
}

TelnetService::TelnetService(uint16_t port, shell_factory_function shell_factory)
    : server_(port)
    , shell_factory_(shell_factory) {
}

void TelnetService::start() {
    server_.begin();
}

void TelnetService::close_all() {
    while (!connections_.empty()) {
        connections_.front().stop();
        connections_.pop_front();
    }
}

void TelnetService::stop() {
    server_.stop();
}

size_t TelnetService::maximum_connections() const {
    return maximum_connections_;
}

void TelnetService::maximum_connections(size_t count) {
    maximum_connections_ = std::max((size_t)1, count);

    if (connections_.size() > maximum_connections_) {
        size_t stop = connections_.size() - maximum_connections_;

        for (auto it = connections_.begin(); it != connections_.end();) {
            if (it->stop()) {
                if (--stop == 0)
                    break;
            }
        }
    }
}

unsigned long TelnetService::initial_idle_timeout() const {
    return initial_idle_timeout_;
}

void TelnetService::initial_idle_timeout(unsigned long timeout) {
    initial_idle_timeout_ = timeout;
}

unsigned long TelnetService::default_write_timeout() const {
    return write_timeout_;
}

void TelnetService::default_write_timeout(unsigned long timeout) {
    write_timeout_ = timeout;
}

void TelnetService::loop() {
    for (auto it = connections_.begin(); it != connections_.end();) {
        if (!it->loop()) {
            it = connections_.erase(it);
        } else {
            it++;
        }
    }

    WiFiClient client = server_.accept();
    if (client) {
        if (connections_.size() >= maximum_connections_) {
#if UUID_TELNET_HAVE_WIFICLIENT_REMOTE
            logger_.info("New connection from [%s]:%u rejected (connection limit reached)",
                         uuid::printable_to_string(client.remoteIP()).c_str(),
                         client.remotePort());
#else
            logger_.info("New connection rejected (connection limit reached)");
#endif
            client.println("Maximum connection limit reached");
            client.stop();
        } else {
#if UUID_TELNET_HAVE_WIFICLIENT_REMOTE
            logger_.info("New connection from [%s]:%u accepted", uuid::printable_to_string(client.remoteIP()).c_str(), client.remotePort());
#endif
            connections_.emplace_back(shell_factory_, std::move(client), initial_idle_timeout_, write_timeout_);
#if !(UUID_TELNET_HAVE_WIFICLIENT_REMOTE)
            logger_.info("New connection %p accepted", &connections_.back());
#endif
        }
    }
}

TelnetService::Connection::Connection(shell_factory_function & shell_factory, WiFiClient && client, unsigned long idle_timeout, unsigned long write_timeout)
    : client_(std::move(client))
    , stream_(client_) {
#if UUID_TELNET_HAVE_WIFICLIENT_REMOTE
    // These have to be copied because they're not accessible on closed connections
    addr_ = client_.remoteIP();
    port_ = client_.remotePort();
#else
    port_ = 0;
#endif

#if UUID_TELNET_HAVE_WIFICLIENT_NODELAY
    client_.setNoDelay(true);
#endif

#if UUID_TELNET_HAVE_WIFICLIENT_KEEPALIVE
    // Disconnect after 30 seconds without a response
    client_.keepAlive(5, 5, 5);
#endif

    if (write_timeout > 0) {
        client_.setTimeout(write_timeout);
    }

    stream_.start();

    if (client_.connected()) {
        std::shared_ptr<uuid::console::Shell> shell = shell_factory(stream_, addr_, port_);
        shell->idle_timeout(idle_timeout);
        shell->start();
        shell_ = shell;
    }
}

bool TelnetService::Connection::loop() {
    if (!shell_.expired()) {
        if (!client_.connected()) {
            auto shell = shell_.lock();

            if (shell) {
                shell->stop();
            }
        }
        return true;
    } else {
#if UUID_TELNET_HAVE_WIFICLIENT_REMOTE
        logger_.info("Connection from [%s]:%u closed", uuid::printable_to_string(addr_).c_str(), port_);
#else
        logger_.info("Connection %p closed", this);
#endif
        return false;
    }
}

bool TelnetService::Connection::stop() {
    auto shell = shell_.lock();

    if (shell) {
        shell->stop();
        return true;
    } else {
        return false;
    }
}

} // namespace telnet

} // namespace uuid
