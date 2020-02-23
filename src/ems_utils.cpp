/*
 * Generic utils
 * 
 * Paul Derbyshire - https://github.com/proddy/EMS-ESP
 *
 */

#include "ems_utils.h"

// convert float to char
char * _float_to_char(char * a, float f, uint8_t precision) {
    long p[] = {0, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000};

    char * ret   = a;
    long   whole = (long)f;
    itoa(whole, a, 10);
    while (*a != '\0')
        a++;
    *a++         = '.';
    long decimal = abs((long)((f - whole) * p[precision]));
    itoa(decimal, a, 10);

    return ret;
}

// convert bool to text. bools are stored as bytes
char * _bool_to_char(char * s, uint8_t value) {
    if ((value == EMS_VALUE_BOOL_ON) || (value == EMS_VALUE_BOOL_ON2)) {
        strlcpy(s, "on", sizeof(s));
    } else if (value == EMS_VALUE_BOOL_OFF) {
        strlcpy(s, "off", sizeof(s));
    } else { // EMS_VALUE_BOOL_NOTSET
        strlcpy(s, "?", sizeof(s));
    }
    return s;
}

// convert short (two bytes) to text string and returns string
// decimals: 0 = no division, 1=divide value by 10, 2=divide by 2, 10=divide value by 100
// negative values are assumed stored as 1-compliment (https://medium.com/@LeeJulija/how-integers-are-stored-in-memory-using-twos-complement-5ba04d61a56c)
char * _short_to_char(char * s, int16_t value, uint8_t decimals) {
    // remove errors or invalid values
    if (value <= EMS_VALUE_SHORT_NOTSET) {
        strlcpy(s, "?", 10);
        return (s);
    }

    // just print
    if (decimals == 0) {
        ltoa(value, s, 10);
        return (s);
    }

    // check for negative values
    if (value < 0) {
        strlcpy(s, "-", 10);
        value *= -1; // convert to positive
    } else {
        strlcpy(s, "", 10);
    }

    // do floating point
    char s2[10] = {0};
    if (decimals == 2) {
        // divide by 2
        strlcat(s, ltoa(value / 2, s2, 10), 10);
        strlcat(s, ".", 10);
        strlcat(s, ((value & 0x01) ? "5" : "0"), 10);

    } else {
        strlcat(s, ltoa(value / (decimals * 10), s2, 10), 10);
        strlcat(s, ".", 10);
        strlcat(s, ltoa(value % (decimals * 10), s2, 10), 10);
    }

    return s;
}

// convert unsigned short (two bytes) to text string and prints it
// decimals: 0 = no division, 1=divide value by 10, 2=divide by 2, 10=divide value by 100
char * _ushort_to_char(char * s, uint16_t value, uint8_t decimals) {
    // remove errors or invalid values
    if (value >= EMS_VALUE_USHORT_NOTSET) { // 0x7D00
        strlcpy(s, "?", 10);
        return (s);
    }

    // just print
    if (decimals == 0) {
        ltoa(value, s, 10);
        return (s);
    }

    // do floating point
    char s2[10] = {0};

    if (decimals == 2) {
        // divide by 2
        strlcpy(s, ltoa(value / 2, s2, 10), 10);
        strlcat(s, ".", 10);
        strlcat(s, ((value & 0x01) ? "5" : "0"), 10);

    } else {
        strlcpy(s, ltoa(value / (decimals * 10), s2, 10), 10);
        strlcat(s, ".", 10);
        strlcat(s, ltoa(value % (decimals * 10), s2, 10), 10);
    }

    return s;
}

// takes a signed short value (2 bytes), converts to a fraction and prints it
// decimals: 0=no division, 1=divide value by 10 (default), 2=divide by 2, 10=divide value by 100
void _renderShortValue(const char * prefix, const char * postfix, int16_t value, uint8_t decimals) {
    static char buffer[200] = {0};
    static char s[20]       = {0};
    strlcpy(buffer, "  ", sizeof(buffer));
    strlcat(buffer, prefix, sizeof(buffer));
    strlcat(buffer, ": ", sizeof(buffer));

    strlcat(buffer, _short_to_char(s, value, decimals), sizeof(buffer));

    if (postfix != nullptr) {
        strlcat(buffer, " ", sizeof(buffer));
        strlcat(buffer, postfix, sizeof(buffer));
    }

    myDebug(buffer);
}

// takes a unsigned short value (2 bytes), converts to a fraction and prints it
// decimals: 0 = no division, 1=divide value by 10, 2=divide by 2, 10=divide value by 100
void _renderUShortValue(const char * prefix, const char * postfix, uint16_t value, uint8_t decimals) {
    static char buffer[200] = {0};
    static char s[20]       = {0};
    strlcpy(buffer, "  ", sizeof(buffer));
    strlcat(buffer, prefix, sizeof(buffer));
    strlcat(buffer, ": ", sizeof(buffer));

    strlcat(buffer, _ushort_to_char(s, value, decimals), sizeof(buffer));

    if (postfix != nullptr) {
        strlcat(buffer, " ", sizeof(buffer));
        strlcat(buffer, postfix, sizeof(buffer));
    }

    myDebug(buffer);
}

