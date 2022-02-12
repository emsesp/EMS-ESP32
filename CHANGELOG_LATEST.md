# Changelog

## [3.4.0]

### Added

- WebUI optimizations, updated look&feel and better performance [#124](https://github.com/emsesp/EMS-ESP32/issues/124)
- Auto refresh of WebUI after successful firmware upload [#178](https://github.com/emsesp/EMS-ESP32/issues/178)
- New Customization Service in WebUI. First feature is the ability to enable/disabled Enitites (device values) from EMS devices [#206](https://github.com/emsesp/EMS-ESP32/issues/206)
- Option to disable Telnet Console [#209](https://github.com/emsesp/EMS-ESP32/issues/209)
- Added Hide SSID, Max Clients and Preferred Channel to Access Point
- Merged in MichaelDvP's changes like Fahrenheit conversion, publish single (for IOBroker) and a few other critical optimizations
- Enabled bi-directional read/write with Home Assistant, so values can be changed automatically from the UI without scripting [#265](https://github.com/emsesp/EMS-ESP32/issues/265)
- Added GC7000F Boiler [#270](https://github.com/emsesp/EMS-ESP32/issues/270)
- Revised LED flash sequence on boot up to show system health (1 flash=no ems, 2 flashes=no wifi) [#224](https://github.com/emsesp/EMS-ESP32/issues/224)
- Analog Sensor support [#271](https://github.com/emsesp/EMS-ESP32/issues/271)
- Solar cylinder priority [#247](https://github.com/emsesp/EMS-ESP32/issues/247)
- Read only mode in Settings, where EMS Tx/Write commands are blocked [#286](https://github.com/emsesp/EMS-ESP32/issues/286)
- Added 8700i Boiler device
- Added Cascade CM10 Controller device
- Add Olimex ESP32-POE-ISO to board profiles plus settings to customize Ethernet modules [#301](https://github.com/emsesp/EMS-ESP32/issues/301)
- Help text for string commands in WebUI [#320](https://github.com/emsesp/EMS-ESP32/issues/320)
- Germany translations (at compile time)
- #entities added to system/info` endpoint [#322](https://github.com/emsesp/EMS-ESP32/issues/322)

### Fixed

- lastcode broke MQTT JSON structure [#228](https://github.com/emsesp/EMS-ESP32/issues/228)
- fixed issue with overlapping while reading sequence of EMS1.0 telegrams
- fixed redundant telegram readings (because of offset overflow)
- added missing RC30/Moduline 400 [#243](https://github.com/emsesp/EMS-ESP32/issues/243)
- Correct modes for RC25 [#106](https://github.com/emsesp/EMS-ESP32/issues/106)
- Clean up old HA config's in MQTT before publishing data. This will prevent HA giving the 'dict' warnings [#229](https://github.com/emsesp/EMS-ESP32/issues/229)
- RC25 temperature setting [#272](https://github.com/emsesp/EMS-ESP32/issues/272)
- Buderus RC25 - "hc1 mode type" incorrect value [#273](https://github.com/emsesp/EMS-ESP32/issues/273)
- Increased number of Mixers and Heating Circuits [#294](https://github.com/emsesp/EMS-ESP32/issues/294)
- Check receive status before removing a telegram fetch [#268](https://github.com/emsesp/EMS-ESP32/issues/268), [#282](https://github.com/emsesp/EMS-ESP32/issues/282)
- Fix uploading firmware on OSX [#345](https://github.com/emsesp/EMS-ESP32/issues/345)
- Non-nested MQTT would corrupt the json [#354](https://github.com/emsesp/EMS-ESP32/issues/354)
- Burner selected max power can have a value higher than 100% [#314](https://github.com/emsesp/EMS-ESP32/issues/314)

### Changed

- Use flash system to show system health (1 flash=no ems, 2 flashes=no wifi) [#224](https://github.com/emsesp/EMS-ESP32/issues/224)
- Renamed Dallas Sensor to Temperature Sensor in UI
- Dallas Format removed. Use the name to give each sensor an alias
- No longer MQTT subscribes to topic `/thermostat_hc<n>` as it supports a path similar to the API endpoint construct
- Show Sensors quality in WebUI
- Controller not shown in WebUI dashboard
- renamed "Home Assistant Integration" to "MQTT Discovery" in MQTT Settings [#290](https://github.com/emsesp/EMS-ESP32/issues/290)
- Show ems tx reads and writes separatly
- Show ems device handlers separated for received, fetched and pending handlers.
- Wired renamed to Ethernet

## **BREAKING CHANGES:**

- Settings:
  - order of Boolean Format has changed in Application Settings - check your settings
  - Dallas Format setting removed. Now customize name of each Dallas sensor via the UI
- MQTT/API
  - Boiler `wwheat` renamed to `ww3wayon` [#211](https://github.com/emsesp/EMS-ESP32/issues/211)
  - Boiler `ww` tag renamed to `dhw`. Any custom Home Assistant lovelace dashboards will need updating.
  - Renamed description of `wwtapactivated` to "turn on/off DHW". Otherwise would have looked like "boiler_dhw_turn_on_off_dhw" in HA.
  - `/api/system/info` endpoint has updated keys. Now lowercase, no underscores and not capitalized. Replace "handlers" with "handlers received", "handlers fetched" and "handlers pending".
