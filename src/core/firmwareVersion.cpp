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

#include "firmwareVersion.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace emsesp {

FirmwareVersion::FirmwareVersion(const std::string & s) {
    parse(s.c_str());
}

FirmwareVersion::FirmwareVersion(const char * s) {
    parse(s ? s : "");
}

int FirmwareVersion::major() const {
    return major_;
}

int FirmwareVersion::minor() const {
    return minor_;
}

int FirmwareVersion::patch() const {
    return patch_;
}

const std::string & FirmwareVersion::prerelease() const {
    return prerelease_;
}

// semver prerelease ordering: a release (empty tag) ranks higher than any prerelease,
// and dot-separated numeric identifiers are compared numerically (so dev.9 < dev.12).
// returns <0, 0 or >0
static int compare_prerelease(const std::string & a, const std::string & b) {
    if (a == b) {
        return 0;
    }

    if (a.empty()) {
        return 1; // release > prerelease
    }

    if (b.empty()) {
        return -1;
    }

    size_t ia = 0;
    size_t ib = 0;
    while (ia < a.size() && ib < b.size()) {
        size_t ea = a.find('.', ia);
        size_t eb = b.find('.', ib);
        if (ea == std::string::npos) {
            ea = a.size();
        }

        if (eb == std::string::npos) {
            eb = b.size();
        }

        std::string id_a = a.substr(ia, ea - ia);
        std::string id_b = b.substr(ib, eb - ib);

        bool num_a = !id_a.empty() && id_a.find_first_not_of("0123456789") == std::string::npos;
        bool num_b = !id_b.empty() && id_b.find_first_not_of("0123456789") == std::string::npos;

        if (num_a && num_b) {
            long va = atol(id_a.c_str());
            long vb = atol(id_b.c_str());
            if (va != vb) {
                return (va < vb) ? -1 : 1;
            }
        } else if (num_a != num_b) {
            return num_a ? -1 : 1; // numeric identifiers rank lower than alphanumeric ones
        } else {
            int cmp = id_a.compare(id_b);
            if (cmp != 0) {
                return (cmp < 0) ? -1 : 1;
            }
        }

        ia = ea + 1;
        ib = eb + 1;
    }

    // all shared identifiers are equal; the one with more identifiers ranks higher
    if (ia < a.size()) {
        return 1;
    }

    if (ib < b.size()) {
        return -1;
    }

    return 0;
}

bool operator<(const FirmwareVersion & a, const FirmwareVersion & b) {
    if (a.major_ != b.major_)
        return a.major_ < b.major_;
    if (a.minor_ != b.minor_)
        return a.minor_ < b.minor_;
    if (a.patch_ != b.patch_)
        return a.patch_ < b.patch_;
    return compare_prerelease(a.prerelease_, b.prerelease_) < 0;
}

bool operator>(const FirmwareVersion & a, const FirmwareVersion & b) {
    return b < a;
}

bool operator==(const FirmwareVersion & a, const FirmwareVersion & b) {
    return a.major_ == b.major_ && a.minor_ == b.minor_ && a.patch_ == b.patch_ && compare_prerelease(a.prerelease_, b.prerelease_) == 0;
}

bool operator!=(const FirmwareVersion & a, const FirmwareVersion & b) {
    return !(a == b);
}

bool operator>=(const FirmwareVersion & a, const FirmwareVersion & b) {
    return !(a < b);
}

bool operator<=(const FirmwareVersion & a, const FirmwareVersion & b) {
    return !(b < a);
}

void FirmwareVersion::parse(const char * s) {
    major_ = minor_ = patch_ = 0;
    prerelease_.clear();
    if (s == nullptr || *s == '\0') {
        return;
    }

    // parse numeric major.minor.patch; accept partial ("3", "3.9", "3.9.0")
    sscanf(s, "%d.%d.%d", &major_, &minor_, &patch_);
    // capture prerelease tag after '-' if present (stop at '+' which is build metadata)
    const char * dash = strchr(s, '-');
    if (dash != nullptr) {
        const char * plus = strchr(dash, '+');
        if (plus != nullptr) {
            prerelease_.assign(dash + 1, plus - dash - 1);
        } else {
            prerelease_.assign(dash + 1);
        }
    }
}

} // namespace emsesp
