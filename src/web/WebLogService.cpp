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
    : _events(EVENT_SOURCE_LOG_PATH)
    , _setLevel(LOG_SETTINGS_PATH, std::bind(&WebLogService::setLevel, this, _1, _2), 256) { // for POSTS

    _events.setFilter(securityManager->filterRequest(AuthenticationPredicates::IS_ADMIN));
    server->addHandler(&_events);
    server->on(EVENT_SOURCE_LOG_PATH, HTTP_GET, std::bind(&WebLogService::forbidden, this, _1));

    // for bring back the whole log
    server->on(FETCH_LOG_PATH, HTTP_GET, std::bind(&WebLogService::fetchLog, this, _1));

    // get when page is loaded
    server->on(LOG_SETTINGS_PATH, HTTP_GET, std::bind(&WebLogService::getLevel, this, _1));

    // for setting a level
    server->addHandler(&_setLevel);

    // start event source service
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
    if (!_events.count() || log_messages_.empty()) {
        return;
    }

    // put a small delay in
    const uint64_t now = uuid::get_uptime_ms();
    if (now < last_transmit_ || now - last_transmit_ < 50) {
        return;
    }

    // see if we've advanced
    if (log_messages_.back().id_ > log_message_id_tail_) {
        transmit(log_messages_.back());
        log_message_id_tail_ = log_messages_.back().id_;
        last_transmit_       = uuid::get_uptime_ms();
    }
}

// send to web eventsource
void WebLogService::transmit(const QueuedLogMessage & message) {
    DynamicJsonDocument jsonDocument = DynamicJsonDocument(EMSESP_JSON_SIZE_SMALL);
    JsonObject          logEvent     = jsonDocument.to<JsonObject>();
    logEvent["t"]                    = uuid::log::format_timestamp_ms(message.content_->uptime_ms, 3);
    logEvent["l"]                    = message.content_->level;
    logEvent["n"]                    = message.content_->name;
    logEvent["m"]                    = message.content_->text;

    size_t len    = measureJson(jsonDocument);
    char * buffer = new char[len + 1];
    if (buffer) {
        serializeJson(jsonDocument, buffer, len + 1);
        _events.send(buffer, "message", millis());
    }
    delete[] buffer;
}

// send the current log buffer to the API
void WebLogService::fetchLog(AsyncWebServerRequest * request) {
    MsgpackAsyncJsonResponse * response = new MsgpackAsyncJsonResponse(false, EMSESP_JSON_SIZE_XXLARGE_DYN); // 8kb buffer
    JsonObject                 root     = response->getRoot();

    JsonArray log = root.createNestedArray("events");

    for (const auto & msg : log_messages_) {
        JsonObject logEvent = log.createNestedObject();
        auto       message  = std::move(msg);
        logEvent["t"]       = uuid::log::format_timestamp_ms(message.content_->uptime_ms, 3);
        logEvent["l"]       = message.content_->level;
        logEvent["n"]       = message.content_->name;
        logEvent["m"]       = message.content_->text;
    }

    log_message_id_tail_ = log_messages_.back().id_;

    response->setLength();
    request->send(response);
}

// sets the level after a POST
void WebLogService::setLevel(AsyncWebServerRequest * request, JsonVariant & json) {
    if (not json.is<JsonObject>()) {
        return;
    }
    auto &&          body  = json.as<JsonObject>();
    uuid::log::Level level = body["level"];
    log_level(level);

    // send the value back
    AsyncJsonResponse * response = new AsyncJsonResponse(false, EMSESP_JSON_SIZE_SMALL);
    JsonObject          root     = response->getRoot();
    root["level"]                = log_level();
    response->setLength();
    request->send(response);
}

// return the current log level after a GET
void WebLogService::getLevel(AsyncWebServerRequest * request) {
    AsyncJsonResponse * response = new AsyncJsonResponse(false, EMSESP_JSON_SIZE_SMALL);
    JsonObject          root     = response->getRoot();
    root["level"]                = log_level();
    root["max_messages"]         = maximum_log_messages();
    response->setLength();
    request->send(response);
}

} // namespace emsesp
