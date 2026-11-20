from pathlib import Path
import os
import shutil
import subprocess
import sys
Import("env")

# This script is used to create a dummy modbus_entity_parameters.hpp so the first pass compiles.
# It is used in the build_modbus target.
# Before that, the current register map is saved to PREVIOUS_MAP_PATH, so the post script can keep all
# existing register offsets.

sys.path.insert(0, os.path.join(env.subst("$PROJECT_DIR"), "scripts"))
from modbus_register_map import HEADER_PATH, parse_header_text

PREVIOUS_MAP_PATH = Path(".pio") / "modbus_entity_parameters.prev.hpp"


def save_previous_map():
    """Save the current register map. Never overwrite a saved map with the dummy header of an aborted run."""
    header_path = Path(HEADER_PATH)
    if header_path.exists() and parse_header_text(header_path.read_text(encoding='utf-8', errors='replace')).active:
        PREVIOUS_MAP_PATH.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(header_path, PREVIOUS_MAP_PATH)
        print(f"Saved current modbus register map to {PREVIOUS_MAP_PATH}")
        return

    if PREVIOUS_MAP_PATH.exists():
        print(f"{HEADER_PATH} has no register map, keeping the saved map {PREVIOUS_MAP_PATH}")
        return

    text = subprocess.run(["git", "show", f"HEAD:{HEADER_PATH}"], check=True, capture_output=True,
                          text=True, encoding='utf-8', errors='replace').stdout
    PREVIOUS_MAP_PATH.parent.mkdir(parents=True, exist_ok=True)
    PREVIOUS_MAP_PATH.write_text(text, encoding='utf-8')
    print(f"{HEADER_PATH} has no register map, saved the committed map to {PREVIOUS_MAP_PATH}")


def create_dummy_modbus_header():
    """Create a dummy modbus_entity_parameters.hpp so the first pass compiles."""
    header_content = '''#include "modbus.h"
#include "emsdevice.h"

/*
 * This file is auto-generated. Do not modify.
*/

// clang-format off

namespace emsesp {

using dt = EMSdevice::DeviceType;

#define REGISTER_MAPPING(device_type, device_value_tag_type, long_name, modbus_register_offset, modbus_register_count) \\
    { device_type, device_value_tag_type, long_name[0], modbus_register_offset, modbus_register_count }

const uint16_t Modbus::REGISTER_MAP_VERSION = 0;

// IMPORTANT: This list MUST be ordered by keys "device_type", "device_value_tag_type" and "modbus_register_offset" in this order.
const std::initializer_list<Modbus::EntityModbusInfo> Modbus::modbus_register_mappings = {};

} // namespace emsesp

// clang-format on
'''
    
    header_path = Path(HEADER_PATH)
    header_path.parent.mkdir(parents=True, exist_ok=True)
    
    with open(header_path, 'w') as f:
        f.write(header_content)
    
    print(f"Created dummy header file: {header_path} ({os.path.getsize(header_path)} bytes)")

if not (env.IsCleanTarget()):
    save_previous_map()
    create_dummy_modbus_header()
