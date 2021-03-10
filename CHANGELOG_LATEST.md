# Changelog

## **Important Breaking Changes**
 - 3.0.0b3 uses a new filesystem. During upgrading all settings will be erased and not migrated.

### Added
- Power settings, disabling BLE and turning off Wifi sleep
- Rx and Tx counts to Heartbeat MQTT payload
- Ethernet support
- id to info command to show only a heatingcircuit
- add sending devices that are not listed to 0x07
- extra MQTT boolean option for "ON" and "OFF"
- Support for chunked MQTT payloads to allow large data sets > 2kb
- External Button support (#708) for resetting to factory defaults and other actions
- new console set command in `system`, `set ethernet <profile>` for quickly enabling cabled ethernet connections without using the captive wifi portal
- Added in MQTT nested mode, for thermostat and mixer, like we had in v2
- Boiler GB172i (product-id 210) (3.0.0b6)

### Fixed
- telegrams matched to masterthermostat 0x18
- multiple roomcontrollers
- readback after write with delay (give ems-devices time to set the value)
- Thermostat ES72/RC20 device 66 to command-set RC20_2
- MQTT payloads not adding to queue when MQTT is re-connecting (fixes #369)
- fix for HA topics with invalid command formats (#728)
- wrong position of values #723, #732

### Changed
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

### Removed

