const express = require('express')
const compression = require('compression')
const SSE = require('express-sse')
const path = require('path')
const msgpack = require('@msgpack/msgpack')
const WebSocket = require('ws')

// REST API
const rest_server = express()
const port = 3080
const REST_ENDPOINT_ROOT = '/rest/'
rest_server.use(compression())
rest_server.use(express.static(path.join(__dirname, '../interface/build')))
rest_server.use(express.json())

// API endpoint
const API_ENDPOINT_ROOT = '/api/'

// LOG
const LOG_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'logSettings'
log_settings = {
  level: 6,
  max_messages: 50,
  compact: false,
}

const FETCH_LOG_ENDPOINT = REST_ENDPOINT_ROOT + 'fetchLog'
const fetch_log = {
  events: [
    {
      t: '000+00:00:00.001',
      l: 3,
      i: 1,
      n: 'system',
      m: 'this is message 3',
    },
    {
      t: '000+00:00:00.002',
      l: 4,
      i: 2,
      n: 'ntp',
      m: 'this is message 4',
    },
    {
      t: '000+00:00:00.002',
      l: 5,
      i: 3,
      n: 'mqtt',
      m: 'this is message 5',
    },
    {
      t: '000+00:00:00.002',
      l: 6,
      i: 444,
      n: 'command',
      m: 'this is message 6',
    },
    {
      t: '000+00:00:00.002',
      l: 7,
      i: 5555,
      n: 'emsesp',
      m: 'this is message 7',
    },
    {
      t: '000+00:00:00.002',
      l: 8,
      i: 666666,
      n: 'thermostat',
      m: 'this is message 8',
    },
  ],
}

// NTP
const NTP_STATUS_ENDPOINT = REST_ENDPOINT_ROOT + 'ntpStatus'
const NTP_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'ntpSettings'
const TIME_ENDPOINT = REST_ENDPOINT_ROOT + 'time'
ntp_settings = {
  enabled: true,
  server: 'time.google.com',
  tz_label: 'Europe/Amsterdam',
  tz_format: 'CET-1CEST,M3.5.0,M10.5.0/3',
}
const ntp_status = {
  status: 1,
  utc_time: '2021-04-01T14:25:42Z',
  local_time: '2021-04-01T16:25:42',
  server: 'time.google.com',
  uptime: 856,
}

// AP
const AP_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'apSettings'
const AP_STATUS_ENDPOINT = REST_ENDPOINT_ROOT + 'apStatus'
ap_settings = {
  provision_mode: 1,
  ssid: 'ems-esp',
  password: 'ems-esp-neo',
  local_ip: '192.168.4.1',
  gateway_ip: '192.168.4.1',
  subnet_mask: '255.255.255.0',
  channel: 1,
  ssid_hidden: true,
  max_clients: 4,
}
const ap_status = {
  status: 1,
  ip_address: '192.168.4.1',
  mac_address: '3C:61:05:03:AB:2D',
  station_num: 0,
}

// NETWORK
const NETWORK_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'networkSettings'
const NETWORK_STATUS_ENDPOINT = REST_ENDPOINT_ROOT + 'networkStatus'
const SCAN_NETWORKS_ENDPOINT = REST_ENDPOINT_ROOT + 'scanNetworks'
const LIST_NETWORKS_ENDPOINT = REST_ENDPOINT_ROOT + 'listNetworks'
network_settings = {
  ssid: 'myWifi',
  password: 'myPassword',
  hostname: 'ems-esp',
  nosleep: true,
  tx_power: 20,
  static_ip_config: false,
}
const network_status = {
  status: 3,
  local_ip: '10.10.10.101',
  mac_address: '3C:61:05:03:AB:2C',
  rssi: -41,
  ssid: 'home',
  bssid: '06:ED:DA:FE:B4:68',
  channel: 11,
  subnet_mask: '255.255.255.0',
  gateway_ip: '10.10.10.1',
  dns_ip_1: '10.10.10.1',
  dns_ip_2: '0.0.0.0',
}
const list_networks = {
  networks: [
    {
      rssi: -40,
      ssid: '',
      bssid: 'FC:EC:DA:FD:B4:68',
      channel: 11,
      encryption_type: 3,
    },
    {
      rssi: -41,
      ssid: 'home',
      bssid: '02:EC:DA:FD:B4:68',
      channel: 11,
      encryption_type: 3,
    },
    {
      rssi: -42,
      ssid: '',
      bssid: '06:EC:DA:FD:B4:68',
      channel: 11,
      encryption_type: 3,
    },
    {
      rssi: -73,
      ssid: '',
      bssid: 'FC:EC:DA:17:D4:7E',
      channel: 1,
      encryption_type: 3,
    },
    {
      rssi: -73,
      ssid: 'office',
      bssid: '02:EC:DA:17:D4:7E',
      channel: 1,
      encryption_type: 3,
    },
    {
      rssi: -75,
      ssid: 'Erica',
      bssid: 'C8:D7:19:9A:88:BD',
      channel: 2,
      encryption_type: 3,
    },
    {
      rssi: -75,
      ssid: '',
      bssid: 'C6:C9:E3:FF:A5:DE',
      channel: 2,
      encryption_type: 3,
    },
    {
      rssi: -76,
      ssid: 'Bruin',
      bssid: 'C0:C9:E3:FF:A5:DE',
      channel: 2,
      encryption_type: 3,
    },
  ],
}

// OTA
const OTA_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'otaSettings'
ota_settings = {
  enabled: true,
  port: 8266,
  password: 'ems-esp-neo',
}

