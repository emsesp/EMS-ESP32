/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020  Paul Derbyshire
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

#ifndef EMSESP_COMMON_H
#define EMSESP_COMMON_H

// logging
#include <uuid/log.h>

using uuid::log::Level;
#define LOG_DEBUG(...) logger_.debug(__VA_ARGS__)
#define LOG_INFO(...) logger_.info(__VA_ARGS__)
#define LOG_TRACE(...) logger_.trace(__VA_ARGS__)
#define LOG_NOTICE(...) logger_.notice(__VA_ARGS__)
#define LOG_WARNING(...) logger_.warning(__VA_ARGS__)
#define LOG_ERROR(...) logger_.err(__VA_ARGS__)

// flash strings
using uuid::flash_string_vector;
using uuid::read_flash_string;

#ifdef FPSTR
#undef FPSTR
#endif

#define FJSON(x) x
// #define FJSON(x) F(x)

// clang-format off

#define MAKE_STR(string_name, string_literal) static constexpr const char * __str__##string_name = string_literal;

#define FPSTR(pstr_pointer) (reinterpret_cast<const __FlashStringHelper *>(pstr_pointer))
#define F_(string_name) FPSTR(__pstr__##string_name)

#define MAKE_PSTR(string_name, string_literal) static const char __pstr__##string_name[] __attribute__((__aligned__(sizeof(uint32_t)))) PROGMEM = string_literal;
#define MAKE_PSTR_WORD(string_name) MAKE_PSTR(string_name, #string_name)

#define FL_(list_name) (__pstr__L_##list_name)
#define MAKE_PSTR_LIST(list_name, ...) static const __FlashStringHelper * const __pstr__L_##list_name[] PROGMEM = {__VA_ARGS__, nullptr};
#define MAKE_PSTR_ENUM(enum_name, ...) static const __FlashStringHelper * const * __pstr__L_##enum_name[] PROGMEM = {__VA_ARGS__, nullptr};

// clang-format on

// load translations
#include "locale_translations.h"
#include "locale_common.h"

#endif
