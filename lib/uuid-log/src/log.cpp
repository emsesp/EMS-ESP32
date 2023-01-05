/*
 * uuid-log - Microcontroller logging framework
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

#include <uuid/log.h>

#include <Arduino.h>

#include <atomic>
#include <cstdarg>
#include <cstdint>
#include <list>
#include <map>
#include <memory>
#if UUID_LOG_THREAD_SAFE
#include <mutex>
#endif
#include <string>
#include <utility>
#include <vector>

namespace uuid {

namespace log {

std::atomic<Level> Logger::global_level_{Level::OFF};
#if UUID_LOG_THREAD_SAFE
std::mutex Logger::mutex_;
#endif

//! @cond false
static Level constrain_level(Level level) {
    if (level < Level::EMERG) {
        level = Level::EMERG;
    } else if (level > Level::TRACE) {
        level = Level::TRACE;
    }
    return level;
}
//! @endcond

Message::Message(uint64_t uptime_ms, Level level, Facility facility, const char * name, const std::string && text)
    : uptime_ms(uptime_ms)
    , level(level)
    , facility(facility)
    , name(name)
    , text(std::move(text)) {
}

Logger::Logger(const char * name, Facility facility)
    : name_(name)
    , facility_(facility){

      };

std::shared_ptr<std::map<Handler *, Level>> & Logger::registered_handlers() {
    static std::shared_ptr<std::map<Handler *, Level>> handlers = std::make_shared<std::map<Handler *, Level>>();

    return handlers;
}

void Logger::register_handler(Handler * handler, Level level) {
#if UUID_LOG_THREAD_SAFE
    std::lock_guard<std::mutex> lock{mutex_};
#endif
    auto & handlers = registered_handlers();

    handler->handlers_   = handlers;
    (*handlers)[handler] = level;
    refresh_log_level();
};

void Logger::unregister_handler(Handler * handler) {
    auto handlers = handler->handlers_.lock();

    if (handlers) {
#if UUID_LOG_THREAD_SAFE
        std::lock_guard<std::mutex> lock{mutex_};
#endif

        if (handlers->erase(handler)) {
            refresh_log_level();
        }
    }
};

Level Logger::get_log_level(const Handler * handler) {
#if UUID_LOG_THREAD_SAFE
    std::lock_guard<std::mutex> lock{mutex_};
#endif
    auto & handlers = registered_handlers();

    const auto level = handlers->find(const_cast<Handler *>(handler));

    if (level != handlers->end()) {
        return level->second;
    }

    return Level::OFF;
}

void Logger::emerg(const char * format, ...) const {
    if (enabled(Level::EMERG)) {
        va_list ap;

        va_start(ap, format);
        vlog(Level::EMERG, format, ap);
        va_end(ap);
    }
};

void Logger::crit(const char * format, ...) const {
    if (enabled(Level::CRIT)) {
        va_list ap;

        va_start(ap, format);
        vlog(Level::CRIT, format, ap);
        va_end(ap);
    }
};

void Logger::alert(const char * format, ...) const {
    if (enabled(Level::ALERT)) {
        va_list ap;

        va_start(ap, format);
        vlog(Level::ALERT, format, ap);
        va_end(ap);
    }
};

void Logger::err(const char * format, ...) const {
    if (enabled(Level::ERR)) {
        va_list ap;

        va_start(ap, format);
        vlog(Level::ERR, format, ap);
        va_end(ap);
    }
};

void Logger::warning(const char * format, ...) const {
    if (enabled(Level::WARNING)) {
        va_list ap;

        va_start(ap, format);
        vlog(Level::WARNING, format, ap);
        va_end(ap);
    }
};

void Logger::notice(const char * format, ...) const {
    if (enabled(Level::NOTICE)) {
        va_list ap;

        va_start(ap, format);
        vlog(Level::NOTICE, format, ap);
        va_end(ap);
    }
};

void Logger::info(const char * format, ...) const {
    if (enabled(Level::INFO)) {
        va_list ap;

        va_start(ap, format);
        vlog(Level::INFO, format, ap);
        va_end(ap);
    }
};

void Logger::debug(const char * format, ...) const {
    if (enabled(Level::DEBUG)) {
        va_list ap;

        va_start(ap, format);
        vlog(Level::DEBUG, format, ap);
        va_end(ap);
    }
};

void Logger::trace(const char * format, ...) const {
    if (enabled(Level::TRACE)) {
        va_list ap;

        va_start(ap, format);
        vlog(Level::TRACE, format, ap);
        va_end(ap);
    }
};

void Logger::log(Level level, const char * format, ...) const {
    level = constrain_level(level);

    if (enabled(level)) {
        va_list ap;

        va_start(ap, format);
        vlog(level, format, ap);
        va_end(ap);
    }
};

void Logger::log(Level level, Facility facility, const char * format, ...) const {
    level = constrain_level(level);

    if (enabled(level)) {
        va_list ap;

        va_start(ap, format);
        vlog(level, facility, format, ap);
        va_end(ap);
    }
};

void Logger::vlog(Level level, const char * format, va_list ap) const {
    vlog(level, facility_, format, ap);
}

void Logger::vlog(Level level, Facility facility, const char * format, va_list ap) const {
    std::vector<char> text(MAX_LOG_LENGTH + 1);

    if (vsnprintf(text.data(), text.size(), format, ap) <= 0) {
        return;
    }

    dispatch(level, facility, text);
}

void Logger::dispatch(Level level, Facility facility, std::vector<char> & text) const {
    std::shared_ptr<Message> message = std::make_shared<Message>(get_uptime_ms(), level, facility, name_, text.data());
    text.resize(0);

#if UUID_LOG_THREAD_SAFE
    std::lock_guard<std::mutex> lock{mutex_};
#endif

    for (auto & handler : *registered_handlers()) {
        if (level <= handler.second) {
            *handler.first << message;
        }
    }
}

/* Mutex already locked by caller. */
void Logger::refresh_log_level() {
    Level level = Level::OFF;

    for (auto & handler : *registered_handlers()) {
        if (level < handler.second) {
            level = handler.second;
        }
    }

    global_level_ = level;
}

} // namespace log

} // namespace uuid