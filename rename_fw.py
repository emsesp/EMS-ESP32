#!/usr/bin/env python
from subprocess import call
import os
Import("env")

#my_flags = env.ParseFlags(env['BUILD_FLAGS'])
#defines = {k: v for (k, v) in my_flags.get("CPPDEFINES")}
# print defines
# print env.Dump()

# see http://docs.platformio.org/en/latest/projectconf/advanced_scripting.html#before-pre-and-after-post-actions
# env.Replace(PROGNAME="firmware_%s" % defines.get("VERSION"))
env.Replace(PROGNAME="firmware_%s" % env['BOARD'])
