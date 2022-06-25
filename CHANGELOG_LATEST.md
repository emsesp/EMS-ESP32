# Changelog

# [3.4.2]

## Added

- RC310 additions [#520](https://github.com/emsesp/EMS-ESP32/pull/520)
  - damping
  - wwprio for RC310 heating circuits
  - switchonoptimization for RC310 heating circuits
  - enum_controlmode for RC310 (new enum list)
  - nofrostmode, reducemode, reducetemp & noreducetemp for RC310
  - emergencyops and emergencytemp, wwmaxtemp, wwflowtempoffset and wwcomfort1 for RC310

## Fixed

- fix Table resizing in WebUI [#519](https://github.com/emsesp/EMS-ESP32/issues/519)

## Changed

- Shorten "friendly names" in Home Assistant [#555](https://github.com/emsesp/EMS-ESP32/issues/555)

- platformio 2.3.0 (IDF 4, Arduino 2)
- remove master-thermostat, support multiple thermostats

## **BREAKING CHANGES:**
