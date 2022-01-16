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

#ifndef EMSESP_HELPERS_H
#define EMSESP_HELPERS_H

#include "telegram.h" // for EMS_VALUE_* settings

#define FJSON(x) x
// #define FJSON(x) F(x)

namespace emsesp {

using flash_string_vector = std::vector<const __FlashStringHelper *>;

class Helpers {
  public:
    static char * render_value(char * result, const float value, const int8_t format); // format is the precision
    static char * render_value(char * result, const uint8_t value, const int8_t format, const uint8_t fahrenheit = 0);
    static char * render_value(char * result, const int8_t value, const int8_t format, const uint8_t fahrenheit = 0);
    static char * render_value(char * result, const uint16_t value, const int8_t format, const uint8_t fahrenheit = 0);
    static char * render_value(char * result, const uint32_t value, const int8_t format, const uint8_t fahrenheit = 0);
    static char * render_value(char * result, const int16_t value, const int8_t format, const uint8_t fahrenheit = 0);
    static char * render_value(char * result, const int32_t value, const int8_t format, const uint8_t fahrenheit = 0);
    static char * render_value(char * result, const char * value, const int8_t format);
    static char * render_boolean(char * result, bool value);

    static char *      hextoa(char * result, const uint8_t value);
    static char *      hextoa(char * result, const uint16_t value);
    static std::string hextoa(const uint8_t value, bool prefix = true);  // default prefix with 0x
    static std::string hextoa(const uint16_t value, bool prefix = true); // default prefix with 0x
    static std::string data_to_hex(const uint8_t * data, const uint8_t length);
    static char *      smallitoa(char * result, const uint8_t value);
    static char *      smallitoa(char * result, const uint16_t value);
    static char *      itoa(int32_t value, char * result, const uint8_t base = 10);
    static std::string itoa(int16_t value);
    static uint32_t    hextoint(const char * hex);
    static int         atoint(const char * value);
    static bool        check_abs(const int32_t i);
    static uint32_t    abs(const int32_t i);
    static float       round2(float value, const int8_t divider, const uint8_t fahrenheit = 0);
    static std::string toLower(std::string const & s);
    static std::string toUpper(std::string const & s);
    static void        replace_char(char * str, char find, char replace);

    static bool hasValue(const uint8_t & v, const uint8_t isBool = 0);
    static bool hasValue(const int8_t & v);
    static bool hasValue(const int16_t & v);
    static bool hasValue(const uint16_t & v);
    static bool hasValue(const uint32_t & v);
    static bool hasValue(char * v);

    static bool value2number(const char * v, int & value, const int min = -2147483648, const int max = 2147483647);
    static bool value2float(const char * v, float & value);
    static bool value2bool(const char * v, bool & value);
    static bool value2string(const char * v, std::string & value);
    static bool value2enum(const char * v, uint8_t & value, const __FlashStringHelper * const * strs);
    static bool value2temperature(const char * v, float & value, bool relative = false);
    static bool value2temperature(const char * v, int & value, const bool relative = false, const int min = -2147483648, const int max = 2147483647);

#ifdef EMSESP_STANDALONE
    static char * ultostr(char * ptr, uint32_t value, const uint8_t base);
#endif
};

} // namespace emsesp

#endif