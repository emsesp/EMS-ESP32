# Changelog

## [3.6.3]

## **IMPORTANT! BREAKING CHANGES**

## Added

- humidity for ventilation devices
- telegrams for RC100H, hc2 (seen on discord)
- names for BC400, GB192i, read temperatures for low loss header and heatblock [#1317](https://github.com/emsesp/EMS-ESP32/discussions/1317)
- option for `forceheatingoff` [#1262](https://github.com/emsesp/EMS-ESP32/issues/1262)
- shower_data MQTT payload contains the timestamp [#1329](https://github.com/emsesp/EMS-ESP32/issues/1329)

## Fixed

- fixed helper text in Web Device Entity dialog box for numerical ranges

## Changed

- update to arduino 2.0.14 / idf 4.4.6
- small changes for arduino 3.0.0 / idf 5.1 compatibility
