# Changelog

# [3.4.0]

## Added

## Fixed

- lastcode broke MQTT JSON structure [#228]
- overlapping while reading sequence of EMS1.0 telegrams
- redundant telegram readings (because of offset overflow)

## Changed

- Refactor WebUI to use ReactHooks and new Theme, plus layout changes

## **BREAKING CHANGES**

- Settings: order of Boolean Format has changed in Application Settings
- Dallas Format removed. Use the name to give each sensor an alias
