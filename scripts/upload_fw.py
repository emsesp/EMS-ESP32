# for calling dos upload from Window WSL2 Linux, because serial ports are not mapped yet
# example file
Import('env')
from subprocess import call

def upload(source, target, env):
    print("bin file: " + str(target[0]))
    call(["cmd.exe", "/c", "c:\\Users\\paul\\OneDrive\\Desktop\\ems-esp32.bat"])

env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", [upload])
