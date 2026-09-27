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

#ifndef EMSESP_LITTLEFS_COMPAT_H_
#define EMSESP_LITTLEFS_COMPAT_H_

namespace emsesp {

// Rewrites the superblock of a LittleFS partition created by an Arduino Core 2 build so
// that this build's littlefs will mount it. Only call once a mount has already failed.
// Returns true if flash was changed, meaning another mount attempt is worth making.
bool littlefs_migrate(const char * partition_label = nullptr);

} // namespace emsesp

#endif
