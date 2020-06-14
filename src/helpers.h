/*
 * EMS-ESP - https://github.com/proddy/EMS-ESP
 * Copyright 2019  Paul Derbyshire
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

namespace emsesp {

class Helpers {
  public:
    static char *      hextoa(char * result, const uint8_t value);
    static std::string data_to_hex(const uint8_t * data, const uint8_t length);

    static char *   render_value(char * result, const float value, const uint8_t format); // format is the precision
    static char *   render_value(char * result, const uint8_t value, const uint8_t format);
    static char *   render_value(char * result, const int8_t value, const uint8_t format);
    static char *   render_value(char * result, const uint16_t value, const uint8_t format);
    static char *   render_value(char * result, const uint32_t value, const uint8_t format);
    static char *   render_value(char * result, const int16_t value, const uint8_t format);
    static char *   smallitoa(char * result, const uint8_t value);
    static char *   smallitoa(char * result, const uint16_t value);
    static char *   itoa(char * result, int16_t value, const uint8_t base = 10);
    static uint32_t hextoint(const char * hex);
    static uint16_t atoint(const char * value);

    static bool check_abs(const int32_t i) {
        return ((i < 0 ? -i : i) != 0xFFFFFF); // UNSET value is from telegram.h
    }

    static double round2(double value);
};

} // namespace emsesp

#endif