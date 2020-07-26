export interface EMSESPSettings {
  tx_mode: number;
  ems_bus_id: number;
  syslog_level: number;
  syslog_mark_interval: number;
  syslog_host: string;
  master_thermostat: number;
  shower_timer: boolean;
  shower_alert: boolean;
}

export enum busConnectionStatus {
  BUS_STATUS_CONNECTED = 0,
  BUS_STATUS_TX_ERRORS = 1,
  BUS_STATUS_OFFLINE = 2
}

export interface EMSESPStatus {
  status: busConnectionStatus;
  rx_received: number;
  tx_sent: number;
  crc_errors: number;
  tx_errors: number;
}

export interface Device {
  id: number;
  type: string;
  brand: string;
  name: string;
  deviceid: number;
  productid: number;
  version: string;
}

export interface EMSESPDevices {
  devices: Device[];
}

export interface DeviceData {
  name: string;
  value: string;
}

export interface EMSESPDeviceData {
  deviceName: string;
  deviceData: DeviceData[];
}

