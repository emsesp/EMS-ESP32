const express = require('express')
const path = require('path')
const msgpack = require('@msgpack/msgpack')

// REST API
const app = express()
const port = process.env.PORT || 3080
const REST_ENDPOINT_ROOT = '/rest/'
app.use(express.static(path.join(__dirname, '../interface/build')))
app.use(express.json())

// ES API
const server = express()
const es_port = 3090
const ES_ENDPOINT_ROOT = '/es/'

// LOG
const LOG_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'logSettings'
const log_settings = {
  level: 6,
  max_messages: 50,
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
const ntp_settings = {
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
const ap_settings = {
  provision_mode: 1,
  ssid: 'ems-esp',
  password: 'ems-esp-neo',
  local_ip: '192.168.4.1',
  gateway_ip: '192.168.4.1',
  subnet_mask: '255.255.255.0',
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
const network_settings = {
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
const ota_settings = {
  enabled: true,
  port: 8266,
  password: 'ems-esp-neo',
}

// MQTT
const MQTT_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'mqttSettings'
const MQTT_STATUS_ENDPOINT = REST_ENDPOINT_ROOT + 'mqttStatus'
const mqtt_settings = {
  enabled: true,
  host: '192.168.1.4',
  port: 1883,
  base: 'ems-esp32',
  username: '',
  password: '',
  client_id: 'ems-esp32',
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
}
const mqtt_status = {
  enabled: true,
  connected: true,
  client_id: 'ems-esp32',
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
  emsesp_version: '3.x demo',
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
const security_settings = {
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
const EMSESP_SETTINGS_ENDPOINT = REST_ENDPOINT_ROOT + 'emsespSettings'
const EMSESP_DATA_ENDPOINT = REST_ENDPOINT_ROOT + 'data'
const EMSESP_SCANDEVICES_ENDPOINT = REST_ENDPOINT_ROOT + 'scanDevices'
const EMSESP_DEVICEDATA_ENDPOINT = REST_ENDPOINT_ROOT + 'deviceData'
const EMSESP_STATUS_ENDPOINT = REST_ENDPOINT_ROOT + 'emsespStatus'
const EMSESP_BOARDPROFILE_ENDPOINT = REST_ENDPOINT_ROOT + 'boardProfile'
const WRITE_VALUE_ENDPOINT = REST_ENDPOINT_ROOT + 'writeValue'
const WRITE_SENSOR_ENDPOINT = REST_ENDPOINT_ROOT + 'writeSensor'

emsesp_settings = {
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
  analog_enabled: false,
  pbutton_gpio: 0,
  board_profile: 'S32',
  dallas_format: 1,
  bool_format: 1,
  enum_format: 1,
}
const emsesp_data = {
  devices: [
    {
      i: 1,
      t: 'Thermostat',
      b: '',
      n: 'RC20/Moduline 300',
      d: 23,
      p: 77,
      v: '03.03',
    },
    {
      i: 2,
      t: 'Boiler',
      b: 'Nefit',
      n: 'GBx72/Trendline/Cerapur/Greenstar Si/27i',
      d: 8,
      p: 123,
      v: '06.01',
    },
    {
      i: 3,
      t: 'Controller',
      b: '',
      n: 'BC10',
      d: 9,
      p: 190,
      v: '01.03',
    },
  ],
  sensors: [
    { n: 1, i: '28-233D-9497-0C03', t: 25.7, o: 1.2 },
    { n: 2, i: '28-243D-7437-1E3A', t: 26.1, o: 0 },
  ],
  analog: 12,
}

const emsesp_status = {
  status: 0,
  rx_received: 344,
  tx_sent: 104,
  rx_quality: 100,
  tx_quality: 100,
}

const emsesp_devicedata_1 = {
  type: 'Thermostat',
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
  type: 'Boiler',
  data: [
    { v: 'off', u: 0, n: 'heating active' },
    { v: 'off', u: 0, n: 'warm water active' },
    { v: 5, u: 1, n: 'selected flow temperature', c: 'selflowtemp' },
    { v: 0, u: 2, n: 'burner selected max power', c: 'selburnpow' },
    { v: 0, u: 2, n: 'heating pump modulation' },
    { v: 42.9, u: 1, n: 'current flow temperature' },
    { v: 41.8, u: 1, n: 'return temperature' },
    { v: 1.6, u: 9, n: 'system pressure' },
    { v: 45, u: 1, n: 'actual boiler temperature' },
    { v: 'off', u: 0, n: 'gas' },
    { v: 0, u: 8, n: 'flame current' },
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
    { v: 90, u: 2, n: 'burner pump max power', c: 'pumpmodmax' },
    { v: 55, u: 2, n: 'burner pump min power', c: 'pumpmodmin' },
    { v: 1, u: 7, n: 'pump delay', c: 'pumpdelay' },
    { v: 10, u: 7, n: 'burner min period', c: 'burnminperiod' },
    { v: 0, u: 2, n: 'burner min power', c: 'burnminpower' },
    { v: 77, u: 2, n: 'burner max power', c: 'burnmaxpower' },
    { v: -6, u: 1, n: 'hysteresis on temperature', c: 'boilhyston' },
    { v: 6, u: 1, n: 'hysteresis off temperature', c: 'boilhystoff' },
    { v: 0, u: 2, n: 'burner current power' },
    { v: 317694, u: 16, n: 'burner starts' },
    { v: 524115, u: 7, n: 'total burner operating time' },
    { v: 424286, u: 7, n: 'total heat operating time' },
    { v: 4571225, u: 7, n: 'total UBA operating time' },
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
    { v: 6000, u: 6, n: 'maintenance set time', c: 'maintenancetime' },
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
    { v: 100, u: 2, n: 'ww max power', c: 'wwmaxpower' },
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
      l: [
        'off',
        '1x3min',
        '2x3min',
        '3x3min',
        '4x3min',
        '5x3min',
        '6x3min',
        'continuous',
      ],
    },
    {
      v: 'off',
      u: 0,
      n: 'ww circulation active',
      c: 'wwcirc',
      l: ['off', 'on'],
    },
    { v: 37.1, u: 1, n: 'ww current intern temperature' },
    { v: 0, u: 3, n: 'ww current tap water flow' },
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
    { v: 282323, u: 16, n: 'ww starts' },
    { v: 99829, u: 7, n: 'ww active time' },
  ],
}

const emsesp_devicedata_3 = {
  type: 'Controller',
  data: [],
}

// LOG
app.get(FETCH_LOG_ENDPOINT, (req, res) => {
  const encoded = msgpack.encode(fetch_log)
  res.write(encoded, 'binary')
  res.end(null, 'binary')
})
app.get(LOG_SETTINGS_ENDPOINT, (req, res) => {
  console.log(
    'Fetching log settings ' +
      log_settings.level +
      ',' +
      log_settings.max_messages,
  )
  res.json(log_settings)
})
app.post(LOG_SETTINGS_ENDPOINT, (req, res) => {
  console.log(
    'Setting new level=' +
      req.body.level +
      ' max_messages=' +
      req.body.max_messages,
  )
  res.sendStatus(200)
})

// NETWORK
app.get(NETWORK_STATUS_ENDPOINT, (req, res) => {
  res.json(network_status)
})
app.get(NETWORK_SETTINGS_ENDPOINT, (req, res) => {
  res.json(network_settings)
})
app.post(NETWORK_SETTINGS_ENDPOINT, (req, res) => {
  res.json(network_settings)
})
app.get(LIST_NETWORKS_ENDPOINT, (req, res) => {
  res.json(list_networks)
})
app.get(SCAN_NETWORKS_ENDPOINT, (req, res) => {
  res.sendStatus(202)
})

// AP
app.get(AP_SETTINGS_ENDPOINT, (req, res) => {
  res.json(ap_settings)
})
app.get(AP_STATUS_ENDPOINT, (req, res) => {
  res.json(ap_status)
})
app.post(AP_SETTINGS_ENDPOINT, (req, res) => {
  res.json(ap_settings)
})

// OTA
app.get(OTA_SETTINGS_ENDPOINT, (req, res) => {
  res.json(ota_settings)
})
app.post(OTA_SETTINGS_ENDPOINT, (req, res) => {
  res.json(ota_settings)
})

// MQTT
app.get(MQTT_SETTINGS_ENDPOINT, (req, res) => {
  res.json(mqtt_settings)
})
app.post(MQTT_SETTINGS_ENDPOINT, (req, res) => {
  res.json(mqtt_settings)
})
app.get(MQTT_STATUS_ENDPOINT, (req, res) => {
  res.json(mqtt_status)
})

// NTP
app.get(NTP_SETTINGS_ENDPOINT, (req, res) => {
  res.json(ntp_settings)
})
app.post(NTP_SETTINGS_ENDPOINT, (req, res) => {
  res.json(ntp_settings)
})
app.get(NTP_STATUS_ENDPOINT, (req, res) => {
  res.json(ntp_status)
})
app.post(TIME_ENDPOINT, (req, res) => {
  res.sendStatus(200)
})

// SYSTEM
app.get(SYSTEM_STATUS_ENDPOINT, (req, res) => {
  res.json(system_status)
})
app.get(SECURITY_SETTINGS_ENDPOINT, (req, res) => {
  res.json(security_settings)
})
app.post(SECURITY_SETTINGS_ENDPOINT, (req, res) => {
  res.json(security_settings)
})
app.get(FEATURES_ENDPOINT, (req, res) => {
  res.json(features)
})
app.get(VERIFY_AUTHORIZATION_ENDPOINT, (req, res) => {
  res.json(verify_authentication)
})
app.post(RESTART_ENDPOINT, (req, res) => {
  res.sendStatus(200)
})
app.post(FACTORY_RESET_ENDPOINT, (req, res) => {
  res.sendStatus(200)
})
app.post(UPLOAD_FIRMWARE_ENDPOINT, (req, res) => {
  res.sendStatus(200)
})
app.post(SIGN_IN_ENDPOINT, (req, res) => {
  res.json(signin)
})
app.get(GENERATE_TOKEN_ENDPOINT, (req, res) => {
  res.json(generate_token)
})

// EMS-ESP Project stuff
app.get(EMSESP_SETTINGS_ENDPOINT, (req, res) => {
  res.json(emsesp_settings)
})
app.post(EMSESP_SETTINGS_ENDPOINT, (req, res) => {
  console.log(req.body)
  emsesp_settings = req.body
  res.json(emsesp_settings)
})
app.get(EMSESP_DATA_ENDPOINT, (req, res) => {
  res.json(emsesp_data)
})
app.post(EMSESP_SCANDEVICES_ENDPOINT, (req, res) => {
  res.sendStatus(200)
})
app.get(EMSESP_STATUS_ENDPOINT, (req, res) => {
  res.json(emsesp_status)
})
app.post(EMSESP_DEVICEDATA_ENDPOINT, (req, res) => {
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
})

app.post(WRITE_VALUE_ENDPOINT, (req, res) => {
  const devicevalue = req.body.devicevalue
  const id = req.body.id
  console.log(id)
  console.log(devicevalue)

  res.sendStatus(200)
})

app.post(WRITE_SENSOR_ENDPOINT, (req, res) => {
  const sensor = req.body.sensor
  console.log(sensor)

  res.sendStatus(200)
})

app.post(EMSESP_BOARDPROFILE_ENDPOINT, (req, res) => {
  const board_profile = req.body.code

  const data = {
    led_gpio: 1,
    dallas_gpio: 2,
    rx_gpio: 3,
    tx_gpio: 4,
    pbutton_gpio: 5,
    phy_type: 0,
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

  res.json(data)
})

// create helper middleware so we can reuse server-sent events
const useServerSentEventsMiddleware = (req, res, next) => {
  res.setHeader('Content-Type', 'text/event-stream')
  res.setHeader('Cache-Control', 'no-cache')

  // only if you want anyone to access this endpoint
  res.setHeader('Access-Control-Allow-Origin', '*')

  res.flushHeaders()

  const sendEventStreamData = (data) => {
    const sseFormattedResponse = `data: ${JSON.stringify(data)}\n\n`
    res.write(sseFormattedResponse)
  }

  // we are attaching sendEventStreamData to res, so we can use it later
  Object.assign(res, {
    sendEventStreamData,
  })

  next()
}

const streamLog = (req, res) => {
  let interval = setInterval(function generateAndSendLog() {
    count = count + 1

    const data = {
      time: '000+00:00:00.000',
      level: 3,
      name: 'system',
      message: 'this is message #' + count,
    }

    res.sendEventStreamData(data)
  }, 1000)

  res.on('close', () => {
    clearInterval(interval)
    res.end()
  })
}

// event source, server-sent events SSE
const ES_LOG_ENDPOINT = ES_ENDPOINT_ROOT + 'log'
let count = 0
server.get(ES_LOG_ENDPOINT, useServerSentEventsMiddleware, streamLog)
server.listen(es_port, () =>
  console.log(
    `Mock EventSource server for EMS-ESP listening at http://localhost:${es_port}`,
  ),
)

// rest API
app.listen(port)
console.log(
  `Mock RESTful API server for EMS-ESP is up and running at http://localhost:${port}`,
)
