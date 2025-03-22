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

// same as hextoa but uses to a hex std::string
std::string Helpers::hextoa(const uint8_t value, bool prefix) {
    char buf[3];
    if (prefix) {
        return std::string("0x") + hextoa(buf, value);
    }
    return std::string(hextoa(buf, value));
}

// same for 16 bit values
char * Helpers::hextoa(char * result, const uint16_t value) {
    if (value <= 0xFF) {
        return hextoa(result, (uint8_t)value);
    }
    hextoa(result, (uint8_t)(value >> 8));
    hextoa(&result[2], (uint8_t)(value & 0xFF));
    return result;
}

// same as above but to a hex string
std::string Helpers::hextoa(const uint16_t value, bool prefix) {
    char buf[5];
    if (prefix) {
        return std::string("0x") + hextoa(buf, value);
    }
    return std::string(hextoa(buf, value));
}

#ifdef EMSESP_STANDALONE
// special function to work outside of ESP's libraries
char * Helpers::ultostr(char * ptr, uint32_t value, const uint8_t base) {
    if (nullptr == ptr) {
        return nullptr;
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

// fast itoa returning a std::string
// http://www.strudel.org.uk/itoa/
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
 * fast itoa
 * written by Lukás Chmela, Released under GPLv3. http://www.strudel.org.uk/itoa/ version 0.4
 * optimized for ESP32
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
char * Helpers::render_boolean(char * result, const bool value, const bool dashboard) {
    uint8_t bool_format_ = dashboard ? EMSESP::system_.bool_dashboard() : EMSESP::system_.bool_format();

    if (bool_format_ == BOOL_FORMAT_ONOFF_STR) {
        strlcpy(result, value ? translated_word(FL_(on)) : translated_word(FL_(off)), 12);
    } else if (bool_format_ == BOOL_FORMAT_ONOFF_STR_CAP) {
        strlcpy(result, value ? translated_word(FL_(ON)) : translated_word(FL_(OFF)), 12);
    } else if ((bool_format_ == BOOL_FORMAT_10) || (bool_format_ == BOOL_FORMAT_10_STR)) {
        strlcpy(result, value ? "1" : "0", 2);
    } else {
        strlcpy(result, value ? "true" : "false", 7); // default
    }
    return result;
}

// convert unsigned int (single byte) to text value and returns it
// format: 255(0xFF)=boolean, 0=no formatting, otherwise divide by format
char * Helpers::render_value(char * result, uint8_t value, int8_t format, const uint8_t fahrenheit) {
    // special check if its a boolean
    if ((uint8_t)format == EMS_VALUE_BOOL) {
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
char * Helpers::render_value(char * result, const double value, const int8_t format) {
    if (format > 8) {
        return nullptr;
    }

    uint32_t p[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000};

    char * ret   = result;
    double v     = value < 0 ? value - 1.0 / (2 * p[format]) : value + 1.0 / (2 * p[format]);
    auto   whole = (long long)v;

    if (whole <= 0 && v < 0) {
        result[0] = '-';
        result++;
        whole = -whole;
        v     = -v;
    }
#ifndef EMSESP_STANDALONE
    lltoa(whole, result, 10);
#else
    ultostr(result, whole, 10);
#endif

    while (*result != '\0') {
        result++;
    }

    *result++    = '.';
    auto decimal = abs((int32_t)((v - whole) * p[format]));
    for (int8_t i = 1; i < format; i++) {
        if (decimal < p[i]) {
            *result++ = '0'; // add leading zeros
        }
    }
    itoa(decimal, result, 10);

    return ret;
}

// int32: convert signed 32bit to text string and returns string
// format: 0=no division, other divide by the value given and render with a decimal point
char * Helpers::render_value(char * result, const int32_t value, const int8_t format, const uint8_t fahrenheit) {
    int32_t new_value = fahrenheit ? format ? value * 1.8 + 32 * format * (fahrenheit - 1) : value * 1.8 + 32 * (fahrenheit - 1) : value;
    char    s[13]     = {0};
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
        strlcat(result, itoa(((new_value % format) * 10) / format, s, 10), sizeof(s));
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
    result[0]          = '\0';
    uint32_t new_value = fahrenheit ? format ? value * 1.8 + 32 * format * (fahrenheit - 1) : value * 1.8 + 32 * (fahrenheit - 1) : value;
    char     s[14]     = {0};

#ifndef EMSESP_STANDALONE
    if (!format) {
        strlcpy(result, lltoa(new_value, s, 10), sizeof(s)); // format is 0
    } else if (format > 0) {
        strlcpy(result, lltoa(new_value / format, s, 10), sizeof(s));
        strlcat(result, ".", sizeof(s));
        strlcat(result, itoa(((new_value % format) * 10) / format, s, 10), sizeof(s));
        if (format == 100) {
            strlcat(result, itoa(new_value % 10, s, 10), sizeof(s));
        }
    } else {
        strlcpy(result, lltoa(new_value * format * -1, s, 10), sizeof(s));
    }
#else
    if (!format) {
        strlcpy(result, ultostr(s, new_value, 10), sizeof(s)); // format is 0
    } else {
        strlcpy(result, ultostr(s, new_value / format, 10), sizeof(s));
        strlcat(result, ".", sizeof(s));
        strncat(result, ultostr(s, new_value % format, 10), sizeof(s));
    }
#endif

    return result;
}

// creates string of hex values from an array of bytes
std::string Helpers::data_to_hex(const uint8_t * data, const uint8_t length) {
    if (length == 0) {
        return "<empty>";
    }

    char str[length * 3];
    memset(str, 0, sizeof(str));

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
    if (hex == nullptr) {
        return 0;
    }

    uint32_t val = 0;

    // skip leading '0x'
    if (hex[0] == '0' && hex[1] == 'x') {
        hex += 2;
    }

    while (*hex) {
        // get current character then increment
        char byte = *hex++;
        // transform hex character to the 4bit equivalent number, using the ascii table indexes
        if (byte == ' ')
            byte = *hex++; // skip spaces
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
//  The conversion to Fahrenheit is different for absolute temperatures and relative temperatures like hysteresis.
//  fahrenheit=0 - off, no conversion
//  fahrenheit=1 - relative, 1.8t
//  fahrenheit=2 - absolute, 1.8t + 32(fahrenheit-1)
double Helpers::transformNumFloat(double value, const int8_t numeric_operator, const uint8_t fahrenheit) {
    double val;

    if (numeric_operator == 0) { // DV_NUMOP_NONE
        val = value * 100;
    } else if (numeric_operator > 0) { // DV_NUMOP_DIVxx
        val = value * 100 / numeric_operator;
    } else { // DV_NUMOP_MULxx
        val = value * -100 * numeric_operator;
    }

    if (fahrenheit) {
        val = val * 1.8 + 3200 * (fahrenheit - 1);
    }

    return (round(val)) / 100.0;
}

// abs of a signed 32-bit integer
uint32_t Helpers::abs(const int32_t i) {
    return (i < 0 ? -i : i);
}

// for booleans, use isBool true (EMS_VALUE_BOOL)
bool Helpers::hasValue(const uint8_t & value, const uint8_t isBool) {
    if (isBool == EMS_VALUE_BOOL) {
        return (value != EMS_VALUE_BOOL_NOTSET);
    }
    return (value != EMS_VALUE_UINT8_NOTSET);
}

bool Helpers::hasValue(const int8_t & value) {
    return (value != EMS_VALUE_INT8_NOTSET);
}

bool Helpers::hasValue(const char * value) {
    if ((value == nullptr) || (strlen(value) == 0)) {
        return false;
    }

    return (value[0] != '\0');
}

// for short these are typically 0x8300, 0x7D00 and sometimes 0x8000
bool Helpers::hasValue(const int16_t & value) {
    return (abs(value) < EMS_VALUE_UINT16_NOTSET);
}

bool Helpers::hasValue(const uint16_t & value) {
    return (value < EMS_VALUE_UINT16_NOTSET);
}

bool Helpers::hasValue(const uint32_t & value) {
    return (value != EMS_VALUE_UINT24_NOTSET && value != EMS_VALUE_UINT32_NOTSET);
}

// checks if we can convert a char string to an int value
bool Helpers::value2number(const char * value, int & value_i, const int min, const int max) {
    if ((value == nullptr) || (strlen(value) == 0)) {
        value_i = 0;
        return false;
    }

    value_i = atoi(value);
    if (value_i >= min && value_i <= max) {
        return true;
    }
    return false;
}

// checks if we can convert a char string to a float value
bool Helpers::value2float(const char * value, float & value_f) {
    value_f = 0;
    if ((value == nullptr) || (strlen(value) == 0)) {
        return false;
    }

    if (value[0] == '-' || value[0] == '.' || (value[0] >= '0' && value[0] <= '9')) {
        value_f = atof(value);
        return true;
    }

    if (value[0] == '+' && (value[1] == '.' || (value[1] >= '0' && value[1] <= '9'))) {
        value_f = atof(value + 1);
        return true;
    }
    return false;
}

bool Helpers::value2temperature(const char * value, float & value_f, bool relative) {
    if (value2float(value, value_f)) {
        if (EMSESP::system_.fahrenheit()) {
            value_f = relative ? (value_f / 1.8) : (value_f - 32) / 1.8;
        }
        return true;
    }
    return false;
}

bool Helpers::value2temperature(const char * value, int & value_i, const bool relative, const int min, const int max) {
    if (value2number(value, value_i, min, max)) {
        if (EMSESP::system_.fahrenheit()) {
            value_i = relative ? (value_i / 1.8) : (value_i - 32) / 1.8;
        }
        return true;
    }
    return false;
}

// checks if we can convert a char string to a lowercase string
bool Helpers::value2string(const char * value, std::string & value_s) {
    if ((value == nullptr) || (strlen(value) == 0)) {
        value_s = std::string{};
        return false;
    }

    value_s = toLower(value);
    return true;
}

// checks to see if a string (usually a command or payload cmd) looks like a boolean
// on, off, true, false, 1, 0
bool Helpers::value2bool(const char * value, bool & value_b) {
    if ((value == nullptr) || (strlen(value) == 0)) {
        return false;
    }

    std::string bool_str = toLower(value);

    if ((bool_str == std::string(Helpers::translated_word(FL_(on)))) || (bool_str == toLower(Helpers::translated_word(FL_(ON)))) || (bool_str == "on")
        || (bool_str == "1") || (bool_str == "true")) {
        value_b = true;
        return true; // is a bool
    }

    if ((bool_str == std::string(Helpers::translated_word(FL_(off)))) || (bool_str == toLower(Helpers::translated_word(FL_(OFF)))) || (bool_str == "off")
        || (bool_str == "0") || (bool_str == "false")) {
        value_b = false;
        return true; // is a bool
    }

#ifdef EMSESP_STANDALONE
    emsesp::EMSESP::logger().debug("Error. value2bool: %s is not a boolean", value);
#endif

    return false; // not a bool
}

// checks to see if a string is member of a vector and return the index, also allow true/false for on/off
// this for a list of lists, when using translated strings
bool Helpers::value2enum(const char * value, uint8_t & value_ui, const char * const ** strs) {
    if ((value == nullptr) || (strlen(value) == 0)) {
        return false;
    }
    std::string str = toLower(value);

    for (value_ui = 0; strs[value_ui]; value_ui++) {
        std::string str1 = toLower(std::string(Helpers::translated_word(strs[value_ui])));
        std::string str2 = toLower((strs[value_ui][0])); // also check for default language
        if ((str1 != "")
            && ((str2 == "off" && str == "false") || (str2 == "on" && str == "true") || (str == str1) || (str == str2)
                || (value[0] == ('0' + value_ui) && value[1] == '\0'))) {
            return true;
        }
    }
    value_ui = 0;

    return false;
}

// finds the string (value) of a list vector (strs)
// returns true if found, and sets the value_ui to the index, else false
// also allow true/false for on/off
bool Helpers::value2enum(const char * value, uint8_t & value_ui, const char * const * strs) {
    if ((value == nullptr) || (strlen(value) == 0)) {
        return false;
    }
    std::string str = toLower(value);

    std::string s_on  = Helpers::translated_word(FL_(on));
    std::string s_off = Helpers::translated_word(FL_(off));

    // stops when a nullptr is found, which is the end delimeter of a MAKE_TRANSLATION()
    // could use count_items() to avoid buffer over-run but this works
    for (value_ui = 0; strs[value_ui]; value_ui++) {
        std::string enum_str = toLower((strs[value_ui]));

        if ((enum_str != "")
            && ((enum_str == "off" && (str == s_off || str == "false")) || (enum_str == "on" && (str == s_on || str == "true")) || (str == enum_str)
                || (value[0] == ('0' + value_ui) && value[1] == '\0'))) {
            return true;
        }
    }

    return false;
}

// https://stackoverflow.com/questions/313970/how-to-convert-stdstring-to-lower-case
std::string Helpers::toLower(std::string const & s) {
    std::string lc = s;
    std::transform(lc.begin(), lc.end(), lc.begin(), [](unsigned char c) { return std::tolower(c); });
    return lc;
}

std::string Helpers::toLower(const char * s) {
    return toLower(std::string(s));
}

std::string Helpers::toUpper(std::string const & s) {
    std::string lc = s;
    std::transform(lc.begin(), lc.end(), lc.begin(), [](unsigned char c) { return std::toupper(c); });
    return lc;
}

// capitalizes one UTF-8 character in char array
// works with Latin1 (1 byte), Polish amd some other (2 bytes) characters
// TODO add special characters that occur in other supported languages
#if defined(EMSESP_STANDALONE)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif
void Helpers::CharToUpperUTF8(char * c) {
    auto p   = (c + 1); // pointer to 2nd char of 2-byte unicode char
    char p_v = *p;      // value of 2nd char in 2-byte unicode char

    switch (*c) {
    case (char)0xC3:
        // grave, acute, circumflex, diaeresis, etc.
        if ((p_v >= (char)0xA0) && (p_v <= (char)0xBE)) {
            *p -= 0x20;
        }
        break;
    case (char)0xC4:
        switch (p_v) {
        case (char)0x85: //ą (0xC4,0x85) -> Ą (0xC4,0x84)
        case (char)0x87: //ć (0xC4,0x87) -> Ć (0xC4,0x86)
        case (char)0x99: //ę (0xC4,0x99) -> Ę (0xC4,0x98)
            *p -= 1;
            break;
        }
        break;
    case (char)0xC5:
        switch (p_v) {
        case (char)0x82: //ł (0xC5,0x82) -> Ł (0xC5,0x81)
        case (char)0x84: //ń (0xC5,0x84) -> Ń (0xC5,0x83)
        case (char)0x9B: //ś (0xC5,0x9B) -> Ś (0xC5,0x9A)
        case (char)0xBA: //ź (0xC5,0xBA) -> Ź (0xC5,0xB9)
        case (char)0xBC: //ż (0xC5,0xBC) -> Ż (0xC5,0xBB)
            *p -= 1;
            break;
        }
        break;
    default:
        *c = toupper(*c); // works on Latin1 letters
        break;
    }
}
#if defined(EMSESP_STANDALONE)
#pragma GCC diagnostic pop
#endif

// replace char in char string
void Helpers::replace_char(char * str, char find, char replace) {
    if (str == nullptr) {
        return;
    }

    int i = 0;

    while (str[i] != '\0') {
        // Replace the matched character...
        if (str[i] == find)
            str[i] = replace;

        i++;
    }
}

// count number of items in a list
// the end of a list has a nullptr
uint8_t Helpers::count_items(const char * const * list) {
    uint8_t list_size = 0;
    if (list != nullptr) {
        while (list[list_size]) {
            list_size++;
        }
    }
    return list_size;
}

// count number of items in a list of lists
// the end of a list has a nullptr
uint8_t Helpers::count_items(const char * const ** list) {
    uint8_t list_size = 0;
    if (list != nullptr) {
        while (list[list_size]) {
            list_size++;
        }
    }
    return list_size;
}

// returns char pointer to translated description or fullname
// if force_en is true always take the EN non-translated word
const char * Helpers::translated_word(const char * const * strings, const bool force_en) {
    uint8_t language_index = EMSESP::system_.language_index();
    uint8_t index          = 0; // default en

    if (!strings) {
        return ""; // no translations
    }

    // see how many translations we have for this entity. if there is no translation for this, revert to EN
    if (!force_en && (Helpers::count_items(strings) >= language_index + 1 && strlen(strings[language_index]))) {
        index = language_index;
    }

    return strings[index];
}

uint16_t Helpers::string2minutes(const std::string & str) {
    uint8_t  i     = 0;
    uint16_t res   = 0;
    uint16_t tmp   = 0;
    uint8_t  state = 0;

    while (str[i] != '\0') {
        // If we got a digit
        if (str[i] >= '0' && str[i] <= '9') {
            tmp = tmp * 10 + str[i] - '0';
        }
        // Or if we got a colon
        else if (str[i] == ':') {
            // If we were reading the hours
            if (state == 0) {
                res = 60 * tmp;
            }
            // Or if we were reading the minutes
            else if (state == 1) {
                if (tmp > 60) {
                    return 0;
                }
                res += tmp;
            }
            // Or we got an extra colon
            else {
                return 0;
            }

            state++;
            tmp = 0;
        }

        // Or we got something wrong
        else {
            return 0;
        }
        i++;
    }

    if (state == 1 && tmp < 60) {
        return res + tmp;
    } else if (state == 0) { // without : it's only minutes
        return tmp;
    } else {
        return 0; // Or if we were not, something is wrong in the given string
    }
}

float Helpers::numericoperator2scalefactor(int8_t numeric_operator) {
    if (numeric_operator == 0)
        return 1.0f;
    else if (numeric_operator > 0)
        return 1.0f / numeric_operator;
    else
        return -numeric_operator;
}

} // namespace emsesp