// MQTT
const MQTT_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'mqttSettings'
const MQTT_STATUS_ENDPOINT = REST_ENDPOINT_ROOT + 'mqttStatus'
mqtt_settings = {
  enabled: true,
  host: '192.168.1.4',
  port: 1883,
  base: 'ems-esp',
  username: '',
  password: '',
  client_id: 'ems-esp',
  keep_alive: 60,
  clean_session: true,
  max_topic_length: 128,
  publish_time_boiler: 10,
  publish_time_thermostat: 10,
  publish_time_solar: 10,
  publish_time_mixer: 10,
  publish_time_other: 10,
  publish_time_sensor: 10,
  mqtt_qos: 0,
  mqtt_retain: false,
  ha_climate_format: 1,
  ha_enabled: true,
  nested_format: 1,
  send_response: true,
  publish_single: false,
}
const mqtt_status = {
  enabled: true,
  connected: true,
  client_id: 'ems-esp',
  disconnect_reason: 0,
  mqtt_fails: 0,
}

// SYSTEM
const FEATURES_ENDPOINT = REST_ENDPOINT_ROOT + 'features'
const VERIFY_AUTHORIZATION_ENDPOINT = REST_ENDPOINT_ROOT + 'verifyAuthorization'
const SYSTEM_STATUS_ENDPOINT = REST_ENDPOINT_ROOT + 'systemStatus'
const SECURITY_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'securitySettings'
const RESTART_ENDPOINT = REST_ENDPOINT_ROOT + 'restart'
const FACTORY_RESET_ENDPOINT = REST_ENDPOINT_ROOT + 'factoryReset'
const UPLOAD_FIRMWARE_ENDPOINT = REST_ENDPOINT_ROOT + 'uploadFirmware'
const SIGN_IN_ENDPOINT = REST_ENDPOINT_ROOT + 'signIn'
const GENERATE_TOKEN_ENDPOINT = REST_ENDPOINT_ROOT + 'generateToken'
const system_status = {
  emsesp_version: '3.4.0demo',
  esp_platform: 'ESP32',
  max_alloc_heap: 113792,
  psram_size: 0,
  free_psram: 0,
  cpu_freq_mhz: 240,
  free_heap: 193340,
  sdk_version: 'v3.3.5-1-g85c43024c',
  flash_chip_size: 4194304,
  flash_chip_speed: 40000000,
  fs_total: 65536,
  fs_used: 16384,
  uptime: '000+00:15:42.707',
}
security_settings = {
  jwt_secret: 'naughty!',
  users: [
    { username: 'admin', password: 'admin', admin: true },
    { username: 'guest', password: 'guest', admin: false },
  ],
}
const features = {
  project: true,
  security: true,
  mqtt: true,
  ntp: true,
  ota: true,
  upload_firmware: true,
}
const verify_authentication = { access_token: '1234' }
const signin = {
  access_token:
    'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6ImFkbWluIiwiYWRtaW4iOnRydWUsInZlcnNpb24iOiIzLjAuMmIwIn0.MsHSgoJKI1lyYz77EiT5ZN3ECMrb4mPv9FNy3udq0TU',
}
const generate_token = { token: '1234' }

// EMS-ESP Project specific
const EMSESP_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'settings'
const EMSESP_CORE_DATA_ENDPOINT = REST_ENDPOINT_ROOT + 'coreData'
const EMSESP_SENSOR_DATA_ENDPOINT = REST_ENDPOINT_ROOT + 'sensorData'
const EMSESP_DEVICES_ENDPOINT = REST_ENDPOINT_ROOT + 'devices'
const EMSESP_SCANDEVICES_ENDPOINT = REST_ENDPOINT_ROOT + 'scanDevices'
const EMSESP_DEVICEDATA_ENDPOINT = REST_ENDPOINT_ROOT + 'deviceData'
const EMSESP_DEVICEENTITIES_ENDPOINT = REST_ENDPOINT_ROOT + 'deviceEntities'
const EMSESP_STATUS_ENDPOINT = REST_ENDPOINT_ROOT + 'status'
const EMSESP_BOARDPROFILE_ENDPOINT = REST_ENDPOINT_ROOT + 'boardProfile'
const EMSESP_WRITE_VALUE_ENDPOINT = REST_ENDPOINT_ROOT + 'writeValue'
const EMSESP_WRITE_SENSOR_ENDPOINT = REST_ENDPOINT_ROOT + 'writeSensor'
const EMSESP_WRITE_ANALOG_ENDPOINT = REST_ENDPOINT_ROOT + 'writeAnalog'
const EMSESP_EXCLUDE_ENTITIES_ENDPOINT = REST_ENDPOINT_ROOT + 'excludeEntities'
const EMSESP_RESET_CUSTOMIZATIONS_ENDPOINT = REST_ENDPOINT_ROOT + 'resetCustomizations'

settings = {
  tx_mode: 1,
  ems_bus_id: 11,
  syslog_enabled: false,
  syslog_level: 3,
  trace_raw: false,
  syslog_mark_interval: 0,
  syslog_host: '192.168.1.4',
  syslog_port: 514,
  master_thermostat: 0,
  shower_timer: true,
  shower_alert: false,
  rx_gpio: 23,
  tx_gpio: 5,
  phy_type: 0,
  dallas_gpio: 3,
  dallas_parasite: false,
  led_gpio: 2,
  hide_led: false,
  notoken_api: false,
  low_clock: false,
  telnet_enabled: true,
  analog_enabled: false,
  pbutton_gpio: 0,
  board_profile: 'S32',
  bool_format: 1,
  enum_format: 1,
}

const emsesp_devices = {
  devices: [
    {
      i: 1,
      d: 23,
      p: 77,
      s: 'Thermostat1',
    },
    {
      i: 2,
      d: 8,
      p: 123,
      s: 'Boiler',
    },
    {
      i: 4,
      d: 16,
      p: 165,
      s: 'Thermostat2',
    },
  ],
}

