# Changelog

## Added

- support for IPv6 (#83)
- System Log in Web UI will show current time if the NTP Service is enabled (#82)

## Fixed

## Changed

- removed Rx echo failures counting as incomplete telegrams. BAd telegrams show as Warning and not Errors. [#80](https://github.com/emsesp/EMS-ESP32/issues/80)
- add upload_sec to `api/system/info` and removed # from some names to keep consistent with MQTT heartbeat
- added debug target to PlatformIO build to help hunt down system crashes

## Removed
