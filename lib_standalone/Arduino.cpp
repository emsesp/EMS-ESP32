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

#ifdef EMSESP_STANDALONE

#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <string>

#include "Arduino.h"
#include "Network.h"

NativeConsole Serial;

ETHClass  ETH;
WiFiClass WiFi;

/* millis() on C++ native could be
for non-Arduino
millis():
#include <chrono>
    auto start = std::chrono::system_clock::now();
    auto end = std::chrono::system_clock::now();
    diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
*/

static unsigned long __millis = 0;
static bool          __output_pins[256];
static int           __output_level[256];

std::atomic_bool exitProgram(false);

void ClientLoop(void * arg) {
    (void)arg;
    for (;;) {
        loop();
        if (exitProgram)
            break;
    }
}

#ifndef EMSESP_UNITY
// we have another main that overrides this when using Unity in test_api.cpp
int main(int argc __attribute__((unused)), char * argv[] __attribute__((unused))) {
    setup();

    std::thread t = std::thread(ClientLoop, nullptr);
    // while (millis() <= 10 * 1000) {
    while (1) {
        if (exitProgram)
            break;
        std::this_thread::yield();
    }
    t.join();
    return EXIT_SUCCESS;
}
#endif

int64_t esp_timer_get_time() {
    return __millis;
}

void delay(unsigned long millis) {
    __millis += millis;
}

void yield(void) {
    std::this_thread::yield();
}

int snprintf_P(char * str, size_t size, const char * format, ...) {
    va_list ap;

    va_start(ap, format);
    int ret = vsnprintf_P(str, size, format, ap);
    va_end(ap);

    return ret;
}

int vsnprintf_P(char * str, size_t size, const char * format, va_list ap) {
    std::string native_format;

    char previous = 0;
    for (size_t i = 0; i < strlen(format); i++) {
        char c = format[i];

        // This would be a lot easier if the ESP8266 platform
        // simply read all strings with 32-bit accesses instead
        // of repurposing %S (wchar_t).
        if (previous == '%' && c == 'S') {
            c = 's';
        }

        native_format += c;
        previous = c;
    }

    return vsnprintf(str, size, native_format.c_str(), ap);
}

void pinMode(uint8_t pin, uint8_t mode) {
    __output_pins[pin] = (mode == OUTPUT);
}

void digitalWrite(uint8_t pin, uint8_t value) {
    __output_level[pin] = (value == HIGH) ? HIGH : LOW;
}

int digitalRead(uint8_t pin) {
    if (__output_pins[pin]) {
        return __output_level[pin];
    } else if (pin & 1) {
        return HIGH;
    } else {
        return LOW;
    }
}

uint32_t analogReadMilliVolts(uint8_t pin) {
    return 0;
}

void   analogSetPinAttenuation(uint8_t pin, adc_attenuation_t attenuation) {};
void   analogSetAttenuation(adc_attenuation_t attenuation) {};
void   dacWrite(uint8_t pin, uint8_t value) {};
double ledcSetup(uint8_t chan, double freq, uint8_t bit_num) {
    return 0;
};
void ledcAttachPin(uint8_t pin, uint8_t chan) {};
void ledcWrite(uint8_t chan, uint32_t duty) {};
void neopixelWrite(uint8_t pin, uint8_t red_val, uint8_t green_val, uint8_t blue_val) {};

#endif