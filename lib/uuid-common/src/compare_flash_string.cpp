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

#include <uuid/common.h>

// #ifdef ESP8266
// #include <pgmspace.h>
// #else
// #include <avr/pgmspace.h>
// #endif

#include <Arduino.h>

namespace uuid {

// On ESP8266, pgm_read_byte() already takes care of 4-byte alignment, and
// memcpy_P(s, p, 4) makes 4 calls to pgm_read_byte() anyway, so don't bother
// optimizing for 4-byte alignment here.

// class __FlashStringHelper;

int compare_flash_string(const __FlashStringHelper * a, const __FlashStringHelper * b) {
    const char * aa = reinterpret_cast<const char *>(a);
    const char * bb = reinterpret_cast<const char *>(b);

    while (true) {
        uint8_t ca = pgm_read_byte(aa);
        uint8_t cb = pgm_read_byte(bb);
        if (ca != cb)
            return (int)ca - (int)cb;
        if (ca == 0)
            return 0;
        aa++;
        bb++;
    }
}

int compare_flash_string(const __FlashStringHelper * a, const __FlashStringHelper * b, size_t n) {
    const char * aa = reinterpret_cast<const char *>(a);
    const char * bb = reinterpret_cast<const char *>(b);

    while (n > 0) {
        uint8_t ca = pgm_read_byte(aa);
        uint8_t cb = pgm_read_byte(bb);
        if (ca != cb)
            return (int)ca - (int)cb;
        if (ca == 0)
            return 0;
        aa++;
        bb++;
        n--;
    }
    return 0;
}

} // namespace uuid
