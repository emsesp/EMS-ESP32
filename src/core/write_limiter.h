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

#ifndef EMSESP_WRITE_LIMITER_H
#define EMSESP_WRITE_LIMITER_H

#include <array>
#include <cstdint>
#include <mutex>

// Protects the heating devices from automations that write too often.
// A write of the value last sent to the same entity within REPEAT_MS is skipped.
// More than limit writes to one entity, or GLOBAL_MAX writes to all entities, within WINDOW_MS are logged or refused.
class WriteLimiter {
  public:
    static constexpr uint32_t REPEAT_MS    = 60 * 1000;
    static constexpr uint32_t WINDOW_MS    = 60 * 1000;
    static constexpr uint16_t GLOBAL_MAX   = 120;
    static constexpr size_t   MAX_ENTITIES = 32;

    enum class Verdict : uint8_t { SEND, SKIP, OVER_LIMIT, REFUSE };

    static WriteLimiter & instance() {
        static WriteLimiter limiter;
        return limiter;
    }

    // limit 0 turns the per-entity and global limits off
    void configure(bool skip_repeat, uint8_t limit, bool refuse) {
        std::lock_guard<std::mutex> lock(mutex_);
        skip_repeat_ = skip_repeat;
        limit_       = limit;
        refuse_      = refuse;
    }

    // entity nullptr is a raw telegram, which only counts towards the global limit.
    // ms is how long ago the value was sent for SKIP, and the wait for REFUSE.
    // log is set for the first OVER_LIMIT or REFUSE of an entity in a window
    Verdict check(const char * entity, const char * value, uint32_t now, uint32_t & ms, bool & log) {
        std::lock_guard<std::mutex> lock(mutex_);
        ms  = 0;
        log = false;

        Entry * entry = nullptr;
        if (entity) {
            const uint32_t k = key(entity);
            entry            = find(k);
            if (entry && skip_repeat_ && entry->sent_value && entry->value == key(value) && now - entry->sent < REPEAT_MS) {
                skipped_++;
                ms = now - entry->sent;
                return Verdict::SKIP;
            }
            if (!entry) {
                entry  = oldest();
                *entry = Entry{k, 0, 0, now, now, 0, false, false, true};
            }
            entry->last = now;
        }

        if (!limit_) {
            return Verdict::SEND;
        }

        if (global_count_ == 0 || now - global_start_ >= WINDOW_MS) {
            global_start_  = now;
            global_count_  = 0;
            global_warned_ = false;
        }
        bool entity_over = false;
        if (entry) {
            if (now - entry->window_start >= WINDOW_MS) {
                entry->window_start = now;
                entry->count        = 0;
                entry->warned       = false;
            }
            entity_over = entry->count >= limit_;
        }
        const bool global_over = global_count_ >= GLOBAL_MAX;
        if (!entity_over && !global_over) {
            return Verdict::SEND;
        }

        limited_++;
        if (entity_over && !entry->warned) {
            entry->warned = true;
            log           = true;
        }
        if (global_over && !global_warned_) {
            global_warned_ = true;
            log            = true;
        }
        if (!refuse_) {
            return Verdict::OVER_LIMIT;
        }
        if (entity_over) {
            ms = WINDOW_MS - (now - entry->window_start);
        }
        if (global_over && WINDOW_MS - (now - global_start_) > ms) {
            ms = WINDOW_MS - (now - global_start_);
        }
        return Verdict::REFUSE;
    }

    // after the command took the value
    void sent(const char * entity, const char * value, uint32_t now) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (global_count_ < UINT16_MAX) {
            global_count_++;
        }
        if (!entity) {
            return;
        }
        Entry * entry = find(key(entity));
        if (!entry) {
            return;
        }
        entry->value      = key(value);
        entry->sent       = now;
        entry->sent_value = true;
        if (entry->count < UINT8_MAX) {
            entry->count++;
        }
    }

    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        entries_       = {};
        global_start_  = 0;
        global_count_  = 0;
        global_warned_ = false;
    }

    uint32_t skipped() const {
        return skipped_;
    }

    uint32_t limited() const {
        return limited_;
    }

  private:
    struct Entry {
        uint32_t key;
        uint32_t value; // hash of the last value sent
        uint32_t sent;
        uint32_t last;
        uint32_t window_start;
        uint8_t  count;
        bool     sent_value;
        bool     warned;
        bool     used;
    };

    std::array<Entry, MAX_ENTITIES> entries_{};
    uint32_t                        global_start_  = 0;
    uint16_t                        global_count_  = 0;
    bool                            global_warned_ = false;
    bool                            skip_repeat_   = true;
    uint8_t                         limit_         = 10;
    bool                            refuse_        = false;
    uint32_t                        skipped_       = 0;
    uint32_t                        limited_       = 0;
    std::mutex                      mutex_;

    static uint32_t key(const char * s) {
        uint32_t hash = 2166136261u; // FNV-1a
        for (; *s; s++) {
            hash = (hash ^ static_cast<uint8_t>(*s)) * 16777619u;
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

    // a free slot, otherwise the one used longest ago
    Entry * oldest() {
        Entry * result = &entries_[0];
        for (auto & entry : entries_) {
            if (!entry.used) {
                return &entry;
            }
            if (static_cast<int32_t>(entry.last - result->last) < 0) {
                result = &entry;
            }
        }
        return result;
    }
};

#endif
