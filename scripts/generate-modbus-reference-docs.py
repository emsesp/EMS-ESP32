#!/usr/bin/env python3
#
# Generates the Modbus reference pages used by https://github.com/emsesp/docs:
#   docs/Modbus-Server-IDs.md
#   docs/Modbus-Register-Blocks.md
#   docs/Modbus-System-Server.md
#
# Device types and tags are taken from the C++ enums. The system server registers
# are listed here and must stay in sync with Modbus::handleSystemRead().
#
# Run from the repository root, or via the build_standalone target:
#   python3 ./scripts/generate-modbus-reference-docs.py
#

import argparse
import re
from pathlib import Path

from modbus_register_map import HEADER_PATH, read_header_file

REPO_ROOT = Path(__file__).resolve().parents[1]
DOCS_DIR = REPO_ROOT / "docs"
EMSDEVICE_H = REPO_ROOT / "src" / "core" / "emsdevice.h"
EMSDEVICEVALUE_H = REPO_ROOT / "src" / "core" / "emsdevicevalue.h"
MODBUS_H = REPO_ROOT / "src" / "core" / "modbus.h"

# Unit ID used by Modbus::start() for the system server. Not EMSdevice::DeviceType::SYSTEM (0).
SYSTEM_UNIT_ID = 1

# Device types that have no Modbus server of their own (internal EMS-ESP features).
INTERNAL_DEVICE_TYPES = {
    "TEMPERATURESENSOR",
    "ANALOGSENSOR",
    "SCHEDULER",
    "CUSTOM",
    "COMMAND",
    "UNKNOWN",
}

AUTOGEN_WARNING = """:::warning
This file has been auto-generated. Do not modify.
:::
"""


def parse_enum_members(header_path, enum_name):
    """Return [(name, value), ...] for a C++ enum, following = and comma increments."""
    text = header_path.read_text(encoding="utf-8", errors="replace")
    # Require a real enum declaration (optional ": type"), not a comment that mentions the name.
    match = re.search(rf"enum\s+{re.escape(enum_name)}\s*(?::[^{{]+)?\{{(.*?)\}};", text, re.S)
    if match is None:
        raise ValueError(f"enum {enum_name} not found in {header_path}")

    members = []
    next_value = 0
    for line in match.group(1).splitlines():
        line = re.sub(r"//.*", "", line).strip().rstrip(",")
        if not line:
            continue
        if "=" in line:
            name, value = [part.strip() for part in line.split("=", 1)]
            next_value = int(value, 0)
        else:
            name = line.strip()
        members.append((name, next_value))
        next_value += 1
    return members


def register_block_size():
    text = MODBUS_H.read_text(encoding="utf-8", errors="replace")
    match = re.search(r"REGISTER_BLOCK_SIZE\s*=\s*(\d+)", text)
    if match is None:
        raise ValueError(f"REGISTER_BLOCK_SIZE not found in {MODBUS_H}")
    return int(match.group(1))


def register_map_version():
    header = REPO_ROOT / HEADER_PATH
    if not header.exists():
        return 1
    version = read_header_file(header).version
    return version if version is not None else 1


def write_markdown(path, body):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(body, encoding="utf-8")
    print(f"Generated {path} ({path.stat().st_size} bytes)")


def generate_server_ids():
    members = parse_enum_members(EMSDEVICE_H, "DeviceType")
    rows = [
        "| Device type | Unit Identifier |",
        "| ---------------- | --------------- |",
        f"| SYSTEM (EMS-ESP) | {SYSTEM_UNIT_ID} |",
    ]
    for name, value in members:
        if name == "SYSTEM" or name in INTERNAL_DEVICE_TYPES:
            continue
        rows.append(f"| {name} | {value} |")

    table = "\n".join(rows)
    return f"""---
id: Modbus-Server-IDs
title: Modbus Server IDs
description: Reference table of Modbus unit identifiers for EMS device types
---

# Modbus Server IDs

{AUTOGEN_WARNING}
This table shows how device types are mapped to server unit identifiers.

{table}
"""


def generate_register_blocks():
    block_size = register_block_size()
    members = parse_enum_members(EMSDEVICEVALUE_H, "DeviceValueTAG")
    rows = ["| `Tag` | `Register Offset` |", "| ----------- | --------------- |"]
    for name, value in members:
        if name == "TAG_NONE" or value < 0:
            continue
        tag = name[len("TAG_"):] if name.startswith("TAG_") else name
        rows.append(f"| {tag} | {value * block_size} |")

    table = "\n".join(rows)
    return f"""---
id: Modbus-Register-Blocks
title: Modbus Register Blocks
description: Entity tag to Modbus register block offset mapping reference
---
# Modbus Register Blocks

{AUTOGEN_WARNING}
This table shows how entity tags are mapped to Modbus register block offsets.

{table}
"""


def generate_system_server():
    version = register_map_version()
    return f"""---
id: Modbus-System-Server
title: Modbus System Server
description: EMS-ESP system information accessible via Modbus registers including device count and register map version
---
# Modbus System Server

{AUTOGEN_WARNING}
EMS-ESP provides a server with general read only information with unit ID `{SYSTEM_UNIT_ID}`.
It answers function code `04` (read input registers) and provides the following data:

| `Name` | `Description` | `Register offset` | `Register count` |
| ------------------ | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | --------------- | -------------- |
| NUM_DEVICES | The number of devices recognized on the EMS bus | 1 | 1 |
| REGISTER_MAP_VERSION | Version of the entity/register map. It starts at {version} and only increases when an existing register is moved or removed, not when new registers are added. A Modbus client can read this once and stop if the value is not the one it was mapped against. | 2 | 1 |
| DEVICE_TYPE_AND_ID | The device type and ID for each device. "n" in the register offset is the index of the device with 0 <= n < NUM_DEVICES. The first byte of the 16 bit value contains the [device type](Modbus-Server-IDs.md), the second byte contains the device ID. | 1000 + n | 1 |
"""


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--output-dir", type=Path, default=DOCS_DIR)
    args = parser.parse_args()

    write_markdown(args.output_dir / "Modbus-Server-IDs.md", generate_server_ids())
    write_markdown(args.output_dir / "Modbus-Register-Blocks.md", generate_register_blocks())
    write_markdown(args.output_dir / "Modbus-System-Server.md", generate_system_server())


if __name__ == "__main__":
    main()
