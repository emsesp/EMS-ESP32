# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

# [3.5.0] February 6 2023

## **IMPORTANT! BREAKING CHANGES**

- When upgrading to v3.5 for the first time from v3.4 on a BBQKees Gateway board you will need to use the [EMS-EPS Flasher](https://github.com/emsesp/EMS-ESP-Flasher/releases) to correctly re-partition the flash. Make sure you backup the settings and customizations from the WebUI (System->Upload/Download) and restore after the upgrade.

## Added

- Translations in Web UI and all device entity names (DE, NL, SV, PL, NO, FR) [#22](https://github.com/emsesp/EMS-ESP32/issues/22)
- Add support for Lolin C3 mini [#620](https://github.com/emsesp/EMS-ESP32/pull/620)
- Add support for ESP32-S2 [#667](https://github.com/emsesp/EMS-ESP32/pull/667)
- Add devices: Greenstar 30Ri boiler, Junkers FW500 thermostat, Buderus BC30 controller
- Add program memory info
- Add mqtt queue and connection infos
- Adapt min/max if ems-value is not in this range
- Add heat pump settings for inputs and limits [#600](https://github.com/emsesp/EMS-ESP32/issues/600)
- Add hybrid heatpump [#500](https://github.com/emsesp/EMS-ESP32/issues/500)
- Add translated tags
- Add min/max to customization table [#686](https://github.com/emsesp/EMS-ESP32/issues/686)
- Add MD5 check [#637](https://github.com/emsesp/EMS-ESP32/issues/637)
- Add more bus-ids [#673](https://github.com/emsesp/EMS-ESP32/issues/673)
- Use HA connectivity device class for Status, added boot time [#751](https://github.com/emsesp/EMS-ESP32/issues/751)
- Add commands for analog sensors outputs
- Support for multiple EMS-ESPs with MQTT and HA [[#759](https://github.com/emsesp/EMS-ESP32/issues/759)]
- Settings for heatpump silent mode and additional heater [[#802](https://github.com/emsesp/EMS-ESP32/issues/802)] [[#803](https://github.com/emsesp/EMS-ESP32/issues/803)]
- Zone module MZ100 [#826](https://github.com/emsesp/EMS-ESP32/issues/826)
- Default MQTT hostname is blank [#829](https://github.com/emsesp/EMS-ESP32/issues/829)
- wwCurFlow for ems+ devices [#829](https://github.com/emsesp/EMS-ESP32/issues/829)
- Add Rego 3000, TR120RF thermostats [#917](https://github.com/emsesp/EMS-ESP32/issues/917)
- Add config for ESP32-S3
- Add heatpump silent mode and other entities [#896](https://github.com/emsesp/EMS-ESP32/issues/896)
- Allow reboot to other partition (factory or asymetric OTA)
- Blacklist entities to remove from memory [#891](https://github.com/emsesp/EMS-ESP32/issues/891)
- Add boiler pump operating mode [#944](https://github.com/emsesp/EMS-ESP32/issues/944)

## Fixed

- Factory Reset not working [#628](https://github.com/emsesp/EMS-ESP32/issues/628)
- Valid 4 byte values [#820](https://github.com/emsesp/EMS-ESP32/issues/820)
- Commands for multiple thermostats [#826](https://github.com/emsesp/EMS-ESP32/issues/826)
- API queries for multiple devices [#865](https://github.com/emsesp/EMS-ESP32/issues/865)
- Console crash when using call with command `hcx` only. [#841](https://github.com/emsesp/EMS-ESP32/issues/841)
- `heatingPump2Mod` was wrong, changed to absBurnPow [[#908](https://github.com/emsesp/EMS-ESP32/issues/908)
- Rounding of web input values
- Analog sensor with single gpio number [#915](https://github.com/emsesp/EMS-ESP32/issues/915)
- HA dallas and analog configs: remove/rebuild on change [#888](https://github.com/emsesp/EMS-ESP32/issues/888)
- Modes and set seltemp for RC30 and RC20 [#932](https://github.com/emsesp/EMS-ESP32/issues/932)

## Changed

- Discovery in HomeAssistant don't work with custom base topic. [#596](https://github.com/emsesp/EMS-ESP32/issues/596) Base topic containing `/` are changed to `_`
- RF room temperature sensor are shown as thermostat
- Render mqtt float json values with trailing zero
- Removed flash strings, to increase available heap memory
- Reload page after restart button is pressed
- Analog/dallas values command as list like ems-devices
- Analog/dallas HA-entities based on id
- MQTT Base is a mandatory field. Removed MQTT topic length from settings
- HA duration class for time entities [[#822](https://github.com/emsesp/EMS-ESP32/issues/822)
- AM200 alternative heatsource as class heatsource [[#857](https://github.com/emsesp/EMS-ESP32/issues/857)

# [3.4.2] September 18 2022

## Added

- RC310 additions [#520](https://github.com/emsesp/EMS-ESP32/pull/520)
  - damping
  - wwprio for RC310 heating circuits
  - switchonoptimization for RC310 heating circuits
  - enum_controlmode for RC310 (new enum list)
  - nofrostmode, reducemode, reducetemp & noreducetemp for RC310
  - emergencyops and emergencytemp, wwmaxtemp, wwflowtempoffset and wwcomfort1 for RC310
  - HM200 hybrid module [#500](https://github.com/emsesp/EMS-ESP32/issues/500)
  - AM200 alternative heatsource module [#573](https://github.com/emsesp/EMS-ESP32/issues/573)
  - EM10 error module as gateway [#575](https://github.com/emsesp/EMS-ESP32/issues/575)

## Fixed

- fix Table resizing in WebUI [#519](https://github.com/emsesp/EMS-ESP32/issues/519)
- allow larger customization files [#570](https://github.com/emsesp/EMS-ESP32/issues/570)
- losing entitiy wwcomfort [#581](https://github.com/emsesp/EMS-ESP32/issues/581)

## Changed

- Shorten "friendly names" in Home Assistant [#555](https://github.com/emsesp/EMS-ESP32/issues/555)
- platformio 2.3.0 (IDF 4, Arduino 2)
- remove master-thermostat, support multiple thermostats
- merge up- and download in webui [#577](https://github.com/emsesp/EMS-ESP32/issues/577)

# [3.4.1] May 29 2022

## Fixed

- Fix memory leak in api [#524](https://github.com/emsesp/EMS-ESP32/issues/524)

## Changed

- Controller data in web-ui only for IVT [#522](https://github.com/emsesp/EMS-ESP32/issues/522)
- Rename hidden `climate` to a more explaining name [#523](https://github.com/emsesp/EMS-ESP32/issues/523)
- Minor changes to the Customizations web page [#527](https://github.com/emsesp/EMS-ESP32/pull/527)

# [3.4.0] May 23 2022

## Added

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
- analog outputs digital/pwm/dac
- remove MQTT retained configs if discovery is disabled
- timeout 10 min for MQTT-QoS wait
- Moduline 300 auto-temperatures T1-T4, RC300 romminfluencefactor
- RC35 parameters [#392](https://github.com/emsesp/EMS-ESP32/issues/392), [#398](https://github.com/emsesp/EMS-ESP32/issues/398)
- sync time with thermostat [#386](https://github.com/emsesp/EMS-ESP32/issues/386), [#408](https://github.com/emsesp/EMS-ESP32/issues/408)
- set mode has immediate effect [#395](https://github.com/emsesp/EMS-ESP32/issues/395)
- min/max in web value setting
- Extend customization to select if an entity is to be shown in the WebUI or forced as read-only [#317](https://github.com/emsesp/EMS-ESP32/issues/317)
- Added Moduline 400 installation parameters [PR #449 by @kwertie01](https://github.com/emsesp/EMS-ESP32/pull/449)
- Read time from IVT-controller [#439](https://github.com/emsesp/EMS-ESP32/issues/439)
- Hybrid Heatpump product-id 168 [#459](https://github.com/emsesp/EMS-ESP32/issues/459), thermostat settings
- Junkers ISM2 and IPM in warm water mode [#437](https://github.com/emsesp/EMS-ESP32/issues/437)
- Added Shower Alert trigger time and cold shot time [#436](https://github.com/emsesp/EMS-ESP32/issues/436)
- Improved Table layout in Web UI (searching, filtering, sorting, exporting to CSV)
- API fetch individual attributes from an entity [#462](https://github.com/emsesp/EMS-ESP32/issues/462)
- Option to disable mDNS
- Option for rendering booleans on dashboard [#456](https://github.com/emsesp/EMS-ESP32/issues/456)
- Upload customization settings from a file [#256](https://github.com/emsesp/EMS-ESP32/issues/256)

## Fixed

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
- some missing fahrenheit calculations
- limited number of exclusions [#339](https://github.com/emsesp/EMS-ESP32/issues/339)
- MQTT sometimes would not reconnect after a WiFi outage

## Changed

- Use flash system to show system health (1 flash=no ems, 2 flashes=no wifi) [#224](https://github.com/emsesp/EMS-ESP32/issues/224)
- Renamed Dallas Sensor to Temperature Sensor in UI
- Dallas Format removed. Use the name to give each sensor an alias
- No longer MQTT subscribes to topic `/thermostat_hc<n>` as it supports a path similar to the API endpoint construct
- Show Sensors quality in WebUI
- Controller not shown in WebUI dashboard
- renamed "Home Assistant Integration" to "MQTT Discovery" in MQTT Settings [#290](https://github.com/emsesp/EMS-ESP32/issues/290)
- Show ems tx reads and writes separately
- Show ems device handlers separated for received, fetched and pending handlers.
- Wired renamed to Ethernet
- removed system/pin command, new commands in analogsensors
- system/info device-info split to name/version/brand
- exclude list uses short-names, possible flags for web/api/mqtt excludes, readonly and favorite (selection not yet implemented)
- thermostat clock formate date-time: dd.mm.yyyy hh:mm
- RC300 summermode as other thermostats `winter/summer` instead of `off/on`

## **BREAKING CHANGES:**

- Settings:
  - order of Boolean Format has changed in Application Settings - check your settings
  - Dallas Format setting removed. Now customize name of each Dallas sensor via the UI
- MQTT/API
  - Boiler `wwheat` renamed to `ww3wayon` [#211](https://github.com/emsesp/EMS-ESP32/issues/211)
  - Boiler `ww` tag renamed to `dhw`. Any custom Home Assistant lovelace dashboards will need updating.
  - Renamed description of `wwtapactivated` to "turn on/off DHW". Otherwise would have looked like "boiler_dhw_turn_on_off_dhw" in HA.
  - `/api/system/info` endpoint has updated keys. Now lowercase, no underscores and not capitalized. Replace "handlers" with "handlers received", "handlers fetched" and "handlers pending".

# [3.3.1] January 20 2022

- lastcode broke MQTT JSON structure [#228](https://github.com/emsesp/EMS-ESP32/issues/228)
- overlapping while reading sequence of EMS1.0 telegrams
- redundant telegram readings (because of offset overflow)
- added missing RC30/Moduline400 [#243](https://github.com/emsesp/EMS-ESP32/issues/243)
- check received status before toggling fetch on empty telegram [#268][#282]

# [3.3.0] November 28 2021

## Added

- Add system commands for syslog level and watch [#98](https://github.com/emsesp/EMS-ESP32/issues/98)
- Added pool data to telegrams 0x494 & 0x495 [#102](https://github.com/emsesp/EMS-ESP32/issues/102)
- Add RC300 second summermode telegram [#108](https://github.com/emsesp/EMS-ESP32/issues/108)
- Add support for the RC25 thermostat [#106](https://github.com/emsesp/EMS-ESP32/issues/106)
- Add new command 'entities' for a device, e.g. http://ems-esp/api/boiler/entities to show the shortname, description and HA Entity name (if HA enabled) [#116](https://github.com/emsesp/EMS-ESP32/issues/116)
- Support for Junkers program and remote (fb10/fb110) temperature
- Home Assistant `state_class` attribute for Wh, kWh, W and KW [#129](https://github.com/emsesp/EMS-ESP32/issues/129)
- Add current room influence for RC300 [#136](https://github.com/emsesp/EMS-ESP32/issues/136)
- Added Home Assistant device_class to sensor entities
- Added another Buderus RC10 thermostat with Product ID 65 [#160](https://github.com/emsesp/EMS-ESP32/issues/160)
- Added support for mDNS [#161](https://github.com/emsesp/EMS-ESP32/issues/161)
- Added last system ESP32 reset code to log (and `system info` output)
- Firmware Checker in WebUI [#168](https://github.com/emsesp/EMS-ESP32/issues/168)
- Added new MQTT setting for enabling 'response' topic
- Support for non-standard Thermostats like Tado [#174](https://github.com/emsesp/EMS-ESP32/issues/174)
- Include MQTT connection status in 'api/system/info'
- Include Network status in 'api/system/info' and also the MQTT topic `info` [#202](https://github.com/emsesp/EMS-ESP32/issues/202)
- Added Ethernet PHY module as an option in the Board Profile [#210](https://github.com/emsesp/EMS-ESP32/issues/210)

## Fixed

- MQTT reconnecting after WiFi reconnect [#99](https://github.com/emsesp/EMS-ESP32/issues/99)
- Manually Controlling Solar Circuit [#107](https://github.com/emsesp/EMS-ESP32/issues/107)
- Fix thermostat commands not defaulting to the master thermostat [#110](https://github.com/emsesp/EMS-ESP32/issues/110)
- Enlarge parse-buffer for long names like `cylinderpumpmodulation`
- MQTT not subscribing to all device entities [#166](https://github.com/emsesp/EMS-ESP32/issues/166)
- Help fix issues with WebUI unable to fully load UI over Ethernet [#177](https://github.com/emsesp/EMS-ESP32/issues/177)
- Shower alert never reset after limit reached when enabled [(PR #185)]
- Remove HA entity entries when a device value goes dormant [#196](https://github.com/emsesp/EMS-ESP32/issues/196)
- deciphering last error code dates on 0xC2 telegram [#204](https://github.com/emsesp/EMS-ESP32/issues/204)

## Changed

- Syslog BOM only for utf-8 messages [#91](https://github.com/emsesp/EMS-ESP32/issues/91)
- Check for KM200 by device-id 0x48, remove tx-delay [#90](https://github.com/emsesp/EMS-ESP32/issues/90)
- rename `fastheatupfactor` to `fastheatup` and add percent [#122](https://github.com/emsesp/EMS-ESP32/issues/122)
- "unit" renamed to "uom" in API call to recall a Device Value
- initial backend React changes to replace the class components (HOCs) with React Hooks
- Use program-names instead of numbers
- Boiler's maintenancemessage always published in MQTT (to prevent HA missing entity)
- Unit of Measure 'times' added to MQTT Fails, Rx fails, Rx received, Tx fails, Tx reads & Tx writes
- Improved API. Restful HTTP API works in the same way as MQTT calls
- Removed settings for MQTT subscribe format [#173](https://github.com/emsesp/EMS-ESP32/issues/173)
- Improve Nefit Moduline 200 functionality [#183](https://github.com/emsesp/EMS-ESP32/issues/183)
- `status` in the MQTT heartbeat renamed to `bus_status`
- Layout changes in the WebUI, showing stripped table rows in Dashboard
- Alternative font for log window [#219](https://github.com/emsesp/EMS-ESP32/issues/219)

## **BREAKING CHANGES**

- API: "unit" renamed to "uom" in API call to recall a Device Value
- HA: `sensor.boiler_boiler_temperature` renamed to `sensor.actual_boiler_temperature`
- HA: `binary_sensor.boiler_ww_disinfecting` renamed to `binary_sensor.boiler_ww_disinfection`
- HA: # removed from counts in MQTT Fails, Rx fails, Rx received, Tx fails, Tx reads & Tx writes
- `txread` renamed to `txreads` and `txwrite` renamed to `txwrites` in MQTT heartbeat payload
- 'dallas sensors' in api/system/info moved to the "System" section. Renamed "uptime (seconds)" and "reset reason"
- `status` in the MQTT heartbeat renamed to `bus_status`

# [3.2.1] August 8 2021

## Added

- json body in API can now take device, name, cmd, hc and id
- added example of how to use API directly to control values from Home Assistant
- API calls are shown in debug log (For troubleshooting)

## Fixed

- fixed issue with Home Assistant entity naming where boiler's ww was duplicated in entity name
- fixed issue where wwSetTemp was written too instead of wwSelTemp

## Changed

- fixed case on mqtt names, like 'wwtankmiddletemp'
- renamed Product ID to 'EMS Product ID' in Home Assistant
- removed brackets around tags, e.g. (hc1) selected room temperature" is now just "hc1 selected room temperature"

# [3.2.0] August 6 2021

## Added

- support for IPv6 (web/api/mqtt, not syslog yet) [#83](https://github.com/emsesp/EMS-ESP32/issues/83)
- System Log in Web UI will show current time if the NTP Service is enabled [#82](https://github.com/emsesp/EMS-ESP32/issues/82)
- Network settings for Tx-power, WiFi-bandwidth, WiFi-sleepmode [#83](https://github.com/emsesp/EMS-ESP32/issues/83)
- optional low CPU clockrate (160 MHz) [#83](https://github.com/emsesp/EMS-ESP32/issues/83)
- select format for enumerated values in web
- settings for water hysteresis on/off
- dallas sensor name editable. `sensorname` console-command, replace sensorid with a unique name [#84](https://github.com/emsesp/EMS-ESP32/issues/84)
- 'restart' system command. Can be invoked via API with authentication. [#87](https://github.com/emsesp/EMS-ESP32/issues/87)
- add Download button in Web UI for log

## Fixed

- set mode allow numbers
- Junkers thermostat shows mode as selected by set_mode
- HA thermostat mode if bool-format: numbers is selected
- Web UI System Log sometimes skipped a few log messages when watching real-time
- fix wwactivated [#89](https://github.com/emsesp/EMS-ESP32/issues/89)
- don't show commands (like reset) as Device values in the Web or Console

## Changed

- removed Rx echo failures counting as incomplete telegrams. Bad telegrams show as Warning and not Errors. [#80](https://github.com/emsesp/EMS-ESP32/issues/80)
- add upload_sec to `api/system/info` and removed # from some names to keep consistent with MQTT heartbeat
- added debug target to PlatformIO build to help hunt down system crashes
- enumerated values always start at zero
- maintenance settings for time/date as extra setting
- move api/mqtt formats to `settings`, add `enum format`
- UI improvements for editing Dallas Sensor details
- RESTful GET commands can also require authentication (via bearer access token) for better security
- Updated AsyncMqttClient to 0.9.0 and ArduinoJson to 6.18.3
- Download buttons for settings and info under the Help tab

# [3.1.1] June 26 2021

## Added

- new command called `commands` which lists all available commands. `ems-esp/api/{device}/commands`
- More Home Assistant icons to match the UOMs
- new API. Using secure access tokens and OpenAPI standard. See `doc/EMS-ESP32 API.md` and [#50](https://github.com/emsesp/EMS-ESP32/issues/50)
- show log messages in Web UI [#71](https://github.com/emsesp/EMS-ESP32/issues/71)

## Fixed

- HA thermostat mode was not in sync with actual mode [#66](https://github.com/emsesp/EMS-ESP32/issues/66)
- Don't publish rssi if Wifi is disabled and ethernet is being used
- Booleans are shown as true/false in API GETs

## Changed

- `info` command always shows full names in API. For short names query the device or name directly, e.g. `http://ems-esp/api/boiler`
- free memory is shown in kilobytes
- boiler's warm water entities have ww added to the Home Assistant entity name [#67](https://github.com/emsesp/EMS-ESP32/issues/67)
- improved layout and rendering of device values in the WebUI, also the edit value screen

# [3.1.0] May 4 2021

## Added

- Mock API to simulate an ESP, for testing web
- Able to write values from the Web UI
- check values with `"cmd":<valuename>` and data empty or `?`
- set hc for values and commands by id or prefix `hc<x>`+separator, separator can be any char

## Fixed

- Don't create Home Assistant MQTT discovery entries for device values that don't exists (#756 on EMS-ESP repo)
- Update shower MQTT when a shower start is detected
- S32 board profile

## Changed

- Icon for Network
- MQTT Formatting payload (nested vs single) is a pull-down option
- moved mqtt-topics and texts to local_EN, all topics lower case
- Re-enabled Shower Alert (still experimental)
- lowercased Flow temp in commands
- system console commands to main

# [3.0.1] March 30 2021

## Added

- individual mqtt commands (#31)
- board Profiles (#11)

## Fixed

- Rx and Tx quality % would sometimes show > 100

## Changed

- invert LED changed to Hide LED. Default is off.
- renamed Scan Network to Scan WiFi Network
- added version to cmd=settings
- Allow both WiFi and Ethernet together, fall back to AP when Ethernet disconnects

## Removed

- Shower Alert (disabled for now)

# [3.0.0] March 18 2021

## Added

- Power settings, disabling BLE and turning off Wifi sleep
- Rx and Tx counts to Heartbeat MQTT payload
- Ethernet support
- id to info command to show only a heatingcircuit
- add sending devices that are not listed to 0x07
- extra MQTT boolean option for "ON" and "OFF"
- Support for chunked MQTT payloads to allow large data sets > 2kb
- External Button support (#708) for resetting to factory defaults and other actions
- new console set command in `system`, `set ethernet <profile>` for quickly enabling cabled ethernet connections without using the captive wifi portal
- Added in MQTT nested mode, for thermostat and mixer, like we had back in v2
- Cascade MC400 (product-id 210) (3.0.0b6)
- values for wwMaxPower, wwFlowtempOffset
- RC300 `thermostat temp -1` to clear temporary setpoint in auto mode
- Syslog port selectable (#744)

## Fixed

- telegrams matched to masterthermostat 0x18
- multiple roomcontrollers
- readback after write with delay (give ems-devices time to set the value)
- Thermostat ES72/RC20 device 66 to command-set RC20_2
- MQTT payloads not adding to queue when MQTT is re-connecting (fixes #369)
- fix for HA topics with invalid command formats (#728)
- wrong position of values #723, #732
- OTA Upload via Web on OSX

## Changed

- changed how telegram parameters are rendered for mqtt, console and web (#632)
- split `show values` in smaller packages (edited)
- extended length of IP/hostname from 32 to 48 chars (#676)
- check flowsensor for `tap_water_active`
- mqtt prefixed with `Base`
- count Dallas sensor fails
- switch from SPIFFS to LITTLEFS
- Added ID to MQTT payloads which is the Device's product ID and used in HA to identify a unique HA device
- Increased MQTT buffer and reduced wait time between publishes
- Updated to the latest ArduinoJson library
- some names of mqtt-tags like in v2.2.1
- new ESP32 partition side to allow for smoother OTA and fallback
- Network Gateway IP is optional (#682)emsesp/EMS-ESP
- moved to a new GitHub repo https://github.com/emsesp/EMS-ESP32
