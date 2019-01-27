# EMS-ESP Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.4.0] 2019-01-27

### Changed

- last will MQTT topic prefixed with a header like the rest of the topics
- All double and float numbers rendered to 2 decimal places (precision = 2)
- Default logging set to None when starting a telnet session

### Added

- Added support for external Dallas sensors (DS1822, DS18S20, DS18B20, DS1825). See readme
- Added UBAParametersMessage type to fetch boiler modulation min & max values
- Report shows system load average

## [1.3.2] 2019-01-23

### Fixed

- Handle thermostats that don't have builtin temperature sensors when showing current temperature (https://github.com/proddy/EMS-ESP/issues/18#issuecomment-451012963)

### Changed

- Improved way to identify if the EMS bus is connected
- Improved 'types' command to show more details
- Improved auto detect of thermostat types

### Added

- Some more devices like the Nefit Topline & RC310 thermostat recognition
- Added a check to see Tx is possible. See 'Tx Capable' under the 'info' screen

### Removed

- Removed `MY_BOILER_MODELID` from `my_config.h`. It's always hardcoded.

## [1.3.1] 2019-01-12

### Fixed

- telnet commands with set are no longer forced to lower case

### Changed

- Custom settings (e.g set led) moved outside MyESP
- Moved all MQTT to my_config.h making it independent from Home Assistant

### Added

