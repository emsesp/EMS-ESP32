import shutil
import re
import os
Import("env")
import hashlib

OUTPUT_DIR = "build{}".format(os.path.sep)

def bin_copy(source, target, env):

    # get the application version from version.h
    bag = {}
    exprs = [(re.compile(r'^#define EMSESP_APP_VERSION\s+"(\S+)"'), 'app_version')]
    with open('./src/version.h', 'r') as f:
        for l in f.readlines():
            for expr, var in exprs:
                m = expr.match(l)
                if m and len(m.groups()) > 0:
                    bag[var] = m.group(1)

    app_version = bag.get('app_version')
    
    # get the chip type, in uppercase
    mcu = env.get('BOARD_MCU').upper()
    # alternatively take platform from the pio target
    # platform = str(target[0]).split(os.path.sep)[2]
    
    # work out the flash memory from the PIO env name (sloppy but works)
    # unfortunately the board_upload.flash_size is not passed down
    flash_mem = "4MB"
    pio_env = env.get('PIOENV').upper()
    parts = pio_env.split('_')
    # if it ends with a _P skip (we use this to denote PSRAM)
    if parts[-1].endswith("P"):
        index = -2
    else:
        index = -1
        
    # if doesn't have an M at the end 
    if parts[index].endswith("M"):
        flash_mem = parts[index] + "B"
    
    # print(env.Dump())
    
    # my_flags = env.ParseFlags(env['BUILD_FLAGS'])
    # defines = {k: v for (k, v) in my_flags.get("CPPDEFINES")}
    
    # find if BOARD_HAS_PSRAM is in the cppdefines
    cppdefines = env.get("CPPDEFINES")
    if 'BOARD_HAS_PSRAM' in cppdefines:
        psram = True
    else:
        psram = False
        
    print("*********************************************")
    print("EMS-ESP version: " + app_version)
    
    # show psram as Yes or No
    psram_status = "Yes" if psram else "No"
    print("Has PSRAM: " + psram_status)
    print("MCU: "+str(mcu))
    print("Flash Mem: " + flash_mem)

    # convert . to _ so Windows doesn't complain
    # Format is EMS-ESP-<version>-<mcu>-<flash> with + at the end if it has PSRAM
    variant = "EMS-ESP-" + app_version.replace(".", "_") + "-" +  mcu + "-" + flash_mem + ("+" if psram else "")

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

    print("Filename: "+bin_file)

    # copy firmware.bin to firmware/<variant>.bin
    shutil.copy(str(target[0]), bin_file)

    with open(bin_file,"rb") as f:
        result = hashlib.md5(f.read())
        print("MD5: "+result.hexdigest())
        file1 = open(md5_file, 'w')
        file1.write(result.hexdigest())
        file1.close()
        
    print("*********************************************")

env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", [bin_copy])
env.AddPostAction("$BUILD_DIR/${PROGNAME}.md5", [bin_copy])
