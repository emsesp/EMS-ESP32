/*
 * EMS-ESP - https://github.com/proddy/EMS-ESP
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

#ifndef EMSESP_HELPERS_H
#define EMSESP_HELPERS_H

#include <Arduino.h>
#include <uuid/common.h>

#include "telegram.h" // for EMS_VALUE_* settings

#define BOOL_FORMAT_ONOFF 1
#define BOOL_FORMAT_TRUEFALSE 2
#define BOOL_FORMAT_NUMBERS 3

// #define FJSON(x) x
#define FJSON(x) F(x)

namespace emsesp {

class Helpers {
  public:
    static char * render_value(char * result, const float value, const uint8_t format); // format is the precision
    static char * render_value(char * result, const uint8_t value, const uint8_t format);
    static char * render_value(char * result, const int8_t value, const uint8_t format);
    static char * render_value(char * result, const uint16_t value, const uint8_t format);
    static char * render_value(char * result, const uint32_t value, const uint8_t format);
    static char * render_value(char * result, const int16_t value, const uint8_t format);
    static char * render_value(char * result, const char * value, uint8_t format);
    static char * render_boolean(char * result, bool value);
    static char * render_enum(char * result, const std::vector<const __FlashStringHelper *> & value, const uint8_t no);

    static char *      hextoa(char * result, const uint8_t value);
    static std::string data_to_hex(const uint8_t * data, const uint8_t length);
    static char *      smallitoa(char * result, const uint8_t value);
    static char *      smallitoa(char * result, const uint16_t value);
    static char *      itoa(char * result, int32_t value, const uint8_t base = 10);
    static uint32_t    hextoint(const char * hex);
    static uint16_t    atoint(const char * value);
    static bool        check_abs(const int32_t i);
    static double      round2(double value);
    static std::string toLower(std::string const & s);

    static bool hasValue(const uint8_t & v, const uint8_t isBool = 0);
    static bool hasValue(const int8_t & v);
    static bool hasValue(const int16_t & v);
    static bool hasValue(const uint16_t & v);
    static bool hasValue(const uint32_t & v);

    static bool value2number(const char * v, int & value);
    static bool value2float(const char * v, float & value);
    static bool value2bool(const char * v, bool & value);
    static bool value2string(const char * v, std::string & value);
    static bool value2enum(const char * v, uint8_t & value, const std::vector<const __FlashStringHelper *> & strs);

    static void bool_format(uint8_t bool_format) {
        bool_format_ = bool_format;
    }

    static uint8_t bool_format() {
        return bool_format_;
    }

#ifdef EMSESP_STANDALONE
    static char * ultostr(char * ptr, uint32_t value, const uint8_t base);
#endif

  private:
    static uint8_t bool_format_;
};

} // namespace emsesp

#endif