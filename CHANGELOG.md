# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.1.0] October 31 2020

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

## [2.0.1] September 13 2020

### Added
- Able to set individual MQTT publish intervals per device
- Option to automatically MQTT publish when device data is updated
- Immediately send out Rx read request after a successful write, and publish via MQTT
- Added clearer steps in documentation on how to erase & upload
- Show Boiler's pump modulation in Web
- Support parasite Dallas temperature sensors
- Improvements to `watch` command, including publishing the telegram to MQTT
- Support for analog measurements on a GPIO (fixed)
- New `read <device ID> <type ID>` command in console

### Fixed
- Sometimes the automatic upgrade from 1.9 to 2.0 bricked the ESP8266
- Thermostat `set master` wasn't preserved after restart
- Correctly detect Thermostat heating circuits in Home Assistant
- Logamatic TC100 reading of thermostat data (and other Easy devices)
- Rendering 3-byte parameters like the UBA uptime
- MM100/200 MQTT data would be mixed up between heating circuit and ww circuit
- External Dallas sensor support for DS18S20

### Changed
- Web user-interface improvements, table alignment and number formatting
- Spelling of disinfection in MQTT payload
- Many small minor code improvements and optimizations
- External dallas temperature sensors rounded to a single decimal point
- Syslog hostname always shown in Web

### Removed
- NO_LED build option

## [2.0.0] 29-08-2020

