# Changelog

# [3.2.2]

## Added

- Add system commands for syslog level and watch [#98](https://github.com/emsesp/EMS-ESP32/issues/98)
- Added pool data to telegrams 0x494 & 0x495 #102 [#102](https://github.com/emsesp/EMS-ESP32/issues/102) (@Sunbuzz)
- Add RC300 second summermode telegram [#108](https://github.com/emsesp/EMS-ESP32/issues/108)
- Add support for the RC25 thermostat [#106](https://github.com/emsesp/EMS-ESP32/issues/106)

## Fixed

- MQTT reconnecting after WiFi reconnect [#99](https://github.com/emsesp/EMS-ESP32/issues/99)
- Manually Controlling Solar Circuit [#107](https://github.com/emsesp/EMS-ESP32/issues/107)

## Changed

- Syslog BOM only for utf-8 messages [#91](https://github.com/emsesp/EMS-ESP32/issues/91)
- Check for KM200 by device-id 0x48, remove tx-delay[#90](https://github.com/emsesp/EMS-ESP32/issues/90)
