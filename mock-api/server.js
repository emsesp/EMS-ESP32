const express = require('express');
const compression = require('compression');
const path = require('path');
const msgpack = require('@msgpack/msgpack');

// REST API
const rest_server = express();
const port = 3080;

rest_server.use(compression());
rest_server.use(express.static(path.join(__dirname, '../interface/build')));
rest_server.use(express.json());

// endpoints
const API_ENDPOINT_ROOT = '/api/';
const REST_ENDPOINT_ROOT = '/rest/';

// LOG
const LOG_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'logSettings';
log_settings = {
  level: 6,
  max_messages: 50,
  compact: false
};

const FETCH_LOG_ENDPOINT = REST_ENDPOINT_ROOT + 'fetchLog';
const fetch_log = {
  events: [
    {
      t: '000+00:00:00.001',
      l: 3,
      i: 1,
      n: 'system',
      m: 'this is message 3'
    },
    {
      t: '000+00:00:00.002',
      l: 4,
      i: 2,
      n: 'ntp',
      m: 'this is message 4'
    },
    {
      t: '000+00:00:00.002',
      l: 5,
      i: 3,
      n: 'mqtt',
      m: 'this is message 5'
    },
    {
      t: '000+00:00:00.002',
      l: 6,
      i: 444,
      n: 'command',
      m: 'this is message 6'
    },
    {
      t: '000+00:00:00.002',
      l: 7,
      i: 5555,
      n: 'emsesp',
      m: 'this is message 7'
    },
    {
      t: '000+00:00:00.002',
      l: 8,
      i: 666666,
      n: 'thermostat',
      m: 'this is message 8'
    }
  ]
};

// NTP
const NTP_STATUS_ENDPOINT = REST_ENDPOINT_ROOT + 'ntpStatus';
const NTP_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'ntpSettings';
const TIME_ENDPOINT = REST_ENDPOINT_ROOT + 'time';
ntp_settings = {
  enabled: true,
  server: 'time.google.com',
  tz_label: 'Europe/Amsterdam',
  tz_format: 'CET-1CEST,M3.5.0,M10.5.0/3'
};
const ntp_status = {
  status: 1,
  utc_time: '2021-04-01T14:25:42Z',
  local_time: '2021-04-01T16:25:42',
  server: 'time.google.com',
  uptime: 856
};

// AP
const AP_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'apSettings';
const AP_STATUS_ENDPOINT = REST_ENDPOINT_ROOT + 'apStatus';
ap_settings = {
  provision_mode: 1,
  ssid: 'ems-esp',
  password: 'ems-esp-neo',
  local_ip: '192.168.4.1',
  gateway_ip: '192.168.4.1',
  subnet_mask: '255.255.255.0',
  channel: 1,
  ssid_hidden: true,
  max_clients: 4
};
ap_status = {
  status: 1,
  ip_address: '192.168.4.1',
  mac_address: '3C:61:05:03:AB:2D',
  station_num: 0
};

// NETWORK
const NETWORK_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'networkSettings';
const NETWORK_STATUS_ENDPOINT = REST_ENDPOINT_ROOT + 'networkStatus';
const SCAN_NETWORKS_ENDPOINT = REST_ENDPOINT_ROOT + 'scanNetworks';
const LIST_NETWORKS_ENDPOINT = REST_ENDPOINT_ROOT + 'listNetworks';
network_settings = {
  ssid: 'myWifi',
  password: 'myPassword',
  hostname: 'ems-esp',
  nosleep: true,
  tx_power: 20,
  bandwidth20: false,
  static_ip_config: false,
  enableMDNS: true,
  enableCORS: false,
  CORSOrigin: '*',
  enableIPv6: false,
  local_ip: '',
  gateway_ip: '',
  subnet_mask: '',
  dns_ip_1: '',
  dns_ip_2: ''
};
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
  dns_ip_2: '0.0.0.0'
};
const list_networks = {
  networks: [
    {
      rssi: -40,
      ssid: '',
      bssid: 'FC:EC:DA:FD:B4:68',
      channel: 11,
      encryption_type: 3
    },
    {
      rssi: -41,
      ssid: 'home',
      bssid: '02:EC:DA:FD:B4:68',
      channel: 11,
      encryption_type: 3
    },
    {
      rssi: -42,
      ssid: '',
      bssid: '06:EC:DA:FD:B4:68',
      channel: 11,
      encryption_type: 3
    },
    {
      rssi: -73,
      ssid: '',
      bssid: 'FC:EC:DA:17:D4:7E',
      channel: 1,
      encryption_type: 3
    },
    {
      rssi: -73,
      ssid: 'office',
      bssid: '02:EC:DA:17:D4:7E',
      channel: 1,
      encryption_type: 3
    },
    {
      rssi: -75,
      ssid: 'Erica',
      bssid: 'C8:D7:19:9A:88:BD',
      channel: 2,
      encryption_type: 3
    },
    {
      rssi: -75,
      ssid: '',
      bssid: 'C6:C9:E3:FF:A5:DE',
      channel: 2,
      encryption_type: 3
    },
    {
      rssi: -76,
      ssid: 'Bruin',
      bssid: 'C0:C9:E3:FF:A5:DE',
      channel: 2,
      encryption_type: 3
    }
  ]
};

// OTA
const OTA_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'otaSettings';
ota_settings = {
  enabled: true,
  port: 8266,
  password: 'ems-esp-neo'
};

// MQTT
const MQTT_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'mqttSettings';
const MQTT_STATUS_ENDPOINT = REST_ENDPOINT_ROOT + 'mqttStatus';
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
  entity_format: 1,
  publish_time_boiler: 10,
  publish_time_thermostat: 10,
  publish_time_solar: 10,
  publish_time_mixer: 10,
  publish_time_other: 10,
  publish_time_sensor: 10,
  publish_time_heartbeat: 60,
  mqtt_qos: 0,
  mqtt_retain: false,
  ha_enabled: true,
  nested_format: 1,
  discovery_type: 0,
  discovery_prefix: 'homeassistant',
  send_response: true,
  publish_single: false
};
const mqtt_status = {
  enabled: true,
  connected: true,
  client_id: 'ems-esp',
  disconnect_reason: 0,
  mqtt_fails: 0,
  mqtt_queued: 1,
  connect_count: 2
};

// SYSTEM
const FEATURES_ENDPOINT = REST_ENDPOINT_ROOT + 'features';
const VERIFY_AUTHORIZATION_ENDPOINT = REST_ENDPOINT_ROOT + 'verifyAuthorization';
const SYSTEM_STATUS_ENDPOINT = REST_ENDPOINT_ROOT + 'systemStatus';
const SECURITY_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'securitySettings';
const RESTART_ENDPOINT = REST_ENDPOINT_ROOT + 'restart';
const FACTORY_RESET_ENDPOINT = REST_ENDPOINT_ROOT + 'factoryReset';
const UPLOAD_FILE_ENDPOINT = REST_ENDPOINT_ROOT + 'uploadFile';
const SIGN_IN_ENDPOINT = REST_ENDPOINT_ROOT + 'signIn';
const GENERATE_TOKEN_ENDPOINT = REST_ENDPOINT_ROOT + 'generateToken';
const system_status = {
  emsesp_version: '3.6.0-demo',
  esp_platform: 'ESP32',
  max_alloc_heap: 89,
  psram_size: 0,
  free_psram: 0,
  cpu_freq_mhz: 240,
  free_heap: 143,
  sdk_version: 'v4.4.2',
  flash_chip_size: 4096,
  flash_chip_speed: 40000000,
  fs_used: 40,
  fs_free: 24,
  app_used: 1863,
  app_free: 121,
  uptime: '000+00:15:42.707'
};
security_settings = {
  jwt_secret: 'naughty!',
  users: [
    { username: 'admin', password: 'admin', admin: true },
    { username: 'guest', password: 'guest', admin: false }
  ]
};
const features = {
  project: true,
  security: true,
  mqtt: true,
  ntp: true,
  ota: true,
  upload_firmware: true
};
const verify_authentication = { access_token: '1234' };
const signin = {
  access_token:
    'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6ImFkbWluIiwiYWRtaW4iOnRydWUsInZlcnNpb24iOiIzLjAuMmIwIn0.MsHSgoJKI1lyYz77EiT5ZN3ECMrb4mPv9FNy3udq0TU'
};
const generate_token = { token: '1234' };

// EMS-ESP Project specific
const EMSESP_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'settings';
const EMSESP_CORE_DATA_ENDPOINT = REST_ENDPOINT_ROOT + 'coreData';
const EMSESP_SENSOR_DATA_ENDPOINT = REST_ENDPOINT_ROOT + 'sensorData';
const EMSESP_DEVICES_ENDPOINT = REST_ENDPOINT_ROOT + 'devices';
const EMSESP_SCANDEVICES_ENDPOINT = REST_ENDPOINT_ROOT + 'scanDevices';
const EMSESP_DEVICEDATA_ENDPOINT = REST_ENDPOINT_ROOT + 'deviceData';
const EMSESP_DEVICEENTITIES_ENDPOINT = REST_ENDPOINT_ROOT + 'deviceEntities';
const EMSESP_STATUS_ENDPOINT = REST_ENDPOINT_ROOT + 'status';
const EMSESP_BOARDPROFILE_ENDPOINT = REST_ENDPOINT_ROOT + 'boardProfile';
const EMSESP_WRITE_VALUE_ENDPOINT = REST_ENDPOINT_ROOT + 'writeDeviceValue';
const EMSESP_WRITE_SENSOR_ENDPOINT = REST_ENDPOINT_ROOT + 'writeTemperatureSensor';
const EMSESP_WRITE_ANALOG_ENDPOINT = REST_ENDPOINT_ROOT + 'writeAnalogSensor';
const EMSESP_CUSTOM_ENTITIES_ENDPOINT = REST_ENDPOINT_ROOT + 'customEntities';
const EMSESP_RESET_CUSTOMIZATIONS_ENDPOINT = REST_ENDPOINT_ROOT + 'resetCustomizations';
const EMSESP_WRITE_SCHEDULE_ENDPOINT = REST_ENDPOINT_ROOT + 'schedule';
const EMSESP_WRITE_ENTITIES_ENDPOINT = REST_ENDPOINT_ROOT + 'entities';

