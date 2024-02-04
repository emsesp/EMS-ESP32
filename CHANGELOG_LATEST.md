# Changelog

## [3.6.5]

## **IMPORTANT! BREAKING CHANGES**

## Added

- thermostat boost mode and boost time [#1446](https://github.com/emsesp/EMS-ESP32/issues/1446)
- heatpump energy meters [#1463](https://github.com/emsesp/EMS-ESP32/issues/1463)
- heatpump max power [#1475](https://github.com/emsesp/EMS-ESP32/issues/1475)
- checkbox for MQTT-TLS enable [#1474](https://github.com/emsesp/EMS-ESP32/issues/1474)
- added SK (Slovak) language. Thanks @misa1515
- CPU info [#1497](https://github.com/emsesp/EMS-ESP32/pull/1497)
- Show network hostname in Web UI under Network Status
- Improved HA Discovery so each section (EMS device, Scheduler, Analog, Temperature, Custom, Shower) have their own section
- boiler Bosch C1200W, id 12, [#1536](https://github.com/emsesp/EMS-ESP32/issues/1536)
- mixer MM100 telegram 0x2CC [#1554](https://github.com/emsesp/EMS-ESP32/issues/1554)
- boiler hpSetDiffPressure [#1563](https://github.com/emsesp/EMS-ESP32/issues/1563)
- custom variables [#1423](https://github.com/emsesp/EMS-ESP32/issues/1423)

## Fixed

- exhaust temperature for some boilers
- add back boil2hyst [#1477](https://github.com/emsesp/EMS-ESP32/issues/1477)
- subscribed MQTT topics not detecting changes by EMS-ESP [#1494](https://github.com/emsesp/EMS-ESP32/issues/1494)
- changed HA name and grouping to be consistent [#1528](https://github.com/emsesp/EMS-ESP32/issues/1528)
- MQTT autodiscovery in Domoticz not working [#1360](https://github.com/emsesp/EMS-ESP32/issues/1528)
- dhw comfort for new ems+, [#1495](https://github.com/emsesp/EMS-ESP32/issues/1495)
- added writeable icon to Web's Custom Entity page for each entity shown in the table

## Changed

- HA don't set entity_category to Diagnostic/Configuration for EMS entities [#1459](https://github.com/emsesp/EMS-ESP32/discussions/1459)
- upgraded ArduinoJson to 7.0.0 #1538 and then 7.0.2
- small changes to the API for analog and temperature sensors
