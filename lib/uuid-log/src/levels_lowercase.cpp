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

std::vector<std::string> levels_lowercase() {
    return {uuid::read_flash_string(format_level_lowercase(Level::OFF)),
            uuid::read_flash_string(format_level_lowercase(Level::EMERG)),
            uuid::read_flash_string(format_level_lowercase(Level::ALERT)),
            uuid::read_flash_string(format_level_lowercase(Level::CRIT)),
            uuid::read_flash_string(format_level_lowercase(Level::ERR)),
            uuid::read_flash_string(format_level_lowercase(Level::WARNING)),
            uuid::read_flash_string(format_level_lowercase(Level::NOTICE)),
            uuid::read_flash_string(format_level_lowercase(Level::INFO)),
            uuid::read_flash_string(format_level_lowercase(Level::DEBUG)),
            uuid::read_flash_string(format_level_lowercase(Level::TRACE)),
            uuid::read_flash_string(format_level_lowercase(Level::ALL))};
}

} // namespace log

} // namespace uuid