settings = {
  locale: 'en',
  tx_mode: 4,
  ems_bus_id: 11,
  syslog_enabled: false,
  syslog_level: 3,
  trace_raw: false,
  syslog_mark_interval: 0,
  syslog_host: '192.168.1.4',
  syslog_port: 514,
  shower_timer: true,
  shower_alert: true,
  shower_alert_trigger: 7,
  shower_alert_coldshot: 10,
  rx_gpio: 23,
  tx_gpio: 5,
  phy_type: 0,
  eth_power: 0,
  eth_phy_addr: 0,
  eth_clock_mode: 0,
  dallas_gpio: 3,
  dallas_parasite: false,
  led_gpio: 2,
  hide_led: false,
  notoken_api: false,
  readonly_mode: false,
  low_clock: false,
  telnet_enabled: true,
  analog_enabled: false,
  pbutton_gpio: 0,
  board_profile: 'S32',
  bool_format: 1,
  bool_dashboard: 1,
  enum_format: 1,
  fahrenheit: false
};

// this is used in customizations
const emsesp_devices = {
  devices: [
    {
      i: 2,
      s: 'Thermostat (RC20/Moduline 300)',
      t: 5,
      tn: 'thermostat'
    },
    {
      i: 7,
      s: 'Boiler (GBx72/Trendline/Cerapur/Greenstar Si/27i)',
      t: 4,
      tn: 'boiler'
    },
    {
      i: 4,
      s: 'Thermostat (RC100/Moduline 1000/1010)',
      t: 5,
      tn: 'thermostat'
    }
  ]
};

const emsesp_coredata = {
  connected: true,
  // devices: [],
  devices: [
    {
      id: 7,
      t: 4,
      tn: 'Boiler',
      b: 'Nefit',
      n: 'GBx72/Trendline/Cerapur/Greenstar Si/27i',
      d: 8,
      p: 123,
      v: '06.01'
    },
    {
      id: 3,
      t: 4,
      tn: 'Boiler',
      b: 'Buderus',
      n: 'GB125/GB135/MC10',
      d: 8,
      p: 123,
      v: '06.01'
    },
    {
      id: 1,
      t: 5,
      tn: 'Thermostat',
      b: 'Buderus',
      n: 'RC35',
      d: 24,
      p: 86,
      v: '04.01'
    },
    {
      id: 2,
      t: 5,
      tn: 'Thermostat',
      b: '',
      n: 'RC20/Moduline 300',
      d: 23,
      p: 77,
      v: '03.03'
    },
    {
      id: 4,
      t: 5,
      tn: 'Thermostat',
      b: 'Buderus',
      n: 'RC100/Moduline 1000/1010',
      d: 16,
      p: 165,
      v: '04.01'
    },
    {
      id: 5,
      t: 6,
      tn: 'Mixer Module',
      b: 'Buderus',
      n: 'MM10',
      d: 32,
      p: 69,
      v: '01.01'
    },
    {
      id: 6,
      t: 7,
      tn: 'Solar Module',
      b: 'Buderus',
      n: 'SM10',
      d: 48,
      p: 73,
      v: '01.02'
    },
    {
      id: 99,
      t: 17,
      tn: 'Custom',
      b: '',
      n: 'User defined entities',
      d: 1,
      p: 1,
      v: ''
    }
  ]
};

const emsesp_sensordata = {
  // ts: [],
  ts: [
    { id: '28-233D-9497-0C03', n: 'Dallas 1', t: 25.7, o: 1.2, u: 1 },
    { id: '28-243D-7437-1E3A', n: 'Dallas 2 outside', t: 26.1, o: 0, u: 1 },
    { id: '28-243E-7437-1E3B', n: 'Zolder', t: 27.1, o: 0, u: 16 },
    { id: '28-183D-1892-0C33', n: 'Roof', o: 2, u: 1 }
  ],
  // as: [],
  as: [
    { id: 1, g: 36, n: 'motor', v: 0, u: 0, o: 17, f: 0, t: 0, d: false },
    { id: 2, g: 37, n: 'External switch', v: 13, u: 0, o: 17, f: 0, t: 1, d: false },
    { id: 3, g: 39, n: 'Pulse count', v: 144, u: 0, o: 0, f: 0, t: 2, d: false },
    { id: 4, g: 40, n: 'Pressure', v: 16, u: 17, o: 0, f: 0, t: 3, d: false }
  ],
  analog_enabled: true
};

const status = {
  status: 0,
  // status: 2,
  tx_mode: 1,
  uptime: 77186,
  num_devices: 2,
  num_sensors: 1,
  num_analogs: 1,
  stats: [
    { id: 0, s: 56506, f: 11, q: 100 },
    { id: 1, s: 9026, f: 0, q: 100 },
    { id: 2, s: 33, f: 2, q: 95 },
    { id: 3, s: 56506, f: 11, q: 100 },
    { id: 4, s: 0, f: 0, q: 100 },
    { id: 5, s: 12, f: 10, q: 20 },
    { id: 6, s: 0, f: 0, q: 0 }
  ]
};

// Dashboard data
// 7 - Nefit Trendline boiler
// 1 - RC35 thermo
// 2 - RC20 thermo
// 3 - Buderus GB125 boiler
// 4 - RC100 themo
// 5 - Mixer MM10
// 6 - Solar SM10
// 99 - Custom

const emsesp_devicedata_7 = {
  data: [
    { v: '', u: 0, id: '08reset', c: 'reset', l: ['-', 'maintenance', 'error'] },
    { v: 'off', u: 0, id: '08heating active' },
    { v: 'off', u: 0, id: '04tapwater active' },
    { v: 5, u: 1, id: '04selected flow temperature', c: 'selflowtemp' },
    { v: 0, u: 3, id: '0Eburner selected max power', c: 'selburnpow' },
    { v: 0, u: 3, id: '00heating pump modulation' },
    { v: 53.4, u: 1, id: '00current flow temperature' },
    { v: 52.7, u: 1, id: '00return temperature' },
    { v: 1.3, u: 10, id: '00system pressure' },
    { v: 54.9, u: 1, id: '00actual boiler temperature' },
    { v: 'off', u: 0, id: '00gas' },
    { v: 'off', u: 0, id: '00gas stage 2' },
    { v: 0, u: 9, id: '00flame current' },
    { v: 'off', u: 0, id: '00heating pump' },
    { v: 'off', u: 0, id: '00fan' },
    { v: 'off', u: 0, id: '00ignition' },
    { v: 'off', u: 0, id: '00oil preheating' },
    { v: 'on', u: 0, id: '00heating activated', c: 'heatingactivated', l: ['off', 'on'] },
    { v: 80, u: 1, id: '00heating temperature', c: 'heatingtemp' },
    { v: 70, u: 3, id: '00burner pump max power', c: 'pumpmodmax' },
    { v: 30, u: 3, id: '00burner pump min power', c: 'pumpmodmin' },
    { v: 1, u: 8, id: '00pump delay', c: 'pumpdelay' },
    { v: 10, u: 8, id: '00burner min period', c: 'burnminperiod' },
    { v: 0, u: 3, id: '00burner min power', c: 'burnminpower' },
    { v: 50, u: 3, id: '00burner max power', c: 'burnmaxpower' },
    { v: -6, u: 2, id: '00hysteresis on temperature', c: 'boilhyston' },
    { v: 6, u: 2, id: '00hysteresis off temperature', c: 'boilhystoff' },
    { v: 0, u: 1, id: '00set flow temperature' },
    { v: 0, u: 3, id: '00burner set power' },
    { v: 0, u: 3, id: '00burner current power' },
    { v: 326323, u: 0, id: '00burner starts' },
    { v: 553437, u: 8, id: '00total burner operating time' },
    { v: 451286, u: 8, id: '00total heat operating time' },
    { v: 4672173, u: 8, id: '00total UBA operating time' },
    { v: '1C(210) 06.06.2020 12:07 (0 min)', u: 0, id: '00last error code' },
    { v: '0H', u: 0, id: '00service code' },
    { v: 203, u: 0, id: '00service code number' },
    { v: 'H00', u: 0, id: '00maintenance message' },
    { v: 'manual', u: 0, id: '00maintenance scheduled', c: 'maintenance', l: ['off', 'time', 'date', 'manual'] },
    { v: 6000, u: 7, id: '00time to next maintenance', c: 'maintenancetime' },
    { v: '01.01.2012', u: 0, id: '00next maintenance date', c: 'maintenancedate', o: 'Format: < dd.mm.yyyy >' },
    { v: 'on', u: 0, id: '00dhw turn on/off', c: 'wwtapactivated', l: ['off', 'on'] },
    { v: 62, u: 1, id: '00dhw set temperature' },
    { v: 60, u: 1, id: '00dhw selected temperature', c: 'wwseltemp' },
    { v: 'flow', u: 0, id: '00dhw type' },
    { v: 'hot', u: 0, id: '00dhw comfort', c: 'wwcomfort', l: ['hot', 'eco', 'intelligent'] },
    { v: 40, u: 2, id: '00dhw flow temperature offset', c: 'wwflowtempoffset' },
    { v: 100, u: 3, id: '00dhw max power', c: 'wwmaxpower' },
    { v: 'off', u: 0, id: '00dhw circulation pump available', c: 'wwcircpump', l: ['off', 'on'] },
    { v: '3-way valve', u: 0, id: '00dhw charging type' },
    { v: -5, u: 2, id: '00dhw hysteresis on temperature', c: 'wwhyston' },
    { v: 0, u: 2, id: '00dhw hysteresis off temperature', c: 'wwhystoff' },
    { v: 70, u: 1, id: '00dhw disinfection temperature', c: 'wwdisinfectiontemp' },
    {
      v: 'off',
      u: 0,
      id: '00dhw circulation pump mode',
      c: 'wwcircmode',
      l: ['off', '1x3min', '2x3min', '3x3min', '4x3min', '5x3min', '6x3min', 'continuous']
    },
    { v: 'off', u: 0, id: '00dhw circulation active', c: 'wwcirc', l: ['off', 'on'] },
    { v: 47.3, u: 1, id: '00dhw current intern temperature' },
    { v: 0, u: 4, id: '00dhw current tap water flow' },
    { v: 47.3, u: 1, id: '00dhw storage intern temperature' },
    { v: 'on', u: 0, id: '00dhw activated', c: 'wwactivated', l: ['off', 'on'] },
    { v: 'off', u: 0, id: '00dhw one time charging', c: 'wwonetime', l: ['off', 'on'] },
    { v: 'off', u: 0, id: '00dhw disinfecting', c: 'wwdisinfecting', l: ['off', 'on'] },
    { v: 'off', u: 0, id: '00dhw charging' },
    { v: 'off', u: 0, id: '00dhw recharging' },
    { v: 'on', u: 0, id: '00dhw temperature ok' },
    { v: 'off', u: 0, id: '00dhw active' },
    { v: 'on', u: 0, id: '00dhw 3way valve active' },
    { v: 0, u: 3, id: '00dhw set pump power' },
    { v: 288768, u: 0, id: '00dhw starts' },
    { v: 102151, u: 8, id: '00dhw active time' }
  ]
};

