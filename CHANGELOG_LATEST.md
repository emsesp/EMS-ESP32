# Changelog

# [3.5.0]

## **IMPORTANT! BREAKING CHANGES**

- When upgrading to v3.5 for the first time from v3.4 on a BBQKees Gateway board you will need to use the [EMS-EPS Flasher](https://github.com/emsesp/EMS-ESP-Flasher/releases) to correctly re-partition the flash. Make sure you backup the settings and customizations from the WebUI (System->Upload/Download) and restore after the upgrade.
- Since 3.5.0b11 we added support for multiple EMS-ESPs [#759] so they can co-exist with MQTT Discovery. The mqtt base is now included in each of the entity names to make them unique. Unfortunately this means in Home Assistant the entity IDs have also been renamed (for example from `number.boiler_burnminperiod` to `number.ems_esp_boiler_burnminperiod`) and any dashboards and automation scripts need to be adjusted accordingly.

## Added

- Translations in Web UI and all device entity names (DE, NL, SE, PL, NO) [#22](https://github.com/emsesp/EMS-ESP32/issues/22)
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

## Fixed

- Factory Reset not working [#628](https://github.com/emsesp/EMS-ESP32/issues/628)

## Changed

- Discovery in HomeAssistant don't work with custom base topic. [#596](https://github.com/emsesp/EMS-ESP32/issues/596) Base topic containing `/` are changed to `_`
- RF room temperature sensor are shown as thermostat
- render mqtt float json values with trailing zero
- removed flash strings, to increase available heap memory
- reload page after restart button is pressed
- analog/dallas values command as list like ems-devices
- analog/dallas HA-entities based on id
- MQTT Base is a mandatory field. Removed MQTT topic length from settings.
