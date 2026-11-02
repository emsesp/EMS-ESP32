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

#include <array>
#include <cstdint>
#include <cstring>
#include <mutex>

// The sign-ins whose password was entered again in the last few minutes, so a copied access token alone
// can't change the firmware, the users or the security settings. Kept in memory only, a restart clears it.
// A sign-in is known by the signature of its access token.
class PasswordConfirm {
  public:
    static constexpr uint32_t DURATION_MS  = 5 * 60 * 1000;
    static constexpr size_t   MAX_SESSIONS = 4;
    static constexpr size_t   MAX_ID       = 64; // a base64url HMAC-SHA256 signature is 43 characters

    void confirm(const char * token, uint32_t now) {
        char id[MAX_ID + 1];
        if (!signature(token, id)) {
            return;
        }

        std::lock_guard<std::mutex> lock(mutex_);

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
            strcpy(entry->id, id);
        }
        entry->until = now + DURATION_MS;
        entry->used  = true;
    }

    bool confirmed(const char * token, uint32_t now) {
        char id[MAX_ID + 1];
        if (!signature(token, id)) {
            return false;
        }

        std::lock_guard<std::mutex> lock(mutex_);
        return find(id, now) != nullptr;
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        entries_ = {};
    }

  private:
    struct Entry {
        char     id[MAX_ID + 1];
        uint32_t until;
        bool     used;
    };

    std::array<Entry, MAX_SESSIONS> entries_{};
    std::mutex                      mutex_;

    // works across the millis() wrap around
    static bool before(uint32_t a, uint32_t b) {
        return static_cast<int32_t>(a - b) < 0;
    }

    static bool active(const Entry & entry, uint32_t now) {
        return entry.used && before(now, entry.until);
    }

    // the part after the last dot
    static bool signature(const char * token, char * id) {
        if (!token) {
            return false;
        }
        const char * dot = strrchr(token, '.');
        if (!dot) {
            return false;
        }
        const size_t len = strlen(dot + 1);
        if (len == 0 || len > MAX_ID) {
            return false;
        }
        memcpy(id, dot + 1, len + 1);
        return true;
    }

    Entry * find(const char * id, uint32_t now) {
        for (auto & entry : entries_) {
            if (active(entry, now) && strcmp(entry.id, id) == 0) {
                return &entry;
            }
        }
        return nullptr;
    }
};

#endif
