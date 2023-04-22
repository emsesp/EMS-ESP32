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
const EMSESP_WRITE_VALUE_ENDPOINT = REST_ENDPOINT_ROOT + 'writeValue';
const EMSESP_WRITE_SENSOR_ENDPOINT = REST_ENDPOINT_ROOT + 'writeSensor';
const EMSESP_WRITE_ANALOG_ENDPOINT = REST_ENDPOINT_ROOT + 'writeAnalog';
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

const emsesp_devices = {
  devices: [
    {
      i: 1,
      s: 'Thermostat (RC20/Moduline 300)',
      t: 5,
      tn: 'thermostat'
    },
    {
      i: 2,
      s: 'Boiler (Nefit GBx72/Trendline/Cerapur/Greenstar Si/27i)',
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
      id: 2,
      t: 4,
      tn: 'Boiler',
      b: 'Nefit',
      n: 'GBx72/Trendline/Cerapur/Greenstar Si/27i',
      d: 8,
      p: 123,
      v: '06.01',
      e: 68
    },
    {
      id: 1,
      t: 5,
      tn: 'Thermostat',
      b: '',
      n: 'RC20/Moduline 300',
      d: 23,
      p: 77,
      v: '03.03',
      e: 5
    },
    {
      id: 4,
      t: 5,
      tn: 'Thermostat',
      b: 'Buderus',
      n: 'RC100/Moduline 1000/1010',
      d: 16,
      p: 165,
      v: '04.01',
      e: 3
    }
  ],
  s_n: 'Sensors',
  active_sensors: 8,
  analog_enabled: true
};

const emsesp_sensordata = {
  sensors: [
    { id: '28-233D-9497-0C03', n: 'Dallas 1', t: 25.7, o: 1.2, u: 1 },
    { id: '28-243D-7437-1E3A', n: 'Dallas 2 outside', t: 26.1, o: 0, u: 1 },
    { id: '28-243E-7437-1E3B', n: 'Zolder', t: 27.1, o: 0, u: 16 },
    { id: '28-183D-1892-0C33', n: 'Roof', o: 2, u: 1 }
  ],
  // sensors: [],
  analogs: [
    { id: '1', g: 36, n: 'motor', v: 0, u: 0, o: 17, f: 0, t: 0 },
    { id: '2', g: 37, n: 'External switch', v: 13, u: 0, o: 17, f: 0, t: 1 },
    { id: '3', g: 39, n: 'Pulse count', v: 144, u: 0, o: 0, f: 0, t: 2 },
    { id: '4', g: 40, n: 'Pressure', v: 16, u: 17, o: 0, f: 0, t: 3 }
  ]
  // analogs: [],
};

const status = {
  status: 0,
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
const emsesp_devicedata_1 = {
  label: 'Thermostat: RC20/Moduline 300',
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
      v: 18,
      u: 1,
      id: '00hc1 selected room temperature',
      c: 'hc1/seltemp'
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
      c: 'hc1/mode'
    }
  ]
};

