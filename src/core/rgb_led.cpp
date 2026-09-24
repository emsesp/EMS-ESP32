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

#include "rgb_led.h"

#ifndef EMSESP_STANDALONE
#include <esp32-hal-rmt.h>
#endif

namespace emsesp {

namespace rgb_led {

#ifndef EMSESP_STANDALONE

namespace {

// durations in 100 ns ticks
constexpr uint16_t T0H = 3; // 300 ns
constexpr uint16_t T0L = 9; // 900 ns
constexpr uint16_t T1H = 8; // 800 ns
constexpr uint16_t T1L = 6; // 600 ns

// rmt_data_t.val layout: duration0:15, level0:1, duration1:15, level1:1 (high then low)
constexpr uint32_t SYMBOL_0 = T0H | (1UL << 15) | (static_cast<uint32_t>(T0L) << 16);
constexpr uint32_t SYMBOL_1 = T1H | (1UL << 15) | (static_cast<uint32_t>(T1L) << 16);

constexpr uint32_t RESET_US = 300;
constexpr uint8_t  NO_PIN   = 0xFF;
constexpr uint8_t  MAX_PINS = 2; // status LED + an RGB analog sensor; the ESP32-C3 only has 2 RMT TX channels

#if ESP_ARDUINO_VERSION_MAJOR < 3
constexpr float TICK_NS = 100.0f; // 1 tick = 100 ns
#else
constexpr uint32_t RMT_FREQ_HZ = 10000000; // 10 MHz, 1 tick = 100 ns
#endif

struct Channel {
    uint8_t  pin        = NO_PIN;
    uint32_t last_tx_us = 0;
#if ESP_ARDUINO_VERSION_MAJOR < 3
    rmt_obj_t * rmt = nullptr; // Core 2 addresses the channel by handle, Core 3 by pin
#endif
};

Channel channels[MAX_PINS];

void release(Channel & ch) {
    if (ch.pin == NO_PIN) {
        return;
    }
#if ESP_ARDUINO_VERSION_MAJOR < 3
    rmtDeinit(ch.rmt);
    ch.rmt = nullptr;
#else
    rmtDeinit(ch.pin);
#endif
    ch.pin = NO_PIN;
}

Channel * init(uint8_t pin) {
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
        release(*free_slot);
    }

#if ESP_ARDUINO_VERSION_MAJOR < 3
    rmt_obj_t * rmt = rmtInit(pin, RMT_TX_MODE, RMT_MEM_64);
    if (!rmt) {
        return nullptr;
    }
    rmtSetTick(rmt, TICK_NS);
    free_slot->rmt = rmt;
#else
    if (!rmtInit(pin, RMT_TX_MODE, RMT_MEM_NUM_BLOCKS_1, RMT_FREQ_HZ)) {
        return nullptr;
    }
#endif

    free_slot->pin        = pin;
    free_slot->last_tx_us = micros() - RESET_US; // no wait on first write
    return free_slot;
}

} // namespace

void write(uint8_t pin, uint8_t red, uint8_t green, uint8_t blue, Order order) {
    Channel * ch = init(pin);
    if (!ch) {
        return;
    }

    // enforce the latch/reset gap since the previous frame (V6 needs > 280 us)
    const uint32_t elapsed = micros() - ch->last_tx_us;
    if (elapsed < RESET_US) {
        delayMicroseconds(RESET_US - elapsed);
    }

    const uint8_t bytes[3] = {order == Order::GRB ? green : red, order == Order::GRB ? red : green, blue};

    rmt_data_t frame[24];
    for (uint8_t i = 0; i < 24; i++) {
        frame[i].val = (bytes[i >> 3] & (0x80 >> (i & 7))) ? SYMBOL_1 : SYMBOL_0;
    }

#if ESP_ARDUINO_VERSION_MAJOR < 3
    rmtWriteBlocking(ch->rmt, frame, 24);
#else
    rmtWrite(pin, frame, 24, RMT_WAIT_FOR_EVER);
#endif
    ch->last_tx_us = micros();
}

void end(uint8_t pin) {
    for (auto & ch : channels) {
        if (ch.pin == pin) {
            release(ch);
        }
    }
}

#else

void write(uint8_t, uint8_t, uint8_t, uint8_t, Order) {
}

void end(uint8_t) {
}

#endif

} // namespace rgb_led

} // namespace emsesp
