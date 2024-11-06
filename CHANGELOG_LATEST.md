# Changelog

For more details go to [www.emsesp.org](https://www.emsesp.org/).

## [3.7.1]

## **IMPORTANT! BREAKING CHANGES since v3.7.0**

## Added

- include HA "unit_of_meas", "stat_cla" and "dev_cla" attributes for Number sensors [#2149](https://github.com/emsesp/EMS-ESP32/issues/2149)
- Bosch CS6800i AW - Silent Mode + Electrical Power Reduction (HP) [#2147](https://github.com/emsesp/EMS-ESP32/issues/2147)
- /api/system/showeralert and /api/system/showertimer [#2182](https://github.com/emsesp/EMS-ESP32/issues/2182)

## Fixed

- Modbus integration in 3.7.0 missing offset [#2148](https://github.com/emsesp/EMS-ESP32/issues/2148)
- fix changing TZ in NTPsettings without clearing enable+server, added DST support [#2142](https://github.com/emsesp/EMS-ESP32/issues/2142)
- Support MQTT Discovery (AD) with Domoticz [#2177](https://github.com/emsesp/EMS-ESP32/issues/2177)
- wwExtra (dhw extra) changed from temperature reading to number

## Changed
