import { AutoRouter, error, status } from 'itty-router';
import { Encoder } from '@msgpack/msgpack';

const encoder = new Encoder();

const router = AutoRouter({
  port: 3080,
  missing: () => error(404, 'Error, endpoint not found')
});

const REST_ENDPOINT_ROOT = '/rest/';
const API_ENDPOINT_ROOT = '/api/';

// HTTP HEADERS
const headers = {
  'Access-Control-Allow-Origin': '*',
  'Content-type': 'application/json'
};

// GLOBAL VARIABLES
let countWifiScanPoll = 0; // wifi network scan

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
    // console.log('looking for ' + fullname + ' in ' + dd.data);
    const dd_objIndex = dd.data.findIndex((obj: any) => obj.id.slice(2) === fullname);
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
        dd.data[dd_objIndex].id = current_mask.toString(16).padStart(2, '0') + new_fullname;
        dd.data[dd_objIndex].cn = new_fullname;
      }
    }
  }
}

// START DATA

// LOG
const LOG_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'logSettings';
let log_settings = {
  level: 6,
  max_messages: 50,
  compact: true
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
  dns_ip_2: '0.0.0.0',
  hostname: 'ems-esp'
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
let ota_settings = {
  enabled: false,
  port: 8266,
  password: 'ems-esp-neo'
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
const SYSTEM_STATUS_ENDPOINT = REST_ENDPOINT_ROOT + 'systemStatus';
const ACTIVITY_ENDPOINT = REST_ENDPOINT_ROOT + 'activity';

// SETTINGS
const ESPSYSTEM_STATUS_ENDPOINT = REST_ENDPOINT_ROOT + 'ESPSystemStatus';
const SECURITY_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'securitySettings';
const RESTART_ENDPOINT = REST_ENDPOINT_ROOT + 'restart';
const FACTORY_RESET_ENDPOINT = REST_ENDPOINT_ROOT + 'factoryReset';

// SYSTEM SIGNIN
const VERIFY_AUTHORIZATION_ENDPOINT = REST_ENDPOINT_ROOT + 'verifyAuthorization';
const SIGN_IN_ENDPOINT = REST_ENDPOINT_ROOT + 'signIn';
const GENERATE_TOKEN_ENDPOINT = REST_ENDPOINT_ROOT + 'generateToken';

const ESPsystem_status = {
  emsesp_version: '3.7-demo',
  esp_platform: 'ESP32',
  cpu_type: 'ESP32-S3',
  cpu_rev: '0',
  cpu_cores: 2,
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
  partition: 'app0',
  app_used: 1863,
  app_free: 121,
  arduino_version: 'ESP32 Arduino v2.0.14'
};

const system_status = {
  emsesp_version: '3.7-demo',
  esp_platform: 'ESP32',
  status: 0,
  // status: 2,
  uptime: 77186,
  bus_uptime: 77121,
  num_devices: 2,
  num_sensors: 1,
  num_analogs: 1,
  free_heap: 143,
  ntp_status: 2,
  ota_status: false,
  mqtt_status: true,
  ap_status: false
};

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

//
// EMS-ESP Project specific
//
const EMSESP_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'settings';
const EMSESP_CORE_DATA_ENDPOINT = REST_ENDPOINT_ROOT + 'coreData';
const EMSESP_SENSOR_DATA_ENDPOINT = REST_ENDPOINT_ROOT + 'sensorData';
const EMSESP_DEVICES_ENDPOINT = REST_ENDPOINT_ROOT + 'devices';
const EMSESP_SCANDEVICES_ENDPOINT = REST_ENDPOINT_ROOT + 'scanDevices';

const EMSESP_DEVICEDATA_ENDPOINT1 = REST_ENDPOINT_ROOT + 'deviceData';
const EMSESP_DEVICEDATA_ENDPOINT2 = REST_ENDPOINT_ROOT + 'deviceData/:id?';
const EMSESP_DEVICEENTITIES_ENDPOINT1 = REST_ENDPOINT_ROOT + 'deviceEntities';
const EMSESP_DEVICEENTITIES_ENDPOINT2 = REST_ENDPOINT_ROOT + 'deviceEntities/:id?';

const EMSESP_BOARDPROFILE_ENDPOINT = REST_ENDPOINT_ROOT + 'boardProfile';
const EMSESP_WRITE_DEVICEVALUE_ENDPOINT = REST_ENDPOINT_ROOT + 'writeDeviceValue';
const EMSESP_WRITE_TEMPSENSOR_ENDPOINT = REST_ENDPOINT_ROOT + 'writeTemperatureSensor';
const EMSESP_WRITE_ANALOGSENSOR_ENDPOINT = REST_ENDPOINT_ROOT + 'writeAnalogSensor';
const EMSESP_CUSTOMIZATION_ENTITIES_ENDPOINT = REST_ENDPOINT_ROOT + 'customizationEntities';
const EMSESP_RESET_CUSTOMIZATIONS_ENDPOINT = REST_ENDPOINT_ROOT + 'resetCustomizations';

const EMSESP_SCHEDULE_ENDPOINT = REST_ENDPOINT_ROOT + 'schedule';
const EMSESP_CUSTOMENTITIES_ENDPOINT = REST_ENDPOINT_ROOT + 'customEntities';

const EMSESP_GET_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'getSettings';
const EMSESP_GET_CUSTOMIZATIONS_ENDPOINT = REST_ENDPOINT_ROOT + 'getCustomizations';
const EMSESP_GET_ENTITIES_ENDPOINT = REST_ENDPOINT_ROOT + 'getEntities';
const EMSESP_GET_SCHEDULE_ENDPOINT = REST_ENDPOINT_ROOT + 'getSchedule';

const EMSESP_SYSTEM_INFO_ENDPOINT = API_ENDPOINT_ROOT + 'system/info';

const emsesp_info = {
  System: {
    version: '3.7-demo',
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

const emsesp_allvalues = {
  'Boiler Nefit GBx72/Trendline/Cerapur/Greenstar Si (DeviceID:0x08, ProductID:123, Version:06.01)': {
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
  'Thermostat RC20/Moduline 300 (DeviceID:0x17, ProductID:77, Version:03.03)': {
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

let settings = {
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
  boiler_heatingoff: false,
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
      i: 7,
      s: 'Boiler (GBx72/Trendline/Cerapur/Greenstar Si/27i)',
      t: 4,
      tn: 'boiler'
    },
    {
      i: 2,
      s: 'Thermostat (RC20/Moduline 300)',
      t: 5,
      tn: 'thermostat'
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
  // connected: false,
  // devices: [],
  devices: [
    {
      id: 7,
      t: 5,
      tn: 'Boiler',
      b: 'Nefit',
      n: 'GBx72/Trendline/Cerapur/Greenstar Si/27i',
      // n: 'Enviline/Compress 6000AW/Hybrid 3000-7000iAW/SupraEco/Geo 5xx/WLW196i/WSW196i',
      d: 8,
      p: 123,
      v: '06.01',
      e: 69
    },
    {
      id: 3,
      t: 5,
      tn: 'Boiler',
      b: 'Buderus',
      n: 'GB125/GB135/MC10',
      d: 8,
      p: 123,
      v: '06.01',
      e: 73
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
      e: 57
    },
    {
      id: 2,
      t: 6,
      tn: 'Thermostat',
      b: '',
      n: 'RC20/Moduline 300',
      d: 23,
      p: 77,
      v: '03.03',
      e: 6
    },
    {
      id: 4,
      t: 6,
      tn: 'Thermostat',
      b: 'Buderus',
      n: 'RC100/Moduline 1000/1010',
      d: 16,
      p: 165,
      v: '04.01',
      e: 3
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
      e: 22
    },
    {
      id: 99,
      t: 4,
      tn: 'Custom',
      b: '',
      n: 'Custom Entities',
      d: 1,
      p: 1,
      v: '',
      e: 2
    }
  ]
};

const emsesp_sensordata = {
  // ts: [],
  ts: [
    { id: '28-233D-9497-0C03', n: 'Dallas 1', t: 25.7, o: 1.2, u: 1 },
    { id: '28-243D-7437-1E3A', n: 'Dallas 2 outside', t: 26.1, o: 0, u: 1 },
    { id: '28-243E-7437-1E3B', n: 'Zolder', t: 27.1, o: 0, u: 16 },
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
// 1 - RC35 thermo
// 2 - RC20 thermo
// 3 - Buderus GB125 boiler
// 4 - RC100 thermostat
// 5 - Mixer MM10
// 6 - Solar SM10
// 7 - Nefit Trendline boiler
// 99 - Custom

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
      l: ['own 1', 'family', 'morning', 'evening', 'am', 'pm', 'midday', 'singles', 'seniors', 'new', 'own 2']
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
      id: '00dhw comfort',
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
      l: ['off', '1x3min', '2x3min', '3x3min', '4x3min', '5x3min', '6x3min', 'continuous']
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
    { v: '01.01.2012', u: 0, id: '00next maintenance date', c: 'maintenancedate', h: 'dd.mm.yyyy' },
    { v: 'on', u: 0, id: '00dhw turn on/off', c: 'dhw/tapactivated', l: ['off', 'on'] },
    { v: 62, u: 1, id: '00dhw set temperature' },
    { v: 60, u: 1, id: '00dhw selected temperature', c: 'dhw/seltemp' },
    { v: 'flow', u: 0, id: '00dhw type' },
    { v: 'hot', u: 0, id: '00dhw comfort', c: 'dhw/comfort', l: ['hot', 'eco', 'intelligent'] },
    { v: 40, u: 2, id: '00dhw flow temperature offset', c: 'dhw/flowtempoffset' },
    { v: 100, u: 3, id: '00dhw max power', c: 'dhw/maxpower' },
    { v: 'off', u: 0, id: '00dhw circulation pump available', c: 'dhw/circpump', l: ['off', 'on'] },
    { v: '3-way valve', u: 0, id: '00dhw charging type' },
    { v: -5, u: 2, id: '00dhw hysteresis on temperature', c: 'dhw/hyston' },
    { v: 0, u: 2, id: '00dhw hysteresis off temperature', c: 'dhw/hystoff' },
    { v: 70, u: 1, id: '00dhw disinfection temperature', c: 'dhw/disinfectiontemp' },
    {
      v: 'off',
      u: 0,
      id: '00dhw circulation pump mode',
      c: 'dhw/circmode',
      l: ['off', '1x3min', '2x3min', '3x3min', '4x3min', '5x3min', '6x3min', 'continuous']
    },
    { v: 'off', u: 0, id: '00dhw circulation active', c: 'dhw/circ', l: ['off', 'on'] },
    { v: 47.3, u: 1, id: '00dhw current intern temperature' },
    { v: 0, u: 4, id: '00dhw current tap water flow' },
    { v: 47.3, u: 1, id: '00dhw storage intern temperature' },
    { v: 'on', u: 0, id: '00dhw activated', c: 'dhw/activated', l: ['off', 'on'] },
    { v: 'off', u: 0, id: '00dhw one time charging', c: 'dhw/onetime', l: ['off', 'on'] },
    { v: 'off', u: 0, id: '00dhw disinfecting', c: 'dhw/disinfecting', l: ['off', 'on'] },
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

const emsesp_devicedata_99 = {
  data: [
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

// CUSTOMIZATION
const emsesp_deviceentities_1 = [{ v: 'dummy value', n: 'dummy name', id: 'dummy', m: 0, w: false }];
const emsesp_deviceentities_3 = [{ v: 'dummy value', n: 'dummy name', id: 'dummy', m: 0, w: false }];
const emsesp_deviceentities_5 = [{ v: 'dummy value', n: 'dummy name', id: 'dummy', m: 0, w: false }];
const emsesp_deviceentities_6 = [{ v: 'dummy value', n: 'dummy name', id: 'dummy', m: 0, w: false }];

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
  { v: true, n: 'dhw turn on/off', id: 'dhw/tapactivated', m: 0, w: false },
  { v: 62, n: 'dhw set temperature', id: 'dhw/settemp', m: 0, w: false },
  { v: 60, n: 'dhw selected temperature', id: 'dhw/seltemp', m: 0, w: true },
  { n: 'dhw selected lower temperature', id: 'dhw/seltemplow', m: 2 },
  { n: 'dhw selected temperature for off', id: 'dhw/seltempoff', m: 2 },
  { n: 'dhw single charge temperature', id: 'dhw/seltempsingle', m: 2 },
  { v: 'flow', n: 'dhw type', id: 'dhw/type', m: 0, w: false },
  { v: 'hot', n: 'dhw comfort', id: 'dhw/comfort', m: 0, w: false },
  { v: 40, n: 'dhw flow temperature offset', id: 'dhw/flowtempoffset', m: 0, w: false },
  { v: 100, n: 'dhw max power', id: 'dhw/maxpower', m: 0, w: false },
  { v: false, n: 'dhw circulation pump available', id: 'dhw/circpump', m: 0, w: false },
  { v: '3-way valve', n: 'dhw charging type', id: 'dhw/chargetype', m: 0, w: false },
  { v: -5, n: 'dhw hysteresis on temperature', id: 'dhw/hyston', m: 0, w: false },
  { v: 0, n: 'dhw hysteresis off temperature', id: 'dhw/hystoff', m: 0, w: false },
  { v: 70, n: 'dhw disinfection temperature', id: 'dhw/disinfectiontemp', m: 0, w: false },
  { v: 'off', n: 'dhw circulation pump mode', id: 'dhw/circmode', m: 0, w: false },
  { v: false, n: 'dhw circulation active', id: 'dhw/circ', m: 0, w: false },
  { v: 46.4, n: 'dhw current intern temperature', id: 'dhw/curtemp', m: 0, w: false },
  { n: 'dhw current extern temperature', id: 'dhw/curtemp2', m: 2 },
  { v: 0, n: 'dhw current tap water flow', id: 'dhw/curflow', m: 0, w: false },
  { v: 46.3, n: 'dhw storage intern temperature', id: 'dhw/storagetemp1', m: 0, w: false },
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

// END DATA

// ROUTING STARTS HERE

// LOG
router
  .post(FETCH_LOG_ENDPOINT, () => {
    return status(200);
  })
  .get(LOG_SETTINGS_ENDPOINT, () => log_settings)
  .post(LOG_SETTINGS_ENDPOINT, async (request: any) => {
    log_settings = await request.json();
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
    return status(200);
  });

// AP
router
  .get(AP_SETTINGS_ENDPOINT, () => ap_settings)
  .get(AP_STATUS_ENDPOINT, () => ap_status)
  .post(AP_SETTINGS_ENDPOINT, async (request: any) => {
    ap_settings = await request.json();
    return status(200);
  });

// OTA
router
  .get(OTA_SETTINGS_ENDPOINT, () => ota_settings)
  .post(OTA_SETTINGS_ENDPOINT, async (request: any) => {
    ota_settings = await request.json();
    return status(200);
  });

// MQTT
router
  .get(MQTT_SETTINGS_ENDPOINT, () => mqtt_settings)
  .get(MQTT_STATUS_ENDPOINT, () => mqtt_status)
  .post(MQTT_SETTINGS_ENDPOINT, async (request: any) => {
    mqtt_settings = await request.json();
    return status(200);
  });

// NTP
router
  .get(NTP_SETTINGS_ENDPOINT, () => ntp_settings)
  .get(NTP_STATUS_ENDPOINT, () => ntp_status)
  .post(TIME_ENDPOINT, () => status(200))
  .post(NTP_SETTINGS_ENDPOINT, async (request: any) => {
    ntp_settings = await request.json();
    return status(200);
  });

// SYSTEM and SETTINGS
router
  .get(SYSTEM_STATUS_ENDPOINT, () => system_status)
  .get(ACTIVITY_ENDPOINT, () => activity)
  .get(ESPSYSTEM_STATUS_ENDPOINT, () => ESPsystem_status)
  .get(SECURITY_SETTINGS_ENDPOINT, () => security_settings)
  .post(SECURITY_SETTINGS_ENDPOINT, async (request: any) => {
    security_settings = await request.json();
    return status(200);
  })
  .get(VERIFY_AUTHORIZATION_ENDPOINT, () => verify_authentication)
  .post(RESTART_ENDPOINT, () => status(200))
  .post(FACTORY_RESET_ENDPOINT, () => status(200))
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
}

router

  // EMS-ESP Settings
  .get(EMSESP_SETTINGS_ENDPOINT, () => settings)
  .post(EMSESP_SETTINGS_ENDPOINT, async (request: any) => {
    settings = await request.json();
    status(200); // no restart needed
    status(205); // restart needed
  })

  // Device Dashboard Data
  .get(EMSESP_CORE_DATA_ENDPOINT, () => emsesp_coredata)
  .get(EMSESP_SENSOR_DATA_ENDPOINT, () => emsesp_sensordata)
  .get(EMSESP_DEVICES_ENDPOINT, () => emsesp_devices)
  .post(EMSESP_SCANDEVICES_ENDPOINT, () => status(200))

  .get(EMSESP_DEVICEDATA_ENDPOINT1, (request) =>
    request.query.id ? deviceData(Number(request.query.id)) : status(404)
  )
  .get(EMSESP_DEVICEDATA_ENDPOINT2, ({ params }) => (params.id ? deviceData(Number(params.id)) : status(404)))
  .get(EMSESP_DEVICEENTITIES_ENDPOINT1, (request) =>
    request.query.id ? deviceEntities(Number(request.query.id)) : status(404)
  )
  .get(EMSESP_DEVICEENTITIES_ENDPOINT2, ({ params }) => (params.id ? deviceEntities(Number(params.id)) : status(404)))

  // Customization
  .post(EMSESP_CUSTOMIZATION_ENTITIES_ENDPOINT, async (request: any) => {
    const content = await request.json();
    const id = content.id;
    for (const entity of content.entity_ids) {
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
    return status(200);
  })
  .post(EMSESP_RESET_CUSTOMIZATIONS_ENDPOINT, async (request: any) => {
    return status(200);
  })

  // Scheduler
  .post(EMSESP_SCHEDULE_ENDPOINT, async (request: any) => {
    const content = await request.json();
    emsesp_schedule = content;
    return status(200);
  })
  .get(EMSESP_SCHEDULE_ENDPOINT, () => emsesp_schedule)

  // Custom Entities
  .post(EMSESP_CUSTOMENTITIES_ENDPOINT, async (request: any) => {
    const content = await request.json();
    emsesp_customentities = content;
    return status(200);
  })
  .get(EMSESP_CUSTOMENTITIES_ENDPOINT, () => emsesp_customentities)

  // Device Dashboard
  .post(EMSESP_WRITE_DEVICEVALUE_ENDPOINT, async (request: any) => {
    const content = await request.json();
    const command = content.c;
    const value = content.v;
    const id = content.id;

    var objIndex;
    if (id === 1) {
      objIndex = emsesp_devicedata_1.data.findIndex((obj) => obj.c == command);
      emsesp_devicedata_1.data[objIndex].v = value;
    }
    if (id === 2) {
      objIndex = emsesp_devicedata_2.data.findIndex((obj) => obj.c == command);
      emsesp_devicedata_2.data[objIndex].v = value;
    }
    if (id === 3) {
      objIndex = emsesp_devicedata_3.data.findIndex((obj) => obj.c == command);
      emsesp_devicedata_3.data[objIndex].v = value;
    }
    if (id === 4) {
      objIndex = emsesp_devicedata_4.data.findIndex((obj) => obj.c == command);
      emsesp_devicedata_4.data[objIndex].v = value;
    }
    if (id === 5) {
      objIndex = emsesp_devicedata_5.data.findIndex((obj) => obj.c == command);
      emsesp_devicedata_5.data[objIndex].v = value;
    }
    if (id === 6) {
      objIndex = emsesp_devicedata_6.data.findIndex((obj) => obj.c == command);
      emsesp_devicedata_6.data[objIndex].v = value;
    }
    if (id === 7) {
      objIndex = emsesp_devicedata_7.data.findIndex((obj) => obj.c == command);
      emsesp_devicedata_7.data[objIndex].v = value;
    }
    if (id === 99) {
      // custom entities
      objIndex = emsesp_devicedata_99.data.findIndex((obj) => obj.c == command);
      emsesp_devicedata_99.data[objIndex].v = value;
    }

    // await delay(1000); // wait to show spinner
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

    return status(200);
  })

  // Settings - board profile
  .post(EMSESP_BOARDPROFILE_ENDPOINT, async (request: any) => {
    const content = await request.json();
    const board_profile = content.code;

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

    return data;
  })

  // Download Settings
  .get(EMSESP_GET_SETTINGS_ENDPOINT, () => emsesp_info)
  .get(EMSESP_GET_CUSTOMIZATIONS_ENDPOINT, () => emsesp_deviceentities_1)
  .get(EMSESP_GET_ENTITIES_ENDPOINT, () => emsesp_customentities)
  .get(EMSESP_GET_SCHEDULE_ENDPOINT, () => emsesp_schedule);

// API which are usually POST for security
router
  .post(EMSESP_SYSTEM_INFO_ENDPOINT, () => emsesp_info)
  .get(EMSESP_SYSTEM_INFO_ENDPOINT, () => emsesp_info)
  .post(API_ENDPOINT_ROOT, async (request: any) => {
    const data = await request.json();
    if (data.device === 'system') {
      if (data.entity === 'info') {
        return emsesp_info;
      }
      if (data.entity === 'allvalues') {
        return emsesp_allvalues;
      }
    }
    return status(404); // not found
  });

export default router;

// use this with cloudflare workers instead
// export default { ...router };
