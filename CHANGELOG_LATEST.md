# Changelog

# [3.6.0]

## **IMPORTANT! BREAKING CHANGES**

There are breaking changes in 3.6.0. Please read carefully before applying the update.

- The sensors have been renamed. `dallassensor` is now `temperaturesensor` in MQTT and `ts` in the Customizations file. Also `analogs` is now `analogsensor` in MQTT and `as` in the Customizations file. If you have customizations, make backup first using the Download option and rename the JSON arrays to `as` and `ts` respectively. Also removed any MQTT topics that start with `dallassensor` using something like MQTTExplorer.
- The format of the Custom Entities has changed, so you will need to manually re-create them.

## Added

- Workaround for better Domoticz MQTT intergration? [#904](https://github.com/emsesp/EMS-ESP32/issues/904)
- Warn user in WebUI of unsaved changes [#911](https://github.com/emsesp/EMS-ESP32/issues/911)
- Detect old Tado thermostat, device-id 0x19, no entities
- Some more HM200 entities [#500](https://github.com/emsesp/EMS-ESP32/issues/500)
- Custom Scheduler [#701](https://github.com/emsesp/EMS-ESP32/issues/701)
- Custom Entities read from EMS bus
- Build S3 binary with github actions
- Greenstar HIU [#1158](https://github.com/emsesp/EMS-ESP32/issues/1158)
- AM200 code 10 [#1161](https://github.com/emsesp/EMS-ESP32/issues/1161)
- Ventilation device [#1172](https://github.com/emsesp/EMS-ESP32/issues/1172)
- Turn ETH off on wifi connect [#1167](https://github.com/emsesp/EMS-ESP32/issues/1167)
- Support for multiple EMS-ESPs with HA [#1196](https://github.com/emsesp/EMS-ESP32/issues/1196)

## Fixed

- HA-discovery for analog sensor commands [#1035](https://github.com/emsesp/EMS-ESP32/issues/1035)
- Enum order of RC3x nofrost mode
- Heartbeat interval

## Changed

- Optional upgrade to platform-espressif32 6.3.0 (after 5.3.0) [#862](https://github.com/emsesp/EMS-ESP32/issues/862)
- Use byte 3 for detection RC30 active heatingcircuit [#786](https://github.com/emsesp/EMS-ESP32/issues/786)
- Write repeated selflowtemp if tx-queue is empty without verify [#954](https://github.com/emsesp/EMS-ESP32/issues/954)
- HA discovery recreate after disconnect by device [#1067](https://github.com/emsesp/EMS-ESP32/issues/1067)
- File upload: check flash size (overflow) instead of filesize
- Improved HA Discovery so previous configs no longer need to be removed when starting [#1077](https://github.com/emsesp/EMS-ESP32/pull/1077) (thanks @pswid!)
- Enlarge UART-Stack to 2,5k
- Retry timeout for Mqtt-QOS1/2 10seconds
- Optimize WebUI rendering when using Dialog Boxes [#1116](https://github.com/emsesp/EMS-ESP32/issues/1116)
- Optimize Web libraries to reduce bundle size (3.6.x) [#1112](https://github.com/emsesp/EMS-ESP32/issues/1112)
