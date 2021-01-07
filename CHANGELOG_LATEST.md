# Changelog

See https://github.com/proddy/EMS-ESP/issues/632

### Added
- Power settings for ESP32
- Rx and Tx counts to Heartbeat MQTT payload

### Fixed
- telegrams matched to masterthermostat 0x18

### Changed
- split `show values` in smaller packages (edited)
- extended length of IP/hostname from 32 to 48 chars (#676)
- check flowsensor for `tap_water_active`

### Removed

