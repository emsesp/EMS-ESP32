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

static const char * log_level_uppercase[(int)Level::ALL - (int)Level::OFF + 1] = {pstr_level_uppercase_off,
                                                                                  pstr_level_uppercase_emerg,
                                                                                  pstr_level_uppercase_crit,
                                                                                  pstr_level_uppercase_alert,
                                                                                  pstr_level_uppercase_err,
                                                                                  pstr_level_uppercase_warning,
                                                                                  pstr_level_uppercase_notice,
                                                                                  pstr_level_uppercase_info,
                                                                                  pstr_level_uppercase_debug,
                                                                                  pstr_level_uppercase_trace,
                                                                                  pstr_level_uppercase_all};

const char * format_level_uppercase(Level level) {
    return log_level_uppercase[(int)level + 1];
}

} // namespace log

} // namespace uuid
