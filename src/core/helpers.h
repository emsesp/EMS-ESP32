/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2024  emsesp.org - proddy, MichaelDvP
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
#include "common.h"

namespace emsesp {

class Helpers {
  public:
    static char * render_value(char * result, const double value, const int8_t format); // format is the precision
    static char * render_value(char * result, const uint8_t value, const int8_t format, const uint8_t fahrenheit = 0);
    static char * render_value(char * result, const int8_t value, const int8_t format, const uint8_t fahrenheit = 0);
    static char * render_value(char * result, const uint16_t value, const int8_t format, const uint8_t fahrenheit = 0);
    static char * render_value(char * result, const uint32_t value, const int8_t format, const uint8_t fahrenheit = 0);
    static char * render_value(char * result, const int16_t value, const int8_t format, const uint8_t fahrenheit = 0);
    static char * render_value(char * result, const int32_t value, const int8_t format, const uint8_t fahrenheit = 0);
    static char * render_boolean(char * result, const bool value, const bool dashboard = false);

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
    static uint16_t    string2minutes(const std::string & str);
    static float       numericoperator2scalefactor(int8_t numeric_operator);

    static double transformNumFloat(double value, const int8_t numeric_operator = 0, const uint8_t fahrenheit = 0);

    static std::string toLower(std::string const & s);
    static std::string toUpper(std::string const & s);
    static std::string toLower(const char * s);
    static void        CharToUpperUTF8(char * c);

    static void replace_char(char * str, char find, char replace);

    static bool hasValue(const uint8_t & value, const uint8_t isBool = 0);
    static bool hasValue(const int8_t & value);
    static bool hasValue(const int16_t & value);
    static bool hasValue(const uint16_t & value);
    static bool hasValue(const uint32_t & value);
    static bool hasValue(const char * value);

    static bool value2number(const char * value, int & value_i, const int min = -2147483648, const int max = 2147483647);
    static bool value2float(const char * value, float & value_f);
    static bool value2bool(const char * value, bool & value_b);
    static bool value2string(const char * value, std::string & value_s);
    static bool value2enum(const char * value, uint8_t & value_ui, const char * const ** strs);
    static bool value2enum(const char * value, uint8_t & value_ui, const char * const * strs);
    static bool value2temperature(const char * value, float & value_f, bool relative = false);
    static bool value2temperature(const char * value, int & value_i, const bool relative = false, const int min = -2147483648, const int max = 2147483647);

    static uint8_t count_items(const char * const ** list);
    static uint8_t count_items(const char * const * list);

    static const char * translated_word(const char * const * strings, const bool force_en = false);

#ifdef EMSESP_STANDALONE
    static char * ultostr(char * ptr, uint32_t value, const uint8_t base);
#endif
};

} // namespace emsesp

#endif