const emsesp_coredata = {
  devices: [
    {
      i: 1,
      t: 'Thermostat',
      s: 'Thermostat1',
      b: '',
      n: 'RC20/Moduline 300',
      d: 23,
      p: 77,
      v: '03.03',
      e: 5,
    },
    {
      i: 2,
      t: 'Boiler',
      s: 'Boiler',
      b: 'Nefit',
      n: 'GBx72/Trendline/Cerapur/Greenstar Si/27i',
      d: 8,
      p: 123,
      v: '06.01',
      e: 62,
    },
    {
      i: 3,
      t: 'Controller',
      s: 'Controller',
      b: '',
      n: 'BC10',
      d: 9,
      p: 190,
      v: '01.03',
      e: 0,
    },
    {
      i: 4,
      t: 'Thermostat',
      s: 'Thermostat2',
      b: 'Buderus',
      n: 'RC100/Moduline 1000/1010',
      d: 16,
      p: 165,
      v: '04.01',
      e: 3,
    },
  ],
  active_sensors: 8,
  analog_enabled: false,
}

const emsesp_sensordata = {
  sensors: [
    { is: '28-233D-9497-0C03', n: 'name1', t: 25.7, o: 1.2, u: 1 },
    { is: '28-243D-7437-1E3A', n: 'name2', t: 26.1, o: 0, u: 1 },
    { is: '28-243E-7437-1E3B', n: 'name3', t: 27.1, o: 0, u: 16 },
    { is: '28-183D-1892-0C33', n: 'name4', o: 2, u: 1 },
  ],
  // sensors: [],
  analogs: [
    { i: 36, n: 'name1', v: 12, u: 17, o: 17, f: 0, t: 0 },
    { i: 37, n: 'name2', v: 13, u: 17, o: 17, f: 0, t: 1 },
    { i: 39, n: 'name4', v: 14, u: 0, o: 0, f: 0, t: 2 },
    { i: 40, n: 'name5', v: 16, u: 0, o: 0, f: 0, t: 3 },
  ],
  // analogs: [],
}

const status = {
  status: 0,
  tx_mode: 1,
  rx_received: 344,
  tx_sent: 104,
  rx_quality: 100,
  tx_quality: 100,
  tx_fails: 0,
  rx_fails: 0,
  sensor_fails: 0,
  sensor_reads: 222,
  sensor_quality: 100,
  mqtt_count: 50,
  mqtt_fails: 12,
  mqtt_quality: 1,
  api_calls: 50,
  api_fails: 0,
  api_quality: 100,
  num_devices: 3,
  num_sensors: 1,
}

// Dashboard data
const emsesp_devicedata_1 = {
  label: 'RC20/Moduline 300',
  data: [
    {
      v: '(0)',
      u: 0,
      n: 'error code',
      c: '',
    },
    {
      v: '14:54:39 06/06/2021',
      u: 0,
      n: 'date/time',
      c: '',
    },
    {
      v: 18,
      u: 1,
      n: 'hc1 selected room temperature',
      c: 'hc1/seltemp',
    },
    {
      v: 22.6,
      u: 1,
      n: 'hc1 current room temperature',
      c: '',
    },
    {
      v: 'auto',
      u: 0,
      n: 'hc1 mode',
      c: 'hc1/mode',
    },
  ],
}

