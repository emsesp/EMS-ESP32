# Changelog

# [3.5.0]

## Added

- Translations in Web UI and all device entity names to German. [#22](https://github.com/emsesp/EMS-ESP32/issues/22)
- Add support for Lolin C3 mini [#620](https://github.com/emsesp/EMS-ESP32/pull/620)
- Add Greenstar 30Ri boiler
- Add program memory info
- Add min/max setting to customizations
- Adapt min/max if ems-value is not in this range
- Add heatpump settings for inputs and limits

## Fixed

- Factory Reset not working [#628](https://github.com/emsesp/EMS-ESP32/issues/628)

## Changed

- Discovery in HomeAssistant don't work with custom base topic. [#596](https://github.com/emsesp/EMS-ESP32/issues/596) Base topic containing `/` are changed to `_`
- RF room temperature sensor are shown as thermostat

## **BREAKING CHANGES:**

- When upgrading from 3.4.x you may need to erase the flash on the ESP32 before uploading the firmware. Make sure you make a backup of the settings and customizations via the WebUI (System->Upload/Download)
