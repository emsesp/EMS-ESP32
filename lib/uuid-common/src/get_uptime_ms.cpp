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

// added by proddy for EMS-ESP
static uint64_t now_millis = 0;

// returns system uptime in seconds
uint32_t get_uptime_sec() {
    return (uint32_t)(now_millis / 1000ULL);
}

uint64_t get_uptime_ms() {
    return now_millis;
}

void set_uptime() {
    now_millis = esp_timer_get_time() / 1000ULL;
}

uint32_t get_uptime() {
    return (uint32_t)now_millis;
}

} // namespace uuid
