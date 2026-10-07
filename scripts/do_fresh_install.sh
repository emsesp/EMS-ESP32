#!/usr/bin/env bash
# Full fresh install of EMS-ESP on an ESP32 16MB board (s_16M_P).
#
# Run from the scripts folder:
#   ./do_fresh_install.sh
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
# installed in factory for fallback. Do not write recovery at 0x1000 — that
# is the bootloader, and the image overlaps the partition table at 0x8000.

set -euo pipefail

ESPTOOL=~/.platformio/penv/bin/esptool
PORT=/dev/ttyUSB0
BAUD=${BAUD:-2000000}

BUILD_BIN=../.pio/build/s_16M_P
RECOVERY_BIN=../.pio/build/recovery_s_16M_P
BOOT_APP0=~/.platformio/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin

for f in \
  "$BUILD_BIN/bootloader.bin" \
  "$BUILD_BIN/partitions.bin" \
  "$BUILD_BIN/firmware.bin" \
  "$RECOVERY_BIN/firmware.bin" \
  "$BOOT_APP0"; do
  if [[ ! -f $f ]]; then
    echo "Missing $f — build s_16M_P and recovery_s_16M_P first" >&2
    exit 1
  fi
done

# full erase, stay in download mode
$ESPTOOL --chip esp32 --port $PORT --after no-reset erase-flash

# create the filesystem, in case it's needed
~/.platformio/penv/bin/python build_littlefs.py data $BUILD_BIN/littlefs.bin

# bootloader, partitions, otadata, recovery, app, filesystem; then boot
$ESPTOOL --chip esp32 --port $PORT --baud $BAUD \
  --before default-reset --after hard-reset \
  write-flash -z --flash-mode dio --flash-freq 80m --flash-size detect \
  0x1000   $BUILD_BIN/bootloader.bin \
  0x8000   $BUILD_BIN/partitions.bin \
  0xe000   $BOOT_APP0 \
  0x10000  $RECOVERY_BIN/firmware.bin \
  0x490000 $BUILD_BIN/firmware.bin \
  0xdf0000 $BUILD_BIN/littlefs.bin