const emsesp_devicedata_1 = {
  data: [
    {
      v: '22(816) 01.05.2023 13:07 (1 min)',
      u: 0,
      id: '00last error code'
    },
    {
      v: '05.05.2023 09:44',
      u: 0,
      id: '00date/time',
      c: 'datetime',
      h: '< NTP | dd.mm.yyyy-hh:mm:ss-day(0-6)-dst(0/1) >'
    },
    {
      v: -2.4,
      u: 2,
      id: '00internal temperature offset',
      c: 'intoffset',
      m: -5,
      x: 5,
      s: '0.1'
    },
    {
      v: -11,
      u: 1,
      id: '00minimal external temperature',
      c: 'minexttemp',
      m: -30,
      x: 0,
      s: '1'
    },
    {
      v: 29.5,
      u: 1,
      id: '00temperature sensor 1'
    },
    {
      v: 32.5,
      u: 1,
      id: '00temperature sensor 2'
    },
    {
      v: 'on',
      u: 0,
      id: '00damping outdoor temperature',
      c: 'damping',
      l: ['off', 'on']
    },
    {
      v: 13,
      u: 1,
      id: '00damped outdoor temperature'
    },
    {
      v: 'medium',
      u: 0,
      id: '00building type',
      c: 'building',
      l: ['light', 'medium', 'heavy']
    },
    {
      v: 'auto',
      u: 0,
      id: '00dhw mode',
      c: 'wwmode',
      l: ['off', 'on', 'auto']
    },
    {
      v: 'off',
      u: 0,
      id: '00dhw circulation pump mode',
      c: 'wwcircmode',
      l: ['off', 'on', 'auto']
    },
    {
      v: 'std prog',
      u: 0,
      id: '00dhw program',
      c: 'wwprogmode',
      l: ['std prog', 'own prog']
    },
    {
      v: 'std prog',
      u: 0,
      id: '00dhw circulation program',
      c: 'wwcircprog',
      l: ['std prog', 'own prog']
    },
    {
      v: 'off',
      u: 0,
      id: '00dhw disinfecting',
      c: 'wwdisinfecting',
      l: ['off', 'on']
    },
    {
      v: 'tu',
      u: 0,
      id: '00dhw disinfection day',
      c: 'wwdisinfectday',
      l: ['mo', 'tu', 'we', 'th', 'fr', 'sa', 'su', 'all']
    },
    {
      v: 1,
      u: 0,
      id: '00dhw disinfection hour',
      c: 'wwdisinfecthour',
      m: 0,
      x: 23,
      s: '1'
    },
    {
      v: 60,
      u: 1,
      id: '00dhw maximum temperature',
      c: 'wwmaxtemp',
      m: 60,
      x: 80,
      s: '1'
    },
    {
      v: 'on',
      u: 0,
      id: '00dhw one time key function',
      c: 'wwonetimekey',
      l: ['off', 'on']
    },
    {
      v: '00 mo 06:00 on',
      u: 0,
      id: '00dhw program switchtime',
      c: 'wwswitchtime',
      h: '<nn> [ not_set | day hh:mm on|off ]'
    },
    {
      v: '00 mo 06:30 on',
      u: 0,
      id: '00dhw circulation program switchtime',
      c: 'wwcircswitchtime',
      h: '<nn> [ not_set | day hh:mm on|off ]'
    },
    {
      v: '01.01.2000-01.01.2000',
      u: 0,
      id: '00dhw holiday dates',
      c: 'wwholidays',
      h: '< dd.mm.yyyy-dd.mm.yyyy >'
    },
    {
      v: '01.01.2019-12.01.2019',
      u: 0,
      id: '00dhw vacation dates',
      c: 'wwvacations',
      h: '< dd.mm.yyyy-dd.mm.yyyy >'
    },
    {
      v: 21,
      u: 1,
      id: '00hc2 selected room temperature',
      c: 'hc2/seltemp',
      m: 0,
      x: 30,
      s: '0.5'
    },
    {
      v: 'auto',
      u: 0,
      id: '00hc2 mode',
      c: 'hc2/mode',
      l: ['night', 'day', 'auto']
    },
    {
      v: 'day',
      u: 0,
      id: '00hc2 mode type'
    },
    {
      v: 21,
      u: 1,
      id: '00hc2 day temperature',
      c: 'hc2/daytemp',
      m: 5,
      x: 30,
      s: '0.5'
    },
    {
      v: 17,
      u: 1,
      id: '00hc2 night temperature',
      c: 'hc2/nighttemp',
      m: 5,
      x: 30,
      s: '0.5'
    },
    {
      v: 58,
      u: 1,
      id: '00hc2 design temperature',
      c: 'hc2/designtemp',
      m: 30,
      x: 90,
      s: '1'
    },
    {
      v: 0,
      u: 2,
      id: '00hc2 offset temperature',
      c: 'hc2/offsettemp',
      m: -5,
      x: 5,
      s: '0.5'
    },
    {
      v: 15,
      u: 1,
      id: '00hc2 holiday temperature',
      c: 'hc2/holidaytemp',
      m: 5,
      x: 30,
      s: '0.5'
    },
    {
      v: 34,
      u: 1,
      id: '00hc2 target flow temperature'
    },
    {
      v: 17,
      u: 1,
      id: '00hc2 summer temperature',
      c: 'hc2/summertemp',
      m: 9,
      x: 25,
      s: '1'
    },
    {
      v: 'winter',
      u: 0,
      id: '00hc2 summer mode'
    },
    {
      v: 'off',
      u: 0,
      id: '00hc2 holiday mode'
    },
    {
      v: -10,
      u: 1,
      id: '00hc2 nofrost temperature',
      c: 'hc2/nofrosttemp',
      m: -20,
      x: 10,
      s: '1'
    },
    {
      v: 'outdoor',
      u: 0,
      id: '00hc2 nofrost mode',
      c: 'hc2/nofrostmode',
      l: ['off', 'outdoor', 'room']
    },
    {
      v: 0,
      u: 2,
      id: '00hc2 room influence',
      c: 'hc2/roominfluence',
      m: 0,
      x: 10,
      s: '1'
    },
    {
      v: 15,
      u: 1,
      id: '00hc2 min flow temperature',
      c: 'hc2/minflowtemp',
      m: 5,
      x: 70,
      s: '1'
    },
    {
      v: 85,
      u: 1,
      id: '00hc2 max flow temperature',
      c: 'hc2/maxflowtemp',
      m: 30,
      x: 90,
      s: '1'
    },
    {
      v: 0,
      u: 2,
      id: '00hc2 flow temperature offset for mixer',
      c: 'hc2/flowtempoffset',
      m: 0,
      x: 20,
      s: '1'
    },
    {
      v: 'radiator',
      u: 0,
      id: '00hc2 heating type',
      c: 'hc2/heatingtype',
      l: ['off', 'radiator', 'convector', 'floor']
    },
    {
      v: 'outdoor',
      u: 0,
      id: '00hc2 reduce mode',
      c: 'hc2/reducemode',
      l: ['nofrost', 'reduce', 'room', 'outdoor']
    },
    {
      v: 'outdoor',
      u: 0,
      id: '00hc2 control mode',
      c: 'hc2/controlmode',
      l: ['outdoor', 'room']
    },
    {
      v: 'RC3x',
      u: 0,
      id: '00hc2 control device',
      c: 'hc2/control',
      l: ['off', 'RC20', 'RC3x']
    },
    {
      v: '01.01.2000-01.01.2000',
      u: 0,
      id: '00hc2 holiday dates',
      c: 'hc2/holidays',
      h: '< dd.mm.yyyy-dd.mm.yyyy >'
    },
    {
      v: '01.01.2020-12.01.2020',
      u: 0,
      id: '00hc2 vacation dates',
      c: 'hc2/vacations',
      h: '< dd.mm.yyyy-dd.mm.yyyy >'
    },
    {
      v: 'own 1',
      u: 0,
      id: '00hc2 program',
      c: 'hc2/program',
      l: ['own 1', 'family', 'morning', 'evening', 'am', 'pm', 'midday', 'singles', 'seniors', 'new', 'own 2']
    },
    {
      v: 0,
      u: 7,
      id: '00hc2 pause time',
      c: 'hc2/pause',
      m: 0,
      x: 99,
      s: '1'
    },
    {
      v: 0,
      u: 7,
      id: '00hc2 party time',
      c: 'hc2/party',
      m: 0,
      x: 99,
      s: '1'
    },
    {
      v: 0,
      u: 1,
      id: '00hc2 temporary set temperature automode',
      c: 'hc2/tempautotemp',
      m: 0,
      x: 30,
      s: '0.5'
    },
    {
      v: -20,
      u: 1,
      id: '00hc2 no reduce below temperature',
      c: 'hc2/noreducetemp',
      m: -30,
      x: 10,
      s: '1'
    },
    {
      v: 8,
      u: 1,
      id: '00hc2 off/reduce switch temperature',
      c: 'hc2/reducetemp',
      m: -20,
      x: 10,
      s: '1'
    },
    {
      v: 5,
      u: 1,
      id: '00hc2 vacations off/reduce switch temperature',
      c: 'hc2/vacreducetemp',
      m: -20,
      x: 10,
      s: '1'
    },
    {
      v: 'outdoor',
      u: 0,
      id: '00hc2 vacations reduce mode',
      c: 'hc2/vacreducemode',
      l: ['nofrost', 'reduce', 'room', 'outdoor']
    },
    {
      v: 'off',
      u: 0,
      id: '00hc2 dhw priority',
      c: 'hc2/wwprio',
      l: ['off', 'on']
    },
    {
      v: '00 mo 05:50 on',
      u: 0,
      id: '00hc2 own1 program switchtime',
      c: 'hc2/switchtime1',
      h: '<nn> [ not_set | day hh:mm on|off ]'
    },
    {
      v: '00 mo 06:30 on',
      u: 0,
      id: '00hc2 own2 program switchtime',
      c: 'hc2/switchtime2',
      h: '<nn> [ not_set | day hh:mm on|off ]'
    }
  ]
};

