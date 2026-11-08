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

#ifndef firmwareVersion_H
#define firmwareVersion_H

#include <string>

namespace emsesp {

class FirmwareVersion {
  public:
    FirmwareVersion() = default;

    // Construct from a version string like "3.9.0-dev.14" or "3.9.0".
    // Anything past a '-' or '+' is kept as the prerelease string and not interpreted.
    explicit FirmwareVersion(const std::string & s);
    explicit FirmwareVersion(const char * s);

    int                 major() const;
    int                 minor() const;
    int                 patch() const;
    const std::string & prerelease() const;

    friend bool operator<(const FirmwareVersion & a, const FirmwareVersion & b);
    friend bool operator>(const FirmwareVersion & a, const FirmwareVersion & b);
    friend bool operator==(const FirmwareVersion & a, const FirmwareVersion & b);
    friend bool operator!=(const FirmwareVersion & a, const FirmwareVersion & b);
    friend bool operator>=(const FirmwareVersion & a, const FirmwareVersion & b);
    friend bool operator<=(const FirmwareVersion & a, const FirmwareVersion & b);

  private:
    int         major_ = 0;
    int         minor_ = 0;
    int         patch_ = 0;
    std::string prerelease_;

    void parse(const char * s);
};

} // namespace emsesp

#endif
