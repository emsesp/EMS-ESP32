from pathlib import Path
import os
Import("env")

# This script is used to create a dummy modbus_entity_parameters.hpp so the first pass compiles.
# It is used in the build_modbus target.

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

// IMPORTANT: This list MUST be ordered by keys "device_type", "device_value_tag_type" and "modbus_register_offset" in this order.
const std::initializer_list<Modbus::EntityModbusInfo> Modbus::modbus_register_mappings = {};

} // namespace emsesp

// clang-format on
'''
    
    header_path = Path("src") / "core" / "modbus_entity_parameters.hpp"
    header_path.parent.mkdir(parents=True, exist_ok=True)
    
    with open(header_path, 'w') as f:
        f.write(header_content)
    
    print(f"Created dummy header file: {header_path} ({os.path.getsize(header_path)} bytes)")

if not (env.IsCleanTarget()):
    create_dummy_modbus_header()
