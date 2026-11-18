// Mock data shared by all mockup concepts. Shape loosely follows the real /rest/dashboard and /rest/deviceData output.
window.MOCK = {
  system: {
    name: 'ems-esp',
    version: '3.10.0-dev.4',
    latest: '3.10.0-dev.5',
    board: 'BBQKees E32 V2 (S3_16M_P)',
    uptime: '12d 04:17:52',
    freeHeap: 187,
    maxAlloc: 110,
    psram: 7890,
    psramTotal: 8192,
    flashUsed: 62,
    cpuTemp: 46.1,
    network: { type: 'Ethernet', ip: '192.168.1.42', rssi: null, hostname: 'ems-esp', mac: '24:0A:C4:8F:11:3C' },
    wifi: { ssid: 'home-iot', rssi: -58 },
    mqtt: { connected: true, broker: '192.168.1.10:1883', published: 48211, failed: 3, discovery: 'Home Assistant' },
    ntp: { synced: true, server: 'pool.ntp.org', time: '2026-11-16 21:07:13' },
    ap: { active: false },
    bus: { status: 'connected', tx: 'EMS+', rx: 912340, txOk: 18422, txFail: 12, quality: 99.9 },
    api: { calls: 3021, failed: 0 },
    sensors: { temp: 4, analog: 2 },
    lastLogs: [
      { t: '21:07:02', l: 'INFO', m: 'mqtt', s: 'Published 12 entities for boiler' },
      { t: '21:06:48', l: 'NOTICE', m: 'emsesp', s: 'Write to thermostat hc1 seltemp 21.5 confirmed' },
      { t: '21:05:10', l: 'WARNING', m: 'emsesp', s: 'Last Tx write rejected by host, retrying' },
      { t: '21:01:33', l: 'INFO', m: 'system', s: 'Free heap 187 KB, max alloc 110 KB' }
    ]
  },

  // w: writeable descriptor. type number|enum|bool|cmd. fav: shown on dashboard
  devices: [
    {
      id: 1,
      type: 'boiler',
      icon: 'local_fire_department',
      name: 'Boiler',
      brand: 'Buderus',
      model: 'GB125/BC10',
      productId: 123,
      deviceId: '0x08',
      version: '04.04',
      state: 'heating',
      entities: [
        { id: 'curflowtemp', name: 'current flow temperature', v: 58.4, u: '°C', fav: true, spark: [48, 50, 53, 55, 57, 58, 58.4] },
        { id: 'rettemp', name: 'return temperature', v: 44.1, u: '°C', fav: true },
        { id: 'selflowtemp', name: 'selected flow temperature', v: 60, u: '°C', fav: true, w: { type: 'number', min: 0, max: 90, step: 1 } },
        { id: 'curburnpow', name: 'burner current power', v: 72, u: '%', fav: true },
        { id: 'burngas', name: 'gas', v: true, u: '', fav: true },
        { id: 'heatingactive', name: 'heating active', v: true, u: '', fav: true },
        { id: 'tapwateractive', name: 'tapwater active', v: false, u: '' },
        { id: 'syspress', name: 'system pressure', v: 1.6, u: 'bar', fav: true },
        { id: 'outdoortemp', name: 'outside temperature', v: 4.3, u: '°C', fav: true, spark: [6.1, 5.8, 5.2, 4.9, 4.6, 4.4, 4.3] },
        { id: 'flamecurr', name: 'flame current', v: 8.2, u: 'µA' },
        { id: 'heatingtemp', name: 'heating temperature', v: 70, u: '°C', w: { type: 'number', min: 0, max: 90, step: 1 } },
        { id: 'pumpmodmax', name: 'boiler pump max power', v: 100, u: '%', w: { type: 'number', min: 0, max: 100, step: 1 } },
        { id: 'heatingactivated', name: 'heating activated', v: true, u: '', w: { type: 'bool' } },
        { id: 'burnstarts', name: 'burner starts', v: 41283, u: '' },
        { id: 'burnworkmin', name: 'total burner operating time', v: '612 days 4 hours', u: '' },
        { id: 'servicecode', name: 'service code', v: '-H', u: '' },
        { id: 'reset', name: 'reset', v: '', u: '', w: { type: 'cmd' } }
      ]
    },
    {
      id: 2,
      type: 'thermostat',
      icon: 'thermostat',
      name: 'Thermostat',
      brand: 'Buderus',
      model: 'RC310',
      productId: 158,
      deviceId: '0x10',
      version: '73.04',
      state: 'auto',
      entities: [
        { id: 'hc1/currtemp', name: 'hc1 current room temperature', v: 20.8, u: '°C', fav: true, spark: [20.1, 20.3, 20.4, 20.6, 20.7, 20.8, 20.8] },
        { id: 'hc1/seltemp', name: 'hc1 selected room temperature', v: 21.5, u: '°C', fav: true, w: { type: 'number', min: 5, max: 30, step: 0.5 } },
        { id: 'hc1/mode', name: 'hc1 operating mode', v: 'auto', u: '', fav: true, w: { type: 'enum', options: ['manual', 'auto', 'off'] } },
        { id: 'hc1/daytemp', name: 'hc1 day temperature', v: 21.5, u: '°C', w: { type: 'number', min: 5, max: 30, step: 0.5 } },
        { id: 'hc1/nighttemp', name: 'hc1 night temperature', v: 17, u: '°C', w: { type: 'number', min: 5, max: 30, step: 0.5 } },
        { id: 'hc2/currtemp', name: 'hc2 current room temperature', v: 19.4, u: '°C', fav: true },
        { id: 'hc2/seltemp', name: 'hc2 selected room temperature', v: 19, u: '°C', fav: true, w: { type: 'number', min: 5, max: 30, step: 0.5 } },
        { id: 'hc2/mode', name: 'hc2 operating mode', v: 'auto', u: '', w: { type: 'enum', options: ['manual', 'auto', 'off'] } },
        { id: 'dhw/mode', name: 'dhw operating mode', v: 'eco', u: '', fav: true, w: { type: 'enum', options: ['off', 'eco', 'high', 'auto'] } },
        { id: 'dhw/settemp', name: 'dhw set temperature', v: 52, u: '°C', fav: true, w: { type: 'number', min: 30, max: 70, step: 1 } },
        { id: 'dhw/curtemp', name: 'dhw current temperature', v: 49.7, u: '°C', fav: true },
        { id: 'dhw/onetime', name: 'dhw one time charging', v: false, u: '', w: { type: 'bool' } },
        { id: 'datetime', name: 'date/time', v: '16.11.2026 21:07', u: '' },
        { id: 'building', name: 'building type', v: 'medium', u: '', w: { type: 'enum', options: ['light', 'medium', 'heavy'] } }
      ]
    },
    {
      id: 3,
      type: 'mixer',
      icon: 'valve',
      name: 'Mixer',
      brand: 'Buderus',
      model: 'MM100',
      productId: 160,
      deviceId: '0x21',
      version: '26.01',
      state: 'open',
      entities: [
        { id: 'hc2/flowtemphc', name: 'hc2 flow temperature', v: 34.2, u: '°C', fav: true },
        { id: 'hc2/valvestatus', name: 'hc2 mixing valve position', v: 38, u: '%', fav: true },
        { id: 'hc2/pumpstatus', name: 'hc2 pump status', v: true, u: '' },
        { id: 'hc2/flowsettemp', name: 'hc2 setpoint flow temperature', v: 35, u: '°C' }
      ]
    },
    {
      id: 4,
      type: 'solar',
      icon: 'solar_power',
      name: 'Solar',
      brand: 'Buderus',
      model: 'SM100',
      productId: 163,
      deviceId: '0x30',
      version: '11.05',
      state: 'idle',
      entities: [
        { id: 'collectortemp', name: 'collector temperature (TS1)', v: 7.8, u: '°C', fav: true },
        { id: 'cylbottomtemp', name: 'cylinder bottom temperature (TS2)', v: 31.0, u: '°C' },
        { id: 'solarpump', name: 'pump (PS1)', v: false, u: '' },
        { id: 'energytoday', name: 'total energy today', v: 0.4, u: 'kWh', fav: true },
        { id: 'energytotal', name: 'total energy', v: 4218.3, u: 'kWh' }
      ]
    }
  ],

  sensors: {
    temp: [
      { id: '28-233D-9497-0C03', name: 'living_room', v: 21.1, u: '°C', o: 0, sys: false },
      { id: '28-8A1C-0A5B-1F0E', name: 'cellar', v: 13.6, u: '°C', o: -0.4, sys: false },
      { id: '28-4B2D-1C07-3A01', name: 'dhw_outlet', v: 47.9, u: '°C', o: 0.5, sys: false },
      { id: '28-1F3C-0D9A-2B11', name: 'garage', v: 8.2, u: '°C', o: 0, sys: false },
      { id: '28-0000-0000-0001', name: 'gateway_internal', v: 31.4, u: '°C', o: 0, sys: true }
    ],
    analog: [
      { id: 1, gpio: 36, name: 'gas_meter', v: 1834.12, u: 'm³', type: 'Counter', f: 0.01, o: 0, sys: false },
      { id: 2, gpio: 39, name: 'water_pressure', v: 2.1, u: 'bar', type: 'ADC In', f: 0.004, o: -0.5, sys: false },
      { id: 3, gpio: 13, name: 'circ_pump_relay', v: false, u: '', type: 'Digital Out', f: 1, o: 0, sys: false },
      { id: 4, gpio: 2, name: 'gateway_led', v: true, u: '', type: 'Digital Out', f: 1, o: 0, sys: true }
    ],
    gpios: [4, 5, 14, 15, 16, 32, 33],
    types: ['Digital In', 'Counter', 'ADC In', 'Timer', 'Rate', 'Digital Out', 'PWM 0', 'PWM 1', 'PWM 2', 'NTC Temp', 'RGB Led', 'Pulse']
  },

  // ram: 0 EMS value, 1 RAM value, 2 NVS value (kept across restarts)
  custom: [
    { id: 1, name: 'flow_rate', ram: 0, device_id: '0x08', type_id: '0x18', offset: 9, value_type: 'uint16', factor: 0.1, u: 'l/min', writeable: false, v: 12.4 },
    { id: 2, name: 'boiler_max_power', ram: 0, device_id: '0x08', type_id: '0x16', offset: 6, value_type: 'uint8', factor: 1, u: '%', writeable: true, v: 100 },
    { id: 3, name: 'away_mode', ram: 1, u: '', writeable: true, v: 0 },
    { id: 4, name: 'frost_alarm', ram: 1, u: '', writeable: true, v: 0 },
    { id: 5, name: 'season_start', ram: 2, u: '', writeable: true, v: '2026-10-01' }
  ],

  // named commands, referenced by the scheduler with cmd_name
  commands: [
    { id: 1, name: 'comfort', cmd: 'thermostat/hc1/seltemp', value: '21.5', guest: true },
    { id: 2, name: 'setback', cmd: 'thermostat/hc1/seltemp', value: '17', guest: false },
    { id: 3, name: 'dhw_boost', cmd: 'thermostat/dhw/onetime', value: 'on', guest: true },
    { id: 4, name: 'legionella', cmd: 'boiler/disinfect', value: 'on', guest: false },
    { id: 5, name: 'frost_alarm_on', cmd: 'custom/frost_alarm', value: '1', guest: false },
    { id: 6, name: 'restart', cmd: 'system/restart', value: '', guest: false }
  ],

  // type: day (days 0=Sun..6=Sat at time), timer (every time, empty = at start-up), onchange (time is an entity), condition (time is an expression)
  schedules: [
    { id: 1, name: 'night_setback', type: 'day', days: [0, 1, 2, 3, 4, 5, 6], time: '22:30', cmd_name: 'setback', active: true },
    { id: 2, name: 'morning', type: 'day', days: [1, 2, 3, 4, 5], time: '06:15', cmd_name: 'comfort', active: true },
    { id: 3, name: 'weekend_morning', type: 'day', days: [0, 6], time: '08:00', cmd_name: 'comfort', active: true },
    { id: 4, name: 'legionella', type: 'day', days: [0], time: '02:00', cmd_name: 'legionella', active: false },
    { id: 5, name: 'boost_on_boot', type: 'timer', time: '', cmd_name: 'dhw_boost', active: true },
    { id: 6, name: 'frost_watch', type: 'condition', time: 'boiler/outdoortemp < 0', cmd_name: 'frost_alarm_on', active: true },
    { id: 7, name: 'mode_changed', type: 'onchange', time: 'thermostat/hc1/mode', cmd_name: 'dhw_boost', active: false }
  ]
};