const emsesp_devicedata_2 = {
  label: 'GBx72/Trendline/Cerapur/Greenstar Si/27i',
  data: [
    { u: 0, n: 'reset message', c: 'testcommand' },
    { v: 'off', u: 0, n: 'heating active' },
    { v: 'off', u: 0, n: 'warm water active' },
    { v: 5, u: 1, n: 'selected flow temperature', c: 'selflowtemp' },
    { v: 0, u: 3, n: 'burner selected max power', c: 'selburnpow' },
    { v: 0, u: 3, n: 'heating pump modulation' },
    { v: 42.9, u: 1, n: 'current flow temperature' },
    { v: 41.8, u: 1, n: 'return temperature' },
    { v: 1.6, u: 10, n: 'system pressure' },
    { v: 45, u: 1, n: 'actual boiler temperature' },
    { v: 'off', u: 0, n: 'gas' },
    { v: 0, u: 9, n: 'flame current' },
    { v: 'off', u: 0, n: 'heating pump' },
    { v: 'off', u: 0, n: 'fan' },
    { v: 'off', u: 0, n: 'ignition' },
    {
      v: 'on',
      u: 0,
      n: 'heating activated',
      c: 'heatingactivated',
      l: ['off', 'on'],
    },
    { v: 75, u: 1, n: 'heating temperature', c: 'heatingtemp' },
    { v: 90, u: 3, n: 'burner pump max power', c: 'pumpmodmax' },
    { v: 55, u: 3, n: 'burner pump min power', c: 'pumpmodmin' },
    { v: 1, u: 8, n: 'pump delay', c: 'pumpdelay' },
    { v: 10, u: 8, n: 'burner min period', c: 'burnminperiod' },
    { v: 0, u: 3, n: 'burner min power', c: 'burnminpower' },
    { v: 77, u: 3, n: 'burner max power', c: 'burnmaxpower' },
    { v: -6, u: 1, n: 'hysteresis on temperature', c: 'boilhyston' },
    { v: 6, u: 1, n: 'hysteresis off temperature', c: 'boilhystoff' },
    { v: 0, u: 3, n: 'burner current power' },
    { v: 317694, u: 17, n: 'burner starts' },
    { v: 524115, u: 8, n: 'total burner operating time' },
    { v: 424286, u: 8, n: 'total heat operating time' },
    { v: 4571225, u: 8, n: 'total UBA operating time' },
    { v: '1C(210) 06.06.2020 12:07', u: 0, n: 'last error code' },
    { v: '0H', u: 0, n: 'service code' },
    { v: 203, u: 0, n: 'service code number' },
    { v: ' ', u: 0, n: 'maintenance message' },
    {
      v: 'off',
      u: 0,
      n: 'maintenance scheduled',
      c: 'maintenance',
      l: ['off', 'time', 'date'],
    },
    { v: 6000, u: 7, n: 'maintenance set time', c: 'maintenancetime' },
    { v: '01.01.2012', u: 0, n: 'maintenance set date', c: 'maintenancedate' },
    { v: 60, u: 1, n: 'ww selected temperature', c: 'wwseltemp' },
    { v: 62, u: 1, n: 'ww set temperature' },
    { v: 'flow', u: 0, n: 'ww type' },
    {
      v: 'eco',
      u: 0,
      n: 'ww comfort',
      c: 'wwcomfort',
      l: ['hot', 'eco', 'intelligent'],
    },
    { v: 40, u: 0, n: 'ww flow temperature offset', c: 'wwflowtempoffset' },
    { v: 100, u: 3, n: 'ww max power', c: 'wwmaxpower' },
    {
      v: 'off',
      u: 0,
      n: 'ww circulation pump available',
      c: 'wwcircpump',
      l: ['off', 'on'],
    },
    { v: '3-way valve', u: 0, n: 'ww charging type' },
    { v: -5, u: 1, n: 'ww hysteresis on temperature', c: 'wwhyston' },
    { v: 0, u: 1, n: 'ww hysteresis off temperature', c: 'wwhystoff' },
    { v: 70, u: 1, n: 'ww disinfection temperature', c: 'wwdisinfectiontemp' },
    {
      v: 'off',
      u: 0,
      n: 'ww circulation pump frequency',
      c: 'wwcircmode',
      l: ['off', '1x3min', '2x3min', '3x3min', '4x3min', '5x3min', '6x3min', 'continuous'],
    },
    {
      v: 'off',
      u: 0,
      n: 'ww circulation active',
      c: 'wwcirc',
      l: ['off', 'on'],
    },
    { v: 37.1, u: 1, n: 'ww current intern temperature' },
    { v: 0, u: 4, n: 'ww current tap water flow' },
    { v: 37.2, u: 1, n: 'ww storage intern temperature' },
    { v: 'on', u: 0, n: 'ww activated', c: 'wwactivated', l: ['off', 'on'] },
    {
      v: 'off',
      u: 0,
      n: 'ww one time charging',
      c: 'wwonetime',
      l: ['off', 'on'],
    },
    {
      v: 'off',
      u: 0,
      n: 'ww disinfection',
      c: 'wwdisinfect',
      l: ['off', 'on'],
    },
    { v: 'off', u: 0, n: 'ww charging' },
    { v: 'off', u: 0, n: 'ww recharging' },
    { v: 'on', u: 0, n: 'ww temperature ok' },
    { v: 'off', u: 0, n: 'ww active' },
    { v: 'on', u: 0, n: 'ww heating' },
    { v: 282323, u: 17, n: 'ww starts' },
    { v: 99829, u: 8, n: 'ww active time' },
  ],
}

const emsesp_devicedata_3 = {
  label: 'Controller',
  data: [],
}

const emsesp_devicedata_4 = {
  label: 'RC100/Moduline 1000/1010',
  data: [
    {
      v: 16,
      u: 1,
      n: 'hc2 selected room temperature',
      c: 'hc2/seltemp',
      x: false,
    },
    {
      v: 18.6,
      u: 1,
      n: 'hc2 current room temperature',
      c: '',
      x: true,
    },
    {
      v: 'off',
      u: 0,
      n: 'hc2 mode',
      c: 'hc2/mode',
      x: true,
    },
  ],
}

// Device Entities
const emsesp_deviceentities_1 = [
  {
    v: '(0)',
    n: 'error code',
    s: 'errorcode',
    x: false,
    i: 1,
  },
  {
    v: '14:54:39 06/06/2021',
    n: 'date/time',
    s: 'datetime',
    x: false,
    i: 2,
  },
  {
    v: 18.22,
    n: 'hc1 selected room temperature',
    s: 'hc1/seltemp',
    x: false,
    i: 3,
  },
  {
    v: 22.6,
    n: 'hc1 current room temperature',
    s: 'hc1/curtemp',
    x: false,
    i: 4,
  },
  {
    v: 'auto',
    n: 'hc1 mode',
    s: 'hc1/mode',
    x: false,
    i: 5,
  },
]