const emsesp_devicedata_2 = {
  data: [
    {
      v: '(0)',
      u: 0,
      id: '08my custom error code'
    },
    {
      v: '14:54:39 06/06/2021',
      u: 0,
      id: '00date/time'
    },
    {
      v: 18.2,
      u: 1,
      id: '00Chosen Room Temperature',
      c: 'hc1/seltemp',
      m: 5,
      x: 52,
      s: 0.5
    },
    {
      v: 22.6,
      u: 1,
      id: '00hc1 current room temperature'
    },
    {
      v: 'auto',
      u: 0,
      id: '00hc1 mode',
      c: 'hc1/mode',
      l: ['off', 'on', 'auto']
    },
    {
      v: '00 mo 00:00 T1',
      u: 0,
      id: '00hc1 program switchtime',
      c: 'hc1/switchtime',
      h: '<nn> [ not_set | day hh:mm Tn ]'
    }
  ]
};

const emsesp_devicedata_3 = {
  data: [
    {
      v: '',
      u: 0,
      id: '08reset',
      c: 'reset',
      l: ['-', 'maintenance', 'error']
    },
    {
      v: 34,
      u: 1,
      id: '08selected flow temperature',
      c: 'selflowtemp',
      m: 0,
      x: 90,
      s: '1'
    },
    {
      v: 30.7,
      u: 1,
      id: '08current flow temperature'
    },
    {
      v: 176544,
      u: 0,
      id: '08burner starts'
    },
    {
      v: '6L(517) 18.01.2023 10:18 (0 min)',
      u: 0,
      id: '08last error code'
    },
    {
      v: 'off',
      u: 0,
      id: '00force heating off',
      c: 'heatingoff',
      l: ['off', 'on']
    },
    {
      v: 'off',
      u: 0,
      id: '00heating active'
    },
    {
      v: 'off',
      u: 0,
      id: '00tapwater active'
    },
    {
      v: 0,
      u: 3,
      id: '00heating pump modulation'
    },
    {
      v: 15,
      u: 1,
      id: '00outside temperature'
    },
    {
      v: 30.7,
      u: 1,
      id: '00actual boiler temperature'
    },
    {
      v: 29,
      u: 1,
      id: '00exhaust temperature'
    },
    {
      v: 'off',
      u: 0,
      id: '00gas'
    },
    {
      v: 'off',
      u: 0,
      id: '00gas stage 2'
    },
    {
      v: 0,
      u: 9,
      id: '00flame current'
    },
    {
      v: 'off',
      u: 0,
      id: '00heating pump'
    },
    {
      v: 'off',
      u: 0,
      id: '00fan'
    },
    {
      v: 'off',
      u: 0,
      id: '00ignition'
    },
    {
      v: 'off',
      u: 0,
      id: '00oil preheating'
    },
    {
      v: 'on',
      u: 0,
      id: '00heating activated',
      c: 'heatingactivated',
      l: ['off', 'on']
    },
    {
      v: 90,
      u: 1,
      id: '00heating temperature',
      c: 'heatingtemp',
      m: 0,
      x: 90,
      s: '1'
    },
    {
      v: 100,
      u: 3,
      id: '00boiler pump max power',
      c: 'pumpmodmax',
      m: 0,
      x: 100,
      s: '1'
    },
    {
      v: 100,
      u: 3,
      id: '00boiler pump min power',
      c: 'pumpmodmin',
      m: 0,
      x: 100,
      s: '1'
    },
    {
      v: 'deltaP-2',
      u: 0,
      id: '00boiler pump mode',
      c: 'pumpmode',
      l: ['proportional', 'deltaP-1', 'deltaP-2', 'deltaP-3', 'deltaP-4']
    },
    {
      v: 6,
      u: 8,
      id: '00pump delay',
      c: 'pumpdelay',
      m: 0,
      x: 60,
      s: '1'
    },
    {
      v: 15,
      u: 8,
      id: '00burner min period',
      c: 'burnminperiod',
      m: 0,
      x: 120,
      s: '1'
    },
    {
      v: 0,
      u: 3,
      id: '00burner min power',
      c: 'burnminpower',
      m: 0,
      x: 100,
      s: '1'
    },
    {
      v: 100,
      u: 3,
      id: '00burner max power',
      c: 'burnmaxpower',
      m: 0,
      x: 254,
      s: '1'
    },
    {
      v: -8,
      u: 2,
      id: '00hysteresis on temperature',
      c: 'boilhyston',
      m: -20,
      x: 0,
      s: '1'
    },
    {
      v: 15,
      u: 2,
      id: '00hysteresis off temperature',
      c: 'boilhystoff',
      m: 0,
      x: 20,
      s: '1'
    },
    {
      v: -8,
      u: 2,
      id: '00hysteresis stage 2 on temperature',
      c: 'boil2hyston',
      m: -20,
      x: 0,
      s: '1'
    },
    {
      v: 8,
      u: 2,
      id: '00hysteresis stage 2 off temperature',
      c: 'boil2hystoff',
      m: 0,
      x: 20,
      s: '1'
    },
    {
      v: 34,
      u: 1,
      id: '00set flow temperature'
    },
    {
      v: 100,
      u: 3,
      id: '00burner set power'
    },
    {
      v: 100,
      u: 3,
      id: '00burner selected max power',
      c: 'selburnpow',
      m: 0,
      x: 254,
      s: '1'
    },
    {
      v: 0,
      u: 3,
      id: '00burner current power'
    },
    {
      v: 822273,
      u: 8,
      id: '00total burner operating time'
    },
    {
      v: 0,
      u: 8,
      id: '00burner stage 2 operating time'
    },
    {
      v: 787124,
      u: 8,
      id: '00total heat operating time'
    },
    {
      v: 173700,
      u: 0,
      id: '00burner starts heating'
    },
    {
      v: 5495341,
      u: 8,
      id: '00total UBA operating time'
    },
    {
      v: '0Y',
      u: 0,
      id: '00service code'
    },
    {
      v: 0,
      u: 0,
      id: '00service code number'
    },
    {
      v: 'H00',
      u: 0,
      id: '00maintenance message'
    },
    {
      v: 'date',
      u: 0,
      id: '00maintenance scheduled',
      c: 'maintenance',
      l: ['off', 'time', 'date', 'manual']
    },
    {
      v: 6000,
      u: 7,
      id: '00time to next maintenance',
      c: 'maintenancetime',
      m: 0,
      x: 31999,
      s: '1'
    },
    {
      v: '30.06.2023',
      u: 0,
      id: '00next maintenance date',
      c: 'maintenancedate',
      h: '< dd.mm.yyyy >'
    },
    {
      v: 46,
      u: 1,
      id: '00dhw set temperature'
    },
    {
      v: 47,
      u: 1,
      id: '00dhw selected temperature',
      c: 'wwseltemp',
      m: 0,
      x: 254,
      s: '1'
    },
    {
      v: 'buffer',
      u: 0,
      id: '00dhw type'
    },
    {
      v: 'hot',
      u: 0,
      id: '00dhw comfort',
      c: 'wwcomfort',
      l: ['hot', 'eco', 'intelligent']
    },
    {
      v: 40,
      u: 2,
      id: '00dhw flow temperature offset',
      c: 'wwflowtempoffset',
      m: 0,
      x: 100,
      s: '1'
    },
    {
      v: 'on',
      u: 0,
      id: '00dhw circulation pump available',
      c: 'wwcircpump',
      l: ['off', 'on']
    },
    {
      v: 'chargepump',
      u: 0,
      id: '00dhw charging type'
    },
    {
      v: -5,
      u: 2,
      id: '00dhw hysteresis on temperature',
      c: 'wwhyston',
      m: -126,
      x: 126,
      s: '1'
    },
    {
      v: -1,
      u: 2,
      id: '00dhw hysteresis off temperature',
      c: 'wwhystoff',
      m: -126,
      x: 126,
      s: '1'
    },
    {
      v: 70,
      u: 1,
      id: '00dhw disinfection temperature',
      c: 'wwdisinfectiontemp',
      m: 0,
      x: 254,
      s: '1'
    },
    {
      v: 'continuous',
      u: 0,
      id: '00dhw circulation pump mode',
      c: 'wwcircmode',
      l: ['off', '1x3min', '2x3min', '3x3min', '4x3min', '5x3min', '6x3min', 'continuous']
    },
    {
      v: 'off',
      u: 0,
      id: '00dhw circulation active',
      c: 'wwcirc',
      l: ['off', 'on']
    },
    {
      v: 60.7,
      u: 1,
      id: '00dhw current intern temperature'
    },
    {
      v: 0,
      u: 4,
      id: '00dhw current tap water flow'
    },
    {
      v: 60.7,
      u: 1,
      id: '00dhw storage intern temperature'
    },
    {
      v: 'on',
      u: 0,
      id: '00dhw activated',
      c: 'wwactivated',
      l: ['off', 'on']
    },
    {
      v: 'off',
      u: 0,
      id: '00dhw one time charging',
      c: 'wwonetime',
      l: ['off', 'on']
    },
    {
      v: 'off',
      u: 0,
      id: '00dhw disinfecting',
      c: 'wwdisinfecting',
      l: ['off', 'on']
    },
    {
      v: 'off',
      u: 0,
      id: '00dhw charging'
    },
    {
      v: 'off',
      u: 0,
      id: '00dhw recharging'
    },
    {
      v: 'on',
      u: 0,
      id: '00dhw temperature ok'
    },
    {
      v: 'off',
      u: 0,
      id: '00dhw active'
    },
    {
      v: 'off',
      u: 0,
      id: '00dhw 3-way valve active'
    },
    {
      v: 0,
      u: 3,
      id: '00dhw set pump power'
    },
    {
      v: 6976,
      u: 0,
      id: '00dhw starts'
    },
    {
      v: 80644,
      u: 8,
      id: '00dhw active time'
    }
  ]
};

