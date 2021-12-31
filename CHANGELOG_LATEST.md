# Changelog

# [3.4.0]

## Added

- New WebUI upgrade
- Auto reload of WebUI after successful firmware upload [#178](https://github.com/emsesp/EMS-ESP32/issues/178)
- Customization Service. First is the Entity Exclusion for devices [#206](https://github.com/emsesp/EMS-ESP32/issues/206)
- Option to disable Telnet Console
- Added Hide SSID, Max Clients and Preferred Channel to Access Point
- Merged in MichaelDvP's changes like fahrenheit conversion, publish single (for iobroker) and a few other changes
- Extended support for Home Assistant to write back values [#265](https://github.com/emsesp/EMS-ESP32/issues/265)
- Added GC7000F [#270](https://github.com/emsesp/EMS-ESP32/issues/270)

## Fixed

- lastcode broke MQTT JSON structure [#228](https://github.com/emsesp/EMS-ESP32/issues/228)
- overlapping while reading sequence of EMS1.0 telegrams
- redundant telegram readings (because of offset overflow)
- added missing RC30/Moduline400 [#243](https://github.com/emsesp/EMS-ESP32/issues/243)
- Correct modes for RC25 [#106](https://github.com/emsesp/EMS-ESP32/issues/106)
- Clean up old HA config's in MQTT before publishing data. This will prevent HA giving the 'dict' warnings

## Changed

- Refactor WebUI to use ReactHooks. New theme, increased performance, better mobile responsiveness [#124](https://github.com/emsesp/EMS-ESP32/issues/124)

**Breaking changes:**

- Settings: order of Boolean Format has changed in Application Settings
- Boiler `wwheat` renamed to `ww3wayon` [#211](https://github.com/emsesp/EMS-ESP32/issues/211)
- Boiler ww tag renamed to dhw. Any custom HA dashboards will need updating.
- Renamed Dallas Sensor to Temperature Sensor
- Dallas Format removed. Use the name to give each sensor an alias
- Renamed description of `wwtapactivated` to "turn on/off DHW". Otherwise would look like "boiler_dhw_turn_on_off_dhw" in HA
