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
#include <math.h>
#include <algorithm>
#include <Print.h>
#include <Printable.h>
#include <Stream.h>
#include <iostream>

#include "WString.h"
#include "Network.h"

extern ETHClass  ETH;
extern WiFiClass WiFi;

typedef double double_t;

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

#define snprintf snprintf_P // to keep backwards compatibility

void     pinMode(uint8_t pin, uint8_t mode);
void     digitalWrite(uint8_t pin, uint8_t value);
int      digitalRead(uint8_t pin);
uint32_t analogReadMilliVolts(uint8_t pin);

typedef enum { ADC_0db, ADC_2_5db, ADC_6db, ADC_11db } adc_attenuation_t;
void   analogSetPinAttenuation(uint8_t pin, adc_attenuation_t attenuation);
void   analogSetAttenuation(adc_attenuation_t attenuation);
void   dacWrite(uint8_t pin, uint8_t value);
double ledcSetup(uint8_t chan, double freq, uint8_t bit_num);
void   ledcAttachPin(uint8_t pin, uint8_t chan);
void   ledcWrite(uint8_t chan, uint32_t duty);
void   neopixelWrite(uint8_t pin, uint8_t red_val, uint8_t green_val, uint8_t blue_val);

#define PROGMEM
#define PGM_P const char *

class __FlashStringHelper;
#define FPSTR(string_literal) (reinterpret_cast<const __FlashStringHelper *>(string_literal))
#define F(string_literal) (FPSTR((string_literal)))

int snprintf_P(char * str, size_t size, const char * format, ...);
int vsnprintf_P(char * str, size_t size, const char * format, va_list ap);

// #define pgm_read_byte(addr) (*reinterpret_cast<const char *>(addr))
// #define pgm_read_ptr(p) (reinterpret_cast<const void *>(p))
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#define pgm_read_float(addr) (*(const float *)(addr))
#define pgm_read_ptr(addr) (*(const void **)(addr))

// #if defined(__linux__)
#include <chrono> // NOLINT [build/c++11]
#include <thread> // NOLINT [build/c++11] for yield()
#define millis() std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count()
// #endif

int64_t esp_timer_get_time();

void delay(unsigned long millis);

void yield(void);

void setup(void);
void loop(void);

class Print;

class NativeConsole : public Stream {
  public:
    void begin(unsigned long baud __attribute__((unused))) {
    }

    int available() override {
#if defined(_WIN32)
        // added for EMS-ESP
        return 1;
#else

        if (peek_ != -1)
            return 1;

        struct timeval timeout;
        fd_set         rfds;

        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);

        timeout.tv_sec  = 0;
        timeout.tv_usec = 1000;

        return ::select(STDIN_FILENO + 1, &rfds, NULL, NULL, &timeout) > 0 ? 1 : 0;
#endif
    }

    int read() override {
        if (peek_ != -1) {
            uint8_t c = peek_;
            peek_     = -1;
            return c;
        }

        if (available() > 0) {
            uint8_t c;
            int     ret = ::read(STDIN_FILENO, &c, 1);

            if (ret == 0) {
                /* Ctrl+D */
                return '\x04';
            } else if (ret == 1) {
                /* Remap Ctrl+Z to Ctrl-\ */
                if (c == '\x1A')
                    c = '\x1C';
                return c;
            } else {
                exit(1);
            }
        }

        return -1;
    }

    int peek() override {
        if (peek_ == -1)
            peek_ = read();

        return peek_;
    }

    size_t write(uint8_t c) override {
        if (::write(STDOUT_FILENO, &c, 1) == 1) {
            return 1;
        } else {
            exit(1);
        }
    }

    size_t write(const uint8_t * buffer, size_t size) override {
        if (::write(STDOUT_FILENO, buffer, size) == (ssize_t)size) {
            return size;
        } else {
            exit(1);
        }
    }

  private:
    int peek_ = -1;
};

extern NativeConsole Serial;

#endif
