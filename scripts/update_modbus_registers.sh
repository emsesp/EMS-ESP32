#!/bin/bash

# Update modbus parameters from entity definitions.
# This script generates c++ code for the modbus parameter definitions.
#
# Run this script from the EMS-ESP32 root directory with the command `scripts/update_modbus_registers.sh`.

while [[ $# -gt 0 ]]; do
  case $1 in
    --reset)
      RESET=YES
      shift
      ;;
    --force)
      FORCE=YES
      shift
      ;;
    *|-*|--*)
      echo "Unknown option $1"
      exit 1
      ;;
  esac
done

if [ "$RESET" = "YES" ]; then
  if [ "$FORCE" != "YES" ]; then
    read -p  "Are you sure you want to reset all modbus entity parameters, potentially generating new register mappings? WARNING: This might introduce breaking changes for end users. [yes/no] " CONFIRMATION
    if [ "$CONFIRMATION" != "yes" ]; then
      echo "Aborted."
      exit 1
    fi
  fi
  echo "Resetting modbus configuration"
  cat >./src/modbus_entity_parameters.hpp <<EOL
#include "modbus.h"
#include "emsdevice.h"

namespace emsesp {

using dt = EMSdevice::DeviceType;

#define REGISTER_MAPPING(device_type, device_value_tag_type, long_name, modbus_register_offset, modbus_register_count) \\
    { device_type, device_value_tag_type, long_name[0], modbus_register_offset, modbus_register_count }

// IMPORTANT: This list MUST be ordered by keys "device_type", "device_value_tag_type" and "modbus_register_offset" in this order.
const std::initializer_list<Modbus::EntityModbusInfo> Modbus::modbus_register_mappings = {};

} // namespace emsesp
EOL
fi

make clean
make ARGS=-DEMSESP_STANDALONE
echo "test entity_dump" | ./emsesp | python3 ./scripts/strip_csv.py | python3 ./scripts/update_modbus_registers.py > ./src/modbus_entity_parameters.hpp
echo "Modbus entity parameters written to ./src/modbus_entity_parameters.hpp."
