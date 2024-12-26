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
    uuid::log::Logger::register_handler(this, uuid::log::Level::INFO);
}

// apply the user settings
void WebLogService::start() {
    EMSESP::webSettingsService.read([&](WebSettings & settings) {
        maximum_log_messages_ = settings.weblog_buffer;
        limit_log_messages_   = maximum_log_messages_;
        compact_              = settings.weblog_compact;
        uuid::log::Logger::register_handler(this, (uuid::log::Level)settings.weblog_level);
        if ((uuid::log::Level)settings.weblog_level == uuid::log::Level::OFF) {
            log_messages_.clear();
        }
    });
}

uuid::log::Level WebLogService::log_level() const {
    return uuid::log::Logger::get_log_level(this);
}

void WebLogService::log_level(uuid::log::Level level) {
    EMSESP::webSettingsService.update([&](WebSettings & settings) {
        settings.weblog_level = level;
        return StateUpdateResult::CHANGED;
    });
    uuid::log::Logger::register_handler(this, level);
    if (level == uuid::log::Level::OFF) {
        log_messages_.clear();
    }
}

size_t WebLogService::num_log_messages() const {
    return log_messages_.size();
}

size_t WebLogService::maximum_log_messages() const {
    return maximum_log_messages_;
}

void WebLogService::maximum_log_messages(size_t count) {
    maximum_log_messages_ = std::max((size_t)1, count);

    if (limit_log_messages_ > maximum_log_messages_) {
        limit_log_messages_ = maximum_log_messages_;
    }

    while (log_messages_.size() > maximum_log_messages_) {
        log_messages_.pop_front();
    }

    EMSESP::webSettingsService.update([&](WebSettings & settings) {
        settings.weblog_buffer = count;
        return StateUpdateResult::CHANGED;
    });
}

bool WebLogService::compact() const {
    return compact_;
}

void WebLogService::compact(bool compact) {
    compact_ = compact;
    EMSESP::webSettingsService.update([&](WebSettings & settings) {
        settings.weblog_compact = compact;
        return StateUpdateResult::CHANGED;
    });
}

WebLogService::QueuedLogMessage::QueuedLogMessage(unsigned long id, std::shared_ptr<uuid::log::Message> && content)
    : id_(id)
    , content_(std::move(content)) {
}

void WebLogService::operator<<(std::shared_ptr<uuid::log::Message> message) {
#ifndef EMSESP_STANDALONE
    size_t maxAlloc = ESP.getMaxAllocHeap();
    if (limit_log_messages_ > 5 && maxAlloc < 46080) {
        --limit_log_messages_;
    } else if (limit_log_messages_ < maximum_log_messages_ && maxAlloc > 51200) {
        ++limit_log_messages_;
    }
#endif
    while (log_messages_.size() >= limit_log_messages_) {
        log_messages_.pop_front();
    }

    log_messages_.emplace_back(++log_message_id_, std::move(message));

    EMSESP::esp32React.getNTPSettingsService()->read([&](NTPSettings & settings) {
        if (!settings.enabled || (time(nullptr) < 1500000000L)) {
            time_offset_ = 0;
        } else {
            uint32_t offset = time(nullptr) - uuid::get_uptime_sec();
            // if NTP is more that 1 sec apart, correct setting
            if (time_offset_ < offset - 1 || time_offset_ > offset + 1) {
                time_offset_ = offset;
            }
        }
    });
}

// dumps out the contents of log buffer to shell console
void WebLogService::show(Shell & shell) {
    if (log_messages_.empty()) {
        return;
    }

    shell.println();
    shell.printfln("Recent Log (level %s, max %d messages):", format_level_uppercase(log_level()), maximum_log_messages());
    shell.println();

    for (const auto & message : log_messages_) {
        log_message_id_tail_ = message.id_;

        shell.print(uuid::log::format_timestamp_ms(message.content_->uptime_ms, 3));
        shell.printf(" %c %lu: [%s] ", uuid::log::format_level_char(message.content_->level), message.id_, message.content_->name);

        if ((message.content_->level == uuid::log::Level::ERR) || (message.content_->level == uuid::log::Level::WARNING)) {
            shell.print(COLOR_RED);
            shell.println(message.content_->text);
            shell.print(COLOR_RESET);
        } else if (message.content_->level == uuid::log::Level::INFO) {
            shell.print(COLOR_YELLOW);
            shell.println(message.content_->text);
            shell.print(COLOR_RESET);
        } else if (message.content_->level == uuid::log::Level::DEBUG) {
            shell.print(COLOR_CYAN);
            shell.println(message.content_->text);
            shell.print(COLOR_RESET);
        } else {
            shell.println(message.content_->text);
        }
    }

    shell.println();
}

void WebLogService::loop() {
    if (!events_.count() || log_messages_.empty()) {
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
            log_message_id_tail_ = message.id_;
            transmit(message);
            return;
        }
    }
}

// convert time to real offset
char * WebLogService::messagetime(char * out, const uint64_t t, const size_t bufsize) {
    if (!time_offset_) {
        strlcpy(out, uuid::log::format_timestamp_ms(t, 3).c_str(), bufsize);
    } else {
        time_t t1 = time_offset_ + t / 1000ULL;
        char   timestr[bufsize];
        strftime(timestr, bufsize, "%F %T", localtime(&t1));
        snprintf(out, bufsize, "%s.%03d", timestr, (uint16_t)(t % 1000));
    }
    return out;
}

// send to web eventsource
void WebLogService::transmit(const QueuedLogMessage & message) {
    JsonDocument jsonDocument;

    JsonObject logEvent = jsonDocument.to<JsonObject>();
    char       time_string[25];

    logEvent["t"] = messagetime(time_string, message.content_->uptime_ms, sizeof(time_string));
    logEvent["l"] = message.content_->level;
    logEvent["i"] = message.id_;
    logEvent["n"] = message.content_->name;
    logEvent["m"] = message.content_->text;

    size_t len    = measureJson(jsonDocument);
    char * buffer = new char[len + 1];
    if (buffer) {
        serializeJson(jsonDocument, buffer, len + 1);
        events_.send(buffer, "message", message.id_);
    }
    delete[] buffer;
}

// sets the values after a POST
void WebLogService::getSetValues(AsyncWebServerRequest * request, JsonVariant json) {
    if ((request->method() == HTTP_GET) || (!json.is<JsonObject>())) {
        // GET - return the values
        auto *     response    = new AsyncJsonResponse(false);
        JsonObject root        = response->getRoot();
        root["level"]          = log_level();
        root["max_messages"]   = maximum_log_messages();
        root["compact"]        = compact();
        root["psram"]          = (EMSESP::system_.PSram() > 0);
        root["developer_mode"] = EMSESP::system_.developer_mode();

        response->setLength();
        request->send(response);

        // reset the tail pointer so complete log is sent
        log_message_id_tail_ = 0;

        return;
    }

    // POST - set the values
    auto && body = json.as<JsonObject>();

    uuid::log::Level level = body["level"];
    log_level(level);

    uint8_t max_messages = body["max_messages"];
    maximum_log_messages(max_messages);

    bool comp = body["compact"];
    compact(comp);

    request->send(200); // OK
}

} // namespace emsesp