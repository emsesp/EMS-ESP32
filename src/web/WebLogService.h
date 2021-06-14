/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020  Paul Derbyshire
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

#ifndef WebLogService_h
#define WebLogService_h

#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <ESPAsyncWebServer.h>
#include <SecurityManager.h>

#include <uuid/log.h>

#define EVENT_SOURCE_LOG_PATH "/es/log"

namespace emsesp {

class WebLogService : public uuid::log::Handler {
  public:
    static constexpr size_t MAX_LOG_MESSAGES = 50;

    WebLogService(AsyncWebServer * server, SecurityManager * securityManager);

    void             start();
    uuid::log::Level log_level() const;
    void             log_level(uuid::log::Level level);
    size_t           maximum_log_messages() const;
    void             maximum_log_messages(size_t count);
    void             loop();

    virtual void operator<<(std::shared_ptr<uuid::log::Message> message);

  private:
    AsyncEventSource _events;

    class QueuedLogMessage {
      public:
        QueuedLogMessage(unsigned long id, std::shared_ptr<uuid::log::Message> && content);
        ~QueuedLogMessage() = default;

        unsigned long                                   id_;      // Sequential identifier for this log message
        struct timeval                                  time_;    // Time message was received
        const std::shared_ptr<const uuid::log::Message> content_; // Log message content
    };

    void forbidden(AsyncWebServerRequest * request);
    void remove_queued_messages(uuid::log::Level level);
    bool can_transmit();
    void transmit(const QueuedLogMessage & message);

    uint64_t                    last_transmit_        = 0;                // Last transmit time
    size_t                      maximum_log_messages_ = MAX_LOG_MESSAGES; // Maximum number of log messages to buffer before they are output
    unsigned long               log_message_id_       = 0;                // The next identifier to use for queued log messages
    std::list<QueuedLogMessage> log_messages_;                            // Queued log messages, in the order they were received
};

} // namespace emsesp

#endif
