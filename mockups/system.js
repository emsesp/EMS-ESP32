// Status and Settings pages for concept A.
(function () {
  const { h, toast, M } = Shell;
  const S = M.system;
  const clone = (x) => JSON.parse(JSON.stringify(x));
  const plural = (n, w) => `${n} ${w}${n === 1 ? '' : 's'}`;
  const nf = (n) => n.toLocaleString('en-US');

  // ---------- shared pieces ----------
  const hero = (state, icon, title, sub, actions = '') =>
    `<div class="section hero ${state}"><div class="hero-icon"><span class="ms">${icon}</span></div>
      <div class="grow"><div class="hero-title">${title}</div><div class="cap">${sub}</div></div>${actions}</div>`;
  const kv = (rows) =>
    `<div class="kv">${rows
      .filter(Boolean)
      .map(([k, v, extra]) => `<div class="kv-row"><span class="k">${k}</span><span class="v">${v}</span>${extra ? `<span class="x">${extra}</span>` : ''}</div>`)
      .join('')}</div>`;
  const card = (icon, title, body, right = '') =>
    `<div class="section"><div class="mod-head"><span class="ms lb">${icon}</span><h3 class="h6 grow">${title}</h3>${right}</div>${body}</div>`;
  const meter = (label, used, total, unit, note = '') => {
    const pct = Math.round((used / total) * 100);
    return `<div class="meter"><div class="row"><span>${label}</span><span class="grow"></span><span class="cap">${nf(used)} / ${nf(total)} ${unit} used${note ? ` · ${note}` : ''}</span></div>
      <div class="bar"><div class="${pct > 85 ? 'hot' : ''}" style="width:${pct}%"></div></div></div>`;
  };
  const navBtn = (nav, icon, label, cls = 'secondary') => `<button class="btn ${cls}" data-nav="${nav}"><span class="ms sm">${icon}</span>${label}</button>`;
  const subTabs = (items, active, attr) =>
    `<div class="subtabs">${items.map(([k, l, i]) => `<button class="${k === active ? 'on' : ''}" ${attr}="${k}"><span class="ms sm">${i}</span>${l}</button>`).join('')}</div>`;
  const confirmDialog = (title, text, okLabel, okCls, onOk) => {
    const bd = document.getElementById('dialog');
    bd.innerHTML = `<div class="dialog" style="width:440px" onclick="event.stopPropagation()"><h2>${title}</h2>
      <div class="content"><p style="margin:12px 0 0">${text}</p></div>
      <div class="actions"><button class="btn secondary" data-close><span class="ms sm">cancel</span>Cancel</button>
      <button class="btn ${okCls} contained" data-ok>${okLabel}</button></div></div>`;
    bd.classList.add('open');
    const close = () => bd.classList.remove('open');
    bd.onclick = close;
    bd.querySelector('[data-close]').onclick = close;
    bd.querySelector('[data-ok]').onclick = () => { close(); onOk(); };
  };
  const APP = M.settings.application;
  const NET = M.settings.network;
  const MQ = M.settings.mqtt;

  // ================= STATUS =================
  const errCount = () => M.log.filter((l) => l[1] === 'ERROR').length;
  const warnCount = () => M.log.filter((l) => l[1] === 'WARNING').length;

  function status() {
    const tiles = [
      ['settings-version', '#c5572c', 'new_releases', 'EMS-ESP Firmware', `${S.version} · ${S.latest} available`, 'warn'],
      ['status-hardware', '#72caf9', 'memory', 'Hardware', `${S.freeHeap} KB free heap · ${S.cpuTemp} °C`, 'ok'],
      ['status-activity', '#68374d', 'swap_horiz', 'EMS Bus & Data Traffic', `${S.bus.status} · ${S.bus.tx} · ${S.bus.quality}% quality`, 'ok'],
      ['status-network', '#66bb6a', 'lan', 'Network', `${S.network.type} · ${S.network.ip}`, 'ok'],
      ['status-mqtt', '#66bb6a', 'device_hub', 'MQTT', `connected · ${S.mqtt.broker}`, 'ok'],
      ['status-ntp', '#66bb6a', 'schedule', 'NTP', `synchronized · ${S.ntp.time.slice(11)}`, 'ok'],
      ['status-ap', '#616161', 'wifi_tethering', 'Access Point', 'off, network is connected', 'off'],
      ['status-log', '#40828f', 'list_alt', 'System Log', `${plural(errCount(), 'error')}, ${plural(warnCount(), 'warning')} recently`, errCount() ? 'err' : 'ok']
    ];
    const resetOff = APP.disable_reset;
    return `${hero('ok', 'check_circle', 'EMS-ESP is running', `${S.board} · uptime ${S.uptime} · last restart: software restart`)}
    <div class="section">
      <div class="tiles">${tiles
        .map(([nav, c, i, k, v, st]) => `<div class="tile" data-nav="${nav}"><div class="avatar" style="background:${c}"><span class="ms">${i}</span></div>
          <div class="grow"><div class="k">${k}</div><div class="v">${v}</div></div><span class="dot ${st}"></span></div>`)
        .join('')}</div>
    </div>
    <div class="section">
      <div class="row wrap" style="gap:24px">
        <div class="grow" style="min-width:200px">${meter('Heap', 320 - S.freeHeap, 320, 'KB')}</div>
        <div class="grow" style="min-width:200px">${meter('PSRAM', S.psramTotal - S.psram, S.psramTotal, 'KB')}</div>
        <div class="grow" style="min-width:200px">${meter('File system', 112, 1920, 'KB')}</div>
      </div>
      <div class="row" style="margin-top:16px; justify-content:flex-end">
        <button class="btn warning" id="st-restart"><span class="ms sm">restart_alt</span>Restart</button>
        <button class="btn error contained" id="st-reset" ${resetOff ? 'disabled title="Factory reset is disabled in Application Settings"' : ''}><span class="ms sm">delete_forever</span>Factory Reset</button>
      </div>
    </div>`;
  }

  function hardware() {
    return `<div class="grid2">
    ${card('developer_board', 'Device', kv([
      ['Board', S.board],
      ['Chip', 'ESP32-S3 rev 0.2, 2 cores @ 240 MHz'],
      ['CPU temperature', `${S.cpuTemp} °C`],
      ['SDK', 'Arduino 3.1.1 / ESP-IDF 5.3.2'],
      ['MAC address', S.network.mac]
    ]))}
    ${card('memory', 'Memory', `${meter('Heap', 320 - S.freeHeap, 320, 'KB', `${S.maxAlloc} KB max block`)}${meter('PSRAM', S.psramTotal - S.psram, S.psramTotal, 'KB')}
      <p class="cap" style="margin:8px 0 0">Max block is the largest piece of heap that can be allocated at once. Below 40 KB the web UI and MQTT can fail.</p>`)}
    ${card('sd_storage', 'Storage', `${kv([['Flash chip', '16,384 KB · 80 MHz · QIO']])}
      ${meter('App partition (app0)', 2412, 3072, 'KB')}${meter('File system', 112, 1920, 'KB')}`)}
    ${card('battery_charging_full', 'Power and reset', kv([
      ['Last reset', 'Software restart / CPU 0'],
      ['Uptime', S.uptime],
      ['Brownouts since boot', '0']
    ]))}
    </div>`;
  }

  function activity() {
    const rows = M.activity
      .map(([name, ok, fail]) => {
        const q = ok + fail ? Math.round((ok / (ok + fail)) * 1000) / 10 : 100;
        return `<tr><td>${name}</td><td class="r val">${nf(ok)}</td><td class="r val ${fail ? 'warnc' : 'muted'}">${nf(fail)}</td>
          <td style="width:200px"><div class="row"><div class="bar grow" style="margin:0"><div class="${q < 90 ? 'hot' : ''}" style="width:${q}%"></div></div><span class="cap" style="width:46px;text-align:right">${q}%</span></div></td></tr>`;
      })
      .join('');
    return `${hero('ok', 'swap_horiz', `EMS bus ${S.bus.status}`, `Tx mode ${APP.tx_mode} · bus ID ${APP.ems_bus_id} · ${M.devices.length} devices · ${S.bus.quality}% Tx quality`, navBtn('settings-application', 'tune', 'EMS settings'))}
    <div class="section">
      <div class="mod-head"><span class="ms lb">monitoring</span><h3 class="h6 grow">Data Traffic</h3><span class="chip"><span class="dot ok"></span>live, updates every 2 s</span></div>
      <table class="tbl mtbl" style="margin-top:10px"><thead><tr><th>Counter</th><th class="r">Success</th><th class="r">Fail</th><th>Quality</th></tr></thead><tbody>${rows}</tbody></table>
      <p class="cap" style="margin:10px 0 0">A few Rx fails are normal. Many Tx fails usually mean the wrong Tx mode for your boiler.</p>
    </div>`;
  }

  // ---------- System Log ----------
  const LEVELS = ['ERROR', 'WARNING', 'NOTICE', 'INFO', 'DEBUG', 'ALL'];
  let logLevel = 'INFO', logQuery = '', compact = false, autoScroll = true, paused = false;
  const rank = (l) => (l === 'ALL' ? 9 : LEVELS.indexOf(l));

  function log() {
    const lines = M.log
      .slice()
      .reverse()
      .filter((l) => rank(l[1]) <= rank(logLevel) && (!logQuery || l.join(' ').toLowerCase().includes(logQuery)));
    return `<div class="section">
      <div class="toolbar" style="margin-top:0">
        <div class="field"><label>Log level</label><select class="input sm" id="lg-level">${LEVELS.map((l) => `<option ${l === logLevel ? 'selected' : ''}>${l}</option>`).join('')}</select></div>
        <div class="field"><label>Buffer</label><select class="input sm"><option>25</option><option selected>50</option><option>100</option><option>500</option></select></div>
        <span class="search" style="align-self:flex-end"><span class="ms">search</span><input class="input" id="lg-q" placeholder="Filter lines" value="${h(logQuery)}"></span>
        <span class="grow"></span>
        <span class="toggle-group" style="align-self:flex-end">
          <button class="${compact ? 'on' : ''}" id="lg-compact"><span class="ms xs">density_small</span>Compact</button>
          <button class="${autoScroll ? 'on' : ''}" id="lg-scroll"><span class="ms xs">vertical_align_bottom</span>Auto-scroll</button>
          <button class="${paused ? 'on' : ''}" id="lg-pause"><span class="ms xs">${paused ? 'play_arrow' : 'pause'}</span>${paused ? 'Resume' : 'Pause'}</button>
        </span>
        <button class="btn" id="lg-dl" style="align-self:flex-end"><span class="ms xs">download</span>Download</button>
      </div>
      <div class="console ${compact ? 'compact' : ''}">${lines
        .map(([t, l, m, s]) => `<div class="ln ${l.toLowerCase()}"><span class="t">${compact ? t.slice(0, 8) : t}</span><span class="l">${compact ? l[0] : l.padEnd(7)}</span>${compact ? '' : `<span class="m">${m}</span>`}<span class="s">${h(s)}</span></div>`)
        .join('') || '<div class="ln muted">no lines at this level</div>'}
        ${paused ? '<div class="ln muted">paused, new lines are buffered</div>' : ''}</div>
      <div class="cmdline"><span class="mono lb">ems-esp:/#</span><input class="input mono grow" id="lg-cmd" placeholder="console command, e.g. show values"><button class="btn" id="lg-run"><span class="ms xs">play_arrow</span>Run</button></div>
      <p class="cap" style="margin:8px 0 0">Changing the log level here also changes what EMS-ESP logs to Syslog. Showing ${plural(lines.length, 'line')}.</p>
    </div>`;
  }

  function networkStatus() {
    return `${hero('ok', 'lan', 'Connected over Ethernet', `${S.network.ip} · 100 Mbit full duplex · link up for ${S.uptime}`, navBtn('settings-network', 'settings_ethernet', 'Network settings'))}
    ${card('info', 'Details', kv([
      ['Hostname', `${NET.hostname} <span class="cap">${NET.enableMDNS ? '· ems-esp.local via mDNS' : ''}</span>`],
      ['IPv4 address', S.network.ip, 'DHCP'],
      ['IPv6 address', 'fe80::260a:c4ff:fe8f:113c'],
      ['MAC address', S.network.mac],
      ['Subnet mask', '255.255.255.0'],
      ['Gateway', '192.168.1.1'],
      ['DNS', '192.168.1.1']
    ]))}`;
  }

  function mqttStatus() {
    return `${hero('ok', 'device_hub', 'Connected to the MQTT broker', `${S.mqtt.broker} · ${MQ.discovery_type} discovery on`, navBtn('settings-mqtt', 'settings', 'MQTT settings'))}
    <div class="stat-row">
      <div class="stat"><div class="n">${nf(S.mqtt.published)}</div><div class="cap">published</div></div>
      <div class="stat"><div class="n">0</div><div class="cap">queued</div></div>
      <div class="stat"><div class="n warnc">${S.mqtt.failed}</div><div class="cap">publish errors</div></div>
      <div class="stat"><div class="n">1</div><div class="cap">reconnects</div></div>
    </div>
    ${card('info', 'Details', kv([
      ['Client ID', MQ.client_id],
      ['Base topic', `<span class="mono">${MQ.base}/</span>`],
      ['Last disconnect', 'TCP disconnected', '3 days ago'],
      ['Connected since', '2026-11-13 18:02']
    ]))}`;
  }

  function ntpStatus() {
    return `${hero('ok', 'schedule', 'Time synchronized', `${M.settings.ntp.server} · ${M.settings.ntp.tz_label}`, `<button class="btn" id="ntp-set"><span class="ms sm">edit_calendar</span>Set time</button>${navBtn('settings-ntp', 'settings', 'NTP settings')}`)}
    ${card('info', 'Details', kv([
      ['Local time', S.ntp.time],
      ['UTC time', '2026-11-16 20:07:13'],
      ['Last sync', '4 minutes ago'],
      ['Thermostat clock', M.settings.ntp.thermostat_sync ? 'kept in sync with EMS-ESP' : 'not synced']
    ]))}`;
  }

  function apStatus() {
    return `${hero('off', 'wifi_tethering_off', 'Access Point is off', `starts automatically when the network connection is lost (${M.settings.ap.provision_mode.toLowerCase()})`, navBtn('settings-ap', 'settings', 'Access Point settings'))}
    ${card('info', 'Details', kv([
      ['SSID', M.settings.ap.ssid],
      ['IP address', 'none'],
      ['MAC address', '24:0A:C4:8F:11:3D'],
      ['Connected clients', '0']
    ]))}`;
  }

  // ================= SETTINGS =================
  function settings() {
    const fw = M.firmware;
    const items = [
      ['settings-application', 'settings_applications', 'Application', `${APP.system_name} · Tx ${APP.tx_mode} · ${APP.board_profile} board`],
      ['settings-network', 'settings_ethernet', 'Network', `${S.network.type} · ${NET.hostname}${NET.static_ip_config ? ' · fixed IP' : ' · DHCP'}`],
      ['settings-ap', 'wifi_tethering', 'Access Point', M.settings.ap.provision_mode],
      ['settings-ntp', 'access_time', 'NTP', M.settings.ntp.enabled ? `${M.settings.ntp.server} · ${M.settings.ntp.tz_label}` : 'off'],
      ['settings-mqtt', 'device_hub', 'MQTT', MQ.enabled ? `${MQ.host}:${MQ.port} · ${MQ.ha_enabled ? MQ.discovery_type + ' discovery' : 'no discovery'}` : 'off'],
      ['settings-security', 'lock', 'Security', `${plural(M.users.length, 'user')} · ${plural(M.apikeys.length, 'API key')}`],
      ['settings-modules', 'extension', 'Modules', `${M.modules.filter((m) => m.enabled).length} of ${M.modules.length} enabled`],
      ['downloadupload', 'import_export', 'Download/Upload', 'System backup, heating settings backup and data export'],
      ['settings-version', 'new_releases', 'Firmware', `${S.version} · <span class="lb">${fw.dev.version} available</span>`, true]
    ];
    return `<div class="section set-list">${items
      .map(([nav, i, k, v, badge]) => `<div class="set-item" data-nav="${nav}">
        <div class="avatar" style="background:#40828f"><span class="ms">${i}</span></div><div class="grow"><div class="k">${k}${badge ? '<span class="badge-dot inline"></span>' : ''}</div><div class="v">${v}</div></div>
        <span class="ms muted">chevron_right</span></div>`)
      .join('')}</div>`;
  }

  // ---------- generic settings form ----------
  const snaps = {};
  const restartPending = {};
  Object.keys(M.settings).forEach((k) => (snaps[k] = clone(M.settings[k])));

  const LOCALES = [['en', 'English'], ['de', 'Deutsch'], ['nl', 'Nederlands'], ['fr', 'Français'], ['it', 'Italiano'], ['pl', 'Polski'], ['no', 'Norsk'], ['sv', 'Svenska'], ['cs', 'Čeština'], ['sk', 'Slovenčina'], ['hu', 'Magyar'], ['tr', 'Türkçe']];
  const BOARDS = [['S32', 'BBQKees Gateway S32'], ['S32S3', 'BBQKees Gateway S3'], ['E32', 'BBQKees Gateway E32'], ['E32V2', 'BBQKees Gateway E32 V2'], ['E32V2_2', 'BBQKees Gateway E32 V2.2'], ['NODEMCU', 'NodeMCU 32S'], ['LOLIN', 'Lolin D32'], ['OLIMEX', 'Olimex ESP32-EVB'], ['OLIMEXPOE', 'Olimex ESP32-POE'], ['C3MINI', 'Wemos C3 Mini'], ['S2MINI', 'Wemos S2 Mini'], ['CUSTOM', 'Custom...']];
  const on = (k) => (v) => !!v[k];
  const F = (type, k, label, x = {}) => ({ type, k, label, ...x });

  const FORMS = {
    'settings-application': {
      store: 'application',
      search: true,
      groups: [
        { id: 'general', icon: 'badge', title: 'General', fields: [
          F('text', 'system_name', 'System name', { help: 'shown in the menu and browser tab' }),
          F('select', 'locale', 'Language for device entities', { opts: LOCALES, help: 'names in MQTT, the API and Home Assistant' }),
          F('switch', 'auto_fw_check', 'Check for firmware updates automatically', { help: 'once a day, shows a dot on Settings when one is available' }),
          F('switch', 'fahrenheit', 'Show temperatures in Fahrenheit')
        ] },
        { id: 'ems', icon: 'swap_horiz', title: 'EMS Bus', desc: 'How EMS-ESP talks to your heating system.', fields: [
          F('select', 'tx_mode', 'Tx mode', { opts: ['EMS', 'EMS+', 'HT3', 'Hardware', 'Auto'], restart: true, help: 'EMS+ for most Buderus/Nefit, HT3 for Junkers' }),
          F('select', 'ems_bus_id', 'EMS bus ID', { opts: ['Terminal (0x0A)', 'Service Key (0x0B)', 'Modem (0x0D)', 'Converter (0x0E)', 'Time Module (0x0F)', 'Gateway 1 (0x48)', 'Gateway 2 (0x49)', 'Gateway 3 (0x4A)', 'Gateway 4 (0x4B)', 'Gateway 5 (0x4C)', 'Gateway 7 (0x4D)'], restart: true }),
          F('switch', 'readonly_mode', 'Read-only mode', { help: 'blocks every write to the EMS bus; devices are still detected', warn: true }),
          F('switch', 'write_protection', 'Protect devices from repeated and frequent writes'),
          F('number', 'write_limit', 'Max writes per minute', { show: on('write_protection'), help: '0 = no limit' }),
          F('switch', 'write_limit_refuse', 'Refuse writes over the limit', { show: on('write_protection'), help: 'otherwise only a warning is logged' }),
          F('switch', 'boiler_heatingoff', 'Start boiler with forced heating off'),
          F('switch', 'remote_timeout_en', 'Remote timeout'),
          F('number', 'remote_timeout', 'Timeout', { unit: 'hours', show: on('remote_timeout_en') })
        ] },
        { id: 'board', icon: 'developer_board', title: 'Board', desc: 'Pins come from the board profile. Pick Custom to set them yourself.', fields: [
          F('select', 'board_profile', 'Board profile', { opts: BOARDS, restart: true, full: true }),
          F('pins', 'board_pins', 'Pins', { show: (v) => v.board_profile !== 'CUSTOM' }),
          F('number', 'rx_gpio', 'Rx GPIO', { show: (v) => v.board_profile === 'CUSTOM', restart: true }),
          F('number', 'tx_gpio', 'Tx GPIO', { show: (v) => v.board_profile === 'CUSTOM', restart: true }),
          F('number', 'pbutton_gpio', 'Button GPIO', { show: (v) => v.board_profile === 'CUSTOM', restart: true }),
          F('number', 'dallas_gpio', '1-Wire GPIO (0 = off)', { show: (v) => v.board_profile === 'CUSTOM', restart: true }),
          F('number', 'led_gpio', 'LED GPIO (0 = off)', { show: (v) => v.board_profile === 'CUSTOM', restart: true }),
          F('select', 'led_type', 'LED type', { opts: ['LED', 'RGB-LED'], show: (v) => v.board_profile === 'CUSTOM', restart: true }),
          F('select', 'phy_type', 'Ethernet PHY', { opts: ['Disabled', 'LAN8720', 'TLK110', 'RTL8201'], show: (v) => v.board_profile === 'CUSTOM', restart: true }),
          F('number', 'eth_power', 'PHY power GPIO (-1 = off)', { show: (v) => v.board_profile === 'CUSTOM' && v.phy_type !== 'Disabled', restart: true }),
          F('number', 'eth_phy_addr', 'PHY I²C address', { show: (v) => v.board_profile === 'CUSTOM' && v.phy_type !== 'Disabled', restart: true }),
          F('select', 'eth_clock_mode', 'PHY clock', { opts: ['GPIO0_IN', 'GPIO0_OUT', 'GPIO16_OUT', 'GPIO17_OUT'], show: (v) => v.board_profile === 'CUSTOM' && v.phy_type !== 'Disabled', restart: true })
        ] },
        { id: 'sensors', icon: 'sensors', title: 'Sensors', fields: [
          F('switch', 'analog_enabled', 'Enable analog sensors', { restart: true }),
          F('switch', 'dallas_parasite', 'Enable 1-Wire parasite power', { restart: true })
        ] },
        { id: 'format', icon: 'text_format', title: 'Formatting', desc: 'How values look in the web UI and in MQTT/API payloads.', fields: [
          F('select', 'bool_dashboard', 'Booleans in the web UI', { opts: ['on/off', 'ON/OFF', 'true/false', '1/0'] }),
          F('select', 'bool_format', 'Booleans in API/MQTT', { opts: ['on/off', 'ON/OFF', '"true"/"false"', 'true/false', '"1"/"0"', '1/0', '"on"/"off"'] }),
          F('select', 'enum_format', 'Lists in API/MQTT', { opts: ['Value', 'Index'], help: 'Value sends "auto", Index sends 1' })
        ] },
        { id: 'services', icon: 'hub', title: 'Services', fields: [
          F('switch', 'telnet_enabled', 'Telnet console', { restart: true, help: 'port 23, same commands as the serial console' }),
          F('switch', 'modbus_enabled', 'Modbus TCP', { restart: true }),
          F('number', 'modbus_port', 'Port', { show: on('modbus_enabled'), restart: true }),
          F('number', 'modbus_max_clients', 'Max clients', { show: on('modbus_enabled'), restart: true }),
          F('number', 'modbus_timeout', 'Timeout', { unit: 'ms', show: on('modbus_enabled'), restart: true }),
          F('switch', 'syslog_enabled', 'Syslog'),
          F('text', 'syslog_host', 'Host', { show: on('syslog_enabled'), ph: '192.168.1.5' }),
          F('number', 'syslog_port', 'Port', { show: on('syslog_enabled') }),
          F('select', 'syslog_level', 'Log level', { opts: ['OFF', 'ERR', 'WARN', 'NOTICE', 'INFO', 'ALL'], show: on('syslog_enabled') }),
          F('number', 'syslog_mark_interval', 'Mark interval', { unit: 's', show: on('syslog_enabled') }),
          F('switch', 'email_enabled', 'Email notifications', { help: 'the Scheduler and Commands can send mail' }),
          F('text', 'email_server', 'SMTP server', { show: on('email_enabled') }),
          F('number', 'email_port', 'Port', { show: on('email_enabled') }),
          F('select', 'email_security', 'Security', { opts: ['Off', 'SSL', 'StartTLS'], show: on('email_enabled') }),
          F('text', 'email_login', 'Login', { show: on('email_enabled') }),
          F('password', 'email_pass', 'Password', { show: on('email_enabled') }),
          F('text', 'email_sender', 'From', { show: on('email_enabled') }),
          F('text', 'email_recp', 'To', { show: on('email_enabled') }),
          F('text', 'email_subject', 'Subject', { show: on('email_enabled'), full: true })
        ] },
        { id: 'advanced', icon: 'science', title: 'Advanced', fields: [
          F('switch', 'trace_raw', 'Log EMS telegrams in hexadecimal'),
          F('switch', 'hide_led', 'Hide LED'),
          F('switch', 'low_clock', 'Underclock CPU speed', { restart: true, help: 'saves power, slows the web UI' }),
          F('switch', 'developer_mode', 'Developer mode', { help: 'shows extra tools for testing raw telegrams' }),
          F('switch', 'disable_reset', 'Disable factory reset', { help: 'hides Factory Reset in Status and Firmware' })
        ] }
      ]
    },
    'settings-network': {
      store: 'network',
      top: () => `<div class="msg info"><span class="ms">lan</span><span>Connected over Ethernet now (${S.network.ip}). Wi-Fi is only used without a cable.</span></div>`,
      groups: [
        { id: 'wifi', icon: 'wifi', title: 'Wi-Fi', fields: [
          F('text', 'ssid', 'SSID', { action: ['wifi_find', 'Scan', 'scan'] }),
          F('password', 'password', 'Password'),
          F('text', 'bssid', 'BSSID', { ph: 'leave blank to use only the SSID', mono: true }),
          F('select', 'tx_power', 'Tx power', { opts: ['Auto', '19.5 dBm', '17 dBm', '15 dBm', '13 dBm', '11 dBm', '8.5 dBm', '7 dBm', '5 dBm', '2 dBm'] }),
          F('switch', 'nosleep', 'Disable Wi-Fi sleep mode', { help: 'more reliable, uses a little more power' }),
          F('switch', 'bandwidth20', 'Use lower Wi-Fi bandwidth (20 MHz)')
        ] },
        { id: 'eth', icon: 'settings_ethernet', title: 'Ethernet', fields: [F('switch', 'eth_10mbit', 'Force 10 Mbit half duplex', { restart: true, help: 'only for problem switches or long cables' })] },
        { id: 'host', icon: 'dns', title: 'Hostname and access', fields: [
          F('text', 'hostname', 'Hostname', { mono: true, help: `reach it as ${NET.hostname}.local with mDNS` }),
          F('switch', 'enableMDNS', 'Enable mDNS'),
          F('switch', 'enableCORS', 'Enable CORS', { help: 'only needed when another web page calls the API' }),
          F('text', 'CORSOrigin', 'CORS origin', { show: on('enableCORS'), mono: true }),
          F('text', 'allowed_hosts', 'Allowed host names', { mono: true, full: true, ph: 'empty = any', help: 'comma separated; blocks requests that use another name' })
        ] },
        { id: 'ip', icon: 'router', title: 'IP address', fields: [
          F('switch', 'static_ip_config', 'Use a fixed IP address', { help: 'otherwise DHCP is used' }),
          F('text', 'local_ip', 'IP address', { show: on('static_ip_config'), mono: true, ph: '192.168.1.42' }),
          F('text', 'gateway_ip', 'Gateway', { show: on('static_ip_config'), mono: true, ph: '192.168.1.1' }),
          F('text', 'subnet_mask', 'Subnet mask', { show: on('static_ip_config'), mono: true, ph: '255.255.255.0' }),
          F('text', 'dns_ip_1', 'DNS 1', { show: on('static_ip_config'), mono: true }),
          F('text', 'dns_ip_2', 'DNS 2', { show: on('static_ip_config'), mono: true })
        ] }
      ]
    },
    'settings-ap': {
      store: 'ap',
      groups: [
        { id: 'mode', icon: 'wifi_tethering', title: 'When to start the Access Point', fields: [
          F('seg', 'provision_mode', '', { full: true, opts: [['Always', 'Always'], ['When network connection not found', 'When the network is lost'], ['Never', 'Never']], help: 'the Access Point lets you reach EMS-ESP when your network is down' })
        ] },
        { id: 'wifi', icon: 'wifi', title: 'Access Point', show: (v) => v.provision_mode !== 'Never', fields: [
          F('text', 'ssid', 'SSID'),
          F('password', 'password', 'Password', { help: 'at least 8 characters' }),
          F('select', 'channel', 'Preferred channel', { opts: Array.from({ length: 13 }, (_, i) => i + 1) }),
          F('number', 'max_clients', 'Max clients'),
          F('switch', 'ssid_hidden', 'Hide SSID', { full: true })
        ] },
        { id: 'ip', icon: 'router', title: 'Addresses', show: (v) => v.provision_mode !== 'Never', fields: [
          F('text', 'local_ip', 'IP address', { mono: true }),
          F('text', 'gateway_ip', 'Gateway', { mono: true }),
          F('text', 'subnet_mask', 'Subnet mask', { mono: true })
        ] }
      ]
    },
    'settings-ntp': {
      store: 'ntp',
      top: () => `<div class="section clock"><span class="ms">schedule</span><div class="grow"><div class="hero-title">${S.ntp.time}</div><div class="cap">time on EMS-ESP · synchronized 4 minutes ago</div></div>
        <button class="btn" id="ntp-set"><span class="ms sm">edit_calendar</span>Set time from this browser</button></div>`,
      groups: [
        { id: 'ntp', icon: 'access_time', title: 'Time server', fields: [
          F('switch', 'enabled', 'Synchronize the time with NTP'),
          F('text', 'server', 'NTP server', { show: on('enabled'), mono: true }),
          F('select', 'tz_label', 'Time zone', { full: true, opts: ['Europe/Amsterdam', 'Europe/Berlin', 'Europe/Brussels', 'Europe/London', 'Europe/Paris', 'Europe/Prague', 'Europe/Stockholm', 'Europe/Warsaw', 'Europe/Zurich', 'America/New_York', 'UTC'] })
        ] },
        { id: 'thermostat', icon: 'thermostat', title: 'Thermostat clock', fields: [
          F('switch', 'thermostat_sync', 'Keep the thermostat clock in sync', { help: 'sets the date and time on the EMS thermostat' }),
          F('select', 'tz_label_t', 'Thermostat time zone', { show: on('thermostat_sync'), opts: ['Same as EMS-ESP', 'UTC'] })
        ] }
      ]
    },
    'settings-mqtt': {
      store: 'mqtt',
      top: (v) => `<div class="section topic-preview"><div class="mod-head"><span class="ms lb">preview</span><h3 class="h6 grow">What gets published</h3><span class="cap">changes as you edit</span></div>
        <pre class="summary mono">${h(mqttPreview(v))}</pre></div>`,
      groups: [
        { id: 'broker', icon: 'dns', title: 'Broker', fields: [
          F('switch', 'enabled', 'Enable MQTT', { full: true }),
          F('text', 'host', 'Broker address', { show: on('enabled'), mono: true }),
          F('number', 'port', 'Port', { show: on('enabled') }),
          F('text', 'username', 'Username', { show: on('enabled') }),
          F('password', 'password', 'Password', { show: on('enabled') }),
          F('text', 'client_id', 'Client ID', { show: on('enabled'), mono: true }),
          F('text', 'base', 'Base topic', { show: on('enabled'), mono: true }),
          F('number', 'keep_alive', 'Keep alive', { show: on('enabled'), unit: 's' }),
          F('select', 'mqtt_qos', 'QoS', { show: on('enabled'), opts: ['0', '1', '2'] }),
          F('switch', 'enableTLS', 'Use TLS', { show: on('enabled'), full: true }),
          F('select', 'tlsCheck', 'Check the broker with', { show: (v) => v.enabled && v.enableTLS, opts: ['Nothing (insecure)', 'Certificate (CA)', 'Certificate fingerprint (SHA-1)'] }),
          F('textarea', 'rootCA', 'Root CA certificate', { show: (v) => v.enabled && v.enableTLS && v.tlsCheck !== 'Nothing (insecure)', full: true, ph: '-----BEGIN CERTIFICATE-----' })
        ] },
        { id: 'behaviour', icon: 'tune', title: 'Behaviour', show: on('enabled'), fields: [
          F('select', 'mqtt_commands', 'Commands received over MQTT', { full: true, opts: ['Off, only read values', 'Change values and run Commands', 'Everything, also system commands'] }),
          F('switch', 'clean_session', 'Set clean session'),
          F('switch', 'mqtt_retain', 'Always set the retain flag'),
          F('switch', 'send_response', 'Publish command output to a response topic', { full: true })
        ] },
        { id: 'format', icon: 'data_object', title: 'Topics and payloads', show: on('enabled'), fields: [
          F('select', 'nested_format', 'Topic/payload format', { full: true, opts: ['Nested in a single topic', 'As individual topics'] }),
          F('switch', 'publish_single', 'Also publish each value to its own topic on change', { disabled: on('ha_enabled'), help: 'not available with discovery on' }),
          F('switch', 'publish_single2cmd', 'Publish to command topics (ioBroker)', { show: on('publish_single') })
        ] },
        { id: 'discovery', icon: 'travel_explore', title: 'Discovery', show: on('enabled'), fields: [
          F('switch', 'ha_enabled', 'Enable MQTT discovery', { full: true, disabled: on('publish_single'), help: 'creates the entities in Home Assistant or Domoticz automatically' }),
          F('select', 'discovery_type', 'Discovery type', { show: on('ha_enabled'), opts: ['Home Assistant', 'Domoticz', 'Domoticz (latest)'] }),
          F('text', 'discovery_prefix', 'Discovery topic prefix', { show: on('ha_enabled'), mono: true }),
          F('select', 'entity_format', 'Entity ID format', { show: on('ha_enabled'), opts: ['Single instance, long name (v3.4)', 'Single instance, short name (v3.5)', 'Multiple instances, short name (v3.5)', 'Single instance, short name (latest)', 'Multiple instances, short name (latest)'], warn: true, help: 'changing this renames every entity in Home Assistant' }),
          F('select', 'ha_number_mode', 'Number input', { show: (v) => v.ha_enabled && v.discovery_type === 'Home Assistant', opts: ['Box', 'Slider'] })
        ] },
        { id: 'intervals', icon: 'timer', title: 'Publish intervals', desc: 'Seconds between publishes, 0 = publish on every change.', show: on('enabled'), fields: [
          F('number', 'publish_time_boiler', 'Boilers and heat pumps', { unit: 's' }),
          F('number', 'publish_time_thermostat', 'Thermostats', { unit: 's' }),
          F('number', 'publish_time_solar', 'Solar modules', { unit: 's' }),
          F('number', 'publish_time_mixer', 'Mixer modules', { unit: 's' }),
          F('number', 'publish_time_water', 'Water modules', { unit: 's' }),
          F('number', 'publish_time_sensor', 'Sensors', { unit: 's' }),
          F('number', 'publish_time_heartbeat', 'Heartbeat', { unit: 's' })
        ] }
      ]
    }
  };

  function mqttPreview(v) {
    if (!v.enabled) return 'MQTT is off, nothing is published.';
    const b = v.base;
    const lines = [];
    if (v.nested_format === 'Nested in a single topic') lines.push(`${b}/thermostat_data   {"hc1":{"seltemp":21.5,"mode":"auto"},"hc2":{...}}`);
    else lines.push(`${b}/thermostat_data_hc1   {"seltemp":21.5,"mode":"auto"}`, `${b}/thermostat_data_hc2   {"seltemp":19,"mode":"auto"}`);
    lines.push(`${b}/boiler_data   {"curflowtemp":58.4,"selflowtemp":60,...}`);
    if (v.publish_single) lines.push(`${b}/thermostat/hc1/seltemp   21.5`);
    if (v.mqtt_commands !== 'Off, only read values') lines.push(`${b}/thermostat/hc1/seltemp/set   ← accepted, writes to the thermostat`);
    if (v.ha_enabled) lines.push(`${v.discovery_prefix}/climate/${b}/thermostat_hc1/config   ← ${v.discovery_type} discovery`);
    if (v.send_response) lines.push(`${b}/response   ← output of commands`);
    return lines.join('\n');
  }

  let appQuery = '';
  const boardPins = (v) => {
    const p = { E32V2: [4, 5, 34, 14, 2, 'LAN8720'], E32V2_2: [4, 5, 34, 14, 2, 'LAN8720'], E32: [4, 5, 34, 14, 2, 'LAN8720'], S32: [23, 5, 0, 18, 2, 'none'], S32S3: [4, 5, 0, 14, 2, 'none'] }[v.board_profile] || [16, 17, 0, 4, 2, 'none'];
    return ['Rx', 'Tx', 'Button', '1-Wire', 'LED', 'PHY'].map((n, i) => `<span class="pin"><span class="cap">${n}</span>${p[i]}</span>`).join('');
  };

  function renderField(f, v, snap) {
    const changed = f.k && JSON.stringify(v[f.k]) !== JSON.stringify(snap[f.k]);
    const dis = f.disabled && f.disabled(v);
    const flags = `${f.restart ? '<span class="ms xs muted" title="needs a restart">restart_alt</span>' : ''}${changed ? '<span class="chg" title="changed"></span>' : ''}`;
    const help = f.help ? `<span class="cap ${f.warn ? 'warnc' : ''}">${f.help}</span>` : '';
    const cls = `field ${f.full ? 'full' : ''} ${dis ? 'disabled' : ''}`;
    const lab = f.label ? `<label>${f.label}${flags}</label>` : flags ? `<label>${flags}</label>` : '';
    const val = v[f.k];
    switch (f.type) {
      case 'switch':
        return `<div class="${cls} full swrow"><div class="switch ${val ? 'on' : ''}" data-fsw="${f.k}"></div><div class="grow"><div>${f.label}${flags}</div>${help}</div></div>`;
      case 'select':
        return `<div class="${cls}">${lab}<select class="input" data-f="${f.k}">${f.opts
          .map((o) => (Array.isArray(o) ? o : [o, o]))
          .map(([o, t]) => `<option value="${h(o)}" ${String(o) === String(val) ? 'selected' : ''}>${h(t)}</option>`)
          .join('')}</select>${help}</div>`;
      case 'seg':
        return `<div class="${cls}">${lab}<span class="toggle-group">${f.opts.map(([o, t]) => `<button class="${o === val ? 'on' : ''}" data-fseg="${f.k}" data-val="${h(o)}">${t}</button>`).join('')}</span>${help}</div>`;
      case 'textarea':
        return `<div class="${cls}">${lab}<textarea class="input mono" rows="4" data-f="${f.k}" placeholder="${h(f.ph || '')}">${h(val)}</textarea>${help}</div>`;
      case 'pins':
        return `<div class="field full"><label>Pins from the profile</label><div class="row wrap">${boardPins(v)}</div></div>`;
      default:
        return `<div class="${cls}">${lab}<div class="row"><input class="input grow ${f.mono ? 'mono' : ''}" data-f="${f.k}" data-num="${f.type === 'number' ? 1 : ''}" type="${f.type === 'password' ? 'password' : f.type === 'number' ? 'number' : 'text'}" value="${h(val ?? '')}" placeholder="${h(f.ph || '')}">${f.unit ? `<span class="muted">${f.unit}</span>` : ''}${f.type === 'password' ? '<button class="icon-btn plain" data-peek title="show"><span class="ms sm">visibility</span></button>' : ''}${f.action ? `<button class="btn sm" data-faction="${f.action[2]}"><span class="ms xs">${f.action[0]}</span>${f.action[1]}</button>` : ''}</div>${help}</div>`;
    }
  }

  const changedKeys = (store) => Object.keys(M.settings[store]).filter((k) => JSON.stringify(M.settings[store][k]) !== JSON.stringify(snaps[store][k]));

  function formPage(key) {
    const Fm = FORMS[key];
    const v = M.settings[Fm.store];
    const snap = snaps[Fm.store];
    const allFields = Fm.groups.flatMap((g) => g.fields);
    const match = (f) => !appQuery || `${f.label} ${f.help || ''} ${f.k}`.toLowerCase().includes(appQuery);
    const groups = Fm.groups
      .filter((g) => !g.show || g.show(v))
      .map((g) => ({ g, fields: g.fields.filter((f) => (!f.show || f.show(v)) && (!Fm.search || match(f))) }))
      .filter((x) => x.fields.length);
    const changed = changedKeys(Fm.store);
    const needsRestart = changed.some((k) => (allFields.find((f) => f.k === k) || {}).restart);
    const index = Fm.search
      ? `<div class="section sticky-index"><span class="search"><span class="ms">search</span><input class="input" id="set-q" placeholder="Search settings" value="${h(appQuery)}"></span>
          <span class="row wrap">${Fm.groups.map((g) => `<a class="chip clickable" data-jump="g-${g.id}"><span class="ms xs">${g.icon}</span>${g.title}</a>`).join('')}</span></div>`
      : '';
    const banner = restartPending[Fm.store]
      ? `<div class="msg warning" style="margin:12px 12px 0"><span class="ms">restart_alt</span><span class="grow">Saved. Restart EMS-ESP to use the new settings.</span><button class="btn sm warning" id="do-restart">Restart now</button></div>`
      : '';
    return `${banner}${Fm.top ? (Fm.top(v).startsWith('<div class="msg') ? `<div style="margin:12px 12px 0">${Fm.top(v)}</div>` : Fm.top(v)) : ''}${index}
      ${groups
        .map(({ g, fields }) => `<div class="section" id="g-${g.id}"><div class="mod-head"><span class="ms lb">${g.icon}</span><h3 class="h6 grow">${g.title}</h3></div>
          ${g.desc ? `<p class="cap" style="margin:4px 0 0">${g.desc}</p>` : ''}<div class="form sform">${fields.map((f) => renderField(f, v, snap)).join('')}</div></div>`)
        .join('') || '<div class="section muted">No settings match your search.</div>'}
      ${changed.length
        ? `<div class="savebar"><span class="ms">edit_note</span><span>${plural(changed.length, 'changed setting')}${needsRestart ? ' · <span class="warnc">needs a restart</span>' : ''}</span><span class="grow"></span>
          <button class="btn secondary" data-fcancel="${Fm.store}"><span class="ms sm">undo</span>Cancel</button>
          <button class="btn contained" data-fsave="${Fm.store}" data-restart="${needsRestart ? 1 : ''}"><span class="ms sm">check</span>Save</button></div>`
        : ''}`;
  }

  // ---------- Network with Wi-Fi scan tab ----------
  let netTab = 'settings';
  function network() {
    const tabs = subTabs([['settings', 'Settings', 'settings_ethernet'], ['scan', 'Wi-Fi Scan', 'wifi_find']], netTab, 'data-ntab');
    if (netTab === 'settings') return tabs + formPage('settings-network');
    const bars = (rssi) => (rssi > -60 ? 4 : rssi > -70 ? 3 : rssi > -80 ? 2 : 1);
    return `${tabs}<div class="section">
      <div class="mod-head"><span class="ms lb">wifi_find</span><h3 class="h6 grow">Networks nearby</h3><button class="btn sm" id="rescan"><span class="ms xs">refresh</span>Scan again</button></div>
      <table class="tbl mtbl" style="margin-top:10px"><thead><tr><th>Network</th><th>Signal</th><th>Channel</th><th>Security</th><th></th></tr></thead><tbody>
      ${M.wifiScan
        .map((n) => `<tr data-pick-ssid="${h(n.ssid)}"><td>${h(n.ssid)} ${n.ssid === NET.ssid ? '<span class="tag mod">saved</span>' : ''}</td>
          <td><span class="sig s${bars(n.rssi)}"><i></i><i></i><i></i><i></i></span> <span class="cap">${n.rssi} dBm</span></td>
          <td>${n.channel}</td><td>${n.secure ? '<span class="ms xs">lock</span> WPA2' : '<span class="warnc">open</span>'}</td>
          <td class="r"><button class="btn sm">Use</button></td></tr>`)
        .join('')}</tbody></table>
    </div>`;
  }

  // ---------- Security ----------
  let secTab = 'users';
  let newKey = null;
  function security() {
    const tabs = subTabs([['users', 'Users', 'group'], ['apikeys', 'API Keys', 'key']], secTab, 'data-stab');
    if (secTab === 'users') {
      const admins = M.users.filter((u) => u.admin).length;
      return `${tabs}<div class="section">
        <div class="mod-head"><div class="grow"><h2 class="sec-title">Users</h2><div class="cap">Admins can change everything. Guests can look, and run commands marked for guests.</div></div>
          <button class="btn contained" id="user-add"><span class="ms sm">person_add</span>Add user</button></div>
        ${admins ? '' : '<div class="msg warning" style="margin-top:10px"><span class="ms">warning</span>You must have at least one admin user.</div>'}
        <table class="tbl mtbl" style="margin-top:10px"><thead><tr><th>User</th><th>Role</th><th></th></tr></thead><tbody>
        ${M.users
          .map((u, i) => `<tr data-user="${i}"><td><span class="ms sm muted">person</span> ${h(u.username)} ${u.username === 'admin' ? '<span class="cap">· you</span>' : ''}</td>
            <td>${u.admin ? '<span class="tag mod">admin</span>' : '<span class="tag sys">guest</span>'}</td>
            <td class="r"><button class="icon-btn" title="Edit"><span class="ms sm">edit</span></button></td></tr>`)
          .join('')}</tbody></table>
      </div>`;
    }
    return `${tabs}<div class="section">
      <div class="mod-head"><div class="grow"><h2 class="sec-title">API Keys</h2><div class="cap">For Home Assistant and scripts. Send the key as <span class="mono">Authorization: Bearer &lt;key&gt;</span>.</div></div>
        <button class="btn contained" id="key-add"><span class="ms sm">add</span>Create key</button></div>
      ${newKey ? `<div class="msg info" style="margin-top:10px"><span class="ms">key</span><span class="grow">Copy this key now and keep it safe. It will not be shown again.<div class="mono lb" style="margin-top:4px">${newKey}</div></span><button class="btn sm" id="key-copy"><span class="ms xs">content_copy</span>Copy</button></div>` : ''}
      <table class="tbl mtbl" style="margin-top:10px"><thead><tr><th>Name</th><th>Access</th><th>Key</th><th>Created</th><th></th></tr></thead><tbody>
      ${M.apikeys
        .map((k) => `<tr><td class="mono">${h(k.name)}</td><td>${k.access === 'Read' ? '<span class="tag sys">read</span>' : '<span class="tag mod">read and write</span>'}</td>
          <td class="mono muted">emsesp_••••••••${k.tail}</td><td class="cap">${k.created}</td>
          <td class="r"><button class="icon-btn" data-key-rm="${k.id}" title="Revoke"><span class="ms sm">delete</span></button></td></tr>`)
        .join('') || '<tr><td colspan="5" class="muted">no API keys</td></tr>'}</tbody></table>
      <p class="cap" style="margin:10px 0 0">Read keys can read values and run guest commands. Read and write keys can do everything an admin can do through the API.</p>
    </div>`;
  }

  function editUser(i, route) {
    const creating = i === null;
    const u = creating ? { username: '', password: '', admin: false } : { ...M.users[i], password: '' };
    Modules.openForm({
      title: creating ? 'Add User' : 'Edit User',
      width: 440,
      values: u,
      fields: [
        { k: 'username', label: 'Username', full: true },
        { k: 'password', label: creating ? 'Password' : 'New password', type: 'password', full: true, ph: creating ? '' : 'leave empty to keep the current one' },
        { k: 'admin', label: 'Admin', type: 'switch', full: true }
      ],
      okLabel: creating ? 'Add' : 'Update',
      validate: (v) => (!v.username.trim() ? 'Username is required' : creating && !v.password ? 'Password is required' : ''),
      onRemove: creating ? null : () => { M.users.splice(i, 1); toast('User removed'); route(); },
      onSave: (v) => {
        if (creating) M.users.push({ username: v.username, admin: v.admin });
        else Object.assign(M.users[i], { username: v.username, admin: v.admin });
        toast('Users saved');
        route();
      }
    });
  }

  // ---------- Modules (library modules) ----------
  function modulesSettings() {
    return `<div class="section">
      <div class="mod-head"><div class="grow"><h2 class="sec-title">Modules</h2><div class="cap">Optional features built into this firmware. Turn them on or off here.</div></div></div>
      <div class="mod-cards">${M.modules
        .map((m) => `<div class="mod-card ${m.enabled ? 'on' : ''}">
          <div class="row"><span class="ms lb">extension</span><span class="h6 grow">${h(m.name)}</span><div class="switch ${m.enabled ? 'on' : ''}" data-mod="${m.id}"></div></div>
          <div class="cap" style="margin:6px 0">v${m.version} · by ${h(m.author)}</div>
          <div class="row"><span class="pill ${m.enabled ? 'ok' : m.message.includes('license') ? 'warn' : 'ro'}"><span class="dot ${m.enabled ? 'ok' : 'off'}"></span>${m.enabled ? 'active' : 'off'} · ${h(m.message)}</span></div>
          ${m.message.includes('license') ? `<div class="row" style="margin-top:10px"><input class="input grow mono" placeholder="license key" data-lic="${m.id}"><button class="btn sm" data-lic-save="${m.id}">Save</button></div>` : ''}
        </div>`)
        .join('')}</div>
    </div>`;
  }

  // ---------- Firmware ----------
  let channel = 'dev', installing = null;
  function version() {
    const fw = M.firmware;
    const latest = fw[channel];
    const isNewer = latest.version !== S.version;
    return `<div class="section">
      <div class="mod-head"><span class="ms lb">inventory_2</span><h3 class="h6 grow">Installed</h3>${fw.official ? '<span class="tag guest"><span class="ms xs">verified</span>official build</span>' : '<span class="tag warn">custom build</span>'}</div>
      <table class="tbl mtbl" style="margin-top:10px"><tbody>
        <tr><td><b>EMS-ESP</b> <span class="cap">${fw.partition} · running</span></td><td class="mono">${S.version}</td><td><span class="tag type">development</span></td><td class="r"></td></tr>
        <tr><td>Previous version <span class="cap">${fw.other.partition}</span></td><td class="mono">${fw.other.version}</td><td><span class="tag sys">stable</span></td>
          <td class="r"><button class="btn sm secondary" id="fw-switch"><span class="ms xs">swap_horiz</span>Switch back</button></td></tr>
        <tr><td>Recovery app <span class="cap">factory partition</span></td><td class="mono">${fw.recovery}</td><td><span class="tag sys">recovery</span></td>
          <td class="r"><button class="btn sm secondary" id="fw-recovery"><span class="ms xs">build</span>Restart into recovery</button></td></tr>
      </tbody></table>
    </div>
    <div class="section">
      <div class="mod-head"><span class="ms lb">system_update</span><h3 class="h6 grow">Updates</h3>
        <span class="toggle-group"><button class="${channel === 'stable' ? 'on' : ''}" data-chan="stable">Stable</button><button class="${channel === 'dev' ? 'on' : ''}" data-chan="dev">Development</button></span></div>
      <div class="update-card">
        <span class="ms big">${isNewer ? 'new_releases' : 'check_circle'}</span>
        <div class="grow"><div class="hero-title">${isNewer ? `${latest.version} is available` : 'You are up to date'}</div>
          <div class="cap">${channel === 'stable' ? 'stable release' : 'development build'} · ${latest.date} · <a>release notes</a> · checked automatically every day</div>
          ${installing !== null ? `<div class="progress"><div style="width:${installing}%"></div></div><div class="cap">downloading and installing... ${installing}%</div>` : ''}</div>
        ${isNewer ? `<button class="btn contained" id="fw-install" ${installing !== null ? 'disabled' : ''}><span class="ms sm">download</span>Install ${latest.version}</button>` : ''}
      </div>
      ${channel === 'stable' ? '<p class="cap" style="margin:10px 0 0">Going from a development build to stable installs an older version number. Your settings are kept.</p>' : ''}
    </div>
    <div class="section">
      <div class="mod-head"><span class="ms lb">upload_file</span><h3 class="h6 grow">Upload firmware</h3><span class="cap">for a .bin file you downloaded yourself</span></div>
      <div class="drop" id="fw-drop"><span class="ms">upload_file</span>Drop a firmware .bin file here, or click to select one</div>
    </div>
    <div class="section">
      <div class="mod-head"><span class="ms lb">power_settings_new</span><h3 class="h6 grow">Restart and reset</h3></div>
      <div class="row wrap" style="margin-top:10px">
        <button class="btn warning" id="v-restart"><span class="ms sm">restart_alt</span>Restart</button>
        <span class="grow"></span>
        ${APP.disable_reset ? '<span class="cap">Factory reset is disabled in Application Settings</span>' : '<button class="btn error contained" id="v-reset"><span class="ms sm">delete_forever</span>Factory Reset</button>'}
      </div>
    </div>`;
  }

  function installDialog(route) {
    const latest = M.firmware[channel];
    confirmDialog(
      `Install ${latest.version}`,
      `EMS-ESP downloads the firmware and restarts, which takes about a minute. The heating keeps running.<br><br>
       <span class="cap">Recommended: <a data-nav="downloadupload">download a system backup</a> first.</span>`,
      '<span class="ms sm">download</span>Install',
      '',
      () => {
        installing = 0;
        route();
        const t = setInterval(() => {
          installing += 10;
          if (installing >= 100) { clearInterval(t); installing = null; toast('Installed. Restarting...'); }
          if (location.hash === '#settings-version') route();
        }, 400);
      }
    );
  }

  // ---------- wiring ----------
  function refocus(el, route, sel, after) {
    const pos = el.selectionStart;
    after();
    route();
    const n = document.querySelector(sel);
    if (n) { n.focus(); if (pos !== null && n.setSelectionRange && n.type !== 'number') n.setSelectionRange(pos, pos); }
  }

  function wireForm(el, route) {
    const Fm = FORMS[location.hash.slice(1)];
    if (!Fm) return;
    const v = M.settings[Fm.store];
    el.querySelectorAll('[data-f]').forEach((i) => {
      const evt = i.tagName === 'SELECT' ? 'change' : 'input';
      i.addEventListener(evt, () => {
        const num = i.dataset.num || typeof v[i.dataset.f] === 'number';
        const val = num ? (i.value === '' ? '' : +i.value) : i.value;
        if (i.tagName === 'SELECT') { v[i.dataset.f] = val; route(); }
        else refocus(i, route, `[data-f="${i.dataset.f}"]`, () => (v[i.dataset.f] = val));
      });
    });
    el.querySelectorAll('[data-fsw]').forEach((s) => s.addEventListener('click', () => {
      if (s.closest('.field.disabled')) return toast('Not available with the current settings');
      v[s.dataset.fsw] = !v[s.dataset.fsw];
      route();
    }));
    el.querySelectorAll('[data-fseg]').forEach((b) => b.addEventListener('click', () => { v[b.dataset.fseg] = b.dataset.val; route(); }));
    el.querySelectorAll('[data-peek]').forEach((b) => b.addEventListener('click', () => {
      const inp = b.parentElement.querySelector('input');
      inp.type = inp.type === 'password' ? 'text' : 'password';
    }));
    el.querySelectorAll('[data-fcancel]').forEach((b) => b.addEventListener('click', () => {
      Object.assign(v, clone(snaps[b.dataset.fcancel]));
      route();
    }));
    el.querySelectorAll('[data-fsave]').forEach((b) => b.addEventListener('click', () => {
      snaps[b.dataset.fsave] = clone(v);
      if (b.dataset.restart) restartPending[b.dataset.fsave] = true;
      else toast('Settings saved');
      route();
    }));
    const rs = el.querySelector('#do-restart');
    if (rs) rs.onclick = () => { delete restartPending[Fm.store]; toast('Restarting...'); route(); };
    el.querySelectorAll('[data-jump]').forEach((a) => a.addEventListener('click', () => document.getElementById(a.dataset.jump)?.scrollIntoView({ behavior: 'smooth', block: 'start' })));
    const q = el.querySelector('#set-q');
    if (q) q.addEventListener('input', () => refocus(q, route, '#set-q', () => (appQuery = q.value.toLowerCase())));
    el.querySelectorAll('[data-faction="scan"]').forEach((b) => b.addEventListener('click', () => { netTab = 'scan'; route(); }));
  }

  function setTimeDialog() {
    confirmDialog('Set Time', `Set the EMS-ESP clock to this browser's time?<div class="mono lb" style="margin-top:8px">${new Date().toLocaleString('en-GB')}</div>
      <span class="cap">NTP will correct it again at the next sync if NTP is on.</span>`, '<span class="ms sm">check</span>Set time', '', () => toast('Time set'));
  }

  function afterRender(key, el, route) {
    wireForm(el, route);
    const click = (sel, fn) => el.querySelectorAll(sel).forEach((n) => n.addEventListener('click', (ev) => fn(n, ev)));
    click('#st-restart, #v-restart', () => confirmDialog('Restart', 'Restart EMS-ESP now? The web UI reconnects by itself.', '<span class="ms sm">restart_alt</span>Restart', 'warning', () => toast('Restarting...')));
    click('#st-reset, #v-reset', () => confirmDialog('Factory Reset', 'This erases all settings, customizations, schedules and users, then restarts in Access Point mode. Download a system backup first.', '<span class="ms sm">delete_forever</span>Factory Reset', 'error', () => toast('Factory reset started')));
    click('#ntp-set', setTimeDialog);

    if (key === 'status-log') {
      const lv = el.querySelector('#lg-level');
      lv.onchange = () => { logLevel = lv.value; route(); };
      const q = el.querySelector('#lg-q');
      q.addEventListener('input', () => refocus(q, route, '#lg-q', () => (logQuery = q.value.toLowerCase())));
      el.querySelector('#lg-compact').onclick = () => { compact = !compact; route(); };
      el.querySelector('#lg-scroll').onclick = () => { autoScroll = !autoScroll; route(); };
      el.querySelector('#lg-pause').onclick = () => { paused = !paused; route(); };
      el.querySelector('#lg-dl').onclick = () => toast('Download successful');
      const run = () => {
        const c = el.querySelector('#lg-cmd');
        if (!c.value.trim()) return;
        M.log.push([new Date().toTimeString().slice(0, 8) + '.000', 'INFO', 'shell', `$ ${c.value} (output shown here)`]);
        route();
      };
      el.querySelector('#lg-run').onclick = run;
      el.querySelector('#lg-cmd').addEventListener('keydown', (e) => e.key === 'Enter' && run());
    }

    if (key === 'settings-network') {
      click('[data-ntab]', (b) => { netTab = b.dataset.ntab; route(); });
      click('[data-pick-ssid]', (r) => { NET.ssid = r.dataset.pickSsid; netTab = 'settings'; route(); toast(`Selected ${NET.ssid}, enter its password`); });
      click('#rescan', () => toast('Scanning...'));
    }

    if (key === 'settings-security') {
      click('[data-stab]', (b) => { secTab = b.dataset.stab; newKey = null; route(); });
      click('[data-user]', (r) => editUser(+r.dataset.user, route));
      click('#user-add', () => editUser(null, route));
      click('#key-add', () =>
        Modules.openForm({
          title: 'Create API Key',
          width: 440,
          values: { name: '', access: 'Read' },
          fields: [
            { k: 'name', label: 'Name', full: true, ph: 'e.g. home_assistant' },
            { k: 'access', label: 'Access', type: 'seg', full: true, opts: [['Read', 'Read', 'visibility'], ['Read and Write', 'Read and Write', 'edit']] }
          ],
          okLabel: 'Create',
          okIcon: 'key',
          validate: (v) => (!v.name.trim() ? 'Name is required' : ''),
          onSave: (v) => {
            const tail = Math.random().toString(16).slice(2, 6);
            newKey = `emsesp_${Math.random().toString(16).slice(2, 14)}${Math.random().toString(16).slice(2, 14)}${tail}`;
            M.apikeys.push({ id: Date.now(), name: v.name, access: v.access, tail, created: '2026-11-16' });
            route();
          }
        })
      );
      click('#key-copy', () => { navigator.clipboard?.writeText(newKey); toast('Key copied'); });
      click('[data-key-rm]', (b) => confirmDialog('Revoke API Key', 'Anything using this key stops working straight away.', '<span class="ms sm">delete</span>Revoke', 'error', () => {
        M.apikeys = M.apikeys.filter((k) => k.id != b.dataset.keyRm);
        route();
      }));
    }

    if (key === 'settings-modules') {
      click('[data-mod]', (s) => {
        const m = M.modules.find((x) => x.id == s.dataset.mod);
        if (!m.enabled && m.message.includes('license')) return toast('Enter a license key first');
        m.enabled = !m.enabled;
        m.message = m.enabled ? 'running' : 'stopped';
        toast(`${m.name} ${m.enabled ? 'enabled' : 'disabled'}`);
        route();
      });
      click('[data-lic-save]', (b) => {
        const m = M.modules.find((x) => x.id == b.dataset.licSave);
        if (!el.querySelector(`[data-lic="${m.id}"]`).value) return toast('Enter a license key');
        m.message = 'license accepted';
        route();
      });
    }

    if (key === 'settings-version') {
      click('[data-chan]', (b) => { channel = b.dataset.chan; route(); });
      click('#fw-install', () => installDialog(route));
      click('#fw-switch', () => confirmDialog('Switch back', `Restart into ${M.firmware.other.version} from ${M.firmware.other.partition}? Settings saved by a newer version may not all be understood.`, '<span class="ms sm">swap_horiz</span>Switch', 'warning', () => toast('Restarting into the previous version...')));
      click('#fw-recovery', () => confirmDialog('Recovery app', 'Restart into the recovery app? It starts an Access Point where you can upload firmware.', '<span class="ms sm">build</span>Restart', 'warning', () => toast('Restarting into recovery...')));
      click('#fw-drop', () => toast('Not part of this mockup'));
    }
  }

  const views = {
    status,
    'status-hardware': hardware,
    'status-activity': activity,
    'status-log': log,
    'status-network': networkStatus,
    'status-mqtt': mqttStatus,
    'status-ntp': ntpStatus,
    'status-ap': apStatus,
    settings,
    'settings-application': () => formPage('settings-application'),
    'settings-network': network,
    'settings-ap': () => formPage('settings-ap'),
    'settings-ntp': () => formPage('settings-ntp'),
    'settings-mqtt': () => formPage('settings-mqtt'),
    'settings-security': security,
    'settings-modules': modulesSettings,
    'settings-version': version
  };
  const titles = {
    'status-hardware': 'Hardware',
    'status-activity': 'Data Traffic',
    'status-log': 'System Log',
    'status-network': 'Network',
    'status-mqtt': 'MQTT',
    'status-ntp': 'NTP',
    'status-ap': 'Access Point',
    'settings-application': 'Application',
    'settings-network': 'Network',
    'settings-ap': 'Access Point',
    'settings-ntp': 'NTP',
    'settings-mqtt': 'MQTT',
    'settings-security': 'Security',
    'settings-modules': 'Modules',
    'settings-version': 'Firmware'
  };
  const parents = Object.fromEntries(Object.keys(titles).map((k) => [k, k.split('-')[0]]));
  parents.downloadupload = 'settings';
  parents.restore = 'downloadupload';

  window.System = { views, titles, parents, afterRender };
})();
