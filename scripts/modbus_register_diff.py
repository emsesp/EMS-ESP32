#
# Lists the Modbus register changes between two versions of src/core/modbus_entity_parameters.hpp as markdown,
# for the changelog. Each side is a file path or a git ref (tag, branch or commit).
#
#   python3 ./scripts/modbus_register_diff.py v3.9.1 src/core/modbus_entity_parameters.hpp
#
# Exits with code 1 when --fail-on-breaking is given and a register was moved or retired.

import argparse
import os
import sys

from modbus_register_map import read_header_file, read_header_at_ref

TAG_TYPE_NAMES = {
    "TAG_TYPE_DEVICE_DATA": "DEVICE_DATA",
    "TAG_TYPE_HC": "HC",
    "TAG_TYPE_DHW": "DHW",
    "TAG_TYPE_AHS": "AHS",
    "TAG_TYPE_HS": "HS",
    "TAG_TYPE_SRC": "SRC",
}


def load(source):
    if os.path.isfile(source):
        return read_header_file(source)
    return read_header_at_ref(source)


def row(key, names, *columns):
    device_type, tag_type, fl_name = key
    name = names.get(key, fl_name.lower())
    return "| " + " | ".join([device_type.lower(), TAG_TYPE_NAMES.get(tag_type, tag_type), name, *map(str, columns)]) + " |"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("old", help="file or git ref of the previous release")
    parser.add_argument("new", help="file or git ref of the new release")
    parser.add_argument("--fail-on-breaking", action="store_true")
    args = parser.parse_args()

    old = load(args.old)
    new = load(args.new)
    names = {**old.names, **new.names}

    def by_offset(item):
        (device_type, tag_type, _), (offset, _) = item
        return (device_type, list(TAG_TYPE_NAMES).index(tag_type) if tag_type in TAG_TYPE_NAMES else 99, offset)

    moved = []
    retired = []
    added = []
    for key, (offset, count) in sorted(old.active.items(), key=by_offset):
        if key not in new.active:
            retired.append(row(key, names, offset, count))
        elif new.active[key] != (offset, count):
            new_offset, new_count = new.active[key]
            moved.append(row(key, names, f"{offset} ({count})", f"{new_offset} ({new_count})"))
    for key, (offset, count) in sorted(new.active.items(), key=by_offset):
        if key not in old.active:
            added.append(row(key, names, offset, count))

    print(f"### Modbus register changes {args.old} → {args.new}")
    print()
    print(f"Register map version: {old.version or 1} → {new.version or 1}")
    print()
    if not (moved or retired or added):
        print("No changes.")
    if moved:
        print("**Moved registers (breaking)**, the old offset now returns an error:")
        print()
        print("| device type | tag type | entity | old offset (count) | new offset (count) |")
        print("|-|-|-|-|-|")
        print("\n".join(moved))
        print()
    if retired:
        print("**Removed registers (breaking)**, these offsets now return an error:")
        print()
        print("| device type | tag type | entity | offset | count |")
        print("|-|-|-|-|-|")
        print("\n".join(retired))
        print()
    if added:
        print("**New registers:**")
        print()
        print("| device type | tag type | entity | offset | count |")
        print("|-|-|-|-|-|")
        print("\n".join(added))
        print()

    if args.fail_on_breaking and (moved or retired):
        sys.exit(1)


if __name__ == "__main__":
    main()
