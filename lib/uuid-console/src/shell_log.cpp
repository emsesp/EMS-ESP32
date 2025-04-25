/*
 * uuid-console - Microcontroller console shell
 * Copyright 2019  Simon Arlott
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

#include <uuid/console.h>

#include <Arduino.h>

#include <algorithm>
#include <memory>
#if UUID_CONSOLE_THREAD_SAFE
#include <mutex>
#endif
#include <string>

#include <uuid/log.h>

namespace uuid {

namespace console {

static const char __pstr__logger_name[] = "shell";

const uuid::log::Logger & Shell::logger() {
    static const uuid::log::Logger logger_instance{reinterpret_cast<const char *>(__pstr__logger_name), uuid::log::Facility::LPR};

    return logger_instance;
}

Shell::QueuedLogMessage::QueuedLogMessage(unsigned long id, std::shared_ptr<uuid::log::Message> && content)
    : id_(id)
    , content_(std::move(content)) {
}

void Shell::operator<<(std::shared_ptr<uuid::log::Message> message) {
#if UUID_CONSOLE_THREAD_SAFE
    std::lock_guard<std::mutex> lock{mutex_};
#endif
    if (log_messages_.size() >= maximum_log_messages_) {
        log_messages_.pop_front();
    }

    log_messages_.emplace_back(log_message_id_++, std::move(message));
}

uuid::log::Level Shell::log_level() const {
    return uuid::log::Logger::get_log_level(this);
}

void Shell::log_level(uuid::log::Level level) {
    uuid::log::Logger::register_handler(this, level);
}

size_t Shell::maximum_log_messages() const {
#if UUID_CONSOLE_THREAD_SAFE
    std::lock_guard<std::mutex> lock{mutex_};
#endif

    return maximum_log_messages_;
}

void Shell::maximum_log_messages(size_t count) {
#if UUID_CONSOLE_THREAD_SAFE
    std::lock_guard<std::mutex> lock{mutex_};
#endif

    maximum_log_messages_ = std::max((size_t)1, count);
    while (log_messages_.size() > maximum_log_messages_) {
        log_messages_.pop_front();
    }
}

void Shell::output_logs() {
#if UUID_CONSOLE_THREAD_SAFE
    std::unique_lock<std::mutex> lock{mutex_};
#endif

    if (log_messages_.empty())
        return;

    size_t count   = std::max((size_t)1, MAX_LOG_MESSAGES);
    auto   message = log_messages_.front();

    log_messages_.pop_front();
#if UUID_CONSOLE_THREAD_SAFE
    lock.unlock();
#endif

    if (mode_ != Mode::DELAY) {
        erase_current_line();
        prompt_displayed_ = false;
    }

    while (1) {
        time_t offset = time(nullptr) - uuid::get_uptime_sec();
        if (offset < 1500000000L) {
            print(uuid::log::format_timestamp_ms(message.content_->uptime_ms, 3));
        } else {
            time_t t1 = offset + (time_t)(message.content_->uptime_ms / 1000);
            char   timestr[25];
            strftime(timestr, 25, "%FT%T", localtime(&t1));
            printf("%s.%03d", timestr, (uint16_t)(message.content_->uptime_ms % 1000));
        }
        printf(" %c %lu: [%s] ", uuid::log::format_level_char(message.content_->level), message.id_, message.content_->name);

        if ((message.content_->level == uuid::log::Level::ERR) || (message.content_->level == uuid::log::Level::WARNING)) {
            print(COLOR_RED);
            println(message.content_->text);
            print(COLOR_RESET);
        } else if (message.content_->level == uuid::log::Level::INFO) {
            print(COLOR_YELLOW);
            println(message.content_->text);
            print(COLOR_RESET);
        } else if (message.content_->level == uuid::log::Level::DEBUG) {
            print(COLOR_CYAN);
            println(message.content_->text);
            print(COLOR_RESET);
        } else {
            println(message.content_->text);
        }

        ::yield();

        count--;
        if (count == 0) {
            break;
        }

#if UUID_CONSOLE_THREAD_SAFE
        lock.lock();
#endif
        if (log_messages_.empty()) {
#if UUID_CONSOLE_THREAD_SAFE
            lock.unlock();
#endif
            break;
        }

        message = log_messages_.front();
        log_messages_.pop_front();
#if UUID_CONSOLE_THREAD_SAFE
        lock.unlock();
#endif
    }

    display_prompt();
}

} // namespace console

} // namespace uuid
