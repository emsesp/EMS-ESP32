#!/usr/bin/env python3
#
# Builds a LittleFS image from the data/ folder without going through PlatformIO.
# Mirrors build_fs_image() in the pioarduino espressif32 platform builder, so the
# output is interchangeable with "platformio run --target buildfs".
#
# Needs littlefs-python, which is already installed in the PlatformIO venv:
#   ~/.platformio/penv/bin/python scripts/build_littlefs.py
#
# Defaults match the s_16M_P board (2MB spiffs partition, littlefs 2.0).

import argparse
from pathlib import Path

from littlefs import LittleFS


def auto_int(value):
    return int(value, 0)


def build(source_dir, target_file, fs_size, block_size, disk_version):
    fs = LittleFS(
        block_size=block_size,
        block_count=fs_size // block_size,
        read_size=1,
        prog_size=1,
        cache_size=block_size,
        lookahead_size=32,
        block_cycles=500,
        name_max=64,  # ESP-IDF default filename length
        disk_version=disk_version,
        mount=False,
    )

    # explicit format() + mount() on a fresh instance, see the platform builder
    # for why the constructor's auto-mount is avoided
    fs.format()
    fs.mount()

    for item in sorted(source_dir.rglob("*")):
        rel_path = item.relative_to(source_dir)
        fs_path = rel_path.as_posix()

        if item.is_dir():
            fs.makedirs(fs_path, exist_ok=True)
        else:
            if rel_path.parent != Path("."):
                fs.makedirs(rel_path.parent.as_posix(), exist_ok=True)
            with fs.open(fs_path, "wb") as dest:
                dest.write(item.read_bytes())
        print(f"  {fs_path}")

        # ESP-IDF compatible mtime attribute
        mtime = int(item.stat().st_mtime)
        fs.setattr(fs_path, "t", mtime.to_bytes(4, "little"))

    target_file.parent.mkdir(parents=True, exist_ok=True)
    target_file.write_bytes(fs.context.buffer)


def main():
    parser = argparse.ArgumentParser(description="Build a LittleFS image from a folder")
    parser.add_argument("source", nargs="?", default="data", type=Path)
    parser.add_argument("target", nargs="?", default=Path(".pio/build/s_16M_P/littlefs.bin"), type=Path)
    parser.add_argument("--size", default=0x200000, type=auto_int, help="partition size, default 0x200000")
    parser.add_argument("--block", default=0x1000, type=auto_int, help="block size, default 0x1000")
    parser.add_argument("--version", default="2.0", help="littlefs disk version, default 2.0")
    args = parser.parse_args()

    major, _, minor = args.version.partition(".")
    disk_version = (int(major) << 16) | int(minor or 0)

    print(f"Building {args.target} ({args.size // 1024} KB) from {args.source}/")
    build(args.source, args.target, args.size, args.block, disk_version)
    print(f"Wrote {args.target.stat().st_size} bytes")


if __name__ == "__main__":
    main()
