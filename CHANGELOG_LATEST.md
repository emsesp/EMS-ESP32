# Changelog

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
- new secure API ([#50](https://github.com/emsesp/EMS-ESP32/issues/50))

## Removed
