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

#ifndef EMSESP_BASIC_AUTH_H
#define EMSESP_BASIC_AUTH_H

#include "password_hash.h"
#include "spin_lock.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <string>

// HTTP Basic authentication on /api, so a browser can read values after its own login prompt.
// The browser sends these credentials with every request to the device, also ones started by other web sites,
// so a Basic login only ever reads.
class BasicAuth {
  public:
    static constexpr size_t MAX_HEADER = 256;

    // "Basic <base64 of username:password>" to the username and password, false if it isn't one
    static bool parse(const char * header, std::string & username, std::string & password) {
        static constexpr const char * PREFIX     = "Basic ";
        static constexpr size_t       PREFIX_LEN = 6;
        if (header == nullptr || strncmp(header, PREFIX, PREFIX_LEN) || strlen(header) > MAX_HEADER) {
            return false;
        }

        std::string decoded;
        if (!decode_base64(header + PREFIX_LEN, decoded)) {
            return false;
        }
        const size_t colon = decoded.find(':');
        if (colon == std::string::npos || colon == 0 || decoded.find('\0') != std::string::npos) {
            return false;
        }
        username = decoded.substr(0, colon);
        password = decoded.substr(colon + 1);
        return true;
    }

  private:
    static bool decode_base64(const char * in, std::string & out) {
        uint32_t buffer = 0;
        int      bits   = 0;
        size_t   pad    = 0;
        for (const char * p = in; *p; p++) {
            int v;
            if (*p >= 'A' && *p <= 'Z') {
                v = *p - 'A';
            } else if (*p >= 'a' && *p <= 'z') {
                v = *p - 'a' + 26;
            } else if (*p >= '0' && *p <= '9') {
                v = *p - '0' + 52;
            } else if (*p == '+') {
                v = 62;
            } else if (*p == '/') {
                v = 63;
            } else if (*p == '=') {
                pad++;
                continue;
            } else {
                return false;
            }
            if (pad) {
                return false; // data after the padding
            }
            buffer = (buffer << 6) | v;
            bits += 6;
            if (bits >= 8) {
                bits -= 8;
                out += static_cast<char>((buffer >> bits) & 0xFF);
            }
        }
        return pad <= 2 && !out.empty();
    }
};

// Recent successful Basic logins with a password. The browser sends the password with every request, and checking
// it is a PBKDF2 run, so a match here skips that. Only a salted hash of the login is kept, in memory. The user's
// stored password hash is part of it, so a changed password no longer matches
class BasicAuthCache {
  public:
    static constexpr uint32_t DURATION_MS = 5 * 60 * 1000;
    static constexpr size_t   MAX_ENTRIES = 4;

    BasicAuthCache() {
        PasswordHash::random_bytes(salt_.data(), salt_.size());
    }

    bool find(const std::string & username, const std::string & password, const char * stored_hash, uint32_t now) {
        Fingerprint fp;
        fingerprint(username, password, stored_hash, fp);
        std::lock_guard<SpinLock> lock(lock_);
        for (auto & entry : entries_) {
            if (entry.used && now - entry.at < DURATION_MS && PasswordHash::equal(entry.fingerprint.data(), fp.data(), fp.size())) {
                return true;
            }
        }
        return false;
    }

    void add(const std::string & username, const std::string & password, const char * stored_hash, uint32_t now) {
        Fingerprint fp;
        fingerprint(username, password, stored_hash, fp);
        std::lock_guard<SpinLock> lock(lock_);
        Entry * slot = &entries_[0];
        for (auto & entry : entries_) {
            if (!entry.used || now - entry.at >= DURATION_MS) {
                slot = &entry;
                break;
            }
            if (now - entry.at > now - slot->at) {
                slot = &entry; // the oldest
            }
        }
        slot->fingerprint = fp;
        slot->at          = now;
        slot->used        = true;
    }

  private:
    using Fingerprint = std::array<uint8_t, PasswordHash::HASH_BYTES>;

    struct Entry {
        Fingerprint fingerprint{};
        uint32_t    at   = 0;
        bool        used = false;
    };

    void fingerprint(const std::string & username, const std::string & password, const char * stored_hash, Fingerprint & out) const {
        const std::string login = username + ':' + password + '\n' + (stored_hash ? stored_hash : "");
        PasswordHash::pbkdf2(String(login.c_str()), salt_.data(), salt_.size(), 1, out.data());
    }

    std::array<uint8_t, PasswordHash::SALT_BYTES> salt_{};
    std::array<Entry, MAX_ENTRIES>                 entries_{};
    SpinLock                                       lock_;
};

#endif
