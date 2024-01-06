# Changelog

## [3.6.5]

## **IMPORTANT! BREAKING CHANGES**

- new device WATER shows dhw entities from MM100 and SM100 in dhw setting

## Added

- some more entities for dhw with SM100 module
- thermostat boost mode and boost time [#1446](https://github.com/emsesp/EMS-ESP32/issues/1446)
- heatpump energy meters [#1463](https://github.com/emsesp/EMS-ESP32/issues/1463)
- heatpump max power [#1475](https://github.com/emsesp/EMS-ESP32/issues/1475)
- checkbox for MQTT-TLS enable [#1474](https://github.com/emsesp/EMS-ESP32/issues/1474)
- added SK (Slovenian) language. Thanks @misa1515
- CPU info [#1497](https://github.com/emsesp/EMS-ESP32/pull/1497)
- Show network hostname in Web UI under Network Status
- Improved HA Discovery so each section (EMS device, Scheduler, Analog, Temperature, Custom, Shower) have their own section

## Fixed

- exhaust temperature for some boilers
- add back boil2hyst [#1477](https://github.com/emsesp/EMS-ESP32/issues/1477)
- subscribed MQTT topics not detecting changes by EMS-ESP [#1494](https://github.com/emsesp/EMS-ESP32/issues/1494)
- changed HA name and grouping to be consistent [#1528](https://github.com/emsesp/EMS-ESP32/issues/1528)
- MQTT autodiscovery in Domoticz not working [#1360](https://github.com/emsesp/EMS-ESP32/issues/1528)

## Changed

- use flag for BC400 compatible thermostats, manage different mode settings
- HA don't set entity_category to Diagnostic/Configuration for EMS entities [#1459](https://github.com/emsesp/EMS-ESP32/discussions/1459)
- Upgraded ArduinoJson to 7.0.0 #1538