const emsesp_deviceentities_2 = [
  { n: 'heating active', s: 'heatingactive', x: false, i: 1 },
  { v: 'off', n: 'warm water active', s: 'wwactive', x: false, i: 2 },
  { v: 5, n: 'selected flow temperature', s: 'selflowtemp', x: false, i: 3 },
  { v: 0, n: 'burner selected max power', s: 'selburnpow', x: false, i: 4 },
  { v: 0, n: 'heating pump modulation', s: 'heatingpumpmod', x: false, i: 5 },
  { v: 42.9, n: 'current flow temperature', s: 'curflowtemp', x: false, i: 6 },
  { v: 41.8, n: 'return temperature', s: 'returntemp', x: false, i: 7 },
  { v: 1.6, n: 'system pressure', s: 'systempressure', x: false, i: 8 },
  { v: 45, n: 'actual boiler temperature', s: 'actboilertemp', x: false, i: 9 },
  { v: 'off', n: 'gas', s: 'gas', x: false, i: 10 },
  { v: 0, n: 'flame current', s: 'flamecur', x: false, i: 11 },
  { v: 'off', n: 'heating pump', s: 'heatingpump', x: false, i: 12 },
  { v: 'off', n: 'fan', s: 'fan', x: false, i: 13 },
  { v: 'off', n: 'ignition', s: 'ignition', x: false, i: 14 },
  {
    v: 'on',
    n: 'heating activated',
    s: 'heatingactivated',
    x: false,
    i: 15,
  },
  { v: 75, n: 'heating temperature', s: 'heatingtemp', x: false, i: 16 },
  { v: 90, n: 'burner pump max power', s: 'pumpmodmax', x: false, i: 17 },
  { v: 55, n: 'burner pump min power', s: 'pumpmodmin', x: false, i: 18 },
  { v: 1, n: 'pump delay', s: 'pumpdelay', x: false, i: 19 },
  { v: 10, n: 'burner min period', s: 'burnminperiod', x: false, i: 20 },
  { v: 0, n: 'burner min power', s: 'burnminpower', x: false, i: 21 },
  { v: 77, n: 'burner max power', s: 'burnmaxpower', x: false, i: 22 },
  { v: -6, n: 'hysteresis on temperature', s: 'boilhyston', x: false, i: 23 },
  { v: 6, n: 'hysteresis off temperature', s: 'boilhystoff', x: false, i: 24 },
  { v: 0, n: 'burner current power', s: 'burncurpower', x: false, i: 25 },
  { v: 317694, n: 'burner starts', s: 'burnstarts', x: false, i: 26 },
  { v: 524115, n: 'total burner operating time', s: 'burnoptime', x: false, i: 27 },
  { v: 424286, n: 'total heat operating time', s: 'heatoptime', x: false, i: 28 },
  { v: 4571225, n: 'total UBA operating time', s: 'ubaoptime', x: false, i: 29 },
  { v: '1C(210) 06.06.2020 12:07', n: 'last error code', s: 'lasterror', x: false, i: 30 },
  { v: '0H', n: 'service code', s: 'servicecode', x: false, i: 31 },
  { v: 203, n: 'service code number', s: 'servicecodenumber', x: false, i: 32 },
  { v: ' ', n: 'maintenance message', s: 'maintenancemessage', x: false, i: 33 },
  {
    v: 'off',
    n: 'maintenance scheduled',
    s: 'maintenance',
    x: false,
    i: 34,
  },
  { v: 6000, n: 'maintenance set time', s: 'maintenancetime', x: false, i: 35 },
  { v: '01.01.2012', n: 'maintenance set date', s: 'maintenancedate', x: false, i: 36 },
  { v: 60, n: 'ww selected temperature', s: 'wwseltemp', x: false, i: 37 },
  { v: 62, n: 'ww set temperature', s: 'wwsettemp', x: false, i: 38 },
  { v: 'flow', n: 'ww type', s: 'wwtype', x: false, i: 39 },
  {
    v: 'eco',
    n: 'ww comfort',
    s: 'wwcomfort',
    x: false,
    i: 40,
  },
  { v: 40, n: 'ww flow temperature offset', s: 'wwflowtempoffset', x: false, i: 41 },
  { v: 100, n: 'ww max power', s: 'wwmaxpower', x: false, i: 42 },
  {
    v: 'off',
    n: 'ww circulation pump available',
    s: 'wwcircpump',
    x: false,
    i: 43,
  },
  { v: '3-way valve', n: 'ww charging type', s: 'wwchargingtype', x: false, i: 44 },
  { v: -5, n: 'ww hysteresis on temperature', s: 'wwhyston', x: false, i: 45 },
  { v: 0, n: 'ww hysteresis off temperature', s: 'wwhystoff', x: false, i: 46 },
  { v: 70, n: 'ww disinfection temperature', s: 'wwdisinfectiontemp', x: false, i: 47 },
  {
    v: 'off',
    n: 'ww circulation pump frequency',
    s: 'wwcircmode',
    x: false,
    i: 48,
  },
  {
    v: 'off',
    n: 'ww circulation active',
    s: 'wwcirc',
    x: false,
    i: 49,
  },
  { v: 37.1, n: 'ww current intern temperature', s: 'wwcurtemp', x: false, i: 50 },
  { v: 0, n: 'ww current tap water flow', s: 'wwtapwaterflow', x: false, i: 51 },
  { v: 37.2, n: 'ww storage intern temperature', s: 'wwstorageintemp', x: false, i: 52 },
  { v: 'on', n: 'ww activated', s: 'wwactivated', x: false, i: 53 },
  {
    v: 'off',
    n: 'ww one time charging',
    s: 'wwonetime',
    x: false,
    i: 54,
  },
  {
    v: 'off',
    n: 'ww disinfection',
    s: 'wwdisinfect',
    x: false,
    i: 55,
  },
  { v: 'off', n: 'ww charging', s: 'wwcharging', x: false, i: 56 },
  { v: 'off', n: 'ww recharging', s: 'wwrecharging', x: false, i: 57 },
  { v: 'on', n: 'ww temperature ok', s: 'wwtempok', x: false, i: 58 },
  { v: 'off', n: 'ww active', s: 'wwactive', x: false, i: 59 },
  { v: 'on', n: 'ww heating', s: 'wwheating', x: false, i: 60 },
  { v: 282323, n: 'ww starts', s: 'wwstarts', x: false, i: 61 },
  { v: 99829, n: 'ww active time', s: 'wwactivetime', x: false, i: 62 },
]

const emsesp_deviceentities_4 = [
  {
    v: 16,
    n: 'hc2 selected room temperature',
    s: 'hc2/seltemp',
    x: false,
    i: 1,
  },
  {
    n: 'hc2 current room temperature',
    s: 'hc2/curtemp',
    x: true,
    i: 2,
  },
  {
    v: 'off',
    n: 'hc2 mode',
    s: 'hc2/mode',
    x: true,
    i: 3,
  },
]

// LOG
rest_server.get(FETCH_LOG_ENDPOINT, (req, res) => {
  const encoded = msgpack.encode(fetch_log)
  res.write(encoded, 'binary')
  res.end(null, 'binary')
})
rest_server.get(LOG_SETTINGS_ENDPOINT, (req, res) => {
  res.json(log_settings)
})
rest_server.post(LOG_SETTINGS_ENDPOINT, (req, res) => {
  log_settings = req.body
  console.log(JSON.stringify(log_settings))
  res.json(log_settings)
})

