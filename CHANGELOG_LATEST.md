# Changelog

## [3.6.3]

## **IMPORTANT! BREAKING CHANGES**

Writeable Text entities have moved from type `sensor` to `text` in Home Assistant to make them also editable within an HA dashboard. Examples are `datetime`, `holidays`, `switchtime`, `vacations`, `maintenancedate`...). You will need to manually remove any old discovery topics from your MQTT broker using an application like MQTT Explorer.

## Added

- humidity for ventilation devices
- telegrams for RC100H, hc2 (seen on discord)
- names for BC400, GB192i, read temperatures for low loss header and heatblock [#1317](https://github.com/emsesp/EMS-ESP32/discussions/1317)
- option for `forceheatingoff` [#1262](https://github.com/emsesp/EMS-ESP32/issues/1262)
- shower_data MQTT payload contains the timestamp [#1329](https://github.com/emsesp/EMS-ESP32/issues/1329)
- HA discovery for writeable text entities [#1337](https://github.com/emsesp/EMS-ESP32/pull/1377)
- Added 'system values' command that dumps all the EMS device values, plus sensors and any custom entities

## Fixed

- fixed helper text in Web Device Entity dialog box for numerical ranges
- MQTT base with paths not working in HA [#1393](https://github.com/emsesp/EMS-ESP32/issues/1393)

## Changed

- update to arduino 2.0.14 / idf 4.4.6
- small changes for arduino 3.0.0 / idf 5.1 compatibility
