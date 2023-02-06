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

#include <string>

namespace uuid {

namespace log {

static constexpr const char * pstr_level_uppercase_off     = "OFF";
static constexpr const char * pstr_level_uppercase_emerg   = "EMERG";
static constexpr const char * pstr_level_uppercase_crit    = "CRIT";
static constexpr const char * pstr_level_uppercase_alert   = "ALERT";
static constexpr const char * pstr_level_uppercase_err     = "ERR";
static constexpr const char * pstr_level_uppercase_warning = "WARNING";
static constexpr const char * pstr_level_uppercase_notice  = "NOTICE";
static constexpr const char * pstr_level_uppercase_info    = "INFO";
static constexpr const char * pstr_level_uppercase_debug   = "DEBUG";
static constexpr const char * pstr_level_uppercase_trace   = "TRACE";
static constexpr const char * pstr_level_uppercase_all     = "ALL";

static const __FlashStringHelper * log_level_uppercase[(int)Level::ALL - (int)Level::OFF + 1] __attribute__((__aligned__(sizeof(uint32_t))))
PROGMEM = {reinterpret_cast<const __FlashStringHelper *>(pstr_level_uppercase_off),
           reinterpret_cast<const __FlashStringHelper *>(pstr_level_uppercase_emerg),
           reinterpret_cast<const __FlashStringHelper *>(pstr_level_uppercase_crit),
           reinterpret_cast<const __FlashStringHelper *>(pstr_level_uppercase_alert),
           reinterpret_cast<const __FlashStringHelper *>(pstr_level_uppercase_err),
           reinterpret_cast<const __FlashStringHelper *>(pstr_level_uppercase_warning),
           reinterpret_cast<const __FlashStringHelper *>(pstr_level_uppercase_notice),
           reinterpret_cast<const __FlashStringHelper *>(pstr_level_uppercase_info),
           reinterpret_cast<const __FlashStringHelper *>(pstr_level_uppercase_debug),
           reinterpret_cast<const __FlashStringHelper *>(pstr_level_uppercase_trace),
           reinterpret_cast<const __FlashStringHelper *>(pstr_level_uppercase_all)};

const __FlashStringHelper * format_level_uppercase(Level level) {
    return log_level_uppercase[(int)level + 1];
}

} // namespace log

} // namespace uuid
