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

#include <Arduino.h>

#include <cstdint>
#include <string>

namespace uuid {

namespace log {

std::string format_timestamp_ms(uint64_t timestamp_ms, unsigned int days_width) {
    unsigned long days;
    unsigned int  hours, minutes, seconds, milliseconds;

    days = timestamp_ms / 86400000UL;
    timestamp_ms %= 86400000UL;

    hours = timestamp_ms / 3600000UL;
    timestamp_ms %= 3600000UL;

    minutes = timestamp_ms / 60000UL;
    timestamp_ms %= 60000UL;

    seconds = timestamp_ms / 1000UL;
    timestamp_ms %= 1000UL;

    milliseconds = timestamp_ms;

    static std::vector<char> text(10 + 1 /* days */ + 2 + 1 /* hours */ + 2 + 1 /* minutes */ + 2 + 1 /* seconds */ + 3 /* milliseconds */ + 1);

    snprintf_P(text.data(), text.size(), PSTR("%0*lu+%02u:%02u:%02u.%03u"), std::min(days_width, 10U), days, hours, minutes, seconds, milliseconds);

    return text.data();
}

} // namespace log

} // namespace uuid
