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

Shell::Shell(Stream & stream, std::shared_ptr<Commands> commands, unsigned int context, unsigned int flags)
    : stream_(stream)
    , commands_(std::move(commands))
    , flags_(flags) {
    enter_context(context);
}

void Shell::started() {
}

bool Shell::running() const {
    return !stopped_;
}

void Shell::start() {
    // added for EMS-ESP
#if defined(EMSESP_DEBUG)
    uuid::log::Logger::register_handler(this, uuid::log::Level::DEBUG); // added for EMS-ESP
#else
    uuid::log::Logger::register_handler(this, uuid::log::Level::INFO);
#endif

    line_buffer_.reserve(maximum_command_line_length_);
    for (uint8_t i = 0; i < MAX_LINES; i++) {
        line_old_[i].reserve(maximum_command_line_length_);
        line_old_[i].clear();
    }
    display_banner();
    display_prompt();
    registered_shells().insert(shared_from_this());
    idle_time_ = uuid::get_uptime_ms();
    started();
};

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

Commands::AvailableCommands Shell::available_commands() const {
    return commands_->available_commands(*this);
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

void Shell::set_command_str(const char * str) {
    line_buffer_ = (str);
    erase_current_line();
    prompt_displayed_ = false;
    display_prompt();
    process_command();
}

void Shell::loop_normal() {
    const int input = stream_.read();

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
        cursor_  = 0;
        line_no_ = 0;
        // prompt_displayed_ = false;
        // display_prompt();
        break;

    case '\x04':
        // End of transmission (^D)
        if (line_buffer_.empty()) {
            end_of_transmission();
        }
        break;

    case '\x08':
        // Backspace (^H)
    case '\x7F':
        // Del/Backspace (^?)
        if (line_buffer_.length() > cursor_) {
            line_buffer_.erase(line_buffer_.length() - cursor_ - 1, 1);
            line_no_ = 0;
        }
        break;

    case '\x09':
        // Tab (^I)
        process_completion();
        cursor_  = 0;
        line_no_ = 0;
        break;

    case '\x0A':
        // Line feed (^J)
        if (previous_ != '\x0D') {
            process_command();
        }
        break;

    case '\x0D':
        // Carriage return (^M)
        process_command();
        break;

    case '\x0C':
        // New page (^L)
    case '\x15':
        // Delete line (^U)
        line_buffer_.clear();
        cursor_  = 0;
        line_no_ = 0;
        break;

    case '\x17':
        // Delete word (^W)
        delete_buffer_word(true);
        line_no_ = 0;
        break;

    case '\033':
        // esc
        esc_ = 0x80;
        break;

    default:
        if (esc_) {
            if (c == 'A') { // cursor up
                line_buffer_ = line_old_[line_no_];
                if (line_no_ < MAX_LINES - 1) {
                    line_no_++;
                }
                cursor_ = 0;
            } else if (c == 'B') { // cursor down
                if (line_no_) {
                    line_no_--;
                }
                if (line_no_) {
                    line_buffer_ = line_old_[line_no_ - 1];
                } else {
                    line_buffer_.clear();
                }
                cursor_ = 0;
            } else if (c == 'C') { // cursor  right
                if (cursor_) {
                    cursor_--;
                }
            } else if (c == 'D') { // cursor left
                if (cursor_ < line_buffer_.length()) {
                    cursor_++;
                }
            } else if (c == 'H') { // Home
                cursor_ = line_buffer_.length();
            } else if (c == 'F') { // End
                cursor_ = 0;
            } else if (c >= 'P' && c <= 'Z') { // F1 - F11, Linux, VT100
                // set esc-number like ESCn~
                esc_ = 11 + c - 'P';
            }
            if (c == '~' || (c >= 'P' && c <= 'Z')) { // function keys with number ESCn~
                if ((esc_ == 3) && cursor_) {         // del
                    cursor_--;
                    line_buffer_.erase(line_buffer_.length() - cursor_ - 1, 1);
                    line_no_ = 0;
                } else if (esc_ == 4) { // end
                    cursor_ = 0;
                } else if (esc_ == 1) { // pos1
                    cursor_ = line_buffer_.length();
                } else if (esc_ == 11) { // F1
                    set_command_str("help");
                } else if (esc_ == 12) { // F2
                    set_command_str("show values");
                } else if (esc_ == 13) { // F3
                    set_command_str("log notice");
                } else if (esc_ == 14) { // F4
                    set_command_str("log info");
                } else if (esc_ == 15) { // F5
                    set_command_str("log debug");
                } else if (esc_ == 17) { // F6
                    set_command_str("watch off");
                } else if (esc_ == 18) { // F7
                    set_command_str("watch on");
                } else if (esc_ == 19) { // F8
                    set_command_str("watch raw");
                } else if (esc_ == 20) { // F9
                    set_command_str("call system info");
                } else if (esc_ == 21) { // F10
                    set_command_str("call system settings");
                } else if (esc_ == 23) { // F11
                    line_buffer_ = ("call send \"0B \"");
                    cursor_      = 1;
                } else if (esc_ == 24) { // F12
                    set_command_str("log debug; watch raw");
                }
                esc_ = 0;
            } else if (c >= '0' && (c <= '9')) { // numbers
                esc_ = (esc_ & 0x7F) * 10 + c - '0';
            } else if ((c != '[') && (c != 'O')) { // all other chars except start of sequence
                esc_ = 0;
            }
            // process normal ascii text
        } else if (c >= '\x20' && c <= '\x7E') {
            if (line_buffer_.length() < maximum_command_line_length_) {
                line_buffer_.insert(line_buffer_.length() - cursor_, 1, c);
                line_no_ = 0;
            }
        }
        break;
    }

#ifndef EMSESP_STANDALONE
    // added for EMS-ESP. Display the complete line
    erase_current_line();
    prompt_displayed_ = false;
    display_prompt();
    if (cursor_) {
        printf("\033[%dD", cursor_);
    }
#endif

    previous_ = c;

    // This is a hack to let TelnetStream know that command
    // execution is complete and that output can be flushed.
    stream_.available();

    idle_time_ = uuid::get_uptime_ms();
}

