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

#include "helpers.h"
#include "mqtt.h"

namespace emsesp {

// like itoa but for hex, and quicker
// note: only for single byte hex values
char * Helpers::hextoa(char * result, const uint8_t value) {
    char *  p    = result;
    uint8_t nib1 = (value >> 4) & 0x0F;
    uint8_t nib2 = (value >> 0) & 0x0F;
    *p++         = nib1 < 0xA ? '0' + nib1 : 'A' + nib1 - 0xA;
    *p++         = nib2 < 0xA ? '0' + nib2 : 'A' + nib2 - 0xA;
    *p           = '\0'; // null terminate just in case
    return result;
}

#ifdef EMSESP_STANDALONE
// special function to work outside of ESP's libraries
char * Helpers::ultostr(char * ptr, uint32_t value, const uint8_t base) {
    unsigned long t = 0, res = 0;
    unsigned long tmp   = value;
    int           count = 0;

    if (NULL == ptr) {
        return NULL;
    }

    if (tmp == 0) {
        count++;
    }

    while (tmp > 0) {
        tmp = tmp / base;
        count++;
    }

    ptr += count;

    *ptr = '\0';

    do {
        res = value - base * (t = value / base);
        if (res < 10) {
            *--ptr = '0' + res;
        } else if ((res >= 10) && (res < 16)) {
            *--ptr = 'A' - 10 + res;
        }
    } while ((value = t) != 0);

    return (ptr);
}
#endif

/*
 * itoa for 2 byte signed (short) integers
 * written by Lukás Chmela, Released under GPLv3. http://www.strudel.org.uk/itoa/ version 0.4
 */
char * Helpers::itoa(char * result, int32_t value, const uint8_t base) {
    // check that the base if valid
    if (base < 2 || base > 36) {
        *result = '\0';
        return result;
    }

    char *  ptr = result, *ptr1 = result;
    int16_t tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + (tmp_value - value * base)];
    } while (value);

    // Apply negative sign
    if (tmp_value < 0) {
        *ptr++ = '-';
    }

    *ptr-- = '\0';
    while (ptr1 < ptr) {
        char tmp_char = *ptr;
        *ptr--        = *ptr1;
        *ptr1++       = tmp_char;
    }

    return result;
}

// for decimals 0 to 99, printed as a 2 char string
char * Helpers::smallitoa(char * result, const uint8_t value) {
    result[0] = ((value / 10) == 0) ? '0' : (value / 10) + '0';
    result[1] = (value % 10) + '0';
    result[2] = '\0';
    return result;
}

// for decimals 0 to 999, printed as a string
char * Helpers::smallitoa(char * result, const uint16_t value) {
    result[0] = ((value / 100) == 0) ? '0' : (value / 100) + '0';
    result[1] = (((value % 100) / 10) == 0) ? '0' : ((value % 100) / 10) + '0';
    result[2] = (value % 10) + '0';
    result[3] = '\0';
    return result;
}

// work out how to display booleans
char * Helpers::render_boolean(char * result, bool value) {
    uint8_t bool_format_ = Mqtt::bool_format();
    if (bool_format_ == BOOL_FORMAT_ONOFF) {
        strlcpy(result, value ? "on" : "off", 5);
    } else if (bool_format_ == BOOL_FORMAT_ONOFF_CAP) {
        strlcpy(result, value ? "ON" : "OFF", 5);
    } else if (bool_format_ == BOOL_FORMAT_TRUEFALSE) {
        strlcpy(result, value ? "true" : "false", 7);
    } else {
        strlcpy(result, value ? "1" : "0", 2);
    }

    return result;
}

// render for native char strings
char * Helpers::render_value(char * result, const char * value, uint8_t format __attribute__((unused))) {
    strcpy(result, value);
    return result;
}

// convert unsigned int (single byte) to text value and returns it
// format: 255(0xFF)=boolean, 0=no formatting, otherwise divide by format
char * Helpers::render_value(char * result, uint8_t value, uint8_t format) {
    // special check if its a boolean
    if (format == EMS_VALUE_BOOL) {
        if (value == EMS_VALUE_BOOL_OFF) {
            render_boolean(result, false);
        } else if (value == EMS_VALUE_BOOL_NOTSET) {
            return nullptr;
        } else {
            render_boolean(result, true); // assume on. could have value 0x01 or 0xFF
        }
        return result;
    }

    if (!hasValue(value)) {
        return nullptr;
    }

    if (!format) {
        itoa(result, value, 10); // format = 0
        return result;
    }

    char s2[10];

    // special case for / 2
    if (format == 2) {
        strlcpy(result, itoa(s2, value >> 1, 10), 5);
        strlcat(result, ".", 5);
        strlcat(result, ((value & 0x01) ? "5" : "0"), 5);
        return result;
    }

    strlcpy(result, itoa(s2, value / format, 10), 5);
    strlcat(result, ".", 5);
    strlcat(result, itoa(s2, value % format, 10), 5);

    return result;
}

