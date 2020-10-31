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
#include <stdarg.h>

#include <algorithm>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <uuid/common.h>
#include <uuid/log.h>

#ifndef __cpp_lib_make_unique
namespace std {

template <typename _Tp, typename... _Args>
inline unique_ptr<_Tp> make_unique(_Args &&... __args) {
    return unique_ptr<_Tp>(new _Tp(std::forward<_Args>(__args)...));
}

} // namespace std
#endif

namespace uuid {

namespace console {

// cppcheck-suppress passedByValue
Shell::Shell(std::shared_ptr<Commands> commands, unsigned int context, unsigned int flags)
    : commands_(std::move(commands))
    , flags_(flags) {
    enter_context(context);
}

Shell::~Shell() {
    uuid::log::Logger::unregister_handler(this);
}

void Shell::start() {
#ifdef EMSESP_DEBUG
    uuid::log::Logger::register_handler(this, uuid::log::Level::DEBUG); // added by proddy
    //uuid::log::Logger::register_handler(this, uuid::log::Level::INFO); // added by proddy
#else
    uuid::log::Logger::register_handler(this, uuid::log::Level::NOTICE);
#endif

    line_buffer_.reserve(maximum_command_line_length_);
    oldline_.reserve(maximum_command_line_length_);
    oldline_.clear();
    display_banner();
    display_prompt();
    shells_.insert(shared_from_this());
    idle_time_ = uuid::get_uptime_ms();
    started();
};

void Shell::started() {
}

bool Shell::running() const {
    return !stopped_;
}

void Shell::stop() {
    if (mode_ == Mode::BLOCKING) {
        auto * blocking_data = reinterpret_cast<Shell::BlockingData *>(mode_data_.get());

        blocking_data->stop_ = true;
    } else {
        if (running()) {
            stopped_ = true;
            stopped();
        }
    }
}

void Shell::stopped() {
}

bool Shell::exit_context() {
    if (context_.size() > 1) {
        context_.pop_back();
        return true;
    } else {
        return false;
    }
}

void Shell::loop_one() {
    if (!running()) {
        return;
    }

    switch (mode_) {
    case Mode::NORMAL:
        output_logs();
        loop_normal();
        break;

    case Mode::PASSWORD:
        output_logs();
        loop_password();
        break;

    case Mode::DELAY:
        output_logs();
        loop_delay();
        break;

    case Mode::BLOCKING:
        loop_blocking();
        break;
    }
}

void Shell::loop_normal() {
    const int input = read_one_char();

    if (input < 0) {
        check_idle_timeout();
        return;
    }

    const unsigned char c = input;

    switch (c) {
    case '\x03':
        // Interrupt (^C)
        line_buffer_.clear();
        println();
        prompt_displayed_ = false;
        display_prompt();
        break;

    case '\x04':
        // End of transmission (^D)
        if (line_buffer_.empty()) {
            end_of_transmission();
        }
        break;

    case '\x08':
    case '\x7F':
        // Backspace (^H)
        // Delete (^?)
        if (!line_buffer_.empty()) {
            erase_characters(1);
            line_buffer_.pop_back();
        }
        break;

    case '\x09':
        // Tab (^I)
        process_completion();
        break;

    case '\x0A':
        // Line feed (^J)
        if (previous_ != '\x0D') {
            if (!line_buffer_.empty()) {
                oldline_ = line_buffer_;
            }
            process_command();
        }
        break;

    case '\x0C':
        // New page (^L)
        erase_current_line();
        prompt_displayed_ = false;
        display_prompt();
        break;

    case '\x0D':
        if (!line_buffer_.empty()) {
            oldline_ = line_buffer_;
        }
        // Carriage return (^M)
        process_command();
        break;

    case '\x15':
        // Delete line (^U)
        erase_current_line();
        prompt_displayed_ = false;
        line_buffer_.clear();
        display_prompt();
        break;

    case '\x17':
        // Delete word (^W)
        delete_buffer_word(true);
        break;

    default:
        if (c >= '\x20' && c <= '\x7E') {
            // ASCII text
            if (line_buffer_.length() < maximum_command_line_length_) {
                line_buffer_.push_back(c);
                write((uint8_t)c);
            }
            // cursor up, get last command
            if ((c == 'A') && (previous_ == '[')) {
                erase_current_line();
                prompt_displayed_ = false;
                line_buffer_ = oldline_;
                display_prompt();
            }
            // cursor back, delete cursor chars
            if ((c == 'D') && (previous_ == '[')) {
                line_buffer_.pop_back();
                line_buffer_.pop_back();
                // alternative work as backspace
                // if (line_buffer_.length() > 0) {
                //     line_buffer_.pop_back();
                // }
                erase_current_line();
                prompt_displayed_ = false;
                display_prompt();
            }
            // cursor forward, only delete cursor chars
            if ((c == 'C') && (previous_ == '[')) {
                line_buffer_.pop_back();
                line_buffer_.pop_back();
                erase_current_line();
                prompt_displayed_ = false;
                display_prompt();
            }
            // cursor down(B): Delete line
            if ((c == 'B') && (previous_ == '[')) {
                erase_current_line();
                prompt_displayed_ = false;
                line_buffer_.clear();
                display_prompt();
            }
        }
        break;
    }

    previous_ = c;

    // This is a hack to let TelnetStream know that command
    // execution is complete and that output can be flushed.
    available_char();

    idle_time_ = uuid::get_uptime_ms();
}

Shell::PasswordData::PasswordData(const __FlashStringHelper * password_prompt, password_function && password_function)
    : password_prompt_(password_prompt)
    , password_function_(std::move(password_function)) {
}

void Shell::loop_password() {
    const int input = read_one_char();

    if (input < 0) {
        check_idle_timeout();
        return;
    }

    const unsigned char c = input;

    switch (c) {
    case '\x03':
        // Interrupt (^C)
        process_password(false);
        break;

    case '\x08':
    case '\x7F':
        // Backspace (^H)
        // Delete (^?)
        if (!line_buffer_.empty()) {
            line_buffer_.pop_back();
        }
        break;

    case '\x0A':
        // Line feed (^J)
        if (previous_ != '\x0D') {
            process_password(true);
        }
        break;

    case '\x0C':
        // New page (^L)
        erase_current_line();
        prompt_displayed_ = false;
        display_prompt();
        break;

    case '\x0D':
        // Carriage return (^M)
        process_password(true);
        break;

    case '\x15':
        // Delete line (^U)
        line_buffer_.clear();
        break;

    case '\x17':
        // Delete word (^W)
        delete_buffer_word(false);
        break;

    default:
        if (c >= '\x20' && c <= '\x7E') {
            // ASCII text
            if (line_buffer_.length() < maximum_command_line_length_) {
                line_buffer_.push_back(c);
            }
        }
        break;
    }

    previous_ = c;

    // This is a hack to let TelnetStream know that command
    // execution is complete and that output can be flushed.
    available_char();

    idle_time_ = uuid::get_uptime_ms();
}

Shell::DelayData::DelayData(uint64_t delay_time, delay_function && delay_function)
    : delay_time_(delay_time)
    , delay_function_(std::move(delay_function)) {
}

void Shell::loop_delay() {
    auto * delay_data = reinterpret_cast<Shell::DelayData *>(mode_data_.get());

    if (uuid::get_uptime_ms() >= delay_data->delay_time_) {
        auto function_copy = delay_data->delay_function_;

        mode_ = Mode::NORMAL;
        mode_data_.reset();

        function_copy(*this);

        if (running()) {
            display_prompt();
        }

        idle_time_ = uuid::get_uptime_ms();
    }
}

Shell::BlockingData::BlockingData(blocking_function && blocking_function)
    : blocking_function_(std::move(blocking_function)) {
}

void Shell::loop_blocking() {
    auto * blocking_data = reinterpret_cast<Shell::BlockingData *>(mode_data_.get());

    /* It is not possible to change mode while executing this function,
	 * because that would require copying either the std::shared_ptr or
	 * the std::function on every loop execution (to ensure that the
	 * function captures aren't destroyed while executing).
	 */
    if (blocking_data->blocking_function_(*this, blocking_data->stop_)) {
        bool stop_pending = blocking_data->stop_;

        mode_ = Mode::NORMAL;
        mode_data_.reset();

        if (stop_pending) {
            stop();
        }

        if (running()) {
            display_prompt();
        }

        idle_time_ = uuid::get_uptime_ms();
    }
}

void Shell::enter_password(const __FlashStringHelper * prompt, password_function function) {
    if (mode_ == Mode::NORMAL) {
        mode_      = Mode::PASSWORD;
        mode_data_ = std::make_unique<Shell::PasswordData>(prompt, std::move(function));
    }
}

void Shell::delay_for(unsigned long ms, delay_function function) {
    delay_until(uuid::get_uptime_ms() + ms, std::move(function));
}

void Shell::delay_until(uint64_t ms, delay_function function) {
    if (mode_ == Mode::NORMAL) {
        mode_      = Mode::DELAY;
        mode_data_ = std::make_unique<Shell::DelayData>(ms, std::move(function));
    }
}

void Shell::block_with(blocking_function function) {
    if (mode_ == Mode::NORMAL) {
        mode_      = Mode::BLOCKING;
        mode_data_ = std::make_unique<Shell::BlockingData>(std::move(function));
    }
}

void Shell::delete_buffer_word(bool display) {
    size_t pos = line_buffer_.find_last_of(' ');

    if (pos == std::string::npos) {
        line_buffer_.clear();
        if (display) {
            erase_current_line();
            prompt_displayed_ = false;
            display_prompt();
        }
    } else {
        if (display) {
            erase_characters(line_buffer_.length() - pos);
        }
        line_buffer_.resize(pos);
    }
}

size_t Shell::maximum_command_line_length() const {
    return maximum_command_line_length_;
}

void Shell::maximum_command_line_length(size_t length) {
    maximum_command_line_length_ = std::max((size_t)1, length);
    line_buffer_.reserve(maximum_command_line_length_);
}

void Shell::process_command() {
    CommandLine command_line{line_buffer_};

    line_buffer_.clear();
    println();
    prompt_displayed_ = false;

    if (!command_line->empty()) {
        if (commands_) {
            auto execution = commands_->execute_command(*this, std::move(command_line));

            if (execution.error != nullptr) {
                println(execution.error);
            }
        } else {
            println(F("No commands configured"));
        }
    }

    if (running()) {
        display_prompt();
    }
    ::yield();
}

void Shell::process_completion() {
    CommandLine command_line{line_buffer_};

    if (!command_line->empty() && commands_) {
        auto completion = commands_->complete_command(*this, command_line);
        bool redisplay  = false;

        if (!completion.help.empty()) {
            println();
            redisplay = true;

            for (auto & help : completion.help) {
                std::string help_line = help.to_string(maximum_command_line_length_);

                println(help_line);
            }
        }

        if (!completion.replacement->empty()) {
            if (!redisplay) {
                erase_current_line();
                prompt_displayed_ = false;
                redisplay         = true;
            }

            line_buffer_ = completion.replacement.to_string(maximum_command_line_length_);
        }

        if (redisplay) {
            display_prompt();
        }
    }

    ::yield();
}

void Shell::process_password(bool completed) {
    println();

    auto * password_data = reinterpret_cast<Shell::PasswordData *>(mode_data_.get());
    auto   function_copy = password_data->password_function_;

    mode_ = Mode::NORMAL;
    mode_data_.reset();

    function_copy(*this, completed, line_buffer_);
    line_buffer_.clear();

    if (running()) {
        display_prompt();
    }
}

void Shell::invoke_command(const std::string & line) {
    if (!line_buffer_.empty()) {
        println();
        prompt_displayed_ = false;
    }
    if (!prompt_displayed_) {
        display_prompt();
    }
    line_buffer_ = line;
    print(line_buffer_);
    process_command();
}

unsigned long Shell::idle_timeout() const {
    return idle_timeout_ / 1000;
}

void Shell::idle_timeout(unsigned long timeout) {
    idle_timeout_ = (uint64_t)timeout * 1000;
}

void Shell::check_idle_timeout() {
    if (idle_timeout_ > 0 && uuid::get_uptime_ms() - idle_time_ >= idle_timeout_) {
        println();
        stop();
    }
}

} // namespace console

} // namespace uuid
