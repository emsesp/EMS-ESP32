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

#ifndef EMSESP_PASSWORD_HASH_H
#define EMSESP_PASSWORD_HASH_H

#include <Arduino.h>

#include <array>
#include <cstdint>
#include <cstring>
#include <string>

#ifdef EMSESP_STANDALONE
#include <PortableSha256.h>
#include <random>
#else
#include <esp_random.h>
#include <mbedtls/pkcs5.h>
#endif

// stored as $pbkdf2-sha256$<iterations>$<salt hex>$<hash hex>
class PasswordHash {
  public:
    static constexpr uint32_t ITERATIONS     = 10000;
    static constexpr uint32_t MAX_ITERATIONS = 100000; // a stored hash can't make a sign-in take minutes
    static constexpr size_t   SALT_BYTES     = 16;
    static constexpr size_t   HASH_BYTES     = 32;

    static String hash(const String & password) {
        std::array<uint8_t, SALT_BYTES> salt{};
        random_bytes(salt.data(), salt.size());
        return hash(password, salt.data(), ITERATIONS);
    }

    static String hash(const String & password, const uint8_t * salt, uint32_t iterations) {
        std::array<uint8_t, HASH_BYTES> derived{};
        pbkdf2(password, salt, SALT_BYTES, iterations, derived.data());
        std::string out = PREFIX;
        out += std::to_string(iterations);
        out += '$';
        out += to_hex(salt, SALT_BYTES);
        out += '$';
        out += to_hex(derived.data(), HASH_BYTES);
        return String(out.c_str());
    }

    static bool is_hashed(const String & stored) {
        uint32_t                        iterations;
        std::array<uint8_t, SALT_BYTES> salt;
        std::array<uint8_t, HASH_BYTES> expected;
        return parse(stored.c_str(), iterations, salt.data(), expected.data());
    }

    // an empty stored password never matches. Plain text is still accepted until it's been hashed at boot
    static bool verify(const String & password, const String & stored) {
        if (stored.isEmpty()) {
            return false;
        }

        uint32_t                        iterations;
        std::array<uint8_t, SALT_BYTES> salt;
        std::array<uint8_t, HASH_BYTES> expected;
        if (!parse(stored.c_str(), iterations, salt.data(), expected.data())) {
            return password.length() == stored.length()
                   && equal(reinterpret_cast<const uint8_t *>(password.c_str()), reinterpret_cast<const uint8_t *>(stored.c_str()), stored.length());
        }

        std::array<uint8_t, HASH_BYTES> derived{};
        pbkdf2(password, salt.data(), salt.size(), iterations, derived.data());
        return equal(derived.data(), expected.data(), HASH_BYTES);
    }

    static void pbkdf2(const String & password, const uint8_t * salt, size_t salt_len, uint32_t iterations, uint8_t * out) {
#ifdef EMSESP_STANDALONE
        Hmac base(reinterpret_cast<const uint8_t *>(password.c_str()), password.length());

        std::array<uint8_t, 32> u{};
        Hmac                    first = base;
        const uint8_t           block[4] = {0, 0, 0, 1}; // a single block, the output is one SHA-256 long
        first.update(salt, salt_len);
        first.update(block, sizeof(block));
        first.finish(u.data());
        memcpy(out, u.data(), HASH_BYTES);

        for (uint32_t i = 1; i < iterations; i++) {
            Hmac next = base;
            next.update(u.data(), u.size());
            next.finish(u.data());
            for (size_t j = 0; j < HASH_BYTES; j++) {
                out[j] ^= u[j];
            }
        }
#else
        mbedtls_pkcs5_pbkdf2_hmac_ext(MBEDTLS_MD_SHA256,
                                      reinterpret_cast<const unsigned char *>(password.c_str()),
                                      password.length(),
                                      salt,
                                      salt_len,
                                      iterations,
                                      HASH_BYTES,
                                      out);
#endif
    }

    // constant time, so the comparison doesn't reveal how many bytes matched
    static bool equal(const uint8_t * a, const uint8_t * b, size_t len) {
        uint8_t diff = 0;
        for (size_t i = 0; i < len; i++) {
            diff |= a[i] ^ b[i];
        }
        return diff == 0;
    }

    static std::string to_hex(const uint8_t * data, size_t len) {
        static constexpr char digits[] = "0123456789abcdef";
        std::string           hex;
        hex.reserve(len * 2);
        for (size_t i = 0; i < len; i++) {
            hex += digits[data[i] >> 4];
            hex += digits[data[i] & 0x0F];
        }
        return hex;
    }

    static void random_bytes(uint8_t * data, size_t len) {
#ifdef EMSESP_STANDALONE
        static std::random_device rd;
        for (size_t i = 0; i < len; i++) {
            data[i] = rd() & 0xFF;
        }
#else
        esp_fill_random(data, len);
#endif
    }

  private:
    static constexpr const char * PREFIX = "$pbkdf2-sha256$";

    static bool parse(const char * stored, uint32_t & iterations, uint8_t * salt, uint8_t * expected) {
        const size_t prefix_len = strlen(PREFIX);
        if (strncmp(stored, PREFIX, prefix_len)) {
            return false;
        }

        const char * p = stored + prefix_len;
        iterations     = 0;
        size_t digits  = 0;
        for (; *p >= '0' && *p <= '9'; p++, digits++) {
            if (digits == 6) {
                return false;
            }
            iterations = iterations * 10 + (*p - '0');
        }
        if (*p != '$' || iterations == 0 || iterations > MAX_ITERATIONS) {
            return false;
        }

        const char * salt_hex = p + 1;
        const char * hash_hex = salt_hex + SALT_BYTES * 2 + 1;
        if (strlen(salt_hex) != SALT_BYTES * 2 + 1 + HASH_BYTES * 2 || hash_hex[-1] != '$') {
            return false;
        }
        return from_hex(salt_hex, salt, SALT_BYTES) && from_hex(hash_hex, expected, HASH_BYTES);
    }

    static bool from_hex(const char * hex, uint8_t * out, size_t len) {
        for (size_t i = 0; i < len; i++) {
            int hi = nibble(hex[i * 2]);
            int lo = nibble(hex[i * 2 + 1]);
            if (hi < 0 || lo < 0) {
                return false;
            }
            out[i] = (hi << 4) | lo;
        }
        return true;
    }

    static int nibble(char c) {
        if (c >= '0' && c <= '9') {
            return c - '0';
        }
        if (c >= 'a' && c <= 'f') {
            return c - 'a' + 10;
        }
        return -1;
    }

#ifdef EMSESP_STANDALONE
    // HMAC for the native build, which has no mbedtls
    using Sha256 = PortableSha256;

    class Hmac {
      public:
        Hmac(const uint8_t * key, size_t key_len) {
            uint8_t block[64]{};
            if (key_len > 64) {
                Sha256 kh;
                kh.update(key, key_len);
                kh.finish(block);
            } else {
                memcpy(block, key, key_len);
            }
            uint8_t ipad[64], opad[64];
            for (int i = 0; i < 64; i++) {
                ipad[i] = block[i] ^ 0x36;
                opad[i] = block[i] ^ 0x5c;
            }
            inner_.update(ipad, 64);
            outer_.update(opad, 64);
        }

        void update(const uint8_t * data, size_t len) {
            inner_.update(data, len);
        }

        void finish(uint8_t * out) {
            uint8_t inner_hash[32];
            inner_.finish(inner_hash);
            outer_.update(inner_hash, sizeof(inner_hash));
            outer_.finish(out);
        }

      private:
        Sha256 inner_;
        Sha256 outer_;
    };
#endif
};

#endif
