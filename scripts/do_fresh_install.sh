
ESPTOOL=~/.platformio/penv/bin/esptool
PORT=/dev/ttyUSB0
BUILD=../.pio/build/s_16M_P
BOOT_APP0=~/.platformio/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin

# erase
$ESPTOOL --chip esp32 --port $PORT --after no-reset erase-flash

# create the filesystem
~/.platformio/penv/bin/python build_littlefs.py data $BUILD/littlefs.bin

# upload filesystem (spiffs partition @ 0xdf0000), leave halted so it doesn't boot yet
$ESPTOOL --chip esp32 --port $PORT --baud 460800 \
  --before default-reset --after no-reset \
  write-flash -z --flash-mode dio --flash-freq 80m --flash-size detect \
  0xdf0000 $BUILD/littlefs.bin

# flash bootloader + partition table + app (app0 @ 0x490000), then boot
$ESPTOOL --chip esp32 --port $PORT --baud 460800 \
  --before default-reset --after hard-reset \
  write-flash -z --flash-mode dio --flash-freq 80m --flash-size detect \
  0x1000   $BUILD/bootloader.bin \
  0x8000   $BUILD/partitions.bin \
  0xe000   $BOOT_APP0 \
  0x490000 $BUILD/firmware.bin
