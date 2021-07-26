# Changelog

## Added

- support for IPv6 (web/api/mqtt, not syslog yet) [#83](https://github.com/emsesp/EMS-ESP32/issues/83)
- System Log in Web UI will show current time if the NTP Service is enabled [#82](https://github.com/emsesp/EMS-ESP32/issues/82)
- Network settings for Tx-power, WiFi-bandwidth, WiFi-sleepmode [#83](https://github.com/emsesp/EMS-ESP32/issues/83)
- optional low CPU clockrate (160 MHz) [#83](https://github.com/emsesp/EMS-ESP32/issues/83)
- select format for enumerated values in web
- settings for water hysteresis on/off
- dallas sensor name editable. `sensorname` console-command, replace sensorid with a unique name [#84](https://github.com/emsesp/EMS-ESP32/issues/84)
- 'restart' system command. Can be invoked via API with authentication. [#87](https://github.com/emsesp/EMS-ESP32/issues/87)

## Fixed

- set mode allow numbers
- Junkers thermostat shows mode as selected by set_mode
- HA thermostat mode if bool-format: numbers is selected
- Web UI System Log sometimes skipped a few log messages when watching real-time
- fix wwactivated [#89](https://github.com/emsesp/EMS-ESP32/issues/89)

## Changed

- removed Rx echo failures counting as incomplete telegrams. Bad telegrams show as Warning and not Errors. [#80](https://github.com/emsesp/EMS-ESP32/issues/80)
- add upload_sec to `api/system/info` and removed # from some names to keep consistent with MQTT heartbeat
- added debug target to PlatformIO build to help hunt down system crashes
- enumerated values always start at zero
- maintenance settings for time/date as extra setting
- move api/mqtt formats to `settings`, add `enum format`
- UI improvements for editing Dallas Sensor details
- Rest GET commands can also require authentication (via bearer access token) for better security