First version of v2 with
- Supporting both ESP8266 and ESP32 modules from Espressif
- A new multi-user Web interface (based on React/TypeScript)
- A new Console, accessible via Serial and Telnet
- Tighter security in both Web and Console. Admin privileges required to access core settings and commands.
- Support for Home Assistant MQTT Discovery (https://www.home-assistant.io/docs/mqtt/discovery/)
- Can be run standalone as an independent Access Point or join an existing WiFi network
- Easier first-time configuration via a web Captive Portal
- Supporting over 70 EMS devices (boilers, thermostats, solar modules, mixer modules, heat pumps, gateways)

See README.me for more details.

## [1.9.5] 30-04-2020

### Added
- Solar Module SM200 support
- Support writing to Junkers FR100 thermostats
- Support writing to RC100, Moduline 1000/1010 thermostats
- MM10 Mixing module support (thanks @MichaelDvP)
- MM200 warm water circuits (https://github.com/proddy/EMS-ESP/pull/315)
- Support for Moduline 200 and Sieger ES72 thermostats
- First implementation of writing to generic Junker Thermostats (thanks @Neonox31)
- Added model type (Buderus, Sieger, Junkers, Nefit, Bosch, Worcester) to device names
- `set master_thermostat <product id>` to choose with thermostat is master when there are multiple on the bus
- `boiler wwonetime` command from Telnet
- `set bus_id <ID>` to support multiple EMS-ESP circuits. Default is 0x0B to mimic a service key.
- `mqtt_nestedjson` option to disable multiple data records being nested into a single JSON string
- MQTT publish messages are queued and gracefully published every second to avoid TCP blocks
- Added features to WW messages (0x33, 0x34) to improve WW monitoring. (PR#338 by @ypaindaveine)
- Added mixing log and stub for EMS type 0xAC (PR#338 by @ypaindaveine)
- Added Thermostat retrieving settings (0xA5) (validated on RC30N) with MQTT support (thanks Yves @ypaindaveine. See #352)
- Merged with PR https://github.com/proddy/EMS-ESP/pull/366 from @MichaelDvP fixing RC20 and MM50

### Fixed
- set boiler warm water temp on Junkers/Bosch HT3
- fixed detection of the Moduline 400 thermostat
- RC35 setting temperature also forces the current select temp to change, irrespective of the mode

### Changed
- improved MQTT publishing to stop network flooding. `publish_time` of -1 is no publish, 0 is automatic otherwise its a time interval
- External sensors (like Dallas DS18*) are sent as a nested MQTT topic including their unique identifier
- `mqttlog` console command renamed to `mqttqueue` to only show the current publish queue
- `status` payload on start-up shows the IP and Version of EMS-ESP
- `thermostat mode` takes a string like manual,auto,heat,day,night,eco,comfort,holiday,nofrost
- `thermostat temp` also takes a mode string, e.g. `thermostat temp 20 heat`
- `queue` renamed to `txqueue`

### Removed
 - `autodetect scan`. Replaced with `devices scan` and `devices scan+` for deep scanning
 - `mqttlog all` and showing MQTT log in the web interface - no point showing history of previous mqtt publishes in ESP's precious memory. For debugging I recommend using MQTT Explorer or another external tool.

## [1.9.4] 15-12-2019

There are breaking changes in this release. See `publish_time` below and make sure you set this value to 0.

### Added

- Added `publish_always` forcing MQTT topics to be always sent regardless if the data hasn't changed
- Support for DHW once (OneTime water) heating command via MQTT [issue 195](https://github.com/proddy/EMS-ESP/issues/195)
- Added scripts to automatically build firmware images on every Commit/Pull and nightly builds using TravisCI
- Added option to WebUI to also download the latest development build
- Added build scripts for automated CI with TravisCI
- Implemented timezone support and automatic adjustment, also taking daylight saving times into account
- Added `kick` command to reset core services like NTP, Web, Web Sockets
- Added WiFi static IP (setting done in WebUI only)
- `log w <type_id>` for watching a specific telegram type ID
- initial support for EMS+ GB125s and MC110's
- Buderus RFM200 receiver

### Fixed

- Stability for some Wemos clones by decreasing wifi Tx strength and adding small delay

### Changed

- Debug log times show real internet time (if NTP enabled)
- `system` shows local time instead of UTC
- fixed version numbers of libraries in `platformio.ini`
- Normalized Heating modes to `off`, `manual`, `auto`, `night` and `day` to keep generic and not Home Assistant specific (like `heat`)
- Keeping Thermostat day/night modes separate from off/auto/manual, and setting this for the Junkers FR50
- Removed `publish_always`
- Changed NTP interval from 1 hour to 12 hours
- Refactored EMS device library to make it support multi-EMS devices easier (e.g. multiple thermostats)
- `autodetect deep` removed and replaced with `autodetect scan` for scanning known devices.
- MQTT data will be sent when new data arrives. So `publish_time` is used to force a publish at a given frequency (2 mins is default), or 0 for off.

### Removed

- thermostat scan and autodetect deep functions
- removed Event Logging to SPIFFS (worried about wearing). Replaced with SysLog.

## [1.9.3] 2019-10-26

### Added

- Report # TCP dropouts in the `system` command. These could be due to WiFI or MQTT disconnected.
- Added temp and mode to the MQTT `thermostat_cmd` topic
  
### Fixed

- vertical bar showing in WebUI sidebar menu using FireFox

### Changed

- Heartbeat MQTT payload is now in JSON
- platformio.ini targets. Use `debug` for custom builds.

## [1.9.2] 2019-10-19

#### Important! This build has breaking changes:
 - MQTT topics have changed. Use the `mqttlog` command to see the names of the subscriptions and the format of the payload data.
 - Home Assistant `.yaml` files need updating to reflect the recent MQTT changes
 - The web builder has been upgraded to use Gulp 4. Remove `tools/webfilesbuilder/node_modules` and re-install the libraries using `npm ci` from within the `tools/webfilesbuilder` folder

### Added

- Handling of Mixing Module MM100 Status Messages (thanks @kstaniek)
- Retrieve/Set thermostat mode for Junkers FW100/120 thermostats (thanks @Neonox31)
- Added sending of all Mixer Module data via MQTT (thanks @peclik)
- Improved handling of MQTT publish and subscribe errors
- Added MQTT QOS (`mqtt_qos`, default 0), Keep Alive (`mqtt_keepalive`, default 60 seconds) and Retain (`mqtt_retain`, default off) as parameters to both telnet and WebUI

### Fixed

- `publish` command also sends Dallas external temperature sensor values
- `log_events` setting wasn't persisted in config file

### Changed

- External dallas sensor values sent in MQTT payload as float values and not strings
- All MQTT topics for the Thermostat have the Heating Circuit appended (e.g. `thermostat_data1`). This includes the commands.
- Shower timer and shower alert and not MQTT published at boot up
- Heating Active logic change to use Selected Flow Temp of min 30 instead of 70 (https://github.com/proddy/EMS-ESP/issues/193)
- Cleaned up Telnet messages during bootup to only show key information.

### Removed

- Removed telnet command `shower timer` and `shower alert` to toggle the switches

## [1.9.1] 2019-10-05

### Added

- Support for multiple Heating Circuits - https://github.com/proddy/EMS-ESP/issues/162
- new `mqttlog` command also shows which MQTT topics it is subscribed too
- Optimized event log loading in web and added integrity checks on all config and log files during boot
- `autodetect quick` for detecting known devices from our database list
- `log_events` option, now optional to save the log events to SPIFFS

### Fixed

- fixed zero values (0.0) for setpoint temperature with the RC35 thermostat when in Auto mode - https://github.com/proddy/EMS-ESP/issues/180
- added check for corrupted event log, which could happen due to SPIFFS writing while UART is active
- made Junkers work again (broke in 1.9.0)

### Changed

- Web login password is now mandatory
- Faster detection of EMS devices on bus by using the 0x07 telegram instead of the brute-force scan
- Fixes to the default HA climate component .yaml file to support latest Home Assistance ('heat' added)
- Update documentation on MQTT and troubleshooting
- Slowed down firmware upload via the Web to prevent users rebooting too early
- Change way WiFi is initialized to prevent dual AP and Client
  
### Removed

- Removed `heating_circuit` config setting
- Removed showing the JSON config files when Saving from the Web

## [1.9.0] 2019-09-01

### Changed

- New web interface with more features showing Boiler, Thermostat, Solar Module and Heat Pump.
- Merged with @susisstrolch's TxMode2 branch for improved support for sending EMS packages. This is the default tx mode.
- Upgraded MyESP library optimizations for WiFi, AP and error handling
- `reboot` command renamed to `restart` to keep consistent with web interface
- Renamed `heartbeat` to `mqtt_heartbeat` in config settings
- Renamed MQTT topic "wwactivated" to "boiler_cmd_wwactivated"

### Fixed

- Handle Read and Write to EMS+ device logic changed, tested with RC3000

## [1.8.1] 2019-07-27

### Added

- Added back -DCRASH in Debug build target for capturing any ESP8266 stack dumps during crashes
- Web Interface, for checking stats and setting wifi credentials. See documentation for more details.
- reset firmware option. If the reset button on the ESP is pressed during boot up sequence (the LED is flashing very fast) all settings are erased and goes into AP mode.
- Added tx_mode back with options 0,1 and 2 until we've fixed option 2 that works for everyone and doesn't reset ESP
- More solar module data captured, thanks to @Vuego123
- Detect thermostat mode for EMS+ RC300/Moduline 3000
- MQTT message to set boiler flowtemp (`boiler_cmd_flowtemp`).

### Fixed

- Detecting unset values in the SPIFFS and setting default values
- Bosch Easy Connect wrongly classified as a thermostat
- Correctly handle telegrams who's size are exactly 32 bytes (e.g. 0x19 MonitorSlow)
- Telnet also available when in AP mode
- Handling of thermostat temperatures that were single bytes and couldn't exceed 25.5 (0xFF) degrees!

### Changed

- Improved handling of Solar Modules (thanks @Vuego123)
- `publish_wait` renamed to `publish_time`, a value of 0 means disabling all MQTT sending
- How signed shorts are handled such as the current and setpoint temps on RC300s
- Stopped automatic refresh of web page, which causes crashes/memory loss after a short time
- Support HA 0.96 climate component changes
- -DDEFAULT_NO_SERIAL changed to -DFORCE_SERIAL
- some code cleanups, removing NULLS and moving some things frond heap to stack to prevent memory fragmentation

## [1.8.0] 2019-06-15

### Added

- HeatPump support (e.g. the Nefit Enviline)
- new device: Buderus MM50 Mixer
- new devices: Junkers FW100 and ISM1 (thanks Vuego123)
- Improved Tx logic to support EMS, EMS+ and Heatronic 3 (thanks kwertie01, susisstrolch, philrich)
- MQTT heartbeat

### Fixed

- runtime exceptions with latest 2.5.1 arduino core

## [1.7.0] 2019-05-11

### Added

- EMS+ core support (thanks too @gl3nni3 for doing the first research)
- MQTT 'restart' topic to reboot ESP (thanks @balk77)
- Support for multiple thermostat heating circuits like the HC1/HC2 on a RC35, also via MQTT (thanks @lobocobra)
- `boiler flowtemp` command to set the flow temperature [(issue 59)](https://github.com/proddy/EMS-ESP/issues/59)
- added a test harness to try out response to various telegrams (test command)
- `tx_delay` setting for circuits where we needed to slow down Tx transmission
- new boiler: Nefit proline hrc 24 cw4 thermostat and Nefit Enviline heatpump
- new boiler: Buderus Logamax plus
- new thermostat: Buderus RC300 and RC310 thermostats, read-only [(issue 37)](https://github.com/proddy/EMS-ESP/issues/37)
- new thermostat: Junkers FR10, read-only [(issue 98)](https://github.com/proddy/EMS-ESP/issues/98)
- new devices: Buderus Web Gateway KM200, Buderus Solar Module SM100

### Changed

- `types` renamed to `devices` to also show all detected devices
- renamed `silent_mode` to `listen_mode`
- increased Tx queue to 100

## [1.6.0] 2019-03-24

### Added

- `system` command to show ESP8266 stats
- `crash` command to see stack of last system crash, with .py files to track stack dump (compile with `-DCRASH`)
- publish dallas external temp sensors to MQTT (thanks @JewelZB)
- shower timer and shower alert options available via set commands
- added support for warm water modes Hot, Comfort and Intelligent [(issue 67)](https://github.com/proddy/EMS-ESP/issues/67)
- added `set publish_time` to set how often to force a publish of MQTT
- support for SM10 Solar Module including MQTT [(issue 77)](https://github.com/proddy/EMS-ESP/issues/77)
- `refresh` command to force a fetch of all known data from the connected EMS devices

### Fixed

- incorrect rendering of null temperature values (the -3200 degrees issue)
- OTA is more stable
- Added a hack to overcome WiFi power issues in arduino core 2.5.0 libraries causing constant wifi re-connects
- Performance issues with telnet output

### Changed

- included various fixes and suggestions from @nomis
- upgraded MyESP library with many optimizations
- `test_mode` renamed to `silent_mode`
- `set wifi` replaced with `set wifi_ssid` and `set wifi_password` to allow values with spaces
- EMS values are stored in the raw format and only converted to strings when displayed or published, removing the need for parsing floats
- All floating point temperatures are to one decimal place [(issue 79)](https://github.com/proddy/EMS-ESP/issues/79)

## [1.5.6] 2019-03-09

### Added

- test_mode option

### Changed

- upgraded MyESP library
- minor changes

## [1.5.5] 2019-03-07

### Fixed
- Support the latest ArduinoJson v6 and espressif8266 2.0.4 libraries (in PlatformIO do a `pio lib update` and `pio update`)

### Changed

- MQTT keep alive to 2 minutes (60 seconds was just too short for slower networks)
- Improved MQTT startup time
- Setting wifi or mqtt settings are immediate, no need to restart the ESP
- Text changes in the help

### Added
- Show if MQTT is connected
- Show version of MyESP (the custom MQTT, Wifi, OTA, MDNS, Telnet library)
- EMS-OT OpenTherm connector

## [1.5.4] 2019-03-03

### Changed

- MQTT keep alive changed from 5 minutes to 1 minute

### Added
- Callback for OTA. This is used to disable EMS bus during a firmware OTA update, which caused problems with the latest ESP89266 core libraries
- Added rough estimate of WiFi signal strength to info page
- Added the build time & date to the info page (optional in platformio.ini)
  
## [1.5.3] 2019-02-22

### Changed

- Support for latest esp8266 arduino core version [2.5.0](https://github.com/esp8266/Arduino/releases/tag/2.5.0) and  platform espressif8266 version 2.0.0
- Added board type to the info screen

### Added

- Improved MQTT LWT (Last Will Testament). Uses payload called 'online' and 'offline'. See https://github.com/proddy/EMS-ESP/issues/57
- Added ESP32 support to MyESP library
- Added Bosch Easy thermostat, Buderus Logamax U122
- Support for changing boiler wwtemp via MQTT (merge #58 from egrekov). thanks!

### Removed

- Custom MDNS support. Now handled much better in the new esp core under OTA

## [1.5.2] 2019-02-04

### Changed

- Change wifi settings using the `set wifi <ssid> <password>` command

### Added

- Added incoming MQTT "TOPIC_BOILER_WWACTIVATED" to set the warm water on/off. Payload is 1 or 0. See [issue](https://github.com/proddy/EMS-ESP/issues/46#issuecomment-460375689).
- Added the list of all MQTT topics to the README file

## [1.5.1] 2019-02-03

### Fixed

- issue with Serial monitoring conflicting with UART when both running
- Fixed typo with -D settings in the example platformio.ini

### Changed

- `thermostat temp` now except floats (e.g. 20.5). Some thermostats may round up or down if they use 0.5 intervals.

## [1.5.0] 2019-02-03

### Added

- Support for RC10 thermostat
- New command `set serial`

### Changed

- Improved Tx logic. Retries are more efficient and startup is faster and less error prone.
- "# Rx telegrams" and "# Tx telegrams" show number of successful Reads and Writes initiated by the user or automatically by the code. This makes it easy to see if the Tx is working.
- Some refactoring in preparation for the EMS+ support coming soon

### Removed

- Removed the `poll` and `tx` commands
- `DEBUG_SUPPORT`. Now controlled with the 'set serial' command
- removed MQTT and WIFI settings from my_config.h. These have to be set either within the application (using set) or hardcoded in platformio.ini You can now check in `my_config.h` without everyone seeing your passwords! 
- TxCapable removed from `info` 

## [1.4.1] 2019-01-29

### Added

- The led pin, dallas pin and both thermostat and boiler type IDs can be set with the application, and stored.

### Changed

- some minor improvements to autodetect

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
- Support for RC20RF and RFM20 (https://github.com/proddy/EMS-ESP/issues/18)
- Moved all EMS device information into a separate file `ems_devices.h` so no longer need to touch `ems.h`
- Telnet commands can be strings now and output is suspended when typing

### Removed

- Removed SHOWER_TEST
- Removed WIFI and MQTT credentials from the platformio.ini file

### Added

- Settings are saved and loaded from the ESP8266's file system (SPIFFS). Can be set using the 'set' command
- Improved support when in Access Point mode (192.168.4.1)
- pre-built firmware's are back

## [1.2.4] 2019-01-04

### Changed

- Scanning known EMS Devices now ignores duplicates (https://github.com/proddy/EMS-ESP/pull/30)
- ServiceCode stored as a two byte char
- Support for RC20RF and RFM20 (https://github.com/proddy/EMS-ESP/issues/18)

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
- Removed distinguishing between noise on the line and corrupt telegrams (https://github.com/proddy/EMS-ESP/issues/24)

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
