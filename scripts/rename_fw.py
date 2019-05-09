#!/usr/bin/env python
from subprocess import call
import os
Import("env")

# see http://docs.platformio.org/en/latest/projectconf/advanced_scripting.html#before-pre-and-after-post-actions
# env.Replace(PROGNAME="firmware_%s" % defines.get("VERSION"))
env.Replace(PROGNAME="firmware_%s" % env['BOARD'])
