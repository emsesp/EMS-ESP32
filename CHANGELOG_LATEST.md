# Changelog

## Added

- new command called `commands` which lists all available commands. `ems-esp/api/{device}/commands`
- More Home Assistant icons to match the UOMs
- new API. Using secure access tokens and OpenAPI standard. See `doc/EMS-ESP32 API.md` and [#50](https://github.com/emsesp/EMS-ESP32/issues/50)

## Fixed

- HA thermostat mode was not in sync with actual mode [#66](https://github.com/emsesp/EMS-ESP32/issues/66)
- Don't publish rssi if Wifi is disabled and ethernet is being used

## Changed

- `info` command always shows full names in API. For short names query the device or name directly, e.g. `http://ems-esp/api/boiler`
- free memory is shown in kilobytes
- boiler's warm water entities have ww added to the Home Assistant entity name [#67](https://github.com/emsesp/EMS-ESP32/issues/67)
- improved layout and rendering of device values in the WebUI, also the edit value screen

## Removed
