# Changelog

For more details go to [docs.emsesp.org](https://docs.emsesp.org/).

## [3.7.3]

## Added

- analogsensor types: NTC and RGB-Led
- Flag for HMC310 [#2465](https://github.com/emsesp/EMS-ESP32/issues/2465)
- boiler auxheatersource [#2489](https://github.com/emsesp/EMS-ESP32/discussions/2489)
- thermostat last error for RC100/300 [#2501](https://github.com/emsesp/EMS-ESP32/issues/2501)
- boiler 0xC6 telegram [#1963](https://github.com/emsesp/EMS-ESP32/issues/1963)
- CS6800i changes [#2448](https://github.com/emsesp/EMS-ESP32/issues/2448), [#2449](https://github.com/emsesp/EMS-ESP32/issues/2449)
- charging pump [#2544](https://github.com/emsesp/EMS-ESP32/issues/2544)
- hybrid CSH5800iG [#2569](https://github.com/emsesp/EMS-ESP32/issues/2569)
- add EMS Device details to Home Assistant MQTT Discovery
- disinfection command [#2601](https://github.com/emsesp/EMS-ESP32/issues/2601)
- added new board profile for upcoming BBQKees E32V2.2
- set differential pressure entity in Mixer device
- set set climate action cooling/heating in HA
- Internal sensors of E32V2_2
- FW200 display options
- CR11 mode settings OFF/MANUAL depends on selTemp
- Fuse settings for BBQKees boards

## Fixed

- dhw/switchtime [#2490](https://github.com/emsesp/EMS-ESP32/issues/2490)
- switch to secure mqtt [#2492](https://github.com/emsesp/EMS-ESP32/issues/2492)
- update link buttons [#2497](https://github.com/emsesp/EMS-ESP32/issues/2497)
- refresh scheduler states [#2502](https://github.com/emsesp/EMS-ESP32/discussions/2502)
- also rebuild HA config on mqtt connect for scheduler, custom and shower
- FB100 controls the hc, not the master [#2510](https://github.com/emsesp/EMS-ESP32/issues/2510)
- IPM DHW module, [#2524](https://github.com/emsesp/EMS-ESP32/issues/2524)
- charge optimization [#2543](https://github.com/emsesp/EMS-ESP32/issues/2543)
- shower active state retained, shows correctly in HA
- MQTT Command Topic with slashes [#2571](https://github.com/emsesp/EMS-ESP32/issues/2571)
- Add pulsed water meter input to V1.3 gateway with Lilygo S3 [#2550](https://github.com/emsesp/EMS-ESP32/issues/2550)
- fix missing long 10-second press of Button to perform a factory reset
- fix wwMaxPower on Junkers ZBS14 [#2609](https://github.com/emsesp/EMS-ESP32/issues/2609)

## Changed

- show console log with ISO date/time [#2533](https://github.com/emsesp/EMS-ESP32/discussions/2533)
- remove ESP32 CPU temperature
- updated core libraries like AsyncTCP, AsyncWebServer and Modbus
