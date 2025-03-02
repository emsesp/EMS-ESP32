#!/usr/bin/env python3
# app-tls-size - Calculate size of Thread-Local Storage
# Copyright 2022  Simon Arlott

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

# PlatformIO usage:
#
# [env:...]
# extra_scripts = post:app-tls-size.py

import argparse
import collections
import re
import subprocess
import sys

RE_ELF_SECTION = re.compile(
    r"^\s*(?P<type>\w+)\s+(?P<offset>\w+)\s+(?P<virtaddr>\w+)\s+(?P<physaddr>\w+)\s+(?P<filesiz>\w+)\s+(?P<memsiz>\w+)\s+(?P<ndx>\w+)\s+")
Symbol = collections.namedtuple("Symbol", ["value", "size", "line"])
RE_ELF_SYMBOL = re.compile(
    r"^(?P<before_value>\s*(?P<num>\w+):\s+)(?P<value>\w+)(?P<after_value>\s+(?P<size>\w+)\s+(?P<type>\w+)\s+(?P<bind>\w+)\s+(?P<visibility>\w+)\s+(?P<ndx>\w+)\s+(?P<name>\w+))")


def print_tls_size(fw_elf):
    tls_offset = None
    width = 8

    lines = subprocess.run(["readelf", "-W", "--program-headers", fw_elf],
                           check=True, universal_newlines=True, stdout=subprocess.PIPE
                           ).stdout.strip().split("\n")

    for line in lines:
        match = RE_ELF_SECTION.match(line)
        if match:
            if tls_offset is None and match["type"] == "TLS":
                tls_offset = int(match["virtaddr"], 16)

    header = True
    lines = subprocess.run(["readelf", "-W", "--syms", "--dyn-syms", fw_elf],
                           check=True, universal_newlines=True, stdout=subprocess.PIPE
                           ).stdout.strip().split("\n")
    syms = set()

    for line in lines:
        match = RE_ELF_SYMBOL.match(line)
        if match:
            header = False

            if match["type"] == "TLS":
                syms.add(
                    Symbol(int(match["value"], 16), int(match["size"]), line))
                width = len(match['value'])
            elif tls_offset is not None and (match["type"] == "NOTYPE" and match["bind"] == "GLOBAL"
                                             and match["visibility"] == "DEFAULT"
                                             and match["name"] in set(["_thread_local_start", "_thread_local_end"])
                                             ):
                value = int(match["value"], 16) - tls_offset
                line = ("{1}{2:0{0}x}{3}").format(len(match['value']),
                                                  match["before_value"], value, match["after_value"])
                syms.add(Symbol(value, int(match["size"]), line))

        elif header:
            print(line)

    if syms:
        syms = list(syms)
        syms.sort()
        size = (syms[-1].value + syms[-1].size) - syms[0].value
    else:
        size = 0

    value = syms[0].value
    for sym in syms:
        if sym.value > value:
            print("\t{1:0{0}x} {2:5d} TLS            UNKNOWN".format(
                width, value, sym.value - value))
        print(sym.line)
        value = sym.value + sym.size

    print()
    print(f"Total Thread-Local Storage size: {size} bytes")


def after_fw_elf(source, target, env):
    fw_elf = str(target[0])
    print_tls_size(fw_elf)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Calculate size of Thread-Local Storage")
    parser.add_argument("fw_elf", metavar="ELF", type=str,
                        help="Firmware ELF filename")

    args = parser.parse_args()
    print_tls_size(**vars(args))
elif __name__ == "SCons.Script":
    Import("env")

    env.AddPostAction("${BUILD_DIR}/${PROGNAME}.elf", after_fw_elf)