// NETWORK
rest_server.get(NETWORK_STATUS_ENDPOINT, (req, res) => {
  res.json(network_status)
})
rest_server.get(NETWORK_SETTINGS_ENDPOINT, (req, res) => {
  res.json(network_settings)
})
rest_server.post(NETWORK_SETTINGS_ENDPOINT, (req, res) => {
  network_settings = req.body
  console.log(JSON.stringify(network_settings))
  res.json(network_settings)
})
rest_server.get(LIST_NETWORKS_ENDPOINT, (req, res) => {
  res.json(list_networks)
})
rest_server.get(SCAN_NETWORKS_ENDPOINT, (req, res) => {
  res.sendStatus(202)
})

// AP
rest_server.get(AP_SETTINGS_ENDPOINT, (req, res) => {
  res.json(ap_settings)
})
rest_server.get(AP_STATUS_ENDPOINT, (req, res) => {
  res.json(ap_status)
})
rest_server.post(AP_SETTINGS_ENDPOINT, (req, res) => {
  ap_status = req.body
  console.log(JSON.stringify(ap_settings))
  res.json(ap_settings)
})

// OTA
rest_server.get(OTA_SETTINGS_ENDPOINT, (req, res) => {
  res.json(ota_settings)
})
rest_server.post(OTA_SETTINGS_ENDPOINT, (req, res) => {
  ota_settings = req.body
  console.log(JSON.stringify(ota_settings))
  res.json(ota_settings)
})

// MQTT
rest_server.get(MQTT_SETTINGS_ENDPOINT, (req, res) => {
  res.json(mqtt_settings)
})
rest_server.post(MQTT_SETTINGS_ENDPOINT, (req, res) => {
  mqtt_settings = req.body
  console.log(JSON.stringify(mqtt_settings))
  res.json(mqtt_settings)
})
rest_server.get(MQTT_STATUS_ENDPOINT, (req, res) => {
  res.json(mqtt_status)
})

// NTP
rest_server.get(NTP_SETTINGS_ENDPOINT, (req, res) => {
  res.json(ntp_settings)
})
rest_server.post(NTP_SETTINGS_ENDPOINT, (req, res) => {
  ntp_settings = req.body
  console.log(JSON.stringify(ntp_settings))
  res.json(ntp_settings)
})
rest_server.get(NTP_STATUS_ENDPOINT, (req, res) => {
  res.json(ntp_status)
})
rest_server.post(TIME_ENDPOINT, (req, res) => {
  res.sendStatus(200)
})

// SYSTEM
rest_server.get(SYSTEM_STATUS_ENDPOINT, (req, res) => {
  res.json(system_status)
})
rest_server.get(SECURITY_SETTINGS_ENDPOINT, (req, res) => {
  res.json(security_settings)
})
rest_server.post(SECURITY_SETTINGS_ENDPOINT, (req, res) => {
  security_settings = req.body
  console.log(JSON.stringify(security_settings))
  res.json(security_settings)
})
rest_server.get(FEATURES_ENDPOINT, (req, res) => {
  res.json(features)
})
rest_server.get(VERIFY_AUTHORIZATION_ENDPOINT, (req, res) => {
  res.json(verify_authentication)
})
rest_server.post(RESTART_ENDPOINT, (req, res) => {
  res.sendStatus(200)
})
rest_server.post(FACTORY_RESET_ENDPOINT, (req, res) => {
  res.sendStatus(200)
})
rest_server.post(UPLOAD_FIRMWARE_ENDPOINT, (req, res) => {
  res.sendStatus(200)
})
rest_server.post(SIGN_IN_ENDPOINT, (req, res) => {
  res.json(signin)
})
rest_server.get(GENERATE_TOKEN_ENDPOINT, (req, res) => {
  res.json(generate_token)
})

// EMS-ESP Project stuff
rest_server.post(EMSESP_RESET_CUSTOMIZATIONS_ENDPOINT, (req, res) => {
  console.log('Removing all customizations...')
  res.sendStatus(200)
})
rest_server.get(EMSESP_SETTINGS_ENDPOINT, (req, res) => {
  console.log('Get settings: ' + JSON.stringify(settings))
  res.json(settings)
})
rest_server.post(EMSESP_SETTINGS_ENDPOINT, (req, res) => {
  settings = req.body
  console.log('Write settings: ' + JSON.stringify(settings))
  res.status(202).json(settings) // restart needed
  // res.status(200).json(settings); // no restart needed
})
rest_server.get(EMSESP_CORE_DATA_ENDPOINT, (req, res) => {
  res.json(emsesp_coredata)
})
rest_server.get(EMSESP_SENSOR_DATA_ENDPOINT, (req, res) => {
  res.json(emsesp_sensordata)
})
rest_server.get(EMSESP_DEVICES_ENDPOINT, (req, res) => {
  res.json(emsesp_devices)
})
rest_server.post(EMSESP_SCANDEVICES_ENDPOINT, (req, res) => {
  console.log('Scan devices...')
  res.sendStatus(200)
})
rest_server.get(EMSESP_STATUS_ENDPOINT, (req, res) => {
  res.json(status)
})
rest_server.post(EMSESP_DEVICEDATA_ENDPOINT, (req, res) => {
  const id = req.body.id
  if (id === 1) {
    const encoded = msgpack.encode(emsesp_devicedata_1)
    res.write(encoded, 'binary')
    res.end(null, 'binary')
  }
  if (id === 2) {
    const encoded = msgpack.encode(emsesp_devicedata_2)
    res.write(encoded, 'binary')
    res.end(null, 'binary')
  }
  if (id === 3) {
    const encoded = msgpack.encode(emsesp_devicedata_3)
    res.write(encoded, 'binary')
    res.end(null, 'binary')
  }
  if (id === 4) {
    const encoded = msgpack.encode(emsesp_devicedata_4)
    res.write(encoded, 'binary')
    res.end(null, 'binary')
  }
})