const emsesp_devicedata_2 = {
  label: 'Boiler: Nefit GBx72/Trendline/Cerapur/Greenstar Si/27i',
  data: [
    { v: '', u: 0, id: '08reset', c: 'reset', l: ['-', 'maintenance', 'error'] },
    { v: 'false', u: 0, id: '08heating active' },
    { v: 'false', u: 0, id: '04tapwater active' },
    { v: 5, u: 1, id: '04selected flow temperature', c: 'selflowtemp' },
    { v: 0, u: 3, id: '0Eburner selected max power', c: 'selburnpow' },
    { v: 0, u: 3, id: '00heating pump modulation' },
    { v: 53.4, u: 1, id: '00current flow temperature' },
    { v: 52.7, u: 1, id: '00return temperature' },
    { v: 1.3, u: 10, id: '00system pressure' },
    { v: 54.9, u: 1, id: '00actual boiler temperature' },
    { v: 'false', u: 0, id: '00gas' },
    { v: 'false', u: 0, id: '00gas stage 2' },
    { v: 0, u: 9, id: '00flame current' },
    { v: 'false', u: 0, id: '00heating pump' },
    { v: 'false', u: 0, id: '00fan' },
    { v: 'false', u: 0, id: '00ignition' },
    { v: 'false', u: 0, id: '00oil preheating' },
    { v: 'true', u: 0, id: '00heating activated', c: 'heatingactivated', l: ['off', 'on'] },
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
    { v: 'true', u: 0, id: '00dhw turn on/off', c: 'wwtapactivated', l: ['off', 'on'] },
    { v: 62, u: 1, id: '00dhw set temperature' },
    { v: 60, u: 1, id: '00dhw selected temperature', c: 'wwseltemp' },
    { v: 'flow', u: 0, id: '00dhw type' },
    { v: 'hot', u: 0, id: '00dhw comfort', c: 'wwcomfort', l: ['hot', 'eco', 'intelligent'] },
    { v: 40, u: 2, id: '00dhw flow temperature offset', c: 'wwflowtempoffset' },
    { v: 100, u: 3, id: '00dhw max power', c: 'wwmaxpower' },
    { v: 'false', u: 0, id: '00dhw circulation pump available', c: 'wwcircpump', l: ['off', 'on'] },
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
    { v: 'false', u: 0, id: '00dhw circulation active', c: 'wwcirc', l: ['off', 'on'] },
    { v: 47.3, u: 1, id: '00dhw current intern temperature' },
    { v: 0, u: 4, id: '00dhw current tap water flow' },
    { v: 47.3, u: 1, id: '00dhw storage intern temperature' },
    { v: 'true', u: 0, id: '00dhw activated', c: 'wwactivated', l: ['off', 'on'] },
    { v: 'false', u: 0, id: '00dhw one time charging', c: 'wwonetime', l: ['off', 'on'] },
    { v: 'false', u: 0, id: '00dhw disinfecting', c: 'wwdisinfecting', l: ['off', 'on'] },
    { v: 'false', u: 0, id: '00dhw charging' },
    { v: 'false', u: 0, id: '00dhw recharging' },
    { v: 'true', u: 0, id: '00dhw temperature ok' },
    { v: 'false', u: 0, id: '00dhw active' },
    { v: 'true', u: 0, id: '00dhw 3way valve active' },
    { v: 0, u: 3, id: '00dhw set pump power' },
    { v: 288768, u: 0, id: '00dhw starts' },
    { v: 102151, u: 8, id: '00dhw active time' }
  ]
};

const emsesp_devicedata_4 = {
  label: 'Thermostat: RC100/Moduline 1000/1010',
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
      c: 'hc2/mode'
    }
  ]
};

