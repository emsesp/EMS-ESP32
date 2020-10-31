# Changelog

### Added
- boiler `heatingactivated`, automatic select parameter telegrams for write
- boiler `wWType` parameter, in Console and MQTT
- support for uploading compressed firmware binaries in web UI
- setting to manually override the MQTT retain flag
- New API via HTTP REST API to read and set values. See https://emsesp.github.io/docs/#/API
- `show commands` command
- exporting of system settings using the `system info` command in Web and Console. Added link into the Web's Settings page.
- setting to change how booleans are rendered in MQTT (on/off, true/false, 1/0)
- enable ADC setting, add boiler circulation commands, add thermostat RC300 summermodes
- Added all device info to web UI for Thermostat and Boiler
- Added all device values to Home Assistant MQTT Discovery under separate devices and entities
- Show Rx and Tx quality in Console and Web UI
- Added button and tooltip to EMS Devices in Web
- wwtemp and wwtemplow to MQTT, Console and Web
- summer, winter modes for the CW400 thermostat
- new command under system called `report`. http://ems-esp/api?device=system&cmd=report to generate a report log for troubleshooting
- thermostat error codes
- Console command `publish ha` to also force the creation of the Home Assistant MQTT Discovery topics
- Heat pump values (dew temperature and relative air humidity)
- Console up key to repeat last command
- added RC300 floordrying, building, damped temperature 

### Fixed
- fix wwontime readback
- fixed support for RC300 via MQTT commands (#505)
- Some minor optimizations to memory handling in the MQTT service
- Prevent MQTT from publishing empty json payloads
- Accurate detection of warm water and heating (#515)
- Fix writing to the Junkers FR120 thermostat
- support for changing summermode
- added missing `heatingtype` to thermostat data
- handle incomming ems+ read requests, ignore F7 telegrams with 3byte-id
- fix month for setting clock from NTP

### Changed
- renamed wWCircPumpType to wWChargeType
- Installation and Configuration notes moved to the official EMS-ESP documentation site
- `call` commands can be done from the Console root for all devices
- Updated EMS-ESP official documentation (https://emsesp.github.io/docs/#/)
- JWT Secret renamed to Super User Password
- EMS Devices in Web UI shows button and tooltip to remind users they can click on a device
- MQTT topic name changes (see doc)
- Mixing renamed to Mixer

### Removed
- Console contexts for thermostat and boiler
- Removed option to enable/disable the MQTT Heartbeat. It's always on.