rest_server.post(EMSESP_DEVICEENTITIES_ENDPOINT, (req, res) => {
  const id = req.body.id
  if (id === 1) {
    const encoded = msgpack.encode(emsesp_deviceentities_1)
    res.write(encoded, 'binary')
    res.end(null, 'binary')
  }
  if (id === 2) {
    const encoded = msgpack.encode(emsesp_deviceentities_2)
    res.write(encoded, 'binary')
    res.end(null, 'binary')
  }
  if (id === 4) {
    const encoded = msgpack.encode(emsesp_deviceentities_4)
    res.write(encoded, 'binary')
    res.end(null, 'binary')
  }
})

rest_server.post(EMSESP_EXCLUDE_ENTITIES_ENDPOINT, (req, res) => {
  console.log('exclude list for productid ' + req.body.product_id + ' device_id ' + req.body.device_id + ' entities:')
  console.log(req.body.entity_ids)
  res.sendStatus(200)
})

rest_server.post(EMSESP_WRITE_VALUE_ENDPOINT, (req, res) => {
  const devicevalue = req.body.devicevalue
  const id = req.body.id
  if (id === 1) {
    console.log('Write device value for Thermostat: ' + JSON.stringify(devicevalue))
    objIndex = emsesp_devicedata_1.data.findIndex((obj) => obj.c == devicevalue.c)
    emsesp_devicedata_1.data[objIndex] = devicevalue
  }
  if (id === 2) {
    console.log('Write device value for Boiler: ' + JSON.stringify(devicevalue))
    objIndex = emsesp_devicedata_2.data.findIndex((obj) => obj.c == devicevalue.c)
    emsesp_devicedata_2.data[objIndex] = devicevalue
  }
  if (id === 4) {
    console.log('Write device value for Thermostat2: ' + JSON.stringify(devicevalue))
    objIndex = emsesp_devicedata_4.data.findIndex((obj) => obj.c == devicevalue.c)
    emsesp_devicedata_4.data[objIndex] = devicevalue
  }

  res.sendStatus(200)
})

rest_server.post(EMSESP_WRITE_SENSOR_ENDPOINT, (req, res) => {
  const sensor = req.body
  console.log('Write sensor: ' + JSON.stringify(sensor))
  objIndex = emsesp_sensordata.sensors.findIndex((obj) => obj.is == sensor.id_str)
  emsesp_sensordata.sensors[objIndex].n = sensor.name
  emsesp_sensordata.sensors[objIndex].o = sensor.offset
  res.sendStatus(200)
})

rest_server.post(EMSESP_WRITE_ANALOG_ENDPOINT, (req, res) => {
  const analog = req.body
  console.log('Write analog: ' + JSON.stringify(analog))
  objIndex = emsesp_sensordata.analogs.findIndex((obj) => obj.i == analog.id)

  if (objIndex === -1) {
    console.log('new analog')
    emsesp_sensordata.analogs.push({
      i: analog.id,
      n: analog.name,
      f: analog.factor,
      o: analog.offset,
      u: analog.uom,
      t: analog.type,
    })
  } else {
    if (analog.type === -1) {
      console.log('removing analog ' + analog.id)
      emsesp_sensordata.analogs[objIndex].t = -1
    } else {
      emsesp_sensordata.analogs[objIndex].n = analog.name
      emsesp_sensordata.analogs[objIndex].o = analog.offset
      emsesp_sensordata.analogs[objIndex].f = analog.factor
      emsesp_sensordata.analogs[objIndex].u = analog.uom
      emsesp_sensordata.analogs[objIndex].t = analog.type
    }
  }

  res.sendStatus(200)
})

rest_server.post(EMSESP_BOARDPROFILE_ENDPOINT, (req, res) => {
  const board_profile = req.body.board_profile

  const data = {
    led_gpio: settings.led_gpio,
    dallas_gpio: settings.dallas_gpio,
    rx_gpio: settings.rx_gpio,
    tx_gpio: settings.tx_gpio,
    pbutton_gpio: settings.pbutton_gpio,
    phy_type: settings.phy_type,
  }

  if (board_profile == 'S32') {
    // BBQKees Gateway S32
    data.led_gpio = 2
    data.dallas_gpio = 18
    data.rx_gpio = 23
    data.tx_gpio = 5
    data.pbutton_gpio = 0
    data.phy_type = 0
  } else if (board_profile == 'E32') {
    // BBQKees Gateway E32
    data.led_gpio = 2
    data.dallas_gpio = 4
    data.rx_gpio = 5
    data.tx_gpio = 17
    data.pbutton_gpio = 33
    data.phy_type = 1
  } else if (board_profile == 'MH-ET') {
    // MH-ET Live D1 Mini
    data.led_gpio = 2
    data.dallas_gpio = 18
    data.rx_gpio = 23
    data.tx_gpio = 5
    data.pbutton_gpio = 0
    data.phy_type = 0
  } else if (board_profile == 'NODEMCU') {
    // NodeMCU 32S
    data.led_gpio = 2
    data.dallas_gpio = 18
    data.rx_gpio = 23
    data.tx_gpio = 5
    data.pbutton_gpio = 0
    data.phy_type = 0
  } else if (board_profile == 'LOLIN') {
    // Lolin D32
    data.led_gpio = 2
    data.dallas_gpio = 18
    data.rx_gpio = 17
    data.tx_gpio = 16
    data.pbutton_gpio = 0
    data.phy_type = 0
  } else if (board_profile == 'OLIMEX') {
    // Olimex ESP32-EVB (uses U1TXD/U1RXD/BUTTON, no LED or Dallas)
    data.led_gpio = 0
    data.dallas_gpio = 0
    data.rx_gpio = 36
    data.tx_gpio = 4
    data.pbutton_gpio = 34
    data.phy_type = 1
  }

  console.log('boardProfile POST. Sending back, profile: ' + board_profile + ', ' + 'data: ' + JSON.stringify(data))

  res.send(data)
})

