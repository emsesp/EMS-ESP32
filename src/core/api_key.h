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

#ifndef EMSESP_API_KEY_H
#define EMSESP_API_KEY_H

#include "password_hash.h"

// API keys for /api, sent as "Authorization: Bearer emsesp_<40 hex digits>".
// The first ID_CHARS hex digits identify the key, only a hash of the whole key is stored.
// A single HMAC is enough as the keys are random, unlike passwords.
class ApiKey {
  public:
    static constexpr const char * PREFIX     = "emsesp_";
    static constexpr size_t       KEY_BYTES  = 20;
    static constexpr size_t       ID_CHARS   = 8;
    static constexpr size_t       MAX_KEYS   = 10;
    static constexpr size_t       MAX_LABEL  = 32;
    static constexpr size_t       PREFIX_LEN = 7;

    static String generate() {
        uint8_t random[KEY_BYTES];
        PasswordHash::random_bytes(random, sizeof(random));
        return String((std::string(PREFIX) + PasswordHash::to_hex(random, sizeof(random))).c_str());
    }

    static bool is_key(const char * key) {
        if (strncmp(key, PREFIX, PREFIX_LEN) || strlen(key) != PREFIX_LEN + KEY_BYTES * 2) {
            return false;
        }
        for (const char * p = key + PREFIX_LEN; *p; p++) {
            if (!((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f'))) {
                return false;
            }
        }
        return true;
    }

    // "" if it's not a key
    static String id(const char * key) {
        return is_key(key) ? String(std::string(key + PREFIX_LEN, ID_CHARS).c_str()) : String("");
    }

    static String hash(const char * key) {
        const std::string id_str = std::string(key + PREFIX_LEN, ID_CHARS);
        uint8_t           digest[PasswordHash::HASH_BYTES];
        PasswordHash::pbkdf2(String(key), reinterpret_cast<const uint8_t *>(id_str.c_str()), id_str.length(), 1, digest);
        return String(PasswordHash::to_hex(digest, sizeof(digest)).c_str());
    }

    // an empty stored hash never matches
    static bool matches(const char * key, const char * stored_hash) {
        if (!is_key(key) || !stored_hash || strlen(stored_hash) != PasswordHash::HASH_BYTES * 2) {
            return false;
        }
        const String computed = hash(key);
        return PasswordHash::equal(reinterpret_cast<const uint8_t *>(computed.c_str()),
                                   reinterpret_cast<const uint8_t *>(stored_hash),
                                   PasswordHash::HASH_BYTES * 2);
    }

    static bool matches(const char * key, const String & stored_hash) {
        return matches(key, stored_hash.c_str());
    }
};

#endif
