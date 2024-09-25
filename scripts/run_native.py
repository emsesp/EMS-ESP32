import shutil
import re
import os
Import("env")

OUTPUT_DIR = "build{}".format(os.path.sep)


def move_file(source, target, env):

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
    platform = "native"

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

    # TODO do we need to add a .exe extension for windows? - need to test
    variant = "native"

    # check if output directories exist and create if necessary
    if not os.path.isdir(OUTPUT_DIR):
        os.mkdir(OUTPUT_DIR)

    # create string with location and file names based on variant
    bin_file = "{}firmware{}{}".format(OUTPUT_DIR, os.path.sep, variant)

    # check if new target files exist and remove if necessary
    for f in [bin_file]:
        if os.path.isfile(f):
            os.remove(f)

    print("Renaming file to "+bin_file)

    shutil.copy(str(target[0]), bin_file)

    print("Executing file")
    os.system(bin_file)


env.AddPostAction("$BUILD_DIR/${PROGNAME}", [move_file])