Shell::PasswordData::PasswordData(const char * password_prompt, password_function && password_function)
    : password_prompt_(password_prompt)
    , password_function_(std::move(password_function)) {
}

void Shell::loop_password() {
    const int input = stream_.read();

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
    stream_.available();

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

void Shell::enter_password(const char * prompt, password_function function) {
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
        cursor_ = 0;
    } else {
        if (display) {
            size_t pos1 = 0;
            pos         = 0;
            while (pos1 < line_buffer_.length() - cursor_) {
                pos  = pos1;
                pos1 = line_buffer_.find(' ', pos + 1);
            }
            line_buffer_.erase(pos, pos1 - pos);
            if (line_buffer_.find(' ') == 0) {
                line_buffer_.erase(0, 1);
            }
            cursor_ = line_buffer_.length() - pos;
        } else {
            line_buffer_.resize(pos);
            cursor_ = 0;
        }
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
    // added for EMS-ESP
    if (line_buffer_.empty()) {
#ifndef EMSESP_STANDALONE
        println();
#else
        display_prompt();
#endif
        return;
    }

    uint8_t no = line_no_ ? line_no_ : MAX_LINES;
    while (--no) {
        line_old_[no] = line_old_[no - 1];
    }

    line_no_     = 0;
    line_old_[0] = line_buffer_;
    while (!line_buffer_.empty()) {
        size_t      pos = line_buffer_.find(';');
        std::string line1;
        if (pos < line_buffer_.length()) {
            line1 = line_buffer_.substr(0, pos);
            line_buffer_.erase(0, pos + 1);
        } else {
            line1 = line_buffer_;
            line_buffer_.clear();
            cursor_ = 0;
        }

        CommandLine command_line{line1};

#ifndef EMSESP_STANDALONE
        println();
#endif
        prompt_displayed_ = false;

        if (!command_line->empty()) {
            if (commands_) {
                auto execution = commands_->execute_command(*this, std::move(command_line));

                if (execution.error != nullptr) {
                    println(execution.error);
                }
            } else {
                println("No commands configured");
            }
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
    erase_current_line();
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
