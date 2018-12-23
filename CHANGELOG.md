# EMS-ESP-Boiler Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- Setting the mode and setpoint temperature on a RC35

## [1.1.1] 2018-12-23

### Removed

- Espurna build notes and ready made firmware

## [1.1.0] 2018-12-22

### Fixed

- Fixed handling of negative flaoting point values (like outdoor temp)
- Fixed handling of auto & manual mode on an RC30
- [Fixed condition where all telegram types were processed, instead of only broadcasts or our own reads](https://github.com/proddy/EMS-ESP-Boiler/issues/15)

### Added

- Created this CHANGELOG.md file!
- [Added support for the Nefit Easy thermostat, reading of temperature values only](https://github.com/proddy/EMS-ESP-Boiler/issues/9) - note *read only* (big thanks @**kroon040** for lending me an Easy device) 
- [Added support for RC35/Moduline 400](https://github.com/proddy/EMS-ESP-Boiler/issues/14) - *read only*
- [New raw logging mode for logging](https://github.com/proddy/EMS-ESP-Boiler/issues/11)
- [New 'r'command to send raw data to EMS](https://github.com/proddy/EMS-ESP-Boiler/issues/11)
- [Added MQTT messages for hot water on and heating on](https://github.com/proddy/EMS-ESP-Boiler/issues/10)
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