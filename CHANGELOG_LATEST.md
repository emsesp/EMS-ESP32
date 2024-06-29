# Changelog

## [3.7.0]

## **IMPORTANT! BREAKING CHANGES**

- new device WATER shows dhw entities from MM100 and SM100 in dhw setting
- renamed WWC to DHW, always create DHW nests/topics, remove ww prefix from mqtt names [#1634](https://github.com/emsesp/EMS-ESP32/issues/1634)

## Added

- some more entities for dhw with SM100 module
- thermostat second dhw circuit [#1634](https://github.com/emsesp/EMS-ESP32/issues/1634)
- remote thermostat emulation for RC100H, RC200 and FB10 [#1287](https://github.com/emsesp/EMS-ESP32/discussions/1287), [#1602](https://github.com/emsesp/EMS-ESP32/discussions/1602), [#1551](https://github.com/emsesp/EMS-ESP32/discussions/1551)
- heatpump dhw stop temperatures [#1624](https://github.com/emsesp/EMS-ESP32/issues/1624)
- reset history [#1695](https://github.com/emsesp/EMS-ESP32/issues/1695)
- heatpump entities `fan` and `shutdown` [#1690](https://github.com/emsesp/EMS-ESP32/discussions/1690)
- mqtt HA-mode 3 for v3.6 compatible HA entities, set on update v3.6->v3.7
- HP input states [#1723](https://github.com/emsesp/EMS-ESP32/discussions/1723)
- holiday settings for rego 3000  [#1735](https://github.com/emsesp/EMS-ESP32/issues/1735)
- Added scripts for OTA (scripts/upload.py and upload_cli.py) [#1738](https://github.com/emsesp/EMS-ESP32/issues/1738)
- timeout for remote thermostat emulation [#1680](https://github.com/emsesp/EMS-ESP32/discussions/1680), [#1774](https://github.com/emsesp/EMS-ESP32/issues/1774)
- CR120 thermostat as own model() [#1779](https://github.com/emsesp/EMS-ESP32/discussions/1779)
- Modules - external linkable module library [#1778](https://github.com/emsesp/EMS-ESP32/issues/1778)
- Scheduler onChange and Conditions [#1806](https://github.com/emsesp/EMS-ESP32/issues/1806)
- make remote control timeout editable [#1774](https://github.com/emsesp/EMS-ESP32/issues/1774)
- added extra pump characteristics (mode and pressure for EMS+) by @SLTKA [#1802](https://github.com/emsesp/EMS-ESP32/pull/1802)

## Fixed

- remote thermostat emulation for RC200 on Rego2000/3000 thermostats [#1691](https://github.com/emsesp/EMS-ESP32/discussions/1691)
- log shows data for F7/F9 requests

## Changed

- use flag for BC400 compatible thermostats, manage different mode settings
- use factory partition for 16M flash
- store digital out states to nvs
- Refresh UI - moving settings to one location [#1665](https://github.com/emsesp/EMS-ESP32/issues/1665)
- rename DeviceValueTypes, add UINT32 for custom entities
- dynamic register dhw circuits for thermostat
- removed OTA feature [#1738](https://github.com/emsesp/EMS-ESP32/issues/1738)
- added shower min duration [[#1801](https://github.com/emsesp/EMS-ESP32/issues/1801)]
- Include TXT file along with the generated CSV for Device Data export/download
