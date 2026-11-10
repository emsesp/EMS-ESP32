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

#ifndef EMSESP_FS_WIPE_H
#define EMSESP_FS_WIPE_H

#ifndef EMSESP_STANDALONE

#include <LittleFS.h>
#include <esp_partition.h>

// LittleFS.format() only writes a new superblock, the old settings files (passwords, API keys) stay readable in the
// flash. This erases every block of the filesystem partition. Only call it right before a restart: the filesystem is
// unmounted, and the next boot formats it.
inline bool fs_wipe() {
    const esp_partition_t * part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "spiffs");
    if (part == nullptr) {
        return false;
    }

    LittleFS.end();

    constexpr size_t CHUNK = 0x10000; // a 64KB block at a time, so other tasks and the watchdogs get a turn
    for (size_t offset = 0; offset < part->size; offset += CHUNK) {
        const size_t len = (part->size - offset < CHUNK) ? part->size - offset : CHUNK;
        if (esp_partition_erase_range(part, offset, len) != ESP_OK) {
            return false;
        }
        delay(1);
    }
    return true;
}

#endif

#endif
