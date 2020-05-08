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

#include <memory>
#include <string>

namespace uuid {

namespace console {

StreamConsole::StreamConsole(Stream & stream)
    : Shell()
    , stream_(stream) {
}

// cppcheck-suppress passedByValue
StreamConsole::StreamConsole(std::shared_ptr<Commands> commands, Stream & stream, unsigned int context, unsigned int flags)
    : Shell(std::move(commands), context, flags)
    , stream_(stream) {
}

size_t StreamConsole::write(uint8_t data) {
    return stream_.write(data);
}

size_t StreamConsole::write(const uint8_t * buffer, size_t size) {
    return stream_.write(buffer, size);
}

bool StreamConsole::available_char() {
    return stream_.available() > 0;
}

int StreamConsole::read_one_char() {
    return stream_.read();
}

int StreamConsole::peek_one_char() {
    return stream_.peek();
}

} // namespace console

} // namespace uuid
