# Changelog

## [3.6.5]

## **IMPORTANT! BREAKING CHANGES**

- new device WATER shows dhw entities from MM100 and SM100 in dhw setting

## Added

- some more entities for dhw with SM100 module
- thermostat boost mode and boost time
- heatpump energy meters

## Fixed

- exhaust temperature for some boilers

## Changed

- use flag for BC400 compatible thermostats, manage different mode settings
- HA don't set entity_category to Diagnostic/Configuration for EMS entities [#1459](https://github.com/emsesp/EMS-ESP32/discussions/1459)
