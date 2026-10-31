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
    // SHA-256 and HMAC for the native build, which has no mbedtls
    class Sha256 {
      public:
        Sha256() {
            static constexpr uint32_t init[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
            memcpy(state_, init, sizeof(state_));
        }

        void update(const uint8_t * data, size_t len) {
            for (size_t i = 0; i < len; i++) {
                buffer_[buffer_len_++] = data[i];
                if (buffer_len_ == 64) {
                    transform();
                    buffer_len_ = 0;
                }
            }
            total_ += len;
        }

        void finish(uint8_t * out) {
            const uint64_t bits = total_ * 8;
            const uint8_t  pad  = 0x80;
            update(&pad, 1);
            const uint8_t zero = 0;
            while (buffer_len_ != 56) {
                update(&zero, 1);
            }
            for (int i = 7; i >= 0; i--) {
                const uint8_t b = (bits >> (i * 8)) & 0xFF;
                update(&b, 1);
            }
            for (int i = 0; i < 8; i++) {
                out[i * 4]     = state_[i] >> 24;
                out[i * 4 + 1] = state_[i] >> 16;
                out[i * 4 + 2] = state_[i] >> 8;
                out[i * 4 + 3] = state_[i];
            }
        }

      private:
        uint32_t state_[8];
        uint8_t  buffer_[64]{};
        size_t   buffer_len_ = 0;
        uint64_t total_      = 0;

        static uint32_t rotr(uint32_t x, int n) {
            return (x >> n) | (x << (32 - n));
        }

        void transform() {
            static constexpr uint32_t k[64] = {
                0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be,
                0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa,
                0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85,
                0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
                0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f,
                0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

            uint32_t w[64];
            for (int i = 0; i < 16; i++) {
                w[i] = (uint32_t)buffer_[i * 4] << 24 | (uint32_t)buffer_[i * 4 + 1] << 16 | (uint32_t)buffer_[i * 4 + 2] << 8 | buffer_[i * 4 + 3];
            }
            for (int i = 16; i < 64; i++) {
                const uint32_t s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
                const uint32_t s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
                w[i]              = w[i - 16] + s0 + w[i - 7] + s1;
            }

            uint32_t a = state_[0], b = state_[1], c = state_[2], d = state_[3], e = state_[4], f = state_[5], g = state_[6], h = state_[7];
            for (int i = 0; i < 64; i++) {
                const uint32_t t1 = h + (rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25)) + ((e & f) ^ (~e & g)) + k[i] + w[i];
                const uint32_t t2 = (rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22)) + ((a & b) ^ (a & c) ^ (b & c));
                h                 = g;
                g                 = f;
                f                 = e;
                e                 = d + t1;
                d                 = c;
                c                 = b;
                b                 = a;
                a                 = t1 + t2;
            }
            state_[0] += a;
            state_[1] += b;
            state_[2] += c;
            state_[3] += d;
            state_[4] += e;
            state_[5] += f;
            state_[6] += g;
            state_[7] += h;
        }
    };

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
