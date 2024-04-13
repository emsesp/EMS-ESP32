# Changelog

## [3.7.0]

## **IMPORTANT! BREAKING CHANGES**

- new device WATER shows dhw entities from MM100 and SM100 in dhw setting
- rename WWC to DHW, always create DHW nests/topics, remove ww prefix from mqtt names [#1634](https://github.com/emsesp/EMS-ESP32/issues/1634)

## Added

- some more entities for dhw with SM100 module
- thermostat second dhw circuit [#1634](https://github.com/emsesp/EMS-ESP32/issues/1634)
- remote thermostat emulation for RC100H, RC200 and FB10 [#1287](https://github.com/emsesp/EMS-ESP32/discussions/1287), [#1602](https://github.com/emsesp/EMS-ESP32/discussions/1602), [#1551](https://github.com/emsesp/EMS-ESP32/discussions/1551)
- heatpump dhw stop temperatures [#1624](https://github.com/emsesp/EMS-ESP32/issues/1624)
- reset history [#1695](https://github.com/emsesp/EMS-ESP32/issues/1695)

## Fixed

- remote thermostat emulation for RC200 on Rego2000/3000 thermostats [#1691](https://github.com/emsesp/EMS-ESP32/discussions/1691)
- log shows data for F7/F9 requests

## Changed

- use flag for BC400 compatible thermostats, manage different mode settings
- use factory partition for 16M flash
- store digital out states to nvs
- Refresh UI - moving settings to one location [#1665](https://github.com/emsesp/EMS-ESP32/issues/1665)
