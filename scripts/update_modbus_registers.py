#
# Update modbus parameters from entity definitions.
# This script generates c++ code for the modbus parameter definitions.
#
# Register offsets are stable: every entity keeps the offset it has in the previous map (--previous),
# new entities are appended after the highest offset ever used, and removed entities are kept as
# RETIRED comments so their offsets are never handed out again. An entity whose register count grows
# is moved to the end. Moving or retiring a register bumps REGISTER_MAP_VERSION.
#
# Called by the pio build_modbus target. Can be called manually with:
#   cp ./src/core/modbus_entity_parameters.hpp /tmp/prev.hpp
#   python3 ./scripts/update_modbus_registers.py --previous /tmp/prev.hpp < ./docs/dump_entities.csv > ./src/core/modbus_entity_parameters.hpp

import argparse
import fileinput
import csv
import json
import re
import sys
from string import Template

from modbus_register_map import read_header_file

parser = argparse.ArgumentParser()
parser.add_argument("--previous", required=True,
                    help="previous modbus_entity_parameters.hpp whose offsets must be kept")
parser.add_argument("--allow-new-map", action="store_true",
                    help="allow a previous map without entries, i.e. renumber everything from scratch")
parser.add_argument("csv", nargs="*", help="entity dump csv (default: stdin)")
args = parser.parse_args()

modbus_block_size = 1000  # block size of a register block for each tag

# string sizes including terminating NUL. Extracted from the source code.
string_sizes = {
    "boiler/lastcode": 55,
    "boiler/servicecode": 4,
    "boiler/maintenancemessage": 4,
    "boiler/maintenancedate": 12,
    "boiler/hpin1opt": 16,
    "boiler/hpin2opt": 16,
    "boiler/hpin3opt": 16,
    "boiler/hpin4opt": 16,
    "thermostat/errorcode": 16,
    "thermostat/lastcode": 50,
    "thermostat/datetime": 25,
    "thermostat/wwswitchtime": 16,
    "thermostat/wwcircswitchtime": 16,
    "thermostat/circswitchtime": 16,
    "thermostat/wwholidays": 26,
    "thermostat/wwvacations": 26,
    "thermostat/vacations1": 22,
    "thermostat/vacations2": 22,
    "thermostat/vacations3": 22,
    "thermostat/vacations4": 22,
    "thermostat/vacations5": 22,
    "thermostat/vacations6": 22,
    "thermostat/vacations7": 22,
    "thermostat/holidays": 26,
    "thermostat/vacations": 26,
    "thermostat/switchtime1": 16,
    "thermostat/switchtime2": 16,
    "thermostat/switchtime": 16,
    "thermostat/switchtimeww": 21,
    "controller/datetime": 25,
    "connect/datetime": 25,
    "connect/name": 51
}

tag_to_tagtype = {
    -1: "TAG_TYPE_NONE",
    0: "TAG_TYPE_DEVICE_DATA",
    1: "TAG_TYPE_HC",
    2: "TAG_TYPE_HC",
    3: "TAG_TYPE_HC",
    4: "TAG_TYPE_HC",
    5: "TAG_TYPE_HC",
    6: "TAG_TYPE_HC",
    7: "TAG_TYPE_HC",
    8: "TAG_TYPE_HC",
    9: "TAG_TYPE_DHW",
    10: "TAG_TYPE_DHW",
    11: "TAG_TYPE_DHW",
    12: "TAG_TYPE_DHW",
    13: "TAG_TYPE_DHW",
    14: "TAG_TYPE_DHW",
    15: "TAG_TYPE_DHW",
    16: "TAG_TYPE_DHW",
    17: "TAG_TYPE_DHW",
    18: "TAG_TYPE_DHW",
    19: "TAG_TYPE_AHS",
    20: "TAG_TYPE_HS",
    21: "TAG_TYPE_HS",
    22: "TAG_TYPE_HS",
    23: "TAG_TYPE_HS",
    24: "TAG_TYPE_HS",
    25: "TAG_TYPE_HS",
    26: "TAG_TYPE_HS",
    27: "TAG_TYPE_HS",
    28: "TAG_TYPE_HS",
    29: "TAG_TYPE_HS",
    30: "TAG_TYPE_HS",
    31: "TAG_TYPE_HS",
    32: "TAG_TYPE_HS",
    33: "TAG_TYPE_HS",
    34: "TAG_TYPE_HS",
    35: "TAG_TYPE_HS",
    36: "TAG_TYPE_SRC",
    37: "TAG_TYPE_SRC",
    38: "TAG_TYPE_SRC",
    39: "TAG_TYPE_SRC",
    40: "TAG_TYPE_SRC",
    41: "TAG_TYPE_SRC",
    42: "TAG_TYPE_SRC",
    43: "TAG_TYPE_SRC",
    44: "TAG_TYPE_SRC",
    45: "TAG_TYPE_SRC",
    46: "TAG_TYPE_SRC",
    47: "TAG_TYPE_SRC",
    48: "TAG_TYPE_SRC",
    49: "TAG_TYPE_SRC",
    50: "TAG_TYPE_SRC",
    51: "TAG_TYPE_SRC"
}

