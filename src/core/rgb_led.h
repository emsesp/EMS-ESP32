/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2025  emsesp.org
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

#ifndef EMSESP_RGB_LED_H_
#define EMSESP_RGB_LED_H_

#include <Arduino.h>

namespace emsesp {

// WS2812B driver replacing arduino-esp32's rgbLedWrite()/neopixelWrite(), which hard-code
// T0H = 400 ns, T1L = 400 ns and have no reset gap. The WS2812B-2020-V6 needs T0H 220-380 ns,
// T1L 580-1000 ns and a reset > 280 us, otherwise it misreads 0-bits as 1-bits (washed-out white).
// Timings used (10 MHz RMT clock, 1 tick = 100 ns) fit both the original WS2812B and the V6:
//
//            sent     old WS2812B    WS2812B-2020-V6
//   T0H     300 ns    250-550 ns     220-380 ns
//   T0L     900 ns    700-1000 ns    580-1000 ns
//   T1H     800 ns    650-950 ns     580-1000 ns
//   T1L     600 ns    300-600 ns     580-1000 ns
//   Reset  >300 us    >50 us         >280 us
//
// Supports both Arduino Core 2.x (handle based RMT API) and Core 3.x (pin based RMT API).
// Do not mix with rgbLedWrite()/neopixelWrite() on the same pin, as both claim the pin's RMT channel.
namespace rgb_led {

enum class Order : uint8_t { GRB, RGB }; // WS2812B is GRB

// Blocks ~30 us for the transmit, plus up to the reset gap if called again right after the previous update.
// Returns void so it can be used as a drop-in for neopixelWrite()/rgbLedWrite(), e.g. in a ternary with digitalWrite()
void write(uint8_t pin, uint8_t red, uint8_t green, uint8_t blue, Order order = Order::GRB);
void end(uint8_t pin); // release the pin's RMT channel, e.g. before reusing the pin for something else

} // namespace rgb_led

} // namespace emsesp

#endif