// settings pages, shapes follow the real /rest/*Settings endpoints (enums kept as their label for readability)
window.MOCK.settings = {
  application: {
    system_name: 'ems-esp',
    telnet_enabled: true,
    modbus_enabled: false, modbus_max_clients: 10, modbus_port: 502, modbus_timeout: 10000,
    syslog_enabled: false, syslog_host: '', syslog_port: 514, syslog_level: 'INFO', syslog_mark_interval: 0,
    email_enabled: false, email_server: '', email_port: 587, email_security: 'StartTLS', email_login: '', email_pass: '', email_sender: '', email_recp: '', email_subject: 'EMS-ESP',
    auto_fw_check: true,
    analog_enabled: true, dallas_parasite: false,
    locale: 'en', bool_dashboard: 'on/off', bool_format: '"on"/"off"', enum_format: 'Value', fahrenheit: false, trace_raw: false,
    board_profile: 'E32V2', rx_gpio: 4, tx_gpio: 5, pbutton_gpio: 34, dallas_gpio: 14, led_gpio: 2, led_type: 'LED',
    phy_type: 'LAN8720', eth_power: 15, eth_phy_addr: 0, eth_clock_mode: 'GPIO0_IN',
    tx_mode: 'EMS+', ems_bus_id: 'Gateway 1 (0x48)',
    readonly_mode: false, write_protection: true, write_limit: 10, write_limit_refuse: false,
    hide_led: false, low_clock: false, developer_mode: false, disable_reset: false, boiler_heatingoff: false, remote_timeout_en: false, remote_timeout: 24
  },
  network: {
    ssid: 'home-iot', bssid: '', password: 'secret', tx_power: 'Auto', nosleep: true, bandwidth20: false, eth_10mbit: false,
    hostname: 'ems-esp', enableMDNS: true, enableCORS: false, CORSOrigin: '*', allowed_hosts: '',
    static_ip_config: false, local_ip: '', gateway_ip: '', subnet_mask: '', dns_ip_1: '', dns_ip_2: ''
  },
  ap: { provision_mode: 'When network connection not found', ssid: 'ems-esp', password: 'ems-esp-neo', channel: 1, ssid_hidden: false, max_clients: 4, local_ip: '192.168.4.1', gateway_ip: '192.168.4.1', subnet_mask: '255.255.255.0' },
  ntp: { enabled: true, server: 'pool.ntp.org', tz_label: 'Europe/Amsterdam', thermostat_sync: true, tz_label_t: 'Same as EMS-ESP' },
  mqtt: {
    enabled: true, host: '192.168.1.10', port: 1883, base: 'ems-esp', client_id: 'ems-esp', username: 'mqtt', password: 'secret', keep_alive: 60, mqtt_qos: '0',
    enableTLS: false, tlsCheck: 'Certificate (CA)', rootCA: '',
    clean_session: false, mqtt_retain: false, mqtt_commands: 'Change values and run Commands',
    nested_format: 'Nested in a single topic', send_response: true, publish_single: false, publish_single2cmd: false,
    ha_enabled: true, discovery_type: 'Home Assistant', discovery_prefix: 'homeassistant', entity_format: 'Single instance, short name (latest)', ha_number_mode: 'Box',
    publish_time_boiler: 10, publish_time_thermostat: 10, publish_time_solar: 10, publish_time_mixer: 10, publish_time_water: 0, publish_time_sensor: 10, publish_time_heartbeat: 60
  }
};

