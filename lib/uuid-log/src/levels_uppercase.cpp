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

#include <vector>

#include <uuid/common.h>

namespace uuid {

namespace log {

std::vector<std::string> levels_uppercase() {
    return {format_level_uppercase(Level::OFF),
            format_level_uppercase(Level::EMERG),
            format_level_uppercase(Level::ALERT),
            format_level_uppercase(Level::CRIT),
            format_level_uppercase(Level::ERR),
            format_level_uppercase(Level::WARNING),
            format_level_uppercase(Level::NOTICE),
            format_level_uppercase(Level::INFO),
            format_level_uppercase(Level::DEBUG),
            format_level_uppercase(Level::TRACE),
            format_level_uppercase(Level::ALL)};
}

} // namespace log

} // namespace uuid
