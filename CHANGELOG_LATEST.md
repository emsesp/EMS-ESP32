# Changelog

# [3.5.0]

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
