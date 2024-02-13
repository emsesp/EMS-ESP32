import fileinput
import csv
import json
import re
from string import Template

max_circuit_num = {
    'hc1': 8,  # max. 8 heating circuits
    'wwc1': 10,  # max. 10 warm water circuits
    'hs1': 16  # max. 16 heat sources
}

modbus_block_size = 1000  # block size of a register block for each tag

# string sizes including terminating NUL. Extracted from the source codes.
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
    "thermostat/wwholidays": 26,
    "thermostat/wwvacations": 26,
    "thermostat/holidays": 26,
    "thermostat/vacations": 26,
    "thermostat/switchtime1": 16,
    "thermostat/switchtime2": 16,
    "thermostat/switchtime": 16,
    "controller/datetime": 25
}

tag_to_tagtype = {
    0: "TAG_TYPE_NONE = 0",
    1: "TAG_TYPE_HEARTBEAT",
    2: "TAG_TYPE_BOILER_DATA_WW",
    3: "TAG_TYPE_DEVICE_DATA",
    4: "TAG_TYPE_DEVICE_DATA_WW",
    5: "TAG_TYPE_HC",
    6: "TAG_TYPE_HC",
    7: "TAG_TYPE_HC",
    8: "TAG_TYPE_HC",
    9: "TAG_TYPE_HC",
    10: "TAG_TYPE_HC",
    11: "TAG_TYPE_HC",
    12: "TAG_TYPE_HC",
    13: "TAG_TYPE_WWC",
    14: "TAG_TYPE_WWC",
    15: "TAG_TYPE_WWC",
    16: "TAG_TYPE_WWC",
    17: "TAG_TYPE_WWC",
    18: "TAG_TYPE_WWC",
    19: "TAG_TYPE_WWC",
    20: "TAG_TYPE_WWC",
    21: "TAG_TYPE_WWC",
    22: "TAG_TYPE_WWC",
    23: "TAG_TYPE_AHS",
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
    36: "TAG_TYPE_HS",
    37: "TAG_TYPE_HS",
    38: "TAG_TYPE_HS",
    39: "TAG_TYPE_HS"
}

cpp_file_template = Template('''#include "modbus.h"
#include "emsdevice.h"

namespace emsesp {

using dt = EMSdevice::DeviceType;

void Modbus::register_mappings() {
$entries};

} // namespace emsesp''')
#cpp_entry_template = Template(
#    '    {std::make_tuple($devtype, $tagtype, std::string(\"$shortname\")), {$registeroffset, $registercount}},\n')
cpp_entry_template = Template(
    '    register_mapping($devtype, $tagtype, \"$shortname\", $registeroffset, $registercount);\n')
entities = []

with fileinput.input() as f_input:
    entities_reader = csv.reader(f_input, delimiter=',', quotechar='"')
    headers = next(entities_reader)

    for row in entities_reader:
        entity = {}
        for i, val in enumerate(row):
            entity[headers[i]] = val
            entities.append(entity)

# print(json.dumps(entities, indent="  "))

device_types = {}
string_entities = []

entity_modbus_property_names = [
    "modbus offset",
    "modbus count"
]

for entity in entities:
    device_type_name = entity['device type']
    if device_type_name not in device_types:
        device_types[device_type_name] = {}
    device_type = device_types[device_type_name]
    tag_name = entity['tag']
    if tag_name not in device_type:
        device_type[tag_name] = {}
    tag = device_type[tag_name]
    entity_shortname = entity['shortname']
    entity_dev_name = device_type_name + "/" + entity_shortname

    # set size for string entities
    if entity["modbus count"] == "0" and entity_dev_name in string_sizes:
        entity["modbus count"] = -(-string_sizes[entity_dev_name] // 2)  # divide and round up

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

# exit(0)

# OUTPUT

cpp_entries = ""

for device_type_name, device_type in device_types.items():
    for tag, entities in device_type.items():
        for entity_name, modbus_info in entities.items():
            params = {
                'devtype': "dt::" + device_type_name.upper(),
                "tagtype": tag_to_tagtype[int(tag)],  # re.sub(r"[0-9]+", "*", tag),
                "shortname": entity_name,
                'registeroffset': modbus_info["modbus offset"],
                'registercount': modbus_info["modbus count"]
            }
            # print(entitypath + ": " + str(modbus_info))
            cpp_entries += cpp_entry_template.substitute(params)

cpp_src = cpp_file_template.substitute({'entries': cpp_entries})
print(cpp_src)
