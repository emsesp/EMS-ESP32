#
# Shared helpers for reading the Modbus register map out of src/core/modbus_entity_parameters.hpp.
# Used by update_modbus_registers.py, build_modbus_entity_parameters_pre.py and modbus_register_diff.py.
#

import re
import subprocess
from collections import namedtuple

HEADER_PATH = "src/core/modbus_entity_parameters.hpp"

# version: None for headers that predate the map version register
# active: (device_type, tag_type, fl_name) -> (offset, count)
# retired: [((device_type, tag_type, fl_name), (offset, count))], one entity can have several
# names: (device_type, tag_type, fl_name) -> shortname
RegisterMap = namedtuple("RegisterMap", ["version", "active", "retired", "names"])

_mapping_re = re.compile(
    r'^\s*(//\s*RETIRED\s*)?REGISTER_MAPPING\(dt::(\w+),\s*(\w+),\s*FL_\((\w+)\),\s*(\d+),\s*(\d+)\),?\s*(?://\s*(\S+))?')
_version_re = re.compile(r'REGISTER_MAP_VERSION\s*=\s*(\d+)')


def parse_header_text(text):
    register_map = RegisterMap(None, {}, [], {})
    version = None
    for line in text.splitlines():
        m = _version_re.search(line)
        if m is not None and version is None:
            version = int(m.group(1))
            continue
        m = _mapping_re.match(line)
        if m is None:
            continue
        key = (m.group(2), m.group(3), m.group(4))
        value = (int(m.group(5)), int(m.group(6)))
        if m.group(1):
            register_map.retired.append((key, value))
        else:
            register_map.active[key] = value
        if m.group(7):
            register_map.names.setdefault(key, m.group(7))
    return register_map._replace(version=version)


def read_header_file(path):
    with open(path, 'r', encoding='utf-8', errors='replace') as f:
        return parse_header_text(f.read())


def read_header_at_ref(ref, path=HEADER_PATH):
    text = subprocess.run(["git", "show", f"{ref}:{path}"], check=True, capture_output=True,
                          text=True, encoding='utf-8', errors='replace').stdout
    return parse_header_text(text)
