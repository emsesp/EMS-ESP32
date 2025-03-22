# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [3.7.2] 22 March 2025

## Added

- change enum_heatingtype for remote control [#2268](https://github.com/emsesp/EMS-ESP32/issues/2268)
- system service commands [#2182](https://github.com/emsesp/EMS-ESP32/issues/2182)
- read 0x02A5 for thermostat CT200 [#2277](https://github.com/emsesp/EMS-ESP32/issues/2277)
- add "duplicate" option to Custom Entities [#2266](https://github.com/emsesp/EMS-ESP32/discussion/2266)
- mask bits for bool custom entities
- thermostat `reduce threshold` [#2288](https://github.com/emsesp/EMS-ESP32/issues/2288)
- thermostat `absent` [#1957](https://github.com/emsesp/EMS-ESP32/issues/1957)
- CR11 thermostat [#2295](https://github.com/emsesp/EMS-ESP32/issues/2295)
- Show ESP32's CPU temp in Hardware Status
- vacation mode for the CR50 [#2403](https://github.com/emsesp/EMS-ESP32/issues/2403)
- new Console command "set admin password" to set WebUI admin password
- support nested conditions in scheduler [#2451](https://github.com/emsesp/EMS-ESP32/issues/2451)
- allow mixed case in scheduler expressions [#2457](https://github.com/emsesp/EMS-ESP32/issues/2457)
- Suprapur-o [#2470](https://github.com/emsesp/EMS-ESP32/issues/2470)

## Fixed

- long numbers of custom entities [#2267](https://github.com/emsesp/EMS-ESP32/issues/2267)
- modbus command path to `api/` [#2276](https://github.com/emsesp/EMS-ESP32/issues/2276)
- info command for devices without entity-commands [#2274](https://github.com/emsesp/EMS-ESP32/issues/2274)
- CW100 settings telegram 0x241 [#2290](https://github.com/emsesp/EMS-ESP32/issues/2290)
- modbus signed 8bit values [#2294](https://github.com/emsesp/EMS-ESP32/issues/2294)
- thermostat date [#2313](https://github.com/emsesp/EMS-ESP32/issues/2313)
- Updated unknown compressor stati "enum_hpactivity" [#2311](https://github.com/emsesp/EMS-ESP32/pull/2311)
- Underline Tab headers in WebUI
- console unit tests fixed due to changed shell output
- tx-queue overflow in some heatpump systems [#2455](https://github.com/emsesp/EMS-ESP32/issues/2455)

## Changed

- show operation in pretty telegram between src and dst [#2263](https://github.com/emsesp/EMS-ESP32/discussions/2263)
- update eModbus to 1.7.2 [#2254](https://github.com/emsesp/EMS-ESP32/issues/2254)
- modbus timeout default to 300 sec, change setting from ms to sec [#2254](https://github.com/emsesp/EMS-ESP32/issues/2254)
- update AsyncTCP and ESPAsyncWebServer to latest versions
- update Arduino pio platform to 3.10.0 and optimized flash using build flags
- Version checker in WebUI improved
- rename `remoteseltemp` to `cooltemp` [#2456](https://github.com/emsesp/EMS-ESP32/issues/2456)

## [3.7.1] 29 November 2024

## Added

- include HA "unit_of_meas", "stat_cla" and "dev_cla" attributes for Number sensors [#2149](https://github.com/emsesp/EMS-ESP32/issues/2149)
- Bosch CS6800i AW - Silent Mode + Electrical Power Reduction (HP) [#2147](https://github.com/emsesp/EMS-ESP32/issues/2147)
- `/api/system/showeralert` and `/api/system/showertimer` [#2182](https://github.com/emsesp/EMS-ESP32/issues/2182)
- MX400 [#2198](https://github.com/emsesp/EMS-ESP32/issues/2198)
- SM200 values [#2212](https://github.com/emsesp/EMS-ESP32/discussions/2212)

## Fixed

- Modbus integration in 3.7.0 missing offset [#2148](https://github.com/emsesp/EMS-ESP32/issues/2148)
- fix changing TZ in NTPsettings without clearing enable+server, added DST support [#2142](https://github.com/emsesp/EMS-ESP32/issues/2142)
- Support MQTT Discovery (AD) with Domoticz [#2177](https://github.com/emsesp/EMS-ESP32/issues/2177)
- wwExtra (dhw extra) changed from temperature reading to number
- auxheaterstatus [#2192](https://github.com/emsesp/EMS-ESP32/issues/2192)
- lastCode character check [#2189](https://github.com/emsesp/EMS-ESP32/issues/2189)
- reading too many telegram parts
- heatpump cost UOMs [#2188](https://github.com/emsesp/EMS-ESP32/issues/2188)
- analog dac output and inputs on dac pins [#2201](https://github.com/emsesp/EMS-ESP32/discussions/2201)
- api memory leak [#2216](https://github.com/emsesp/EMS-ESP32/issues/2216)
- modbus multiple mixers [#2229](https://github.com/emsesp/EMS-ESP32/issues/2229)
- Last Will (LWT) not set on MQTT Connect [#2247](https://github.com/emsesp/EMS-ESP32/issues/2247)

## Changed

- name of wwstarts2 [#2217](https://github.com/emsesp/EMS-ESP32/discussions/2217)

## [3.7.0] 27 October 2024

## **IMPORTANT! BREAKING CHANGES with 3.6.5**

- "ww" and "wwc" has been renamed to "dhw". It is nested JSON object in both the MQTT and API outputs. The old prefix has also been removed from MQTT topics ([#1634](https://github.com/emsesp/EMS-ESP32/issues/1634)). This will impact historical data in home automation systems like Home Assistant and IOBroker. To preserve the current value of dhw energy (was previously nrgww) refer to this issue [#1938](https://github.com/emsesp/EMS-ESP32/issues/1938).
- dhw entities from the MM100/SM100 have been moved under a new Device called 'water'.
- The automatically generated temperature sensor ID has replaced dashes (`-`) with underscores (`_`) to be compatible with Home Assistant.
- `api/system/info` has it's JSON key names changed to camelCase syntax.

For more details go to [docs.emsesp.org](https://docs.emsesp.org/).

## Added

- some more entities for dhw with SM100 module
- thermostat second dhw circuit [#1634](https://github.com/emsesp/EMS-ESP32/issues/1634)
- remote thermostat emulation for RC100H, RC200 and FB10 [#1287](https://github.com/emsesp/EMS-ESP32/discussions/1287), [#1602](https://github.com/emsesp/EMS-ESP32/discussions/1602), [#1551](https://github.com/emsesp/EMS-ESP32/discussions/1551)
- heatpump dhw stop temperatures [#1624](https://github.com/emsesp/EMS-ESP32/issues/1624)
- reset history [#1695](https://github.com/emsesp/EMS-ESP32/issues/1695)
- heatpump entities `fan` and `shutdown` [#1690](https://github.com/emsesp/EMS-ESP32/discussions/1690)
- mqtt HA-mode 3 for v3.6 compatible HA entities, set on update v3.6->v3.7
- HP input states [#1723](https://github.com/emsesp/EMS-ESP32/discussions/1723)
- holiday settings for rego 3000  [#1735](https://github.com/emsesp/EMS-ESP32/issues/1735)
- Added scripts for OTA (scripts/upload.py and upload_cli.py) [#1738](https://github.com/emsesp/EMS-ESP32/issues/1738)
- timeout for remote thermostat emulation [#1680](https://github.com/emsesp/EMS-ESP32/discussions/1680), [#1774](https://github.com/emsesp/EMS-ESP32/issues/1774)
- CR120 thermostat as own model() [#1779](https://github.com/emsesp/EMS-ESP32/discussions/1779)
- modules - external linkable module library [#1778](https://github.com/emsesp/EMS-ESP32/issues/1778)
- scheduler onChange and Conditions [#1806](https://github.com/emsesp/EMS-ESP32/issues/1806)
- make remote control timeout editable [#1774](https://github.com/emsesp/EMS-ESP32/issues/1774)
- added extra pump characteristics (mode and pressure for EMS+) by @SLTKA [#1802](https://github.com/emsesp/EMS-ESP32/pull/1802)
- allow device name to be customized [#1174](https://github.com/emsesp/EMS-ESP32/issues/1174)
- Modbus support by @mheyse [#1744](https://github.com/emsesp/EMS-ESP32/issues/1744)
- System Message command [#1854](https://github.com/emsesp/EMS-ESP32/issues/1854)
- scheduler can use web get/post for values and commands [#1806](https://github.com/emsesp/EMS-ESP32/issues/1806)
- RT800 remote emulation [#1867](https://github.com/emsesp/EMS-ESP32/issues/1867)
- RC310 cooling parameters [#1857](https://github.com/emsesp/EMS-ESP32/issues/1857)
- command `api/device/entities` [#1897](https://github.com/emsesp/EMS-ESP32/issues/1897)
- switchprogmode [#1903](https://github.com/emsesp/EMS-ESP32/discussions/1903)
- autodetect and download firmware upgrades via the WebUI
- command 'show log' that lists out the current weblog buffer, showing last messages.
- default web log buffer to 25 lines for ESP32s with no PSRAM
- try and determine correct board profile if none is set during boot
- auto Scroll in WebLog UI - reduced delay so incoming logs are faster
- uploading custom support info, shown to Guest users in Help page [#2054](https://github.com/emsesp/EMS-ESP32/issues/2054)
- feature: Dashboard showing all data (favorites, sensors, custom) [#1958](https://github.com/emsesp/EMS-ESP32/issues/1958)
- entity for low-temperature boilers pump start temp (pumpOnTemp) #2088 [#2088](https://github.com/emsesp/EMS-ESP32/issues/2088)
- internal ESP32 temperature sensor on the S3 [#2077](https://github.com/emsesp/EMS-ESP32/issues/2077)
- MQTT status topic (used in connect and last will) set to Retain [#2086](https://github.com/emsesp/EMS-ESP32/discussions/2086)
- Czech language [2096](https://github.com/emsesp/EMS-ESP32/issues/2096)
- Developer Mode and send EMS Read Commands from WebUI [#2116](https://github.com/emsesp/EMS-ESP32/issues/2116)
- Scheduler functions [#2115](https://github.com/emsesp/EMS-ESP32/issues/2115)
- Set device custom name from telegram 0x01 [#2073](https://github.com/emsesp/EMS-ESP32/issues/2073)

## Fixed

- remote thermostat emulation for RC200 on Rego2000/3000 thermostats [#1691](https://github.com/emsesp/EMS-ESP32/discussions/1691)
- log shows data for F7/F9 requests
- Detection of LittleFS for factory setting wasn't working
- Check for bad GPIOs with Ethernet before the ethernet is initialized
- Show values with factor 50 on webUI [#2064](https://github.com/emsesp/EMS-ESP32/issues/2064)
- Rendering of values between -1 and 0
- Value for 32bit times not-set [#2109](https://github.com/emsesp/EMS-ESP32/issues/2109)

## Changed

- use flag for BC400 compatible thermostats, manage different mode settings
- use factory partition for 16M flash
- store digital out states to nvs
- Refresh UI - moving settings to one location [#1665](https://github.com/emsesp/EMS-ESP32/issues/1665)
- rename DeviceValueTypes, add UINT32 for custom entities
- dynamic register dhw circuits for thermostat
- removed OTA feature [#1738](https://github.com/emsesp/EMS-ESP32/issues/1738)
- added shower min duration [#1801](https://github.com/emsesp/EMS-ESP32/issues/1801)
- Include TXT file along with the generated CSV for Device Data export/download
- thermostat/remotetemp as command [#1835](https://github.com/emsesp/EMS-ESP32/discussions/1835)
- temperaturesensor id notation with underscore [#1794](https://github.com/emsesp/EMS-ESP32/discussions/1794)
- Change key-names in JSON to be compliant and consistent [#1860](https://github.com/emsesp/EMS-ESP32/issues/1860)
- Updates to webUI [#1920](https://github.com/emsesp/EMS-ESP32/issues/1920)
- Correct firmware naming #1933 [#1933](https://github.com/emsesp/EMS-ESP32/issues/1933)
- Don't start Serial console if not connected to a Serial port. Will initiate manually after a CTRL-C/CTRL-S  
- WebLog UI matches color schema of the terminal console correctly
- Updated Web libraries, ArduinoJson
- Help page doesn't show detailed tech info if the user is not 'admin' role [#2054](https://github.com/emsesp/EMS-ESP32/issues/2054)
- removed system command `allvalues` and moved to an action called `export`
- Show ems-esp internal devices in device list of system/info
- Scheduler and mqtt run async on systems with psram
- Show IPv6 address type (local/global/ula) in log

## [3.6.5] March 23 2024

## **IMPORTANT! BREAKING CHANGES**

- The Wifi Tx Power setting in Network Settings will be reset to Auto

## Added

- thermostat boost mode and boost time [#1446](https://github.com/emsesp/EMS-ESP32/issues/1446)
- heatpump energy meters [#1463](https://github.com/emsesp/EMS-ESP32/issues/1463)
- heatpump max power [#1475](https://github.com/emsesp/EMS-ESP32/issues/1475)
- checkbox for MQTT-TLS enable [#1474](https://github.com/emsesp/EMS-ESP32/issues/1474)
- added SK (Slovak) language. Thanks @misa1515
- CPU info [#1497](https://github.com/emsesp/EMS-ESP32/pull/1497)
- Show network hostname in Web UI under Network Status
- Improved HA Discovery so each section (EMS device, Scheduler, Analog, Temperature, Custom, Shower) have their own section
- boiler Bosch C1200W, id 12, [#1536](https://github.com/emsesp/EMS-ESP32/issues/1536)
- mixer MM100 telegram 0x2CC [#1554](https://github.com/emsesp/EMS-ESP32/issues/1554)
- boiler hpSetDiffPressure [#1563](https://github.com/emsesp/EMS-ESP32/issues/1563)
- custom variables [#1423](https://github.com/emsesp/EMS-ESP32/issues/1423)
- weather compensation [#1642](https://github.com/emsesp/EMS-ESP32/issues/1642)
- env and partitions for DevKitC-1-N32R8 [#1635](https://github.com/emsesp/EMS-ESP32/discussions/1635)
- command `restart partitionname` and button long press to start with other partition [#1657](https://github.com/emsesp/EMS-ESP32/issues/1657)
- command `set service  <mqtt|ota|ntp|ap> <enable|disable>` [#1663](https://github.com/emsesp/EMS-ESP32/issues/1663)

## Fixed

- exhaust temperature for some boilers
- add back boil2hyst [#1477](https://github.com/emsesp/EMS-ESP32/issues/1477)
- subscribed MQTT topics not detecting changes by EMS-ESP [#1494](https://github.com/emsesp/EMS-ESP32/issues/1494)
- changed HA name and grouping to be consistent [#1528](https://github.com/emsesp/EMS-ESP32/issues/1528)
- MQTT autodiscovery in Domoticz not working [#1360](https://github.com/emsesp/EMS-ESP32/issues/1528)
- dhw comfort for new ems+, [#1495](https://github.com/emsesp/EMS-ESP32/issues/1495)
- added writeable icon to Web's Custom Entity page for each entity shown in the table
- Wifi Tx Power not adjusted [#1614](https://github.com/emsesp/EMS-ESP32/issues/1614)
- MQTT discovery of custom entity doesn't consider type of data [#1587](https://github.com/emsesp/EMS-ESP32/issues/1587)
- WiFi TxPower wasn't correctly used. Added an 'Auto' setting, which is the default.
- dns w/wo IPv6 [#1644](https://github.com/emsesp/EMS-ESP32/issues/1644)

## Changed

- HA don't set entity_category to Diagnostic/Configuration for EMS entities [#1459](https://github.com/emsesp/EMS-ESP32/discussions/1459)
- upgraded ArduinoJson to 7.0.0 #1538 and then 7.0.2
- small changes to the API for analog and temperature sensors
- Length of mqtt Broker adress [#1619](https://github.com/emsesp/EMS-ESP32/issues/1619)
- C++ optimizations - see <https://github.com/emsesp/EMS-ESP32/pull/1615>
- Send MQTT heartbeat immediately after connection [#1628](https://github.com/emsesp/EMS-ESP32/issues/1628)
- 16MB partitions with second nvs, larger FS, Coredump, optional factory partition
- stop fetching empty telegrams after 5 min

## [3.6.4] November 24 2023

## **IMPORTANT! BREAKING CHANGES**

Writeable Text entities have moved from type `sensor` to `text` in Home Assistant to make them also editable within an HA dashboard. Examples are `datetime`, `holidays`, `switchtime`, `vacations`, `maintenancedate`. You will need to manually remove any old discovery topics from your MQTT broker using an application like MQTT Explorer.

## Added

- humidity for ventilation devices
- telegrams for RC100H, hc2, etc. (seen on discord, not tested)
- names for BC400, GB192i.2, read temperatures for low loss header and heatblock [#1317](https://github.com/emsesp/EMS-ESP32/discussions/1317)
- option for `forceheatingoff` [#1262](https://github.com/emsesp/EMS-ESP32/issues/1262)
- remote thermostat emulation RC100H for RC3xx [#1278](https://github.com/emsesp/EMS-ESP32/discussions/1278)
- shower_data MQTT payload contains the timestamp [#1329](https://github.com/emsesp/EMS-ESP32/issues/1329)
- HA discovery for writeable text entities [#1337](https://github.com/emsesp/EMS-ESP32/pull/1377)
- autodetect board_profile, store in nvs, add telnet command option, add E32V2
- heat pump high res energy counters [#1348, #1349. #1350](https://github.com/emsesp/EMS-ESP32/issues/1348)
- optional bssid in network settings
- extension module EM100 [#1315](https://github.com/emsesp/EMS-ESP32/discussions/1315)
- digital_out with new options for polarity and startup state
- added 'system allvalues' command that dumps all the EMS device values, plus sensors and any custom entities

## Fixed

- remove command `remoteseltemp`, thermostat accept it only from remote thermostat
- shower_data MQTT payload contains the timestamp [#1329](https://github.com/emsesp/EMS-ESP32/issues/1329)
- fixed helper text in Web Device Entity dialog box for numerical ranges
- MQTT base with paths not working in HA [#1393](https://github.com/emsesp/EMS-ESP32/issues/1393)
- set/read thermostat mode for RC100-RC300, [#1440](https://github.com/emsesp/EMS-ESP32/issues/1440) [#1442](https://github.com/emsesp/EMS-ESP32/issues/1442)
- some setting commands for ems-boiler have used wrong ems+ telegram in 3.6.3

## Changed

- update to platform 6.4.0, arduino 2.0.14 / idf 4.4.6
- small changes for arduino 3.0.0 / idf 5.1 compatibility (not backward compatible to platform 6.3.2 and before)
- AP start after 10 sec, stay until station/eth connected
- tested wifi-all-channel-scan (3.6.3-dev4 a-e), removed again because of connect issues
- mqtt disconnect stops queue

## [3.6.2] October 1 2023

## **IMPORTANT! BREAKING CHANGES**

## Added

- Power entities
- Optional input of BSSID for AP connection
- Return empty json if no entries in scheduler/custom/analogsensor/temperaturesensor

## Fixed

- Wifi full scan to get strongest AP
- Add missing dhw tags
- Sending a dash/- to the Reset command doesn't return an error [#1308](https://github.com/emsesp/EMS-ESP32/discussions/1308)

## Changed

- MQTT queue max 300 messages, check heap and maxAlloc
- API call commands are logged as WARN in the log
- Reset Command renamed to 'reset' in lowercase in EN

## [3.6.1] September 9 2023

## **IMPORTANT! BREAKING CHANGES**

- `shower_data` MQTT topic shows duration is seconds (was previously a full english sentence)

## Added

- Show WiFi rssi in Network Status Page, show quality as color

## Fixed

- Issue in espMqttClient causing a memory leak when MQTT broker is disconnected due to network unavailability [#1264](https://github.com/emsesp/EMS-ESP32/issues/1264)
- Using MQTT enum values correctly formatted in MQTT Discovery [#1280](https://github.com/emsesp/EMS-ESP32/issues/1280)

## Changed

- MQTT free mem check set to 60 kb
- Small cosmetic changes to Searching in Customization web page
- Updated to espressif32@6.4.0

# [3.6.0] August 13 2023

## **IMPORTANT! BREAKING CHANGES**

There are breaking changes between 3.5.x and earlier versions of 3.6.0. Please read carefully before applying the update.

- The sensors have been renamed. `dallassensor` is now `temperaturesensor` in the MQTT topic and named `ts` in the Customizations file. Likewise `analogs` is now `analogsensor` in MQTT and called `as` in the Customizations file. If you have previous customizations you will need to manually update by downloading, changing the JSON file and uploading. It's also recommended cleaning up any old MQTT topics from your broker using an application like MQTTExplorer.

## Added

- Workaround for better Domoticz MQTT integration? [#904](https://github.com/emsesp/EMS-ESP32/issues/904)
- Show MAC address without connecting to network enhancement [#933](https://github.com/emsesp/EMS-ESP32/issues/933)
- Warn user in WebUI of unsaved changes [#911](https://github.com/emsesp/EMS-ESP32/issues/911)
- Detect old Tado thermostat, device-id 0x19, no entities
- Some more HM200 entities [#500](https://github.com/emsesp/EMS-ESP32/issues/500)
- Added Scheduler [#701](https://github.com/emsesp/EMS-ESP32/issues/701)
- Added Custom Entities read/write from EMS bus
- Build S3 binary with github actions
- Greenstar HIU [#1158](https://github.com/emsesp/EMS-ESP32/issues/1158)
- AM200 code 10 [#1161](https://github.com/emsesp/EMS-ESP32/issues/1161)
- Ventilation device (Logavent HRV176) [#1172](https://github.com/emsesp/EMS-ESP32/issues/1172)
- Turn ETH off on wifi connect [#1167](https://github.com/emsesp/EMS-ESP32/issues/1167)
- Support for multiple EMS-ESPs with HA [#1196](https://github.com/emsesp/EMS-ESP32/issues/1196)
- Italian translation [#1199](https://github.com/emsesp/EMS-ESP32/issues/1199)
- Turkish language support [#1076](https://github.com/emsesp/EMS-ESP32/issues/1076)
- Buderus GB182 - HC1 mode change not work bug [#1193](https://github.com/emsesp/EMS-ESP32/issues/1193)
- Minimal flow temperature enhancement [#1192](https://github.com/emsesp/EMS-ESP32/issues/1192)
- Roomtemperature Switching Difference enhancement [#1191](https://github.com/emsesp/EMS-ESP32/issues/1191)
- Dew Point Temperature Difference enhancement [#1190](https://github.com/emsesp/EMS-ESP32/issues/1190)
- Control of heating circuit mode enhancement [#1187](https://github.com/emsesp/EMS-ESP32/issues/1187)
- Warn user in WebUI of unsaved changes enhancement [#911](https://github.com/emsesp/EMS-ESP32/issues/911)
- Create safebuild app to fit into factory partition to give ESP32 more flash memory enhancement [#608](https://github.com/emsesp/EMS-ESP32/issues/608)
- Support ESP32 S2, C3 mini and S3 [#605](https://github.com/emsesp/EMS-ESP32/issues/605)
- Support Buderus AM200 [#1161](https://github.com/emsesp/EMS-ESP32/issues/1161)
- Custom telegram handler [#1155](https://github.com/emsesp/EMS-ESP32/issues/1155)
- Added support for TLS in MQTT (ESP32-S3 only) [#1178](https://github.com/emsesp/EMS-ESP32/issues/1178)
- Boardprofile BBQKees Gateway S3
- Custom entity type RAW [#1212](https://github.com/emsesp/EMS-ESP32/discussions/1212)
- API command response [#1212](https://github.com/emsesp/EMS-ESP32/discussions/1212)

## Fixed

- HA-discovery for analog sensor commands [#1035](https://github.com/emsesp/EMS-ESP32/issues/1035)
- Enum order of RC3x nofrost mode
- Heartbeat interval
- Exhaust temperature always zero on GB125/MC110/RC310 bug [#1147](https://github.com/emsesp/EMS-ESP32/issues/1147)
- thermostat modetype is not changing when mode changes (e.g. to night) bugSomething isn't working [#1098](https://github.com/emsesp/EMS-ESP32/issues/1098)
- NTP: cant apply changed timezone [#1182](https://github.com/emsesp/EMS-ESP32/issues/1182)
- Missing Status of VS1 for Buderus SM200 enhancement [#1034](https://github.com/emsesp/EMS-ESP32/issues/1034)
- Allowed gpios for S3

## Changed

- Optional upgrade to platform-espressif32 6.3.0 (after 5.3.0) [#862](https://github.com/emsesp/EMS-ESP32/issues/862)
- Use byte 3 for detection RC30 active heatingcircuit [#786](https://github.com/emsesp/EMS-ESP32/issues/786)
- Write repeated selflowtemp if tx-queue is empty without verify [#954](https://github.com/emsesp/EMS-ESP32/issues/954)
- HA discovery recreate after disconnect by device [#1067](https://github.com/emsesp/EMS-ESP32/issues/1067)
- File upload: check flash size (overflow) instead of filesize
- Improved HA Discovery so previous configs no longer need to be removed when starting [#1077](https://github.com/emsesp/EMS-ESP32/pull/1077) (thanks @pswid)
- Enlarge UART-Stack to 2,5k
- Retry timeout for Mqtt-QOS1/2 10seconds
- Optimize WebUI rendering when using Dialog Boxes [#1116](https://github.com/emsesp/EMS-ESP32/issues/1116)
- Optimize Web libraries to reduce bundle size (3.6.x) [#1112](https://github.com/emsesp/EMS-ESP32/issues/1112)
- Use [espMqttClient](https://github.com/bertmelis/espMqttClient) with integrated queue [#1178](https://github.com/emsesp/EMS-ESP32/issues/1178)
- Move Sensors from Web dashboard to it's own tab enhancement [#1170](https://github.com/emsesp/EMS-ESP32/issues/1170)
- Optimize WebUI dashboard data [#1169](https://github.com/emsesp/EMS-ESP32/issues/1169)
- Replace React core library with Preact to save on memory footprint
- Response to `system/send` raw reads gives combined data for telegrams with more parts

# [3.5.0] February 6 2023

## **IMPORTANT! BREAKING CHANGES**

- When upgrading to v3.5 for the first time from v3.4 on a BBQKees Gateway board you will need to use the [EMS-EPS Flasher](https://github.com/emsesp/EMS-ESP-Flasher/releases) to correctly re-partition the flash. Make sure you backup the settings and customizations from the WebUI (System->Upload/Download) and restore after the upgrade.

## Added

- Translations in Web UI and all device entity names (DE, NL, SV, PL, NO, FR) [#22](https://github.com/emsesp/EMS-ESP32/issues/22)
- Add support for Lolin C3 mini [#620](https://github.com/emsesp/EMS-ESP32/pull/620)
- Add support for ESP32-S2 [#667](https://github.com/emsesp/EMS-ESP32/pull/667)
- Add devices: Greenstar 30Ri boiler, Junkers FW500 thermostat, Buderus BC30 controller
- Add program memory info
- Add mqtt queue and connection infos
- Adapt min/max if ems-value is not in this range
- Add heat pump settings for inputs and limits [#600](https://github.com/emsesp/EMS-ESP32/issues/600)
- Add hybrid heatpump [#500](https://github.com/emsesp/EMS-ESP32/issues/500)
- Add translated tags
- Add min/max to customization table [#686](https://github.com/emsesp/EMS-ESP32/issues/686)
- Add MD5 check [#637](https://github.com/emsesp/EMS-ESP32/issues/637)
- Add more bus-ids [#673](https://github.com/emsesp/EMS-ESP32/issues/673)
- Use HA connectivity device class for Status, added boot time [#751](https://github.com/emsesp/EMS-ESP32/issues/751)
- Add commands for analog sensors outputs
- Support for multiple EMS-ESPs with MQTT and HA [[#759](https://github.com/emsesp/EMS-ESP32/issues/759)]
- Settings for heatpump silent mode and additional heater [[#802](https://github.com/emsesp/EMS-ESP32/issues/802)] [[#803](https://github.com/emsesp/EMS-ESP32/issues/803)]
- Zone module MZ100 [#826](https://github.com/emsesp/EMS-ESP32/issues/826)
- Default MQTT hostname is blank [#829](https://github.com/emsesp/EMS-ESP32/issues/829)
- wwCurFlow for ems+ devices [#829](https://github.com/emsesp/EMS-ESP32/issues/829)
- Add Rego 3000, TR120RF thermostats [#917](https://github.com/emsesp/EMS-ESP32/issues/917)
- Add config for ESP32-S3
- Add heatpump silent mode and other entities [#896](https://github.com/emsesp/EMS-ESP32/issues/896)
- Allow reboot to other partition (factory or asymetric OTA)
- Blacklist entities to remove from memory [#891](https://github.com/emsesp/EMS-ESP32/issues/891)
- Add boiler pump operating mode [#944](https://github.com/emsesp/EMS-ESP32/issues/944)

## Fixed

- Factory Reset not working [#628](https://github.com/emsesp/EMS-ESP32/issues/628)
- Valid 4 byte values [#820](https://github.com/emsesp/EMS-ESP32/issues/820)
- Commands for multiple thermostats [#826](https://github.com/emsesp/EMS-ESP32/issues/826)
- API queries for multiple devices [#865](https://github.com/emsesp/EMS-ESP32/issues/865)
- Console crash when using call with command `hcx` only. [#841](https://github.com/emsesp/EMS-ESP32/issues/841)
- `heatingPump2Mod` was wrong, changed to absBurnPow [[#908](https://github.com/emsesp/EMS-ESP32/issues/908)
- Rounding of web input values
- Analog sensor with single gpio number [#915](https://github.com/emsesp/EMS-ESP32/issues/915)
- HA dallas and analog configs: remove/rebuild on change [#888](https://github.com/emsesp/EMS-ESP32/issues/888)
- Modes and set seltemp for RC30 and RC20 [#932](https://github.com/emsesp/EMS-ESP32/issues/932)

## Changed

- Discovery in HomeAssistant don't work with custom base topic. [#596](https://github.com/emsesp/EMS-ESP32/issues/596) Base topic containing `/` are changed to `_`
- RF room temperature sensor are shown as thermostat
- Render mqtt float json values with trailing zero
- Removed flash strings, to increase available heap memory
- Reload page after restart button is pressed
- Analog/dallas values command as list like ems-devices
- Analog/dallas HA-entities based on id
- MQTT Base is a mandatory field. Removed MQTT topic length from settings
- HA duration class for time entities [[#822](https://github.com/emsesp/EMS-ESP32/issues/822)
- AM200 alternative heatsource as class heatsource [[#857](https://github.com/emsesp/EMS-ESP32/issues/857)

# [3.4.2] September 18 2022

## Added

- RC310 additions [#520](https://github.com/emsesp/EMS-ESP32/pull/520)
  - damping
  - wwprio for RC310 heating circuits
  - switchonoptimization for RC310 heating circuits
  - enum_controlmode for RC310 (new enum list)
  - nofrostmode, reducemode, reducetemp & noreducetemp for RC310
  - emergencyops and emergencytemp, wwmaxtemp, wwflowtempoffset and wwcomfort1 for RC310
  - HM200 hybrid module [#500](https://github.com/emsesp/EMS-ESP32/issues/500)
  - AM200 alternative heatsource module [#573](https://github.com/emsesp/EMS-ESP32/issues/573)
  - EM10 error module as gateway [#575](https://github.com/emsesp/EMS-ESP32/issues/575)

## Fixed

- fix Table resizing in WebUI [#519](https://github.com/emsesp/EMS-ESP32/issues/519)
- allow larger customization files [#570](https://github.com/emsesp/EMS-ESP32/issues/570)
- losing entity wwcomfort [#581](https://github.com/emsesp/EMS-ESP32/issues/581)

## Changed

- Shorten "friendly names" in Home Assistant [#555](https://github.com/emsesp/EMS-ESP32/issues/555)
- platformio 2.3.0 (IDF 4, Arduino 2)
- remove master-thermostat, support multiple thermostats
- merge up- and download in webui [#577](https://github.com/emsesp/EMS-ESP32/issues/577)

# [3.4.1] May 29 2022

## Fixed

- Fix memory leak in api [#524](https://github.com/emsesp/EMS-ESP32/issues/524)

## Changed

- Controller data in web-ui only for IVT [#522](https://github.com/emsesp/EMS-ESP32/issues/522)
- Rename hidden `climate` to a more explaining name [#523](https://github.com/emsesp/EMS-ESP32/issues/523)
- Minor changes to the Customizations web page [#527](https://github.com/emsesp/EMS-ESP32/pull/527)

# [3.4.0] May 23 2022

## Added

- WebUI optimizations, updated look&feel and better performance [#124](https://github.com/emsesp/EMS-ESP32/issues/124)
- Auto refresh of WebUI after successful firmware upload [#178](https://github.com/emsesp/EMS-ESP32/issues/178)
- New Customization Service in WebUI. First feature is the ability to enable/disabled Entities (device values) from EMS devices [#206](https://github.com/emsesp/EMS-ESP32/issues/206)
- Option to disable Telnet Console [#209](https://github.com/emsesp/EMS-ESP32/issues/209)
- Added Hide SSID, Max Clients and Preferred Channel to Access Point
- Merged in MichaelDvP's changes like Fahrenheit conversion, publish single (for IOBroker) and a few other critical optimizations
- Enabled bi-directional read/write with Home Assistant, so values can be changed automatically from the UI without scripting [#265](https://github.com/emsesp/EMS-ESP32/issues/265)
- Added GC7000F Boiler [#270](https://github.com/emsesp/EMS-ESP32/issues/270)
- Revised LED flash sequence on boot up to show system health (1 flash=no ems, 2 flashes=no wifi) [#224](https://github.com/emsesp/EMS-ESP32/issues/224)
- Analog Sensor support [#271](https://github.com/emsesp/EMS-ESP32/issues/271)
- Solar cylinder priority [#247](https://github.com/emsesp/EMS-ESP32/issues/247)
- Read only mode in Settings, where EMS Tx/Write commands are blocked [#286](https://github.com/emsesp/EMS-ESP32/issues/286)
- Added 8700i Boiler device
- Added Cascade CM10 Controller device
- Add Olimex ESP32-POE-ISO to board profiles plus settings to customize Ethernet modules [#301](https://github.com/emsesp/EMS-ESP32/issues/301)
- Help text for string commands in WebUI [#320](https://github.com/emsesp/EMS-ESP32/issues/320)
- Germany translations (at compile time)
- #entities added to system/info` endpoint [#322](https://github.com/emsesp/EMS-ESP32/issues/322)
- analog outputs digital/pwm/dac
- remove MQTT retained configs if discovery is disabled
- timeout 10 min for MQTT-QoS wait
- Moduline 300 auto-temperatures T1-T4, RC300 romminfluencefactor
- RC35 parameters [#392](https://github.com/emsesp/EMS-ESP32/issues/392), [#398](https://github.com/emsesp/EMS-ESP32/issues/398)
- sync time with thermostat [#386](https://github.com/emsesp/EMS-ESP32/issues/386), [#408](https://github.com/emsesp/EMS-ESP32/issues/408)
- set mode has immediate effect [#395](https://github.com/emsesp/EMS-ESP32/issues/395)
- min/max in web value setting
- Extend customization to select if an entity is to be shown in the WebUI or forced as read-only [#317](https://github.com/emsesp/EMS-ESP32/issues/317)
- Added Moduline 400 installation parameters [PR #449 by @kwertie01](https://github.com/emsesp/EMS-ESP32/pull/449)
- Read time from IVT-controller [#439](https://github.com/emsesp/EMS-ESP32/issues/439)
- Hybrid Heatpump product-id 168 [#459](https://github.com/emsesp/EMS-ESP32/issues/459), thermostat settings
- Junkers ISM2 and IPM in warm water mode [#437](https://github.com/emsesp/EMS-ESP32/issues/437)
- Added Shower Alert trigger time and cold shot time [#436](https://github.com/emsesp/EMS-ESP32/issues/436)
- Improved Table layout in Web UI (searching, filtering, sorting, exporting to CSV)
- API fetch individual attributes from an entity [#462](https://github.com/emsesp/EMS-ESP32/issues/462)
- Option to disable mDNS
- Option for rendering booleans on dashboard [#456](https://github.com/emsesp/EMS-ESP32/issues/456)
- Upload customization settings from a file [#256](https://github.com/emsesp/EMS-ESP32/issues/256)

## Fixed

- lastcode broke MQTT JSON structure [#228](https://github.com/emsesp/EMS-ESP32/issues/228)
- fixed issue with overlapping while reading sequence of EMS1.0 telegrams
- fixed redundant telegram readings (because of offset overflow)
- added missing RC30/Moduline 400 [#243](https://github.com/emsesp/EMS-ESP32/issues/243)
- Correct modes for RC25 [#106](https://github.com/emsesp/EMS-ESP32/issues/106)
- Clean up old HA config's in MQTT before publishing data. This will prevent HA giving the 'dict' warnings [#229](https://github.com/emsesp/EMS-ESP32/issues/229)
- RC25 temperature setting [#272](https://github.com/emsesp/EMS-ESP32/issues/272)
- Buderus RC25 - "hc1 mode type" incorrect value [#273](https://github.com/emsesp/EMS-ESP32/issues/273)
- Increased number of Mixers and Heating Circuits [#294](https://github.com/emsesp/EMS-ESP32/issues/294)
- Check receive status before removing a telegram fetch [#268](https://github.com/emsesp/EMS-ESP32/issues/268), [#282](https://github.com/emsesp/EMS-ESP32/issues/282)
- Fix uploading firmware on OSX [#345](https://github.com/emsesp/EMS-ESP32/issues/345)
- Non-nested MQTT would corrupt the json [#354](https://github.com/emsesp/EMS-ESP32/issues/354)
- Burner selected max power can have a value higher than 100% [#314](https://github.com/emsesp/EMS-ESP32/issues/314)
- some missing fahrenheit calculations
- limited number of exclusions [#339](https://github.com/emsesp/EMS-ESP32/issues/339)
- MQTT sometimes would not reconnect after a WiFi outage

## Changed

- Use flash system to show system health (1 flash=no ems, 2 flashes=no wifi) [#224](https://github.com/emsesp/EMS-ESP32/issues/224)
- Renamed Dallas Sensor to Temperature Sensor in UI
- Dallas Format removed. Use the name to give each sensor an alias
- No longer MQTT subscribes to topic `/thermostat_hc<n>` as it supports a path similar to the API endpoint construct
- Show Sensors quality in WebUI
- Controller not shown in WebUI dashboard
- renamed "Home Assistant Integration" to "MQTT Discovery" in MQTT Settings [#290](https://github.com/emsesp/EMS-ESP32/issues/290)
- Show ems tx reads and writes separately
- Show ems device handlers separated for received, fetched and pending handlers.
- Wired renamed to Ethernet
- removed system/pin command, new commands in analogsensors
- system/info device-info split to name/version/brand
- exclude list uses short-names, possible flags for web/api/mqtt excludes, readonly and favorite (selection not yet implemented)
- thermostat clock formate date-time: dd.mm.yyyy hh:mm
- RC300 summermode as other thermostats `winter/summer` instead of `off/on`

## **BREAKING CHANGES:**

- Settings:
  - order of Boolean Format has changed in Application Settings - check your settings
  - Dallas Format setting removed. Now customize name of each Dallas sensor via the UI
- MQTT/API
  - Boiler `wwheat` renamed to `ww3wayon` [#211](https://github.com/emsesp/EMS-ESP32/issues/211)
  - Boiler `ww` tag renamed to `dhw`. Any custom Home Assistant lovelace dashboards will need updating.
  - Renamed description of `wwtapactivated` to "turn on/off DHW". Otherwise would have looked like "boiler_dhw_turn_on_off_dhw" in HA.
  - `/api/system/info` endpoint has updated keys. Now lowercase, no underscores and not capitalized. Replace "handlers" with "handlers received", "handlers fetched" and "handlers pending".

# [3.3.1] January 20 2022

- lastcode broke MQTT JSON structure [#228](https://github.com/emsesp/EMS-ESP32/issues/228)
- overlapping while reading sequence of EMS1.0 telegrams
- redundant telegram readings (because of offset overflow)
- added missing RC30/Moduline400 [#243](https://github.com/emsesp/EMS-ESP32/issues/243)
- check received status before toggling fetch on empty telegram [#268][#282]

# [3.3.0] November 28 2021

## Added

- Add system commands for syslog level and watch [#98](https://github.com/emsesp/EMS-ESP32/issues/98)
- Added pool data to telegrams 0x494 & 0x495 [#102](https://github.com/emsesp/EMS-ESP32/issues/102)
- Add RC300 second summermode telegram [#108](https://github.com/emsesp/EMS-ESP32/issues/108)
- Add support for the RC25 thermostat [#106](https://github.com/emsesp/EMS-ESP32/issues/106)
- Add new command 'entities' for a device, e.g. <http://ems-esp/api/boiler/entities> to show the shortname, description and HA Entity name (if HA enabled) [#116](https://github.com/emsesp/EMS-ESP32/issues/116)
- Support for Junkers program and remote (fb10/fb110) temperature
- Home Assistant `state_class` attribute for Wh, kWh, W and KW [#129](https://github.com/emsesp/EMS-ESP32/issues/129)
- Add current room influence for RC300 [#136](https://github.com/emsesp/EMS-ESP32/issues/136)
- Added Home Assistant device_class to sensor entities
- Added another Buderus RC10 thermostat with Product ID 65 [#160](https://github.com/emsesp/EMS-ESP32/issues/160)
- Added support for mDNS [#161](https://github.com/emsesp/EMS-ESP32/issues/161)
- Added last system ESP32 reset code to log (and `system info` output)
- Firmware Checker in WebUI [#168](https://github.com/emsesp/EMS-ESP32/issues/168)
- Added new MQTT setting for enabling 'response' topic
- Support for non-standard Thermostats like Tado [#174](https://github.com/emsesp/EMS-ESP32/issues/174)
- Include MQTT connection status in 'api/system/info'
- Include Network status in 'api/system/info' and also the MQTT topic `info` [#202](https://github.com/emsesp/EMS-ESP32/issues/202)
- Added Ethernet PHY module as an option in the Board Profile [#210](https://github.com/emsesp/EMS-ESP32/issues/210)

## Fixed

- MQTT reconnecting after WiFi reconnect [#99](https://github.com/emsesp/EMS-ESP32/issues/99)
- Manually Controlling Solar Circuit [#107](https://github.com/emsesp/EMS-ESP32/issues/107)
- Fix thermostat commands not defaulting to the master thermostat [#110](https://github.com/emsesp/EMS-ESP32/issues/110)
- Enlarge parse-buffer for long names like `cylinderpumpmodulation`
- MQTT not subscribing to all device entities [#166](https://github.com/emsesp/EMS-ESP32/issues/166)
- Help fix issues with WebUI unable to fully load UI over Ethernet [#177](https://github.com/emsesp/EMS-ESP32/issues/177)
- Shower alert never reset after limit reached when enabled [(PR #185)]
- Remove HA entity entries when a device value goes dormant [#196](https://github.com/emsesp/EMS-ESP32/issues/196)
- deciphering last error code dates on 0xC2 telegram [#204](https://github.com/emsesp/EMS-ESP32/issues/204)

## Changed

- Syslog BOM only for utf-8 messages [#91](https://github.com/emsesp/EMS-ESP32/issues/91)
- Check for KM200 by device-id 0x48, remove tx-delay [#90](https://github.com/emsesp/EMS-ESP32/issues/90)
- rename `fastheatupfactor` to `fastheatup` and add percent [#122](https://github.com/emsesp/EMS-ESP32/issues/122)
- "unit" renamed to "uom" in API call to recall a Device Value
- initial backend React changes to replace the class components (HOCs) with React Hooks
- Use program-names instead of numbers
- Boiler's maintenancemessage always published in MQTT (to prevent HA missing entity)
- Unit of Measure 'times' added to MQTT Fails, Rx fails, Rx received, Tx fails, Tx reads & Tx writes
- Improved API. Restful HTTP API works in the same way as MQTT calls
- Removed settings for MQTT subscribe format [#173](https://github.com/emsesp/EMS-ESP32/issues/173)
- Improve Nefit Moduline 200 functionality [#183](https://github.com/emsesp/EMS-ESP32/issues/183)
- `status` in the MQTT heartbeat renamed to `bus_status`
- Layout changes in the WebUI, showing stripped table rows in Dashboard
- Alternative font for log window [#219](https://github.com/emsesp/EMS-ESP32/issues/219)

## **BREAKING CHANGES**

- API: "unit" renamed to "uom" in API call to recall a Device Value
- HA: `sensor.boiler_boiler_temperature` renamed to `sensor.actual_boiler_temperature`
- HA: `binary_sensor.boiler_ww_disinfecting` renamed to `binary_sensor.boiler_ww_disinfection`
- HA: # removed from counts in MQTT Fails, Rx fails, Rx received, Tx fails, Tx reads & Tx writes
- `txread` renamed to `txreads` and `txwrite` renamed to `txwrites` in MQTT heartbeat payload
- 'dallas sensors' in api/system/info moved to the "System" section. Renamed "uptime (seconds)" and "reset reason"
- `status` in the MQTT heartbeat renamed to `bus_status`

# [3.2.1] August 8 2021

## Added

- json body in API can now take device, name, cmd, hc and id
- added example of how to use API directly to control values from Home Assistant
- API calls are shown in debug log (For troubleshooting)

## Fixed

- fixed issue with Home Assistant entity naming where boiler's ww was duplicated in entity name
- fixed issue where wwSetTemp was written too instead of wwSelTemp

## Changed

- fixed case on mqtt names, like 'wwtankmiddletemp'
- renamed Product ID to 'EMS Product ID' in Home Assistant
- removed brackets around tags, e.g. (hc1) selected room temperature" is now just "hc1 selected room temperature"

# [3.2.0] August 6 2021

## Added

- support for IPv6 (web/api/mqtt, not syslog yet) [#83](https://github.com/emsesp/EMS-ESP32/issues/83)
- System Log in Web UI will show current time if the NTP Service is enabled [#82](https://github.com/emsesp/EMS-ESP32/issues/82)
- Network settings for Tx-power, WiFi-bandwidth, WiFi-sleepmode [#83](https://github.com/emsesp/EMS-ESP32/issues/83)
- optional low CPU clockrate (160 MHz) [#83](https://github.com/emsesp/EMS-ESP32/issues/83)
- select format for enumerated values in web
- settings for water hysteresis on/off
- dallas sensor name editable. `sensorname` console-command, replace sensorid with a unique name [#84](https://github.com/emsesp/EMS-ESP32/issues/84)
- 'restart' system command. Can be invoked via API with authentication. [#87](https://github.com/emsesp/EMS-ESP32/issues/87)
- add Download button in Web UI for log

## Fixed

- set mode allow numbers
- Junkers thermostat shows mode as selected by set_mode
- HA thermostat mode if bool-format: numbers is selected
- Web UI System Log sometimes skipped a few log messages when watching real-time
- fix wwactivated [#89](https://github.com/emsesp/EMS-ESP32/issues/89)
- don't show commands (like reset) as Device values in the Web or Console

## Changed

- removed Rx echo failures counting as incomplete telegrams. Bad telegrams show as Warning and not Errors. [#80](https://github.com/emsesp/EMS-ESP32/issues/80)
- add upload_sec to `api/system/info` and removed # from some names to keep consistent with MQTT heartbeat
- added debug target to PlatformIO build to help hunt down system crashes
- enumerated values always start at zero
- maintenance settings for time/date as extra setting
- move api/mqtt formats to `settings`, add `enum format`
- UI improvements for editing Dallas Sensor details
- RESTful GET commands can also require authentication (via bearer access token) for better security
- Updated AsyncMqttClient to 0.9.0 and ArduinoJson to 6.18.3
- Download buttons for settings and info under the Help tab

# [3.1.1] June 26 2021

## Added

- new command called `commands` which lists all available commands. `ems-esp/api/{device}/commands`
- More Home Assistant icons to match the UOMs
- new API. Using secure access tokens and OpenAPI standard. See `doc/EMS-ESP32 API.md` and [#50](https://github.com/emsesp/EMS-ESP32/issues/50)
- show log messages in Web UI [#71](https://github.com/emsesp/EMS-ESP32/issues/71)

## Fixed

- HA thermostat mode was not in sync with actual mode [#66](https://github.com/emsesp/EMS-ESP32/issues/66)
- Don't publish rssi if Wifi is disabled and ethernet is being used
- Booleans are shown as true/false in API GETs

## Changed

- `info` command always shows full names in API. For short names query the device or name directly, e.g. `http://ems-esp/api/boiler`
- free memory is shown in kilobytes
- boiler's warm water entities have ww added to the Home Assistant entity name [#67](https://github.com/emsesp/EMS-ESP32/issues/67)
- improved layout and rendering of device values in the WebUI, also the edit value screen

# [3.1.0] May 4 2021

## Added

- Mock API to simulate an ESP, for testing web
- Able to write values from the Web UI
- check values with `"cmd":<valuename>` and data empty or `?`
- set hc for values and commands by id or prefix `hc<x>`+separator, separator can be any char

## Fixed

- Don't create Home Assistant MQTT discovery entries for device values that don't exists (#756 on EMS-ESP repo)
- Update shower MQTT when a shower start is detected
- S32 board profile

## Changed

- Icon for Network
- MQTT Formatting payload (nested vs single) is a pull-down option
- moved mqtt-topics and texts to local_EN, all topics lower case
- Re-enabled Shower Alert (still experimental)
- lowercased Flow temp in commands
- system console commands to main

# [3.0.1] March 30 2021

## Added

- individual mqtt commands (#31)
- board Profiles (#11)

## Fixed

- Rx and Tx quality % would sometimes show > 100

## Changed

- invert LED changed to Hide LED. Default is off.
- renamed Scan Network to Scan WiFi Network
- added version to cmd=settings
- Allow both WiFi and Ethernet together, fall back to AP when Ethernet disconnects

## Removed

- Shower Alert (disabled for now)

# [3.0.0] March 18 2021

## Added

- Power settings, disabling BLE and turning off Wifi sleep
- Rx and Tx counts to Heartbeat MQTT payload
- Ethernet support
- id to info command to show only a heatingcircuit
- add sending devices that are not listed to 0x07
- extra MQTT boolean option for "ON" and "OFF"
- Support for chunked MQTT payloads to allow large data sets > 2kb
- External Button support (#708) for resetting to factory defaults and other actions
- new console set command in `system`, `set ethernet <profile>` for quickly enabling cabled ethernet connections without using the captive wifi portal
- Added in MQTT nested mode, for thermostat and mixer, like we had back in v2
- Cascade MC400 (product-id 210) (3.0.0b6)
- values for wwMaxPower, wwFlowtempOffset
- RC300 `thermostat temp -1` to clear temporary setpoint in auto mode
- Syslog port selectable (#744)

## Fixed

- telegrams matched to masterthermostat 0x18
- multiple roomcontrollers
- readback after write with delay (give ems-devices time to set the value)
- Thermostat ES72/RC20 device 66 to command-set RC20_2
- MQTT payloads not adding to queue when MQTT is re-connecting (fixes #369)
- fix for HA topics with invalid command formats (#728)
- wrong position of values #723, #732
- OTA Upload via Web on OSX

## Changed

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
- Network Gateway IP is optional (#682)emsesp/EMS-ESP
- moved to a new GitHub repo <https://github.com/emsesp/EMS-ESP32>
