/*
 * uuid-log - Microcontroller logging framework
 * Copyright 2022  Simon Arlott
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

#include <memory>
#if UUID_LOG_THREAD_SAFE
#include <mutex>
#endif

namespace uuid {

namespace log {

PrintHandler::PrintHandler(::Print & print)
    : print_(print) {
}

size_t PrintHandler::maximum_log_messages() const {
#if UUID_LOG_THREAD_SAFE
    std::lock_guard<std::mutex> lock{mutex_};
#endif

    return maximum_log_messages_;
}

void PrintHandler::maximum_log_messages(size_t count) {
#if UUID_LOG_THREAD_SAFE
    std::lock_guard<std::mutex> lock{mutex_};
#endif

    maximum_log_messages_ = std::max((size_t)1, count);

    while (log_messages_.size() > maximum_log_messages_) {
        log_messages_.pop_front();
    }
}

void PrintHandler::loop(size_t count) {
#if UUID_LOG_THREAD_SAFE
    std::unique_lock<std::mutex> lock{mutex_};
#endif

    count = std::max((size_t)1, count);

    while (!log_messages_.empty()) {
        auto message = log_messages_.front();

        log_messages_.pop_front();
#if UUID_LOG_THREAD_SAFE
        lock.unlock();
#endif

        print_.print(uuid::log::format_timestamp_ms(message->uptime_ms, 3).c_str());
        print_.print(' ');
        print_.print(uuid::log::format_level_char(message->level));
        print_.print(" [");
        print_.print(message->name);
        print_.print("] ");
        print_.println(message->text.c_str());

        count--;
        if (count == 0) {
            break;
        }

        ::yield();

#if UUID_LOG_THREAD_SAFE
        lock.lock();
#endif
    }
}

void PrintHandler::operator<<(std::shared_ptr<Message> message) {
#if UUID_LOG_THREAD_SAFE
    std::lock_guard<std::mutex> lock{mutex_};
#endif

    if (log_messages_.size() >= maximum_log_messages_) {
        log_messages_.pop_front();
    }

    log_messages_.emplace_back(std::move(message));
}

} // namespace log

} // namespace uuid