window.MOCK.users = [
  { username: 'admin', admin: true },
  { username: 'guest', admin: false }
];
window.MOCK.apikeys = [{ id: 1, name: 'home_assistant', access: 'Read and Write', tail: '3f2a', created: '2026-09-02' }];
window.MOCK.modules = [
  { id: 1, key: 'demo', name: 'Demo Module', author: 'proddy', version: '1.0.2', enabled: true, status: 'active', message: 'running', license: '' },
  { id: 2, key: 'heatmeter', name: 'Heat Meter', author: 'community', version: '0.4.0', enabled: false, status: 'disabled', message: 'needs a license key', license: '' }
];
window.MOCK.wifiScan = [
  { ssid: 'home-iot', rssi: -58, channel: 6, secure: true },
  { ssid: 'home-5g', rssi: -64, channel: 11, secure: true },
  { ssid: 'neighbour', rssi: -79, channel: 1, secure: true },
  { ssid: 'guest-wifi', rssi: -83, channel: 6, secure: false }
];
window.MOCK.firmware = {
  partition: 'app0', other: { partition: 'app1', version: '3.9.2' }, recovery: '1.1.0', official: true,
  stable: { version: '3.9.3', date: '2026-10-28' }, dev: { version: '3.10.0-dev.5', date: '2026-11-15' }
};
window.MOCK.activity = [
  ['EMS Telegrams Received (Rx)', 912340, 41],
  ['EMS Reads (Tx)', 18422, 12],
  ['EMS Writes (Tx)', 381, 0],
  ['Temperature Sensor Reads', 105233, 3],
  ['Analog Sensor Reads', 52111, 0],
  ['MQTT Publishes', 48211, 3],
  ['API Calls', 3021, 0],
  ['Syslog Messages', 0, 0]
];
window.MOCK.log = [
  ['21:07:02.118', 'INFO', 'mqtt', 'Published 12 entities for boiler'],
  ['21:06:48.540', 'NOTICE', 'emsesp', 'Write to thermostat hc1 seltemp 21.5 confirmed'],
  ['21:06:47.902', 'DEBUG', 'emsesp', 'Sending write Tx [#81], telegram: 0B 90 FF 00 02 01 A5 2B'],
  ['21:05:10.004', 'WARNING', 'emsesp', 'Last Tx write rejected by host, retrying'],
  ['21:04:33.761', 'DEBUG', 'temperaturesensor', 'Read sensor 28-233D-9497-0C03: 21.1'],
  ['21:01:33.220', 'INFO', 'system', 'Free heap 187 KB, max alloc 110 KB'],
  ['21:00:00.003', 'INFO', 'scheduler', 'Schedule weekend_morning: command comfort executed'],
  ['20:58:12.415', 'ERROR', 'mqtt', 'Publish failed for topic ems-esp/heatpump_data (queue full)'],
  ['20:55:41.009', 'INFO', 'emsesp', 'Thermostat RC310 (DeviceID:0x10) detected'],
  ['20:55:40.877', 'INFO', 'emsesp', 'Boiler GB125/BC10 (DeviceID:0x08) detected']
];

// customization masks: 1 hide from web, 2 exclude from MQTT/API, 4 write disabled, 8 favorite, 128 removed
window.MOCK.devices.forEach((d) => d.entities.forEach((e) => (e.m = e.fav ? 8 : 0)));
window.MOCK.devices[1].entities.find((e) => e.id === 'dhw/settemp').m |= 4;
window.MOCK.devices[0].entities.find((e) => e.id === 'flamecurr').m |= 2;
window.MOCK.devices[0].entities.find((e) => e.id === 'servicecode').cn = 'boiler service code';
