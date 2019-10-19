/*
 * Generic utils
 * 
 * Paul Derbyshire - https://github.com/proddy/EMS-ESP
 *
 */


#pragma once

#include "MyESP.h"
#include "ems.h"

#define myDebug(...) myESP.myDebug(__VA_ARGS__)
#define myDebug_P(...) myESP.myDebug_P(__VA_ARGS__)

char *   _float_to_char(char * a, float f, uint8_t precision = 2);
char *   _bool_to_char(char * s, uint8_t value);
char *   _short_to_char(char * s, int16_t value, uint8_t decimals = 1);
char *   _ushort_to_char(char * s, uint16_t value, uint8_t decimals = 1);
void     _renderShortValue(const char * prefix, const char * postfix, int16_t value, uint8_t decimals = 1);
void     _renderUShortValue(const char * prefix, const char * postfix, uint16_t value, uint8_t decimals = 1);
char *   _int_to_char(char * s, uint8_t value, uint8_t div = 1);
void     _renderIntValue(const char * prefix, const char * postfix, uint8_t value, uint8_t div = 1);
void     _renderLongValue(const char * prefix, const char * postfix, uint32_t value);
void     _renderBoolValue(const char * prefix, uint8_t value);
char *   _hextoa(uint8_t value, char * buffer);
char *   _smallitoa(uint8_t value, char * buffer);
char *   _smallitoa3(uint16_t value, char * buffer);
uint8_t  _readIntNumber();
float    _readFloatNumber();
uint16_t _readHexNumber();
char *   _readWord();
