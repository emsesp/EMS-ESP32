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

#include "helpers.h"
#include "emsesp.h"

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

// sane as above but to a hex string
std::string Helpers::hextoa(const uint8_t value, bool prefix) {
    char buf[3];
    if (prefix) {
        return std::string("0x") + hextoa(buf, value);
    }
    return std::string(hextoa(buf, value));
}

#ifdef EMSESP_STANDALONE
// special function to work outside of ESP's libraries
char * Helpers::ultostr(char * ptr, uint32_t value, const uint8_t base) {
    if (NULL == ptr) {
        return NULL;
    }

    unsigned long t     = 0;
    unsigned long tmp   = value;
    int           count = 0;

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
        unsigned long res = value - base * (t = value / base);
        if (res < 10) {
            *--ptr = '0' + res;
        } else if (res < 16) {
            *--ptr = 'A' - 10 + res;
        }
    } while ((value = t) != 0);

    return (ptr);
}
#endif

/**
 * fast atoi returning a std::string
 * http://www.strudel.org.uk/itoa/
 * 
 */
std::string Helpers::itoa(int16_t value) {
    std::string buf;
    buf.reserve(25); // Pre-allocate enough space.
    int quotient = value;

    do {
        buf += "0123456789abcdef"[std::abs(quotient % 10)];
        quotient /= 10;
    } while (quotient);

    // Append the negative sign
    if (value < 0)
        buf += '-';

    std::reverse(buf.begin(), buf.end());
    return buf;
}

/*
 * fast itoa and optimized for ESP32
 * written by Lukás Chmela, Released under GPLv3. http://www.strudel.org.uk/itoa/ version 0.4
 */
