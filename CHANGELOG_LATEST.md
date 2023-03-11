# Changelog

# [3.5.1]

## Added

- Detect old Tado thermostat, device-id 0x19, no entities
- Some more HM200 entities [#500](https://github.com/emsesp/EMS-ESP32/issues/500)
- Add entity to force heating off (for systems without thermostat) [#951](https://github.com/emsesp/EMS-ESP32/issues/951)

## Fixed

- HA-discovery for analog sensor commands [#1035](https://github.com/emsesp/EMS-ESP32/issues/1035)

## Changed

- Use byte 0 for detection RC30 active heatingcircuit [#786](https://github.com/emsesp/EMS-ESP32/issues/786)
- Write repeated selflowtemp if tx-queue is empty without verify [#954](https://github.com/emsesp/EMS-ESP32/issues/954)
- HA discovery recreate after disconnect by device [#1067](https://github.com/emsesp/EMS-ESP32/issues/1067)
- File upload: check flash size (overflow) instead of filesize
