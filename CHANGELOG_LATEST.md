# Changelog

# [3.5.1]

## **IMPORTANT! BREAKING CHANGES**

-

## Added

- detect old Tado thermostat, device-id 0x19, no entities

## Fixed

-

## Changed

- Use byte 0 for detection RC30 active heatingcircuit [#786](https://github.com/emsesp/EMS-ESP32/issues/786)
- Write repeated selflowtemp if tx-queue is empty without verify [#954](https://github.com/emsesp/EMS-ESP32/issues/954)