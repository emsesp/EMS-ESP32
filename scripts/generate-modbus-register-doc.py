import fileinput
import csv
import sys
from itertools import groupby
from collections import defaultdict

#
# This is used to build the contents of the `Modbus-Entity-Registers.md` file used in the emsesp.org documentation.
#


def get_tag_type(modbus_block):
    """Convert modbus block number to tag type using lookup."""
    block = int(modbus_block)

    # Handle special cases first
    if block == -1:
        return "TAG_TYPE_NONE"
    if block == 0:
        return "DEVICE_DATA"
    if block == 19:
        return "AHS"

    # Use ranges for efficiency
    if 1 <= block <= 8:
        return "HC"
    if 9 <= block <= 18:
        return "DHW"
    if 20 <= block <= 35:
        return "HS"
    if 36 <= block <= 51:
        return "SRC"

    # Default fallback
    return "UNKNOWN"


def read_entities():
    """Read and parse CSV entities from stdin with error handling."""
    entities = []

    try:
        with fileinput.input() as f_input:
            entities_reader = csv.reader(f_input, delimiter=',', quotechar='"')
            headers = next(entities_reader)

            # Validate required headers
            required_headers = {'device name', 'device type', 'shortname', 'fullname',
                                'type [options...] \\| (min/max)', 'uom', 'writeable',
                                'modbus block', 'modbus offset', 'modbus count', 'modbus scale factor'}
            missing_headers = required_headers - set(headers)
            if missing_headers:
                raise ValueError(
                    f"Missing required headers: {missing_headers}")

            for row_num, row in enumerate(entities_reader, start=2):
                if len(row) != len(headers):
                    print(
                        f"Warning: Row {row_num} has {len(row)} columns, expected {len(headers)}", file=sys.stderr)
                    continue

                entity = dict(zip(headers, row))
                entities.append(entity)

    except Exception as e:
        print(f"Error reading CSV data: {e}", file=sys.stderr)
        sys.exit(1)

    return entities


def group_entities_by_device_type(entities):
    """Group entities by device type efficiently using defaultdict."""
    grouped = defaultdict(list)
    for entity in entities:
        grouped[entity["device type"]].append(entity)
    return grouped


def print_device_entities(device_name, device_entities):
    """Print device entities table using f-strings for better performance."""
    print(f"### {device_name}")
    print()
    print("| shortname | fullname | type | uom | writeable | tag type | register offset | register count | scale factor |")
    print("|-|-|-|-|-|-|-|-|-|")

    for entity in device_entities:
        tag_type = get_tag_type(entity["modbus block"])
        type_long = entity['type [options...] \\| (min/max)']
        
        # Split off type and the rest efficiently
        split_type = type_long.split(" ", 1)
        type_base = split_type[0]
        type_suffix = split_type[1] if len(split_type) > 1 else ""

        # Optimize type_rest extraction and int range detection
        type_rest_str = ""
        if "int" in type_base and "(" in type_suffix:
            print(type_base)
            try:
                # Extract inner part of parentheses
                range_inner = type_suffix[type_suffix.index("(")+1:type_suffix.index(")")]
                min_value = max_value = None
                if ">=" in range_inner and "<=" in range_inner:
                    # >=0<=100 style
                    min_value, max_value = range_inner.replace(">=", "").split("<=")
                elif "/" in range_inner:
                    min_value, max_value = range_inner.split("/")
                if min_value is not None and max_value is not None:
                    type_rest_str = f" (&gt;={min_value.strip()}&lt;={max_value.strip()})"
            except Exception:
                # fallback to original
                pass

        print(f"| {entity['shortname']} | {entity['fullname']} | {type_base}{type_rest_str} | "
              f"{entity['uom']} | {entity['writeable']} | {tag_type} | {entity['modbus offset']} | "
              f"{entity['modbus count']} | {entity['modbus scale factor']} |")
    print()


def print_device_type_devices(device_type, devices):
    """Print all devices of a specific type."""
    print(f"## Devices of type \\_{device_type}")
    print()

    # Group devices by name
    device_groups = defaultdict(list)
    for device in devices:
        device_groups[device["device name"]].append(device)

    for device_name, device_entities in device_groups.items():
        print_device_entities(device_name, device_entities)


def print_header():
    """Print the markdown document header."""
    print("---")
    print("id: Modbus-Entity-Registers")
    print("---")
    print("# Modbus Entity/Register Mapping")
    print()
    print(":::warning")
    print("This file has been auto-generated. Do not modify.")
    print(":::")
    print()


def main():
    """Main function to process entities and generate documentation."""
    # Read entities from stdin
    entities = read_entities()

    if not entities:
        print("No entities found in input data.", file=sys.stderr)
        sys.exit(1)

    # Print header
    print_header()

    # Group entities by device type and process
    grouped_entities = group_entities_by_device_type(entities)

    # Print documentation for each device type
    for device_type, devices in grouped_entities.items():
        print_device_type_devices(device_type, devices)


if __name__ == "__main__":
    main()
