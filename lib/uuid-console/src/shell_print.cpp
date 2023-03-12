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

#include <string>

namespace uuid {

namespace console {

size_t Shell::print(const std::string & data) {
    if (data.empty()) {
        return 0;
    } else {
        return write(reinterpret_cast<const uint8_t *>(data.c_str()), data.length());
    }
}

size_t Shell::println(const std::string & data) {
    size_t len = print(data);
    len += println();
    return len;
}

size_t Shell::printf(const char * format, ...) {
    va_list ap;

    va_start(ap, format);
    size_t len = vprintf(format, ap);
    va_end(ap);

    return len;
}

size_t Shell::printfln(const char * format, ...) {
    va_list ap;

    va_start(ap, format);
    size_t len = vprintf(format, ap);
    va_end(ap);

    len += println();
    return len;
}

size_t Shell::vprintf(const char * format, va_list ap) {
    size_t  print_len = 0;
    va_list copy_ap;

    va_copy(copy_ap, ap);

    int format_len = ::vsnprintf(nullptr, 0, format, ap);
    if (format_len > 0) {
        std::string text(static_cast<std::string::size_type>(format_len), '\0');

        ::vsnprintf(&text[0], text.capacity() + 1, format, copy_ap);
        print_len = print(text);
    }

    va_end(copy_ap);
    return print_len;
}

void Shell::print_all_available_commands() {
    for (auto & available_command : available_commands()) {
        CommandLine command_line{available_command.name(), available_command.arguments()};

        command_line.escape_initial_parameters(available_command.name().size());
        println(command_line.to_string(maximum_command_line_length()));
    }
}

void Shell::erase_current_line() {
    print("\033[0G\033[K");
}

void Shell::erase_characters(size_t count) {
    print(std::string(count, '\x08'));
    print("\033[K");
}

} // namespace console

} // namespace uuid