device_type_names = [
    "SYSTEM",
    "TEMPERATURESENSOR",
    "ANALOGSENSOR",
    "SCHEDULER",
    "CUSTOM",
    "COMMAND",
    "BOILER",
    "THERMOSTAT",
    "MIXER",
    "SOLAR",
    "HEATPUMP",
    "GATEWAY",
    "SWITCH",
    "CONTROLLER",
    "CONNECT",
    "ALERT",
    "EXTENSION",
    "GENERIC",
    "HEATSOURCE",
    "VENTILATION",
    "WATER",
    "POOL"
]

cpp_file_template = Template('''#include "modbus.h"
#include "emsdevice.h"

/*
 * This file is auto-generated. Do not modify.
 */

// clang-format off

namespace emsesp {

using dt = EMSdevice::DeviceType;

#define REGISTER_MAPPING(device_type, device_value_tag_type, long_name, modbus_register_offset, modbus_register_count) \\
    { device_type, device_value_tag_type, long_name[0], modbus_register_offset, modbus_register_count }

// Increases whenever an existing register is moved or retired. Readable at address 2 of the system unit.
const uint16_t Modbus::REGISTER_MAP_VERSION = $version;

// IMPORTANT: This list MUST be ordered by keys "device_type", "device_value_tag_type" and "modbus_register_offset" in this order.
const std::initializer_list<Modbus::EntityModbusInfo> Modbus::modbus_register_mappings = {
$entries};

// Registers of removed entities. Their offsets stay reserved and must never be reused.
$retired
} // namespace emsesp

// clang-format on
''')
# cpp_entry_template = Template(
#    '    {std::make_tuple($devtype, $tagtype, std::string(\"$shortname\")), {$registeroffset, $registercount}},\n')
cpp_entry_template = Template(
    '    REGISTER_MAPPING($devtype, $tagtype, $shortname, $registeroffset, $registercount), // $entity_name\n')

# read translations
listNames = {}
transre = re.compile(r'^MAKE_TRANSLATION\(([^,\s]+)\s*,\s*\"([^\"]+)\"')
try:
    with open('./src/core/locale_translations.h', 'r', encoding='utf-8', errors='replace') as transf:
        for line in transf:
            m = transre.match(line)
            if m is not None:
                listNames[m.group(2)] = m.group(1)
except FileNotFoundError:
    # Handle case where file doesn't exist
    raise Exception('Error! locale_translations.h not found')

previous_version, previous_active, previous_retired, previous_names = read_header_file(args.previous)
if not previous_active and not args.allow_new_map:
    raise Exception('Error! Previous modbus map "' + args.previous + '" has no entries. All register offsets would be renumbered. '
                    'Use --allow-new-map if that is really intended.')

entities = []

with fileinput.input(files=args.csv or ('-',), encoding='utf-8', errors='replace') as f_input:
    entities_reader = csv.reader(f_input, delimiter=',', quotechar='"')
    headers = next(entities_reader)

    for row in entities_reader:
        entity = {headers[i]: val for i, val in enumerate(row)}
        entities.append(entity)

# (device type, tag type) -> {fl_name: {"shortname", "count"}}, in order of first appearance
groups = {}