- MQTT keep alive, last will testament and other settings all configurable in my_config.h
- RC35: MQTT day/night/auto mode; sets setpoint temperature in type 0x3D depends on current night/day Mode (@SpaceTeddy) [#33](https://github.com/proddy/EMS-ESP/pull/33)

## [1.3.0] 2019-01-09

### Changed

- Renamed project from EMS-ESP-Boiler to EMS-ESP since it's kinda EMS generic now
- Support for RC20F and RFM20 (https://github.com/proddy/EMS-ESP/issues/18)
- Moved all EMS device information into a separate file `ems_devices.h` so no longer need to touch `ems.h`
- Telnet commands can be strings now and output is suspended when typing

### Removed

- Removed SHOWER_TEST
- Removed WIFI and MQTT credentials from the platformio.ini file

### Added

- Settings are saved and loaded from the ESP8266's file system (SPIFFS). Can be set using the 'set' command
- Improved support when in Access Point mode (192.168.4.1)
- pre-built firmwares are back

## [1.2.4] 2019-01-04

### Changed

- Scanning known EMS Devices now ignores duplicates (https://github.com/proddy/EMS-ESP/pull/30)
- ServiceCode stored as a two byte char
- Support for RC20F and RFM20 (https://github.com/proddy/EMS-ESP/issues/18)

## [1.2.3] 2019-01-03

### Fixed

- Can now hardcode Boiler and Thermostat types in my_config.h to bypass auto-detection
- Fixed MQTT subscribing to Heating and Hot Water active topics
- Fixed for listening to incoming MQTT topics (https://github.com/proddy/EMS-ESP/issues/27)
- Fixed handling of current temperature on an RC35-type thermostat that doesn't have a sensor (https://github.com/proddy/EMS-ESP/issues/18)

## [1.2.2] 2019-01-02

### Fixed

- Issues in 1.2.1 (see https://github.com/proddy/EMS-ESP/issues/25)
- Logic for determining if there is activity on the EMS bus and using the onboard LEDs properly

## [1.2.1] 2019-01-02

### Fixed

- Only process broadcast messages if the offset (byte 4) is 0. (https://github.com/proddy/EMS-ESP/issues/23)
- Improved checking for duplicate sent Tx telegrams by comparing CRCs
- Removed distiquishing between noise on the line and corrupt telegrams (https://github.com/proddy/EMS-ESP/issues/24)

## [1.2.0] 2019-01-01

### Fixed

- Incorrect indenting in `climate.yaml` (thanks @mrfixit1)
- Improved support for slower WiFi connections
- Fixed issue with OTA not always giving back a completion response to platformio
- Fixed issue with repeating reads after a raw mode send
- Fixed handling of long integers (thanks @SpaceTeddy)

### Added

- added 'dout' flashmode to platformio.ini so OTA works now when uploading to a Wemos D1 Pro's or any other board with larger flash's
- added un tested supporting RC35 type of thermostats
- Try and discover and set Boiler and Thermostat types automatically
- Fetch UBATotalUptimeMessage from Boiler to get total working minutes
- Added check to see if bus is connected. Shown in stats page
- If no Wifi connection can be made, start up as a WiFi Access Point (AP)
- Report out service codes and water-flow [pull-request](https://github.com/proddy/EMS-ESP/pull/20/files). Thanks @Bonusbartus

### Changed

- Build option is called `DEBUG_SUPPORT` (was `USE_SERIAL`)
- Replaced old **ESPHelper** with my own **MyESP** library to handle Wifi, MQTT, MDNS and Telnet handlers. Supports asynchronous TCP and has smaller memory footprint. And moved to libs directory.
- Simplified LED error checking. If enabled (by default), solid means connected and flashing means error. Uses either an external pull-up or the onboard ESP8266 LED.
- Improved Telnet debugging which uses TelnetSpy to keep a buffer of previous output
- Optimized memory usage & heap conflicts, removing nasty things like strcpy, sprintf where possible
- Improved checking for tap water on/off (thanks @Bonusbartus)

### Removed

- Time and TimeLib's. Not used in code.
- Removed build option `MQTT_MAX_PACKAGE_SIZE` as not using the PubSubClient library any more
- Removed all of Espurna's pre-built firmwares and instructions to build. Keeping it simple.

## [1.1.1] 2018-12-23

### Removed

- Espurna build notes and ready made firmware

## [1.1.0] 2018-12-22

### Fixed

- Fixed handling of negative floating point values (like outdoor temp)
- Fixed handling of auto & manual mode on an RC30
- [Fixed condition where all telegram types were processed, instead of only broadcasts or our own reads](https://github.com/proddy/EMS-ESP/issues/15)

### Added

- Created this CHANGELOG.md file!
- [Added support for the Nefit Easy thermostat, reading of temperature values only](https://github.com/proddy/EMS-ESP/issues/9) - note *read only* (big thanks @**kroon040** for lending me an Easy device) 
- [Added support for RC35/Moduline 400](https://github.com/proddy/EMS-ESP/issues/14) - *read only*
- [New raw logging mode for logging](https://github.com/proddy/EMS-ESP/issues/11)
- [New 'r'command to send raw data to EMS](https://github.com/proddy/EMS-ESP/issues/11)
- [Added MQTT messages for hot water on and heating on](https://github.com/proddy/EMS-ESP/issues/10)
- Implemented FIFO circular buffer queue for up to 20 Tx messages (Q command to show queue)
- Toggle Tx transmission via telnet (use 'X' command)
- Show thermostat type in help stats (use 's' command)
- Show version is help stats (use '?' command)

### Changed

- Improved overall formatting of logging
- Include app name and version in telnet help
- Improved method to switch off hot tap water in Shower Alert
- Telnet P and M commands have changed
- Enabling Logging in telnet is now the 'l' command
- Logging is set back to None when telnet session closes
- Improved fetching of initial boiler values to post to MQTT at startup
- Improved handling and retrying of write/Tx commands

### Removed

- Hid access from telnet to the Experimental custom function command 'x'
- Tx and Rx stats have gone from the stats page, as they were pretty meaningless
- Removed NO_TX define in platformio and replaced with system parameter 'command X'
- Removed -DDEBUG option in build
- Removed wwtemp MQTT messages to set the boiler temp. You'll never miss it.
- Removed LEDs for Tx, Rx and Err. Too many flashing lights and it drains the current.
- Removed capturing of last Rx and Tx times
- Support for older RC20 thermostats

### Known Issues

- There's a nasty memory leek when in telnet's verbose mode with sending which causes the EMS to reset when running for a while.

## [1.0.1] 2018-09-24

- Initial stable version

## [0.1.0] 2018-05-14

- Initial development version