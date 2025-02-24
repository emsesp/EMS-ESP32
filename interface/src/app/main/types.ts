export interface Settings {
  locale: string;
  tx_mode: number;
  ems_bus_id: number;
  syslog_enabled: boolean;
  syslog_level: number;
  syslog_mark_interval: number;
  syslog_host: string;
  syslog_port: number;
  boiler_heatingoff: boolean;
  remote_timeout_en: boolean;
  remote_timeout: number;
  shower_timer: boolean;
  shower_alert: boolean;
  shower_alert_coldshot: number;
  shower_alert_trigger: number;
  shower_min_duration: number;
  rx_gpio: number;
  tx_gpio: number;
  telnet_enabled: boolean;
  dallas_gpio: number;
  dallas_parasite: boolean;
  led_gpio: number;
  led_type: number;
  hide_led: boolean;
  low_clock: boolean;
  notoken_api: boolean;
  readonly_mode: boolean;
  analog_enabled: boolean;
  pbutton_gpio: number;
  trace_raw: boolean;
  board_profile: string;
  bool_format: number;
  bool_dashboard: number;
  enum_format: number;
  fahrenheit: boolean;
  phy_type: number;
  eth_power: number;
  eth_phy_addr: number;
  eth_clock_mode: number;
  platform: string;
  modbus_enabled: boolean;
  modbus_port: number;
  modbus_max_clients: number;
  modbus_timeout: number;
  developer_mode: boolean;
}

export enum busConnectionStatus {
  BUS_STATUS_CONNECTED = 0,
  BUS_STATUS_TX_ERRORS = 1,
  BUS_STATUS_OFFLINE = 2
}

export interface Stat {
  id: number; // id
  s: number; // success
  f: number; // fail
  q: number; // quality
}

export interface Activity {
  stats: Stat[];
}

export interface Device {
  id: number; // id index
  tn: string; // device type translated name
  t: number; // device type id
  b: string; // brand
  n: string; // name
  d: number; // deviceid
  p: number; // productid
  v: string; // version
  e: number; // total number of entities
  url?: string; // lowercase type name used in API URL
}

export interface TemperatureSensor {
  id: string; // id string
  n: string; // name/alias
  t?: number; // temp, optional
  o: number; // offset
  u: number; // uom
  o_n?: string;
}

export interface AnalogSensor {
  id: number;
  g: number; // GPIO
  n: string;
  v: number;
  u: number;
  o: number;
  f: number;
  t: number;
  d: boolean; // deleted flag
  o_n?: string;
}

export interface WriteTemperatureSensor {
  id: string;
  name: string;
  offset: number;
}

export interface SensorData {
  ts: TemperatureSensor[];
  as: AnalogSensor[];
  analog_enabled: boolean;
  platform: string;
}

export interface CoreData {
  connected: boolean;
  devices: Device[];
}

export interface DashboardItem {
  id: number; // unique index
  t?: number; // type from DeviceType
  n?: string; // name, optional
  dv?: DeviceValue; // device value, optional
  nodes?: DashboardItem[]; // children nodes, optional
}

export interface DashboardData {
  connected: boolean; // true if connected to EMS bus
  nodes: DashboardItem[];
}

export interface DeviceValue {
  id: string; // index, contains mask+name
  v?: unknown; // value, Number, String or Boolean - can be undefined
  u?: number; // uom, optional
  c?: string; // command, optional
  l?: string[]; // list, optional
  h?: string; // help text, optional
  s?: string; // steps for up/down, optional
  m?: number; // min, optional
  x?: number; // max, optional
}

export interface DeviceData {
  nodes: DeviceValue[];
}

export interface DeviceEntity {
  id: string; // shortname
  v?: unknown; // value, in any format, optional
  n?: string; // fullname, optional
  cn?: string; // custom fullname, optional
  t?: string; // tag for name
  m: DeviceEntityMask; // mask
  w: boolean; // writeable
  mi?: number; // min value
  ma?: number; // max value
  o_m?: number; // original mask before edits
  o_cn?: string; // original cn before edits
  o_mi?: number; // original min value
  o_ma?: number; // original max value
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
  L,
  KMIN,
  K,
  VOLTS,
  MBAR,
  LH,
  CTKWH
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
  'µA',
  'bar',
  'kW',
  'W',
  'KB',
  'seconds',
  'dBm',
  '°F',
  'mV',
  'm²',
  'm³',
  'l',
  'K*min',
  'K',
  'V',
  'mbar',
  'l/h',
  'ct/kWh'
];

export enum AnalogType {
  REMOVED = -1,
  NOTUSED = 0,
  DIGITAL_IN = 1,
  COUNTER = 2,
  ADC = 3,
  TIMER = 4,
  RATE = 5,
  DIGITAL_OUT = 6,
  PWM_0 = 7,
  PWM_1 = 8,
  PWM_2 = 9
}

