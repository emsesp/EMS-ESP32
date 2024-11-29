import fileinput
import csv
from itertools import groupby

#
# This is used to build the contents of the `Modbus-Entity-Registers.md` file used in the emsesp.org documentation.
#

# static data

tag_to_tagtype = {
    -1: "TAG_TYPE_NONE",
    0: "DEVICE_DATA",
    1: "HC",
    2: "HC",
    3: "HC",
    4: "HC",
    5: "HC",
    6: "HC",
    7: "HC",
    8: "HC",
    9: "DHW",
    10: "DHW",
    11: "DHW",
    12: "DHW",
    13: "DHW",
    14: "DHW",
    15: "DHW",
    16: "DHW",
    17: "DHW",
    18: "DHW",
    19: "AHS",
    20: "HS",
    21: "HS",
    22: "HS",
    23: "HS",
    24: "HS",
    25: "HS",
    26: "HS",
    27: "HS",
    28: "HS",
    29: "HS",
    30: "HS",
    31: "HS",
    32: "HS",
    33: "HS",
    34: "HS",
    35: "HS"
}

# read entities csv from stdin

entities = []

with fileinput.input() as f_input:
    entities_reader = csv.reader(f_input, delimiter=',', quotechar='"')
    headers = next(entities_reader)

    for row in entities_reader:
        entity = {}
        for i, val in enumerate(row):
            entity[headers[i]] = val
        entities.append(entity)


def device_name_key(e): return e["device name"]


def device_type_key(e): return e["device type"]


def grouped_by(list, key): return groupby(sorted(list, key=key), key)


# entities_by_device_type = grouped_by(entities, device_type_key)


def printDeviceEntities(device_name, device_entities):
    print("### " + device_name)
    print("| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |")
    print("|-|-|-|-|-|-|-|-|-|")
    for de in device_entities:
        print("| " + de["shortname"] + " | " + de["fullname"] + " | " + de["type [options...] \\| (min/max)"] + " | " + de["uom"] + " | " + de["writeable"] +
              " | " + tag_to_tagtype[int(de["modbus block"])] + " | " + de["modbus offset"] + " | " + de["modbus count"] + " | " + de["modbus scale factor"] + " | ")
    print()


def printDeviceTypeDevices(device_type, devices):
    print("## Devices of type *" + device_type + "*")
    for device_name, device_entities in grouped_by(devices, device_name_key):
        printDeviceEntities(device_name, device_entities)


# write header

print("<!-- Use full browser width for this page, the tables are wide -->")
print("<style>")
print(".md-grid {")
print("    max-width: 100%; /* or 100%, if you want to stretch to full-width */")
print("}")
print("</style>")
print()
print("# Entity/Register Mapping")
print()
print("!!! note")
print()
print("    This file has been auto-generated. Do not modify.")
print()

for device_type, devices in grouped_by(entities, device_type_key):
    printDeviceTypeDevices(device_type, devices)

# def printGroupedData(groupedData):
#    for k, v in groupedData:
#        # print("Group {} {}".format(k, list(v)))
#        print(k)


# printGroupedData(grouped_entities)

# for e in entities:
#     print(e)
