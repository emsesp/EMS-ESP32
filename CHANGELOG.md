# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

# Changelog

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

- power settings, disabling BLE and turning off Wifi sleep
- Rx and Tx counts to Heartbeat MQTT payload
- ethernet support
- id to info command to show only a heatingcircuit
- add sending devices that are not listed to 0x07
- extra MQTT boolean option for "ON" and "OFF"
- support for chunked MQTT payloads to allow large data sets > 2kb
- external Button support (#708) for resetting to factory defaults and other actions
- new console set command in `system`, `set board_profile <profile>` for quickly enabling cabled ethernet connections without using the captive wifi portal
- added in MQTT nested mode, for thermostat and mixer, like we had back in v2
- cascade MC400 (product-id 210) (3.0.0b6), power values for heating sources (3.0.1b1)
- values for wwMaxPower, wwFlowtempOffset
- RC300 `thermostat temp -1` to clear temporary setpoint in auto mode
- syslog port selectable (#744)
- individual mqtt commands (#31)
- board Profiles (#11)

## Fixed

- telegrams matched to masterthermostat 0x18
- multiple roomcontrollers
- readback after write with delay (give ems-devices time to set the value)
- thermostat ES72/RC20 device 66 to command-set RC20_2
- MQTT payloads not adding to queue when MQTT is re-connecting (fixes #369)
- fix for HA topics with invalid command formats (#728)
- wrong position of values #723, #732
- OTA Upload via Web on OSX
- Rx and Tx quality % would sometimes show > 100

## Changed

- changed how telegram parameters are rendered for mqtt, console and web (#632)
- split `show values` in smaller packages (edited)
- extended length of IP/hostname from 32 to 48 chars (#676)
- check flowsensor for `tap_water_active`
- mqtt prefixed with `Base`
- count Dallas sensor fails
- switch from SPIFFS to LITTLEFS
- added ID to MQTT payloads which is the Device's product ID and used in HA to identify a unique HA device
- increased MQTT buffer and reduced wait time between publishes
- updated to the latest ArduinoJson library
- some names of mqtt-tags like in v2.2.1
- new ESP32 partition side to allow for smoother OTA and fallback
- network Gateway IP is optional (#682)emsesp/EMS-ESP
- moved to a new GitHub repo https://github.com/emsesp/EMS-ESP32
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
