/*
 * uuid-syslog - Syslog service
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

#include "uuid/syslog.h"

#include <Arduino.h>
#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <WiFiUdp.h>

#ifndef UUID_SYSLOG_HAVE_GETTIMEOFDAY
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
// time() does not return UTC on the ESP8266: https://github.com/esp8266/Arduino/issues/4637
#define UUID_SYSLOG_HAVE_GETTIMEOFDAY 1
#endif
#endif

#ifndef UUID_SYSLOG_HAVE_GETTIMEOFDAY
#define UUID_SYSLOG_HAVE_GETTIMEOFDAY 0
#endif

#ifndef UUID_SYSLOG_HAVE_IPADDRESS_TYPE
#if defined(ARDUINO_ARCH_ESP8266)
#define UUID_SYSLOG_HAVE_IPADDRESS_TYPE 1
#endif
#endif

#ifndef UUID_SYSLOG_HAVE_IPADDRESS_TYPE
#define UUID_SYSLOG_HAVE_IPADDRESS_TYPE 0
#endif

#ifndef UUID_SYSLOG_ARP_CHECK
#if defined(LWIP_VERSION_MAJOR) && defined(LWIP_IPV4) && LWIP_VERSION_MAJOR >= 2 && LWIP_IPV4
#define UUID_SYSLOG_ARP_CHECK 1
#endif
#endif

#ifndef UUID_SYSLOG_ARP_CHECK
#define UUID_SYSLOG_ARP_CHECK 0
#endif

#ifndef UUID_SYSLOG_NDP_CHECK
#if defined(LWIP_VERSION_MAJOR) && defined(LWIP_IPV6) && LWIP_VERSION_MAJOR >= 2 && LWIP_IPV6
#define UUID_SYSLOG_NDP_CHECK 1
#endif
#endif

#ifndef UUID_SYSLOG_NDP_CHECK
#define UUID_SYSLOG_NDP_CHECK 0
#endif

#if UUID_SYSLOG_ARP_CHECK or UUID_SYSLOG_NDP_CHECK
#include <lwip/netif.h>
#endif
#if UUID_SYSLOG_ARP_CHECK
#include <lwip/ip4_addr.h>
#include <lwip/etharp.h>
#endif
#if UUID_SYSLOG_NDP_CHECK
#include <lwip/ip6_addr.h>
#include <lwip/nd6.h>
#endif

#include <algorithm>
#include <list>
#include <memory>
#include <string>

#include <uuid/common.h>
#include <uuid/log.h>

static const char __pstr__logger_name[] __attribute__((__aligned__(sizeof(int)))) PROGMEM = "syslog";

namespace uuid {

namespace syslog {

uuid::log::Logger SyslogService::logger_{FPSTR(__pstr__logger_name), uuid::log::Facility::SYSLOG};
bool              SyslogService::QueuedLogMessage::time_good_ = false;

SyslogService::~SyslogService() {
    uuid::log::Logger::unregister_handler(this);
}

void SyslogService::start() {
    uuid::log::Logger::register_handler(this, uuid::log::Level::ALL);
}

uuid::log::Level SyslogService::log_level() const {
    return uuid::log::Logger::get_log_level(this);
}

void SyslogService::remove_queued_messages(uuid::log::Level level) {
    unsigned long offset = 0;

    for (auto it = log_messages_.begin(); it != log_messages_.end();) {
        if (it->content_->level > level) {
            offset++;
            it = log_messages_.erase(it);
        } else {
            it->id_ -= offset;
            it++;
        }
    }

    log_message_id_ -= offset;
}

void SyslogService::log_level(uuid::log::Level level) {
    if (!started_) {
        remove_queued_messages(level);
    }

    static bool level_set     = false;
    bool        level_changed = !level_set || (level != log_level());
    level_set                 = true;

    if (level_changed && level < uuid::log::Level::NOTICE) {
        logger_.info(F("Log level set to %S"), uuid::log::format_level_uppercase(level));
    }
    uuid::log::Logger::register_handler(this, level);
    if (level_changed && level >= uuid::log::Level::NOTICE) {
        logger_.info(F("Log level set to %S"), uuid::log::format_level_uppercase(level));
    }
}

size_t SyslogService::maximum_log_messages() const {
    return maximum_log_messages_;
}

void SyslogService::maximum_log_messages(size_t count) {
    maximum_log_messages_ = std::max((size_t)1, count);

    while (log_messages_.size() > maximum_log_messages_) {
        log_messages_.pop_front();
    }
}

std::pair<IPAddress, uint16_t> SyslogService::destination() const {
    return std::make_pair(host_, port_);
}

void SyslogService::destination(IPAddress host, uint16_t port) {
    host_ = host;
    port_ = port;

    if ((uint32_t)host_ == (uint32_t)0) {
        started_ = false;
        remove_queued_messages(log_level());
    }
}

std::string SyslogService::hostname() const {
    return hostname_;
}

void SyslogService::hostname(std::string hostname) {
    if (hostname.empty() || hostname.find(' ') != std::string::npos) {
        hostname_ = '-';
    } else {
        hostname_ = std::move(hostname);
    }
}

unsigned long SyslogService::mark_interval() const {
    return mark_interval_ / 1000;
}

void SyslogService::mark_interval(unsigned long interval) {
    mark_interval_ = (uint64_t)interval * 1000;
}

SyslogService::QueuedLogMessage::QueuedLogMessage(unsigned long id, std::shared_ptr<uuid::log::Message> && content)
    : id_(id)
    , content_(std::move(content)) {
    if (time_good_ || WiFi.status() == WL_CONNECTED) {
#if UUID_SYSLOG_HAVE_GETTIMEOFDAY
        if (gettimeofday(&time_, nullptr) != 0) {
            time_.tv_sec = (time_t)-1;
        }
#else
        time_.tv_sec  = time(nullptr);
        time_.tv_usec = 0;
#endif

        if (time_.tv_sec >= 0 && time_.tv_sec < 18140 * 86400) {
            time_.tv_sec = (time_t)-1;
        }

        if (time_.tv_sec != (time_t)-1) {
            time_good_ = true;
        }
    } else {
        time_.tv_sec = (time_t)-1;
    }
}

void SyslogService::operator<<(std::shared_ptr<uuid::log::Message> message) {
    if (log_messages_.size() >= maximum_log_messages_) {
        log_messages_overflow_ = true;
        log_messages_.pop_front();
    }

    log_messages_.emplace_back(log_message_id_++, std::move(message));
}

void SyslogService::loop() {
    while (!log_messages_.empty() && can_transmit()) {
        auto message = log_messages_.front();

        started_               = true;
        log_messages_overflow_ = false;
        auto ok                = transmit(message);
        if (ok) {
            // The transmit() may have called yield() allowing
            // other messages to have been added to the queue.
            if (!log_messages_overflow_) {
                log_messages_.pop_front();
            }
            last_message_ = uuid::get_uptime_ms();
        }

        ::yield();

        if (!ok) {
            break;
        }
    }

    if (started_ && mark_interval_ != 0 && log_messages_.empty()) {
        if (uuid::get_uptime_ms() - last_message_ >= mark_interval_) {
            // This is generated manually because the log level may not
            // be high enough to receive INFO messages.
            operator<<(std::make_shared<uuid::log::Message>(uuid::get_uptime_ms(),
                                                            uuid::log::Level::INFO,
                                                            uuid::log::Facility::SYSLOG,
                                                            reinterpret_cast<const __FlashStringHelper *>(__pstr__logger_name),
                                                            uuid::read_flash_string(F("-- MARK --"))));
        }
    }
}

bool SyslogService::can_transmit() {
#if UUID_SYSLOG_HAVE_IPADDRESS_TYPE
    if (host_.isV4() && (uint32_t)host_ == (uint32_t)0) {
        return false;
    }
#else
    if ((uint32_t)host_ == (uint32_t)0) {
        return false;
    }
#endif

    if (WiFi.status() != WL_CONNECTED) {
        return false;
    }

    const uint64_t now           = uuid::get_uptime_ms();
    uint64_t       message_delay = 100;

#if UUID_SYSLOG_ARP_CHECK
#if UUID_SYSLOG_HAVE_IPADDRESS_TYPE
    if (host_.isV4())
#endif
    {
        message_delay = 10;
    }
#endif
#if UUID_SYSLOG_NDP_CHECK && UUID_SYSLOG_HAVE_IPADDRESS_TYPE
    if (host_.isV6()) {
        message_delay = 10;
    }
#endif

    if (now < last_transmit_ || now - last_transmit_ < message_delay) {
        return false;
    }

#if UUID_SYSLOG_ARP_CHECK
#if UUID_SYSLOG_HAVE_IPADDRESS_TYPE
    if (host_.isV4())
#endif
    {
        ip4_addr_t ipaddr;

        ip4_addr_set_u32(&ipaddr, (uint32_t)host_);

        if (!ip4_addr_isloopback(&ipaddr) && !ip4_addr_ismulticast(&ipaddr) && !ip4_addr_isbroadcast(&ipaddr, netif_default)) {
            struct eth_addr *  eth_ret = nullptr;
            const ip4_addr_t * ip_ret  = nullptr;

            if (!ip4_addr_netcmp(&ipaddr, netif_ip4_addr(netif_default), netif_ip4_netmask(netif_default))) {
                // Replace addresses outside the network with the gateway address
                const ip4_addr_t * gw_addr = netif_ip4_gw(netif_default);

                if (gw_addr != nullptr) {
                    ipaddr = *gw_addr;
                }
            }

            if (etharp_find_addr(netif_default, &ipaddr, &eth_ret, &ip_ret) == -1) {
                etharp_query(netif_default, &ipaddr, NULL);
                // Avoid querying lwIP again for 1 second
                last_transmit_ = uuid::get_uptime_ms() + (uint64_t)1000 - message_delay;

                return false;
            }
        }
    }
#endif

#if UUID_SYSLOG_NDP_CHECK && UUID_SYSLOG_HAVE_IPADDRESS_TYPE
    if (host_.isV6()) {
        ip6_addr_t ip6addr;

        IP6_ADDR(&ip6addr, host_.raw6()[0], host_.raw6()[1], host_.raw6()[2], host_.raw6()[3]);
        ip6_addr_assign_zone(&ip6addr, IP6_UNICAST, netif_default);

        if (!ip6_addr_isloopback(&ip6addr) && !ip6_addr_ismulticast(&ip6addr)) {
            // Don't send to a scoped address until we have a valid address of the same type
            bool         have_address = false;
            const u8_t * hwaddr       = nullptr;

            for (size_t i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
                if (ip6_addr_isvalid(netif_ip6_addr_state(netif_default, i))) {
                    if (ip6_addr_isglobal(&ip6addr)) {
                        if (ip6_addr_isglobal(netif_ip6_addr(netif_default, i))) {
                            have_address = true;
                            break;
                        }
                    } else if (ip6_addr_issitelocal(&ip6addr)) {
                        if (ip6_addr_issitelocal(netif_ip6_addr(netif_default, i))) {
                            have_address = true;
                            break;
                        }
                    } else if (ip6_addr_isuniquelocal(&ip6addr)) {
                        if (ip6_addr_isuniquelocal(netif_ip6_addr(netif_default, i))) {
                            have_address = true;
                            break;
                        }
                    } else if (ip6_addr_islinklocal(&ip6addr)) {
                        if (ip6_addr_islinklocal(netif_ip6_addr(netif_default, i))) {
                            have_address = true;
                            break;
                        }
                    } else {
                        have_address = true;
                        break;
                    }
                }
            }

            if (!have_address) {
                // Avoid checking lwIP again for 1 second
                last_transmit_ = uuid::get_uptime_ms() + (uint64_t)1000 - message_delay;

                return false;
            } else if (nd6_get_next_hop_addr_or_queue(netif_default, NULL, &ip6addr, &hwaddr) != ERR_OK) {
                // Avoid querying lwIP again for 1 second
                last_transmit_ = uuid::get_uptime_ms() + (uint64_t)1000 - message_delay;

                return false;
            }
        }
    }
#endif

    return true;
}

bool SyslogService::transmit(const QueuedLogMessage & message) {
    /*
    // modifications by Proddy. From https://github.com/proddy/EMS-ESP/issues/395#issuecomment-640053528
    struct tm tm;

    tm.tm_year = 0;
    if (message.time_.tv_sec != (time_t)-1) {
        gmtime_r(&message.time_.tv_sec, &tm);
    }
    */

    if (udp_.beginPacket(host_, port_) != 1) {
        last_transmit_ = uuid::get_uptime_ms();
        return false;
    }

    udp_.printf_P(PSTR("<%u>1 "), ((unsigned int)message.content_->facility * 8) + std::min(7U, (unsigned int)message.content_->level));

    /*
    if (tm.tm_year != 0) {
        udp_.printf_P(PSTR("%04u-%02u-%02uT%02u:%02u:%02u.%06luZ"),
                      tm.tm_year + 1900,
                      tm.tm_mon + 1,
                      tm.tm_mday,
                      tm.tm_hour,
                      tm.tm_min,
                      tm.tm_sec,
                      (unsigned long)message.time_.tv_usec);
    } else {
        udp_.print('-');
    }
    */

    udp_.print('-');
    udp_.printf_P(PSTR(" %s - - - - \xEF\xBB\xBF"), hostname_.c_str());
    udp_.print(uuid::log::format_timestamp_ms(message.content_->uptime_ms, 3).c_str());

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
    udp_.printf_P(PSTR(" %c %lu: [%S] "), uuid::log::format_level_char(message.content_->level), message.id_, message.content_->name);
#pragma GCC diagnostic pop
    udp_.print(message.content_->text.c_str());
    bool ok = (udp_.endPacket() == 1);

    last_transmit_ = uuid::get_uptime_ms();
    return ok;
}

} // namespace syslog

} // namespace uuid
