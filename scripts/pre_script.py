#!/usr/bin/env python
from subprocess import call
import os
import re
Import("env")

def build_web():
    print("** Building web...")
    env.Execute(
        "node ./tools/webfilesbuilder/node_modules/gulp/bin/gulp.js --silent --cwd ./tools/webfilesbuilder")

def code_check(source, target, env):
    print("** Starting cppcheck...")
    call(["cppcheck", os.getcwd()+"/.", "--force", "--enable=all"])
    print("\n** Finished cppcheck...\n")
    print("\n** Starting cpplint...")
    call(["cpplint", "--extensions=ino,cpp,h", "--filter=-legal/copyright,-build/include,-whitespace",
          "--linelength=120", "--recursive", "src", "lib/myESP"])
    print("\n** Finished cpplint...")


# build web files
build_web()

# extract application details
bag = {}
exprs = [
    (re.compile(r'^#define APP_VERSION\s+"(\S+)"'), 'app_version'),
    (re.compile(r'^#define APP_NAME\s+"(\S+)"'), 'app_name'),
    (re.compile(r'^#define APP_HOSTNAME\s+"(\S+)"'), 'app_hostname')
]
with open('./src/version.h', 'r') as f:
    for l in f.readlines():
        for expr, var in exprs:
            m = expr.match(l)
            if m and len(m.groups()) > 0:
                bag[var] = m.group(1)

with open('./src/ems-esp.cpp', 'r') as f:
    for l in f.readlines():
        for expr, var in exprs:
            m = expr.match(l)
            if m and len(m.groups()) > 0:
                bag[var] = m.group(1)

app_version = bag.get('app_version')
app_name = bag.get('app_name')
app_hostname = bag.get('app_hostname')
board = env['BOARD']
branch = env['PIOENV']

# build filename, replacing . with _ for the version
env.Replace(PROGNAME=app_name + "-" +
            app_version.replace(".", "_") + "-" + board)
