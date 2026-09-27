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

using ::uuid::console::Shell;

namespace emsesp {

WebLogService::WebLogService(AsyncWebServer * server, SecurityManager * securityManager)
    : events_(EMSESP_EVENT_SOURCE_LOG_PATH) {
    // there is a GET and a POST for LogSettings endpoint, in one function
    securityManager->addEndpoint(
        server,
        EMSESP_LOG_SETTINGS_PATH,
        AuthenticationPredicates::IS_AUTHENTICATED,
        [this](AsyncWebServerRequest * request, JsonVariant json) { getSetValues(request, json); },
        HTTP_ANY);

    server->addHandler(&events_);
}

// start the log service with INFO level
void WebLogService::begin() {
    level_ = uuid::log::Level::INFO;
    uuid::log::Logger::register_handler(this, level_);
}

// apply the user settings
void WebLogService::start() {
    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        // a stored value of 0 would make limit_log_messages_ 0, see the guard in operator<<
        if (settings.weblog_buffer) {
            maximum_log_messages_ = settings.weblog_buffer;
        }
        limit_log_messages_ = maximum_log_messages_;
        compact_            = settings.weblog_compact;
        level_              = (uuid::log::Level)settings.weblog_level;
    });
    uuid::log::Logger::register_handler(this, level_);
    if (level_ == uuid::log::Level::OFF) {
        log_messages_.clear();
    }
}

size_t WebLogService::num_log_messages() const {
    return log_messages_.size();
}

WebLogService::QueuedLogMessage::QueuedLogMessage(unsigned long id, uint64_t uptime, uuid::log::Level level, const char * name, std::string text)
    : id_(id)
    , uptime_(uptime)
    , level_(level)
    , name_(name)
    , text_(text.c_str()) {
}

void WebLogService::operator<<(std::shared_ptr<uuid::log::Message> message) {
#ifndef EMSESP_STANDALONE
    if (emsesp::EMSESP::system_.PSram()) {
        limit_log_messages_ = maximum_log_messages_;
    } else {
        uint32_t maxAlloc = ESP.getMaxAllocHeap();
        if (limit_log_messages_ > 5 && maxAlloc < LOG_HEAP_LOW) {
            --limit_log_messages_;
        } else if (limit_log_messages_ < maximum_log_messages_ && maxAlloc > LOG_HEAP_HIGH) {
            ++limit_log_messages_;
        }
    }
#endif
    // size() is unsigned, so with limit_log_messages_ == 0 this condition is always true
    // and pop_front() would run on an empty deque (undefined behaviour)
    while (!log_messages_.empty() && log_messages_.size() >= limit_log_messages_) {
        log_messages_.pop_front();
    }

    log_messages_.emplace_back(++log_message_id_, message->uptime_ms, message->level, message->name, message->text.c_str());
}

// dumps out the contents of log buffer to shell console
void WebLogService::show(Shell & shell) {
    if (log_messages_.empty()) {
        return;
    }

    shell.println();
    shell.printfln("Last %d messages:", log_messages_.size());
    shell.println();

    for (const auto & message : log_messages_) {
        char time_string[26];
        shell.print(messagetime(time_string, message.uptime_, sizeof(time_string)));
        shell.printf(" %c %lu: [%s] ", uuid::log::format_level_char(message.level_), message.id_, message.name_);

        if ((message.level_ == uuid::log::Level::ERR) || (message.level_ == uuid::log::Level::WARNING)) {
            shell.print(COLOR_RED);
            shell.println(message.text_.c_str());
            shell.print(COLOR_RESET);
        } else if (message.level_ == uuid::log::Level::INFO) {
            shell.print(COLOR_YELLOW);
            shell.println(message.text_.c_str());
            shell.print(COLOR_RESET);
        } else if (message.level_ == uuid::log::Level::DEBUG) {
            shell.print(COLOR_CYAN);
            shell.println(message.text_.c_str());
            shell.print(COLOR_RESET);
        } else {
            shell.println(message.text_.c_str());
        }
    }

    shell.println();
}

