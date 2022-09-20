# Changelog

# [3.5.0]

## Added

- Translations in Web UI and all device entity names to German. [#22](https://github.com/emsesp/EMS-ESP32/issues/22)
- Add support for Lolin C3 mini [#620](https://github.com/emsesp/EMS-ESP32/pull/620)

## Fixed

- Factory Reset not working [#628](https://github.com/emsesp/EMS-ESP32/issues/628)

## Changed

- Discovery in HomeAssistant don't work with custom base topic. [#596](https://github.com/emsesp/EMS-ESP32/issues/596)

## **BREAKING CHANGES:**

- MQTT Discovery (Home Assistant) entity names are now prefixed with the hostname, e.g. `select.thermostat_hc1_mode` becomes `select.emsesp_thermostat_hc1_mode`. You will need to recreate any custom dashboards.
- When upgrading from 3.4.x you may need to erase the flash on the ESP32 before uploading the firmware. Make sure you make a backup of the settings and customizations via the WebUI (System->Upload/Download)
