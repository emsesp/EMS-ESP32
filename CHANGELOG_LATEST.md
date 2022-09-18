# Changelog

# [3.5.0]

## Added

- Translations in Web UI and all device entity names to German. [#22](https://github.com/emsesp/EMS-ESP32/issues/22)

## Fixed

## Changed

- Discovery in HomeAssistant don't work with custom base topic. [#596](https://github.com/emsesp/EMS-ESP32/issues/596)

## **BREAKING CHANGES:**

- MQTT Discovery (Home Assistant) entity names are now prefixed with the hostname, e.g. `select.thermostat_hc1_mode` becomes `select.emsesp_thermostat_hc1_mode`. You will need to recreate any custom dashboards.