// convert int (single byte) to text value and returns it
char * _int_to_char(char * s, uint8_t value, uint8_t div) {
    s[0] = '\0'; // reset
    if (value == EMS_VALUE_INT_NOTSET) {
        strlcpy(s, "?", sizeof(s));
        return (s);
    }

    static char s2[5] = {0};

    switch (div) {
    case 1:
        itoa(value, s, 10);
        break;

    case 2:
        strlcpy(s, itoa(value >> 1, s2, 10), 5);
        strlcat(s, ".", sizeof(s));
        strlcat(s, ((value & 0x01) ? "5" : "0"), 5);
        break;

    case 10:
        strlcpy(s, itoa(value / 10, s2, 10), 5);
        strlcat(s, ".", sizeof(s));
        strlcat(s, itoa(value % 10, s2, 10), 5);
        break;

    default:
        itoa(value, s, 10);
        break;
    }

    return s;
}

// takes an int value (1 byte), converts to a fraction and prints
void _renderIntValue(const char * prefix, const char * postfix, uint8_t value, uint8_t div) {
    static char buffer[200] = {0};
    static char s[20]       = {0};
    strlcpy(buffer, "  ", sizeof(buffer));
    strlcat(buffer, prefix, sizeof(buffer));
    strlcat(buffer, ": ", sizeof(buffer));

    strlcat(buffer, _int_to_char(s, value, div), sizeof(buffer));

    if (postfix != nullptr) {
        strlcat(buffer, " ", sizeof(buffer));
        strlcat(buffer, postfix, sizeof(buffer));
    }

    myDebug(buffer);
}

// takes a long value at prints it to debug log and prints
void _renderLongValue(const char * prefix, const char * postfix, uint32_t value) {
    static char buffer[200] = {0};
    strlcpy(buffer, "  ", sizeof(buffer));
    strlcat(buffer, prefix, sizeof(buffer));
    strlcat(buffer, ": ", sizeof(buffer));

    if (value == EMS_VALUE_LONG_NOTSET) {
        strlcat(buffer, "?", sizeof(buffer));
    } else {
        char s[20] = {0};
        strlcat(buffer, ltoa(value, s, 10), sizeof(buffer));
    }

    if (postfix != nullptr) {
        strlcat(buffer, " ", sizeof(buffer));
        strlcat(buffer, postfix, sizeof(buffer));
    }

    myDebug(buffer);
}

// takes a bool value at prints it to debug log and prints
void _renderBoolValue(const char * prefix, uint8_t value) {
    static char buffer[200] = {0};
    static char s[20]       = {0};
    strlcpy(buffer, "  ", sizeof(buffer));
    strlcat(buffer, prefix, sizeof(buffer));
    strlcat(buffer, ": ", sizeof(buffer));

    strlcat(buffer, _bool_to_char(s, value), sizeof(buffer));

    myDebug(buffer);
}

// like itoa but for hex, and quicker
char * _hextoa(uint8_t value, char * buffer) {
    char * p    = buffer;
    byte   nib1 = (value >> 4) & 0x0F;
    byte   nib2 = (value >> 0) & 0x0F;
    *p++        = nib1 < 0xA ? '0' + nib1 : 'A' + nib1 - 0xA;
    *p++        = nib2 < 0xA ? '0' + nib2 : 'A' + nib2 - 0xA;
    *p          = '\0'; // null terminate just in case
    return buffer;
}

// for decimals 0 to 99, printed as a 2 char string
char * _smallitoa(uint8_t value, char * buffer) {
    buffer[0] = ((value / 10) == 0) ? '0' : (value / 10) + '0';
    buffer[1] = (value % 10) + '0';
    buffer[2] = '\0';
    return buffer;
}

/* for decimals 0 to 999, printed as a string
 */
char * _smallitoa3(uint16_t value, char * buffer) {
    buffer[0] = ((value / 100) == 0) ? '0' : (value / 100) + '0';
    buffer[1] = (((value % 100) / 10) == 0) ? '0' : ((value % 100) / 10) + '0';
    buffer[2] = (value % 10) + '0';
    buffer[3] = '\0';
    return buffer;
}

// used to read the next string from an input buffer and convert to an 8 bit int
uint8_t _readIntNumber() {
    char * numTextPtr = strtok(nullptr, ", \n");
    if (numTextPtr == nullptr) {
        return 0;
    }
    return atoi(numTextPtr);
}

// used to read the next string from an input buffer and convert to a float
float _readFloatNumber() {
    char * numTextPtr = strtok(nullptr, ", \n");
    if (numTextPtr == nullptr) {
        return 0;
    }
    return atof(numTextPtr);
}

// used to read the next string from an input buffer as a hex value and convert to a 16 bit int
uint16_t _readHexNumber() {
    char * numTextPtr = strtok(nullptr, ", \n");
    if (numTextPtr == nullptr) {
        return 0;
    }
    return (uint16_t)strtol(numTextPtr, 0, 16);
}

// used to read the next string from an input buffer
char * _readWord() {
    char * word = strtok(nullptr, ", \n");
    return word;
}
