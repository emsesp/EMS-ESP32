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

#ifndef EMSESP_PSRAM_ASYNC_JSON_RESPONSE_H
#define EMSESP_PSRAM_ASYNC_JSON_RESPONSE_H

#include "psram_json_allocator.h"

#ifndef EMSESP_STANDALONE
#include <AsyncJson.h>
#include <ChunkPrint.h>
#else
#include <AsyncJson.h>
#endif

namespace emsesp {

// AsyncJsonResponse subclass whose JsonDocument lives in PSRAM instead of
// internal SRAM.
//
// Why: every web API response goes through AsyncJsonResponse. The library's
// base class declares `JsonDocument _jsonBuffer;` with the *default*
// allocator, which on ESP32 means malloc() → internal heap. For large
// payloads (Dashboard, /rest/coreData, /rest/sensorData, full settings,
// customizations, etc.) this transiently consumes many KB of the same
// internal heap that LwIP / AsyncTCP / mbedTLS also need. Each concurrent
// browser tab compounds the cost.
//
// We can't change the base class's _jsonBuffer allocator (the upstream
// constructor doesn't take one), but we can route around it: keep our own
// PSRAM-backed document, override the virtual setLength()/_fillBuffer() so
// the framework serialises *our* document, and name-hide getRoot() so
// callers populate *our* document. The base's _jsonBuffer stays empty
// (just one root slot, <~32 bytes).
//
// Callers must use the derived type (or `auto`) when calling getRoot(),
// because getRoot() is non-virtual in the base. `request->send(response)`
// works as-is because setLength()/_fillBuffer() ARE virtual in the
// AsyncAbstractResponse grandparent.
//
// On standalone the lib_standalone AsyncJsonResponse stub never actually
// serves responses, so this whole class still compiles and behaves
// identically (allocator falls back to malloc anyway).
class PsramAsyncJsonResponse : public ::AsyncJsonResponse {
  public:
    explicit PsramAsyncJsonResponse(bool isArray = false)
        : ::AsyncJsonResponse(isArray)
        , psram_doc_(PsramJsonAllocator::instance()) {
        if (isArray) {
            psram_root_ = psram_doc_.add<JsonArray>();
        } else {
            psram_root_ = psram_doc_.add<JsonObject>();
        }
    }

    // Hides AsyncJsonResponse::getRoot(). Must be called through a
    // derived-type pointer/reference (the framework's base pointer keeps
    // pointing at the empty base _jsonBuffer, which is intentional).
    JsonVariant getRoot() {
        return psram_root_;
    }

#ifndef EMSESP_STANDALONE
    size_t setLength() override {
        _contentLength = measureJson(psram_root_);
        if (_contentLength) {
            _isValid = true;
        }
        return _contentLength;
    }

    size_t _fillBuffer(uint8_t * data, size_t len) override {
        ChunkPrint dest(data, _sentLength, len);
        serializeJson(psram_root_, dest);
        return dest.written();
    }
#endif

  private:
    JsonDocument psram_doc_;
    JsonVariant  psram_root_;
};

#if !defined(EMSESP_STANDALONE) && defined(ASYNC_MSG_PACK_SUPPORT) && ASYNC_MSG_PACK_SUPPORT == 1
// MessagePack equivalent — same routing trick but serialises with MsgPack.
class PsramAsyncMessagePackResponse : public ::AsyncMessagePackResponse {
  public:
    explicit PsramAsyncMessagePackResponse(bool isArray = false)
        : ::AsyncMessagePackResponse(isArray)
        , psram_doc_(PsramJsonAllocator::instance()) {
        if (isArray) {
            psram_root_ = psram_doc_.add<JsonArray>();
        } else {
            psram_root_ = psram_doc_.add<JsonObject>();
        }
    }

    JsonVariant getRoot() {
        return psram_root_;
    }

    size_t setLength() override {
        _contentLength = measureMsgPack(psram_root_);
        if (_contentLength) {
            _isValid = true;
        }
        return _contentLength;
    }

    size_t _fillBuffer(uint8_t * data, size_t len) override {
        ChunkPrint dest(data, _sentLength, len);
        serializeMsgPack(psram_root_, dest);
        return dest.written();
    }

  private:
    JsonDocument psram_doc_;
    JsonVariant  psram_root_;
};
#else
// Standalone or no msgpack support: alias to plain JSON response so the
// codebase compiles unchanged.
using PsramAsyncMessagePackResponse = PsramAsyncJsonResponse;
#endif

} // namespace emsesp

#endif
