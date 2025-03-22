import { Encoder } from '@msgpack/msgpack';
import { AutoRouter, status } from 'itty-router';

const encoder = new Encoder();
const router = AutoRouter();

const REST_ENDPOINT_ROOT = '/rest/';
const API_ENDPOINT_ROOT = '/api/';
const GH_ENDPOINT_ROOT = '/gh/'; // for mock GitHub API for version checking

// HTTP HEADERS for msgpack
const headers = {
  'Access-Control-Allow-Origin': '*',
  'Content-type': 'application/msgpack'
};

// EMS-ESP Application Settings
let settings = {
  locale: 'en',
  tx_mode: 1,
  ems_bus_id: 11,
  syslog_enabled: false,
  syslog_level: 3,
  trace_raw: false,
  syslog_mark_interval: 0,
  syslog_host: '192.168.1.8',
  syslog_port: 514,
  boiler_heatingoff: false,
  remote_timeout: 24,
  remote_timeout_en: false,
  shower_timer: true,
  shower_alert: false,
  shower_alert_coldshot: 10,
  shower_alert_trigger: 7,
  shower_min_duration: 180,
  rx_gpio: 4,
  tx_gpio: 5,
  dallas_gpio: 14,
  dallas_parasite: false,
  led_gpio: 2,
  hide_led: true,
  low_clock: false,
  telnet_enabled: true,
  notoken_api: false,
  readonly_mode: false,
  analog_enabled: true,
  pbutton_gpio: 34,
  solar_maxflow: 30,
  board_profile: 'E32V2',
  fahrenheit: false,
  bool_format: 1,
  bool_dashboard: 1,
  enum_format: 1,
  weblog_level: 6,
  weblog_buffer: 50,
  weblog_compact: true,
  phy_type: 1,
  eth_power: 15,
  eth_phy_addr: 0,
  eth_clock_mode: 1,
  platform: 'ESP32',
  modbus_enabled: false,
  modbus_port: 502,
  modbus_max_clients: 10,
  modbus_timeout: 10000,
  developer_mode: true
};

// EMS-ESP System Settings
let system_status = {
  emsesp_version: 'XX.XX.XX', // defined later
  bus_status: 0,
  uptime: 77186,
  bus_uptime: 77121,
  num_devices: 2,
  num_sensors: 1,
  num_analogs: 1,
  free_heap: 143,
  ntp_status: 2,
  ntp_time: '2021-04-01T14:25:42Z',
  mqtt_status: true,
  ap_status: false,
  network_status: 3, // wifi connected
  // network_status: 10, // ethernet connected
  // network_status: 6, // wifi disconnected
  wifi_rssi: -41,
  esp_platform: 'ESP32S3',
  build_flags: 'DEMO',
  cpu_type: 'ESP32-S3',
  cpu_rev: 0,
  cpu_cores: 2,
  cpu_freq_mhz: 240,
  max_alloc_heap: 191,
  arduino_version: 'ESP32 Arduino v2.0.17',
  sdk_version: 'v4.4.7',
  partition: 'app0',
  flash_chip_size: 16384,
  flash_chip_speed: 80000000,
  app_used: 2258,
  app_free: 3438,
  fs_used: 24,
  fs_free: 2024,
  free_caps: 8376,
  psram: true,
  psram_size: 8189,
  free_psram: 8166,
  has_loader: true,
  model: '',
  // model: 'BBQKees Electronics EMS Gateway E32 V2 (E32 V2.0 P3/2024011)',
  // status: 0,
  status: 3
};

let VERSION_IS_UPGRADEABLE: boolean;

// Versions
// default - on latest stable, no stable upgrades
let THIS_VERSION = '3.7.2';
let LATEST_STABLE_VERSION = '3.7.2';
let LATEST_DEV_VERSION = '3.7.3-dev.9';

// scenarios for testing versioning
let version_test = 0;
version_test = 0; // on latest stable, no upgrades, but can switch
// version_test = 1; // on latest dev, no update
// version_test = 2; // on stable, upgrade stable to latest stable
// version_test = 3; // on dev, upgrade dev to latest dev

switch (version_test as number) {
  case 0:
  default:
    // use default - on latest stable, no upgrades, but can switch
    VERSION_IS_UPGRADEABLE = false;
    break;
  case 1:
    // on latest dev, no update
    THIS_VERSION = '3.7.2-dev.9';
    LATEST_STABLE_VERSION = '3.7.2';
    LATEST_DEV_VERSION = '3.7.3-dev.9';
    VERSION_IS_UPGRADEABLE = false;
    break;
  case 2:
    // upgrade stable to latest stable
    THIS_VERSION = '3.6.5';
    LATEST_STABLE_VERSION = '3.7.2';
    LATEST_DEV_VERSION = '3.7.3-dev.12';
    VERSION_IS_UPGRADEABLE = true;
    break;
  case 3:
    // upgrade dev to latest dev
    THIS_VERSION = '3.7.2-dev-1';
    LATEST_STABLE_VERSION = '3.7.2';
    LATEST_DEV_VERSION = '3.7.3-dev.12';
    VERSION_IS_UPGRADEABLE = true;
    break;
}

// set the version
system_status.emsesp_version = THIS_VERSION;

const emulate_esp = 'ESP32S3';
// const emulate_esp = 'ESP32';

switch (emulate_esp as string) {
  // ESP32 4MB
  case 'ESP32':
    system_status.esp_platform = 'ESP32';
    system_status.cpu_type = 'ESP32';
    system_status.arduino_version = 'Tasmota Arduino v2.0.17';
    system_status.sdk_version = 'v4.4.7';
    system_status.psram = false;
    system_status.psram_size = 0;
    system_status.free_psram = 0;
    settings.board_profile = 'E32V2';
    settings.platform = 'ESP32';
    break;

  // ESP32S3
  case 'ESP32S3':
  default:
    system_status.esp_platform = 'ESP32S3';
    system_status.cpu_type = 'ESP32-S3';
    system_status.arduino_version = 'ESP32 Arduino v2.0.18';
    system_status.sdk_version = 'v4.4.7';
    system_status.psram = true;
    system_status.psram_size = 8189;
    system_status.free_psram = 8166;
    settings.board_profile = 'S3';
    settings.platform = 'ESP32S3';
    break;
}

// simulate different ESP32 chips

// GLOBAL VARIABLES
let countWifiScanPoll = 0; // wifi network scan
let countHardwarePoll = 0; // for during an upload

