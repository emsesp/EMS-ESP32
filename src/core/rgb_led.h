/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2026  emsesp.org
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

// Header-only so the recovery app (recovery/main.cpp) can use it without the rest of EMS-ESP

#include <Arduino.h>

#ifndef EMSESP_STANDALONE
#include <esp32-hal-rmt.h>
#endif

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
// Do not mix with rgbLedWrite()/neopixelWrite() on the same pin, as both claim the pin's RMT channel.
namespace rgb_led {

enum class Order : uint8_t { GRB, RGB }; // WS2812B is GRB

#ifndef EMSESP_STANDALONE

namespace detail {

// durations in 12.5 ns ticks
constexpr uint16_t T0H = 3.5 * 8; // 350 ns
constexpr uint16_t T0L = 9 * 8;   // 900 ns
constexpr uint16_t T1H = 8 * 8;   // 800 ns
constexpr uint16_t T1L = 6 * 8;   // 600 ns

// rmt_data_t.val layout: duration0:15, level0:1, duration1:15, level1:1 (high then low)
constexpr uint32_t SYMBOL_0 = T0H | (1UL << 15) | (static_cast<uint32_t>(T0L) << 16);
constexpr uint32_t SYMBOL_1 = T1H | (1UL << 15) | (static_cast<uint32_t>(T1L) << 16);

constexpr uint32_t RMT_FREQ_HZ = 80000000; // 80 MHz
constexpr uint32_t RESET_US    = 300;
constexpr uint8_t  NO_PIN      = 0xFF;
constexpr uint8_t  MAX_PINS    = 2; // status LED + an RGB analog sensor; the ESP32-C3 only has 2 RMT TX channels

struct Channel {
    uint8_t  pin        = NO_PIN;
    uint32_t last_tx_us = 0;
};

inline Channel channels[MAX_PINS];

} // namespace detail

// release the pin's RMT channel, e.g. before reusing the pin for something else
inline void end(uint8_t pin) {
    for (auto & ch : detail::channels) {
        if (ch.pin == pin && pin != detail::NO_PIN) {
            rmtDeinit(pin);
            ch.pin = detail::NO_PIN;
        }
    }
}

namespace detail {

inline Channel * init(uint8_t pin) {
    Channel * free_slot = nullptr;
    for (auto & ch : channels) {
        if (ch.pin == pin) {
            return &ch;
        }

        if (!free_slot && ch.pin == NO_PIN) {
            free_slot = &ch;
        }
    }

    if (!free_slot) {
        // all slots in use: release the least recently written one
        const uint32_t now = micros();
        free_slot          = &channels[0];
        for (auto & ch : channels) {
            if (now - ch.last_tx_us > now - free_slot->last_tx_us) {
                free_slot = &ch;
            }
        }

        end(free_slot->pin);
    }

    if (!rmtInit(pin, RMT_TX_MODE, RMT_MEM_NUM_BLOCKS_1, RMT_FREQ_HZ)) {
        return nullptr;
    }

    free_slot->pin        = pin;
    free_slot->last_tx_us = micros() - RESET_US; // no wait on first write
    return free_slot;
}

} // namespace detail

// Blocks ~30 us for the transmit, plus up to the reset gap if called again right after the previous update
inline bool write(uint8_t pin, uint8_t red, uint8_t green, uint8_t blue, Order order = Order::GRB) {
    detail::Channel * ch = detail::init(pin);
    if (!ch) {
        return false;
    }

    // enforce the latch/reset gap since the previous frame (V6 needs > 280 us)
    const uint32_t elapsed = micros() - ch->last_tx_us;
    if (elapsed < detail::RESET_US) {
        delayMicroseconds(detail::RESET_US - elapsed);
    }

    const uint8_t bytes[3] = {order == Order::GRB ? green : red, order == Order::GRB ? red : green, blue};

    rmt_data_t frame[24];
    for (uint8_t i = 0; i < 24; i++) {
        frame[i].val = (bytes[i >> 3] & (0x80 >> (i & 7))) ? detail::SYMBOL_1 : detail::SYMBOL_0;
    }

    const bool ok  = rmtWrite(pin, frame, 24, RMT_WAIT_FOR_EVER);
    ch->last_tx_us = micros();
    return ok;
}

#else

inline bool write(uint8_t, uint8_t, uint8_t, uint8_t, Order = Order::GRB) {
    return true;
}

inline void end(uint8_t) {
}

#endif

} // namespace rgb_led

} // namespace emsesp

#endif
