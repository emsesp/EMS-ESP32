#!/bin/sh

#
# Builds the dump CSV files, modbus headers and documentation.
# Called by /scripts/generate_csv_and_headers.sh

# build dummy modbus_entity_parameters.hpp  so it compiles
cat >./src/modbus_entity_parameters.hpp <<EOL
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

# build emsesp for standalone
make clean
make -s ARGS=-DEMSESP_STANDALONE

# Generate Modbus entity parameters
# One to build the modbus_entity_parameters.hpp header file
# And then run entity_dump test again to create the dump_entities.csv file with the correct modbus counts
rm -f ./src/modbus_entity_parameters.hpp
echo "test entity_dump" | ./emsesp | python3 ./scripts/strip_csv.py | python3 ./scripts/update_modbus_registers.py >./src/modbus_entity_parameters.hpp
ls -al ./src/modbus_entity_parameters.hpp

# dump_entities.csv
make -s ARGS=-DEMSESP_STANDALONE
rm -f ./docs/dump_entities.csv
echo "test entity_dump" | ./emsesp | python3 ./scripts/strip_csv.py >./docs/dump_entities.csv
ls -al ./docs/dump_entities.csv

# dump_telegrams.csv
rm -f ./docs/dump_telegrams.csv
echo "test telegram_dump" | ./emsesp | python3 ./scripts/strip_csv.py >./docs/dump_telegrams.csv
ls -al ./docs/dump_telegrams.csv

# generate doc - Modbus-Entity-Registers.md used in the emsesp.org documentation
rm -f ./docs/Modbus-Entity-Registers.md
cat ./docs/dump_entities.csv | python3 ./scripts/generate-modbus-register-doc.py >./docs/Modbus-Entity-Registers.md
ls -al ./docs/Modbus-Entity-Registers.md
