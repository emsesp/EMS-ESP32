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

namespace uuid {

namespace console {

int Shell::available() {
    if (mode_ == Mode::BLOCKING) {
        auto * blocking_data = reinterpret_cast<Shell::BlockingData *>(mode_data_.get());

        if (!available_char()) {
            return 0;
        }

        if (blocking_data->consume_line_feed_) {
            const int input = peek_one_char();

            if (input >= 0) {
                const unsigned char c = input;

                blocking_data->consume_line_feed_ = false;

                if (previous_ == '\x0D' && c == '\x0A') {
                    // Consume the first LF following a CR
                    read_one_char();
                    previous_ = c;
                    return available();
                }
            } else {
                // The underlying stream has not implemented peek,
                // so the next read() could return -1 if LF is
                // filtered out.
            }
        }

        return 1;
    } else {
        return 0;
    }
}

int Shell::read() {
    if (mode_ == Mode::BLOCKING) {
        auto *    blocking_data = reinterpret_cast<Shell::BlockingData *>(mode_data_.get());
        const int input         = read_one_char();

        if (input >= 0) {
            const unsigned char c = input;

            if (blocking_data->consume_line_feed_) {
                blocking_data->consume_line_feed_ = false;

                if (previous_ == '\x0D' && c == '\x0A') {
                    // Consume the first LF following a CR
                    previous_ = c;
                    return read();
                }
            }

            // Track read characters so that a final CR means we ignore the next LF
            previous_ = c;
        }

        return input;
    } else {
        return -1;
    }
}

int Shell::peek() {
    if (mode_ == Mode::BLOCKING) {
        auto *    blocking_data = reinterpret_cast<Shell::BlockingData *>(mode_data_.get());
        const int input         = peek_one_char();

        if (blocking_data->consume_line_feed_) {
            if (input >= 0) {
                const unsigned char c = input;

                blocking_data->consume_line_feed_ = false;

                if (previous_ == '\x0D' && c == '\x0A') {
                    // Consume the first LF following a CR
                    read_one_char();
                    previous_ = c;
                    return peek();
                }
            }
        }

        return input;
    } else {
        return -1;
    }
}

void Shell::flush() {
    // This is a pure virtual function in Arduino's Stream class, which
    // makes no sense because that class is for input and this is an
    // output function. Later versions move it to Print as an empty
    // virtual function so this is here for backward compatibility.
}

} // namespace console

} // namespace uuid
