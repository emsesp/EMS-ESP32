#
# Update modbus parameters from entity definitions.
# This script generates c++ code for the modbus parameter definitions.
# Called by /scripts/generate_csv_and_headers.sh

import fileinput
import csv
import json
import re
from string import Template

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
    "controller/datetime": 25
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
    35: "TAG_TYPE_HS"
}

device_type_names = [
    "SYSTEM",
    "TEMPERATURESENSOR",
    "ANALOGSENSOR",
    "SCHEDULER",
    "CUSTOM",
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
    "WATER"
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
    
// IMPORTANT: This list MUST be ordered by keys "device_type", "device_value_tag_type" and "modbus_register_offset" in this order.    
const std::initializer_list<Modbus::EntityModbusInfo> Modbus::modbus_register_mappings = {
$entries};

} // namespace emsesp

// clang-format off
''')
# cpp_entry_template = Template(
#    '    {std::make_tuple($devtype, $tagtype, std::string(\"$shortname\")), {$registeroffset, $registercount}},\n')
cpp_entry_template = Template(
    '    REGISTER_MAPPING($devtype, $tagtype, $shortname, $registeroffset, $registercount), // $entity_name\n')

# read translations
listNames = {}
transre = re.compile(r'^MAKE_TRANSLATION\(([^,\s]+)\s*,\s*\"([^\"]+)\"')
transf = open('./src/core/locale_translations.h', 'r')
while True:
    line = transf.readline()
    if not line:
        break
    m = transre.match(line)
    if m is not None:
        listNames[m.group(2)] = m.group(1)
transf.close()

entities = []

with fileinput.input() as f_input:
    entities_reader = csv.reader(f_input, delimiter=',', quotechar='"')
    headers = next(entities_reader)

    for row in entities_reader:
        entity = {}
        for i, val in enumerate(row):
            # print(headers[i] + ": " + val)
            entity[headers[i]] = val
        entities.append(entity)

# print(json.dumps(entities, indent="  "))

device_types = {}
string_entities = []

entity_modbus_property_names = [
    "modbus block",
    "modbus offset",
    "modbus count"
]

for entity in entities:
    device_type_name = entity['device type'].upper()
    if device_type_name not in device_types:
        device_types[device_type_name] = {}
    device_type = device_types[device_type_name]
    tag_name = entity['modbus block']
    if tag_name not in device_type:
        device_type[tag_name] = {}
    tag = device_type[tag_name]
    entity_shortname = entity['shortname']
    entity_dev_name = (device_type_name + "/" + entity_shortname).lower()

    # set size for string entities
    if entity["modbus count"] == "0" and entity_dev_name in string_sizes:
        entity["modbus count"] = - \
            (-string_sizes[entity_dev_name] // 2)  # divide and round up

    if int(entity["modbus count"]) <= 0:
        raise Exception('Entity "' + entity_dev_name + ' (' + entity_shortname + ')' +
                        '" does not have a size - string sizes need to be added manually to update_modbus_registers.py/string_sizes')

    #    if entity["modbus count"] == "0":
    #        print("ignoring " + entity_dev_name + " - it has a register length of zero")

    if entity_shortname in tag:
        for entity_property_name in entity_modbus_property_names:
            if tag[entity_shortname][entity_property_name] != entity[entity_property_name]:
                raise Exception(
                    "Property mismatch between instances of the same entity for property '" + entity_property_name + "':\n" +
                    json.dumps(tag[entity_shortname]) +
                    json.dumps(entity))
    else:
        tag[entity_shortname] = {}
        for entity_property_name in entity_modbus_property_names:
            tag[entity_shortname][entity_property_name] = entity[entity_property_name]

# print(json.dumps(device_types, indent="  "))

# ASSIGN REGISTERS

for device_type_name, device_type in device_types.items():
    for tag, entities in device_type.items():
        total_registers = 0
        next_free_offset = 0
        for entity_name, modbus_info in entities.items():
            register_offset = int(modbus_info['modbus offset'])
            register_count = int(modbus_info['modbus count'])
            total_registers += register_count
            if register_offset >= 0 and register_offset + register_count > next_free_offset:
                next_free_offset = register_offset + register_count

        # print(device_type_name + "/" + tag + ": total_registers=" + str(total_registers) + "; next_free_offset=" + str(
        #    next_free_offset))

        for entity_name, modbus_info in entities.items():
            register_offset = int(modbus_info['modbus offset'])
            register_count = int(modbus_info['modbus count'])
            if register_offset < 0 and register_count > 0:
                # assign register
                # print("assign " + entity_name + " -> " + str(next_free_offset))
                modbus_info['modbus offset'] = str(next_free_offset)
                next_free_offset += register_count

# OUTPUT

cpp_entries = ""

# traverse all elements in correct order so they are correctly sorted
for device_type_name in device_type_names:
    if device_type_name in device_types:
        device_type = device_types[device_type_name]
        for ntag in range(0, 40):
            tag = str(ntag)
            if tag in device_type:
                entities = device_type[tag]
                for entity_name, modbus_info in sorted(entities.items(), key=lambda x: int(x[1]["modbus offset"])):
                    params = {
                        'devtype': "dt::" + device_type_name,
                        # re.sub(r"[0-9]+", "*", tag),
                        "tagtype": tag_to_tagtype[int(tag)],
                        "shortname": 'FL_(' + listNames[entity_name] + ")",
                        "entity_name": entity_name,
                        'registeroffset': modbus_info["modbus offset"],
                        'registercount': modbus_info["modbus count"]
                    }
                    # print(entitypath + ": " + str(modbus_info))
                    cpp_entries += cpp_entry_template.substitute(params)

cpp_src = cpp_file_template.substitute({'entries': cpp_entries})
print(cpp_src)
