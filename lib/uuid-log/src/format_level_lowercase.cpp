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

static constexpr const char * pstr_level_lowercase_off     = "off";
static constexpr const char * pstr_level_lowercase_emerg   = "emerg";
static constexpr const char * pstr_level_lowercase_crit    = "crit";
static constexpr const char * pstr_level_lowercase_alert   = "alert";
static constexpr const char * pstr_level_lowercase_err     = "err";
static constexpr const char * pstr_level_lowercase_warning = "warning";
static constexpr const char * pstr_level_lowercase_notice  = "notice";
static constexpr const char * pstr_level_lowercase_info    = "info";
static constexpr const char * pstr_level_lowercase_debug   = "debug";
static constexpr const char * pstr_level_lowercase_trace   = "trace";
static constexpr const char * pstr_level_lowercase_all     = "all";

static const char * log_level_lowercase[(int)Level::ALL - (int)Level::OFF + 1] = {pstr_level_lowercase_off,
                                                                                  pstr_level_lowercase_emerg,
                                                                                  pstr_level_lowercase_crit,
                                                                                  pstr_level_lowercase_alert,
                                                                                  pstr_level_lowercase_err,
                                                                                  pstr_level_lowercase_warning,
                                                                                  pstr_level_lowercase_notice,
                                                                                  pstr_level_lowercase_info,
                                                                                  pstr_level_lowercase_debug,
                                                                                  pstr_level_lowercase_trace,
                                                                                  pstr_level_lowercase_all};

const char * format_level_lowercase(Level level) {
    return log_level_lowercase[(int)level + 1];
}

} // namespace log

} // namespace uuid
