# Changelog

## **Important Breaking Changes**
 - >3.0.0b2 uses a new filesystem. During upgrading all settings will be erased and not migrated.

### Added
- power settings, disabling BLE and turning off Wifi sleep
- Rx and Tx counts to Heartbeat MQTT payload
- ethernet support
- id to info command to show only a heatingcircuit
- add sending devices that are not listed to 0x07
- extra MQTT boolean option for "ON" and "OFF"
- support for chunked MQTT payloads to allow large data sets > 2kb
- external Button support (#708) for resetting to factory defaults and other actions
- new console set command in `system`, `set ethernet <profile>` for quickly enabling cabled ethernet connections without using the captive wifi portal
- added in MQTT nested mode, for thermostat and mixer, like we had back in v2
- cascade MC400 (product-id 210) (3.0.0b6), power values for heating sources (3.0.1b1)
- values for wwMaxPower, wwFlowtempOffset
- RC300 `thermostat temp -1` to clear temporary setpoint in auto mode
- syslog port selectable (#744)
- individual mqtt commands (#31)
- board Profiles (#11)

### Fixed
- telegrams matched to masterthermostat 0x18
- multiple roomcontrollers
- readback after write with delay (give ems-devices time to set the value)
- thermostat ES72/RC20 device 66 to command-set RC20_2
- MQTT payloads not adding to queue when MQTT is re-connecting (fixes #369)
- fix for HA topics with invalid command formats (#728)
- wrong position of values #723, #732
- OTA Upload via Web on OSX
- Rx and Tx quality % would sometimes show > 100

### Changed
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
- Allow both WiFi and Ethernet together
### Removed
- Shower Alert (disabled for now)
