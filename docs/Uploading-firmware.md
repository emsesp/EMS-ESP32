
A pre-baked firmware for the common ESP8266 devices are available in the GitHub [releases](https://github.com/proddy/EMS-ESP/releases) which you can upload yourself using the [esptool](https://github.com/espressif/esptool) bootloader.

### via USB

- Install ESPTool by running `pip install esptool`. You'll need [Python](https://www.python.org/downloads/) installed.
- Download the firmware from `https://github.com/proddy/EMS-ESP/releases/latest`. You can use `wget` for this.
- `esptool -p COM5 write_flash 0x00000 *.bin` (use COM* for windows, /dev/tty* for Linux/OSX)

### via OTA (Over The Air)

- Download espota. Eg. `wget https://raw.githubusercontent.com/esp8266/Arduino/master/tools/espota.py`
- Download the firmware from `https://github.com/proddy/EMS-ESP/releases/latest`. You can use `wget` for this.
- `python espota.py -i ems-esp -f *.bin`
