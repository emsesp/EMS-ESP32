/*
 * uuid-log - Microcontroller logging framework
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

#include <uuid/log.h>

namespace uuid {

namespace log {

char format_level_char(Level level) {
    constexpr char log_level_chars[(int)Level::ALL - (int)Level::OFF + 1] = {' ', 'P', 'A', 'C', 'E', 'W', 'N', 'I', 'D', 'T', ' '};
    return log_level_chars[(int)level + 1];
}

} // namespace log

} // namespace uuid
