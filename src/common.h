/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2023  Paul Derbyshire
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
using uuid::string_vector;
using string_vector = std::vector<const char *>;

#ifdef FPSTR
#undef FPSTR
#endif

// clang-format off

#define FPSTR(pstr_pointer) pstr_pointer
#define MAKE_PSTR(string_name, string_literal) static const char __pstr__##string_name[] = string_literal;
#define MAKE_PSTR_WORD(string_name) MAKE_PSTR(string_name, #string_name)

#define F_(string_name) (__pstr__##string_name)
#define FL_(list_name) (__pstr__L_##list_name)

#ifdef EMSESP_DEBUG
// In debug mode just take one language (en) to save Flash space
#define MAKE_PSTR_LIST(list_name, en, ...) static const char * const __pstr__L_##list_name[]  = {en, nullptr};
#else
#define MAKE_PSTR_LIST(list_name, ...) static const char * const __pstr__L_##list_name[]  = {__VA_ARGS__, nullptr};
#endif

#define MAKE_PSTR_ENUM(enum_name, ...) static const char * const * __pstr__L_##enum_name[]  = {__VA_ARGS__, nullptr};

// clang-format on

// load translations
#include "locale_translations.h"
#include "locale_common.h"

#endif
