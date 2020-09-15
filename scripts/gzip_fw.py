Import('env')
import os
import re
import shutil
import gzip

OUTPUT_DIR = "build{}".format(os.path.sep)

def bin_gzip(source, target, env):

    # get the version
    bag = {}
    exprs = [
    (re.compile(r'^#define EMSESP_APP_VERSION\s+"(\S+)"'), 'app_version'),
    ]
    with open('./src/version.h', 'r') as f:
        for l in f.readlines():
            for expr, var in exprs:
                m = expr.match(l)
                if m and len(m.groups()) > 0:
                    bag[var] = m.group(1)

    app_version = bag.get('app_version')

    variant = "EMS-ESP-" +  app_version.replace(".", "_") + "-" + str(target[0]).split(os.path.sep)[2]
        
    # create string with location and file names based on variant
    bin_file = "{}firmware{}{}.bin".format(OUTPUT_DIR, os.path.sep, variant)
    gzip_file = "{}firmware{}{}.bin.gz".format(OUTPUT_DIR, os.path.sep, variant)

    # check if new target files exist and remove if necessary
    if os.path.isfile(gzip_file): os.remove(gzip_file)

    # write gzip firmware file
    with open(bin_file,"rb") as fp:
        with gzip.open(gzip_file, "wb", compresslevel = 9) as f:
            shutil.copyfileobj(fp, f)

env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", [bin_gzip])