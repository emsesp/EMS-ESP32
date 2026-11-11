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

#ifndef EMSESP_MQTT_TLS_H
#define EMSESP_MQTT_TLS_H

#include <cstddef>
#include <cstdint>
#include <cstring>

// How MQTT over TLS checks the broker. The MQTT setting rootCA holds one of:
//   "insecure" (or empty)  encrypted, but the broker isn't checked
//   a SHA-1 fingerprint    the broker's own certificate, 40 hex digits, may be in pairs separated by ':'
//   anything else          a CA certificate, base64 without the PEM header lines
namespace MqttTls {

enum class Check : uint8_t { NONE, FINGERPRINT, CA };

constexpr size_t FINGERPRINT_LEN = 40;

// writes the 40 hex digits in lower case to out, which needs FINGERPRINT_LEN + 1 bytes
inline bool parse_fingerprint(const char * s, char * out = nullptr) {
    size_t n = 0;
    for (; *s; s++) {
        char c = *s;
        if (c == ':') {
            if (n == 0 || n % 2 || s[1] == '\0' || s[1] == ':') {
                return false; // a separator only goes between pairs
            }
            continue;
        }
        if (c >= 'A' && c <= 'F') {
            c = static_cast<char>(c - 'A' + 'a');
        } else if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))) {
            return false;
        }
        if (n == FINGERPRINT_LEN) {
            return false;
        }
        if (out) {
            out[n] = c;
        }
        n++;
    }
    if (n != FINGERPRINT_LEN) {
        return false;
    }
    if (out) {
        out[n] = '\0';
    }
    return true;
}

inline Check check(const char * rootCA) {
    if (rootCA[0] == '\0' || strcmp(rootCA, "insecure") == 0) {
        return Check::NONE;
    }
    return parse_fingerprint(rootCA) ? Check::FINGERPRINT : Check::CA;
}

} // namespace MqttTls

#endif