const emsesp_devicedata_4 = {
  data: [
    {
      v: 16,
      u: 1,
      id: '08hc2 selected room temperature',
      c: 'hc2/seltemp'
    },
    {
      v: 18.6,
      u: 1,
      id: '02hc2 current room temperature',
      c: ''
    },
    {
      v: 'off',
      u: 0,
      id: '02hc2 mode',
      c: 'hc2/mode',
      l: ['off', 'on', 'auto']
    }
  ]
};

const emsesp_devicedata_5 = {
  data: [
    {
      v: 30,
      u: 1,
      id: '00hc2 flow temperature (TC1)'
    },
    {
      v: 100,
      u: 3,
      id: '00hc2 mixing valve actuator (VC1)'
    },
    {
      v: 34,
      u: 1,
      id: '00hc2 setpoint flow temperature',
      c: 'hc2/flowsettemp',
      m: 0,
      x: 254,
      s: '1'
    },
    {
      v: 'off',
      u: 0,
      id: '00hc2 pump status (PC1)',
      c: 'hc2/pumpstatus',
      l: ['off', 'on']
    },
    {
      v: 'on',
      u: 0,
      id: '00hc2 activated',
      c: 'hc2/activated',
      l: ['off', 'on']
    },
    {
      v: 120,
      u: 14,
      id: '00hc2 time to set valve',
      c: 'hc2/valvesettime',
      m: 10,
      x: 120,
      s: '10'
    }
  ]
};

const emsesp_devicedata_6 = {
  data: [
    {
      v: 43.9,
      u: 1,
      id: '00collector temperature (TS1)'
    },
    {
      v: 28.3,
      u: 1,
      id: '00cylinder bottom temperature (TS2)'
    },
    {
      v: 'on',
      u: 0,
      id: '00pump (PS1)'
    },
    {
      v: 181884,
      u: 8,
      id: '00pump working time'
    },
    {
      v: 90,
      u: 1,
      id: '00maximum cylinder temperature',
      c: 'cylmaxtemp',
      m: 0,
      x: 254,
      s: '1'
    },
    {
      v: 'off',
      u: 0,
      id: '00collector shutdown'
    },
    {
      v: 'off',
      u: 0,
      id: '00cyl heated'
    },
    {
      v: 32,
      u: 3,
      id: '00pump modulation (PS1)'
    },
    {
      v: 30,
      u: 3,
      id: '00minimum pump modulation',
      c: 'pumpminmod',
      m: 0,
      x: 100,
      s: '1'
    },
    {
      v: 10,
      u: 2,
      id: '00pump turn on difference',
      c: 'turnondiff',
      m: 0,
      x: 254,
      s: '1'
    },
    {
      v: 5,
      u: 2,
      id: '00pump turn off difference',
      c: 'turnoffdiff',
      m: 0,
      x: 254,
      s: '1'
    },
    {
      v: 899,
      u: 12,
      id: '00actual solar power'
    },
    {
      v: 94,
      u: 6,
      id: '00energy last hour'
    },
    {
      v: 3,
      u: 4,
      id: '00maximum solar flow',
      c: 'maxflow',
      m: 0,
      x: 25,
      s: '0.1'
    },
    {
      v: 37,
      u: 1,
      id: '00dhw minimum temperature',
      c: 'wwmintemp',
      m: 0,
      x: 254,
      s: '1'
    },
    {
      v: 'on',
      u: 0,
      id: '00solarmodule enabled',
      c: 'solarenabled',
      l: ['off', 'on']
    },
    {
      v: 11,
      u: 0,
      id: '00unknown setting 3',
      c: 'setting3',
      m: 0,
      x: 254,
      s: '1'
    },
    {
      v: 2,
      u: 0,
      id: '00unknown setting 4',
      c: 'setting4',
      m: 0,
      x: 254,
      s: '1'
    },
    {
      v: 0,
      u: 0,
      id: '00unknown datafield 11'
    },
    {
      v: 1,
      u: 0,
      id: '00unknown datafield 12'
    },
    {
      v: 0,
      u: 0,
      id: '00unknown datafield 1'
    },
    {
      v: 0,
      u: 0,
      id: '00unknown datafield 0'
    }
  ]
};

const emsesp_devicedata_99 = {
  data: [
    {
      v: 5,
      u: 1,
      id: '00boiler_flowtemp',
      c: 'boiler_flowtemp'
    }
  ]
};

// CUSTOM ENTITIES
let emsesp_entities = {
  // entities: []
  entities: [
    {
      id: 0,
      device_id: 8,
      type_id: 24,
      offset: 0,
      factor: 1,
      name: 'boiler_flowtemp',
      uom: 1,
      value_type: 1,
      writeable: true
    }
  ]
};

// SCHEDULE
let emsesp_schedule = {
  schedule: [
    {
      id: 1,
      active: true,
      flags: 6,
      time: '07:30',
      cmd: 'hc1/mode',
      value: 'day',
      name: 'day_mode'
    },
    {
      id: 2,
      active: true,
      flags: 31,
      time: '23:00',
      cmd: 'hc1/mode',
      value: 'night',
      name: 'night_mode'
    },
    {
      id: 3,
      active: true,
      flags: 10,
      time: '00:00',
      cmd: 'thermostat/hc2/seltemp',
      value: '20',
      name: 'temp_20'
    },
    {
      id: 4,
      active: false,
      flags: 1,
      time: '04:00',
      cmd: 'system/restart',
      value: '',
      name: 'auto_restart'
    }
  ]
};

// CUSTOMIZATIONS

const emsesp_deviceentities_1 = [{}];
const emsesp_deviceentities_3 = [{}];
const emsesp_deviceentities_5 = [{}];
const emsesp_deviceentities_6 = [{}];

const emsesp_deviceentities_2 = [
  {
    v: '(0)',
    n: 'error code',
    cn: 'my custom error code',
    id: 'errorcode',
    m: 8,
    w: false
  },
  {
    v: '14:54:39 06/06/2021',
    n: 'date/time',
    id: 'datetime',
    m: 0,
    w: false
  },
  {
    v: 18.2,
    n: 'Chosen Room Temperature',
    id: 'hc1/seltemp',
    m: 0,
    mi: 5,
    ma: 52,
    w: true
  },
  {
    v: 22.6,
    n: 'hc1 current room temperature',
    id: 'hc1/curtemp',
    m: 0,
    w: false
  },
  {
    v: 'auto',
    n: 'hc1 mode',
    id: 'hc1/mode',
    m: 0,
    w: true
  }
];

