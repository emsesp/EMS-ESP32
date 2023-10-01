# Changelog

## [3.6.2]

## **IMPORTANT! BREAKING CHANGES**

## Added

- Power entities
- Optional input of BSSID for AP connection
- Return empty json if no entries in scheduler/custom/analogsnesor/temperaturesensor

## Fixed

- Wifi full scan to get strongest AP
- Add missing dhw tags
- Sending a dash/- to the Reset command doesn't return an error [#1308](https://github.com/emsesp/EMS-ESP32/discussions/1308)

## Changed

- MQTT queue max 300 messages, check heap and maxAlloc
- API call commands are logged as WARN in the log
- Reset Command renamed to 'reset' in lowercase in EN
