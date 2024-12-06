# Changelog

For more details go to [docs.emsesp.org](https://docs.emsesp.org/).

## [3.7.2]

## Added

- change enum_heatingtype for remote control [#2268](https://github.com/emsesp/EMS-ESP32/issues/2268)
- system service commands [#2182](https://github.com/emsesp/EMS-ESP32/issues/2282)
- read 0x02A5 for thermostat CT200 [#2277](https://github.com/emsesp/EMS-ESP32/issues/2277)
- Add "duplicate" option to Custom Entities [#2266](https://github.com/emsesp/EMS-ESP32/discussion/2266)
- Mask bits for bool custom entities

## Fixed

- long numbers of custom entities [#2267](https://github.com/emsesp/EMS-ESP32/issues/2267)
- modbus command path to `api/` [#2276](https://github.com/emsesp/EMS-ESP32/issues/2276)
- info command for devices without entity-commands [#2274](https://github.com/emsesp/EMS-ESP32/issues/2274)

## Changed

- show operation in pretty telegram between src and dst [#2263](https://github.com/emsesp/EMS-ESP32/discussions/2263)
- update eModbus to 1.7.2 [#2254](https://github.com/emsesp/EMS-ESP32/issues/2254)
- modbus timeout default to 300 sec, change setting from ms to sec [#2254](https://github.com/emsesp/EMS-ESP32/issues/2254)
- update AsyncTCP to v3.2.14
