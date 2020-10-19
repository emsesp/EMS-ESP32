/*
 * Copyright 2019  Simon Arlott
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

#ifndef ARDUINO_H_
#define ARDUINO_H_

#include <unistd.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <algorithm> // for count_if

#include <iostream>

#define IPAddress std::string
#define ICACHE_FLASH_ATTR
#define ICACHE_RAM_ATTR
#define os_event_t void
#define byte uint8_t
#define ltoa itoa
#define boolean bool

#define LOW 0
#define HIGH 1

#ifndef NAN
#define NAN 0
#endif

#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2

void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t value);
int  digitalRead(uint8_t pin);

#define PROGMEM
#define PGM_P const char *
#define PSTR(s)                                                                                                                                                \
    (__extension__({                                                                                                                                           \
        static const char __c[] = (s);                                                                                                                         \
        &__c[0];                                                                                                                                               \
    }))

class __FlashStringHelper;
#define FPSTR(string_literal) (reinterpret_cast<const __FlashStringHelper *>(string_literal))
#define F(string_literal) (FPSTR(PSTR(string_literal)))

int snprintf_P(char * str, size_t size, const char * format, ...);
int vsnprintf_P(char * str, size_t size, const char * format, va_list ap);

// #define pgm_read_byte(addr) (*reinterpret_cast<const char *>(addr))
// #define pgm_read_ptr(p) (reinterpret_cast<const void *>(p))
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#define pgm_read_float(addr) (*(const float *)(addr))
#define pgm_read_ptr(addr) (*(const void **)(addr))

class Print;

class Printable {
  public:
    virtual size_t printTo(Print & print) const = 0;
};

class Print {
  public:
    virtual size_t write(uint8_t c)                           = 0;
    virtual size_t write(const uint8_t * buffer, size_t size) = 0;
    size_t         print(char c) {
        return write((uint8_t)c);
    }
    size_t print(const char * data) {
        return write(reinterpret_cast<const uint8_t *>(data), strlen(data));
    }
    size_t print(const __FlashStringHelper * data) {
        return print(reinterpret_cast<const char *>(data));
    }
    size_t print(const Printable & printable) {
        return printable.printTo(*this);
    }
    size_t print(int value) {
        return print(std::to_string(value).c_str());
    }
    size_t print(unsigned int value) {
        return print(std::to_string(value).c_str());
    }
    size_t print(long value) {
        return print(std::to_string(value).c_str());
    }
    size_t print(unsigned long value) {
        return print(std::to_string(value).c_str());
    }
    size_t println() {
        return print("\r\n");
    }
    size_t println(const char * data) {
        return print(data) + println();
    }
    size_t println(const __FlashStringHelper * data) {
        return print(reinterpret_cast<const char *>(data)) + println();
    }
    size_t println(const Printable & printable) {
        return printable.printTo(*this) + println();
    }
    size_t println(int value) {
        return print(std::to_string(value).c_str()) + println();
    }
    size_t println(unsigned int value) {
        return print(std::to_string(value).c_str()) + println();
    }
    size_t println(long value) {
        return print(std::to_string(value).c_str()) + println();
    }
    size_t println(unsigned long value) {
        return print(std::to_string(value).c_str()) + println();
    }
    virtual void flush(){};
};

class Stream : public Print {
  public:
    virtual int available() = 0;
    virtual int read()      = 0;
    virtual int peek()      = 0;
};

class NativeConsole : public Stream {
  public:
    void begin(unsigned long baud __attribute__((unused))) {
    }

    int available() override {
        if (peek() >= 0) {
            return 1;
        } else {
            return 0;
        }
    }

    int read() override {
        peek();

        if (peek_) {
            peek_ = false;
            return peek_data_;
        } else {
            return -1;
        }
    }

    int peek() override {
        if (!peek_) {
            int ret = ::read(STDIN_FILENO, &peek_data_, 1);
            peek_   = ret > 0;
        }

        if (peek_) {
            return peek_data_;
        } else {
            return -1;
        }
    }

    size_t write(uint8_t c) override {
        return ::write(STDOUT_FILENO, &c, 1);
    }

    size_t write(const uint8_t * buffer, size_t size) override {
        return ::write(STDOUT_FILENO, buffer, size);
    }

  private:
    bool          peek_ = false;
    unsigned char peek_data_;
};

extern NativeConsole Serial;

unsigned long millis();

void delay(unsigned long millis);

void yield(void);

void setup(void);
void loop(void);

#include "WString.h"

#endif
