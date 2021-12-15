export interface Settings {
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
  disable_telnet: boolean;
  phy_type: number;
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
  enum_format: number;
}

export enum busConnectionStatus {
  BUS_STATUS_CONNECTED = 0,
  BUS_STATUS_TX_ERRORS = 1,
  BUS_STATUS_OFFLINE = 2
}

export interface Status {
  status: busConnectionStatus;
  rx_received: number;
  tx_sent: number;
  rx_quality: number;
  tx_quality: number;
  num_devices: number;
}

export interface Device {
  i: number; // id
  t: string; // type
  sn: string; // shortname
  b: string; // brand
  n: string; // name
  d: number; // deviceid
  p: number; // productid
  v: string; // version
  e: number; // number of entries
}

export interface Sensor {
  is: string; // id string
  n: string; // name/alias
  t: number; // temp
  o: number; // offset
}

export interface WriteSensor {
  id_str: string;
  name: string;
  offset: number;
}

export interface Data {
  devices: Device[];
  sensors: Sensor[];
  analog?: number; // is optional
}

export interface DeviceValue {
  v: any; // value, in any format
  u: number; // uom
  n: string; // name
  c: string; // command
  l: string[]; // list
}

export interface DeviceData {
  id: number;
  type: string;
  data: DeviceValue[];
}

export interface DeviceID {
  id: number;
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
  MV,
  TIMES,
  OCLOCK
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
  'second',
  'dBm',
  'mV',
  'time',
  "o'clock"
];

type BoardProfiles = {
  [name: string]: string;
};

export const BOARD_PROFILES: BoardProfiles = {
  S32: 'BBQKees Gateway S32',
  E32: 'BBQKees Gateway E32',
  NODEMCU: 'NodeMCU 32S',
  'MH-ET': 'MH-ET Live D1 Mini',
  LOLIN: 'Lolin D32',
  OLIMEX: 'Olimex ESP32-EVB'
};

export interface BoardProfileName {
  board_profile: string;
}

export interface BoardProfile {
  board_profile: string;
  led_gpio: number;
  dallas_gpio: number;
  rx_gpio: number;
  tx_gpio: number;
  pbutton_gpio: number;
  phy_type: number;
}

export interface APIcall {
  device: string;
  entity: string;
  id: any;
}

export interface WriteValue {
  id: number;
  devicevalue: DeviceValue;
}
