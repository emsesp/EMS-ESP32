import os

Import("env")


def refresh_module():
    print("Fetching latest module for Native target...")
    os.system(
        "pio pkg install -f -s -e native -l file://../../modules/EMS-ESP-Modules")


if not (env.IsCleanTarget()):
    refresh_module()
