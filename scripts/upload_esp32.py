#!/usr/bin/env python
import subprocess
import os, argparse

print("\n** Starting upload...")

ap = argparse.ArgumentParser()
ap.add_argument("-p", "--port", required=True, help="port")
args = vars(ap.parse_args())

# esptool.py --chip esp32 --port "COM4" --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect 0x1000 bootloader_dio_40m.bin 0x8000 partitions.bin 0xe000 boot_app0.bin 0x10000 EMS-ESP-dev-esp32.bin

subprocess.call(["esptool.py", "--chip esp32", "-p", args['port'], "--baud", "921600", "--before", "default_reset", "--after", "hard_reset", "write_flash", "-z", "--flash_mode", "dio", "--flash_freq", "40m","--flash_size", "detect", "0x1000", "bootloader_dio_40m.bin", "0x8000", "partitions.bin","0xe000", "boot_app0.bin", "0x10000", "EMS-ESP-dev-esp32.bin"])

print("\n** Finished upload.")
