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

#define UPTIME_OVERFLOW 4294967295 // Uptime overflow value

// returns system uptime in seconds
uint32_t get_uptime_sec() {
    static uint32_t last_uptime      = 0;
    static uint8_t  uptime_overflows = 0;

    if (millis() < last_uptime) {
        ++uptime_overflows;
    }
    last_uptime             = millis();
    uint32_t uptime_seconds = uptime_overflows * (UPTIME_OVERFLOW / 1000) + (last_uptime / 1000);

    return uptime_seconds;
}

uint64_t get_uptime_ms() {
    static uint32_t high_millis = 0;
    static uint32_t low_millis  = 0;

    if (get_uptime() < low_millis) {
        high_millis++;
    }

    low_millis = get_uptime();

    return ((uint64_t)high_millis << 32) | low_millis;
}

// added by proddy
static uint32_t now_millis;

void set_uptime() {
    now_millis = ::millis();
}

uint32_t get_uptime() {
    return now_millis;
}

} // namespace uuid
