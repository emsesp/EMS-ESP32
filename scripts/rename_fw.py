#!/usr/bin/env python
Import("env")

# see http://docs.platformio.org/en/latest/projectconf/advanced_scripting.html#before-pre-and-after-post-actions
# env.Replace(PROGNAME="firmware_%s" % defines.get("VERSION"))
# print env.Dump()
env.Replace(PROGNAME="firmware_%s" % env['BOARD'] + "_" + env['PIOENV'])
