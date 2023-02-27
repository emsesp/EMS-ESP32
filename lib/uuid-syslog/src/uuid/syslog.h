/*
 * uuid-syslog - Microcontroller syslog service
 * Copyright 2019,2021-2022  Simon Arlott
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

#ifndef UUID_SYSLOG_H_
#define UUID_SYSLOG_H_

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <time.h>

#include <list>
#include <memory>
#include <string>

#include <uuid/log.h>

#ifndef UUID_LOG_THREAD_SAFE
#define UUID_LOG_THREAD_SAFE 0
#endif

#ifndef UUID_COMMON_STD_MUTEX_AVAILABLE
#define UUID_COMMON_STD_MUTEX_AVAILABLE 0
#endif

#if UUID_COMMON_STD_MUTEX_AVAILABLE
#define UUID_SYSLOG_THREAD_SAFE 1
#else
#define UUID_SYSLOG_THREAD_SAFE 0
#endif

#if UUID_LOG_THREAD_SAFE
#include <mutex>
#endif

namespace uuid {

/**
 * Syslog service.
 *
 * - <a href="https://github.com/nomis/mcu-uuid-syslog/">Git Repository</a>
 * - <a href="https://mcu-uuid-syslog.readthedocs.io/">Documentation</a>
 */
namespace syslog {

/**
 * Thread-safe status of the library.
 *
 * @since 2.2.0
 */
#if UUID_COMMON_THREAD_SAFE && UUID_LOG_THREAD_SAFE && UUID_SYSLOG_THREAD_SAFE
static constexpr bool thread_safe = true;
#else
static constexpr bool thread_safe = false;
#endif

/**
 * Log handler for sending messages to a syslog server.
 *
 * @since 1.0.0
 */
class SyslogService : public uuid::log::Handler {
  public:
    static constexpr size_t   MAX_LOG_MESSAGES = 50;  /*!< Maximum number of log messages to buffer before they are output. @since 1.0.0 */
    static constexpr uint16_t DEFAULT_PORT     = 514; /*!< Default UDP port to send messages to. @since 1.0.0 */

    /**
	 * Create a new syslog service log handler.
	 *
	 * @since 1.0.0
	 */
    SyslogService()  = default;
    ~SyslogService() = default;

    /**
	 * Register the log handler with the logging framework.
	 *
	 * @since 1.0.0
	 */
    void start();

    /**
	 * Get the current log level.
	 *
	 * This only affects newly received log messages, not messages that
	 * have already been queued.
	 *
	 * @return The current log level.
	 * @since 2.0.0
	 */
    uuid::log::Level log_level() const;
    /**
	 * Set the current log level.
	 *
	 * Unless this is the first time the log level is being set, this
	 * only affects newly received log messages, not messages that have
	 * already been queued.
	 *
	 * @param[in] level Minimum log level that will be sent to the
	 *                  syslog server.
	 * @since 2.0.0
	 */
    void log_level(uuid::log::Level level);

    /**
	 * Get the maximum number of queued log messages.
	 *
	 * @return The maximum number of queued log messages.
	 * @since 2.0.0
	 */
    size_t maximum_log_messages() const;
    /**
	 * Set the maximum number of queued log messages.
	 *
	 * Defaults to SyslogService::MAX_LOG_MESSAGES.
	 *
	 * @since 2.0.0
	 */
    void maximum_log_messages(size_t count);

    /**
	 * Get the current number of queued log messages.
	 *
	 * @return The current number of queued log messages.
	 * @since 2.1.0
	 */
    size_t current_log_messages() const;

    /**
	 * Get the server to send messages to.
	 *
	 * @since 2.0.0
	 * @return IP address and UDP port of the syslog server.
	 */
    std::pair<IPAddress, uint16_t> destination() const;
    /**
	 * Set the server to send messages to.
	 *
	 * To disable sending messages, set the host to `0.0.0.0` and the
	 * log level to uuid::log::Level::OFF (otherwise they will be
	 * queued but not sent).
	 *
	 * @param[in] host IP address of the syslog server.
	 * @param[in] port UDP port to send messages to.
	 * @since 2.0.0
	 */
    void destination(IPAddress host, uint16_t port = DEFAULT_PORT);

    // added for EMS-ESP
    void destination(const char * host, uint16_t port = DEFAULT_PORT);

    /**
	 * Get local hostname.
	 *
	 * @since 2.0.0
	 * @return Hostname of this device.
	 */
    std::string hostname() const;
    /**
	 * Set local hostname.
	 *
	 * @param[in] hostname Hostname of this device.
	 * @since 2.0.0
	 */
    void hostname(std::string hostname);

    /**
	 * Get mark interval.
	 *
	 * @since 2.0.0
	 * @return Mark interval in seconds (0 = disable).
	 */
    unsigned long mark_interval() const;
    /**
	 * Set mark interval.
	 *
	 * When no messages have been sent for this period of time, a
	 * `-- MARK --` message will be generated automatically.
	 *
	 * @param[in] interval Mark interval in seconds (0 = disable).
	 * @since 2.0.0
	 */
    void mark_interval(unsigned long interval);

