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

#include <string>
#include <vector>

namespace uuid {

namespace console {

CommandLine::CommandLine(const std::string & line) {
    bool string_escape_double = false;
    bool string_escape_single = false;
    bool char_escape          = false;
    bool quoted_argument      = false;

    if (!line.empty()) {
        parameters_.emplace_back(std::string{});
    }

    for (char c : line) {
        switch (c) {
        case ' ':
            if (string_escape_double || string_escape_single) {
                if (char_escape) {
                    parameters_.back().push_back('\\');
                    char_escape = false;
                }
                parameters_.back().push_back(' ');
            } else if (char_escape) {
                parameters_.back().push_back(' ');
                char_escape = false;
            } else {
                // Begin a new argument if the previous
                // one is not empty or it was quoted
                if (quoted_argument || !parameters_.back().empty()) {
                    parameters_.emplace_back(std::string{});
                }
                quoted_argument = false;
            }
            break;

        case '"':
            if (char_escape || string_escape_single) {
                parameters_.back().push_back('"');
                char_escape = false;
            } else {
                string_escape_double = !string_escape_double;
                quoted_argument      = true;
            }
            break;

        case '\'':
            if (char_escape || string_escape_double) {
                parameters_.back().push_back('\'');
                char_escape = false;
            } else {
                string_escape_single = !string_escape_single;
                quoted_argument      = true;
            }
            break;

        case '\\':
            if (char_escape) {
                parameters_.back().push_back('\\');
                char_escape = false;
            } else {
                char_escape = true;
            }
            break;

        default:
            if (char_escape) {
                parameters_.back().push_back('\\');
                char_escape = false;
            }
            parameters_.back().push_back(c);
            break;
        }
    }

    if (!parameters_.empty() && parameters_.back().empty() && !quoted_argument) {
        parameters_.pop_back();
        if (!parameters_.empty()) {
            trailing_space = true;
        }
    }
}

CommandLine::CommandLine(std::initializer_list<const std::vector<std::string>> arguments) {
    for (auto & argument : arguments) {
        parameters_.insert(parameters_.end(), argument.begin(), argument.end());
    }
}

std::string CommandLine::to_string(size_t reserve) const {
    std::string line;
    size_t      escape = escape_parameters_;

    line.reserve(reserve);

    for (auto & item : parameters_) {
        if (!line.empty()) {
            line += ' ';
        }

        if (item.empty()) {
            line += '\"';
            line += '\"';
            goto next;
        }

        for (char c : item) {
            switch (c) {
            case ' ':
            case '\"':
            case '\'':
            case '\\':
                if (escape > 0) {
                    line += '\\';
                }
                break;
            }

            line += c;
        }

    next:
        if (escape > 0) {
            escape--;
        }
    }

    if (trailing_space && !line.empty()) {
        line += ' ';
    }

    return line;
}

void CommandLine::reset() {
    parameters_.clear();
    escape_all_parameters();
    trailing_space = false;
}

} // namespace console

} // namespace uuid
