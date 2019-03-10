#!/usr/bin/env python
from subprocess import call
import os
Import("env")

def clean(source, target, env):
    print("\n** Starting clean...")
    call(["pio", "run", "-t", "erase"])
    call(["esptool.py", "-p COM6", "write_flash 0x00000",  os.getcwd()+"../firmware/*.bin"])
    print("\n** Finished clean.")

#my_flags = env.ParseFlags(env['BUILD_FLAGS'])
#defines = {k: v for (k, v) in my_flags.get("CPPDEFINES")}
# print defines
# print env.Dump()


#  built in targets: (buildprog, size, upload, program, buildfs, uploadfs, uploadfsota)
env.AddPreAction("buildprog", clean)
# env.AddPostAction(.....)

# see http://docs.platformio.org/en/latest/projectconf/advanced_scripting.html#before-pre-and-after-post-actions
# env.Replace(PROGNAME="firmware_%s" % defines.get("VERSION"))
# env.Replace(PROGNAME="firmware_%s" % env['BOARD'])