// float: convert float to char
// format is the precision, 0 to 8
char * Helpers::render_value(char * result, const float value, const uint8_t format) {
    if (format > 8) {
        return nullptr;
    }

    uint32_t p[] = {0, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000};

    char *  ret   = result;
    int32_t whole = (int32_t)value;

    itoa(result, whole, 10);

    while (*result != '\0') {
        result++;
    }

    *result++       = '.';
    int32_t decimal = abs((int32_t)((value - whole) * p[format]));
    itoa(result, decimal, 10);

    return ret;
}

// int16: convert short (two bytes) to text string and returns string
// format: 0=no division, other divide by the value given and render with a decimal point
char * Helpers::render_value(char * result, const int16_t value, const uint8_t format) {
    if (!hasValue(value)) {
        return nullptr;
    }

    // just print it if no conversion required (format = 0)
    if (!format) {
        itoa(result, value, 10);
        return result;
    }

    int16_t new_value = value;
    result[0]         = '\0';

    // check for negative values
    if (new_value < 0) {
        strlcpy(result, "-", 10);
        new_value *= -1; // convert to positive
    } else {
        strlcpy(result, "", 10);
    }

    // do floating point
    char s2[10] = {0};
    if (format == 2) {
        // divide by 2
        strlcat(result, itoa(s2, new_value / 2, 10), 10);
        strlcat(result, ".", 10);
        strlcat(result, ((new_value & 0x01) ? "5" : "0"), 10);
    } else {
        strlcat(result, itoa(s2, new_value / format, 10), 10);
        strlcat(result, ".", 10);
        strlcat(result, itoa(s2, new_value % format, 10), 10);
    }

    return result;
}

// uint16: convert unsigned short (two bytes) to text string and prints it
char * Helpers::render_value(char * result, const uint16_t value, const uint8_t format) {
    if (!hasValue(value)) {
        return nullptr;
    }

    return (render_value(result, (int16_t)value, format)); // use same code, force it to a signed int
}

// int8: convert signed byte to text string and prints it
char * Helpers::render_value(char * result, const int8_t value, const uint8_t format) {
    if (!hasValue(value)) {
        return nullptr;
    }

    return (render_value(result, (int16_t)value, format)); // use same code, force it to a signed int
}

// uint32: render long (4 byte) unsigned values
char * Helpers::render_value(char * result, const uint32_t value, const uint8_t format) {
    if (!hasValue(value)) {
        return nullptr;
    }

    result[0] = '\0';
    char s[20];

#ifndef EMSESP_STANDALONE
    if (!format) {
        strlcpy(result, ltoa(value, s, 10), 20); // format is 0
    } else {
        strlcpy(result, ltoa(value / format, s, 10), 20);
        strlcat(result, ".", 20);
        strlcat(result, ltoa(value % format, s, 10), 20);
    }

#else
    if (!format) {
        strlcpy(result, ultostr(s, value, 10), 20); // format is 0
    } else {
        strncpy(result, ultostr(s, value / format, 10), 20);
        strlcat(result, ".", 20);
        strncat(result, ultostr(s, value % format, 10), 20);
    }
#endif

    return result;
}

// creates string of hex values from an arrray of bytes
std::string Helpers::data_to_hex(const uint8_t * data, const uint8_t length) {
    if (length == 0) {
        return uuid::read_flash_string(F("<empty>"));
    }

    std::string str(160, '\0');
    char        buffer[4];
    char *      p = &str[0];
    for (uint8_t i = 0; i < length; i++) {
        Helpers::hextoa(buffer, data[i]);
        *p++ = buffer[0];
        *p++ = buffer[1];
        *p++ = ' '; // space
    }
    *--p = '\0'; // null terminate just in case, loosing the trailing space

    return str;
}

