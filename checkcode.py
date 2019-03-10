#!/usr/bin/env python
from subprocess import call
import os
Import("env")

def code_check(source, target, env):
    print("\n** Starting cppcheck...")
    call(["cppcheck", os.getcwd()+"/.", "--force", "--enable=all"])
    print("\n** Finished cppcheck...\n")
    print("\n** Starting cpplint...")
    call(["cpplint", "--extensions=ino,cpp,h", "--filter=-legal/copyright,-build/include,-whitespace",
          "--linelength=120", "--recursive", "src", "lib/myESP"])
    print("\n** Finished cpplint...")

#my_flags = env.ParseFlags(env['BUILD_FLAGS'])
#defines = {k: v for (k, v) in my_flags.get("CPPDEFINES")}
# print defines
# print env.Dump()

#  built in targets: (buildprog, size, upload, program, buildfs, uploadfs, uploadfsota)
env.AddPreAction("buildprog", code_check)
# env.AddPostAction(.....)

# see http://docs.platformio.org/en/latest/projectconf/advanced_scripting.html#before-pre-and-after-post-actions
# env.Replace(PROGNAME="firmware_%s" % defines.get("VERSION"))
# env.Replace(PROGNAME="firmware_%s" % env['BOARD'])
