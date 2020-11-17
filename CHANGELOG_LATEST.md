# Changelog

### Added
- function keys in editor: cursor, del, home, end. F1=help, F2=show, and other shortcuts
- SM100 pump working time and energy units
- heating curve parameters for RC300
- `wwonetime` for RC300 thermostat
- expose test framework via api (#611)
- SysLog has enable/disable flag in WebUI
- Add solar configuration telegrams (#616) [thanks @hpanther]

### Fixed
- mixer IPM pumpstatus
- mixer devices in HA were incorrectly named

### Changed
- optimized MQTT for HA to reduce heap fragmentation issues
- change syslog settings without reboot
- HA-config split in smaller blocks
- commands `fetch` and `publish [ha]` as call
- mqtt json package sizes
- renamed the command system info (which showed settings) to `settings`
- renamed the command system report (Which dumped debug info) to `info`
- Changing settings via web restarts only selected services
- renamed pio targets (esp8266-ci and esp32-ci for GitHub CI)

### Removed
- old shell and python build scripts
