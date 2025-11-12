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
- set set climate action cooling/heating in HA [#2583](https://github.com/emsesp/EMS-ESP32/issues/2583)
- Internal sensors of E32V2_2
- FW200 display options [#2610](https://github.com/emsesp/EMS-ESP32/discussions/2610)
- CR11 mode settings OFF/MANUAL depends on selTemp [#2437](https://github.com/emsesp/EMS-ESP32/issues/2437)
- implemented eFuse settings for BBQKees boards to store model type and ESP chipset
- Analogsensors for pulse output [#2624](https://github.com/emsesp/EMS-ESP32/discussions/2624)
- Analogsensors frequency input [#2631](https://github.com/emsesp/EMS-ESP32/discussions/2631)
- SRC plus thermostats [#2636](https://github.com/emsesp/EMS-ESP32/issues/2636)
- Greenstar 2000 [#2645](https://github.com/emsesp/EMS-ESP32/issues/2645)
- RC3xx `dhw modetype` [#2659](https://github.com/emsesp/EMS-ESP32/discussions/2659)
- new boiler entities VR0,VR1, compressor speed [#2669](https://github.com/emsesp/EMS-ESP32/issues/2669)
- solar temperature TS16 [#2690](https://github.com/emsesp/EMS-ESP32/issues/2690)
- pumpmode enum for HT3 boilers, add commands for manual defrost, chimneysweeper [#2727](https://github.com/emsesp/EMS-ESP32/issues/2727)
- pid settings [#2735](https://github.com/emsesp/EMS-ESP32/issues/2735)

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
- ventilation bypass state from telegram 0x55C [#1197](https://github.com/emsesp/EMS-ESP32/issues/1197)
- set selflowtemp for ems+ boilers [#2641](https://github.com/emsesp/EMS-ESP32/discussions/2641)
- syslog timestamp [#2704](https://github.com/emsesp/EMS-ESP32/issues/2704)
- fixed FS format command [#2720](https://github.com/emsesp/EMS-ESP32/discussions/2720)
- dhw priority setting to boiler and mixer, telegrams 0x2CC, 0x2CD, etc.

## Changed

- show console log with ISO date/time [#2533](https://github.com/emsesp/EMS-ESP32/discussions/2533)
- remove ESP32 CPU temperature
- updated core libraries like AsyncTCP, AsyncWebServer and Modbus
- remove command `scan deep`
- ignore repeated `forceheatingoff` commands [#2641](https://github.com/emsesp/EMS-ESP32/discussions/2641)
- optimized web for better performance by adding lazy loading and caching
- internal system analog sensors (core_voltage, supply_voltage and gateway_temperature) cannot be accidentally removed
- double click button reconnects EMS-ESP to AP
- place system message command in side scheduler loop to reduce stack memory usage by 2KB
- syslog mark interval set to 1 hour
