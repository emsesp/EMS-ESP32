Import('env', "projenv")
import os
import re
import shutil
import gzip

OUTPUT_DIR = "build{}".format(os.path.sep)

def bin_copy(source, target, env):

    # get the EMS-ESP version
    bag = {}
    exprs = [(re.compile(r'^#define EMSESP_APP_VERSION\s+"(\S+)"'), 'app_version')]
    with open('./src/version.h', 'r') as f:
        for l in f.readlines():
            for expr, var in exprs:
                m = expr.match(l)
                if m and len(m.groups()) > 0:
                    bag[var] = m.group(1)

    # esp8266 or esp32
    platform = "esp" + env['PIOPLATFORM'].strip("espressif")

    # if using the pio build directory, use str(target[0]).split(os.path.sep)[2]
    variant = "EMS-ESP-" + bag.get('app_version').replace(".", "_") + "-" + platform
    
    # create string with location and file names based on variant
    bin_file = "{}firmware{}{}.bin".format(OUTPUT_DIR, os.path.sep, variant)
    gzip_file = "{}firmware{}{}.bin.gz".format(OUTPUT_DIR, os.path.sep, variant)

    # check if firmware directory and target subdirectories (esp8266/esp32) exist and create if necessary
    if not os.path.isdir(OUTPUT_DIR):
        os.mkdir(OUTPUT_DIR)
    for d in ['firmware']:
        if not os.path.isdir("{}{}".format(OUTPUT_DIR, d)):
            os.mkdir("{}{}".format(OUTPUT_DIR, d))

    # copy firmware.bin to new name and in the build/firmware folder, delete if already exists
    if os.path.isfile(bin_file): os.remove(bin_file)
    shutil.copy(str(target[0]), bin_file)

    # create the gzip'd version
    if os.path.isfile(gzip_file): os.remove(gzip_file)
    with open(bin_file, "rb") as fp:
         with gzip.open(gzip_file, "wb", compresslevel = 9) as f:
             shutil.copyfileobj(fp, f)

    print("Built firmwares: "+ bin_file + ", " + gzip_file)

env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", [bin_copy])
