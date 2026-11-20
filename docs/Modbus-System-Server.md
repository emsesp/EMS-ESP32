---
id: Modbus-System-Server
title: Modbus System Server
description: EMS-ESP system information accessible via Modbus registers including device count and register map version
---
# Modbus System Server

:::warning
This file has been auto-generated. Do not modify.
:::

EMS-ESP provides a server with general read only information with unit ID `1`.
It answers function code `04` (read input registers) and provides the following data:

| `Name` | `Description` | `Register offset` | `Register count` |
| ------------------ | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | --------------- | -------------- |
| NUM_DEVICES | The number of devices recognized on the EMS bus | 1 | 1 |
| REGISTER_MAP_VERSION | Version of the entity/register map. It starts at 1 and only increases when an existing register is moved or removed, not when new registers are added. A Modbus client can read this once and stop if the value is not the one it was mapped against. | 2 | 1 |
| DEVICE_TYPE_AND_ID | The device type and ID for each device. "n" in the register offset is the index of the device with 0 <= n < NUM_DEVICES. The first byte of the 16 bit value contains the [device type](Modbus-Server-IDs.md), the second byte contains the device ID. | 1000 + n | 1 |
