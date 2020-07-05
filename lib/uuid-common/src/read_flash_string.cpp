/*
 * uuid-common - Microcontroller common utilities
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

#include <uuid/common.h>

#include <Arduino.h>

#include <string>

namespace uuid {

std::string read_flash_string(const __FlashStringHelper * flash_str) {
    std::string str(::strlen_P(reinterpret_cast<PGM_P>(flash_str)), '\0');

    ::strncpy_P(&str[0], reinterpret_cast<PGM_P>(flash_str), str.capacity() + 1);

    return str;
}

} // namespace uuid
