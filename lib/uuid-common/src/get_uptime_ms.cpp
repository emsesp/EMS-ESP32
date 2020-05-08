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

namespace uuid {

uint64_t get_uptime_ms() {
    static uint32_t high_millis = 0;
    static uint32_t low_millis  = 0;

    uint32_t now_millis = ::millis();

    if (now_millis < low_millis) {
        high_millis++;
    }

    low_millis = now_millis;

    return ((uint64_t)high_millis << 32) | low_millis;
}

} // namespace uuid
