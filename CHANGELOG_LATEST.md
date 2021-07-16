# Changelog

## Added

- support for IPv6 (web/api/mqtt, not syslog) (#83)
- System Log in Web UI will show current time if the NTP Service is enabled (#82)
- Network settings for Tx-power, WiFi-bandwidth, WiFi-sleepmode (#83)
- optional low clockrate (160 MHz) (#83)
- selectbox for enumerated values in web
- settings for water hysteresis on/off

## Fixed

- set mode allow numbers
- Junkers thermostat shows mode as selected by set_mode
- HA thermostat mode if bool-format: numbers is selected

## Changed

- removed Rx echo failures counting as incomplete telegrams. Bad telegrams show as Warning and not Errors. [#80](https://github.com/emsesp/EMS-ESP32/issues/80)
- add upload_sec to `api/system/info` and removed # from some names to keep consistent with MQTT heartbeat
- added debug target to PlatformIO build to help hunt down system crashes
- enumerated values always start at zero
- maintenance settings for time/date as extra setting
- move api/mqtt formats to `settings`, add `enum format`

## Removed
