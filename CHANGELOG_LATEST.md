# Changelog

## [3.6.3]

## **IMPORTANT! BREAKING CHANGES**

## Added

- humidity for ventilation devices
- telegrams for RC100H, hc2, etc. (seen on discord, not tested)
- names for BC400, GB192i, read temperatures for low loss header and heatblock [#1317](https://github.com/emsesp/EMS-ESP32/discussions/1317)
- option for `forceheatingoff` [#1262](https://github.com/emsesp/EMS-ESP32/issues/1262)
- remote thermostat emulation for RC3xx
- publish time for shower

## Fixed

- remove command `remoteseltemp`, thermostat accept it only from remote thermostat
- shower_data MQTT payload contains the timestamp [#1329](https://github.com/emsesp/EMS-ESP32/issues/1329)
- fixed helper text in Web Device Entity dialog box for numerical ranges

## Changed

- update to platform 6.4.0, arduino 2.0.14 / idf 4.4.6
- small changes for arduino 3.0.0 / idf 5.1 compatibility (not backward compatible to platform 6.3.2 and before)