// CUSTOM ENTITIES
let emsesp_entities = {
  entities: [
    {
      id: 0,
      name: 'test1',
      device_id: 8,
      type_id: 2,
      offset: 0,
      factor: 0,
      uom: 2,
      value: 1,
      value_type: 2,
      writeable: false
    },
    {
      id: 1,
      name: 'test2',
      device_id: 11,
      type_id: 222,
      offset: 2,
      factor: 2,
      uom: 4,
      value: 2,
      value_type: 5,
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

const emsesp_deviceentities_1 = [
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
    n: 'hc1 selected room temperature',
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

const emsesp_deviceentities_2 = [
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
  if (id === 1) {
    const encoded = msgpack.encode(emsesp_devicedata_1);
    res.write(encoded, 'binary');
    res.end(null, 'binary');
  }
  if (id === 2) {
    const encoded = msgpack.encode(emsesp_devicedata_2);
    res.write(encoded, 'binary');
    res.end(null, 'binary');
  }
  if (id === 4) {
    const encoded = msgpack.encode(emsesp_devicedata_4);
    res.write(encoded, 'binary');
    res.end(null, 'binary');
  }
});

rest_server.post(EMSESP_DEVICEENTITIES_ENDPOINT, (req, res) => {
  const id = req.body.id;
  if (id === 1) {
    const encoded = msgpack.encode(emsesp_deviceentities_1);
    res.write(encoded, 'binary');
    res.end(null, 'binary');
  }
  if (id === 2) {
    const encoded = msgpack.encode(emsesp_deviceentities_2);
    res.write(encoded, 'binary');
    res.end(null, 'binary');
  }
  if (id === 4) {
    const encoded = msgpack.encode(emsesp_deviceentities_4);
    res.write(encoded, 'binary');
    res.end(null, 'binary');
  }
});

function updateMask(entity, de, dd) {
  const current_mask = parseInt(entity.slice(0, 2), 16);

  // strip of any min/max ranges
  const shortname_with_customname = entity.slice(2).split('>')[0];

  const shortname = shortname_with_customname.split('|')[0];
  const new_custom_name = shortname_with_customname.split('|')[1];

  // find in de
  de_objIndex = de.findIndex((obj) => obj.id === shortname);
  if (de_objIndex !== -1) {
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
      console.log('comparing old ' + old_custom_name + ' with new ' + new_custom_name);
      if (old_custom_name !== new_custom_name) {
        changed = true;
        new_fullname = new_custom_name;
        console.log('name has changed to ' + new_custom_name);
      } else {
        new_fullname = fullname;
      }

      if (changed) {
        console.log(
          'Updating ' + dd.data[dd_objIndex].id + ' -> ' + current_mask.toString(16).padStart(2, '0') + new_fullname
        );
        de[de_objIndex].m = current_mask;
        de[de_objIndex].cn = new_fullname;
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
    if (id === 1) {
      updateMask(entity, emsesp_deviceentities_1, emsesp_devicedata_1);
    } else if (id === 2) {
      updateMask(entity, emsesp_deviceentities_2, emsesp_devicedata_2);
    } else if (id === 4) {
      updateMask(entity, emsesp_deviceentities_4, emsesp_devicedata_4);
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
    console.log('Write device value for Thermostat: ' + JSON.stringify(devicevalue));
    objIndex = emsesp_devicedata_1.data.findIndex((obj) => obj.c == devicevalue.c);
    emsesp_devicedata_1.data[objIndex] = devicevalue;
  }
  if (id === 2) {
    console.log('Write device value for Boiler: ' + JSON.stringify(devicevalue));
    objIndex = emsesp_devicedata_2.data.findIndex((obj) => obj.c == devicevalue.c);
    emsesp_devicedata_2.data[objIndex] = devicevalue;
  }
  if (id === 4) {
    console.log('Write device value for Thermostat2: ' + JSON.stringify(devicevalue));
    objIndex = emsesp_devicedata_4.data.findIndex((obj) => obj.c == devicevalue.c);
    emsesp_devicedata_4.data[objIndex] = devicevalue;
  }

  res.sendStatus(200);
});

rest_server.post(EMSESP_WRITE_SENSOR_ENDPOINT, (req, res) => {
  const sensor = req.body;
  console.log('Write sensor: ' + JSON.stringify(sensor));
  objIndex = emsesp_sensordata.sensors.findIndex((obj) => obj.id == sensor.id);
  if (objIndex !== -1) {
    emsesp_sensordata.sensors[objIndex].n = sensor.name;
    emsesp_sensordata.sensors[objIndex].o = sensor.offset;
  } else {
    console.log('not found');
  }
  res.sendStatus(200);
});

rest_server.post(EMSESP_WRITE_ANALOG_ENDPOINT, (req, res) => {
  const analog = req.body;
  console.log('Write analog: ' + JSON.stringify(analog));
  objIndex = emsesp_sensordata.analogs.findIndex((obj) => obj.g == analog.gpio);

  if (objIndex === -1) {
    console.log('new analog');
    emsesp_sensordata.analogs.push({
      id: analog.i.toString(),
      g: analog.gpio,
      n: analog.name,
      f: analog.factor,
      o: analog.offset,
      u: analog.uom,
      t: analog.type
    });
  } else {
    if (analog.type === -1) {
      console.log('removing analog gpio' + analog.gpio + ' index ' + objIndex);
      emsesp_sensordata.analogs[objIndex].t = -1;
    } else {
      console.log('updating analog gpio' + analog.gpio + ' index ' + objIndex);
      emsesp_sensordata.analogs[objIndex].n = analog.name;
      emsesp_sensordata.analogs[objIndex].f = analog.factor;
      emsesp_sensordata.analogs[objIndex].o = analog.offset;
      emsesp_sensordata.analogs[objIndex].u = analog.uom;
      emsesp_sensordata.analogs[objIndex].t = analog.type;
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
    'Dallas sensors': 3
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
    'Dallas reads': 22086,
    'Dallas fails': 0
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

    // if buffer full start over
    if (log_index > 50) {
      fetch_log.events = [];
      log_index = 0;
    }
    fetch_log.events.push(data); // append to buffer
  }, 1000);
});
