# Changelog

### Added
- function keys in editor: cursor, del, home, end. F1=help, F2=show, and other shortcuts
- SM100 pump working time and energy units
- heating curve parameters for RC300
- `wwonetime` for RC300 thermostat

### Fixed
- mixer IPM pumpstatus

### Changed
- optimized MQTT for HA to reduce mem fragmentation issues
- change syslog settings without reboot
- HA-config split in smaller blocks
- commands `fetch` and `publish [ha]` as call

### Removed
- old scripts