const emsesp_deviceentities_7 = [
  { u: 0, n: '!reset', id: 'reset', m: 8, w: false },
  { v: false, n: 'heating active', id: 'heatingactive', m: 8, w: false },
  { v: false, n: 'tapwater active', id: 'tapwateractive', m: 4, w: false },
  { v: 5, n: 'selected flow temperature', id: 'selflowtemp', m: 4, w: true },
  { v: 0, n: 'burner selected max power', id: 'selburnpow', m: 14, w: true },
  { v: 0, n: 'heating pump modulation', id: 'heatingpumpmod', m: 0, w: false },
  { n: 'heating pump 2 modulation', id: 'heatingpump2mod', m: 0, w: false },
  { n: 'outside temperature', id: 'outdoortemp', m: 0, w: false },
  { v: 53, n: 'current flow temperature', id: 'curflowtemp', m: 0, w: false },
  { v: 51.8, n: 'return temperature', id: 'rettemp', m: 0, w: false },
  { n: 'mixing switch temperature', id: 'switchtemp', m: 0, w: false },
  { v: 1.3, n: 'system pressure', id: 'syspress', m: 0, w: false },
  { v: 54.6, n: 'actual boiler temperature', id: 'boiltemp', m: 0, w: false },
  { n: 'exhaust temperature', id: 'exhausttemp', m: 0, w: false },
  { v: false, n: 'gas', id: 'burngas', m: 0, w: false },
  { v: false, n: 'gas stage 2', id: 'burngas2', m: 0, w: false },
  { v: 0, n: 'flame current', id: 'flamecurr', m: 0, w: false },
  { v: false, n: 'heating pump', id: 'heatingpump', m: 0, w: false },
  { v: false, n: 'fan', id: 'fanwork', m: 0, w: false },
  { v: false, n: 'ignition', id: 'ignwork', m: 0, w: false },
  { v: false, n: 'oil preheating', id: 'oilpreheat', m: 0, w: false },
  { v: true, n: 'heating activated', id: 'heatingactivated', m: 0, w: false },
  { v: 80, n: 'heating temperature', id: 'heatingtemp', m: 0, w: false },
  { v: 70, n: 'burner pump max power', id: 'pumpmodmax', m: 0, w: false },
  { v: 30, n: 'burner pump min power', id: 'pumpmodmin', m: 0, w: false },
  { v: 1, n: 'pump delay', id: 'pumpdelay', m: 0, w: false },
  { v: 10, n: 'burner min period', id: 'burnminperiod', m: 0, w: false },
  { v: 0, n: 'burner min power', id: 'burnminpower', m: 0, w: false },
  { v: 50, n: 'burner max power', id: 'burnmaxpower', m: 0, w: false },
  { v: -6, n: 'hysteresis on temperature', id: 'boilhyston', m: 0, w: false },
  { v: 6, n: 'hysteresis off temperature', id: 'boilhystoff', m: 0, w: false },
  { v: 0, n: 'set flow temperature', id: 'setflowtemp', m: 0, w: true },
  { v: 0, n: 'burner set power', id: 'setburnpow', m: 0, w: false },
  { v: 0, n: 'burner current power', id: 'curburnpow', m: 0, w: false },
  { v: 326323, n: 'burner starts', id: 'burnstarts', m: 0, w: false },
  { v: 553437, n: 'total burner operating time', id: 'burnworkmin', m: 0, w: false },
  { v: 451286, n: 'total heat operating time', id: 'heatworkmin', m: 0, w: false },
  { v: 4672175, n: 'total UBA operating time', id: 'ubauptime', m: 0, w: false },
  { v: '1C(210) 06.06.2020 12:07 (0 min)', n: 'last error code', id: 'lastcode', m: 0, w: false },
  { v: '0H', n: 'service code', id: 'servicecode', m: 0, w: false },
  { v: 203, n: 'service code number', id: 'servicecodenumber', m: 0, w: false },
  { v: 'H00', n: 'maintenance message', id: 'maintenancemessage', m: 0, w: false },
  { v: 'manual', n: 'maintenance scheduled', id: 'maintenance', m: 0, w: false },
  { v: 6000, n: 'time to next maintenance', id: 'maintenancetime', m: 0, w: false },
  { v: '01.01.2012', n: 'next maintenance date', id: 'maintenancedate', m: 0, w: false },
  { v: true, n: 'dhw turn on/off', id: 'wwtapactivated', m: 0, w: false },
  { v: 62, n: 'dhw set temperature', id: 'wwsettemp', m: 0, w: false },
  { v: 60, n: 'dhw selected temperature', id: 'wwseltemp', m: 0, w: true },
  { n: 'dhw selected lower temperature', id: 'wwseltemplow', m: 2 },
  { n: 'dhw selected temperature for off', id: 'wwseltempoff', m: 2 },
  { n: 'dhw single charge temperature', id: 'wwseltempsingle', m: 2 },
  { v: 'flow', n: 'dhw type', id: 'wwtype', m: 0, w: false },
  { v: 'hot', n: 'dhw comfort', id: 'wwcomfort', m: 0, w: false },
  { v: 40, n: 'dhw flow temperature offset', id: 'wwflowtempoffset', m: 0, w: false },
  { v: 100, n: 'dhw max power', id: 'wwmaxpower', m: 0, w: false },
  { v: false, n: 'dhw circulation pump available', id: 'wwcircpump', m: 0, w: false },
  { v: '3-way valve', n: 'dhw charging type', id: 'wwchargetype', m: 0, w: false },
  { v: -5, n: 'dhw hysteresis on temperature', id: 'wwhyston', m: 0, w: false },
  { v: 0, n: 'dhw hysteresis off temperature', id: 'wwhystoff', m: 0, w: false },
  { v: 70, n: 'dhw disinfection temperature', id: 'wwdisinfectiontemp', m: 0, w: false },
  { v: 'off', n: 'dhw circulation pump mode', id: 'wwcircmode', m: 0, w: false },
  { v: false, n: 'dhw circulation active', id: 'wwcirc', m: 0, w: false },
  { v: 46.4, n: 'dhw current intern temperature', id: 'wwcurtemp', m: 0, w: false },
  { n: 'dhw current extern temperature', id: 'wwcurtemp2', m: 2 },
  { v: 0, n: 'dhw current tap water flow', id: 'wwcurflow', m: 0, w: false },
  { v: 46.3, n: 'dhw storage intern temperature', id: 'wwstoragetemp1', m: 0, w: false },
  { n: 'dhw storage extern temperature', id: 'wwstoragetemp2', m: 2 },
  { v: true, n: 'dhw activated', id: 'wwactivated', m: 0, w: false },
  { v: false, n: 'dhw one time charging', id: 'wwonetime', m: 0, w: false },
  { v: false, n: 'dhw disinfecting', id: 'wwdisinfecting', m: 0, w: false },
  { v: false, n: 'dhw charging', id: 'wwcharging', m: 0, w: false },
  { v: false, n: 'dhw recharging', id: 'wwrecharging', m: 0, w: false },
  { v: true, n: 'dhw temperature ok', id: 'wwtempok', m: 0, w: false },
  { v: false, n: 'dhw active', id: 'wwactive', m: 0, w: false },
  { v: true, n: 'dhw 3way valve active', id: 'ww3wayvalve', m: 0, w: false },
  { v: 0, n: 'dhw set pump power', id: 'wwsetpumppower', m: 0, w: true },
  { n: 'dhw mixer temperature', id: 'wwmixertemp', m: 2 },
  { n: 'dhw cylinder middle temperature (TS3)', id: 'wwcylmiddletemp', m: 2 },
  { v: 288768, n: 'dhw starts', id: 'wwstarts', m: 0, w: false },
  { v: 102151, n: 'dhw active time', id: 'wwworkm', m: 0, w: false }
];

const emsesp_deviceentities_4 = [
  {
    v: 16,
    n: 'hc2 selected room temperature',
    id: 'hc2/seltemp',
    m: 8,
    w: true
  },
  {
    v: 18.5,
    n: 'hc2 current room temperature',
    id: 'hc2/curtemp',
    m: 2,
    w: false
  },
  {
    v: 'off',
    n: 'hc2 mode',
    id: 'hc2/mode',
    m: 2,
    w: true
  }
];

// LOG
rest_server.get(FETCH_LOG_ENDPOINT, (req, res) => {
  const encoded = msgpack.encode(fetch_log);
  console.log('fetchlog');
  res.write(encoded, 'binary');
  res.end(null, 'binary');
});
rest_server.get(LOG_SETTINGS_ENDPOINT, (req, res) => {
  res.json(log_settings);
});
rest_server.post(LOG_SETTINGS_ENDPOINT, (req, res) => {
  log_settings = req.body;
  console.log(JSON.stringify(log_settings));
  res.json(log_settings);
});

// NETWORK
rest_server.get(NETWORK_STATUS_ENDPOINT, (req, res) => {
  res.json(network_status);
});
rest_server.get(NETWORK_SETTINGS_ENDPOINT, (req, res) => {
  res.json(network_settings);
});
rest_server.post(NETWORK_SETTINGS_ENDPOINT, (req, res) => {
  network_settings = req.body;
  console.log(JSON.stringify(network_settings));
  res.json(network_settings);
});
rest_server.get(LIST_NETWORKS_ENDPOINT, (req, res) => {
  res.json(list_networks);
});
rest_server.get(SCAN_NETWORKS_ENDPOINT, (req, res) => {
  res.sendStatus(202);
});

// AP
rest_server.get(AP_SETTINGS_ENDPOINT, (req, res) => {
  res.json(ap_settings);
});
rest_server.get(AP_STATUS_ENDPOINT, (req, res) => {
  res.json(ap_status);
});
rest_server.post(AP_SETTINGS_ENDPOINT, (req, res) => {
  ap_status = req.body;
  console.log(JSON.stringify(ap_settings));
  res.json(ap_settings);
});

// OTA
rest_server.get(OTA_SETTINGS_ENDPOINT, (req, res) => {
  res.json(ota_settings);
});
rest_server.post(OTA_SETTINGS_ENDPOINT, (req, res) => {
  ota_settings = req.body;
  console.log(JSON.stringify(ota_settings));
  res.json(ota_settings);
});

// MQTT
rest_server.get(MQTT_SETTINGS_ENDPOINT, (req, res) => {
  res.json(mqtt_settings);
});
rest_server.post(MQTT_SETTINGS_ENDPOINT, (req, res) => {
  mqtt_settings = req.body;
  console.log(JSON.stringify(mqtt_settings));
  res.json(mqtt_settings);
});
rest_server.get(MQTT_STATUS_ENDPOINT, (req, res) => {
  res.json(mqtt_status);
});

// NTP
rest_server.get(NTP_SETTINGS_ENDPOINT, (req, res) => {
  res.json(ntp_settings);
});
rest_server.post(NTP_SETTINGS_ENDPOINT, (req, res) => {
  ntp_settings = req.body;
  console.log(JSON.stringify(ntp_settings));
  res.json(ntp_settings);
});
rest_server.get(NTP_STATUS_ENDPOINT, (req, res) => {
  res.json(ntp_status);
});
rest_server.post(TIME_ENDPOINT, (req, res) => {
  res.sendStatus(200);
});

// SYSTEM
rest_server.get(SYSTEM_STATUS_ENDPOINT, (req, res) => {
  res.json(system_status);
});
rest_server.get(SECURITY_SETTINGS_ENDPOINT, (req, res) => {
  res.json(security_settings);
});
rest_server.post(SECURITY_SETTINGS_ENDPOINT, (req, res) => {
  security_settings = req.body;
  console.log(JSON.stringify(security_settings));
  res.json(security_settings);
});
rest_server.get(FEATURES_ENDPOINT, (req, res) => {
  res.json(features);
});
rest_server.get(VERIFY_AUTHORIZATION_ENDPOINT, (req, res) => {
  res.json(verify_authentication);
});
rest_server.post(RESTART_ENDPOINT, (req, res) => {
  res.sendStatus(200);
});
rest_server.post(FACTORY_RESET_ENDPOINT, (req, res) => {
  res.sendStatus(200);
});
rest_server.post(UPLOAD_FILE_ENDPOINT, (req, res) => {
  res.sendStatus(200);
});
rest_server.post(SIGN_IN_ENDPOINT, (req, res) => {
  console.log('Signed in as ' + req.body.username);
  res.json(signin);
});
rest_server.get(GENERATE_TOKEN_ENDPOINT, (req, res) => {
  res.json(generate_token);
});

