# Changelog

See https://github.com/proddy/EMS-ESP/issues/632

### Added
- Power settings for ESP32
- Rx and Tx counts to Heartbeat MQTT payload
- Ethernet support (ESP32)
- id to info command to show only a heatingcircuit
- add sending devices that are not listed to 0x07

### Fixed
- telegrams matched to masterthermostat 0x18
- multiple roomcontrollers
- readback after write with delay (give ems-devices time to set the value)
- Thermostat ES72/RC20 device 66 to command-set RC20_2
- MQTT payloads not adding to queue when MQTT is re-connecting (fixes #369)

### Changed
- split `show values` in smaller packages (edited)
- extended length of IP/hostname from 32 to 48 chars (#676)
- check flowsensor for `tap_water_active`
- mqtt prefixed with `Base`
- count Dallas sensor fails
- switch from SPIFFS to LITTLEFS

### Removed

