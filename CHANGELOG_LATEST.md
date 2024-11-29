# Changelog

For more details go to [docs.emsesp.org](https://docs.emsesp.org/).

## [3.7.1]

## **IMPORTANT! BREAKING CHANGES since v3.7.0**

## Added

- include HA "unit_of_meas", "stat_cla" and "dev_cla" attributes for Number sensors [#2149](https://github.com/emsesp/EMS-ESP32/issues/2149)
- Bosch CS6800i AW - Silent Mode + Electrical Power Reduction (HP) [#2147](https://github.com/emsesp/EMS-ESP32/issues/2147)
- `/api/system/showeralert` and `/api/system/showertimer` [#2182](https://github.com/emsesp/EMS-ESP32/issues/2182)
- MX400 [#2198](https://github.com/emsesp/EMS-ESP32/issues/2198)
- SM200 values [#2212](https://github.com/emsesp/EMS-ESP32/discussions/2212)

## Fixed

- Modbus integration in 3.7.0 missing offset [#2148](https://github.com/emsesp/EMS-ESP32/issues/2148)
- fix changing TZ in NTPsettings without clearing enable+server, added DST support [#2142](https://github.com/emsesp/EMS-ESP32/issues/2142)
- Support MQTT Discovery (AD) with Domoticz [#2177](https://github.com/emsesp/EMS-ESP32/issues/2177)
- wwExtra (dhw extra) changed from temperature reading to number
- auxheaterstatus [#2192](https://github.com/emsesp/EMS-ESP32/issues/2192)
- lastCode character check [#2189](https://github.com/emsesp/EMS-ESP32/issues/2189)
- reading too many telegram parts
- heatpump cost UOMs [#2188](https://github.com/emsesp/EMS-ESP32/issues/2188)
- analog dac output and inputs on dac pins [#2201](https://github.com/emsesp/EMS-ESP32/discussions/2201)
- api memory leak [#2216](https://github.com/emsesp/EMS-ESP32/issues/2216)
- modbus multiple mixers [#2229](https://github.com/emsesp/EMS-ESP32/issues/2229)
- Last Will (LWT) not set on MQTT Connect [#2247](https://github.com/emsesp/EMS-ESP32/issues/2247)

## Changed

- name of wwstarts2 [#2217](https://github.com/emsesp/EMS-ESP32/discussions/2217)