// EMS-ESP Project stuff
rest_server.post(EMSESP_RESET_CUSTOMIZATIONS_ENDPOINT, (req, res) => {
  console.log('Removing all customizations...');
  res.sendStatus(200);
});
rest_server.get(EMSESP_SETTINGS_ENDPOINT, (req, res) => {
  console.log('Get settings: ' + JSON.stringify(settings));
  res.json(settings);
});
rest_server.post(EMSESP_SETTINGS_ENDPOINT, (req, res) => {
  settings = req.body;
  console.log('Write settings: ' + JSON.stringify(settings));
  res.status(202).json(settings); // restart needed
  // res.status(200).json(settings); // no restart needed
});
rest_server.get(EMSESP_CORE_DATA_ENDPOINT, (req, res) => {
  console.log('send back core data...');
  res.json(emsesp_coredata);
});
rest_server.get(EMSESP_SENSOR_DATA_ENDPOINT, (req, res) => {
  console.log('send back sensor data...');
  // console.log(emsesp_sensordata);
  res.json(emsesp_sensordata);
});
rest_server.get(EMSESP_DEVICES_ENDPOINT, (req, res) => {
  res.json(emsesp_devices);
});
rest_server.post(EMSESP_SCANDEVICES_ENDPOINT, (req, res) => {
  console.log('Scan devices...');
  res.sendStatus(200);
});
rest_server.get(EMSESP_STATUS_ENDPOINT, (req, res) => {
  res.json(status);
});
rest_server.post(EMSESP_DEVICEDATA_ENDPOINT, (req, res) => {
  const id = req.body.id;
  console.log('send back device data for ' + id);
  let data = {};

  if (id === 1) {
    data = emsesp_devicedata_1;
  }
  if (id === 2) {
    data = emsesp_devicedata_2;
  }
  if (id === 3) {
    data = emsesp_devicedata_3;
  }
  if (id === 4) {
    data = emsesp_devicedata_4;
  }
  if (id === 5) {
    data = emsesp_devicedata_5;
  }
  if (id === 6) {
    data = emsesp_devicedata_6;
  }
  if (id === 7) {
    data = emsesp_devicedata_7;
  }
  if (id === 99) {
    data = emsesp_devicedata_99;
  }
  res.write(msgpack.encode(data), 'binary');
  res.end(null, 'binary');
});

rest_server.post(EMSESP_DEVICEENTITIES_ENDPOINT, (req, res) => {
  const id = req.body.id;
  let data = null;

  if (id === 1) {
    data = emsesp_deviceentities_1;
  }
  if (id === 2) {
    data = emsesp_deviceentities_2;
  }
  if (id === 3) {
    data = emsesp_deviceentities_3;
  }
  if (id === 4) {
    data = emsesp_deviceentities_4;
  }
  if (id === 5) {
    data = emsesp_deviceentities_5;
  }
  if (id === 6) {
    data = emsesp_deviceentities_6;
  }
  if (id === 7) {
    data = emsesp_deviceentities_7;
  }

  res.write(msgpack.encode(data), 'binary');
  res.end(null, 'binary');
});

function updateMask(entity, de, dd) {
  const current_mask = parseInt(entity.slice(0, 2), 16);

  // strip of any min/max ranges
  const shortname_with_customname = entity.slice(2).split('>')[0];
  const shortname = shortname_with_customname.split('|')[0];
  const new_custom_name = shortname_with_customname.split('|')[1];
  const has_min_max = entity.slice(2).split('>')[1];

  // find in de
  de_objIndex = de.findIndex((obj) => obj.id === shortname);
  if (de_objIndex !== -1) {
    // get current name
    if (de[de_objIndex].cn) {
      fullname = de[de_objIndex].cn;
    } else {
      fullname = de[de_objIndex].n;
    }

    // find in dd, either looking for fullname or custom name
    dd_objIndex = dd.data.findIndex((obj) => obj.id.slice(2) === fullname);
    if (dd_objIndex !== -1) {
      let changed = new Boolean(false);

      // see if the mask has changed
      const old_mask = parseInt(dd.data[dd_objIndex].id.slice(0, 2), 16);
      if (old_mask !== current_mask) {
        changed = true;
        console.log('mask has changed to ' + current_mask.toString(16));
      }

      // see if the custom name has changed
      const old_custom_name = dd.data[dd_objIndex].cn;
      console.log('comparing names, old (' + old_custom_name + ') with new (' + new_custom_name + ')');
      if (old_custom_name !== new_custom_name) {
        changed = true;
        new_fullname = new_custom_name;
        console.log('name has changed to ' + new_custom_name);
      } else {
        new_fullname = fullname;
      }

      // see if min or max has changed
      // get current min/max values if they exist
      const current_min = dd.data[dd_objIndex].min;
      const current_max = dd.data[dd_objIndex].max;
      new_min = current_min;
      new_max = current_max;
      if (has_min_max) {
        new_min = parseInt(has_min_max.split('<')[0]);
        new_max = parseInt(has_min_max.split('<')[1]);
        if (current_min !== new_min || current_max !== new_max) {
          changed = true;
          console.log('min/max has changed to ' + new_min + '/' + new_max);
        }
      }

      if (changed === true) {
        console.log(
          'Updating ' + dd.data[dd_objIndex].id + ' -> ' + current_mask.toString(16).padStart(2, '0') + new_fullname
        );
        de[de_objIndex].m = current_mask;
        de[de_objIndex].cn = new_fullname;
        if (new_min) {
          de[de_objIndex].mi = new_min;
        }
        if (new_max) {
          de[de_objIndex].ma = new_max;
        }
        dd.data[dd_objIndex].id = current_mask.toString(16).padStart(2, '0') + new_fullname;
        dd.data[dd_objIndex].cn = new_fullname;
      }

      console.log('new dd:');
      console.log(dd.data[dd_objIndex]);
      console.log('new de:');
      console.log(de[de_objIndex]);
    } else {
      console.log('error, dd not found');
    }
  } else {
    console.log("can't locate record for shortname " + shortname);
  }
}

rest_server.post(EMSESP_CUSTOM_ENTITIES_ENDPOINT, (req, res) => {
  const id = req.body.id;
  console.log('customization id = ' + id);
  console.log(req.body.entity_ids);
  for (const entity of req.body.entity_ids) {
    if (id === 7) {
      updateMask(entity, emsesp_deviceentities_7, emsesp_devicedata_7);
    } else if (id === 1) {
      updateMask(entity, emsesp_deviceentities_1, emsesp_devicedata_1);
    } else if (id === 2) {
      updateMask(entity, emsesp_deviceentities_2, emsesp_devicedata_2);
    } else if (id === 3) {
      updateMask(entity, emsesp_deviceentities_3, emsesp_devicedata_3);
    } else if (id === 4) {
      updateMask(entity, emsesp_deviceentities_4, emsesp_devicedata_4);
    } else if (id === 5) {
      updateMask(entity, emsesp_deviceentities_5, emsesp_devicedata_5);
    } else if (id === 6) {
      updateMask(entity, emsesp_deviceentities_6, emsesp_devicedata_6);
    }
  }
  res.sendStatus(200);
});

rest_server.post(EMSESP_WRITE_SCHEDULE_ENDPOINT, (req, res) => {
  console.log('write schedule');
  console.log(req.body.schedule);
  emsesp_schedule = req.body;
  res.sendStatus(200);
});

rest_server.post(EMSESP_WRITE_ENTITIES_ENDPOINT, (req, res) => {
  console.log('write entities');
  console.log(req.body.entities);
  emsesp_entities = req.body;
  res.sendStatus(200);
});

rest_server.post(EMSESP_WRITE_VALUE_ENDPOINT, (req, res) => {
  const devicevalue = req.body.devicevalue;
  const id = req.body.id;

  if (id === 1) {
    console.log('Write device value for: ' + JSON.stringify(devicevalue));
    objIndex = emsesp_devicedata_1.data.findIndex((obj) => obj.c == devicevalue.c);
    emsesp_devicedata_1.data[objIndex] = devicevalue;
  }
  if (id === 2) {
    console.log('Write device value for: ' + JSON.stringify(devicevalue));
    objIndex = emsesp_devicedata_2.data.findIndex((obj) => obj.c == devicevalue.c);
    emsesp_devicedata_2.data[objIndex] = devicevalue;
  }
  if (id === 3) {
    console.log('Write device value for: ' + JSON.stringify(devicevalue));
    objIndex = emsesp_devicedata_3.data.findIndex((obj) => obj.c == devicevalue.c);
    emsesp_devicedata_3.data[objIndex] = devicevalue;
  }
  if (id === 4) {
    console.log('Write device value for: ' + JSON.stringify(devicevalue));
    objIndex = emsesp_devicedata_4.data.findIndex((obj) => obj.c == devicevalue.c);
    emsesp_devicedata_4.data[objIndex] = devicevalue;
  }
  if (id === 5) {
    console.log('Write device value for: ' + JSON.stringify(devicevalue));
    objIndex = emsesp_devicedata_5.data.findIndex((obj) => obj.c == devicevalue.c);
    emsesp_devicedata_5.data[objIndex] = devicevalue;
  }
  if (id === 6) {
    console.log('Write device value for: ' + JSON.stringify(devicevalue));
    objIndex = emsesp_devicedata_6.data.findIndex((obj) => obj.c == devicevalue.c);
    emsesp_devicedata_6.data[objIndex] = devicevalue;
  }
  if (id === 7) {
    console.log('Write device value for: ' + JSON.stringify(devicevalue));
    objIndex = emsesp_devicedata_7.data.findIndex((obj) => obj.c == devicevalue.c);
    emsesp_devicedata_7.data[objIndex] = devicevalue;
  }
  if (id === 99) {
    console.log('Write device value for: ' + JSON.stringify(devicevalue));
    objIndex = emsesp_devicedata_99.data.findIndex((obj) => obj.c == devicevalue.c);
    emsesp_devicedata_99.data[objIndex] = devicevalue;
  }

  res.sendStatus(200);
});

rest_server.post(EMSESP_WRITE_SENSOR_ENDPOINT, (req, res) => {
  const ts = req.body;
  console.log('Write temperaure sensor: ' + JSON.stringify(ts));
  objIndex = emsesp_sensordata.ts.findIndex((obj) => obj.id == ts.id);
  if (objIndex !== -1) {
    emsesp_sensordata.ts[objIndex].n = ts.name;
    emsesp_sensordata.ts[objIndex].o = ts.offset;
  } else {
    console.log('not found');
  }
  res.sendStatus(200);
});

