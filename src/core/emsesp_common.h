/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2026  emsesp.org
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

#include <uuid/log.h>

using uuid::log::Level;

// Log macros gate on logger_.enabled(level) so that expensive argument
// expressions (e.g. pretty_telegram(...).c_str()) are not evaluated when
// the level is filtered out. Without this, every LOG_TRACE on the RX path
// allocates a std::string even when no handler is interested.
#if defined(EMSESP_DEBUG)
#define LOG_DEBUG(...) (logger_.enabled(uuid::log::Level::DEBUG) ? logger_.debug(__VA_ARGS__) : (void)0)
#else
#define LOG_DEBUG(...) ((void)0)
#endif

#define LOG_INFO(...) (logger_.enabled(uuid::log::Level::INFO) ? logger_.info(__VA_ARGS__) : (void)0)
#define LOG_TRACE(...) (logger_.enabled(uuid::log::Level::TRACE) ? logger_.trace(__VA_ARGS__) : (void)0)
#define LOG_NOTICE(...) (logger_.enabled(uuid::log::Level::NOTICE) ? logger_.notice(__VA_ARGS__) : (void)0)
#define LOG_WARNING(...) (logger_.enabled(uuid::log::Level::WARNING) ? logger_.warning(__VA_ARGS__) : (void)0)
#define LOG_ERROR(...) (logger_.enabled(uuid::log::Level::ERR) ? logger_.err(__VA_ARGS__) : (void)0)

// flash strings
using uuid::string_vector;
using string_vector = std::vector<const char *>;

#ifdef FPSTR
#undef FPSTR
#endif

// clang-format off
 
 #define FPSTR(pstr_pointer) pstr_pointer
 #define MAKE_WORD_CUSTOM(string_name, string_literal) static const char __pstr__##string_name[] = string_literal;
 #define MAKE_WORD(string_name) MAKE_WORD_CUSTOM(string_name, #string_name)
 
 #define F_(string_name) (__pstr__##string_name)
 #define FL_(list_name) (__pstr__L_##list_name)
 
 // Translation counter - capture baseline before any MAKE_TRANSLATION calls
 enum { EMSESP_TRANSLATION_COUNT_START = __COUNTER__ };

 // The language settings below must match system.cpp
 #if defined(EMSESP_EN_ONLY)
 // EN only
 #define MAKE_WORD_TRANSLATION(list_name, en, ...)       static const char * const __pstr__L_##list_name[] = {en, nullptr};
 #define MAKE_TRANSLATION(list_name, shortname, en, ...) static constexpr int __translation_counter_##list_name = __COUNTER__; static const char * const __pstr__L_##list_name[] = {shortname, en, nullptr};
 #elif defined(EMSESP_TEST) || defined(EMSESP_DE_ONLY)
 // EN + DE (Test mode uses two languages to save flash memory)
 #define MAKE_WORD_TRANSLATION(list_name, en, de, ...)       static const char * const __pstr__L_##list_name[] = {en, de, nullptr};
 #define MAKE_TRANSLATION(list_name, shortname, en, de, ...) static constexpr int __translation_counter_##list_name = __COUNTER__; static const char * const __pstr__L_##list_name[] = {shortname, en, de, nullptr};
 #else
 // All languages
 #define MAKE_WORD_TRANSLATION(list_name, ...) static const char * const __pstr__L_##list_name[] = {__VA_ARGS__, nullptr};
 #define MAKE_TRANSLATION(list_name, ...)      static constexpr int __translation_counter_##list_name = __COUNTER__; static const char * const __pstr__L_##list_name[] = {__VA_ARGS__, nullptr};
 #endif
 
 #define MAKE_NOTRANSLATION(list_name, ...) static const char * const __pstr__L_##list_name[] = {__VA_ARGS__, nullptr};
 
 // fixed strings, no translations
 #define MAKE_ENUM_FIXED(enum_name, ...) static const char * const __pstr__L_##enum_name[] = {__VA_ARGS__, nullptr};
 
 // with translations
 #define MAKE_ENUM(enum_name, ...)       static const char * const * __pstr__L_##enum_name[] = {__VA_ARGS__, nullptr};

// clang-format on

// load translations
#include "locale_translations.h"
#include "locale_common.h"

// Translation count - dynamically calculated at compile-time
enum { EMSESP_TRANSLATION_COUNT_END = __COUNTER__ };
static constexpr uint16_t EMSESP_TRANSLATION_COUNT = static_cast<int>(EMSESP_TRANSLATION_COUNT_END) - static_cast<int>(EMSESP_TRANSLATION_COUNT_START) - 1;

#endif