    /**
	 * Dispatch queued log messages.
	 *
	 * @since 1.0.0
	 */
    void loop();

    /**
	 * Add a new log message.
	 *
	 * This will be put in a queue for output at the next loop()
	 * process. The queue has a maximum size of
	 * get_maximum_log_messages() and will discard the oldest message
	 * first.
	 *
	 * @param[in] message New log message, shared by all handlers.
	 * @since 1.0.0
	 */
    virtual void operator<<(std::shared_ptr<uuid::log::Message> message);

    /**
	* added for EMS-ESP
	* query status variables
    */
    size_t queued() {
        return log_messages_.size();
    }
    bool started() {
        return started_;
    }
    IPAddress ip() {
        return ip_;
    }

    unsigned long message_count() {
        return log_message_id_;
    }
    unsigned long message_fails() {
        return log_message_fails_;
    }

  private:
    /**
	 * Log message that has been queued.
	 *
	 * Contains an identifier sequence to indicate when log messages
	 * could not be output because the queue discarded one or more
	 * messages.
	 *
	 * @since 1.0.0
	 */
    class QueuedLogMessage {
      public:
        /**
		 * Create a queued log message.
		 *
		 * @param[in] id Identifier to use for the log message on the queue.
		 * @param[in] content Log message content.
		 * @since 1.0.0
		 */
        QueuedLogMessage(unsigned long id, std::shared_ptr<uuid::log::Message> && content);
        ~QueuedLogMessage() = default;

        unsigned long                             id_;      /*!< Sequential identifier for this log message. @since 1.0.0 */
        struct timeval                            time_;    /*!< Time message was received. @since 1.0.0 */
        std::shared_ptr<const uuid::log::Message> content_; /*!< Log message content. @since 1.0.0 */

      private:
        static bool time_good_; /*!< System time appears to be valid. @since 1.0.0 */
    };

    /**
	 * Add a new log message.
	 *
	 * This will be put in a queue for output at the next loop()
	 * process. The queue has a maximum size of
	 * get_maximum_log_messages() and will discard the oldest message
	 * first.
	 *
	 * @param[in] message New log message, shared by all handlers.
	 * @since 2.2.0
	 */
    void add_message(std::shared_ptr<uuid::log::Message> & message);

    /**
	 * Remove messages that were queued before the log level was set.
	 *
	 * @param[in] level New log level
	 * @since 1.0.0
	 */
    void remove_queued_messages(uuid::log::Level level);

    /**
	 * Check if it is possible to transmit to the server.
	 *
	 * @return True if it is safe to transmit a message to the server,
	 *         otherwise false.
	 * @since 1.0.0
	 */
    bool can_transmit();

    /**
	 * Attempt to transmit one message to the server.
	 *
	 * @param[in] message Log message to be sent.
	 * @return True if the message was successfully set, otherwise
	 *         false.
	 * @since 1.0.0
	 */
    bool transmit(const QueuedLogMessage & message);

    static uuid::log::Logger logger_; /*!< uuid::log::Logger instance for syslog services. @since 1.0.0 */

    bool        started_   = false;            /*!< Flag to indicate that messages have started being transmitted. @since 1.0.0 */
    bool        level_set_ = false;            /*!< Flag to indicate that the log level has been set at least once. @since 2.2.0 */
    WiFiUDP     udp_;                          /*!< UDP client. @since 1.0.0 */
    uint16_t    port_          = DEFAULT_PORT; /*!< Port to send messages to. @since 1.0.0 */
    uint64_t    last_transmit_ = 0;            /*!< Last transmit time. @since 1.0.0 */
    std::string hostname_{'-'};                /*!< Local hostname. @since 1.0.0 */
#if UUID_SYSLOG_THREAD_SAFE
    mutable std::mutex mutex_; /*!< Mutex for queued log messages. @since 2.2.0 */
#endif
    size_t                      maximum_log_messages_ = MAX_LOG_MESSAGES; /*!< Maximum number of log messages to buffer before they are output. @since 1.0.0 */
    unsigned long               log_message_id_       = 0;                /*!< The next identifier to use for queued log messages. @since 1.0.0 */
    std::list<QueuedLogMessage> log_messages_;                            /*!< Queued log messages, in the order they were received. @since 1.0.0 */
    uint64_t                    mark_interval_ = 0;                       /*!< Mark interval in milliseconds. @since 2.0.0 */
    uint64_t                    last_message_  = 0;                       /*!< Last message/mark time. @since 2.0.0 */

    // added by MichaelDvP for EMS-ESP
    IPAddress     ip_;   /*!< Host to send messages to. @since 1.0.0 */
    std::string   host_; /*!< Host-IP to send messages to */
    unsigned long log_message_fails_ = 0;
};

} // namespace syslog

} // namespace uuid

#endif
