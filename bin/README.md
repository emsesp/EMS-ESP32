# How to manually upload the firmware to the ESP32 using esptool

Take the firmware either from the `.pio/build/<target>/firmware.bin` folder if you manually built EMS-ESP using platformio, or alternetively download the .bin file directlyfrom the GitHub Releases page ([dev releases](https://github.com/emsesp/EMS-ESP32/releases) or [stable releases](https://github.com/emsesp/EMS-ESP32/releases)).

IMPORTANT: See the https://docs.emsesp.org/Installing#choosing-the-right-firmware-version to understand which firmware to use for your board.

## Firmware locations

Dev releases: https://github.com/emsesp/EMS-ESP32/releases
Stable releases: https://github.com/emsesp/EMS-ESP32/releases

## s_4M (ESP with 4MB flash) - e.g. BBQKees older S32 and E32 models

`esptool --chip esp32 --port <COM PORT> --baud 460800 --before default-reset --after hard-reset write-flash -z --flash-mode dio --flash-freq 40m --flash-size detect 0x1000
 s_4M/bootloader.bin 0x8000 s_4M/partitions.bin 0xe000 boot_app0.bin 0x10000 firmware.bin`

## s_16M (ESP with 16MB flash) - e.g. BBQKees later S32 V2 models

`esptool --chip esp32 --port <COM PORT> --baud 460800 --before default-reset --after hard-reset write-flash -z --flash-mode dio --flash-freq 40m --flash-size detect 0x1000
 s_16M/bootloader.bin 0x8000 s_16M/partitions.bin 0xe000 boot_app0.bin 0x490000 firmware.bin`


## s_16M_P (ESP with 16MB flash and PSRAM) - e.g. BBQKees E32V2 models

`esptool --chip esp32 --port <COM PORT> --baud 460800 --before default-reset --after hard-reset write-flash -z --flash-mode dio --flash-freq 80m --flash-size detect 0x1000
 s_16M_P/bootloader.bin 0x8000 s_16M_P/partitions.bin 0xe000 boot_app0.bin 0x490000 firmware.bin`


## s3_16M_P (ESP32-S3 with 16MB flash and PSRAM) - e.g. BBQKees S3 models

`esptool --chip esp32s3 --port <COM PORT> --baud 460800 --before default-reset --after hard-reset write-flash -z --flash-mode dio --flash-freq 80m --flash-size detect 0x0000
 s3_16M_P/bootloader.bin 0x8000 s3_16M_P/partitions.bin 0xe000 boot_app0.bin 0x490000 firmware.bin`

## s3_32M_P, s2_4M_P, c3_mini_4M, c3_miniv1_4M, c6_xiao_4M

Please use platformio to build the firmware for these boards.