// takes a hex string and convert it to an unsigned 32bit number (max 8 hex digits)
// works with only positive numbers
uint32_t Helpers::hextoint(const char * hex) {
    uint32_t val = 0;
    // skip leading '0x'
    if (hex[0] == '0' && hex[1] == 'x') {
        hex += 2;
    }
    while (*hex) {
        // get current character then increment
        char byte = *hex++;
        // transform hex character to the 4bit equivalent number, using the ascii table indexes
        if (byte >= '0' && byte <= '9')
            byte = byte - '0';
        else if (byte >= 'a' && byte <= 'f')
            byte = byte - 'a' + 10;
        else if (byte >= 'A' && byte <= 'F')
            byte = byte - 'A' + 10;
        else
            return 0; // error
        // shift 4 to make space for new digit, and add the 4 bits of the new digit
        val = (val << 4) | (byte & 0xF);
    }
    return val;
}

// quick char to long
uint16_t Helpers::atoint(const char * value) {
    unsigned int x = 0;
    while (*value != '\0') {
        x = (x * 10) + (*value - '0');
        ++value;
    }
    return x;
}

// rounds a number to 2 decimal places
// example: round2(3.14159) -> 3.14
double Helpers::round2(double value, const uint8_t divider) {
    return (int)((value / divider) * 100 + 0.5) / 100.0;
}

// abs of a signed 32-bit integer
uint32_t Helpers::abs(const int32_t i) {
    return (i < 0 ? -i : i);
}

// for booleans, use isBool true (EMS_VALUE_BOOL)
bool Helpers::hasValue(const uint8_t & v, const uint8_t isBool) {
    if (isBool == EMS_VALUE_BOOL) {
        return (v != EMS_VALUE_BOOL_NOTSET);
    }
    return (v != EMS_VALUE_UINT_NOTSET);
}

bool Helpers::hasValue(const int8_t & v) {
    return (v != EMS_VALUE_INT_NOTSET);
}

bool Helpers::hasValue(char * v) {
    if ((v == nullptr) || (strlen(v) == 0)) {
        return false;
    }

    return (v[0] != '\0');
}

// for short these are typically 0x8300, 0x7D00 and sometimes 0x8000
bool Helpers::hasValue(const int16_t & v) {
    return (abs(v) < EMS_VALUE_USHORT_NOTSET);
}

bool Helpers::hasValue(const uint16_t & v) {
    return (v < EMS_VALUE_USHORT_NOTSET);
}

bool Helpers::hasValue(const uint32_t & v) {
    return (v != EMS_VALUE_ULONG_NOTSET);
}

// checks if we can convert a char string to an int value
bool Helpers::value2number(const char * v, int & value) {
    if ((v == nullptr) || (strlen(v) == 0)) {
        value = 0;
        return false;
    }
    value = atoi((char *)v);
    return true;
}

// checks if we can convert a char string to a float value
bool Helpers::value2float(const char * v, float & value) {
    if ((v == nullptr) || (strlen(v) == 0)) {
        value = 0;
        return false;
    }
    value = atof((char *)v);
    return true;
}

// https://stackoverflow.com/questions/313970/how-to-convert-stdstring-to-lower-case
std::string Helpers::toLower(std::string const & s) {
    std::string lc = s;
    std::transform(lc.begin(), lc.end(), lc.begin(), [](unsigned char c) { return std::tolower(c); });
    return lc;
}

// checks if we can convert a char string to a lowercase string
bool Helpers::value2string(const char * v, std::string & value) {
    if ((v == nullptr) || (strlen(v) == 0)) {
        value = std::string{};
        return false;
    }
    value = toLower(v);
    return true;
}

// checks to see if a string (usually a command or payload cmd) looks like a boolean
// on, off, true, false, 1, 0
bool Helpers::value2bool(const char * v, bool & value) {
    if ((v == nullptr) || (strlen(v) == 0)) {
        return false;
    }

    std::string bool_str = toLower(v); // convert to lower case

    if ((bool_str == "on") || (bool_str == "1") or (bool_str == "true")) {
        value = true;
        return true; // is a bool
    }

    if ((bool_str == "off") || (bool_str == "0") or (bool_str == "false")) {
        value = false;
        return true; // is a bool
    }

    return false; // not a bool
}

// checks to see if a string is member of a vector and return the index, also allow true/false for on/off
bool Helpers::value2enum(const char * v, uint8_t & value, const flash_string_vector & strs) {
    if ((v == nullptr) || (strlen(v) == 0)) {
        return false;
    }
    std::string str = toLower(v);
    for (value = 0; value < strs.size(); value++) {
        std::string str1 = uuid::read_flash_string(strs[value]);
        if ((str1 == "off" && str == "false") || (str1 == "on" && str == "true") || (str == str1) || (v[0] == ('0' + value) && v[1] == '\0')) {
            return true;
        }
    }
    return false;
}


} // namespace emsesp
