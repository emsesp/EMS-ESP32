# Changelog

For more details go to [docs.emsesp.org](https://docs.emsesp.org/).

## [3.7.2]

## Added

- change enum_heatingtype for remote control [#2268](https://github.com/emsesp/EMS-ESP32/issues/2268)
- system service commands [#2182](https://github.com/emsesp/EMS-ESP32/issues/2182)
- read 0x02A5 for thermostat CT200 [#2277](https://github.com/emsesp/EMS-ESP32/issues/2277)
- add "duplicate" option to Custom Entities [#2266](https://github.com/emsesp/EMS-ESP32/discussion/2266)
- mask bits for bool custom entities
- thermostat `reduce threshold` [#2288](https://github.com/emsesp/EMS-ESP32/issues/2288)
- thermostat `absent` [#1957](https://github.com/emsesp/EMS-ESP32/issues/1957)
- CR11 thermostat [#2295](https://github.com/emsesp/EMS-ESP32/issues/2295)
- Show ESP32's CPU temp in Hardware Status
- vacation mode for the CR50 [#2403](https://github.com/emsesp/EMS-ESP32/issues/2403)
- new Console command "set admin password" to set WebUI admin password

## Fixed

- long numbers of custom entities [#2267](https://github.com/emsesp/EMS-ESP32/issues/2267)
- modbus command path to `api/` [#2276](https://github.com/emsesp/EMS-ESP32/issues/2276)
- info command for devices without entity-commands [#2274](https://github.com/emsesp/EMS-ESP32/issues/2274)
- CW100 settings telegram 0x241 [#2290](https://github.com/emsesp/EMS-ESP32/issues/2290)
- modbus signed 8bit values [#2294](https://github.com/emsesp/EMS-ESP32/issues/2294)
- thermostat date [#2313](https://github.com/emsesp/EMS-ESP32/issues/2313)
- Updated unknown compressor stati "enum_hpactivity" [#2311](https://github.com/emsesp/EMS-ESP32/pull/2311)
- Underline Tab headers in WebUI
- console unit tests fixed due to changed shell output

## Changed

- show operation in pretty telegram between src and dst [#2263](https://github.com/emsesp/EMS-ESP32/discussions/2263)
- update eModbus to 1.7.2 [#2254](https://github.com/emsesp/EMS-ESP32/issues/2254)
- modbus timeout default to 300 sec, change setting from ms to sec [#2254](https://github.com/emsesp/EMS-ESP32/issues/2254)
- update AsyncTCP and ESPAsyncWebServer to latest versions
- update Arduino pio platform to 3.10.0 and optimized flash using build flags
- Version checker in WebUI improved
