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

#ifndef EMSESP_LOGIN_LIMITER_H
#define EMSESP_LOGIN_LIMITER_H

#include "spin_lock.h"

#include <array>
#include <cstdint>

// Slows down password guessing, for each source (an IP address or a console) and for all sources together.
// After FREE_ATTEMPTS failures a source has to wait, and the wait doubles with every further failure.
class LoginLimiter {
  public:
    static constexpr uint8_t  FREE_ATTEMPTS     = 5;
    static constexpr uint32_t FIRST_LOCKOUT_MS  = 30 * 1000;
    static constexpr uint32_t MAX_LOCKOUT_MS    = 15 * 60 * 1000;
    static constexpr uint32_t FORGET_MS         = 30 * 60 * 1000; // a source's failures are forgotten after this long without one
    static constexpr uint8_t  GLOBAL_FAILURES   = 20;             // failures from all sources within GLOBAL_WINDOW_MS
    static constexpr uint32_t GLOBAL_WINDOW_MS  = 60 * 1000;
    static constexpr uint32_t GLOBAL_LOCKOUT_MS = 60 * 1000;
    static constexpr size_t   MAX_SOURCES       = 8;

    // shared by the web sign-in and the consoles
    static LoginLimiter & instance() {
        static LoginLimiter limiter;
        return limiter;
    }

    // milliseconds until the source may try again, 0 if it may try now
    uint32_t retry_after(const char * source, uint32_t now) {
        const uint32_t            k = key(source);
        std::lock_guard<SpinLock> lock(lock_);

        uint32_t wait = 0;
        if (global_locked_ && before(now, global_locked_until_)) {
            wait = global_locked_until_ - now;
        }

        const Entry * entry = find(k);
        if (entry && entry->failures >= FREE_ATTEMPTS && before(now, entry->locked_until) && entry->locked_until - now > wait) {
            wait = entry->locked_until - now;
        }
        return wait;
    }

    void failed(const char * source, uint32_t now) {
        const uint32_t            k = key(source);
        std::lock_guard<SpinLock> lock(lock_);

        Entry * entry = find(k);
        if (!entry) {
            entry  = oldest();
            *entry = Entry{k, 0, now, now, true};
        }
        if (now - entry->last >= FORGET_MS) {
            entry->failures = 0;
        }
        if (entry->failures < UINT8_MAX) {
            entry->failures++;
        }
        entry->last = now;
        if (entry->failures >= FREE_ATTEMPTS) {
            const uint8_t doublings = entry->failures - FREE_ATTEMPTS;
            uint32_t      lockout   = doublings >= 5 ? MAX_LOCKOUT_MS : FIRST_LOCKOUT_MS << doublings;
            entry->locked_until     = now + (lockout < MAX_LOCKOUT_MS ? lockout : MAX_LOCKOUT_MS);
        }

        if (global_count_ == 0 || now - global_start_ >= GLOBAL_WINDOW_MS) {
            global_start_ = now;
            global_count_ = 0;
        }
        if (++global_count_ >= GLOBAL_FAILURES) {
            global_locked_       = true;
            global_locked_until_ = now + GLOBAL_LOCKOUT_MS;
            global_count_        = 0;
        }
    }

    void succeeded(const char * source) {
        const uint32_t            k = key(source);
        std::lock_guard<SpinLock> lock(lock_);

        Entry * entry = find(k);
        if (entry) {
            *entry = Entry{};
        }
    }

  private:
    struct Entry {
        uint32_t key;
        uint8_t  failures;
        uint32_t last;
        uint32_t locked_until;
        bool     used;
    };

    std::array<Entry, MAX_SOURCES> entries_{};
    uint32_t                       global_start_        = 0;
    uint8_t                        global_count_        = 0;
    uint32_t                       global_locked_until_ = 0;
    bool                           global_locked_       = false;
    SpinLock                       lock_;

    // works across the millis() wrap around
    static bool before(uint32_t a, uint32_t b) {
        return static_cast<int32_t>(a - b) < 0;
    }

    static uint32_t key(const char * source) {
        uint32_t hash = 2166136261u; // FNV-1a
        for (; *source; source++) {
            hash = (hash ^ static_cast<uint8_t>(*source)) * 16777619u;
        }
        return hash;
    }

    Entry * find(uint32_t k) {
        for (auto & entry : entries_) {
            if (entry.used && entry.key == k) {
                return &entry;
            }
        }
        return nullptr;
    }

    // a free slot, otherwise the one with the oldest failure
    Entry * oldest() {
        Entry * result = &entries_[0];
        for (auto & entry : entries_) {
            if (!entry.used) {
                return &entry;
            }
            if (before(entry.last, result->last)) {
                result = &entry;
            }
        }
        return result;
    }
};

#endif
