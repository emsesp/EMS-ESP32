# Changelog

# [3.3.0]

## Added

- Add system commands for syslog level and watch [#98](https://github.com/emsesp/EMS-ESP32/issues/98)
- Added pool data to telegrams 0x494 & 0x495 [#102](https://github.com/emsesp/EMS-ESP32/issues/102)
- Add RC300 second summermode telegram [#108](https://github.com/emsesp/EMS-ESP32/issues/108)
- Add support for the RC25 thermostat [#106](https://github.com/emsesp/EMS-ESP32/issues/106)
- Add new command 'entities' for a device, e.g. http://ems-esp/api/boiler/entities to show the shortname, description and HA Entity name (if HA enabled) [#116](https://github.com/emsesp/EMS-ESP32/issues/116)
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

## **BREAKING CHANGES**

- API: "unit" renamed to "uom" in API call to recall a Device Value
- HA: `sensor.boiler_boiler_temperature` renamed to `sensor.actual_boiler_temperature`
- HA: `binary_sensor.boiler_ww_disinfecting` renamed to `binary_sensor.boiler_ww_disinfection`
- HA: # removed from counts in MQTT Fails, Rx fails, Rx received, Tx fails, Tx reads & Tx writes
- `txread` renamed to `txreads` and `txwrite` renamed to `txwrites` in MQTT heartbeat payload
- 'dallas sensors' in api/system/info moved to the "System" section. Renamed "uptime (seconds)" and "reset reason"
- `status` in the MQTT heartbeat renamed to `bus_status`
