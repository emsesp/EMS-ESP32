# Changelog

# [3.6.0]

## **IMPORTANT! BREAKING CHANGES**

There are breaking changes in 3.6.0. Please read carefully before applying the update.

- The sensors have been renamed. `dallassensor` is now `temperaturesensor` in MQTT and `ts` in the Customizations file. Also `analogs` is now `analogsensor` in MQTT and `as` in the Customizations file. If you have customizations, make backup first using the Download option and rename the JSON arrays to `as` and `ts` respectively. Also removed any MQTT topics that start with `dallassensor` using something like MQTTExplorer.
- The format of the Custom Entities has changed, so you will need to manually re-create them.

## Added

- Workaround for better Domoticz MQTT intergration? [#904](https://github.com/emsesp/EMS-ESP32/issues/904)
- Show MAC address without connecting to network enhancement [#933](https://github.com/emsesp/EMS-ESP32/issues/933)
- Warn user in WebUI of unsaved changes [#911](https://github.com/emsesp/EMS-ESP32/issues/911)
- Detect old Tado thermostat, device-id 0x19, no entities
- Some more HM200 entities [#500](https://github.com/emsesp/EMS-ESP32/issues/500)
- Scheduler [#701](https://github.com/emsesp/EMS-ESP32/issues/701)
- Custom Entities read from EMS bus
- Build S3 binary with github actions
- Greenstar HIU [#1158](https://github.com/emsesp/EMS-ESP32/issues/1158)
- AM200 code 10 [#1161](https://github.com/emsesp/EMS-ESP32/issues/1161)
- Ventilation device (Logavent HRV176) [#1172](https://github.com/emsesp/EMS-ESP32/issues/1172)
- Turn ETH off on wifi connect [#1167](https://github.com/emsesp/EMS-ESP32/issues/1167)
- Support for multiple EMS-ESPs with HA [#1196](https://github.com/emsesp/EMS-ESP32/issues/1196)
- Italian translation [#1199](https://github.com/emsesp/EMS-ESP32/issues/1199)
- Turkish language support [#1076](https://github.com/emsesp/EMS-ESP32/issues/1076)
- Buderus GB182 - HC1 mode change not work bug [#1193](https://github.com/emsesp/EMS-ESP32/issues/1193)
- Setting Request: Minimal flow temperature enhancement [#1192](https://github.com/emsesp/EMS-ESP32/issues/1192)
- Setting Request: Roomtemperature Switching Difference enhancement [#1191](https://github.com/emsesp/EMS-ESP32/issues/1191)
- Setting Request: Dew Point Temperature Difference enhancement [#1190](https://github.com/emsesp/EMS-ESP32/issues/1190)
- Setting Request: Control of heating circuit mode enhancement [#1187](https://github.com/emsesp/EMS-ESP32/issues/1187)
- Warn user in WebUI of unsaved changes enhancement [#911](https://github.com/emsesp/EMS-ESP32/issues/911)
- Create safebuild app to fit into factory partition to give ESP32 more flash memory enhancement [#608](https://github.com/emsesp/EMS-ESP32/issues/608)
- Support ESP32 S2, C3 mini and S3 [#605](https://github.com/emsesp/EMS-ESP32/issues/605)
- Support Buderus AM200 [#1161](https://github.com/emsesp/EMS-ESP32/issues/1161)
- Custom telegram handler [#1155](https://github.com/emsesp/EMS-ESP32/issues/1155)

## Fixed

- HA-discovery for analog sensor commands [#1035](https://github.com/emsesp/EMS-ESP32/issues/1035)
- Enum order of RC3x nofrost mode
- Heartbeat interval
- Exhaust temperature always zero on GB125/MC110/RC310 bug [#1147](https://github.com/emsesp/EMS-ESP32/issues/1147)
- thermostat modetype is not changing when mode changes (e.g. to night) bugSomething isn't working [#1098](https://github.com/emsesp/EMS-ESP32/issues/1098)
- NTP: cant apply changed timezone [#1182](https://github.com/emsesp/EMS-ESP32/issues/1182)
- Missing Status of VS1 for Buderus SM200 enhancement  [#1034](https://github.com/emsesp/EMS-ESP32/issues/1034)
- NTP: cant apply changed timezone [#1182](https://github.com/emsesp/EMS-ESP32/issues/1182)

## Changed

- Optional upgrade to platform-espressif32 6.3.0 (after 5.3.0) [#862](https://github.com/emsesp/EMS-ESP32/issues/862)
- Use byte 3 for detection RC30 active heatingcircuit [#786](https://github.com/emsesp/EMS-ESP32/issues/786)
- Write repeated selflowtemp if tx-queue is empty without verify [#954](https://github.com/emsesp/EMS-ESP32/issues/954)
- HA discovery recreate after disconnect by device [#1067](https://github.com/emsesp/EMS-ESP32/issues/1067)
- File upload: check flash size (overflow) instead of filesize
- Improved HA Discovery so previous configs no longer need to be removed when starting [#1077](https://github.com/emsesp/EMS-ESP32/pull/1077) (thanks @pswid)
- Enlarge UART-Stack to 2,5k
- Retry timeout for Mqtt-QOS1/2 10seconds
- Optimize WebUI rendering when using Dialog Boxes [#1116](https://github.com/emsesp/EMS-ESP32/issues/1116)
- Optimize Web libraries to reduce bundle size (3.6.x) [#1112](https://github.com/emsesp/EMS-ESP32/issues/1112)
- Use [espMqttClient](https://github.com/bertmelis/espMqttClient) with integrated queue [#1178](https://github.com/emsesp/EMS-ESP32/issues/1178)
- Move Sensors from Web dashboard to it's own tab enhancement [#1170](https://github.com/emsesp/EMS-ESP32/issues/1170)
- Optimize WebUI dashboard data technical [#1169](https://github.com/emsesp/EMS-ESP32/issues/1169)
