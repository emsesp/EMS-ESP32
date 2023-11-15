# Changelog

## [3.6.3]

## **IMPORTANT! BREAKING CHANGES**

Writeable Text entities have moved from type `sensor` to `text` in Home Assistant to make them also editable within an HA dashboard. Examples are `datetime`, `holidays`, `switchtime`, `vacations`, `maintenancedate`... You will need to manually remove any old discovery topics from your MQTT broker using an application like MQTT Explorer.

## Added

- humidity for ventilation devices
- telegrams for RC100H, hc2, etc. (seen on discord, not tested)
- names for BC400, GB192i.2, read temperatures for low loss header and heatblock [#1317](https://github.com/emsesp/EMS-ESP32/discussions/1317)
- option for `forceheatingoff` [#1262](https://github.com/emsesp/EMS-ESP32/issues/1262)
- remote thermostat emulation RC100H for RC3xx [#1278](https://github.com/emsesp/EMS-ESP32/discussions/1278)
- shower_data MQTT payload contains the timestamp [#1329](https://github.com/emsesp/EMS-ESP32/issues/1329)
- HA discovery for writeable text entities [#1337](https://github.com/emsesp/EMS-ESP32/pull/1377)
- autodetect board_profile, store in nvs, add telnet command option, add E32V2
- heat pump high res energy counters [#1348, #1349. #1350](https://github.com/emsesp/EMS-ESP32/issues/1348)
- optional bssid in network settings
- extension module EM100 [#1315](https://github.com/emsesp/EMS-ESP32/discussions/1315)
- digital_out with new options for polarity and startup state
- added 'system allvalues' command that dumps all the EMS device values, plus sensors and any custom entities

## Fixed

- remove command `remoteseltemp`, thermostat accept it only from remote thermostat
- shower_data MQTT payload contains the timestamp [#1329](https://github.com/emsesp/EMS-ESP32/issues/1329)
- fixed helper text in Web Device Entity dialog box for numerical ranges
- MQTT base with paths not working in HA [#1393](https://github.com/emsesp/EMS-ESP32/issues/1393)

## Changed

- update to platform 6.4.0, arduino 2.0.14 / idf 4.4.6
- small changes for arduino 3.0.0 / idf 5.1 compatibility (not backward compatible to platform 6.3.2 and before)
- AP start after 10 sec, stay until station/eth connected
- tested wifi-all-channel-scan (3.6.3-dev4 a-e), removed again because of connect issues
- mqtt disconnect stops queue