export const AnalogTypeNames = [
  '(disabled)',
  'Digital In',
  'Counter',
  'ADC',
  'Timer',
  'Rate',
  'Digital Out',
  'PWM 0',
  'PWM 1',
  'PWM 2'
];

type BoardProfiles = Record<string, string>;

export const BOARD_PROFILES: BoardProfiles = {
  S32: 'BBQKees Gateway S32',
  S32S3: 'BBQKees Gateway S3',
  E32: 'BBQKees Gateway E32',
  E32V2: 'BBQKees Gateway E32 V2',
  NODEMCU: 'NodeMCU 32S',
  'MH-ET': 'MH-ET Live D1 Mini',
  LOLIN: 'Lolin D32',
  OLIMEX: 'Olimex ESP32-EVB',
  OLIMEXPOE: 'Olimex ESP32-POE',
  C3MINI: 'Wemos C3 Mini',
  S2MINI: 'Wemos S2 Mini',
  S3MINI: 'Liligo S3'
};

export interface BoardProfile {
  board_profile: string;
  led_gpio: number;
  led_type: number;
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
  cmd: string;
  id: number;
  data?: string; // optional
}

export interface Action {
  action: string;
  param?: string; // optional
}

export interface WriteAnalogSensor {
  id: number;
  gpio: number;
  name: string;
  factor: number;
  offset: number;
  uom: number;
  type: number;
  deleted: boolean;
}

export enum DeviceEntityMask {
  DV_DEFAULT = 0,
  DV_WEB_EXCLUDE = 1,
  DV_API_MQTT_EXCLUDE = 2,
  DV_READONLY = 4,
  DV_FAVORITE = 8,
  DV_DELETED = 128
}

export interface ScheduleItem {
  id: number; // unique index
  active: boolean;
  deleted?: boolean;
  flags: number;
  time: string; // also used for Condition and On Change
  cmd: string;
  value: string;
  name: string; // can be empty
  o_id?: number;
  o_active?: boolean;
  o_deleted?: boolean;
  o_flags?: number;
  o_time?: string;
  o_cmd?: string;
  o_value?: string;
  o_name?: string;
}

export interface Schedule {
  schedule: ScheduleItem[];
}

export interface ModuleItem {
  id: number; // unique index
  key: string;
  name: string;
  author: string;
  version: string;
  status: number;
  message: string;
  enabled: boolean;
  license: string;
  o_enabled?: boolean;
  o_license?: string;
}

export interface Modules {
  modules: ModuleItem[];
}

export enum ScheduleFlag {
  SCHEDULE_SUN = 1,
  SCHEDULE_MON = 2,
  SCHEDULE_TUE = 4,
  SCHEDULE_WED = 8,
  SCHEDULE_THU = 16,
  SCHEDULE_FRI = 32,
  SCHEDULE_SAT = 64,
  // types...
  SCHEDULE_DAY = 0, // no bits set
  SCHEDULE_TIMER = 128, // bit 8
  SCHEDULE_ONCHANGE = 129, // bit 1
  SCHEDULE_CONDITION = 130, // bit 2
  SCHEDULE_IMMEDIATE = 132 // bit 3
}

export interface EntityItem {
  id: number; // unique number
  ram: number;
  name: string;
  device_id: number | string;
  type_id: number | string;
  offset: number;
  factor: number | string;
  uom: number;
  value_type: number;
  value?: unknown;
  writeable: boolean;
  deleted?: boolean;
  o_id?: number;
  o_ram?: number;
  o_name?: string;
  o_device_id?: number | string;
  o_type_id?: number | string;
  o_offset?: number;
  o_factor?: number | string;
  o_uom?: number;
  o_value_type?: number;
  o_deleted?: boolean;
  o_writeable?: boolean;
  o_value?: unknown;
}

export interface Entities {
  entities: EntityItem[];
}

// matches emsdevice.h DeviceType
export const enum DeviceType {
  SYSTEM = 0,
  TEMPERATURESENSOR = 1,
  ANALOGSENSOR = 2,
  SCHEDULER = 3,
  CUSTOM = 4,
  BOILER,
  THERMOSTAT,
  MIXER,
  SOLAR,
  HEATPUMP,
  GATEWAY,
  SWITCH,
  CONTROLLER,
  CONNECT,
  ALERT,
  EXTENSION,
  GENERIC,
  HEATSOURCE,
  VENTILATION,
  WATER,
  POOL,
  UNKNOWN
}

// matches emsdevicevalue.h
export const enum DeviceValueType {
  BOOL,
  INT8,
  UINT8,
  INT16,
  UINT16,
  UINT24,
  TIME, // same as UINT24
  UINT32,
  ENUM,
  STRING, // RAW
  CMD
}

export const DeviceValueTypeNames = [
  'BOOL',
  'INT8',
  'UINT8',
  'INT16',
  'UINT16',
  'UINT24',
  'TIME',
  'UINT32',
  'ENUM',
  'RAW',
  'CMD'
];
