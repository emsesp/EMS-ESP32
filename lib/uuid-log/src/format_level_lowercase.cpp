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

static constexpr const char * pstr_level_lowercase_off __attribute__((__aligned__(sizeof(int)))) PROGMEM     = "off";
static constexpr const char * pstr_level_lowercase_emerg __attribute__((__aligned__(sizeof(int)))) PROGMEM   = "emerg";
static constexpr const char * pstr_level_lowercase_crit __attribute__((__aligned__(sizeof(int)))) PROGMEM    = "crit";
static constexpr const char * pstr_level_lowercase_alert __attribute__((__aligned__(sizeof(int)))) PROGMEM   = "alert";
static constexpr const char * pstr_level_lowercase_err __attribute__((__aligned__(sizeof(int)))) PROGMEM     = "err";
static constexpr const char * pstr_level_lowercase_warning __attribute__((__aligned__(sizeof(int)))) PROGMEM = "warning";
static constexpr const char * pstr_level_lowercase_notice __attribute__((__aligned__(sizeof(int)))) PROGMEM  = "notice";
static constexpr const char * pstr_level_lowercase_info __attribute__((__aligned__(sizeof(int)))) PROGMEM    = "info";
static constexpr const char * pstr_level_lowercase_debug __attribute__((__aligned__(sizeof(int)))) PROGMEM   = "debug";
static constexpr const char * pstr_level_lowercase_trace __attribute__((__aligned__(sizeof(int)))) PROGMEM   = "trace";
static constexpr const char * pstr_level_lowercase_all __attribute__((__aligned__(sizeof(int)))) PROGMEM     = "all";

static const __FlashStringHelper * log_level_lowercase[(int)Level::ALL - (int)Level::OFF + 1] __attribute__((__aligned__(sizeof(int))))
PROGMEM = {reinterpret_cast<const __FlashStringHelper *>(pstr_level_lowercase_off),
           reinterpret_cast<const __FlashStringHelper *>(pstr_level_lowercase_emerg),
           reinterpret_cast<const __FlashStringHelper *>(pstr_level_lowercase_crit),
           reinterpret_cast<const __FlashStringHelper *>(pstr_level_lowercase_alert),
           reinterpret_cast<const __FlashStringHelper *>(pstr_level_lowercase_err),
           reinterpret_cast<const __FlashStringHelper *>(pstr_level_lowercase_warning),
           reinterpret_cast<const __FlashStringHelper *>(pstr_level_lowercase_notice),
           reinterpret_cast<const __FlashStringHelper *>(pstr_level_lowercase_info),
           reinterpret_cast<const __FlashStringHelper *>(pstr_level_lowercase_debug),
           reinterpret_cast<const __FlashStringHelper *>(pstr_level_lowercase_trace),
           reinterpret_cast<const __FlashStringHelper *>(pstr_level_lowercase_all)};

const __FlashStringHelper * format_level_lowercase(Level level) {
    return log_level_lowercase[(int)level + 1];
}

} // namespace log

} // namespace uuid
