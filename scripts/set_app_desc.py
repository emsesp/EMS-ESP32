# Writes the project name (custom_fw_name) and version (custom_version_file/custom_version_define) into the
# esp_app_desc_t of firmware.bin, which the prebuilt Arduino libraries leave empty. The application can then
# identify the image with esp_ota_get_partition_description(), or from the first bytes of an upload.
# Must run before rename_fw.py so the renamed copy and its MD5 include the change.

import hashlib
import re
import struct
from pathlib import Path

Import("env")

IMAGE_HEADER_LEN = 24
SEGMENT_HEADER_LEN = 8
APP_DESC_OFFSET = IMAGE_HEADER_LEN + SEGMENT_HEADER_LEN
APP_DESC_MAGIC = 0xABCD5432
HASH_APPENDED_OFFSET = 23


def read_version(env):
    version_file = Path(env.GetProjectOption("custom_version_file", "./src/emsesp_version.h"))
    version_define = env.GetProjectOption("custom_version_define", "EMSESP_APP_VERSION")
    pattern = re.compile(rf'^#define {version_define}\s+"(\S+)"')
    for line in version_file.read_text().splitlines():
        match = pattern.match(line)
        if match:
            return match.group(1)
    raise ValueError(f"{version_define} not found in {version_file}")


def put_string(image, offset, value):
    encoded = value.encode()[:31]
    image[offset:offset + 32] = encoded + b"\0" * (32 - len(encoded))


def set_app_desc(source, target, env):
    path = Path(target[0].get_abspath())
    image = bytearray(path.read_bytes())

    (magic,) = struct.unpack_from("<I", image, APP_DESC_OFFSET)
    if magic != APP_DESC_MAGIC:
        print(f"Error: no app description found in {path}")
        env.Exit(1)

    name = env.GetProjectOption("custom_fw_name", "EMS-ESP")
    version = read_version(env)
    put_string(image, APP_DESC_OFFSET + 16, version)
    put_string(image, APP_DESC_OFFSET + 48, name)

    # the description lives in the first segment, so the image checksum and SHA256 need updating
    checksum = 0xEF
    pos = IMAGE_HEADER_LEN
    for _ in range(image[1]):
        _, length = struct.unpack_from("<II", image, pos)
        pos += SEGMENT_HEADER_LEN
        for b in image[pos:pos + length]:
            checksum ^= b
        pos += length
    pos += 15 - (pos % 16)
    image[pos] = checksum
    pos += 1
    if image[HASH_APPENDED_OFFSET] == 1:
        image[pos:pos + 32] = hashlib.sha256(image[:pos]).digest()

    path.write_bytes(image)
    print(f"App description set to {name} {version}")


env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", set_app_desc)
