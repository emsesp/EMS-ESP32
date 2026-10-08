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

#ifndef EMSESP_PSRAM_JSON_ALLOCATOR_H
#define EMSESP_PSRAM_JSON_ALLOCATOR_H

#include <ArduinoJson.h>

#ifndef EMSESP_STANDALONE
#include <esp_heap_caps.h>
#endif

namespace emsesp {

// PSRAM-backed ArduinoJson allocator with internal-heap fallback.
//
// Rationale: by default ArduinoJson allocates with malloc(), which on the
// ESP32 lands in internal DRAM. Large transient JsonDocuments (full MQTT
// payload, HA discovery configs, Web API responses, settings load/save)
// were eating multiple KB of the same internal heap that LwIP, mbedTLS and
// AsyncTCP also need. Routing them through SPIRAM via heap_caps_malloc
// keeps internal heap available for the network stack, at the cost of a
// small latency penalty on PSRAM reads/writes (a few cycles per access,
// negligible for JSON build-up which is dominated by string formatting).
//
// On the standalone (Linux) build, PSRAM doesn't exist; the allocator
// silently falls through to plain malloc/free/realloc.
//
// Usage:
//   JsonDocument doc(emsesp::PsramJsonAllocator::instance());
// or with the convenience macro:
//   JsonDocument doc(PSRAM_DOC);
class PsramJsonAllocator : public ArduinoJson::Allocator {
  public:
    void * allocate(size_t size) override {
#ifdef EMSESP_STANDALONE
        return malloc(size);
#else
        // Try SPIRAM first; fall back to internal heap so we never fail
        // on boards without PSRAM or when PSRAM is full.
        void * p = heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
        if (p == nullptr) {
            p = malloc(size);
        }
        return p;
#endif
    }

    void deallocate(void * ptr) override {
#ifdef EMSESP_STANDALONE
        free(ptr);
#else
        // heap_caps_free handles both PSRAM- and internal-heap pointers.
        heap_caps_free(ptr);
#endif
    }

    void * reallocate(void * ptr, size_t new_size) override {
#ifdef EMSESP_STANDALONE
        return realloc(ptr, new_size);
#else
        // Prefer keeping the block in PSRAM; heap_caps_realloc will move
        // the data if the original region can't be grown in-place.
        void * p = heap_caps_realloc(ptr, new_size, MALLOC_CAP_SPIRAM);
        if (p == nullptr) {
            p = realloc(ptr, new_size);
        }
        return p;
#endif
    }

    static ArduinoJson::Allocator * instance() {
        static PsramJsonAllocator inst;
        return &inst;
    }

  private:
    PsramJsonAllocator()  = default;
    ~PsramJsonAllocator() = default;
};

} // namespace emsesp

// Convenience shorthand. Use only for *large* or *transient* JsonDocuments
// (MQTT publish payloads, HA discovery, full API responses, big settings
// load/save). For small hot-path docs (single-command output, parse of a
// short HTTP body), keep the default allocator: PSRAM has higher access
// latency than internal SRAM, so tiny docs are faster on the regular heap.
#define PSRAM_DOC emsesp::PsramJsonAllocator::instance()

#endif
