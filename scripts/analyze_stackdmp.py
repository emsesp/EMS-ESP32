#!/usr/bin/env python
from subprocess import call
import os

# example stackdmp.txt would contain text like below copied & pasted from a 'crash dump' command:

# >>>stack>>>
# 3fffff20: 3fff32f0 00000003 3fff3028 402101b2
# 3fffff30: 3fffdad0 3fff3280 0000000d 402148aa
# 3fffff40: 3fffdad0 3fff3280 3fff326c 3fff32f0
# 3fffff50: 0000000d 3fff326c 3fff3028 402103bd
# 3fffff60: 0000000d 3fff34cc 40211de4 3fff34cc
# 3fffff70: 3fff3028 3fff14c4 3fff301c 3fff34cc
# 3fffff80: 3fffdad0 3fff14c4 3fff3028 40210493
# 3fffff90: 3fffdad0 00000000 3fff14c4 4020a738
# 3fffffa0: 3fffdad0 00000000 3fff349c 40211e90
# 3fffffb0: feefeffe feefeffe 3ffe8558 40100b01
# <<<stack<<<

call(['python', 'scripts/decoder.py ', '-s', '-e', os.getcwd()+"/.pio/build/debug/firmware_debug_1_9_1b3.elf", 'scripts/stackdmp.txt'])

# example for linux:
# % cd EMS-ESP
# % python scripts/decoder_linux.py -s -e .pio/build/debug/firmware_d1_mini.elf scripts/stackdmp.txt
