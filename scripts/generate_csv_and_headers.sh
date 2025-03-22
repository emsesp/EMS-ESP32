#!/bin/sh

#
# Builds the dump_*.csv files, modbus headers and modbus documentation.
# Run as `sh scripts/generate_csv_and_headers.sh` from the root of the repository.

# create a dummy modbus_entity_parameters.hpp so the first pass compiles
cat >./src/core/modbus_entity_parameters.hpp <<EOL
#include "modbus.h"
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

EOL

# First generate Modbus entity parameters
# build the modbus_entity_parameters.hpp header file
make clean
make -s ARGS=-DEMSESP_MODBUS
rm -f ./src/core/modbus_entity_parameters.hpp ./docs/dump_entities.csv
echo "test entity_dump" | ./emsesp | python3 ./scripts/strip_csv.py > ./docs/dump_entities.csv
cat ./docs/dump_entities.csv | python3 ./scripts/update_modbus_registers.py > ./src/core/modbus_entity_parameters.hpp

# regenerate dump_entities.csv but without the Modbus entity parameters
make clean
make -s ARGS=-DEMSESP_STANDALONE
rm -f ./docs/dump_entities.csv
echo "test entity_dump" | ./emsesp | python3 ./scripts/strip_csv.py > ./docs/dump_entities.csv

# generate Modbus doc - Modbus-Entity-Registers.md used in the emsesp.org documentation
rm -f ./docs/Modbus-Entity-Registers.md
cat ./docs/dump_entities.csv | python3 ./scripts/generate-modbus-register-doc.py > ./docs/Modbus-Entity-Registers.md

# dump_telegrams.csv
rm -f ./docs/dump_telegrams.csv
echo "test telegram_dump" | ./emsesp | python3 ./scripts/strip_csv.py > ./docs/dump_telegrams.csv

ls -al ./src/core/modbus_entity_parameters.hpp
ls -al ./docs/Modbus-Entity-Registers.md
ls -al ./docs/dump_entities.csv
ls -al ./docs/dump_telegrams.csv