for entity in entities:
    device_type_name = entity['device type'].upper()
    tag = int(entity['modbus block'])
    if tag not in tag_to_tagtype or tag_to_tagtype[tag] == "TAG_TYPE_NONE":
        continue
    tag_type = tag_to_tagtype[tag]
    entity_shortname = entity['shortname']
    entity_dev_name = (device_type_name + "/" + entity_shortname).lower()

    # set size for string entities
    if entity["modbus count"] == "0" and entity_dev_name in string_sizes:
        entity["modbus count"] = - \
            (-string_sizes[entity_dev_name] // 2)  # divide and round up

    register_count = int(entity["modbus count"])
    if register_count <= 0:
        raise Exception('Error! Entity "' + entity_dev_name + ' (' + entity_shortname + ')' +
                        '" does not have a size - string sizes need to be added manually to update_modbus_registers.py/string_sizes[]')

    # Strip device type prefix (e.g., "dhw.nrg" -> "nrg") for translation lookup
    lookup_name = entity_shortname.split('.')[-1]
    if lookup_name not in listNames:
        raise KeyError(f"Error! Translation not found for '{lookup_name}' (entity: '{entity_shortname}'). Please add it to locale_translations.h")
    fl_name = listNames[lookup_name]

    group = groups.setdefault((device_type_name, tag_type), {})
    if fl_name in group:
        if group[fl_name]["count"] != register_count:
            raise Exception("Register count mismatch between instances of the same entity:\n" +
                            json.dumps(group[fl_name]) + json.dumps(entity))
    else:
        group[fl_name] = {"shortname": lookup_name, "count": register_count}

# ASSIGN REGISTERS

active = {}  # (device type, tag type, fl_name) -> (offset, count, shortname)
retired = list(previous_retired)  # [((device type, tag type, fl_name), (offset, count))]
breaking_changes = []

for group_key in set(groups) | {k[:2] for k in previous_active} | {k[:2] for k, _ in previous_retired}:
    used = [v for k, v in previous_active.items() if k[:2] == group_key] + \
        [v for k, v in previous_retired if k[:2] == group_key]
    next_free_offset = max((offset + count for offset, count in used), default=0)

    for fl_name, info in groups.get(group_key, {}).items():
        key = group_key + (fl_name,)
        count = info["count"]
        previous = previous_active.get(key)
        if previous is not None and previous[1] == count:
            offset = previous[0]
        else:
            revived = next((r for r in retired if r[0] == key and r[1][1] == count), None)
            if revived is not None:
                retired.remove(revived)
                offset = revived[1][0]
            else:
                offset = next_free_offset
                next_free_offset += count
            if previous is not None:
                retired.append((key, previous))
                breaking_changes.append(f"moved {key}: {previous} -> {(offset, count)}")
        active[key] = (offset, count, info["shortname"])

for key, value in previous_active.items():
    if key not in active:
        retired.append((key, value))
        breaking_changes.append(f"retired {key}: {value}")

for group_key in {k[:2] for k in active} | {k[:2] for k, _ in retired}:
    ranges = sorted([(v[0], v[1], k[2]) for k, v in active.items() if k[:2] == group_key] +
                    [(v[0], v[1], "retired " + k[2]) for k, v in retired if k[:2] == group_key])
    for a, b in zip(ranges, ranges[1:]):
        if a[0] + a[1] > b[0]:
            raise Exception(f"Error! Overlapping modbus registers in {group_key}: {a} and {b}")
    if ranges and ranges[-1][0] + ranges[-1][1] > modbus_block_size:
        raise Exception(f"Error! Modbus register {ranges[-1]} in {group_key} exceeds the register block size of {modbus_block_size}")

version = (previous_version or 1) + (1 if breaking_changes else 0)
for change in breaking_changes:
    print("Modbus register map change: " + change, file=sys.stderr)

# OUTPUT

tag_type_order = ["TAG_TYPE_DEVICE_DATA", "TAG_TYPE_HC", "TAG_TYPE_DHW", "TAG_TYPE_AHS", "TAG_TYPE_HS", "TAG_TYPE_SRC"]


def sort_key(item):
    (device_type_name, tag_type, _), value = item
    return (device_type_names.index(device_type_name), tag_type_order.index(tag_type), value[0])


cpp_entries = []
for (device_type_name, tag_type, fl_name), (offset, count, shortname) in sorted(active.items(), key=sort_key):
    cpp_entries.append(cpp_entry_template.substitute({
        'devtype': "dt::" + device_type_name,
        "tagtype": tag_type,
        "shortname": 'FL_(' + fl_name + ")",
        "entity_name": shortname,
        'registeroffset': offset,
        'registercount': count
    }))

retired_entries = []
for (device_type_name, tag_type, fl_name), (offset, count) in sorted(retired, key=sort_key):
    shortname = previous_names.get((device_type_name, tag_type, fl_name), fl_name.lower())
    retired_entries.append(f"// RETIRED REGISTER_MAPPING(dt::{device_type_name}, {tag_type}, FL_({fl_name}), {offset}, {count}) // {shortname}\n")

cpp_src = cpp_file_template.substitute({
    'version': version,
    'entries': "".join(cpp_entries),
    'retired': "".join(retired_entries)
})
print(cpp_src)

