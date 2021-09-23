export interface EMSESPSettings {
  tx_mode: number;
  ems_bus_id: number;
  syslog_enabled: boolean;
  syslog_level: number;
  syslog_mark_interval: number;
  syslog_host: string;
  syslog_port: number;
  master_thermostat: number;
  shower_timer: boolean;
  shower_alert: boolean;
  rx_gpio: number;
  tx_gpio: number;
  dallas_gpio: number;
  dallas_parasite: boolean;
  led_gpio: number;
  hide_led: boolean;
  low_clock: boolean;
  notoken_api: boolean;
  analog_enabled: boolean;
  pbutton_gpio: number;
  trace_raw: boolean;
  board_profile: string;
  bool_format: number;
  dallas_format: number;
  enum_format: number;
  aux_gpio: number;
  aux_function: number;
  aux_pump_delay: number;
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
  rx_quality: number;
  tx_quality: number;
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

export interface Sensor {
  no: number;
  id: string;
  temp: number;
  offset: number;
}

export interface EMSESPData {
  devices: Device[];
  sensors: Sensor[];
  analog: number;
}

export interface DeviceValue {
  v: any;
  u: number;
  n: string;
  c: string;
  l: string[];
}

export interface EMSESPDeviceData {
  name: string;
  data: DeviceValue[];
}

export enum DeviceValueUOM {
  NONE = 0,
  DEGREES,
  PERCENT,
  LMIN,
  KWH,
  WH,
  HOURS,
  MINUTES,
  UA,
  BAR,
  KW,
  W,
  KB,
  SECONDS,
  DBM,
  MV
}

export const DeviceValueUOM_s = [
  '',
  '°C',
  '%',
  'l/min',
  'kWh',
  'Wh',
  'hours',
  'minutes',
  'uA',
  'bar',
  'kW',
  'W',
  'KB',
  'seconds',
  'dBm',
  'mV'
];
