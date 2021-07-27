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
    : events_(EVENT_SOURCE_LOG_PATH)
    , setValues_(LOG_SETTINGS_PATH, std::bind(&WebLogService::setValues, this, _1, _2), 256) { // for POSTS

    events_.setFilter(securityManager->filterRequest(AuthenticationPredicates::IS_ADMIN));
    server->addHandler(&events_);
    server->on(EVENT_SOURCE_LOG_PATH, HTTP_GET, std::bind(&WebLogService::forbidden, this, _1));

    // for bring back the whole log
    server->on(FETCH_LOG_PATH, HTTP_GET, std::bind(&WebLogService::fetchLog, this, _1));

    // get when page is loaded
    server->on(LOG_SETTINGS_PATH, HTTP_GET, std::bind(&WebLogService::getValues, this, _1));

    // for setting a level
    server->addHandler(&setValues_);
}

void WebLogService::forbidden(AsyncWebServerRequest * request) {
    request->send(403);
}

// start event source service
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

    EMSESP::esp8266React.getNTPSettingsService()->read([&](NTPSettings & settings) {
        if (!settings.enabled || (time(nullptr) < 1500000000L)) {
            time_offset_ = 0;
        } else if (!time_offset_) {
            time_offset_ = time(nullptr) - uuid::get_uptime_sec();
        }
    });
}

void WebLogService::loop() {
    if (!events_.count() || log_messages_.empty()) {
        return;
    }

    // put a small delay in
    const uint64_t now = uuid::get_uptime_ms();
    if (now < last_transmit_ || now - last_transmit_ < REFRESH_SYNC) {
        return;
    }

    // see if we've advanced
    if (log_messages_.back().id_ <= log_message_id_tail_) {
        return;
    }

    // flush
    for (auto it = log_messages_.begin(); it != log_messages_.end(); it++) {
        if (it->id_ > log_message_id_tail_) {
            transmit(*it);
        }
    }

    log_message_id_tail_ = log_messages_.back().id_;
    last_transmit_       = uuid::get_uptime_ms();
}

// convert time to real offset
char * WebLogService::messagetime(char * out, const uint64_t t) {
    if (!time_offset_) {
        strcpy(out, uuid::log::format_timestamp_ms(t, 3).c_str());
    } else {
        time_t t1 = time_offset_ + t / 1000ULL;
        strftime(out, 25, "%F %T", localtime(&t1));
        snprintf_P(out, 25, "%s.%03d", out, (uint16_t)(t % 1000));
    }
    return out;
}

// send to web eventsource
void WebLogService::transmit(const QueuedLogMessage & message) {
    DynamicJsonDocument jsonDocument = DynamicJsonDocument(EMSESP_JSON_SIZE_SMALL);
    JsonObject          logEvent     = jsonDocument.to<JsonObject>();
    char                time_string[25];

    logEvent["t"] = messagetime(time_string, message.content_->uptime_ms);
    logEvent["l"] = message.content_->level;
    logEvent["i"] = message.id_;
    logEvent["n"] = message.content_->name;
    logEvent["m"] = message.content_->text;

    size_t len    = measureJson(jsonDocument);
    char * buffer = new char[len + 1];
    if (buffer) {
        serializeJson(jsonDocument, buffer, len + 1);
        events_.send(buffer, "message", millis());
    }
    delete[] buffer;
}

// send the complete log buffer to the API, filtering on log level
void WebLogService::fetchLog(AsyncWebServerRequest * request) {
    MsgpackAsyncJsonResponse * response = new MsgpackAsyncJsonResponse(false, EMSESP_JSON_SIZE_XXLARGE_DYN); // 8kb buffer
    JsonObject                 root     = response->getRoot();

    JsonArray log = root.createNestedArray("events");

    for (const auto & msg : log_messages_) {
        if (msg.content_->level <= log_level()) {
            JsonObject logEvent = log.createNestedObject();
            auto       message  = std::move(msg);
            char       time_string[25];

            logEvent["t"] = messagetime(time_string, message.content_->uptime_ms);
            logEvent["l"] = message.content_->level;
            logEvent["i"] = message.id_;
            logEvent["n"] = message.content_->name;
            logEvent["m"] = message.content_->text;
        }
    }

    log_message_id_tail_ = log_messages_.back().id_;

    response->setLength();
    request->send(response);
}

// sets the values like level after a POST
void WebLogService::setValues(AsyncWebServerRequest * request, JsonVariant & json) {
    if (not json.is<JsonObject>()) {
        return;
    }

    auto && body = json.as<JsonObject>();

    uuid::log::Level level = body["level"];
    log_level(level);

    uint8_t max_messages = body["max_messages"];
    maximum_log_messages(max_messages);

    request->send(200); // OK
}

// return the current value settings after a GET
void WebLogService::getValues(AsyncWebServerRequest * request) {
    AsyncJsonResponse * response = new AsyncJsonResponse(false, EMSESP_JSON_SIZE_SMALL);
    JsonObject          root     = response->getRoot();
    root["level"]                = log_level();
    root["max_messages"]         = maximum_log_messages();
    response->setLength();
    request->send(response);
}

} // namespace emsesp
