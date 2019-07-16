#!/usr/bin/env python
import re
Import("env")

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

app_version = bag.get('app_version')
app_name = bag.get('app_name')
app_hostname = bag.get('app_hostname')

board = env['BOARD']
branch = env['PIOENV']

# build filename, replacing . with _ for the version
env.Replace(PROGNAME="firmware_%s" % branch + "_" + app_version.replace(".", "_"))
