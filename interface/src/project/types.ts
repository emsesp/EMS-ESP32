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
  telnet_enabled: boolean;
  dallas_gpio: number;
  dallas_parasite: boolean;
  led_gpio: number;
  hide_led: boolean;
  low_clock: boolean;
  notoken_api: boolean;
  readonly_mode: boolean;
  analog_enabled: boolean;
  pbutton_gpio: number;
  trace_raw: boolean;
  board_profile: string;
  bool_format: number;
  enum_format: number;
  fahrenheit: boolean;
  phy_type: number;
  eth_power: number;
  eth_phy_addr: number;
  eth_clock_mode: number;
}

export enum busConnectionStatus {
  BUS_STATUS_CONNECTED = 0,
  BUS_STATUS_TX_ERRORS = 1,
  BUS_STATUS_OFFLINE = 2
}

export interface Status {
  status: busConnectionStatus;
  tx_mode: number;
  rx_received: number;
  tx_reads: number;
  tx_writes: number;
  rx_quality: number;
  tx_read_quality: number;
  tx_write_quality: number;
  tx_read_fails: number;
  tx_write_fails: number;
  rx_fails: number;
  sensor_fails: number;
  sensor_reads: number;
  sensor_quality: number;
  analog_fails: number;
  analog_reads: number;
  analog_quality: number;
  mqtt_count: number;
  mqtt_fails: number;
  mqtt_quality: number;
  api_calls: number;
  api_fails: number;
  api_quality: number;
  num_devices: number;
  num_sensors: number;
  num_analogs: number;
  uptime: number;
}

export interface Device {
  i: number; // id
  t: string; // type
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
  t?: number; // temp, optional
  o: number; // offset
  u: number; // uom
}

export interface Analog {
  i: number;
  n: string;
  v?: number;
  u: number;
  o: number;
  f: number;
  t: number;
}

export interface WriteSensor {
  id_str: string;
  name: string;
  offset: number;
}

export interface SensorData {
  sensors: Sensor[];
  analogs: Analog[];
}

export interface CoreData {
  devices: Device[];
  active_sensors: number;
  analog_enabled: boolean;
}

export interface DeviceShort {
  i: number; // id
  d: number; // deviceid
  p: number; // productid
  s: string; // shortname
}

export interface Devices {
  devices: DeviceShort[];
}

export interface DeviceValue {
  v?: any; // value, in any format
  u: number; // uom
  n: string; // name
  c: string; // command
  l: string[]; // list
  h?: string; // help text
  s?: string; // steps for up/down
  m?: string; // min
  x?: string; // max
}

export interface DeviceData {
  label: string;
  data: DeviceValue[];
}

export interface DeviceEntity {
  v?: any; // value, in any format
  n: string; // name
  s: string; // shortname
  m: number; // mask
  w?: boolean; // writeable
}

export interface MaskedEntities {
  id: number;
  entity_ids: string[];
}

export interface UniqueID {
  id: number;
}

export enum DeviceValueUOM {
  NONE = 0,
  DEGREES,
  DEGREES_R,
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
  FAHRENHEIT,
  MV,
  SQM,
  M3,
  L
}

export const DeviceValueUOM_s = [
  '',
  '°C',
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
  '°F',
  'mV',
  'sqm',
  'm3',
  'l'
];

export enum AnalogType {
  NOTUSED = 0,
  DIGITAL_IN,
  COUNTER,
  ADC,
  TIMER,
  RATE,
  DIGITAL_OUT,
  PWM_0,
  PWM_1,
  PWM_2
}

export const AnalogTypeNames = [
  '(disabled)',
  'Digital in',
  'Counter',
  'ADC',
  'Timer',
  'Rate',
  'Digital out',
  'PWM 0',
  'PWM 1',
  'PWM 2'
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
  OLIMEX: 'Olimex ESP32-EVB',
  OLIMEXPOE: 'Olimex ESP32-POE'
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
  eth_power: number;
  eth_phy_addr: number;
  eth_clock_mode: number;
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

export interface WriteAnalog {
  id: number;
  name: string;
  factor: number;
  offset: number;
  uom: number;
  type: number;
}
