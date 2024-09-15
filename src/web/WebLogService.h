/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2024  emsesp.org - proddy, MichaelDvP
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

#define EMSESP_EVENT_SOURCE_LOG_PATH "/es/log"
#define EMSESP_LOG_SETTINGS_PATH "/rest/logSettings"

using ::uuid::console::Shell;

namespace emsesp {

class WebLogService : public uuid::log::Handler {
  public:
    static constexpr size_t MAX_LOG_MESSAGES = 25;
    // static constexpr size_t REFRESH_SYNC     = 30;

    WebLogService(AsyncWebServer * server, SecurityManager * securityManager);

    void             begin();
    void             start();
    uuid::log::Level log_level() const;
    void             log_level(uuid::log::Level level);
    size_t           maximum_log_messages() const;
    size_t           num_log_messages() const;
    void             maximum_log_messages(size_t count);
    bool             compact() const;
    void             compact(bool compact);
    void             loop();
    void             show(Shell & shell);

    virtual void operator<<(std::shared_ptr<uuid::log::Message> message);

  private:
    AsyncEventSource events_;

    class QueuedLogMessage {
      public:
        QueuedLogMessage(unsigned long id, std::shared_ptr<uuid::log::Message> && content);
        ~QueuedLogMessage() = default;

        unsigned long                                   id_;      // Sequential identifier for this log message
        struct timeval                                  time_;    // Time message was received
        const std::shared_ptr<const uuid::log::Message> content_; // Log message content
    };

    void transmit(const QueuedLogMessage & message);
    void getSetValues(AsyncWebServerRequest * request, JsonVariant json);

    char * messagetime(char * out, const uint64_t t, const size_t bufsize);

    size_t                       maximum_log_messages_ = MAX_LOG_MESSAGES; // Maximum number of log messages to buffer before they are output
    size_t                       limit_log_messages_   = 1;                // dynamic limit
    unsigned long                log_message_id_       = 0;                // The next identifier to use for queued log messages
    unsigned long                log_message_id_tail_  = 0;                // last event shown on the screen after fetch
    std::deque<QueuedLogMessage> log_messages_;                            // Queued log messages, in the order they were received
    time_t                       time_offset_ = 0;
    bool                         compact_     = true;
};

} // namespace emsesp

#endif
