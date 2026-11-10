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

#ifndef EMSESP_PASSWORD_CONFIRM_H
#define EMSESP_PASSWORD_CONFIRM_H

#include "spin_lock.h"

#include <array>
#include <cstdint>
#include <cstring>

// The sign-ins whose password was entered again in the last few minutes, so a copied access token alone
// can't change the firmware, the users or the security settings. Kept in memory only, a restart clears it.
// A sign-in is known by the start of its access token's signature.
class PasswordConfirm {
  public:
    static constexpr uint32_t DURATION_MS  = 5 * 60 * 1000;
    static constexpr size_t   MAX_SESSIONS = 4;
    static constexpr size_t   ID_CHARS     = 16; // of the base64url HMAC-SHA256 signature, 96 bits can't be guessed

    void confirm(const char * token, uint32_t now) {
        Id id;
        if (!signature(token, id)) {
            return;
        }

        std::lock_guard<SpinLock> lock(lock_);

        Entry * entry = find(id, now);
        if (!entry) {
            entry = &entries_[0];
            for (auto & e : entries_) {
                if (!active(e, now)) {
                    entry = &e;
                    break;
                }
                if (before(e.until, entry->until)) {
                    entry = &e; // the one that expires first
                }
            }
            entry->id = id;
        }
        entry->until = now + DURATION_MS;
        entry->used  = true;
    }

    bool confirmed(const char * token, uint32_t now) {
        Id id;
        if (!signature(token, id)) {
            return false;
        }

        std::lock_guard<SpinLock> lock(lock_);
        return find(id, now) != nullptr;
    }

    void clear() {
        std::lock_guard<SpinLock> lock(lock_);
        entries_ = {};
    }

  private:
    struct Id {
        char    chars[ID_CHARS];
        uint8_t len;

        bool operator==(const Id & other) const {
            return len == other.len && memcmp(chars, other.chars, len) == 0;
        }
    };

    struct Entry {
        Id       id;
        bool     used;
        uint32_t until;
    };

    std::array<Entry, MAX_SESSIONS> entries_{};
    SpinLock                        lock_;

    // works across the millis() wrap around
    static bool before(uint32_t a, uint32_t b) {
        return static_cast<int32_t>(a - b) < 0;
    }

    static bool active(const Entry & entry, uint32_t now) {
        return entry.used && before(now, entry.until);
    }

    // the part after the last dot
    static bool signature(const char * token, Id & id) {
        if (!token) {
            return false;
        }
        const char * dot = strrchr(token, '.');
        if (!dot) {
            return false;
        }
        const size_t len = strlen(dot + 1);
        if (len == 0) {
            return false;
        }
        id.len = len < ID_CHARS ? len : ID_CHARS;
        memcpy(id.chars, dot + 1, id.len);
        return true;
    }

    Entry * find(const Id & id, uint32_t now) {
        for (auto & entry : entries_) {
            if (active(entry, now) && entry.id == id) {
                return &entry;
            }
        }
        return nullptr;
    }
};

#endif
