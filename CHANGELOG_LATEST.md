# Changelog

# [3.6.0]

## **IMPORTANT! BREAKING CHANGES**

## Added

- Workaround for better Domoticz MQTT intergration? [#904](https://github.com/emsesp/EMS-ESP32/issues/904)
- Warn user in WebUI of unsaved changes [#911](https://github.com/emsesp/EMS-ESP32/issues/911)
- Detect old Tado thermostat, device-id 0x19, no entities
- Some more HM200 entities [#500](https://github.com/emsesp/EMS-ESP32/issues/500)
- Custom Scheduler [#701](https://github.com/emsesp/EMS-ESP32/issues/701)

## Fixed

- HA-discovery for analog sensor commands [#1035](https://github.com/emsesp/EMS-ESP32/issues/1035)

## Changed

- Optional upgrade to platform-espressif32 6.0.0 (after 5.3.0) [#862](https://github.com/emsesp/EMS-ESP32/issues/862)
- Use byte 0 for detection RC30 active heatingcircuit [#786](https://github.com/emsesp/EMS-ESP32/issues/786)
- Write repeated selflowtemp if tx-queue is empty without verify [#954](https://github.com/emsesp/EMS-ESP32/issues/954)
- HA discovery recreate after disconnect by device [#1067](https://github.com/emsesp/EMS-ESP32/issues/1067)
- File upload: check flash size (overflow) instead of filesize
- Improved HA Discovery so previous configs no longer need to be removed when starting [#1077](https://github.com/emsesp/EMS-ESP32/pull/1077) (thanks @pswid!)