rest_server.post(EMSESP_WRITE_ANALOG_ENDPOINT, (req, res) => {
  const as = req.body;
  console.log('Write analog sensor: ' + JSON.stringify(as));
  objIndex = emsesp_sensordata.as.findIndex((obj) => obj.g == as.gpio);

  if (objIndex === -1) {
    console.log('new analog entry found');
    emsesp_sensordata.as.push({
      id: as.id,
      g: as.gpio,
      n: as.name,
      f: as.factor,
      o: as.offset,
      u: as.uom,
      t: as.type,
      d: as.deleted
    });
  } else {
    if (as.deleted) {
      console.log('removing analog gpio' + as.gpio + ' index ' + objIndex);
      emsesp_sensordata.as[objIndex].d = true;
      var filtered = emsesp_sensordata.as.filter(function (value, index, arr) {
        return !value.d;
      });
      emsesp_sensordata.as = filtered;
    } else {
      console.log('updating analog gpio' + as.gpio + ' index ' + objIndex);
      emsesp_sensordata.as[objIndex].n = as.name;
      emsesp_sensordata.as[objIndex].f = as.factor;
      emsesp_sensordata.as[objIndex].o = as.offset;
      emsesp_sensordata.as[objIndex].u = as.uom;
      emsesp_sensordata.as[objIndex].t = as.type;
    }
  }

  res.sendStatus(200);
});

rest_server.post(EMSESP_BOARDPROFILE_ENDPOINT, (req, res) => {
  const board_profile = req.body.board_profile;

  const data = {
    led_gpio: settings.led_gpio,
    dallas_gpio: settings.dallas_gpio,
    rx_gpio: settings.rx_gpio,
    tx_gpio: settings.tx_gpio,
    pbutton_gpio: settings.pbutton_gpio,
    phy_type: settings.phy_type,
    eth_power: settings.eth_power,
    eth_phy_addr: settings.eth_phy_addr,
    eth_clock_mode: settings.eth_clock_mode
  };

  if (board_profile == 'S32') {
    // BBQKees Gateway S32
    data.led_gpio = 2;
    data.dallas_gpio = 18;
    data.rx_gpio = 23;
    data.tx_gpio = 5;
    data.pbutton_gpio = 0;
    data.phy_type = 0;
    data.eth_power = 0;
    data.eth_phy_addr = 0;
    data.eth_clock_mode = 0;
  } else if (board_profile == 'E32') {
    // BBQKees Gateway E32
    data.led_gpio = 2;
    data.dallas_gpio = 4;
    data.rx_gpio = 5;
    data.tx_gpio = 17;
    data.pbutton_gpio = 33;
    data.phy_type = 1;
    data.eth_power = 16;
    data.eth_phy_addr = 1;
    data.eth_clock_mode = 0;
  } else if (board_profile == 'MH-ET') {
    // MH-ET Live D1 Mini
    data.led_gpio = 2;
    data.dallas_gpio = 18;
    data.rx_gpio = 23;
    data.tx_gpio = 5;
    data.pbutton_gpio = 0;
    data.phy_type = 0;
    data.eth_power = 0;
    data.eth_phy_addr = 0;
    data.eth_clock_mode = 0;
  } else if (board_profile == 'NODEMCU') {
    // NodeMCU 32S
    data.led_gpio = 2;
    data.dallas_gpio = 18;
    data.rx_gpio = 23;
    data.tx_gpio = 5;
    data.pbutton_gpio = 0;
    data.phy_type = 0;
    data.eth_power = 0;
    data.eth_phy_addr = 0;
    data.eth_clock_mode = 0;
  } else if (board_profile == 'LOLIN') {
    // Lolin D32
    data.led_gpio = 2;
    data.dallas_gpio = 18;
    data.rx_gpio = 17;
    data.tx_gpio = 16;
    data.pbutton_gpio = 0;
    data.phy_type = 0;
    data.eth_power = 0;
    data.eth_phy_addr = 0;
    data.eth_clock_mode = 0;
  } else if (board_profile == 'OLIMEX') {
    // Olimex ESP32-EVB (uses U1TXD/U1RXD/BUTTON, no LED or Dallas)
    data.led_gpio = 0;
    data.dallas_gpio = 0;
    data.rx_gpio = 36;
    data.tx_gpio = 4;
    data.pbutton_gpio = 34;
    data.phy_type = 1;
    data.eth_power = -1;
    data.eth_phy_addr = 0;
    data.eth_clock_mode = 0;
  } else if (board_profile == 'OLIMEXPOE') {
    // Olimex ESP32-POE
    data.led_gpio = 0;
    data.dallas_gpio = 0;
    data.rx_gpio = 36;
    data.tx_gpio = 4;
    data.pbutton_gpio = 34;
    data.phy_type = 1;
    data.eth_power = 12;
    data.eth_phy_addr = 0;
    data.eth_clock_mode = 3;
  } else if (board_profile == 'C3MINI') {
    // Lolin C3 mini
    data.led_gpio = 7;
    data.dallas_gpio = 1;
    data.rx_gpio = 4;
    data.tx_gpio = 5;
    data.pbutton_gpio = 9;
    data.phy_type = 0;
    data.eth_power = 0;
    data.eth_phy_addr = 0;
    data.eth_clock_mode = 0;
  } else if (board_profile == 'S2MINI') {
    // Lolin C3 mini
    data.led_gpio = 15;
    data.dallas_gpio = 7;
    data.rx_gpio = 11;
    data.tx_gpio = 12;
    data.pbutton_gpio = 0;
    data.phy_type = 0;
    data.eth_power = 0;
    data.eth_phy_addr = 0;
    data.eth_clock_mode = 0;
  } else if (board_profile == 'S3MINI') {
    // Liligo S3 mini
    data.led_gpio = 17;
    data.dallas_gpio = 18;
    data.rx_gpio = 8;
    data.tx_gpio = 5;
    data.pbutton_gpio = 0;
    data.phy_type = 0;
    data.eth_power = 0;
    data.eth_phy_addr = 0;
    data.eth_clock_mode = 0;
  }

  console.log('boardProfile POST. Sending back, profile: ' + board_profile + ', ' + 'data: ' + JSON.stringify(data));

  res.send(data);
});

// EMS-ESP API specific
const emsesp_info = {
  System: {
    version: '3.6.0',
    uptime: '001+06:40:34.018',
    'uptime (seconds)': 110434,
    freemem: 131,
    'reset reason': 'Software reset CPU / Software reset CPU',
    'Sensor sensors': 3
  },
  Network: {
    connection: 'Ethernet',
    hostname: 'ems-esp',
    MAC: 'A8:03:2A:62:64:CF',
    'IPv4 address': '192.168.1.134/255.255.255.0',
    'IPv4 gateway': '192.168.1.1',
    'IPv4 nameserver': '192.168.1.1'
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
    'Sensor reads': 22086,
    'Sensor fails': 0
  },
  Devices: [
    {
      type: 'Boiler',
      name: 'Nefit GBx72/Trendline/Cerapur/Greenstar Si/27i (DeviceID:0x08 ProductID:123, Version:06.01)',
      handlers:
        '0x10 0x11 0xC2 0x14 0x15 0x1C 0x18 0x19 0x1A 0x35 0x16 0x33 0x34 0x26 0x2A 0xD1 0xE3 0xE4 0xE5 0xE6 0xE9 0xEA'
    },
    {
      type: 'Thermostat',
      name: 'RC20/Moduline 300 (DeviceID:0x17, ProductID:77, Version:03.03)',
      handlers: '0xA3 0x06 0xA2 0x12 0x91 0xA8'
    }
  ]
};

rest_server.post(API_ENDPOINT_ROOT, (req, res) => {
  console.log('Generic API POST');
  console.log(req.body);
  if (req.body.device === 'system') {
    if (req.body.entity === 'info') {
      console.log('sending system info: ' + JSON.stringify(emsesp_info));
    } else if (req.body.entity === 'settings') {
      console.log('sending system settings: ' + JSON.stringify(settings));
      res.json(settings);
    } else {
      res.sendStatus(200);
    }
  } else {
    res.sendStatus(200);
  }
});
rest_server.get(API_ENDPOINT_ROOT, (req, res) => {
  console.log('Generic API GET');
  res.sendStatus(200);
});

const SYSTEM_INFO_ENDPOINT = API_ENDPOINT_ROOT + 'system/info';
rest_server.post(SYSTEM_INFO_ENDPOINT, (req, res) => {
  console.log('System Info POST: ' + JSON.stringify(req.body));
  res.sendStatus(200);
});
rest_server.get(SYSTEM_INFO_ENDPOINT, (req, res) => {
  console.log('System Info GET');
  res.json(emsesp_info);
});

const GET_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'getSettings';
rest_server.get(GET_SETTINGS_ENDPOINT, (req, res) => {
  console.log('System Settings:');
  res.json(settings);
});

const GET_CUSTOMIZATIONS_ENDPOINT = REST_ENDPOINT_ROOT + 'getCustomizations';
rest_server.get(GET_CUSTOMIZATIONS_ENDPOINT, (req, res) => {
  console.log('Customization');
  // not implemented yet
  res.sendStatus(200);
});

const GET_SCHEDULE_ENDPOINT = REST_ENDPOINT_ROOT + 'schedule';
rest_server.get(GET_SCHEDULE_ENDPOINT, (req, res) => {
  console.log('Sending Schedule data');
  res.json(emsesp_schedule);
});

const GET_ENTITIES_ENDPOINT = REST_ENDPOINT_ROOT + 'entities';
rest_server.get(GET_ENTITIES_ENDPOINT, (req, res) => {
  console.log('Sending Entities data');
  res.json(emsesp_entities);
});

// start server
const expressServer = rest_server.listen(port, () =>
  console.log(`EMS-ESP REST API server running on http://localhost:${port}/`)
);

// event source
var count = 8;
var log_index = 0;
const ES_ENDPOINT_ROOT = '/es/';
const ES_LOG_ENDPOINT = ES_ENDPOINT_ROOT + 'log';
rest_server.get(ES_LOG_ENDPOINT, function (req, res) {
  res.setHeader('Content-Type', 'text/event-stream');
  res.setHeader('Cache-Control', 'no-cache');
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Connection', 'keep-alive');
  res.flushHeaders();

  var timer = setInterval(function () {
    count += 1;
    log_index += 1;
    const data = {
      t: '000+00:00:00.000',
      l: 3, // error
      i: count,
      n: 'system',
      m: 'incoming message #' + count + '/' + log_index
    };
    const sseFormattedResponse = `data: ${JSON.stringify(data)}\n\n`;
    res.write(sseFormattedResponse);
    res.flush(); // this is important

    // if buffer is full, start over
    if (log_index > 50) {
      fetch_log.events = [];
      log_index = 0;
    }
    fetch_log.events.push(data); // append to buffer
  }, 1000);
});
