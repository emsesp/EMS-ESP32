# Changelog

# [3.4.0]

## Added

- Auto reload of WebUI after successful firmware upload [#178](https://github.com/emsesp/EMS-ESP32/issues/178)
- Customization Service, including exclusion lists for device entities [#206](https://github.com/emsesp/EMS-ESP32/issues/206)
- Option to disable Telnet Console
- Added Hide SSID, Max Clients and Preferred Channel to Access Point

## Fixed

- lastcode broke MQTT JSON structure [#228](https://github.com/emsesp/EMS-ESP32/issues/228)
- overlapping while reading sequence of EMS1.0 telegrams
- redundant telegram readings (because of offset overflow)
- added missing RC30/Moduline400 [#243](https://github.com/emsesp/EMS-ESP32/issues/243)

## Changed

- Refactor WebUI to use ReactHooks. New theme, increased performance, better mobile responsiveness [#124](https://github.com/emsesp/EMS-ESP32/issues/124)
- Boiler and Thermostat entity "wwdisinfect" renamed to "wwdisinfecting" [#211](https://github.com/emsesp/EMS-ESP32/issues/211)
- Boiler "wwheat" renamed to "ww3wayon" [#211](https://github.com/emsesp/EMS-ESP32/issues/211)

## **BREAKING CHANGES**

- Settings: order of Boolean Format has changed in Application Settings
- Dallas Format removed. Use the name to give each sensor an alias
- Boiler and Thermostat entity "wwdisinfect" renamed to "wwdisinfecting"
- Boiler "wwheat" renamed to "ww3wayon"
