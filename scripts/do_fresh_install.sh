#!/usr/bin/env bash

# Run like:
#  ./scripts/do_fresh_install.sh
#
# Full fresh install of EMS-ESP on an ESP32 16MB board (s_16M_P).
#
# Flash map (partitions/esp32_partition_16M.csv):
#   0x1000    bootloader
#   0x8000    partition table
#   0xe000    otadata (boot_app0.bin selects app0)
#   0x10000   recovery (factory / boot)
#   0x490000  EMS-ESP (app0)
#   0xdf0000  LittleFS
#
# Because otadata points at app0, the device boots EMS-ESP. Recovery stays
# installed in factory for fallback. Do not write recovery at 0x1000 - that
# is the bootloader, and the image overlaps the partition table at 0x8000.

set -euo pipefail

cd "$(dirname "$0")"

# Bold ANSI: 36 cyan, 33 yellow, 35 magenta, 32 green, 31 red.
step() {
  local color=$1
  shift
  printf '\n\033[1;%sm▶ %s\033[0m\n' "$color" "$*"
}
note() {
  printf '  \033[2m%s\033[0m\n' "$*"
}

ESPTOOL=~/.platformio/penv/bin/esptool
PYTHON=~/.platformio/penv/bin/python

PORT=/dev/ttyUSB0
BAUD=${BAUD:-2000000}
# Fast is the installer's default: NVS, otadata, and the first 4 KB of every
# app partition, so nothing old can boot. About a second. ERASE=full still
# wipes the whole chip.
ERASE=${ERASE:-fast}

BUILD_DIR=../.pio/build/s_16M_P
RECOVERY_DIR=../.pio/build/recovery_s_16M_P
FIRMWARE_DIR=../build/firmware

# bin files
BOOT_APP0_BIN=~/.platformio/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin
FIRMWARE_BIN=$BUILD_DIR/firmware.bin
RECOVERY_BIN=$RECOVERY_DIR/firmware.bin
BOOTLOADER_BIN=$BUILD_DIR/bootloader.bin
PARTITIONS_BIN=$BUILD_DIR/partitions.bin
LITTLEFS_BIN=$FIRMWARE_DIR/emsesp_settings_v39.bin

# welcome message
step 33 "Welcome to the EMS-ESP fresh install"
note "This will install the latest EMS-ESP firmware and LittleFS"

# check if platformio is installed
if [[ ! -f $PYTHON ]]; then
  printf '\033[1;31mPlatformIO is not installed - please install it first\033[0m\n' >&2
  exit 1
fi

# check if esptool is installed
if [[ ! -f $ESPTOOL ]]; then
  printf '\033[1;31mESPTool is not installed - please install it first\033[0m\n' >&2
  exit 1
fi

# Build LittleFS from data folder
step 33 "Building LittleFS from data folder"
note "packs data/pre_load.json -> $LITTLEFS_BIN"
$PYTHON build_littlefs.py data $LITTLEFS_BIN

# Check if all files are present
step 33 "Checking images..."
for f in \
  "$BOOTLOADER_BIN" \
  "$PARTITIONS_BIN" \
  "$FIRMWARE_BIN" \
  "$RECOVERY_BIN" \
  "$BOOT_APP0_BIN" \
  "$LITTLEFS_BIN"; do
  if [[ ! -f $f ]]; then
    printf '\033[1;31mMissing %s - build s_16M_P and recovery_s_16M_P first\033[0m\n' "$f" >&2
    exit 1
  fi
  note "ok  $f"
done

# show port, erase and baud rate
step 33 "Port: $PORT, Erase: $ERASE, Baud: $BAUD"

# Fast erase on the 16 MB table: nvs, otadata, nvs1, and the starts of boot,
# app0 and app1. Stay in the bootloader for the write that follows.
if [[ $ERASE == full ]]; then
  step 33 "Full erase - wiping the whole chip"
  $ESPTOOL --chip esp32 --port $PORT --after no-reset erase-flash
else
  step 33 "Fast erase - NVS, otadata, and the first 4 KB of every app"
  note "nothing old can boot; takes about a second"
  before=default-reset
  for spec in \
    "0x9000 0x5000 nvs" \
    "0xe000 0x2000 otadata" \
    "0x10000 0x1000 boot" \
    "0x490000 0x1000 app0" \
    "0x920000 0x1000 app1" \
    "0xdb0000 0x40000 nvs1"; do
    set -- $spec
    printf '  \033[33merase-region %s %s (%s)\033[0m\n' "$1" "$2" "$3"
    $ESPTOOL --chip esp32 --port $PORT --before $before --after no-reset \
      erase-region "$1" "$2"
    before=no-reset
  done
fi

step 33 "Writing flash, then hard-reset into EMS-ESP"
note "0x1000 bootloader  0x8000 partitions  0xe000 otadata"
note "0x10000 recovery   0x490000 firmware  0xdf0000 LittleFS"
$ESPTOOL --chip esp32 --port $PORT --baud $BAUD \
  --before default-reset --after hard-reset \
  write-flash -z --flash-mode dio --flash-freq 80m --flash-size detect \
  0x1000   $BOOTLOADER_BIN \
  0x8000   $PARTITIONS_BIN \
  0xe000   $BOOT_APP0_BIN \
  0x10000  $RECOVERY_BIN \
  0x490000 $FIRMWARE_BIN \
  0xdf0000 $LITTLEFS_BIN

step 32 "Done - gateway should boot EMS-ESP from app0"
