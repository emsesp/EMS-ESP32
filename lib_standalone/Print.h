/*
 * mcu-app - Microcontroller application framework
 * Copyright 2022  Simon Arlott
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

#ifndef PRINT_H_
#define PRINT_H_

#include <stdarg.h>
#include <unistd.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

#include <Printable.h>

#include "WString.h"

int vsnprintf_P(char * str, size_t size, const char * format, va_list ap);

class Print {
  public:
    virtual size_t write(uint8_t c) = 0;
    virtual size_t write(const uint8_t * buffer, size_t size) {
        for (size_t i = 0; i < size; i++) {
            write(*buffer);
            buffer++;
        };
        return size;
    };
    void setWriteError(int err = 1) {
        err_ = err;
    }
    int getWriteError() {
        return err_;
    }
    void clearWriteError() {
    }
    size_t print(char c) {
        return write((uint8_t)c);
    }
    size_t print(const char * data) {
        return write(reinterpret_cast<const uint8_t *>(data), strlen(data));
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
    size_t printf(const char * format, ...) {
        char    str[4096] = {0};
        va_list ap;

        va_start(ap, format);
        int ret = vsnprintf_P(str, sizeof(str), format, ap);
        va_end(ap);

        if (ret < 0)
            return ret;

        return print(str);
    }
    size_t println() {
        // added for EMS-ESP
#if defined(_WIN32)
        return print("\n");
#else
        return print("\r\n");
#endif
    }
    size_t println(const char * data) {
        return print(data) + println();
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
    virtual void flush() {};

  private:
    int err_{0};
};

#endif
