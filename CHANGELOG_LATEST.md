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
  - HM200 hybrid module [#500](https://github.com/emsesp/EMS-ESP32/issues/500)
  - AM200 alternative heatsource module [#573](https://github.com/emsesp/EMS-ESP32/issues/573)
  - EM10 error module as gateway [#575](https://github.com/emsesp/EMS-ESP32/issues/575)

## Fixed

- fix Table resizing in WebUI [#519](https://github.com/emsesp/EMS-ESP32/issues/519)
- allow larger customization files [#570](https://github.com/emsesp/EMS-ESP32/issues/570)
- losing entitiy wwcomfort [#581](https://github.com/emsesp/EMS-ESP32/issues/581)

## Changed

- Shorten "friendly names" in Home Assistant [#555](https://github.com/emsesp/EMS-ESP32/issues/555)

- platformio 2.3.0 (IDF 4, Arduino 2)
- remove master-thermostat, support multiple thermostats
- merge up- and download in webui [#577](https://github.com/emsesp/EMS-ESP32/issues/577)

## **BREAKING CHANGES:**
