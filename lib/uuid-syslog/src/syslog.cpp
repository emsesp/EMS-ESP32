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
#if UUID_SYSLOG_THREAD_SAFE
#include <mutex>
#endif
#include <string>

#include <uuid/common.h>
#include <uuid/log.h>

#ifndef UUID_SYSLOG_UDP_BASE_MESSAGE_DELAY
#define UUID_SYSLOG_UDP_BASE_MESSAGE_DELAY 100
#endif

#ifndef UUID_SYSLOG_UDP_IPV4_ARP_MESSAGE_DELAY
#define UUID_SYSLOG_UDP_IPV4_ARP_MESSAGE_DELAY 10
#endif

#ifndef UUID_SYSLOG_UDP_IPV6_NDP_MESSAGE_DELAY
#define UUID_SYSLOG_UDP_IPV6_NDP_MESSAGE_DELAY 10
#endif

static const char __pstr__logger_name[] = "syslog";

namespace uuid {

namespace syslog {

uuid::log::Logger SyslogService::logger_{__pstr__logger_name, uuid::log::Facility::SYSLOG};
bool              SyslogService::QueuedLogMessage::time_good_ = false;

void SyslogService::start() {
    uuid::log::Logger::register_handler(this, uuid::log::Level::ALL);
}

uuid::log::Level SyslogService::log_level() const {
    return uuid::log::Logger::get_log_level(this);
}

void SyslogService::remove_queued_messages(uuid::log::Level level) {
#if UUID_SYSLOG_THREAD_SAFE
    std::lock_guard<std::mutex> lock{mutex_};
#endif
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
    log_message_fails_ += offset;
}

void SyslogService::log_level(uuid::log::Level level) {
    if (!started_) {
        remove_queued_messages(level);
    }

    // commented out for EMS-ESP

    /*
    static bool level_set = false;
    level_set             = true;
    bool level_changed = !level_set || (level != log_level());
    
    if (level_changed) {
         logger_.info("Log level set to %S", uuid::log::format_level_uppercase(level));
    }
    */

    uuid::log::Logger::register_handler(this, level);
}

size_t SyslogService::maximum_log_messages() const {
#if UUID_SYSLOG_THREAD_SAFE
    std::lock_guard<std::mutex> lock{mutex_};
#endif
    return maximum_log_messages_;
}

void SyslogService::maximum_log_messages(size_t count) {
#if UUID_SYSLOG_THREAD_SAFE
    std::lock_guard<std::mutex> lock{mutex_};
#endif
    maximum_log_messages_ = std::max((size_t)1, count);

    while (log_messages_.size() > maximum_log_messages_) {
        log_messages_.pop_front();
    }
}

size_t SyslogService::current_log_messages() const {
#if UUID_SYSLOG_THREAD_SAFE
    std::lock_guard<std::mutex> lock{mutex_};
#endif

    return log_messages_.size();
}

std::pair<IPAddress, uint16_t> SyslogService::destination() const {
    return std::make_pair(ip_, port_);
}

void SyslogService::destination(IPAddress ip, uint16_t port) {
    ip_   = ip;
    port_ = port;

    if ((uint32_t)ip_ == (uint32_t)0) {
        started_ = false;
        remove_queued_messages(log_level());
        host_.clear();
    }
}

void SyslogService::destination(const char * host, uint16_t port) {
    if (host == nullptr || host[0] == '\0') {
        started_ = false;
        remove_queued_messages(log_level());
        ip_ = (IPAddress)(uint32_t)0;
        host_.clear();
        return;
    }
    host_ = host;
    port_ = port;
    if (ip_.fromString(host)) {
        host_.clear();
        if ((uint32_t)ip_ == (uint32_t)0) {
            started_ = false;
            remove_queued_messages(log_level());
        }
    } else {
        ip_ = (IPAddress)(uint32_t)0;
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
    // Added for EMS-ESP
    // if (time_good_ || emsesp::EMSESP::system_.network_connected()) {
    if (time_good_) {
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

/* Mutex already locked by caller. */
void SyslogService::add_message(std::shared_ptr<uuid::log::Message> & message) {
    if (log_messages_.size() >= maximum_log_messages_) {
        log_messages_.pop_front();
        log_message_fails_++;
    }

    log_messages_.emplace_back(log_message_id_++, std::move(message));
}

void SyslogService::operator<<(std::shared_ptr<uuid::log::Message> message) {
#if UUID_SYSLOG_THREAD_SAFE
    std::lock_guard<std::mutex> lock{mutex_};
#endif

    add_message(message);
}

void SyslogService::loop() {
#if UUID_SYSLOG_THREAD_SAFE
    std::unique_lock<std::mutex> lock{mutex_};
#endif
    size_t count = std::max((size_t)1, MAX_LOG_MESSAGES);

    while (!log_messages_.empty()) {
#if UUID_SYSLOG_THREAD_SAFE
        lock.unlock();
#endif

        if (!can_transmit())
            return;

#if UUID_SYSLOG_THREAD_SAFE
        lock.lock();

        if (log_messages_.empty())
            break;
#endif

        auto message = log_messages_.front();

        started_ = true;

#if UUID_SYSLOG_THREAD_SAFE
        lock.unlock();
#endif

        auto ok = transmit(message);
        if (ok) {
#if UUID_SYSLOG_THREAD_SAFE
            lock.lock();
#endif

            last_message_ = last_transmit_;
            if (!log_messages_.empty() && log_messages_.front().content_ == message.content_) {
                log_messages_.pop_front();
            }

#if UUID_SYSLOG_THREAD_SAFE
            lock.unlock();
#endif
        }

        ::yield();

#if UUID_SYSLOG_THREAD_SAFE
        lock.lock();
#endif

        if (!ok) {
            break;
        }

        count--;
        if (count == 0) {
            break;
        }
    }

    if (started_ && mark_interval_ != 0 && log_messages_.empty()) {
        if (uuid::get_uptime_ms() - last_message_ >= mark_interval_) {
            // This is generated manually because the log level may not
            // be high enough to receive INFO messages.
            auto message = std::make_shared<uuid::log::Message>(uuid::get_uptime_ms(),
                                                                uuid::log::Level::INFO,
                                                                uuid::log::Facility::SYSLOG,
                                                                (__pstr__logger_name),
                                                                "-- MARK --");
            add_message(message);
        }
    }
}

bool SyslogService::can_transmit() {
    // TODO this should be checked also for Eth
    if (!host_.empty() && (uint32_t)ip_ == 0) {
        WiFi.hostByName(host_.c_str(), ip_);
    }

#if UUID_SYSLOG_HAVE_IPADDRESS_TYPE
    if (ip_.isV4() && (uint32_t)ip_ == (uint32_t)0) {
        return false;
    }
#else
    if ((uint32_t)ip_ == (uint32_t)0) {
        return false;
    }
#endif

    // if (!emsesp::EMSESP::system_.network_connected()) {
    //     return false;
    // }

    const uint64_t now           = uuid::get_uptime_ms();
    uint64_t       message_delay = UUID_SYSLOG_UDP_BASE_MESSAGE_DELAY;

#if UUID_SYSLOG_ARP_CHECK
#if UUID_SYSLOG_HAVE_IPADDRESS_TYPE
    if (ip_.isV4())
#endif
    {
        message_delay = UUID_SYSLOG_UDP_IPV4_ARP_MESSAGE_DELAY;
    }
#endif
#if UUID_SYSLOG_NDP_CHECK && UUID_SYSLOG_HAVE_IPADDRESS_TYPE
    if (ip_.isV6()) {
        message_delay = UUID_SYSLOG_UDP_IPV6_NDP_MESSAGE_DELAY;
    }
#endif

    if (started_ && (now < last_transmit_ || now - last_transmit_ < message_delay)) {
        return false;
    }

#if UUID_SYSLOG_ARP_CHECK
#if UUID_SYSLOG_HAVE_IPADDRESS_TYPE
    if (ip_.isV4())
#endif
    {
        ip4_addr_t ipaddr;

        ip4_addr_set_u32(&ipaddr, (uint32_t)ip_);

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
    if (ip_.isV6()) {
        ip6_addr_t ip6addr;

        IP6_ADDR(&ip6addr, ip_.raw6()[0], ip_.raw6()[1], ip_.raw6()[2], ip_.raw6()[3]);
        ip6_addr_assign_zone(&ip6addr, IP6_UNICAST, netif_default);

        if (!ip6_addr_isloopback(&ip6addr) && !ip6_addr_ismulticast(&ip6addr)) {
            // Don't send to a scoped address until we have a valid address of the same type
            bool         have_address = false;
            const u8_t * hwaddr       = nullptr;

            for (size_t i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
                if (ip6_addr_isvalid(netif_ip6_addr_state(netif_default, i))) {
                    if (ip6_addr_islinklocal(&ip6addr)) {
                        if (ip6_addr_islinklocal(netif_ip6_addr(netif_default, i))) {
                            have_address = true;
                            break;
                        }
                    } else if (ip6_addr_isglobal(&ip6addr) || ip6_addr_isuniquelocal(&ip6addr) || ip6_addr_issitelocal(&ip6addr)) {
                        if (ip6_addr_isglobal(netif_ip6_addr(netif_default, i)) || ip6_addr_isuniquelocal(netif_ip6_addr(netif_default, i))
                            || ip6_addr_issitelocal(netif_ip6_addr(netif_default, i))) {
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
    struct tm tm;

    // Changes for EMS-ESP
    int8_t tzh = 0;
    int8_t tzm = 0;

    tm.tm_year = 0;
    if (message.time_.tv_sec != (time_t)-1) {
        struct tm utc;
        gmtime_r(&message.time_.tv_sec, &utc);
        localtime_r(&message.time_.tv_sec, &tm);
        int16_t diff = 60 * (tm.tm_hour - utc.tm_hour) + tm.tm_min - utc.tm_min;
        diff         = diff > 720 ? diff - 1440 : diff < -720 ? diff + 1440 : diff;

        // added for EMS-ESP
        tzh = diff / 60;
        tzm = diff < 0 ? (0 - diff) % 60 : diff % 60;
    }

    if (udp_.beginPacket(ip_, port_) != 1) {
        last_transmit_ = uuid::get_uptime_ms();
        return false;
    }

    /*
	 * The level is constrained to 0-7 by design in RFC 5424 because higher
	 * severity values would be a different severity in another facility. The
	 * TRACE level and all other invalid values (because of the conversion to
	 * unsigned) are converted to DEBUG.
	 *
	 * RFC 5424 requires that the facility MUST be 0-23. Values here are allowed
	 * to go up to 31 because there is no obvious candidate to convert them to
	 * and because the multiplication by a factor of 256 means that (with the
	 * cast back to uint8_t) higher values just overflow into other facilities
	 * (which is easier than doing it by modulo 24). The enum doesn't allow
	 * values that are out of range of the RFC.
	 *
	 * The maximum possible priority value does not exceed the requirement that
	 * the PRI part MUST be 3-5 characters.
	 */
    udp_.printf("<%u>1 ", (uint8_t)(message.content_->facility * 8U) + std::min(7U, (unsigned int)message.content_->level));

    if (tm.tm_year != 0) {
        // udp_.printf_P("%04u-%02u-%02uT%02u:%02u:%02u.%06luZ",
        //               tm.tm_year + 1900,
        //               tm.tm_mon + 1,
        //               tm.tm_mday,
        //               tm.tm_hour,
        //               tm.tm_min,
        //               tm.tm_sec,
        //               (unsigned long)message.time_.tv_usec);

        // added for EMS-ESP
        udp_.printf("%04u-%02u-%02uT%02u:%02u:%02u.%06lu%+02d:%02d",
                    tm.tm_year + 1900,
                    tm.tm_mon + 1,
                    tm.tm_mday,
                    tm.tm_hour,
                    tm.tm_min,
                    tm.tm_sec,
                    (unsigned long)message.time_.tv_usec,
                    tzh,
                    tzm);
    } else {
        udp_.print('-');
    }

    udp_.printf(" %s %s - - - ", hostname_.c_str(), message.content_->name);

    char id_c_str[15];
    snprintf(id_c_str, sizeof(id_c_str), " %lu: ", message.id_);
    std::string msgstr = uuid::log::format_timestamp_ms(message.content_->uptime_ms, 3) + ' ' + uuid::log::format_level_char(message.content_->level) + id_c_str
                         + message.content_->text;
    for (uint16_t i = 0; i < msgstr.length(); i++) {
        if (msgstr.at(i) & 0x80) {
            udp_.print("\xEF\xBB\xBF");
            // udp_.print("<BOM>"); // marker for testing if BOM is created for udf-8
            break;
        }
    }
    udp_.print(msgstr.c_str());
    bool ok = (udp_.endPacket() == 1);

    last_transmit_ = uuid::get_uptime_ms();
    return ok;
}

} // namespace syslog

} // namespace uuid
