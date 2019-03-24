#!/usr/bin/env python
from subprocess import call
import os
Import("env")

def clean(source, target, env):
    print("\n** Starting clean...")
    call(["pio", "run", "-t", "erase"])
    call(["esptool.py", "-p COM6", "write_flash 0x00000",  os.getcwd()+"../firmware/*.bin"])
    print("\n** Finished clean.")

#  built in targets: (buildprog, size, upload, program, buildfs, uploadfs, uploadfsota)
env.AddPreAction("buildprog", clean)