char * Helpers::itoa(int32_t value, char * result, const uint8_t base) {
    // check that the base if valid
    if (base < 2 || base > 36) {
        *result = '\0';
        return result;
    }

    char *  ptr = result, *ptr1 = result;
    int32_t tmp_value;

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
// for strings only
char * Helpers::render_boolean(char * result, bool value) {
    uint8_t bool_format_ = EMSESP::system_.bool_format();
    if (bool_format_ == BOOL_FORMAT_ONOFF_STR) {
        strlcpy(result, value ? read_flash_string(F_(on)).c_str() : read_flash_string(F_(off)).c_str(), 5);
    } else if (bool_format_ == BOOL_FORMAT_ONOFF_STR_CAP) {
        strlcpy(result, value ? read_flash_string(F_(ON)).c_str() : read_flash_string(F_(OFF)).c_str(), 5);
    } else if ((bool_format_ == BOOL_FORMAT_10) || (bool_format_ == BOOL_FORMAT_10_STR)) {
        strlcpy(result, value ? "1" : "0", 2);
    } else {
        strlcpy(result, value ? "true" : "false", 7); // default
    }
    return result;
}

// render for native char strings
char * Helpers::render_value(char * result, const char * value, const int8_t format __attribute__((unused))) {
    strcpy(result, value);
    return result;
}

// convert unsigned int (single byte) to text value and returns it
// format: 255(0xFF)=boolean, 0=no formatting, otherwise divide by format
char * Helpers::render_value(char * result, uint8_t value, int8_t format, const uint8_t fahrenheit) {
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

    int16_t new_value = fahrenheit ? format ? value * 1.8 + 32 * format * (fahrenheit - 1) : value * 1.8 + 32 * (fahrenheit - 1) : value;

    if (!format) {
        itoa(new_value, result, 10); // format = 0
        return result;
    }

    char s2[10];

    // special case for / 2
    if (format == 2) {
        strlcpy(result, itoa(new_value >> 1, s2, 10), 5);
        strlcat(result, ".", 5);
        strlcat(result, ((new_value & 0x01) ? "5" : "0"), 7);
        return result;
    } else if (format == 4) {
        strlcpy(result, itoa(new_value >> 2, s2, 10), 5);
        strlcat(result, ".", 5);
        new_value = (new_value & 0x03) * 25;
        strlcat(result, itoa(new_value, s2, 10), 7);
        return result;

    } else if (format > 0) {
        strlcpy(result, itoa(new_value / format, s2, 10), 5);
        strlcat(result, ".", 5);
        strlcat(result, itoa(new_value % format, s2, 10), 7);
    } else {
        strlcpy(result, itoa(new_value * format * -1, s2, 10), 5);
    }

    return result;
}

// float: convert float to char
// format is the precision, 0 to 8
char * Helpers::render_value(char * result, const float value, const int8_t format) {
    if (format > 8) {
        return nullptr;
    }

    uint32_t p[] = {0, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000};

    char *  ret   = result;
    int32_t whole = (int32_t)value;

    itoa(whole, result, 10);

    while (*result != '\0') {
        result++;
    }

    *result++       = '.';
    int32_t decimal = abs((int32_t)((value - whole) * p[format]));
    itoa(decimal, result, 10);

    return ret;
}

// int32: convert signed 32bit to text string and returns string
// format: 0=no division, other divide by the value given and render with a decimal point
char * Helpers::render_value(char * result, const int32_t value, const int8_t format, const uint8_t fahrenheit) {
    int32_t new_value = fahrenheit ? format ? value * 1.8 + 32 * format * (fahrenheit - 1) : value * 1.8 + 32 * (fahrenheit - 1) : value;
    char    s[10]     = {0};
    // just print it if no conversion required (format = 0)
    if (!format) {
        strlcpy(result, itoa(new_value, s, 10), sizeof(s)); // format is 0
        return result;
    }

    result[0] = '\0';

    // check for negative values
    if (new_value < 0) {
        strlcpy(result, "-", sizeof(s));
        new_value *= -1; // convert to positive
    } else {
        strlcpy(result, "", sizeof(s));
    }

    // do floating point
    if (format == 2) {
        // divide by 2
        strlcat(result, itoa(new_value / 2, s, 10), sizeof(s));
        strlcat(result, ".", sizeof(s));
        strlcat(result, ((new_value & 0x01) ? "5" : "0"), sizeof(s));
    } else if (format > 0) {
        strlcat(result, itoa(new_value / format, s, 10), sizeof(s));
        strlcat(result, ".", sizeof(s));
        strlcat(result, itoa(new_value % format, s, 10), sizeof(s));
    } else {
        strlcat(result, itoa(new_value * format * -1, s, 10), sizeof(s));
    }

    return result;
}

// int16: convert short (two bytes) to text string and prints it
char * Helpers::render_value(char * result, const int16_t value, const int8_t format, const uint8_t fahrenheit) {
    if (!hasValue(value)) {
        return nullptr;
    }

    return (render_value(result, (int32_t)value, format, fahrenheit)); // use same code, force it to a signed int
}

// uint16: convert unsigned short (two bytes) to text string and prints it
char * Helpers::render_value(char * result, const uint16_t value, const int8_t format, const uint8_t fahrenheit) {
    if (!hasValue(value)) {
        return nullptr;
    }

    return (render_value(result, (int32_t)value, format, fahrenheit)); // use same code, force it to a signed int
}

// int8: convert signed byte to text string and prints it
char * Helpers::render_value(char * result, const int8_t value, const int8_t format, const uint8_t fahrenheit) {
    if (!hasValue(value)) {
        return nullptr;
    }

    return (render_value(result, (int32_t)value, format, fahrenheit)); // use same code, force it to a signed int
}

// uint32: render long (4 byte) unsigned values
char * Helpers::render_value(char * result, const uint32_t value, const int8_t format, const uint8_t fahrenheit) {
    if (!hasValue(value)) {
        return nullptr;
    }
    result[0]         = '\0';
    int32_t new_value = fahrenheit ? format ? value * 1.8 + 32 * format * (fahrenheit - 1) : value * 1.8 + 32 * (fahrenheit - 1) : value;
    char    s[10]     = {0};

#ifndef EMSESP_STANDALONE
    if (!format) {
        strlcpy(result, ltoa(new_value, s, 10), sizeof(s)); // format is 0
    } else if (format > 0) {
        strlcpy(result, ltoa(new_value / format, s, 10), sizeof(s));
        strlcat(result, ".", sizeof(s));
        strlcat(result, ltoa(new_value % format, s, 10), sizeof(s));
    } else {
        strlcpy(result, ltoa(new_value * format * -1, s, 10), sizeof(s));
    }

#else
    if (!format) {
        strlcpy(result, ultostr(s, new_value, 10), sizeof(s)); // format is 0
    } else {
        strncpy(result, ultostr(s, new_value / format, 10), sizeof(s));
        strlcat(result, ".", sizeof(s));
        strncat(result, ultostr(s, new_value % format, 10), sizeof(s));
    }
#endif

    return result;
}

// creates string of hex values from an arrray of bytes
std::string Helpers::data_to_hex(const uint8_t * data, const uint8_t length) {
    if (length == 0) {
        return "<empty>";
    }

    char   str[160] = {0};
    char   buffer[4];
    char * p = &str[0];
    for (uint8_t i = 0; i < length; i++) {
        Helpers::hextoa(buffer, data[i]);
        *p++ = buffer[0];
        *p++ = buffer[1];
        *p++ = ' '; // space
    }
    *--p = '\0'; // null terminate just in case, loosing the trailing space

    return std::string(str);
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
int Helpers::atoint(const char * value) {
    int  x = 0;
    char s = value[0];
    if (s == '-') {
        ++value;
    }
    while (*value >= '0' && *value <= '9') {
        x = (x * 10) + (*value - '0');
        ++value;
    }
    if (s == '-') {
        return (-x);
    }
    return x;
}

// rounds a number to 2 decimal places
// example: round2(3.14159) -> 3.14
// From mvdp:
//  The conversion to Fahrenheit is different for absolute temperatures and relative temperatures like hysteresis.
//  fahrenheit=0 - off, no conversion
//  fahrenheit=1 - relative, 1.8t
//  fahrenheit=2 - absolute, 1.8t + 32(fahrenheit-1).
float Helpers::round2(float value, const int8_t divider, const uint8_t fahrenheit) {
    float val = (value * 100 + 0.5);
    if (divider > 0) {
        val = ((value / divider) * 100 + 0.5);
    } else if (divider < 0) {
        val = value * -100 * divider;
    }
    if (value < 0) { // negative rounding
        val = val - 1;
    }
    if (fahrenheit) {
        val = val * 1.8 + 3200 * (fahrenheit - 1);
    }

    return ((int32_t)val) / 100.0;
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
bool Helpers::value2number(const char * v, int & value, const int min, const int max) {
    if ((v == nullptr) || (strlen(v) == 0)) {
        value = 0;
        return false;
    }
    value = atoi((char *)v);
    if (value >= min && value <= max) {
        return true;
    }
    return false;
}

// checks if we can convert a char string to a float value
bool Helpers::value2float(const char * v, float & value) {
    value = 0;
    if ((v == nullptr) || (strlen(v) == 0)) {
        return false;
    }
    if (v[0] == '-' || v[0] == '.' || (v[0] >= '0' && v[0] <= '9')) {
        value = atof((char *)v);
        return true;
    }
    return false;
}

bool Helpers::value2temperature(const char * v, float & value, bool relative) {
    if (value2float(v, value)) {
        if (EMSESP::system_.fahrenheit()) {
            value = relative ? (value / 1.8) : (value - 32) / 1.8;
        }
        return true;
    }
    return false;
}

bool Helpers::value2temperature(const char * v, int & value, const bool relative, const int min, const int max) {
    if (value2number(v, value, min, max)) {
        if (EMSESP::system_.fahrenheit()) {
            value = relative ? (value / 1.8) : (value - 32) / 1.8;
        }
        return true;
    }
    return false;
}

// https://stackoverflow.com/questions/313970/how-to-convert-stdstring-to-lower-case
std::string Helpers::toLower(std::string const & s) {
    std::string lc = s;
    std::transform(lc.begin(), lc.end(), lc.begin(), [](unsigned char c) { return std::tolower(c); });
    return lc;
}

std::string Helpers::toUpper(std::string const & s) {
    std::string lc = s;
    std::transform(lc.begin(), lc.end(), lc.begin(), [](unsigned char c) { return std::toupper(c); });
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

    if ((bool_str == read_flash_string(F_(on))) || (bool_str == "1") or (bool_str == "true")) {
        value = true;
        return true; // is a bool
    }

    if ((bool_str == read_flash_string(F_(off))) || (bool_str == "0") or (bool_str == "false")) {
        value = false;
        return true; // is a bool
    }

    return false; // not a bool
}

// checks to see if a string is member of a vector and return the index, also allow true/false for on/off
bool Helpers::value2enum(const char * v, uint8_t & value, const __FlashStringHelper * const * strs) {
    if ((v == nullptr) || (strlen(v) == 0)) {
        return false;
    }
    std::string str = toLower(v);
    for (value = 0; strs[value]; value++) {
        std::string str1 = toLower(read_flash_string(strs[value]));
        if ((str1 != "")
            && ((str1 == read_flash_string(F_(off)) && str == "false") || (str1 == read_flash_string(F_(on)) && str == "true") || (str == str1)
                || (v[0] == ('0' + value) && v[1] == '\0'))) {
            return true;
        }
    }
    return false;
}

// replace char in char string
void Helpers::replace_char(char * str, char find, char replace) {
    int i = 0;

    while (str[i] != '\0') {
        /*Replace the matched character...*/
        if (str[i] == find)
            str[i] = replace;

        i++;
    }
}

} // namespace emsesp