// DeviceTypes
const enum DeviceType {
  SYSTEM = 0,
  TEMPERATURESENSOR,
  ANALOGSENSOR,
  SCHEDULER,
  CUSTOM,
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

const enum DeviceTypeUniqueID {
  SCHEDULER_UID = 96,
  ANALOGSENSOR_UID = 97,
  TEMPERATURESENSOR_UID = 98,
  CUSTOM_UID = 99 // always 99
}

function updateMask(entity: any, de: any, dd: any) {
  const current_mask = parseInt(entity.slice(0, 2), 16);

  // strip of any min/max ranges
  const shortname_with_customname = entity.slice(2).split('>')[0];
  const shortname = shortname_with_customname.split('|')[0];
  const new_custom_name = shortname_with_customname.split('|')[1];
  const has_min_max = entity.slice(2).split('>')[1];

  // find in de
  const de_objIndex = de.findIndex((obj: any) => obj.id === shortname);
  let fullname = '';
  let new_fullname = '';
  if (de_objIndex !== -1) {
    // get current name
    if (de[de_objIndex].cn) {
      fullname = de[de_objIndex].cn;
    } else {
      fullname = de[de_objIndex].n;
    }

    // find in dd, either looking for fullname or custom name
    // console.log('looking for ' + fullname + ' in ' + dd.nodes);
    const dd_objIndex = dd.nodes.findIndex(
      (obj: any) => obj.id.slice(2) === fullname
    );
    if (dd_objIndex !== -1) {
      let changed = new Boolean(false);

      // see if the mask has changed
      const old_mask = parseInt(dd.nodes[dd_objIndex].id.slice(0, 2), 16);
      if (old_mask !== current_mask) {
        changed = true;
        console.log('mask has changed to ' + current_mask.toString(16));
      }

      // see if the custom name has changed
      const old_custom_name = dd.nodes[dd_objIndex].cn;
      console.log(
        'comparing names, old (' +
        old_custom_name +
        ') with new (' +
        new_custom_name +
        ')'
      );
      if (old_custom_name !== new_custom_name) {
        changed = true;
        new_fullname = new_custom_name;
        console.log('name has changed to ' + new_custom_name);
      } else {
        new_fullname = fullname;
      }

      // see if min or max has changed
      // get current min/max values if they exist
      const current_min = dd.nodes[dd_objIndex].min;
      const current_max = dd.nodes[dd_objIndex].max;
      let new_min = current_min;
      let new_max = current_max;
      if (has_min_max) {
        new_min = parseInt(has_min_max.split('<')[0]);
        new_max = parseInt(has_min_max.split('<')[1]);
        if (current_min !== new_min || current_max !== new_max) {
          changed = true;
          console.log('min/max has changed to ' + new_min + '/' + new_max);
        }
      }

      if (changed === true) {
        de[de_objIndex].m = current_mask;
        de[de_objIndex].cn = new_fullname;
        if (new_min) {
          de[de_objIndex].mi = new_min;
        }
        if (new_max) {
          de[de_objIndex].ma = new_max;
        }
        dd.nodes[dd_objIndex].id =
          current_mask.toString(16).padStart(2, '0') + new_fullname;
        dd.nodes[dd_objIndex].cn = new_fullname;
      }
    }
  }
}

// called by Action endpoint
function export_data(type: string) {
  console.log('exporting ' + type + '...');
  switch (type) {
    case 'settings':
      return emsesp_info;
    case 'customizations':
      return emsesp_deviceentities_2; // fixed for one device
    case 'entities':
      return emsesp_customentities;
    case 'schedule':
      return emsesp_schedule;
    case 'modules':
      return emsesp_modules;
    case 'allvalues':
      return emsesp_allvalues;
    default:
      return status(404);
  }
}

// called by Action endpoint
function custom_support() {
  // return {};
  return {
    type: 'custom_support',
    Support: {
      html: [
        'This product is installed and managed by:',
        '',
        '<b>Bosch Installer Example</b>',
        '',
        'Nefit Road 12',
        '1234 AB Amsterdam',
        'Phone: +31 123 456 789',
        'email: support@boschinstaller.nl',
        '',
        "For help and questions please <a target='_blank' href='https://emsesp.org'>contact</a> your installer."
      ],
      img_url: 'https://docs.emsesp.org/_media/images/designer.png'
      // img_url: 'https://picsum.photos/200/300'
    }
  };
}

// called by Action endpoint
function check_upgrade(version: string) {
  let data = {};
  if (version) {
    const dev_version = version.split(',')[0];
    const stable_version = version.split(',')[1];
    console.log(
      'latest dev version: ' +
      dev_version +
      ', latest stable version: ' +
      stable_version
    );
    console.log(
      'Version upgrade check from version ' +
      THIS_VERSION +
      ', upgradable: ' +
      VERSION_IS_UPGRADEABLE
    );
    data = {
      emsesp_version: THIS_VERSION,
      upgradeable: VERSION_IS_UPGRADEABLE
    };
  } else {
    console.log('requesting ems-esp version (' + THIS_VERSION + ')');
    data = {
      emsesp_version: THIS_VERSION
    };
  }
  return data;
}

// LOG
const LOG_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'logSettings';
let log_settings = {
  level: 6,
  max_messages: 50,
  compact: true,
  psram: true,
  developer_mode: settings.developer_mode
};

// NTP
const NTP_STATUS_ENDPOINT = REST_ENDPOINT_ROOT + 'ntpStatus';
const NTP_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'ntpSettings';
const TIME_ENDPOINT = REST_ENDPOINT_ROOT + 'time';
let ntp_settings = {
  enabled: true,
  server: 'time.google.com',
  tz_label: 'Europe/Amsterdam',
  tz_format: 'CET-1CEST,M3.5.0,M10.5.0/3'
};
const ntp_status = {
  status: 2,
  utc_time: '2021-04-01T14:25:42Z',
  local_time: '2021-04-01T16:25:42',
  server: 'time.google.com',
  uptime: 856
};

// AP
const AP_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'apSettings';
const AP_STATUS_ENDPOINT = REST_ENDPOINT_ROOT + 'apStatus';
let ap_settings = {
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
const ap_status = {
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
let network_settings = {
  ssid: 'myWifi',
  password: 'myPassword',
  hostname: 'ems-esp',
  nosleep: true,
  tx_power: 0,
  bandwidth20: false,
  static_ip_config: false,
  enableMDNS: true,
  enableCORS: false,
  CORSOrigin: '*',
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
  dns_ip_2: '0.0.0.0',
  hostname: 'ems-esp',
  reconnect_count: 1
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

// MQTT
const MQTT_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'mqttSettings';
const MQTT_STATUS_ENDPOINT = REST_ENDPOINT_ROOT + 'mqttStatus';
let mqtt_settings = {
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
  publish_time_water: 60,
  mqtt_qos: 0,
  rootCA: '',
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

// STATUS
const ACTIVITY_ENDPOINT = REST_ENDPOINT_ROOT + 'activity';

// SETTINGS
const SYSTEM_STATUS_ENDPOINT = REST_ENDPOINT_ROOT + 'systemStatus';
const SECURITY_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'securitySettings';

// SYSTEM SIGNIN
const VERIFY_AUTHORIZATION_ENDPOINT = REST_ENDPOINT_ROOT + 'verifyAuthorization';
const SIGN_IN_ENDPOINT = REST_ENDPOINT_ROOT + 'signIn';
const GENERATE_TOKEN_ENDPOINT = REST_ENDPOINT_ROOT + 'generateToken';

let security_settings = {
  jwt_secret: 'naughty!',
  users: [
    { username: 'admin', password: 'admin', admin: true },
    { username: 'guest', password: 'guest', admin: false }
  ]
};

const verify_authentication = { access_token: '1234' };

const admin_signin = {
  access_token:
    'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6ImFkbWluIiwiYWRtaW4iOnRydWUsInZlcnNpb24iOiIzLjAuMmIwIn0.MsHSgoJKI1lyYz77EiT5ZN3ECMrb4mPv9FNy3udq0TU'
};
const guest_signin = {
  access_token:
    'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6Imd1ZXN0IiwiYWRtaW4iOmZhbHNlfQ.E_lylR_vGIQFZUGNwcl5F6OkHoaELGsC5zqhi0pAiJE'
};

// modify here to simulate admin and guest logins
const signin = admin_signin;
// const signin = guest_signin;

const generate_token = { token: '1234' };

const EMSESP_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'settings';
const EMSESP_CORE_DATA_ENDPOINT = REST_ENDPOINT_ROOT + 'coreData';
const EMSESP_SENSOR_DATA_ENDPOINT = REST_ENDPOINT_ROOT + 'sensorData';

const EMSESP_DEVICEDATA_ENDPOINT1 = REST_ENDPOINT_ROOT + 'deviceData';
const EMSESP_DEVICEDATA_ENDPOINT2 = REST_ENDPOINT_ROOT + 'deviceData/:id?';
const EMSESP_DEVICEENTITIES_ENDPOINT1 = REST_ENDPOINT_ROOT + 'deviceEntities';
const EMSESP_DEVICEENTITIES_ENDPOINT2 = REST_ENDPOINT_ROOT + 'deviceEntities/:id?';

const EMSESP_DASHBOARD_DATA_ENDPOINT = REST_ENDPOINT_ROOT + 'dashboardData';

const EMSESP_BOARDPROFILE_ENDPOINT = REST_ENDPOINT_ROOT + 'boardProfile';

const EMSESP_WRITE_DEVICEVALUE_ENDPOINT = REST_ENDPOINT_ROOT + 'writeDeviceValue';
const EMSESP_WRITE_DEVICENAME_ENDPOINT = REST_ENDPOINT_ROOT + 'writeDeviceName';
const EMSESP_WRITE_TEMPSENSOR_ENDPOINT =
  REST_ENDPOINT_ROOT + 'writeTemperatureSensor';
const EMSESP_WRITE_ANALOGSENSOR_ENDPOINT = REST_ENDPOINT_ROOT + 'writeAnalogSensor';
const EMSESP_CUSTOMIZATION_ENTITIES_ENDPOINT =
  REST_ENDPOINT_ROOT + 'customizationEntities';
const EMSESP_RESET_CUSTOMIZATIONS_ENDPOINT =
  REST_ENDPOINT_ROOT + 'resetCustomizations';

const EMSESP_SCHEDULE_ENDPOINT = REST_ENDPOINT_ROOT + 'schedule';
const EMSESP_CUSTOMENTITIES_ENDPOINT = REST_ENDPOINT_ROOT + 'customEntities';
const EMSESP_MODULES_ENDPOINT = REST_ENDPOINT_ROOT + 'modules';
const EMSESP_ACTION_ENDPOINT = REST_ENDPOINT_ROOT + 'action';

// these are used in the API calls only
const EMSESP_SYSTEM_INFO_ENDPOINT = API_ENDPOINT_ROOT + 'system/info';

const emsesp_info = {
  System: {
    version: THIS_VERSION,
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
      name: 'Nefit Trendline HRC30 (DeviceID:0x08 ProductID:123, Version:06.01)',
      handlers:
        '0x10 0x11 0xC2 0x14 0x15 0x1C 0x18 0x19 0x1A 0x35 0x16 0x33 0x34 0x26 0x2A 0xD1 0xE3 0xE4 0xE5 0xE6 0xE9 0xEA'
    },
    {
      type: 'Thermostat',
      name: 'RC20 (DeviceID:0x17, ProductID:77, Version:03.03)',
      handlers: '0xA3 0x06 0xA2 0x12 0x91 0xA8'
    }
  ]
};

const emsesp_allvalues = {
  'Boiler Nefit Trendline HRC30 (DeviceID:0x08, ProductID:123, Version:06.01)': {
    'force heating off': 'off',
    'heating active': 'off',
    'tapwater active': 'off',
    'selected flow temperature': 5,
    'heating pump modulation': 0,
    'current flow temperature': 41.4,
    'return temperature': 37.7,
    'system pressure': 1.3,
    'actual boiler temperature': 44.2,
    gas: 'off',
    'gas stage 2': 'off',
    'flame current': 0,
    fan: 'off',
    ignition: 'off',
    'oil preheating': 'off',
    'burner min power': 0,
    'burner max power': 50,
    'burner min period': 10,
    'hysteresis on temperature': -6,
    'hysteresis off temperature': 6,
    'heating activated': 'on',
    'heating temperature': 70,
    'heating pump': 'off',
    'boiler pump max power': 70,
    'boiler pump min power': 50,
    'boiler pump mode': 'proportional',
    'pump delay': 2,
    'burner selected max power': 0,
    'burner current power': 0,
    'burner starts': 394602,
    'total burner operating time': '480 days 4 hours 23 minutes',
    'burner stage 2 operating time': '0 days 0 hours 0 minutes',
    'total heat operating time': '395 days 2 hours 14 minutes',
    'burner starts heating': 46245,
    'total UBA operating time': '3932 days 23 hours 58 minutes',
    'last error code': '2E(207) 100.75.2000 65:00 (0 min)',
    'service code': '0H',
    'service code number': 203,
    'maintenance message': 'H00',
    'maintenance scheduled': 'manual',
    'time to next maintenance': 6000,
    'next maintenance date': '01.01.2012',
    'dhw turn on/off': 'on',
    'dhw set temperature': 62,
    'dhw selected temperature': 60,
    'dhw type': 'flow',
    'dhw comfort': 'hot',
    'dhw flow temperature offset': 40,
    'dhw max power': 100,
    'dhw circulation pump available': 'off',
    'dhw charging type': '3-way valve',
    'dhw hysteresis on temperature': -5,
    'dhw hysteresis off temperature': 0,
    'dhw disinfection temperature': 70,
    'dhw circulation pump mode': 'off',
    'dhw circulation active': 'off',
    'dhw current intern temperature': 33.5,
    'dhw current tap water flow': 0,
    'dhw storage intern temperature': 33.5,
    'dhw activated': 'on',
    'dhw one time charging': 'off',
    'dhw disinfecting': 'off',
    'dhw charging': 'off',
    'dhw recharging': 'off',
    'dhw temperature ok': 'on',
    'dhw active': 'off',
    'dhw 3-way valve active': 'on',
    'dhw set pump power': 0,
    'dhw starts': 348357,
    'dhw active time': '85 days 2 hours 9 minutes',
    'nominal Power': 30,
    'total energy': 3088.69,
    'energy heating': 2532.94,
    'dhw energy': 555.75
  },
  'Thermostat RC20 (DeviceID:0x17, ProductID:77, Version:03.03)': {
    'date/time': '10.12.2023 13:49',
    'hc1 how hot lounge should be': 19,
    'hc1 current room temp': 19.5,
    'hc1 mqtt discovery current room temperature': 'roomTemp',
    'hc1 mode': 'auto',
    'hc1 manual temperature': 21.5,
    'hc1 temperature when mode is off': 7,
    'hc1 day temperature T2': 20,
    'hc1 day temperature T3': 20,
    'hc1 day temperature T4': 20,
    'hc1 night temperature T1': 15,
    'hc1 program switchtime': '00 mo 00:00 T1'
  },
  'Controller Module BC10 (DeviceID:0x09, ProductID:190, Version:01.03)': {},
  'Custom Entities': {
    boiler_flowtemp: 5,
    nominalpower: 30,
    minmodulation: 23,
    maxmodulation: 115
  },
  'Analog Sensors': {},
  'Temperature Sensors': {
    zolder: 18.3
  }
};

const emsesp_coredata = {
  connected: true,
  // connected: false,
  // devices: [],
  devices: [
    {
      id: 7,
      t: 5,
      tn: 'Boiler',
      b: 'Nefit',
      n: 'Nefit Trendline HRC30/Generic Heatronic 3',
      d: 8,
      p: 123,
      v: '06.01',
      e: 69,
      url: 'boiler'
    },
    {
      id: 3,
      t: 5,
      tn: 'Boiler',
      b: 'Buderus',
      n: 'Buderus GB125',
      d: 8,
      p: 123,
      v: '06.01',
      e: 73,
      url: 'boiler'
    },
    {
      id: 1,
      t: 6,
      tn: 'Thermostat',
      b: 'Buderus',
      n: 'RC35',
      d: 24,
      p: 86,
      v: '04.01',
      e: 57,
      url: 'thermostat'
    },
    {
      id: 2,
      t: 6,
      tn: 'Thermostat',
      b: '',
      n: 'RC20',
      d: 23,
      p: 77,
      v: '03.03',
      e: 6,
      url: 'thermostat'
    },
    {
      id: 4,
      t: 6,
      tn: 'Thermostat',
      b: 'Nefit',
      n: 'Moduline 1000',
      d: 16,
      p: 165,
      v: '04.01',
      e: 3,
      url: 'thermostat'
    },
    {
      id: 5,
      t: 7,
      tn: 'Mixer Module',
      b: 'Buderus',
      n: 'MM10',
      d: 32,
      p: 69,
      v: '01.01',
      e: 6
    },
    {
      id: 6,
      t: 8,
      tn: 'Solar Module',
      b: 'Buderus',
      n: 'SM10',
      d: 48,
      p: 73,
      v: '01.02',
      e: 22,
      url: 'solar'
    },
    {
      id: 8,
      tn: 'Boiler/HP',
      t: 5,
      b: '',
      n: 'Bosch Compress 7000i AW Heat Pump',
      d: 8,
      p: 172,
      v: '01.20',
      e: 152,
      url: 'boiler'
    },
    {
      id: 9,
      tn: 'Thermostat',
      t: 6,
      b: '',
      n: 'RC100H',
      d: 56,
      p: 200,
      v: '40.07',
      e: 4,
      url: 'thermostat'
    },
    {
      id: 10,
      tn: 'Thermostat',
      t: 6,
      b: '',
      n: 'RC310',
      d: 16,
      p: 158,
      v: '73.03',
      e: 63,
      url: 'thermostat'
    },
    {
      id: 11,
      tn: 'Ventilation',
      t: 18,
      b: '',
      n: 'Vent4000CC',
      d: 81,
      p: 231,
      v: '53.02',
      e: 10,
      url: 'ventilation'
    }
  ]
};

const emsesp_coredata_custom = {
  id: 99,
  t: 4,
  tn: 'Custom',
  b: '',
  n: 'Custom Entities',
  d: 1,
  p: 1,
  v: '',
  e: 2
};

const emsesp_sensordata = {
  // ts: [],
  ts: [
    { id: '28-233D-9497-0C03', n: 'Dallas 1', t: 25.7, o: 1.2, u: 1 },
    { id: '28-243D-7437-1E3A', n: 'Dallas 2 outside', t: 26.1, o: 0, u: 1 },
    { id: '28-243E-7437-1E3B', n: 'Zolder', t: 27.1, o: 0, u: 1 },
    { id: '28-183D-1892-0C33', n: 'Roof', o: 2, u: 1 } // no temperature
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

const activity = {
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
// 1 - RC35 thermostat
// 2 - RC20 thermostat
// 3 - Buderus GB125 boiler
// 4 - Moduline 1000 thermostat
// 5 - MM10 mixer
// 6 - SM10 solar
// 7 - Nefit Trendline boiler
// 8 - Bosch Compress 7000i AW Heat Pump
// 9 - RC100H thermostat
// 10 - Thermostat RC310
// 11 - Ventilation
// 99 - Custom

const emsesp_devicedata_1 = {
  nodes: [
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
      s: 0.1
    },
    {
      v: -11,
      u: 1,
      id: '00minimal external temperature',
      c: 'minexttemp',
      m: -30,
      x: 0,
      s: 1
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
      c: 'dhw/mode',
      l: ['off', 'on', 'auto']
    },
    {
      v: 'off',
      u: 0,
      id: '00dhw circulation pump mode',
      c: 'dhw/circmode',
      l: ['off', 'on', 'auto']
    },
    {
      v: 'std prog',
      u: 0,
      id: '00dhw program',
      c: 'dhw/progmode',
      l: ['std prog', 'own prog']
    },
    {
      v: 'std prog',
      u: 0,
      id: '00dhw circulation program',
      c: 'dhw/circprog',
      l: ['std prog', 'own prog']
    },
    {
      v: 'off',
      u: 0,
      id: '00dhw disinfecting',
      c: 'dhw/disinfecting',
      l: ['off', 'on']
    },
    {
      v: 'tu',
      u: 0,
      id: '00dhw disinfection day',
      c: 'dhw/disinfectday',
      l: ['mo', 'tu', 'we', 'th', 'fr', 'sa', 'su', 'all']
    },
    {
      v: 1,
      u: 0,
      id: '00dhw disinfection hour',
      c: 'dhw/disinfecthour',
      m: 0,
      x: 23,
      s: 1
    },
    {
      v: 60,
      u: 1,
      id: '00dhw maximmu temperature',
      c: 'dhw/maxtemp',
      m: 60,
      x: 80,
      s: 1
    },
    {
      v: 'on',
      u: 0,
      id: '00dhw one time key function',
      c: 'dhw/onetimekey',
      l: ['off', 'on']
    },
    {
      v: '00 mo 06:00 on',
      u: 0,
      id: '00dhw program switchtime',
      c: 'dhw/switchtime',
      h: '<nn> [ not_set | day hh:mm on|off ]'
    },
    {
      v: '00 mo 06:30 on',
      u: 0,
      id: '00dhw circulation program switchtime',
      c: 'dhw/circswitchtime',
      h: '<nn> [ not_set | day hh:mm on|off ]'
    },
    {
      v: '01.01.2000-01.01.2000',
      u: 0,
      id: '00dhw holiday dates',
      c: 'dhw/holidays',
      h: 'dd.mm.yyyy-dd.mm.yyyy'
    },
    {
      v: '01.01.2019-12.01.2019',
      u: 0,
      id: '00dhw vacation dates',
      c: 'dhw/vacations',
      h: 'dd.mm.yyyy-dd.mm.yyyy'
    },
    {
      v: 21,
      u: 1,
      id: '00hc2 selected room temperature',
      c: 'hc2/seltemp',
      m: 0,
      x: 30,
      s: 0.5
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
      s: 0.5
    },
    {
      v: 17,
      u: 1,
      id: '00hc2 night temperature',
      c: 'hc2/nighttemp',
      m: 5,
      x: 30,
      s: 0.5
    },
    {
      v: 58,
      u: 1,
      id: '00hc2 design temperature',
      c: 'hc2/designtemp',
      m: 30,
      x: 90,
      s: 1
    },
    {
      v: 0,
      u: 2,
      id: '00hc2 offset temperature',
      c: 'hc2/offsettemp',
      m: -5,
      x: 5,
      s: 0.5
    },
    {
      v: 15,
      u: 1,
      id: '00hc2 holiday temperature',
      c: 'hc2/holidaytemp',
      m: 5,
      x: 30,
      s: 0.5
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
      s: 1
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
      s: 1
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
      s: 1
    },
    {
      v: 15,
      u: 1,
      id: '00hc2 min flow temperature',
      c: 'hc2/minflowtemp',
      m: 5,
      x: 70,
      s: 1
    },
    {
      v: 85,
      u: 1,
      id: '00hc2 max flow temperature',
      c: 'hc2/maxflowtemp',
      m: 30,
      x: 90,
      s: 1
    },
    {
      v: 0,
      u: 2,
      id: '00hc2 flow temperature offset for mixer',
      c: 'hc2/flowtempoffset',
      m: 0,
      x: 20,
      s: 1
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
      h: 'dd.mm.yyyy-dd.mm.yyyy'
    },
    {
      v: '01.01.2020-12.01.2020',
      u: 0,
      id: '00hc2 vacation dates',
      c: 'hc2/vacations',
      h: 'dd.mm.yyyy-dd.mm.yyyy'
    },
    {
      v: 'own 1',
      u: 0,
      id: '00hc2 program',
      c: 'hc2/program',
      l: [
        'own 1',
        'family',
        'morning',
        'evening',
        'am',
        'pm',
        'midday',
        'singles',
        'seniors',
        'new',
        'own 2'
      ]
    },
    {
      v: 0,
      u: 7,
      id: '00hc2 pause time',
      c: 'hc2/pause',
      m: 0,
      x: 99,
      s: 1
    },
    {
      v: 0,
      u: 7,
      id: '00hc2 party time',
      c: 'hc2/party',
      m: 0,
      x: 99,
      s: 1
    },
    {
      v: 0,
      u: 1,
      id: '00hc2 temporary set temperature automode',
      c: 'hc2/tempautotemp',
      m: 0,
      x: 30,
      s: 0.5
    },
    {
      v: -20,
      u: 1,
      id: '00hc2 no reduce below temperature',
      c: 'hc2/noreducetemp',
      m: -30,
      x: 10,
      s: 1
    },
    {
      v: 8,
      u: 1,
      id: '00hc2 off/reduce switch temperature',
      c: 'hc2/reducetemp',
      m: -20,
      x: 10,
      s: 1
    },
    {
      v: 5,
      u: 1,
      id: '00hc2 vacations off/reduce switch temperature',
      c: 'hc2/vacreducetemp',
      m: -20,
      x: 10,
      s: 1
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
  nodes: [
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
      id: '08Chosen Room Temperature',
      c: 'hc1/seltemp',
      m: 5,
      x: 52,
      s: 0.5
    },
    {
      v: 22.6,
      u: 1,
      id: '08hc1 current room temperature'
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
  nodes: [
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
      s: 1
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
      s: 1
    },
    {
      v: 100,
      u: 3,
      id: '00boiler pump max power',
      c: 'pumpmodmax',
      m: 0,
      x: 100,
      s: 1
    },
    {
      v: 100,
      u: 3,
      id: '00boiler pump min power',
      c: 'pumpmodmin',
      m: 0,
      x: 100,
      s: 1
    },
    {
      v: 'deltaP-2',
      u: 0,
      id: '00boiler pump mode',
      c: 'pumpmode',
      l: ['proportional', 'deltaP-1', 'deltaP-2', 'deltaP-3', 'deltaP-4']
    },
    {
      v: 'pressure3',
      u: 0,
      id: '00boiler pump characteristic',
      c: 'pumpcharacter',
      l: [
        'proportional',
        'pressure1',
        'pressure2',
        'pressure3',
        'pressure4',
        'pressure5',
        'pressure6'
      ]
    },
    {
      v: 6,
      u: 8,
      id: '00pump delay',
      c: 'pumpdelay',
      m: 0,
      x: 60,
      s: 1
    },
    {
      v: 15,
      u: 8,
      id: '00burner min period',
      c: 'burnminperiod',
      m: 0,
      x: 120,
      s: 1
    },
    {
      v: 0,
      u: 3,
      id: '00burner min power',
      c: 'burnminpower',
      m: 0,
      x: 100,
      s: 1
    },
    {
      v: 100,
      u: 3,
      id: '00burner max power',
      c: 'burnmaxpower',
      m: 0,
      x: 254,
      s: 1
    },
    {
      v: -8,
      u: 2,
      id: '00hysteresis on temperature',
      c: 'boilhyston',
      m: -20,
      x: 0,
      s: 1
    },
    {
      v: 15,
      u: 2,
      id: '00hysteresis off temperature',
      c: 'boilhystoff',
      m: 0,
      x: 20,
      s: 1
    },
    {
      v: -8,
      u: 2,
      id: '00hysteresis stage 2 on temperature',
      c: 'boil2hyston',
      m: -20,
      x: 0,
      s: 1
    },
    {
      v: 8,
      u: 2,
      id: '00hysteresis stage 2 off temperature',
      c: 'boil2hystoff',
      m: 0,
      x: 20,
      s: 1
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
      s: 1
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
      s: 1
    },
    {
      v: '30.06.2023',
      u: 0,
      id: '00next maintenance date',
      c: 'maintenancedate',
      h: 'dd.mm.yyyy'
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
      c: 'dhw/seltemp',
      m: 0,
      x: 254,
      s: 1
    },
    {
      v: 'buffer',
      u: 0,
      id: '00dhw type'
    },
    {
      v: 'hot',
      u: 0,
      id: '08dhw comfort',
      c: 'dhw/comfort',
      l: ['hot', 'eco', 'intelligent']
    },
    {
      v: 40,
      u: 2,
      id: '00dhw flow temperature offset',
      c: 'dhw/flowtempoffset',
      m: 0,
      x: 100,
      s: 1
    },
    {
      v: 'on',
      u: 0,
      id: '00dhw circulation pump available',
      c: 'dhw/circpump',
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
      c: 'dhw/hyston',
      m: -126,
      x: 126,
      s: 1
    },
    {
      v: -1,
      u: 2,
      id: '00dhw hysteresis off temperature',
      c: 'dhw/hystoff',
      m: -126,
      x: 126,
      s: 1
    },
    {
      v: 70,
      u: 1,
      id: '00dhw disinfection temperature',
      c: 'dhw/disinfectiontemp',
      m: 0,
      x: 254,
      s: 1
    },
    {
      v: 'continuous',
      u: 0,
      id: '00dhw circulation pump mode',
      c: 'dhw/circmode',
      l: [
        'off',
        '1x3min',
        '2x3min',
        '3x3min',
        '4x3min',
        '5x3min',
        '6x3min',
        'continuous'
      ]
    },
    {
      v: 'off',
      u: 0,
      id: '00dhw circulation active',
      c: 'dhw/circ',
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
      c: 'dhw/activated',
      l: ['off', 'on']
    },
    {
      v: 'off',
      u: 0,
      id: '00dhw one time charging',
      c: 'dhw/onetime',
      l: ['off', 'on']
    },
    {
      v: 'off',
      u: 0,
      id: '00dhw disinfecting',
      c: 'dhw/disinfecting',
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
  nodes: [
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
      id: '03hc2 mode',
      c: 'hc2/mode',
      l: ['off', 'on', 'auto']
    }
  ]
};

const emsesp_devicedata_5 = {
  nodes: [
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
      s: 1
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
      s: 10
    }
  ]
};

const emsesp_devicedata_6 = {
  nodes: [
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
      s: 1
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
      s: 1
    },
    {
      v: 10,
      u: 2,
      id: '00pump turn on difference',
      c: 'turnondiff',
      m: 0,
      x: 254,
      s: 1
    },
    {
      v: 5,
      u: 2,
      id: '00pump turn off difference',
      c: 'turnoffdiff',
      m: 0,
      x: 254,
      s: 1
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
      s: 0.1
    },
    {
      v: 37,
      u: 1,
      id: '00dhw minimum temperature',
      c: 'dhwmintemp',
      m: 0,
      x: 254,
      s: 1
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
      s: 1
    },
    {
      v: 2,
      u: 0,
      id: '00unknown setting 4',
      c: 'setting4',
      m: 0,
      x: 254,
      s: 1
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

const emsesp_devicedata_7 = {
  nodes: [
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
    {
      v: 'on',
      u: 0,
      id: '00heating activated',
      c: 'heatingactivated',
      l: ['off', 'on']
    },
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
    {
      v: 'manual',
      u: 0,
      id: '00maintenance scheduled',
      c: 'maintenance',
      l: ['off', 'time', 'date', 'manual']
    },
    { v: 6000, u: 7, id: '00time to next maintenance', c: 'maintenancetime' },
    {
      v: '01.01.2012',
      u: 0,
      id: '00next maintenance date',
      c: 'maintenancedate',
      h: 'dd.mm.yyyy'
    },
    {
      v: 'on',
      u: 0,
      id: '00dhw turn on/off',
      c: 'dhw/tapactivated',
      l: ['off', 'on']
    },
    { v: 62, u: 1, id: '00dhw set temperature' },
    { v: 60, u: 1, id: '00dhw selected temperature', c: 'dhw/seltemp' },
    { v: 'flow', u: 0, id: '00dhw type' },
    {
      v: 'hot',
      u: 0,
      id: '00dhw comfort',
      c: 'dhw/comfort',
      l: ['hot', 'eco', 'intelligent']
    },
    { v: 40, u: 2, id: '00dhw flow temperature offset', c: 'dhw/flowtempoffset' },
    { v: 100, u: 3, id: '00dhw max power', c: 'dhw/maxpower' },
    {
      v: 'off',
      u: 0,
      id: '00dhw circulation pump available',
      c: 'dhw/circpump',
      l: ['off', 'on']
    },
    { v: '3-way valve', u: 0, id: '00dhw charging type' },
    { v: -5, u: 2, id: '00dhw hysteresis on temperature', c: 'dhw/hyston' },
    { v: 0, u: 2, id: '00dhw hysteresis off temperature', c: 'dhw/hystoff' },
    { v: 70, u: 1, id: '00dhw disinfection temperature', c: 'dhw/disinfectiontemp' },
    {
      v: 'off',
      u: 0,
      id: '00dhw circulation pump mode',
      c: 'dhw/circmode',
      l: [
        'off',
        '1x3min',
        '2x3min',
        '3x3min',
        '4x3min',
        '5x3min',
        '6x3min',
        'continuous'
      ]
    },
    {
      v: 'off',
      u: 0,
      id: '00dhw circulation active',
      c: 'dhw/circ',
      l: ['off', 'on']
    },
    { v: 47.3, u: 1, id: '00dhw current intern temperature' },
    { v: 0, u: 4, id: '00dhw current tap water flow' },
    { v: 47.3, u: 1, id: '00dhw storage intern temperature' },
    { v: 'on', u: 0, id: '00dhw activated', c: 'dhw/activated', l: ['off', 'on'] },
    {
      v: 'off',
      u: 0,
      id: '00dhw one time charging',
      c: 'dhw/onetime',
      l: ['off', 'on']
    },
    {
      v: 'off',
      u: 0,
      id: '00dhw disinfecting',
      c: 'dhw/disinfecting',
      l: ['off', 'on']
    },
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

const emsesp_devicedata_8 = {
  nodes: [
    {
      v: '',
      u: 0,
      id: '00reset',
      c: 'reset',
      l: ['-', 'maintenance', 'error', 'history', 'message']
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
      v: 19,
      u: 1,
      id: '00selected flow temperature',
      c: 'selflowtemp',
      m: 0,
      x: 90,
      s: '1'
    },
    {
      v: 0,
      u: 3,
      id: '00heating pump modulation'
    },
    {
      v: 30.29,
      u: 1,
      id: '00outside temperature'
    },
    {
      v: 18.7,
      u: 1,
      id: '00current flow temperature'
    },
    {
      v: 21.39,
      u: 1,
      id: '00return temperature'
    },
    {
      v: 18.7,
      u: 1,
      id: '00low loss header'
    },
    {
      v: 'on',
      u: 0,
      id: '00heating activated',
      c: 'heatingactivated',
      l: ['off', 'on']
    },
    {
      v: 85,
      u: 1,
      id: '00heating temperature',
      c: 'heatingtemp',
      m: 0,
      x: 90,
      s: '1'
    },
    {
      v: 'on',
      u: 0,
      id: '00heating pump'
    },
    {
      v: 44,
      u: 3,
      id: '00burner selected max power',
      c: 'selburnpow',
      m: 0,
      x: 254,
      s: '1'
    },
    {
      v: 45,
      u: 3,
      id: '00burner current power'
    },
    {
      v: 0,
      u: 0,
      id: '00burner starts'
    },
    {
      v: 0,
      u: 8,
      id: '00total burner operating time'
    },
    {
      v: 0,
      u: 8,
      id: '00burner stage 2 operating time'
    },
    {
      v: 0,
      u: 8,
      id: '00total heat operating time'
    },
    {
      v: 0,
      u: 0,
      id: '00burner starts heating'
    },
    {
      v: 764598,
      u: 8,
      id: '00total UBA operating time'
    },
    {
      v: 'A01(5453) 01.06.2023 09:23 - 01.06.2023 09:31',
      u: 0,
      id: '00last error code'
    },
    {
      v: ' 0H',
      u: 0,
      id: '00service code'
    },
    {
      v: 203,
      u: 0,
      id: '00service code number'
    },
    {
      v: 'off',
      u: 0,
      id: '00emergency operation',
      c: 'emergencyops',
      l: ['off', 'on']
    },
    {
      v: 0,
      u: 1,
      id: '00emergency temperature',
      c: 'emergencytemp',
      m: 0,
      x: 70,
      s: '1'
    },
    {
      v: 763575,
      u: 8,
      id: '00heatpump total uptime'
    },
    {
      v: 309983,
      u: 8,
      id: '00total operating time heat'
    },
    {
      v: 203673,
      u: 8,
      id: '00operating time compressor heating'
    },
    {
      v: 29682,
      u: 8,
      id: '00operating time compressor cooling'
    },
    {
      v: 76627,
      u: 8,
      id: '00dhw operating time compressor'
    },
    {
      v: 0,
      u: 8,
      id: '00operating time compressor pool'
    },
    {
      v: 2054,
      u: 0,
      id: '00total compressor control starts'
    },
    {
      v: 1287,
      u: 0,
      id: '00heating control starts'
    },
    {
      v: 160,
      u: 0,
      id: '00cooling control starts'
    },
    {
      v: 607,
      u: 0,
      id: '00dhw control starts2'
    },
    {
      v: 0,
      u: 0,
      id: '00pool control starts'
    },
    {
      v: 5368,
      u: 5,
      id: '00total energy consumption'
    },
    {
      v: 5336,
      u: 5,
      id: '00total energy consumption compressor'
    },
    {
      v: 2977,
      u: 5,
      id: '00energy consumption compressor heating'
    },
    {
      v: 2047,
      u: 5,
      id: '00dhw energy consumption compressor'
    },
    {
      v: 295,
      u: 5,
      id: '00energy consumption compressor cooling'
    },
    {
      v: 0,
      u: 5,
      id: '00energy consumption compressor pool'
    },
    {
      v: 32,
      u: 5,
      id: '00total aux elec. heater energy consumption'
    },
    {
      v: 0,
      u: 5,
      id: '00aux elec. heater energy consumption heating'
    },
    {
      v: 32,
      u: 5,
      id: '00dhw aux elec. heater energy consumption'
    },
    {
      v: 0,
      u: 5,
      id: '00aux elec. heater energy consumption pool'
    },
    {
      v: 19052,
      u: 5,
      id: '00total energy supplied'
    },
    {
      v: 11053,
      u: 5,
      id: '00total energy supplied heating'
    },
    {
      v: 5978,
      u: 5,
      id: '00dhw total energy warm supplied'
    },
    {
      v: 2019,
      u: 5,
      id: '00total energy supplied cooling'
    },
    {
      v: 0,
      u: 5,
      id: '00total energy supplied pool'
    },
    {
      v: 4.5,
      u: 11,
      id: '00compressor power output'
    },
    {
      v: 100,
      u: 3,
      id: '00compressor max power',
      c: 'hpmaxpower',
      m: 0,
      x: 100,
      s: '1'
    },
    {
      v: 'off',
      u: 0,
      id: '00hp compressor'
    },
    {
      v: 'cooling',
      u: 0,
      id: '00compressor activity'
    },
    {
      v: 0,
      u: 3,
      id: '00brine pump speed'
    },
    {
      v: 'off',
      u: 0,
      id: '00switch valve'
    },
    {
      v: 45,
      u: 3,
      id: '00compressor speed'
    },
    {
      v: 33,
      u: 3,
      id: '00circulation pump speed'
    },
    {
      v: 0,
      u: 1,
      id: '00brine in/evaporator'
    },
    {
      v: 0,
      u: 1,
      id: '00brine out/condenser'
    },
    {
      v: 21.39,
      u: 1,
      id: '00heat carrier return (TC0)'
    },
    {
      v: 17.5,
      u: 1,
      id: '00heat carrier forward (TC1)'
    },
    {
      v: 18,
      u: 1,
      id: '00condenser temperature (TC3)'
    },
    {
      v: 51.59,
      u: 1,
      id: '00compressor temperature (TR1)'
    },
    {
      v: 14.6,
      u: 1,
      id: '00refrigerant temperature liquid side (condenser output) (TR3)'
    },
    {
      v: 37,
      u: 1,
      id: '00evaporator inlet temperature (TR4)'
    },
    {
      v: 20.2,
      u: 1,
      id: '00compressor inlet temperature (TR5)'
    },
    {
      v: 54.59,
      u: 1,
      id: '00compressor outlet temperature (TR6)'
    },
    {
      v: 29.6,
      u: 1,
      id: '00air inlet temperature (TL2)'
    },
    {
      v: 13.89,
      u: 1,
      id: '00low pressure side temperature (PL1)'
    },
    {
      v: 37.79,
      u: 1,
      id: '00high pressure side temperature (PH1)'
    },
    {
      v: 25.6,
      u: 1,
      id: '00drain pan temp (TA4)'
    },
    {
      v: 58.5,
      u: 1,
      id: '00reservoir temp (TW1)'
    },
    {
      v: 28,
      u: 1,
      id: '00pool set temperature',
      c: 'poolsettemp',
      m: 0,
      x: 127,
      s: '0.5'
    },
    {
      v: 'cooling & defrost',
      u: 0,
      id: '004-way valve (VR4)'
    },
    {
      v: 'off',
      u: 0,
      id: '00input 1 state'
    },
    {
      v: '000000000000000',
      u: 0,
      id: '00input 1 options',
      c: 'hpin1opt',
      h: '<inv>[<evu1><evu2><evu3><comp><aux><cool><heat><dhw><pv>]'
    },
    {
      v: 'off',
      u: 0,
      id: '00input 2 state'
    },
    {
      v: '000000000000000',
      u: 0,
      id: '00input 2 options',
      c: 'hpin2opt',
      h: '<inv>[<evu1><evu2><evu3><comp><aux><cool><heat><dhw><pv>]'
    },
    {
      v: 'off',
      u: 0,
      id: '00input 3 state'
    },
    {
      v: '000000000000000',
      u: 0,
      id: '00input 3 options',
      c: 'hpin3opt',
      h: '<inv>[<evu1><evu2><evu3><comp><aux><cool><heat><dhw><pv>]'
    },
    {
      v: 'off',
      u: 0,
      id: '00input 4 state'
    },
    {
      v: '000000000000',
      u: 0,
      id: '00input 4 options',
      c: 'hpin4opt',
      h: '<inv>[<comp><aux><cool><heat><dhw><pv>]'
    },
    {
      v: '6 kW',
      u: 0,
      id: '00heat limit compressor',
      c: 'maxheatcomp',
      l: ['0 kW', '2 kW', '3 kW', '4 kW', '6 kW', '9 kW']
    },
    {
      v: '9 kW',
      u: 0,
      id: '00heat limit heating',
      c: 'maxheatheat',
      l: ['0 kW', '2 kW', '3 kW', '4 kW', '6 kW', '9 kW']
    },
    {
      v: '9 kW',
      u: 0,
      id: '00dhw heat limit',
      c: 'dhw/maxheat',
      l: ['0 kW', '2 kW', '3 kW', '4 kW', '6 kW', '9 kW']
    },
    {
      v: 'on',
      u: 0,
      id: '00manual defrost',
      c: 'mandefrost',
      l: ['off', 'on']
    },
    {
      v: 'off',
      u: 0,
      id: '00cooling only with PV',
      c: 'pvcooling',
      l: ['off', 'on']
    },
    {
      v: 'off',
      u: 0,
      id: '00aux heater only',
      c: 'auxheateronly',
      l: ['off', 'on']
    },
    {
      v: 'off',
      u: 0,
      id: '00disable aux heater',
      c: 'auxheateroff',
      l: ['off', 'on']
    },
    {
      v: 0,
      u: 3,
      id: '00aux heater status'
    },
    {
      v: 500,
      u: 21,
      id: '00aux heater on delay',
      c: 'auxheaterdelay',
      m: 10,
      x: 1000,
      s: '10'
    },
    {
      v: 0,
      u: 22,
      id: '00aux heater max limit',
      c: 'auxmaxlimit',
      m: 0,
      x: 10,
      s: '0.1'
    },
    {
      v: 0.1,
      u: 22,
      id: '00aux heater limit start',
      c: 'auxlimitstart',
      m: 0,
      x: 10,
      s: '0.1'
    },
    {
      v: 'eco',
      u: 0,
      id: '00aux heater mode',
      c: 'auxheatrmode',
      l: ['eco', 'comfort']
    },
    {
      v: 0,
      u: 21,
      id: '00on/off hyst heat',
      c: 'hphystheat',
      m: 0,
      x: 1500,
      s: '5'
    },
    {
      v: 0,
      u: 21,
      id: '00on/off hyst cool',
      c: 'hphystcool',
      m: 0,
      x: 1500,
      s: '5'
    },
    {
      v: 1125,
      u: 21,
      id: '00on/off hyst pool',
      c: 'hphystpool',
      m: 50,
      x: 1500,
      s: '5'
    },
    {
      v: 'auto',
      u: 0,
      id: '00silent mode',
      c: 'silentmode',
      l: ['off', 'auto', 'on']
    },
    {
      v: 1320,
      u: 8,
      id: '00silent mode from',
      c: 'silentfrom',
      m: 0,
      x: 3810,
      s: '15'
    },
    {
      v: 360,
      u: 8,
      id: '00silent mode to',
      c: 'silentto',
      m: 0,
      x: 3810,
      s: '15'
    },
    {
      v: -10,
      u: 1,
      id: '00min outside temp for silent mode',
      c: 'mintempsilent',
      m: -126,
      x: 126,
      s: '1'
    },
    {
      v: -5,
      u: 1,
      id: '00outside temp parallel mode',
      c: 'tempparmode',
      m: -126,
      x: 126,
      s: '1'
    },
    {
      v: 0,
      u: 3,
      id: '00aux heater mixing valve'
    },
    {
      v: 7,
      u: 22,
      id: '00temp diff TC3/TC0 heat',
      c: 'tempdiffheat',
      m: 2,
      x: 10,
      s: '0.1'
    },
    {
      v: 3,
      u: 22,
      id: '00temp diff TC3/TC0 cool',
      c: 'tempdiffcool',
      m: 2,
      x: 10,
      s: '0.1'
    },
    {
      v: 'off',
      u: 0,
      id: '00valve/pump cooling',
      c: 'vpcooling',
      l: ['off', 'on']
    },
    {
      v: 'off',
      u: 0,
      id: '00heating cable',
      c: 'heatcable',
      l: ['off', 'on']
    },
    {
      v: 'off',
      u: 0,
      id: '00VC0 valve',
      c: 'vc0valve',
      l: ['off', 'on']
    },
    {
      v: 'off',
      u: 0,
      id: '00primary heatpump',
      c: 'primepump',
      l: ['off', 'on']
    },
    {
      v: 0,
      u: 3,
      id: '00primary heatpump modulation',
      c: 'primepumpmod',
      m: 0,
      x: 100,
      s: '1'
    },
    {
      v: 'off',
      u: 0,
      id: '003-way valve',
      c: 'hp3way',
      l: ['off', 'on']
    },
    {
      v: 'off',
      u: 0,
      id: '00el. heater step 1',
      c: 'elheatstep1',
      l: ['off', 'on']
    },
    {
      v: 'off',
      u: 0,
      id: '00el. heater step 2',
      c: 'elheatstep2',
      l: ['off', 'on']
    },
    {
      v: 'off',
      u: 0,
      id: '00el. heater step 3',
      c: 'elheatstep3',
      l: ['off', 'on']
    },
    {
      v: 'off',
      u: 0,
      id: '00condensate reservoir heating (EA0)'
    },
    {
      v: 'auto',
      u: 0,
      id: '00primary heatpump mode',
      c: 'hppumpmode',
      l: ['auto', 'continuous']
    },
    {
      v: '',
      u: 0,
      id: '00shutdown',
      c: 'shutdown',
      l: ['off', 'on']
    },
    {
      v: 'on',
      u: 0,
      id: '00dhw alternating operation',
      c: 'dhw/alternatingop',
      l: ['off', 'on']
    },
    {
      v: 35,
      u: 8,
      id: '00dhw prioritise heating during dhw',
      c: 'dhw/altopprioheat',
      m: 20,
      x: 120,
      s: '1'
    },
    {
      v: 45,
      u: 8,
      id: '00dhw prioritise dhw during heating',
      c: 'dhw/altopprio',
      m: 30,
      x: 120,
      s: '1'
    },
    {
      v: 57,
      u: 1,
      id: '00dhw comfort switch off',
      c: 'dhw/comfoff',
      m: 15,
      x: 65,
      s: '1'
    },
    {
      v: 54,
      u: 1,
      id: '00dhw eco switch off',
      c: 'dhw/ecooff',
      m: 15,
      x: 65,
      s: '1'
    },
    {
      v: 50,
      u: 1,
      id: '00dhw eco+ switch off',
      c: 'dhw/ecoplusoff',
      m: 48,
      x: 63,
      s: '1'
    },
    {
      v: 'off',
      u: 0,
      id: '00dhw circulation pump available during dhw',
      c: 'dhw/hpcircpump',
      l: ['off', 'on']
    },
    {
      v: 54,
      u: 1,
      id: '00dhw set temperature'
    },
    {
      v: 45,
      u: 1,
      id: '00dhw selected temperature',
      c: 'dhw/seltemp',
      m: 0,
      x: 254,
      s: '1'
    },
    {
      v: 42,
      u: 1,
      id: '00dhw selected lower temperature',
      c: 'dhw/seltemplow',
      m: 0,
      x: 254,
      s: '1'
    },
    {
      v: 65,
      u: 1,
      id: '00dhw single charge temperature',
      c: 'dhw/seltempsingle',
      m: 0,
      x: 254,
      s: '1'
    },
    {
      v: 'high comfort',
      u: 0,
      id: '00dhw comfort mode',
      c: 'dhw/comfort1',
      l: ['high comfort', 'eco']
    },
    {
      v: 25,
      u: 2,
      id: '00dhw flow temperature offset',
      c: 'dhw/flowtempoffset',
      m: 0,
      x: 100,
      s: '1'
    },
    {
      v: 56,
      u: 1,
      id: '00dhw maximum temperature',
      c: 'dhw/maxtemp',
      m: 0,
      x: 80,
      s: '1'
    },
    {
      v: 'on',
      u: 0,
      id: '00dhw circulation pump available',
      c: 'dhw/circpump',
      l: ['off', 'on']
    },
    {
      v: -2,
      u: 2,
      id: '00dhw hysteresis on temperature',
      c: 'dhw/hyston',
      m: -126,
      x: 126,
      s: '1'
    },
    {
      v: 0,
      u: 2,
      id: '00dhw hysteresis off temperature',
      c: 'dhw/hystoff',
      m: -126,
      x: 126,
      s: '1'
    },
    {
      v: 65,
      u: 1,
      id: '00dhw disinfection temperature',
      c: 'dhw/disinfectiontemp',
      m: 60,
      x: 80,
      s: '1'
    },
    {
      v: '3x3min',
      u: 0,
      id: '00dhw circulation pump mode',
      c: 'dhw/circmode',
      l: [
        'off',
        '1x3min',
        '2x3min',
        '3x3min',
        '4x3min',
        '5x3min',
        '6x3min',
        'continuous'
      ]
    },
    {
      v: 'off',
      u: 0,
      id: '00dhw circulation active',
      c: 'dhw/circ',
      l: ['off', 'on']
    },
    {
      v: 58.7,
      u: 1,
      id: '00dhw current intern temperature'
    },
    {
      v: 58.5,
      u: 1,
      id: '00dhw current extern temperature'
    },
    {
      v: 0,
      u: 4,
      id: '00dhw current tap water flow'
    },
    {
      v: 'on',
      u: 0,
      id: '00dhw activated',
      c: 'dhw/activated',
      l: ['off', 'on']
    },
    {
      v: 'off',
      u: 0,
      id: '00dhw one time charging',
      c: 'dhw/onetime',
      l: ['off', 'on']
    },
    {
      v: 'off',
      u: 0,
      id: '00dhw disinfecting',
      c: 'dhw/disinfecting',
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
      v: 'off',
      u: 0,
      id: '00dhw temperature ok'
    },
    {
      v: 'off',
      u: 0,
      id: '00dhw 3-way valve active'
    },
    {
      v: 0,
      u: 0,
      id: '00dhw starts'
    },
    {
      v: 0,
      u: 8,
      id: '00dhw active time'
    }
  ]
};

const emsesp_devicedata_9 = {
  nodes: [
    {
      v: 24,
      u: 1,
      id: '00hc1 room temperature from remote'
    },
    {
      v: 14,
      u: 1,
      id: '00hc1 dew point temperature'
    },
    {
      v: 46,
      u: 3,
      id: '00hc1 relative air humidity'
    },
    {
      v: 0,
      u: 2,
      id: '00hc1 internal temperature offset',
      c: 'hc1/intoffset',
      m: -12,
      x: 12,
      s: '0.1'
    }
  ]
};

const emsesp_devicedata_10 = {
  nodes: [
    {
      v: '26.06.2024 14:49',
      u: 0,
      id: '00date/time',
      c: 'datetime',
      h: 'NTP | dd.mm.yyyy-hh:mm:ss-day(0-6)-dst(0/1)'
    },
    {
      v: 0,
      u: 2,
      id: '00internal temperature offset',
      c: 'intoffset',
      m: -12,
      x: 12,
      s: '0.1'
    },
    {
      v: 'off',
      u: 0,
      id: '00floor drying'
    },
    {
      v: 30.299999237060547,
      u: 1,
      id: '00damped outdoor temperature'
    },
    {
      v: 0,
      u: 1,
      id: '00floor drying temperature'
    },
    {
      v: 'heavy',
      u: 0,
      id: '00building type',
      c: 'building',
      l: ['light', 'medium', 'heavy']
    },
    {
      v: -12,
      u: 1,
      id: '00minimal external temperature',
      c: 'minexttemp',
      m: -126,
      x: 126,
      s: '1'
    },
    {
      v: 'off',
      u: 0,
      id: '00damping outdoor temperature',
      c: 'damping',
      l: ['off', 'on']
    },
    {
      v: 3.299999952316284,
      u: 0,
      id: '00energy cost ratio',
      c: 'energycostratio',
      m: 0,
      x: 20,
      s: '0.1'
    },
    {
      v: 'off',
      u: 0,
      id: '00enable raise dhw',
      c: 'pvenabledhw',
      l: ['off', 'on']
    },
    {
      v: 3,
      u: 22,
      id: '00raise heating with PV',
      c: 'pvraiseheat',
      m: 0,
      x: 5,
      s: '1'
    },
    {
      v: 0,
      u: 22,
      id: '00lower cooling with PV',
      c: 'pvlowercool',
      m: -5,
      x: 0,
      s: '1'
    },
    {
      v: 21,
      u: 1,
      id: '00hc1 selected room temperature',
      c: 'hc1/seltemp',
      m: 0,
      x: 30,
      s: '0.5'
    },
    {
      v: 24,
      u: 1,
      id: '00hc1 current room temperature'
    },
    {
      v: 'roomTemp',
      u: 0,
      id: '00hc1 mqtt discovery current room temperature'
    },
    {
      v: 'manual',
      u: 0,
      id: '00hc1 mode',
      c: 'hc1/mode',
      l: ['manual', 'auto']
    },
    {
      v: 'comfort',
      u: 0,
      id: '00hc1 mode type'
    },
    {
      v: 15,
      u: 1,
      id: '00hc1 eco temperature',
      c: 'hc1/ecotemp',
      m: 0,
      x: 127,
      s: '0.5'
    },
    {
      v: 22.5,
      u: 1,
      id: '00hc1 manual temperature',
      c: 'hc1/manualtemp',
      m: 0,
      x: 127,
      s: '0.5'
    },
    {
      v: 19,
      u: 1,
      id: '00hc1 comfort temperature',
      c: 'hc1/comforttemp',
      m: 0,
      x: 127,
      s: '0.5'
    },
    {
      v: 11,
      u: 1,
      id: '00hc1 summer temperature',
      c: 'hc1/summertemp',
      m: 10,
      x: 30,
      s: '1'
    },
    {
      v: 35,
      u: 1,
      id: '00hc1 design temperature',
      c: 'hc1/designtemp',
      m: 0,
      x: 254,
      s: '1'
    },
    {
      v: 0,
      u: 2,
      id: '00hc1 offset temperature',
      c: 'hc1/offsettemp',
      m: -126,
      x: 126,
      s: '1'
    },
    {
      v: 22,
      u: 1,
      id: '00hc1 min flow temperature',
      c: 'hc1/minflowtemp',
      m: 0,
      x: 254,
      s: '1'
    },
    {
      v: 60,
      u: 1,
      id: '00hc1 max flow temperature',
      c: 'hc1/maxflowtemp',
      m: 0,
      x: 254,
      s: '1'
    },
    {
      v: 0,
      u: 2,
      id: '00hc1 room influence',
      c: 'hc1/roominfluence',
      m: 0,
      x: 254,
      s: '1'
    },
    {
      v: 4,
      u: 0,
      id: '00hc1 room influence factor',
      c: 'hc1/roominflfactor',
      m: 0,
      x: 25,
      s: '0.1'
    },
    {
      v: 0,
      u: 2,
      id: '00hc1 current room influence'
    },
    {
      v: 'outdoor',
      u: 0,
      id: '00hc1 nofrost mode',
      c: 'hc1/nofrostmode',
      l: ['room', 'outdoor', 'room outdoor']
    },
    {
      v: 2,
      u: 1,
      id: '00hc1 nofrost temperature',
      c: 'hc1/nofrosttemp',
      m: -126,
      x: 126,
      s: '1'
    },
    {
      v: 19,
      u: 1,
      id: '00hc1 target flow temperature'
    },
    {
      v: 'floor',
      u: 0,
      id: '00hc1 heating type',
      c: 'hc1/heatingtype',
      l: ['off', 'radiator', 'convector', 'floor']
    },
    {
      v: 'auto',
      u: 0,
      id: '00hc1 heatpump operating mode',
      c: 'hc1/hpoperatingmode',
      l: ['off', 'auto', 'heating', 'cooling']
    },
    {
      v: 'cooling',
      u: 0,
      id: '00hc1 heatpump operating state'
    },
    {
      v: 'weather compensated',
      u: 0,
      id: '00hc1 control mode',
      c: 'hc1/controlmode',
      l: [
        'weather compensated',
        'outside basepoint',
        'n/a',
        'room',
        'power',
        'constant'
      ]
    },
    {
      v: 'prog 1',
      u: 0,
      id: '00hc1 program',
      c: 'hc1/program',
      l: ['prog 1', 'prog 2']
    },
    {
      v: -1,
      u: 1,
      id: '00hc1 temporary set temperature automode',
      c: 'hc1/tempautotemp',
      m: -1,
      x: 30,
      s: '0.5'
    },
    {
      v: 21,
      u: 1,
      id: '00hc1 temporary set temperature from remote'
    },
    {
      v: 0,
      u: 3,
      id: '00hc1 fast heatup',
      c: 'hc1/fastheatup',
      m: 0,
      x: 100,
      s: '1'
    },
    {
      v: 'off',
      u: 0,
      id: '00hc1 switch-on optimization',
      c: 'hc1/switchonoptimization',
      l: ['off', 'on']
    },
    {
      v: 'reduce',
      u: 0,
      id: '00hc1 reduce mode',
      c: 'hc1/reducemode',
      l: ['outdoor', 'room', 'reduce']
    },
    {
      v: -31,
      u: 1,
      id: '00hc1 no reduce below temperature',
      c: 'hc1/noreducetemp',
      m: -126,
      x: 126,
      s: '1'
    },
    {
      v: 5,
      u: 1,
      id: '00hc1 off/reduce switch temperature',
      c: 'hc1/reducetemp',
      m: -126,
      x: 126,
      s: '1'
    },
    {
      v: 'off',
      u: 0,
      id: '00hc1 dhw priority',
      c: 'hc1/dhwprio',
      l: ['off', 'on']
    },
    {
      v: 'on',
      u: 0,
      id: '00hc1 cooling'
    },
    {
      v: 'heating&cooling',
      u: 0,
      id: '00hc1 HP Mode',
      c: 'hc1/hpmode',
      l: ['heating', 'cooling', 'heating&cooling']
    },
    {
      v: 5,
      u: 22,
      id: '00hc1 dew point offset',
      c: 'hc1/dewoffset',
      m: 2,
      x: 10,
      s: '1'
    },
    {
      v: 1,
      u: 22,
      id: '00hc1 room temp difference',
      c: 'hc1/roomtempdiff',
      m: 0,
      x: 254,
      s: '1'
    },
    {
      v: 11,
      u: 1,
      id: '00hc1 HP min. flow temp.',
      c: 'hc1/hpminflowtemp',
      m: 0,
      x: 254,
      s: '1'
    },
    {
      v: 'RC100H',
      u: 0,
      id: '00hc1 control device',
      c: 'hc1/control',
      l: ['RC310', 'RC200', 'RC100', 'RC100H', 'TC100']
    },
    {
      v: 1,
      u: 7,
      id: '00hc1 heat-on delay',
      c: 'hc1/heatondelay',
      m: 1,
      x: 48,
      s: '1'
    },
    {
      v: 1,
      u: 7,
      id: '00hc1 heat-off delay',
      c: 'hc1/heatoffdelay',
      m: 1,
      x: 48,
      s: '1'
    },
    {
      v: 1,
      u: 22,
      id: '00hc1 instant start',
      c: 'hc1/instantstart',
      m: 1,
      x: 10,
      s: '1'
    },
    {
      v: 'normal',
      u: 0,
      id: '00dhw mode',
      c: 'dhw/mode',
      l: ['off', 'normal', 'comfort', 'auto', 'own prog']
    },
    {
      v: 'on',
      u: 0,
      id: '00dhw circulation pump mode',
      c: 'dhw/circmode',
      l: ['off', 'on', 'auto', 'own prog']
    },
    {
      v: 15,
      u: 8,
      id: '00dhw charge duration',
      c: 'dhw/chargeduration',
      m: 0,
      x: 3810,
      s: '15'
    },
    {
      v: 'off',
      u: 0,
      id: '00dhw charge',
      c: 'dhw/charge',
      l: ['off', 'on']
    },
    {
      v: 0,
      u: 1,
      id: '00dhw extra'
    },
    {
      v: 'off',
      u: 0,
      id: '00dhw disinfecting',
      c: 'dhw/disinfecting',
      l: ['off', 'on']
    },
    {
      v: 'tu',
      u: 0,
      id: '00dhw disinfection day',
      c: 'dhw/disinfectday',
      l: ['mo', 'tu', 'we', 'th', 'fr', 'sa', 'su', 'all']
    },
    {
      v: 120,
      u: 8,
      id: '00dhw disinfection time',
      c: 'dhw/disinfecttime',
      m: 0,
      x: 1431,
      s: '15'
    },
    {
      v: 'off',
      u: 0,
      id: '00dhw daily heating',
      c: 'dhw/dailyheating',
      l: ['off', 'on']
    },
    {
      v: 120,
      u: 8,
      id: '00dhw daily heating time',
      c: 'dhw/dailyheattime',
      m: 0,
      x: 1431,
      s: '15'
    }
  ]
};

const emsesp_devicedata_11 = {
  nodes: [
    {
      v: 11,
      u: 1,
      id: '00outdoor fresh air'
    },
    {
      v: 13.699999809265137,
      u: 1,
      id: '00indoor fresh air'
    },
    {
      v: 11.399999618530273,
      u: 1,
      id: '00outdoor exhaust air'
    },
    {
      v: 14.800000190734863,
      u: 1,
      id: '00indoor exhaust air'
    },
    {
      v: 0,
      u: 3,
      id: '00in blower speed'
    },
    {
      v: 0,
      u: 3,
      id: '00out blower speed'
    },
    {
      v: 'auto',
      u: 0,
      id: '00ventilation mode',
      c: 'ventmode',
      l: [
        'auto',
        'off',
        'L1',
        'L2',
        'L3',
        'L4',
        'demand',
        'sleep',
        'intense',
        'bypass',
        'party',
        'fireplace'
      ]
    },
    {
      v: 1770,
      u: 0,
      id: '00air quality (voc)'
    },
    {
      v: 53,
      u: 3,
      id: '00relative air humidity'
    }
  ]
};

const emsesp_devicedata_99 = {
  nodes: [
    {
      v: 5,
      u: 1,
      id: '00boiler_flowtemp',
      c: 'boiler_flowtemp'
    },
    {
      v: 0,
      u: 0,
      id: '00Extra1'
    }
  ]
};

// CUSTOM ENTITIES
let emsesp_customentities = {
  // entities: []
  entities: [
    {
      id: 0,
      ram: 0,
      device_id: 8,
      type_id: 24,
      offset: 0,
      factor: 1,
      name: 'boiler_flowtemp',
      uom: 1,
      value_type: 1,
      writeable: true,
      value: 30
    },
    {
      id: 1,
      ram: 0,
      device_id: 16,
      type_id: 797,
      offset: 0,
      factor: 1,
      name: 'Extra1',
      uom: 0,
      value_type: 0,
      writeable: false,
      value: 0
    },
    {
      id: 2,
      ram: 1,
      device_id: 0,
      type_id: 0,
      offset: 0,
      factor: 1,
      name: 'setpoint',
      uom: 1,
      value_type: 0,
      writeable: true,
      value: 21
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
    },
    {
      id: 5,
      active: false,
      flags: 130,
      time: 'system/network info/rssi < -70',
      cmd: 'system/restart',
      value: '',
      name: 'bad_wifi'
    },
    {
      id: 6,
      active: false,
      flags: 129,
      time: 'boiler/outdoortemp',
      cmd: 'boiler/selflowtemp',
      value: '(custom/setpoint - boiler/outdoortemp) * 2.8 + 3',
      name: 'heatingcurve'
    },
    {
      id: 7,
      active: false,
      flags: 132,
      time: '',
      cmd: 'system/message',
      value: '"hello world"',
      name: '' // empty
    }
  ]
};

// MODULES
let emsesp_modules = {
  // 'modules': []
  modules: [
    {
      id: 1,
      key: 'ModuleTest1',
      name: 'Module Test 1',
      author: 'proddy',
      version: '1.0.0',
      enabled: true,
      status: 1,
      message: 'Running',
      license: '1234567890'
    },
    {
      id: 2,
      key: 'ModuleTest2',
      name: 'Module Test 2',
      author: 'proddy',
      version: '1.0.0',
      enabled: true,
      status: 2,
      message: 'Running',
      license: 'ABCDEFGHIJKL'
    }
  ]
};

// CUSTOMIZATION
const dummy_deviceentities = [
  {
    v: 'unknown',
    n: 'sorry, no demo entities for this device!',
    id: 'unknown',
    m: 0,
    w: false
  }
];

// no data for these
const emsesp_deviceentities_1 = dummy_deviceentities;
const emsesp_deviceentities_3 = dummy_deviceentities;
const emsesp_deviceentities_5 = dummy_deviceentities;
const emsesp_deviceentities_6 = dummy_deviceentities;
const emsesp_deviceentities_8 = dummy_deviceentities;
const emsesp_deviceentities_9 = dummy_deviceentities;
const emsesp_deviceentities_10 = dummy_deviceentities;
const emsesp_deviceentities_11 = dummy_deviceentities;
const emsesp_deviceentities_none = dummy_deviceentities;

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
    m: 8,
    mi: 5,
    ma: 52,
    w: true
  },
  {
    v: 22.6,
    n: 'hc1 current room temperature',
    id: 'hc1/curtemp',
    m: 8,
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
    m: 3,
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
  {
    v: '1C(210) 06.06.2020 12:07 (0 min)',
    n: 'last error code',
    id: 'lastcode',
    m: 0,
    w: false
  },
  { v: '0H', n: 'service code', id: 'servicecode', m: 0, w: false },
  { v: 203, n: 'service code number', id: 'servicecodenumber', m: 0, w: false },
  { v: 'H00', n: 'maintenance message', id: 'maintenancemessage', m: 0, w: false },
  { v: 'manual', n: 'maintenance scheduled', id: 'maintenance', m: 0, w: false },
  { v: 6000, n: 'time to next maintenance', id: 'maintenancetime', m: 0, w: false },
  {
    v: '01.01.2012',
    n: 'next maintenance date',
    id: 'maintenancedate',
    m: 0,
    w: false
  },
  { v: true, n: 'dhw turn on/off', id: 'dhw/tapactivated', m: 0, w: false },
  { v: 62, n: 'dhw set temperature', id: 'dhw/settemp', m: 0, w: false },
  { v: 60, n: 'dhw selected temperature', id: 'dhw/seltemp', m: 0, w: true },
  { n: 'dhw selected lower temperature', id: 'dhw/seltemplow', m: 2 },
  { n: 'dhw selected temperature for off', id: 'dhw/seltempoff', m: 2 },
  { n: 'dhw single charge temperature', id: 'dhw/seltempsingle', m: 2 },
  { v: 'flow', n: 'dhw type', id: 'dhw/type', m: 0, w: false },
  { v: 'hot', n: 'dhw comfort', id: 'dhw/comfort', m: 0, w: false },
  {
    v: 40,
    n: 'dhw flow temperature offset',
    id: 'dhw/flowtempoffset',
    m: 0,
    w: false
  },
  { v: 100, n: 'dhw max power', id: 'dhw/maxpower', m: 0, w: false },
  {
    v: false,
    n: 'dhw circulation pump available',
    id: 'dhw/circpump',
    m: 0,
    w: false
  },
  { v: '3-way valve', n: 'dhw charging type', id: 'dhw/chargetype', m: 0, w: false },
  { v: -5, n: 'dhw hysteresis on temperature', id: 'dhw/hyston', m: 0, w: false },
  { v: 0, n: 'dhw hysteresis off temperature', id: 'dhw/hystoff', m: 0, w: false },
  {
    v: 70,
    n: 'dhw disinfection temperature',
    id: 'dhw/disinfectiontemp',
    m: 0,
    w: false
  },
  { v: 'off', n: 'dhw circulation pump mode', id: 'dhw/circmode', m: 0, w: false },
  { v: false, n: 'dhw circulation active', id: 'dhw/circ', m: 0, w: false },
  {
    v: 46.4,
    n: 'dhw current intern temperature',
    id: 'dhw/curtemp',
    m: 0,
    w: false
  },
  { n: 'dhw current extern temperature', id: 'dhw/curtemp2', m: 2 },
  { v: 0, n: 'dhw current tap water flow', id: 'dhw/curflow', m: 0, w: false },
  {
    v: 46.3,
    n: 'dhw storage intern temperature',
    id: 'dhw/storagetemp1',
    m: 0,
    w: false
  },
  { n: 'dhw storage extern temperature', id: 'dhw/storagetemp2', m: 2 },
  { v: true, n: 'dhw activated', id: 'dhw/activated', m: 0, w: false },
  { v: false, n: 'dhw one time charging', id: 'dhw/onetime', m: 0, w: false },
  { v: false, n: 'dhw disinfecting', id: 'dhw/disinfecting', m: 0, w: false },
  { v: false, n: 'dhw charging', id: 'dhw/charging', m: 0, w: false },
  { v: false, n: 'dhw recharging', id: 'dhw/recharging', m: 0, w: false },
  { v: true, n: 'dhw temperature ok', id: 'dhw/tempok', m: 0, w: false },
  { v: false, n: 'dhw active', id: 'dhw/active', m: 0, w: false },
  { v: true, n: 'dhw 3way valve active', id: 'dhw/3wayvalve', m: 0, w: false },
  { v: 0, n: 'dhw set pump power', id: 'dhw/setpumppower', m: 0, w: true },
  { n: 'dhw mixer temperature', id: 'dhw/mixertemp', m: 2 },
  { n: 'dhw cylinder middle temperature (TS3)', id: 'dhw/cylmiddletemp', m: 2 },
  { v: 288768, n: 'dhw starts', id: 'dhw/starts', m: 0, w: false },
  { v: 102151, n: 'dhw active time', id: 'dhw/workm', m: 0, w: false }
];

// LOG
router
  .get(LOG_SETTINGS_ENDPOINT, () => log_settings)
  .post(LOG_SETTINGS_ENDPOINT, async (request: any) => {
    log_settings = await request.json();
    console.log('log settings saved', log_settings);
    return status(200);
  });

// NETWORK
router
  .get(NETWORK_STATUS_ENDPOINT, () => network_status)
  .get(NETWORK_SETTINGS_ENDPOINT, () => network_settings)
  .get(LIST_NETWORKS_ENDPOINT, () => {
    if (countWifiScanPoll++ === 3) {
      console.log('done, sending list');
      return list_networks; // send list
    } else {
      console.log('...waiting #' + countWifiScanPoll);
      return status(200);
    }
  })
  .get(SCAN_NETWORKS_ENDPOINT, () => {
    console.log('start scan networks');
    countWifiScanPoll = 0; // stop the poll
    return status(202);
  })
  .post(NETWORK_SETTINGS_ENDPOINT, async (request: any) => {
    network_settings = await request.json();
    console.log('network settings saved', network_settings);
    return status(200);
  });

// AP
router
  .get(AP_SETTINGS_ENDPOINT, () => ap_settings)
  .get(AP_STATUS_ENDPOINT, () => ap_status)
  .post(AP_SETTINGS_ENDPOINT, async (request: any) => {
    ap_settings = await request.json();
    console.log('ap settings saved', ap_settings);
    return status(200);
  });

// MQTT
router
  .get(MQTT_SETTINGS_ENDPOINT, () => mqtt_settings)
  .get(MQTT_STATUS_ENDPOINT, () => mqtt_status)
  .post(MQTT_SETTINGS_ENDPOINT, async (request: any) => {
    mqtt_settings = await request.json();
    console.log('mqtt settings saved', mqtt_settings);
    return status(200);
  });

// NTP
router
  .get(NTP_SETTINGS_ENDPOINT, () => ntp_settings)
  .get(NTP_STATUS_ENDPOINT, () => ntp_status)
  .post(TIME_ENDPOINT, () => status(200))
  .post(NTP_SETTINGS_ENDPOINT, async (request: any) => {
    ntp_settings = await request.json();
    console.log('ntp settings saved', ntp_settings);
    return status(200);
  });

// SYSTEM and SETTINGS
router
  .get(ACTIVITY_ENDPOINT, () => activity)
  .get(SYSTEM_STATUS_ENDPOINT, () => {
    if (countHardwarePoll >= 2) {
      countHardwarePoll = 0;
      system_status.status = 0; // SYSTEM_STATUS_NORMAL
    }
    countHardwarePoll++;

    return system_status;
  })
  .get(SECURITY_SETTINGS_ENDPOINT, () => security_settings)
  .post(SECURITY_SETTINGS_ENDPOINT, async (request: any) => {
    security_settings = await request.json();
    console.log('security settings saved', security_settings);
    return status(200);
  })
  .get(VERIFY_AUTHORIZATION_ENDPOINT, () => verify_authentication)
  .post(SIGN_IN_ENDPOINT, () => signin)
  .get(GENERATE_TOKEN_ENDPOINT, () => generate_token);

//
//  EMS-ESP Project stuff
//

function deviceData(id: number) {
  if (id == 1) {
    return new Response(encoder.encode(emsesp_devicedata_1), { headers });
  }
  if (id == 2) {
    return new Response(encoder.encode(emsesp_devicedata_2), { headers });
  }
  if (id == 3) {
    return new Response(encoder.encode(emsesp_devicedata_3), { headers });
  }
  if (id == 4) {
    return new Response(encoder.encode(emsesp_devicedata_4), { headers });
  }
  if (id == 5) {
    return new Response(encoder.encode(emsesp_devicedata_5), { headers });
  }
  if (id == 6) {
    return new Response(encoder.encode(emsesp_devicedata_6), { headers });
  }
  if (id == 7) {
    return new Response(encoder.encode(emsesp_devicedata_7), { headers });
  }
  if (id == 8) {
    // test changing the selected flow temp on a Bosch Compress 7000i AW Heat Pump (Boiler/HP)
    emsesp_devicedata_8.nodes[4].v = Math.floor(Math.random() * 100);
    return new Response(encoder.encode(emsesp_devicedata_8), { headers });
  }
  if (id == 9) {
    return new Response(encoder.encode(emsesp_devicedata_9), { headers });
  }
  if (id == 10) {
    return new Response(encoder.encode(emsesp_devicedata_10), { headers });
  }
  if (id == 11) {
    return new Response(encoder.encode(emsesp_devicedata_11), { headers });
  }
  if (id == 99) {
    return new Response(encoder.encode(emsesp_devicedata_99), { headers });
  }
}

function deviceEntities(id: number) {
  if (id == 1) {
    return new Response(encoder.encode(emsesp_deviceentities_1), { headers });
  }
  if (id == 2) {
    return new Response(encoder.encode(emsesp_deviceentities_2), { headers });
  }
  if (id == 3) {
    return new Response(encoder.encode(emsesp_deviceentities_3), { headers });
  }
  if (id == 4) {
    return new Response(encoder.encode(emsesp_deviceentities_4), { headers });
  }
  if (id == 5) {
    return new Response(encoder.encode(emsesp_deviceentities_5), { headers });
  }
  if (id == 6) {
    return new Response(encoder.encode(emsesp_deviceentities_6), { headers });
  }
  if (id == 7) {
    return new Response(encoder.encode(emsesp_deviceentities_7), { headers });
  }
  if (id == 8) {
    return new Response(encoder.encode(emsesp_deviceentities_8), { headers });
  }
  if (id == 9) {
    return new Response(encoder.encode(emsesp_deviceentities_9), { headers });
  }
  if (id == 10) {
    return new Response(encoder.encode(emsesp_deviceentities_10), { headers });
  }
  // not found, return empty
  return new Response(encoder.encode(emsesp_deviceentities_none), { headers });
}

// prepare dashboard data
function getDashboardEntityData(id: number) {
  let device_data = {};
  if (id == 1) device_data = emsesp_devicedata_1;
  else if (id == 2) device_data = emsesp_devicedata_2;
  else if (id == 3) device_data = emsesp_devicedata_3;
  else if (id == 4) device_data = emsesp_devicedata_4;
  else if (id == 5) device_data = emsesp_devicedata_5;
  else if (id == 6) device_data = emsesp_devicedata_6;
  else if (id == 7) device_data = emsesp_devicedata_7;
  else if (id == 8) device_data = emsesp_devicedata_8;
  else if (id == 9) device_data = emsesp_devicedata_9;
  else if (id == 10) device_data = emsesp_devicedata_10;
  else if (id == 11) device_data = emsesp_devicedata_11;
  else if (id == 99) device_data = emsesp_devicedata_99;

  // filter device_data on
  //  - only add favorite (mask has bit 8 set) except for Custom Entities (type 99)
  let new_data = (device_data as any).nodes
    .filter((item) => id === 99 || parseInt(item.id.slice(0, 2), 16) & 0x08)
    .map((item, index) => ({
      id: id * 100 + index, // mandatory unique id for table
      dv: item // devicevalue
    }));

  return new_data;
}

// Router routing starts here...
router
  // EMS-ESP Settings
  .get(EMSESP_SETTINGS_ENDPOINT, () => settings)
  .post(EMSESP_SETTINGS_ENDPOINT, async (request: any) => {
    settings = await request.json();
    console.log('application settings saved', settings);
    return status(200); // no restart needed
    // return status(205); // restart needed
  })

  // Device Data
  .get(EMSESP_CORE_DATA_ENDPOINT, () => {
    // sort by type, like its done in the C++ code
    let sorted_devices = [...emsesp_coredata.devices].sort((a, b) => a.t - b.t);
    // append emsesp_coredata to sorted_devices so Custom is always at the end of the list
    sorted_devices.push(emsesp_coredata_custom);
    // sorted_devices = []; // uncomment if simulating no devices...
    return { connected: true, devices: sorted_devices };
  })
  .get(EMSESP_SENSOR_DATA_ENDPOINT, () => {
    // random change the zolder temperature 0-100
    emsesp_sensordata.ts[2].t = Math.floor(Math.random() * 100);
    return emsesp_sensordata;
  })
  .get(EMSESP_DEVICEDATA_ENDPOINT1, (request) =>
    request.query.id ? deviceData(Number(request.query.id)) : status(404)
  )
  .get(EMSESP_DEVICEDATA_ENDPOINT2, ({ params }) =>
    params.id ? deviceData(Number(params.id)) : status(404)
  )
  .get(EMSESP_DEVICEENTITIES_ENDPOINT1, (request) =>
    request.query.id ? deviceEntities(Number(request.query.id)) : status(404)
  )
  .get(EMSESP_DEVICEENTITIES_ENDPOINT2, ({ params }) =>
    params.id ? deviceEntities(Number(params.id)) : status(404)
  )
  .get(EMSESP_DASHBOARD_DATA_ENDPOINT, () => {
    let dashboard_nodes: { id?: number; n?: string; t?: number; nodes?: any[] }[] =
      [];
    let dashboard_object: { id?: number; n?: string; t?: number; nodes?: any[] } =
      {};

    let fake = false;

    // fake = true; // for testing, shows a subset of data

    if (!fake) {
      // pick EMS devices from coredata
      for (const element of emsesp_coredata.devices) {
        const id = element.id;

        dashboard_object = {
          id: id,
          n: element.n,
          t: element.t,
          nodes: getDashboardEntityData(id)
        };
        // only add to dashboard if we have values
        if ((dashboard_object.nodes ?? []).length > 0) {
          dashboard_nodes.push(dashboard_object);
        }
      }

      // add the custom entity data
      dashboard_object = {
        id: DeviceTypeUniqueID.CUSTOM_UID, // unique ID for custom entities
        t: DeviceType.CUSTOM,
        nodes: getDashboardEntityData(99)
      };
      // only add to dashboard if we have values
      if ((dashboard_object.nodes ?? []).length > 0) {
        dashboard_nodes.push(dashboard_object);
      }

      // add temperature sensor data. no command c
      let sensor_data: any[] = [];
      sensor_data = emsesp_sensordata.ts.map((item, index) => ({
        id: DeviceTypeUniqueID.TEMPERATURESENSOR_UID * 100 + index,
        dv: {
          id: '00' + item.n,
          v: item.t, // value is called t in ts (temperature)
          u: item.u
        }
      }));
      dashboard_object = {
        id: DeviceTypeUniqueID.TEMPERATURESENSOR_UID,
        t: DeviceType.TEMPERATURESENSOR,
        nodes: sensor_data
      };
      // only add to dashboard if we have values
      if ((dashboard_object.nodes ?? []).length > 0) {
        dashboard_nodes.push(dashboard_object);
      }

      // add analog sensor data. no command c
      // remove disabled sensors first (t = 0)
      sensor_data = emsesp_sensordata.as.filter((item) => item.t !== 0);
      sensor_data = sensor_data.map((item, index) => ({
        id: DeviceTypeUniqueID.ANALOGSENSOR_UID * 100 + index,
        dv: {
          id: '00' + item.n,
          v: item.v,
          u: item.u
        }
      }));
      dashboard_object = {
        id: DeviceTypeUniqueID.ANALOGSENSOR_UID,
        t: DeviceType.ANALOGSENSOR,
        nodes: sensor_data
      };
      // only add to dashboard if we have values
      if ((dashboard_object.nodes ?? []).length > 0) {
        dashboard_nodes.push(dashboard_object);
      }

      // add the scheduler data
      // filter emsesp_schedule with only if it has a name
      let scheduler_data = emsesp_schedule.schedule.filter((item) => item.name);
      let scheduler_data2 = scheduler_data.map((item, index) => ({
        id: DeviceTypeUniqueID.SCHEDULER_UID * 100 + index,
        dv: {
          id: '00' + item.name,
          v: item.active ? 'on' : 'off',
          c: item.name,
          l: ['off', 'on']
        }
      }));
      dashboard_object = {
        id: DeviceTypeUniqueID.SCHEDULER_UID,
        t: DeviceType.SCHEDULER,
        nodes: scheduler_data2
      };
      // only add to dashboard if we have values
      if ((dashboard_object.nodes ?? []).length > 0) {
        dashboard_nodes.push(dashboard_object);
      }
    } else {
      // for testing only

      // add the custom entity data
      dashboard_object = {
        id: DeviceTypeUniqueID.CUSTOM_UID, // unique ID for custom entities
        t: DeviceType.CUSTOM,
        nodes: getDashboardEntityData(DeviceTypeUniqueID.CUSTOM_UID)
      };
      if ((dashboard_object.nodes ?? []).length > 0) {
        dashboard_nodes.push(dashboard_object);
      }

      // add the scheduler data
      // let scheduler_data = emsesp_schedule.schedule.filter((item) => item.name);
      // let scheduler_data2 = scheduler_data.map((item, index) => ({
      //   id: DeviceTypeUniqueID.SCHEDULER_UID * 100 + index,
      //   dv: {
      //     id: '00' + item.name,
      //     v: item.active ? 'on' : 'off',
      //     c: item.name,
      //     l: ['off', 'on']
      //   }
      // }));
      // dashboard_object = {
      //   id: DeviceTypeUniqueID.SCHEDULER_UID,
      //   t: DeviceType.SCHEDULER,
      //   nodes: scheduler_data2
      // };
      // if ((dashboard_object.nodes ?? []).length > 0) {
      //   dashboard_data.push(dashboard_object);
      // }
    }

    const dashboardData = {
      // connect: false,
      connected: true,
      nodes: dashboard_nodes
    };
    // console.log('dashboardData: ', dashboardData);

    // return dashboard_data; // if not using msgpack
    return new Response(encoder.encode(dashboardData), { headers }); // msgpack it
  })

  // Customizations
  .post(EMSESP_CUSTOMIZATION_ENTITIES_ENDPOINT, async (request: any) => {
    const content = await request.json();
    const id = content.id;
    for (const entity of content.entity_ids) {
      if (id === 1) {
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
      } else if (id === 7) {
        updateMask(entity, emsesp_deviceentities_7, emsesp_devicedata_7);
      } else if (id === 8) {
        updateMask(entity, emsesp_deviceentities_8, emsesp_devicedata_8);
      } else if (id === 9) {
        updateMask(entity, emsesp_deviceentities_9, emsesp_devicedata_9);
      } else if (id === 10) {
        updateMask(entity, emsesp_deviceentities_10, emsesp_devicedata_10);
      } else if (id === 11) {
        updateMask(entity, emsesp_deviceentities_11, emsesp_devicedata_11);
      }
    }
    console.log('customization saved', content);
    return status(200);
  })
  .post(EMSESP_RESET_CUSTOMIZATIONS_ENDPOINT, async (request: any) => {
    return status(200);
  })
  .post(EMSESP_WRITE_DEVICENAME_ENDPOINT, async (request: any) => {
    const content = await request.json();
    const id = content.id;
    console.log('Renaming device ID ' + id + ' to ' + content.name);
    return status(200);
  })

  // Scheduler
  .post(EMSESP_SCHEDULE_ENDPOINT, async (request: any) => {
    const content = await request.json();
    emsesp_schedule = content;
    console.log('schedule saved', emsesp_schedule);
    return status(200);
  })
  .get(EMSESP_SCHEDULE_ENDPOINT, () => emsesp_schedule)

  // Modules
  .post(EMSESP_MODULES_ENDPOINT, async (request: any) => {
    const content = await request.json();
    let modules = content.modules;
    for (let i = 0; i < modules.length; i++) {
      const key = modules[i].key;
      const objIndex = emsesp_modules.modules.findIndex(
        (obj: any) => obj.key === key
      );
      if (objIndex !== -1) {
        emsesp_modules.modules[objIndex].enabled = modules[i].enabled;
        emsesp_modules.modules[objIndex].license = modules[i].license;
      }
    }
    console.log('modules updated', emsesp_modules);
    return status(200);
  })
  .get(EMSESP_MODULES_ENDPOINT, () => emsesp_modules)

  // Custom Entities
  .post(EMSESP_CUSTOMENTITIES_ENDPOINT, async (request: any) => {
    const content = await request.json();
    emsesp_customentities = content;
    console.log('custom entities saved', emsesp_customentities);
    return status(200);
  })
  .get(EMSESP_CUSTOMENTITIES_ENDPOINT, () => emsesp_customentities)

  // Devices page
  .post(EMSESP_WRITE_DEVICEVALUE_ENDPOINT, async (request: any) => {
    const content = await request.json();
    const command = content.c;
    const value = content.v;
    const id = content.id;

    console.log(
      'write device value, id: ' + id + ' command: ' + command + ' value: ' + value
    );

    var objIndex;
    if (id === 1) {
      objIndex = emsesp_devicedata_1.nodes.findIndex((obj) => obj.c == command);
      emsesp_devicedata_1.nodes[objIndex].v = value;
    }
    if (id === 2) {
      objIndex = emsesp_devicedata_2.nodes.findIndex((obj) => obj.c == command);
      emsesp_devicedata_2.nodes[objIndex].v = value;
    }
    if (id === 3) {
      objIndex = emsesp_devicedata_3.nodes.findIndex((obj) => obj.c == command);
      emsesp_devicedata_3.nodes[objIndex].v = value;
    }
    if (id === 4) {
      objIndex = emsesp_devicedata_4.nodes.findIndex((obj) => obj.c == command);
      emsesp_devicedata_4.nodes[objIndex].v = value;
    }
    if (id === 5) {
      objIndex = emsesp_devicedata_5.nodes.findIndex((obj) => obj.c == command);
      emsesp_devicedata_5.nodes[objIndex].v = value;
    }
    if (id === 6) {
      objIndex = emsesp_devicedata_6.nodes.findIndex((obj) => obj.c == command);
      emsesp_devicedata_6.nodes[objIndex].v = value;
    }
    if (id === 7) {
      objIndex = emsesp_devicedata_7.nodes.findIndex((obj) => obj.c == command);
      emsesp_devicedata_7.nodes[objIndex].v = value;
    }
    if (id === 8) {
      objIndex = emsesp_devicedata_8.nodes.findIndex((obj) => obj.c == command);
      emsesp_devicedata_8.nodes[objIndex].v = value;
    }
    if (id === 9) {
      objIndex = emsesp_devicedata_9.nodes.findIndex((obj) => obj.c == command);
      emsesp_devicedata_9.nodes[objIndex].v = value;
    }
    if (id === 10) {
      objIndex = emsesp_devicedata_10.nodes.findIndex((obj) => obj.c == command);
      emsesp_devicedata_10.nodes[objIndex].v = value;
    }
    if (id === 10) {
      objIndex = emsesp_devicedata_11.nodes.findIndex((obj) => obj.c == command);
      emsesp_devicedata_11.nodes[objIndex].v = value;
    }
    if (id === DeviceTypeUniqueID.CUSTOM_UID) {
      // custom entities
      objIndex = emsesp_devicedata_99.nodes.findIndex((obj) => obj.c == command);
      emsesp_devicedata_99.nodes[objIndex].v = value;
    }
    if (id === DeviceTypeUniqueID.SCHEDULER_UID) {
      // toggle scheduler
      // find the schedule in emsesp_schedule via the name and toggle the active
      const objIndex = emsesp_schedule.schedule.findIndex(
        (obj) => obj.name === command
      );
      if (objIndex !== -1) {
        emsesp_schedule.schedule[objIndex].active = value;
        console.log("Toggle schedule '" + command + "' to " + value);
      }
    }

    // await delay(1000); // wait to show spinner
    // console.log(
    //   'Device Value updated. command:' + command + ' value:' + value + ' id:' + id
    // );
    return status(200);
  })

  // Temperature & Analog Sensors
  .post(EMSESP_WRITE_TEMPSENSOR_ENDPOINT, async (request: any) => {
    const ts = await request.json();
    var objIndex = emsesp_sensordata.ts.findIndex((obj) => obj.id == ts.id_str);
    if (objIndex !== -1) {
      emsesp_sensordata.ts[objIndex].n = ts.name;
      emsesp_sensordata.ts[objIndex].o = ts.offset;
    }
    console.log('temp sensor saved', ts);
    return status(200);
  })
  .post(EMSESP_WRITE_ANALOGSENSOR_ENDPOINT, async (request: any) => {
    const as = await request.json();
    var objIndex = emsesp_sensordata.as.findIndex((obj) => obj.g == as.gpio);
    if (objIndex === -1) {
      emsesp_sensordata.as.push({
        id: as.id,
        g: as.gpio,
        n: as.name,
        f: as.factor,
        o: as.offset,
        u: as.uom,
        t: as.type,
        d: as.deleted,
        v: 0 // must be added for demo only
      });
    } else {
      if (as.deleted) {
        emsesp_sensordata.as[objIndex].d = true;
        var filtered = emsesp_sensordata.as.filter(function (value, index, arr) {
          return !value.d;
        });
        emsesp_sensordata.as = filtered;
      } else {
        emsesp_sensordata.as[objIndex].n = as.name;
        emsesp_sensordata.as[objIndex].f = as.factor;
        emsesp_sensordata.as[objIndex].o = as.offset;
        emsesp_sensordata.as[objIndex].u = as.uom;
        emsesp_sensordata.as[objIndex].t = as.type;
      }
    }
    console.log('analog sensor saved', as);

    return status(200);
  })

  // Settings - board profile
  .get(EMSESP_BOARDPROFILE_ENDPOINT, (request) => {
    const board_profile = request.query.boardProfile;

    const data = {
      board_profile: settings.board_profile,
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

    data.board_profile =
      typeof board_profile === 'string' ? board_profile : settings.board_profile;

    console.log('board profile for ' + board_profile + ' fetched: ', data);
    return data;
  })

  // generic action for all /rest/... endpoints
  // takes an action and param in JSON
  .post(EMSESP_ACTION_ENDPOINT, async (request: any) => {
    const content = await request.json();
    if (content.hasOwnProperty('action')) {
      const action = content.action;
      if (action === 'export') {
        // export data
        return export_data(content.param);
      } else if (action === 'getCustomSupport') {
        // send custom support
        return custom_support();
      } else if (action === 'checkUpgrade') {
        // check upgrade
        // check if content has a param
        if (!content.param) {
          return check_upgrade('');
        }
        return check_upgrade(content.param);
      } else if (action === 'uploadURL') {
        // upload URL
        console.log('upload File from URL', content.param);
        return status(200);
      }
    }
    return status(404); // cmd not found
  })

  // API which are usually POST for security
  .post(EMSESP_SYSTEM_INFO_ENDPOINT, () => emsesp_info)
  .get(EMSESP_SYSTEM_INFO_ENDPOINT, () => emsesp_info)
  .post(API_ENDPOINT_ROOT, async (request: any) => {
    const data = await request.json();
    // check if the json data has key called cmd
    let cmd = '';
    if (data.hasOwnProperty('cmd')) {
      cmd = data.cmd;
    } else if (data.hasOwnProperty('entity')) {
      cmd = data.entity;
    } else {
      return status(400); // bad request
    }

    if (data.device === 'system') {
      if (cmd === 'info') {
        return emsesp_info;
      } else if (cmd === 'format') {
        console.log('formatting...');
        return status(200);
      } else if (cmd === 'restart') {
        console.log('restarting...');
        system_status.status = 5;
        countHardwarePoll = 0;
        return status(200);
      } else if (cmd === 'read') {
        console.log('send read command:', data.data);
        return status(200);
      }
    }
    return status(404); // not found
  });

// Mock GitHub API
// https://api.github.com/repos/emsesp/EMS-ESP32/releases

router
  .get(GH_ENDPOINT_ROOT + '/tags/latest', () => {
    const data = {
      name: 'v' + LATEST_DEV_VERSION,
      published_at: new Date().toISOString() // use todays date
    };
    console.log('returning latest development version (today): ', data);
    return data;
  })
  .get(GH_ENDPOINT_ROOT + '/latest', () => {
    const data = {
      name: 'v' + LATEST_STABLE_VERSION,
      published_at: '2025-03-01T13:29:13.999Z'
    };
    console.log('returning latest stable version: ', data);
    return data;
  });

// const logger: ResponseHandler = (response, request) => {
//   console.log(
//     response.status,
//     request.url,
//     request.method,
//     'at',
//     new Date().toLocaleString()
//   );
// };

export default {
  port: 3080,
  fetch: router.fetch
  // missing: () => error(404, 'Error, endpoint not found'),
  // finally: [logger]
};

// use this with cloudflare workers instead
// export default { ...router };