void WebLogService::loop() {
    if (!events_.count() || log_messages_.empty() || events_.avgPacketsWaiting() >= SSE_MAX_QUEUED_MESSAGES) {
        return;
    }

    // see if we've advanced
    if (log_messages_.back().id_ <= log_message_id_tail_) {
        return;
    }

    /*
    // put a small delay in - https://github.com/emsesp/EMS-ESP32/issues/1652
    if (uuid::get_uptime_ms() - last_transmit_ < REFRESH_SYNC) {
        return;
    }
    last_transmit_ = uuid::get_uptime_ms();
    */

    // flush
    for (const auto & message : log_messages_) {
        if (message.id_ > log_message_id_tail_) {
            transmit(message);
            return;
        }
    }
}

// convert time to real offset
char * WebLogService::messagetime(char * out, const uint64_t t, const size_t bufsize) {
    time_t offset = time(nullptr) - uuid::get_uptime_sec();
    if (offset < 1500000000L) {
        strlcpy(out, uuid::log::format_timestamp_ms(t, 3).c_str(), bufsize);
    } else {
        time_t t1 = offset + (time_t)(t / 1000);
        char   timestr[25];
        strftime(timestr, 25, "%FT%T", localtime(&t1));
        snprintf(out, bufsize, "%s.%03d", timestr, (uint16_t)(t % 1000));
    }
    return out;
}

// send to web eventsource
void WebLogService::transmit(const QueuedLogMessage & message) {
    // JsonDocument is PSRAM-backed so its internal node/string pool is on PSRAM
    JsonDocument jsonDocument(PSRAM_DOC);

    JsonObject logEvent = jsonDocument.to<JsonObject>();
    char       time_string[25];

    logEvent["t"] = messagetime(time_string, message.uptime_, sizeof(time_string));
    logEvent["l"] = message.level_;
    logEvent["i"] = message.id_;
    logEvent["n"] = message.name_;
    logEvent["m"] = message.text_;

    // Reuse a PSRAM-backed scratch buffer
    const size_t len = measureJson(jsonDocument);
    if (scratch_buf_.capacity() < len + 1) {
        scratch_buf_.reserve(len + 1);
    }
    scratch_buf_.resize(len);
    serializeJson(jsonDocument, scratch_buf_.data(), len + 1);
    events_.send(scratch_buf_.c_str(), "message", message.id_);
    log_message_id_tail_ = message.id_;
}

// sets the values after a POST
void WebLogService::getSetValues(AsyncWebServerRequest * request, JsonVariant json) {
    if ((request->method() == HTTP_GET) || (!json.is<JsonObject>())) {
        // GET - return the values
        auto *     response    = new PsramAsyncJsonResponse(false);
        JsonObject root        = response->getRoot();
        root["level"]          = level_;
        root["max_messages"]   = maximum_log_messages_;
        root["compact"]        = compact_;
        root["psram"]          = (EMSESP::system_.PSram() > 0);
        root["developer_mode"] = EMSESP::system_.developer_mode();

        response->setLength();
        request->send(response);

        // reset the tail pointer so complete log is sent
        log_message_id_tail_ = 0;

        return;
    }

    // POST - write the settings
    level_ = json["level"];
    // keep the current size if the key is missing or 0 - a 0 would be stored and make
    // limit_log_messages_ 0 on the next boot
    const size_t max_messages = json["max_messages"] | maximum_log_messages_;
    if (max_messages) {
        maximum_log_messages_ = max_messages;
    }
    compact_ = json["compact"];

    uuid::log::Logger::register_handler(this, level_);
    if (level_ == uuid::log::Level::OFF) {
        log_messages_.clear();
    }

    if (limit_log_messages_ > maximum_log_messages_) {
        limit_log_messages_ = maximum_log_messages_;
        while (log_messages_.size() > limit_log_messages_) {
            log_messages_.pop_front();
        }
    }

    EMSESP::webSettingsService.update([&](WebSettings & settings) {
        settings.weblog_compact = compact_;
        settings.weblog_level   = level_;
        settings.weblog_buffer  = maximum_log_messages_;
        return StateUpdateResult::CHANGED;
    });

    request->send(200); // OK
}

} // namespace emsesp