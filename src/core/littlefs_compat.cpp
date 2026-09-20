/*
 * EMS-ESP - https://github.com/emsesp/EMS-ESP
 * Copyright 2020-2025  emsesp.org
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

// A LittleFS partition written by an Arduino Core 2 build will not mount under the
// esp_littlefs that ships with Core 3, even though every byte of it is readable. Two
// fields of the superblock are compatibility gates rather than real format differences,
// and lfs_mount() rejects the filesystem on either one:
//
//   name_max  Core 2 used the upstream default of 255, while esp_littlefs derives 63 from
//             CONFIG_LITTLEFS_OBJ_NAME_LEN and refuses anything larger. Storing 0 means
//             "use the library's own limit", which is what a Core 3 format writes.
//   version   Core 2 recorded v2.1 and this build writes v2.0, and a higher minor is
//             refused. The library here understands everything a v2.1 image holds, so
//             lowering the field is enough.
//
// Rewriting them means walking the metadata block by hand. A block holds a revision count
// followed by a chain of commits, each one a run of tags terminated by a CCRC tag and a
// CRC covering everything that commit wrote. Tags are stored big endian and XORed with the
// preceding tag, so the chain has to be replayed from the start to find anything. The
// superblock struct is not at a fixed offset either: on a filesystem that has been written
// to, directory entries are committed ahead of it.

#ifndef EMSESP_STANDALONE

#include "littlefs_compat.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <esp_partition.h>
#include <sdkconfig.h>

namespace emsesp {

namespace {

#ifndef CONFIG_LITTLEFS_OBJ_NAME_LEN
#define CONFIG_LITTLEFS_OBJ_NAME_LEN 64
#endif
constexpr uint32_t NAME_MAX_SUPPORTED  = CONFIG_LITTLEFS_OBJ_NAME_LEN - 1;
constexpr uint32_t DISK_VERSION_TARGET = 0x00020000;

// littlefs block size on the ESP32, which is the flash sector size
constexpr uint32_t BLOCK_SIZE = 4096;

// byte offsets within lfs_superblock_t, all little endian uint32_t
constexpr uint32_t SB_VERSION    = 0;
constexpr uint32_t SB_BLOCK_SIZE = 4;
constexpr uint32_t SB_NAME_MAX   = 12;
constexpr uint32_t SB_SIZE       = 24;

// tag accessors, mirroring lfs.c
constexpr uint32_t TYPE_INLINESTRUCT = 0x201;
constexpr uint32_t TYPE_CCRC         = 0x500;

constexpr bool tag_isvalid(uint32_t t) {
    return (t & 0x80000000) == 0;
}
constexpr uint32_t tag_type2(uint32_t t) {
    return (t & 0x78000000) >> 20;
}
constexpr uint32_t tag_type3(uint32_t t) {
    return (t & 0x7ff00000) >> 20;
}
constexpr uint32_t tag_chunk(uint32_t t) {
    return (t & 0x0ff00000) >> 20;
}
constexpr uint32_t tag_id(uint32_t t) {
    return (t & 0x000ffc00) >> 10;
}
constexpr uint32_t tag_size(uint32_t t) {
    return t & 0x3ff;
}
// a size of 0x3ff marks a delete tag, which carries no payload
constexpr uint32_t tag_dsize(uint32_t t) {
    return 4 + (tag_size(t) == 0x3ff ? 0 : tag_size(t));
}

uint32_t rd_be32(const uint8_t * p) {
    return (static_cast<uint32_t>(p[0]) << 24) | (static_cast<uint32_t>(p[1]) << 16) | (static_cast<uint32_t>(p[2]) << 8) | p[3];
}
uint32_t rd_le32(const uint8_t * p) {
    return (static_cast<uint32_t>(p[3]) << 24) | (static_cast<uint32_t>(p[2]) << 16) | (static_cast<uint32_t>(p[1]) << 8) | p[0];
}
void wr_le32(uint8_t * p, uint32_t v) {
    p[0] = static_cast<uint8_t>(v);
    p[1] = static_cast<uint8_t>(v >> 8);
    p[2] = static_cast<uint8_t>(v >> 16);
    p[3] = static_cast<uint8_t>(v >> 24);
}

// the CRC-32 littlefs commits with: reflected 0x04C11DB7, seeded 0xffffffff, no final xor
uint32_t lfs_crc(uint32_t crc, const void * buffer, size_t size) {
    static const uint32_t rtable[16] = {
        0x00000000,
        0x1db71064,
        0x3b6e20c8,
        0x26d930ac,
        0x76dc4190,
        0x6b6b51f4,
        0x4db26158,
        0x5005713c,
        0xedb88320,
        0xf00f9344,
        0xd6d6a3e8,
        0xcb61b38c,
        0x9b64c2b0,
        0x86d3d2d4,
        0xa00ae278,
        0xbdbdf21c,
    };
    const uint8_t * data = static_cast<const uint8_t *>(buffer);
    for (size_t i = 0; i < size; i++) {
        crc = (crc >> 4) ^ rtable[(crc ^ data[i]) & 0xf];
        crc = (crc >> 4) ^ rtable[(crc ^ (data[i] >> 4)) & 0xf];
    }
    return crc;
}

// where the superblock lives, and which bytes the CRC of its commit covers
struct Located {
    bool     found        = false;
    uint32_t struct_off   = 0; // start of the 24 byte lfs_superblock_t
    uint32_t commit_start = 0; // first byte the commit's CRC covers
    uint32_t crc_off      = 0; // where that CRC is stored
};

// Replays a metadata block's commit chain the same way lfs_dir_fetchmatch() does, keeping
// the last superblock struct that belongs to a commit whose CRC verifies. Reporting
// nothing until a stored CRC matches means a successful scan also proves we are reading
// the tag stream correctly, so we never write back a block we misunderstood.
Located locate_superblock(const uint8_t * buf) {
    Located  found;
    uint32_t off          = 0;
    uint32_t ptag         = 0xffffffff;
    uint32_t commit_start = 0;
    uint32_t crc          = lfs_crc(0xffffffff, buf, 4); // the revision count leads the first commit
    bool     pending      = false;
    uint32_t pending_off  = 0;

    while (true) {
        off += tag_dsize(ptag);
        if (off + 4 > BLOCK_SIZE) {
            break;
        }

        crc                = lfs_crc(crc, &buf[off], 4);
        const uint32_t tag = rd_be32(&buf[off]) ^ ptag;

        if (!tag_isvalid(tag)) {
            break; // nothing committed beyond this point
        }
        if (off + tag_dsize(tag) > BLOCK_SIZE) {
            break;
        }
        ptag = tag;

        if (tag_type2(tag) == TYPE_CCRC) {
            if (crc != rd_le32(&buf[off + 4])) {
                break; // commit never completed, so the chain ends here
            }
            if (pending) {
                found.found        = true;
                found.struct_off   = pending_off;
                found.commit_start = commit_start;
                found.crc_off      = off + 4;
                pending            = false;
            }
            ptag ^= (tag_chunk(tag) & 1u) << 31;
            crc          = 0xffffffff;
            commit_start = off + tag_dsize(tag); // the padding after the CRC is not covered
            continue;
        }

        crc = lfs_crc(crc, &buf[off + 4], tag_dsize(tag) - 4);

        if (tag_type3(tag) == TYPE_INLINESTRUCT && tag_id(tag) == 0 && tag_size(tag) == SB_SIZE) {
            pending     = true;
            pending_off = off + 4;
        }
    }

    return found;
}

// resolve the same partition esp_littlefs would pick
const esp_partition_t * find_partition(const char * partition_label) {
    if (partition_label != nullptr) {
        return esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, partition_label);
    }
    // 0x83 is the littlefs subtype, 0x82 the spiffs one that older partition tables use
    const esp_partition_t * part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, static_cast<esp_partition_subtype_t>(0x83), nullptr);
    if (part != nullptr) {
        return part;
    }
    return esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, nullptr);
}

bool patch_block(const esp_partition_t * part, uint32_t block, uint8_t * buf) {
    const uint32_t addr = block * BLOCK_SIZE;

    if (esp_partition_read(part, addr, buf, BLOCK_SIZE) != ESP_OK) {
        return false;
    }

    const Located sb = locate_superblock(buf);
    if (!sb.found) {
        return false;
    }

    uint8_t * s = buf + sb.struct_off;
    if (rd_le32(s + SB_BLOCK_SIZE) != BLOCK_SIZE) {
        return false; // geometry we did not expect means we are not reading a superblock
    }

    bool changed = false;

    if (rd_le32(s + SB_NAME_MAX) > NAME_MAX_SUPPORTED) {
        wr_le32(s + SB_NAME_MAX, 0);
        changed = true;
    }

    const uint32_t version = rd_le32(s + SB_VERSION);
    if (version > DISK_VERSION_TARGET && (version >> 16) == (DISK_VERSION_TARGET >> 16)) {
        wr_le32(s + SB_VERSION, DISK_VERSION_TARGET);
        changed = true;
    }

    if (!changed) {
        return false;
    }

    wr_le32(buf + sb.crc_off, lfs_crc(0xffffffff, buf + sb.commit_start, sb.crc_off - sb.commit_start));

    return esp_partition_erase_range(part, addr, BLOCK_SIZE) == ESP_OK && esp_partition_write(part, addr, buf, BLOCK_SIZE) == ESP_OK;
}

} // namespace

bool littlefs_migrate(const char * partition_label) {
    const esp_partition_t * part = find_partition(partition_label);
    if (part == nullptr) {
        return false;
    }

    uint8_t * buf = static_cast<uint8_t *>(malloc(BLOCK_SIZE));
    if (buf == nullptr) {
        return false;
    }

    // Blocks 0 and 1 are the root metadata pair. Rewriting them one at a time keeps one
    // readable copy available for littlefs to fall back on if we lose power mid-write.
    bool patched = false;
    for (uint32_t block = 0; block < 2; block++) {
        if (patch_block(part, block, buf)) {
            patched = true;
        }
    }

    free(buf);
    return patched;
}

} // namespace emsesp

#endif
