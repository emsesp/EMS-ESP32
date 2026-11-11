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

// Header only, the recovery firmware includes it too

#ifndef EMSESP_SPIN_LOCK_H
#define EMSESP_SPIN_LOCK_H

#include <mutex>

#ifndef EMSESP_STANDALONE
#include <freertos/FreeRTOS.h>
#endif

// For a few lines that only read and write memory, used with std::lock_guard. On the ESP32 a std::mutex
// allocates a FreeRTOS mutex on the heap, this doesn't. Interrupts are off while it's held, so nothing
// that blocks, logs or takes long may run under it
class SpinLock {
  public:
#ifdef EMSESP_STANDALONE
    void lock() {
        mutex_.lock();
    }
    void unlock() {
        mutex_.unlock();
    }

  private:
    std::mutex mutex_;
#else
    void lock() {
        portENTER_CRITICAL(&mux_);
    }
    void unlock() {
        portEXIT_CRITICAL(&mux_);
    }

  private:
    portMUX_TYPE mux_ = portMUX_INITIALIZER_UNLOCKED;
#endif
};

#endif
