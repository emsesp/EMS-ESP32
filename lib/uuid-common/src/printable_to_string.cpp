/*
 * uuid-common - Microcontroller common utilities
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

#include <uuid/common.h>

#include <Arduino.h>

#include <string>

namespace uuid {

class PrintableString : public ::Print {
  public:
    explicit PrintableString(std::string & output)
        : output_(output) {
    }
    ~PrintableString() = default;

    size_t write(uint8_t data) final override {
        output_.append(1, reinterpret_cast<unsigned char>(data));
        return 1;
    }

    size_t write(const uint8_t * buffer, size_t size) final override {
        output_.append(reinterpret_cast<const char *>(buffer), size);
        return size;
    }

  private:
    std::string & output_;
};

size_t print_to_string(const Printable & printable, std::string & output) {
    PrintableString pstr{output};

    return printable.printTo(pstr);
}

std::string printable_to_string(const Printable & printable) {
    std::string str;

    print_to_string(printable, str);

    return str;
}

} // namespace uuid
