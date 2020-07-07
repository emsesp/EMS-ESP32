export interface EMSESPSettings {
  tx_mode: number;
  ems_bus_id: number;
  system_heartbeat: boolean;
  syslog_level: number;
  syslog_mark_interval: number;
  syslog_host: string;
  master_thermostat: number;
  shower_timer: boolean;
  shower_alert: boolean;
  publish_time: number;
  mqtt_format: number;
  mqtt_qos: number;
}

export enum busConnectionStatus {
  BUS_STATUS_CONNECTED = 0,
  BUS_STATUS_TX_ERRORS = 1,
  BUS_STATUS_OFFLINE = 2
}

export interface EMSESPStatus {
  version: string;
  status: busConnectionStatus;
  rx_received: number;
  tx_sent: number;
  crc_errors: number;
  tx_errors: number;
  mqtt_fails: number;
  uptime: string;
  free_mem: number;
}

export interface Device {
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

