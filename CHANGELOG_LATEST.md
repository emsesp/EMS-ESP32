# Changelog

# [3.5.0]

## Added

- Translations in Web UI and all device entity names (DE, NL, SE, PL, NO, ...) [#22](https://github.com/emsesp/EMS-ESP32/issues/22)
- Add support for Lolin C3 mini [#620](https://github.com/emsesp/EMS-ESP32/pull/620)
- Add support for ESP32-S2 [#667](https://github.com/emsesp/EMS-ESP32/pull/667)
- Add devices: Greenstar 30Ri boiler, Junkers FW500 thermostat, Buderus BC30 controller
- Add program memory info
- Add mqtt queue and connection infos
- Add min/max setting to customizations
- Adapt min/max if ems-value is not in this range
- Add heat pump settings for inputs and limits [#600](https://github.com/emsesp/EMS-ESP32/issues/600)
- Add hybrid heatpump [#500](https://github.com/emsesp/EMS-ESP32/issues/500)
- Add translated tags

## Fixed

- Factory Reset not working [#628](https://github.com/emsesp/EMS-ESP32/issues/628)

## Changed

- Discovery in HomeAssistant don't work with custom base topic. [#596](https://github.com/emsesp/EMS-ESP32/issues/596) Base topic containing `/` are changed to `_`
- RF room temperature sensor are shown as thermostat
- render mqtt float json values with trailing zero
- removed flash strings

## **BREAKING CHANGES:**

- When upgrading from 3.4.x you may need to erase the flash on the ESP32 before uploading the firmware. Make sure you make a backup of the settings and customizations via the WebUI (System->Upload/Download)