// EMS-ESP API specific
const emsesp_info = {
  System: {
    version: '3.x.x',
    uptime: '001+06:40:34.018',
    'uptime (seconds)': 110434,
    freemem: 131,
    'reset reason': 'Software reset CPU / Software reset CPU',
    'Dallas sensors': 3,
  },
  Network: {
    connection: 'Wired',
    hostname: 'ems-esp',
    MAC: 'A8:03:2A:62:64:CF',
    'IPv4 address': '192.168.1.134/255.255.255.0',
    'IPv4 gateway': '192.168.1.1',
    'IPv4 nameserver': '192.168.1.1',
  },
  Status: {
    'bus status': 'connected',
    'bus protocol': 'Buderus',
    'telegrams received': 84986,
    'read requests sent': 14748,
    'write requests sent': 3,
    'incomplete telegrams': 8,
    'tx fails': 0,
    'rx line quality': 100,
    'tx line quality': 100,
    MQTT: 'connected',
    'MQTT publishes': 46336,
    'MQTT publish fails': 0,
    'Dallas reads': 22086,
    'Dallas fails': 0,
  },
  Devices: [
    {
      type: 'Boiler',
      name: 'Nefit GBx72/Trendline/Cerapur/Greenstar Si/27i (DeviceID:0x08 ProductID:123, Version:06.01)',
      handlers:
        '0x10 0x11 0xC2 0x14 0x15 0x1C 0x18 0x19 0x1A 0x35 0x16 0x33 0x34 0x26 0x2A 0xD1 0xE3 0xE4 0xE5 0xE6 0xE9 0xEA',
    },
    {
      type: 'Thermostat',
      name: 'RC20/Moduline 300 (DeviceID:0x17, ProductID:77, Version:03.03)',
      handlers: '0xA3 0x06 0xA2 0x12 0x91 0xA8',
    },
    {
      type: 'Controller',
      name: 'BC10 (DeviceID:0x09, ProductID:190, Version:01.03)',
    },
  ],
}

rest_server.post(API_ENDPOINT_ROOT, (req, res) => {
  console.log('Generic API POST')
  console.log(req.body)
  if (req.body.device === 'system') {
    if (req.body.entity === 'info') {
      console.log('sending system info: ' + JSON.stringify(emsesp_info))
    } else if (req.body.entity === 'settings') {
      console.log('sending system settings: ' + JSON.stringify(settings))
      res.json(settings)
    } else {
      res.sendStatus(200)
    }
  } else {
    res.sendStatus(200)
  }
})
rest_server.get(API_ENDPOINT_ROOT, (req, res) => {
  console.log('Generic API GET')
  res.sendStatus(200)
})

const SYSTEM_INFO_ENDPOINT = API_ENDPOINT_ROOT + 'system/info'
rest_server.post(SYSTEM_INFO_ENDPOINT, (req, res) => {
  console.log('System Info POST: ' + JSON.stringify(req.body))
  res.sendStatus(200)
})
rest_server.get(SYSTEM_INFO_ENDPOINT, (req, res) => {
  console.log('System Info GET')
  res.json(emsesp_info)
})

const SYSTEM_SETTINGS_ENDPOINT = API_ENDPOINT_ROOT + 'system/settings'
rest_server.post(SYSTEM_SETTINGS_ENDPOINT, (req, res) => {
  console.log('System Settings POST: ' + JSON.stringify(req.body))
  res.sendStatus(200)
})
rest_server.get(SYSTEM_SETTINGS_ENDPOINT, (req, res) => {
  console.log('System Settings GET')
  res.json(settings)
})

// start server
const expressServer = rest_server.listen(port, () =>
  console.log(`Mock server for EMS-ESP is up and running at http://localhost:${port}`),
)
console.log(`EMS-ESP Rest API listening to http://localhost:${port}/api`)

// start websocket server
const websocketServer = new WebSocket.Server({
  noServer: true,
  path: '/ws',
})
console.log('WebSocket server is listening to /ws')

expressServer.on('upgrade', (request, socket, head) => {
  websocketServer.handleUpgrade(request, socket, head, (websocket) => {
    websocketServer.emit('connection', websocket, request)
  })
})

websocketServer.on('connection', function connection(websocketConnection, connectionRequest) {
  const [_path, params] = connectionRequest?.url?.split('?')
  console.log(params)

  websocketConnection.on('message', (message) => {
    const parsedMessage = JSON.parse(message)
    console.log(parsedMessage)
  })
})

var count = 8
var log_index = 0
const ES_ENDPOINT_ROOT = '/es/'
const ES_LOG_ENDPOINT = ES_ENDPOINT_ROOT + 'log'
rest_server.get(ES_LOG_ENDPOINT, function (req, res) {
  res.setHeader('Content-Type', 'text/event-stream')
  res.setHeader('Cache-Control', 'no-cache')
  res.setHeader('Access-Control-Allow-Origin', '*')
  res.setHeader('Connection', 'keep-alive')
  // res.setHeader('Content-Encoding', 'deflate')
  // res.setHeader('X-Accel-Buffering', 'no')
  res.flushHeaders()

  var timer = setInterval(function () {
    count += 1
    log_index += 1
    const data = {
      t: '000+00:00:00.000',
      l: 3, // error
      i: count,
      n: 'system',
      m: 'incoming message #' + count + '/' + log_index,
    }
    const sseFormattedResponse = `data: ${JSON.stringify(data)}\n\n`
    // console.log('sending log #' + count)
    res.write(sseFormattedResponse)
    res.flush() // this is important

    // if buffer full start over
    if (log_index > 50) {
      fetch_log.events = []
      log_index = 0
    }
    fetch_log.events.push(data) // append to buffer
  }, 1000)
})
