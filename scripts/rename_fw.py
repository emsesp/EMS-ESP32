import shutil
import re
import os
Import("env")
import hashlib

OUTPUT_DIR = "build{}".format(os.path.sep)

def bin_copy(source, target, env):

    # get the build info
    bag = {}
    exprs = [(re.compile(r'^#define EMSESP_APP_VERSION\s+"(\S+)"'), 'app_version')]
    with open('./src/version.h', 'r') as f:
        for l in f.readlines():
            for expr, var in exprs:
                m = expr.match(l)
                if m and len(m.groups()) > 0:
                    bag[var] = m.group(1)

    app_version = bag.get('app_version')
    platform = "ESP32"
    chip_target = env.get('PIOENV').upper()

    # this breaks the CI so removed
    # flash_size = env["PIOENV"].split('_')[1]

    # print(env.Dump())
    # my_flags = env.ParseFlags(env['BUILD_FLAGS'])
    # defines = {k: v for (k, v) in my_flags.get("CPPDEFINES")}
    # print(my_flags)
    # print(my_flags.get("CPPDEFINES")

    # alternatively take platform from the pio target
    # platform = str(target[0]).split(os.path.sep)[2]

    print("app version: " + app_version)
    print("platform: " + platform)
    print("chip_target: " + chip_target)

    # convert . to _ so Windows doesn't complain
    variant = "EMS-ESP-" + app_version.replace(".", "_") + "-" +  chip_target.replace("CI","ESP32")

    # check if output directories exist and create if necessary
    if not os.path.isdir(OUTPUT_DIR):
        os.mkdir(OUTPUT_DIR)

    for d in ['firmware']:
        if not os.path.isdir("{}{}".format(OUTPUT_DIR, d)):
            os.mkdir("{}{}".format(OUTPUT_DIR, d))

    # create string with location and file names based on variant
    bin_file = "{}firmware{}{}.bin".format(OUTPUT_DIR, os.path.sep, variant)
    md5_file = "{}firmware{}{}.md5".format(OUTPUT_DIR, os.path.sep, variant)

    # check if new target files exist and remove if necessary
    for f in [bin_file]:
        if os.path.isfile(f):
            os.remove(f)

    # check if new target files exist and remove if necessary
    for f in [md5_file]:
        if os.path.isfile(f):
            os.remove(f)

    print("Renaming file to "+bin_file)

    # copy firmware.bin to firmware/<variant>.bin
    shutil.copy(str(target[0]), bin_file)

    with open(bin_file,"rb") as f:
        result = hashlib.md5(f.read())
        print("Calculating MD5: "+result.hexdigest())
        file1 = open(md5_file, 'w')
        file1.write(result.hexdigest())
        file1.close()

env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", [bin_copy])
env.AddPostAction("$BUILD_DIR/${PROGNAME}.md5", [bin_copy])
