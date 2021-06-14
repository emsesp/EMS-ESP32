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

#include "emsesp.h"

using namespace std::placeholders;

namespace emsesp {

WebLogService::WebLogService(AsyncWebServer * server, SecurityManager * securityManager)
    : _events(EVENT_SOURCE_LOG_PATH) {
    _events.setFilter(securityManager->filterRequest(AuthenticationPredicates::IS_ADMIN));
    server->addHandler(&_events);
    server->on(EVENT_SOURCE_LOG_PATH, HTTP_GET, std::bind(&WebLogService::forbidden, this, _1));
    start();
}

void WebLogService::forbidden(AsyncWebServerRequest * request) {
    request->send(403);
}

void WebLogService::start() {
    uuid::log::Logger::register_handler(this, uuid::log::Level::INFO); // default is INFO
}

uuid::log::Level WebLogService::log_level() const {
    return uuid::log::Logger::get_log_level(this);
}

void WebLogService::remove_queued_messages(uuid::log::Level level) {
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

void WebLogService::log_level(uuid::log::Level level) {
    uuid::log::Logger::register_handler(this, level);
}

size_t WebLogService::maximum_log_messages() const {
    return maximum_log_messages_;
}

void WebLogService::maximum_log_messages(size_t count) {
    maximum_log_messages_ = std::max((size_t)1, count);
    while (log_messages_.size() > maximum_log_messages_) {
        log_messages_.pop_front();
    }
}

WebLogService::QueuedLogMessage::QueuedLogMessage(unsigned long id, std::shared_ptr<uuid::log::Message> && content)
    : id_(id)
    , content_(std::move(content)) {
}

void WebLogService::operator<<(std::shared_ptr<uuid::log::Message> message) {
    if (log_messages_.size() >= maximum_log_messages_) {
        log_messages_.pop_front();
    }
    log_messages_.emplace_back(log_message_id_++, std::move(message));
}

void WebLogService::loop() {
    if (!_events.count()) {
        return;
    }

    while (!log_messages_.empty() && can_transmit()) {
        transmit(log_messages_.front());
        log_messages_.pop_front();
    }
}

bool WebLogService::can_transmit() {
    const uint64_t now = uuid::get_uptime_ms();
    if (now < last_transmit_ || now - last_transmit_ < 100) {
        return false;
    }
    return true;
}

// send to web eventsource
void WebLogService::transmit(const QueuedLogMessage & message) {
    DynamicJsonDocument jsonDocument = DynamicJsonDocument(EMSESP_JSON_SIZE_SMALL);
    JsonObject          logEvent     = jsonDocument.to<JsonObject>();
    logEvent["time"]                 = uuid::log::format_timestamp_ms(message.content_->uptime_ms, 3);
    logEvent["level"]                = message.content_->level;
    logEvent["message"]              = message.content_->text;

    size_t len    = measureJson(jsonDocument);
    char * buffer = new char[len + 1];
    if (buffer) {
        serializeJson(jsonDocument, buffer, len + 1);
        _events.send(buffer, "message", millis());
    }
    delete[] buffer;

    last_transmit_ = uuid::get_uptime_ms();
}

} // namespace emsesp
