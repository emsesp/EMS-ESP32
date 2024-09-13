# Changelog

## [3.7.0]

## **IMPORTANT! BREAKING CHANGES with 3.6.5**

- new device WATER shows dhw entities from MM100 and SM100 in dhw setting
- renamed WWC to DHW, always create DHW nests/topics, remove ww prefix from mqtt names [#1634](https://github.com/emsesp/EMS-ESP32/issues/1634). To preserve current value of dhw energy (nrgww), follow ([#1938]<https://github.com/emsesp/EMS-ESP32/issues/1938>)
- change temperaturesensor id to underscore
- system/info API command has it's JSON keys and names changed to camelCase

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
- modules - external linkable module library [#1778](https://github.com/emsesp/EMS-ESP32/issues/1778)
- scheduler onChange and Conditions [#1806](https://github.com/emsesp/EMS-ESP32/issues/1806)
- make remote control timeout editable [#1774](https://github.com/emsesp/EMS-ESP32/issues/1774)
- added extra pump characteristics (mode and pressure for EMS+) by @SLTKA [#1802](https://github.com/emsesp/EMS-ESP32/pull/1802)
- allow device name to be customized [#1174](https://github.com/emsesp/EMS-ESP32/issues/1174)
- Modbus support by @mheyse [#1744](https://github.com/emsesp/EMS-ESP32/issues/1744)
- System Message command [#1854](https://github.com/emsesp/EMS-ESP32/issues/1854)
- scheduler can use web get/post for values and commands [#1806](https://github.com/emsesp/EMS-ESP32/issues/1806)
- RT800 remote emulation [#1867](https://github.com/emsesp/EMS-ESP32/issues/1867)
- RC310 cooling parameters [#1857](https://github.com/emsesp/EMS-ESP32/issues/1857)
- command `api/device/entities` [#1897](https://github.com/emsesp/EMS-ESP32/issues/1897)
- switchprogmode [#1903]<https://github.com/emsesp/EMS-ESP32/discussions/1903>
- autodetect and download firmware upgrades via the WebUI
- command 'show log' that lists out the current weblog buffer, showing last messages.
- default web log buffer to 25 lines for ESP32s with no PSRAM
- Try and determine correct board profile if none is set
- auto Scroll in WebLog UI - reduced delay so incoming logs are faster

## Fixed

- remote thermostat emulation for RC200 on Rego2000/3000 thermostats [#1691](https://github.com/emsesp/EMS-ESP32/discussions/1691)
- log shows data for F7/F9 requests
- Detection of LittleFS for factory setting wasn't working
- Check for bad GPIOs with Ethernet before the ethernet is initialized

## Changed

- use flag for BC400 compatible thermostats, manage different mode settings
- use factory partition for 16M flash
- store digital out states to nvs
- Refresh UI - moving settings to one location [#1665](https://github.com/emsesp/EMS-ESP32/issues/1665)
- rename DeviceValueTypes, add UINT32 for custom entities
- dynamic register dhw circuits for thermostat
- removed OTA feature [#1738](https://github.com/emsesp/EMS-ESP32/issues/1738)
- added shower min duration [#1801](https://github.com/emsesp/EMS-ESP32/issues/1801)
- Include TXT file along with the generated CSV for Device Data export/download
- thermostat/remotetemp as command [#1835](https://github.com/emsesp/EMS-ESP32/discussions/1835)
- temperaturesensor id notation with underscore [#1794](https://github.com/emsesp/EMS-ESP32/discussions/1794)
- Change key-names in JSON to be compliant and consistent [#1860](https://github.com/emsesp/EMS-ESP32/issues/1860)
- Updates to webUI [#1920](https://github.com/emsesp/EMS-ESP32/issues/1920)
- Correct firmware naming #1933 [#1933](https://github.com/emsesp/EMS-ESP32/issues/1933)
- Don't start Serial console if not connected to a Serial port. Will initiate manually after a CTRL-C  
- WebLog UI matches color schema of the terminal console correctly
