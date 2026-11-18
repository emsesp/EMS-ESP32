// Module pages for concept A: Sensors, Customizations, Commands, Scheduler, Custom Entities.
// Edits stay pending until "Apply changes", like the real UI, but show in the rows and a sticky save bar.
(function () {
  const { h, fmt, toast, M } = Shell;
  const clone = (x) => JSON.parse(JSON.stringify(x));
  const strip = (x) => JSON.stringify({ ...x, _st: undefined });
  const plural = (n, w) => `${n} ${w}${n === 1 ? '' : 's'}`;

  // ---------- pending changes ----------
  const stores = {
    sensors: () => [M.sensors.temp, M.sensors.analog],
    commands: () => [M.commands],
    scheduler: () => [M.schedules],
    customentities: () => [M.custom]
  };
  const snaps = {};
  const snap = (key) => (snaps[key] = stores[key]().map(clone));
  Object.keys(stores).forEach(snap);

  function stateOf(key, listIdx, item) {
    if (item._st === 'del') return 'del';
    const o = snaps[key][listIdx].find((x) => x.id === item.id);
    if (!o) return 'new';
    return strip(item) !== strip(o) ? 'mod' : '';
  }
  const pending = (key) => stores[key]().reduce((n, a, i) => n + a.filter((x) => stateOf(key, i, x)).length, 0);
  function cancel(key) {
    stores[key]().forEach((a, i) => a.splice(0, a.length, ...clone(snaps[key][i])));
  }
  function apply(key) {
    stores[key]().forEach((a) => {
      for (let i = a.length - 1; i >= 0; i--) if (a[i]._st === 'del') a.splice(i, 1);
      a.forEach((x) => delete x._st);
    });
    snap(key);
  }
  const nextId = (a) => a.reduce((m, x) => Math.max(m, typeof x.id === 'number' ? x.id : 0), 0) + 1;

  // customizations keep their own snapshot of mask, custom name and limits per entity
  const ckey = (d, e) => `${d.id}:${e.id}`;
  let csnap = {};
  const cpick = (e) => ({ m: e.m, cn: e.cn || '', mi: e.mi, ma: e.ma });
  const csnapAll = () => {
    csnap = {};
    M.devices.forEach((d) => d.entities.forEach((e) => (csnap[ckey(d, e)] = cpick(e))));
  };
  csnapAll();
  const cChanged = (d, e) => JSON.stringify(cpick(e)) !== JSON.stringify(csnap[ckey(d, e)]);
  const cPending = () => M.devices.reduce((n, d) => n + d.entities.filter((e) => cChanged(d, e)).length, 0);

  const saveBar = (key, n) =>
    n
      ? `<div class="savebar"><span class="ms">edit_note</span><span>${plural(n, 'unsaved change')}</span><span class="grow"></span>
        <button class="btn secondary" data-cancel="${key}"><span class="ms sm">undo</span>Cancel</button>
        <button class="btn contained" data-apply="${key}"><span class="ms sm">check</span>Apply changes</button></div>`
      : '';
  const stCls = (st) => (st ? `st-${st}` : '');
  const stTag = (st) => (st === 'new' ? '<span class="tag new">new</span>' : st === 'mod' ? '<span class="tag mod">changed</span>' : '');
  const undoBtn = (attr) => `<button class="btn sm secondary" ${attr}><span class="ms xs">undo</span>Undo</button>`;

  const head = (title, cap, right = '') =>
    `<div class="mod-head"><div class="grow"><h2 class="sec-title">${title}</h2><div class="cap">${cap}</div></div>${right}</div>`;
  const search = (id, val, ph = 'Search') =>
    `<span class="search"><span class="ms">search</span><input class="input" id="${id}" placeholder="${ph}" value="${h(val)}"></span>`;
  const addBtn = (id, label = 'Add') => `<button class="btn contained" id="${id}"><span class="ms sm">add</span>${label}</button>`;

  // ---------- generic form dialog ----------
  function field(f, v) {
    const lab = `<label>${f.label}</label>`;
    const help = f.help ? `<span class="cap" style="margin-top:4px">${f.help}</span>` : '';
    const cls = `field ${f.full ? 'full' : ''}`;
    switch (f.type) {
      case 'select':
        return `<div class="${cls}">${lab}<select class="input" data-k="${f.k}">${f.opts
          .map((o) => (Array.isArray(o) ? o : [o, o]))
          .map(([val, txt]) => `<option value="${h(val)}" ${String(val) === String(v) ? 'selected' : ''}>${h(txt)}</option>`)
          .join('')}</select>${help}</div>`;
      case 'switch':
        return `<div class="${cls} sw"><div class="row"><div class="switch ${v ? 'on' : ''}" data-sw="${f.k}"></div><span>${f.label}</span></div>${help}</div>`;
      case 'seg':
        return `<div class="${cls}">${lab}<span class="toggle-group">${f.opts
          .map(([val, txt, icon]) => `<button class="${val === v ? 'on' : ''}" data-seg="${f.k}" data-val="${val}">${icon ? `<span class="ms xs">${icon}</span>` : ''}${txt}</button>`)
          .join('')}</span>${help}</div>`;
      case 'days':
        return `<div class="${cls}">${lab}<div class="row">${DAY_ORDER.map((d) => `<button class="daypill ${v.includes(d) ? 'on' : ''}" data-day="${d}">${DAYS[d]}</button>`).join('')}
          <span class="grow"></span><button class="btn sm secondary" data-days="wk">Mon-Fri</button><button class="btn sm secondary" data-days="all">Every day</button></div>${help}</div>`;
      case 'info':
        return `<div class="${cls}">${lab}<div class="mono lb" style="padding:8px 0">${h(v)}</div>${help}</div>`;
      default:
        return `<div class="${cls}">${lab}<div class="row"><input class="input grow ${f.mono ? 'mono' : ''}" data-k="${f.k}" type="${f.type || 'text'}" ${f.step ? `step="${f.step}"` : ''} value="${h(v ?? '')}" placeholder="${h(f.ph || '')}">${f.unit ? `<span class="muted">${f.unit}</span>` : ''}</div>${help}</div>`;
    }
  }

  function openForm(cfg) {
    const v = clone(cfg.values);
    const bd = document.getElementById('dialog');
    const close = () => bd.classList.remove('open');
    let err = '';
    const render = () => {
      const fields = cfg.fields.filter((f) => !f.show || f.show(v));
      bd.innerHTML = `<div class="dialog" style="width:${cfg.width || 520}px" onclick="event.stopPropagation()">
        <h2>${cfg.title}</h2>
        <div class="content">${cfg.intro ? cfg.intro(v) : ''}<div class="form">${fields.map((f) => field(f, v[f.k])).join('')}</div>
          ${err ? `<div class="msg warning" style="margin:12px 0 0"><span class="ms">error</span>${err}</div>` : ''}</div>
        <div class="actions">
          ${cfg.onRemove ? '<button class="btn danger" data-rm><span class="ms sm">delete</span>Remove</button>' : ''}
          ${(cfg.extra || []).map((x, i) => `<button class="btn" data-x="${i}"><span class="ms sm">${x.icon}</span>${x.label}</button>`).join('')}
          <span class="grow"></span>
          <button class="btn secondary" data-close><span class="ms sm">cancel</span>Cancel</button>
          <button class="btn contained" data-ok><span class="ms sm">${cfg.okIcon || 'check'}</span>${cfg.okLabel || 'Update'}</button>
        </div></div>`;
      bd.querySelectorAll('[data-k]').forEach((i) =>
        i.addEventListener(i.tagName === 'SELECT' ? 'change' : 'input', () => {
          v[i.dataset.k] = i.type === 'number' ? (i.value === '' ? '' : +i.value) : i.value;
          if (i.tagName === 'SELECT') render();
        })
      );
      bd.querySelectorAll('[data-sw]').forEach((s) => (s.onclick = () => { v[s.dataset.sw] = !v[s.dataset.sw]; render(); }));
      bd.querySelectorAll('[data-seg]').forEach((b) => (b.onclick = () => { v[b.dataset.seg] = b.dataset.val; render(); }));
      bd.querySelectorAll('[data-day]').forEach((b) => (b.onclick = () => {
        const d = +b.dataset.day;
        v.days = v.days.includes(d) ? v.days.filter((x) => x !== d) : [...v.days, d];
        render();
      }));
      bd.querySelectorAll('[data-days]').forEach((b) => (b.onclick = () => { v.days = b.dataset.days === 'wk' ? [1, 2, 3, 4, 5] : [0, 1, 2, 3, 4, 5, 6]; render(); }));
      bd.querySelector('[data-close]').onclick = close;
      bd.querySelectorAll('.content [data-nav]').forEach((a) => a.addEventListener('click', close));
      bd.querySelector('[data-ok]').onclick = () => {
        err = cfg.validate ? cfg.validate(v) || '' : '';
        if (err) return render();
        close();
        cfg.onSave(v);
      };
      const rm = bd.querySelector('[data-rm]');
      if (rm) rm.onclick = () => { close(); cfg.onRemove(); };
      bd.querySelectorAll('[data-x]').forEach((b) => (b.onclick = () => { const x = cfg.extra[+b.dataset.x]; if (x.close !== false) close(); x.run(v); }));
    };
    bd.onclick = close;
    render();
    bd.classList.add('open');
  }

  // ---------- Sensors ----------
  let sQuery = '';
  const NUMERIC = ['Counter', 'ADC In', 'Timer', 'Rate', 'NTC Temp', 'Pulse'];

  function sensors() {
    const S = M.sensors;
    const match = (x) => !sQuery || `${x.name} ${x.id} ${x.gpio ?? ''}`.toLowerCase().includes(sQuery);
    const temps = S.temp.filter(match);
    const analogs = S.analog.filter(match);
    const tRows = temps
      .map((t) => {
        const st = stateOf('sensors', 0, t);
        return `<tr class="${stCls(st)}" data-ts="${t.id}">
          <td>${h(t.name)} ${t.sys ? '<span class="tag sys">system</span>' : ''}${stTag(st)}</td>
          <td class="mono muted">${t.id}</td>
          <td class="r ${t.o ? 'lb' : 'muted'}">${t.o > 0 ? '+' : ''}${t.o} °C</td>
          <td class="r val">${fmt(t)}</td></tr>`;
      })
      .join('');
    const aRows = analogs
      .map((a) => {
        const st = stateOf('sensors', 1, a);
        if (st === 'del')
          return `<tr class="st-del"><td><span class="gpio">${a.gpio}</span></td><td>${h(a.name)}</td><td colspan="2" class="muted">removed when you apply</td><td class="r">${undoBtn(`data-aundo="${a.id}"`)}</td></tr>`;
        const out = a.type === 'Digital Out';
        const val = out
          ? `<div class="switch ${a.v ? 'on' : ''}" data-aout="${a.id}" title="switch output" style="margin-left:auto"></div>`
          : typeof a.v === 'boolean'
            ? (a.v ? 'on' : 'off')
            : fmt(a);
        const calc = NUMERIC.includes(a.type) && (a.f !== 1 || a.o) ? `×${a.f}${a.o ? ` ${a.o > 0 ? '+' : ''}${a.o}` : ''}` : '';
        return `<tr class="${stCls(st)}" data-as="${a.id}">
          <td><span class="gpio">${a.gpio}</span></td>
          <td>${h(a.name)} ${a.sys ? '<span class="tag sys">system</span>' : ''}${stTag(st)}</td>
          <td><span class="tag type">${a.type}</span></td>
          <td class="mono muted">${calc}</td>
          <td class="r val">${val}</td></tr>`;
      })
      .join('');
    return `<div class="section">
      ${head('Sensors', `${plural(S.temp.length, 'temperature sensor')} on the 1-Wire bus · ${plural(S.analog.length, 'analog sensor')} · free GPIOs ${S.gpios.join(', ')}`, search('sq', sQuery, 'Search name, ID or GPIO'))}
    </div>
    <div class="section">
      <div class="mod-head"><span class="ms lb">device_thermostat</span><h3 class="h6 grow">Temperature Sensors <span class="muted">(${S.temp.length})</span></h3>
        <span class="cap">found automatically · click to rename or set an offset</span></div>
      <table class="tbl mtbl"><thead><tr><th>Name</th><th>ID</th><th class="r">Offset</th><th class="r">Value</th></tr></thead>
        <tbody>${tRows || '<tr><td colspan="4" class="muted">no sensors match</td></tr>'}</tbody></table>
    </div>
    <div class="section">
      <div class="mod-head"><span class="ms lb">sensors</span><h3 class="h6 grow">Analog Sensors <span class="muted">(${S.analog.length})</span></h3>${addBtn('as-add')}</div>
      <table class="tbl mtbl"><thead><tr><th style="width:64px">GPIO</th><th>Name</th><th>Type</th><th>Calculation</th><th class="r">Value</th></tr></thead>
        <tbody>${aRows || '<tr><td colspan="5" class="muted">no sensors match</td></tr>'}</tbody></table>
    </div>
    ${saveBar('sensors', pending('sensors'))}`;
  }

  function editTemp(t, route) {
    openForm({
      title: 'Edit Temperature Sensor',
      values: t,
      intro: () => `<p class="cap" style="margin:12px 0 0">ID <span class="mono lb">${t.id}</span> · reading ${fmt(t)}</p>`,
      fields: [
        { k: 'name', label: 'Name', full: true, help: 'used for the MQTT topic and Home Assistant entity' },
        { k: 'o', label: 'Offset', type: 'number', step: 0.1, unit: '°C', help: 'added to every reading' },
        { k: 'sys', label: 'System sensor', type: 'switch' }
      ],
      validate: (v) => (!v.name.trim() ? 'Name is required' : ''),
      onSave: (v) => { Object.assign(t, v); route(); }
    });
  }

  function editAnalog(a, route) {
    const creating = !a;
    const S = M.sensors;
    const item = a || { id: nextId(S.analog), gpio: S.gpios[0], name: '', v: 0, u: '', type: 'ADC In', f: 1, o: 0, sys: false };
    const gpios = [...new Set([item.gpio, ...S.gpios])].sort((x, y) => x - y);
    openForm({
      title: creating ? 'Add Analog Sensor' : 'Edit Analog Sensor',
      values: item,
      fields: [
        { k: 'name', label: 'Name', full: true },
        { k: 'gpio', label: 'GPIO', type: 'select', opts: gpios.map((g) => [g, `GPIO ${g}`]), help: 'only free pins are listed' },
        { k: 'type', label: 'Type', type: 'select', opts: S.types },
        { k: 'u', label: 'Unit', show: (v) => NUMERIC.includes(v.type), ph: 'e.g. bar' },
        { k: 'f', label: 'Factor', type: 'number', step: 0.001, show: (v) => NUMERIC.includes(v.type) },
        { k: 'o', label: 'Offset', type: 'number', step: 0.1, show: (v) => NUMERIC.includes(v.type), help: 'value = raw × factor + offset' },
        { k: 'sys', label: 'System sensor', type: 'switch', full: true }
      ],
      okLabel: creating ? 'Add' : 'Update',
      okIcon: creating ? 'add' : 'check',
      validate: (v) => (!v.name.trim() ? 'Name is required' : ''),
      onRemove: creating ? null : () => { item._st = 'del'; route(); },
      onSave: (v) => {
        v.gpio = +v.gpio;
        if (v.type === 'Digital Out' || v.type === 'Digital In') v.v = !!v.v;
        if (creating) S.analog.push(v);
        else Object.assign(item, v);
        route();
      }
    });
  }

  // ---------- Customizations ----------
  const MASKS = [
    { bit: 8, icon: 'star', label: 'Favorite', help: 'show on the Dashboard' },
    { bit: 4, icon: 'edit_off', label: 'Write disabled', help: 'nothing can change this value, including a heating settings restore', writable: true },
    { bit: 2, icon: 'comments_disabled', label: 'No MQTT/API', help: 'not published to MQTT, Home Assistant or the API' },
    { bit: 1, icon: 'visibility_off', label: 'Hidden', help: 'not shown on the Devices page' },
    { bit: 128, icon: 'delete_forever', label: 'Removed', help: 'dropped from memory after a restart' }
  ];
  let cDev = 1, cFilter = 0, cQuery = '';
  let cSel = new Set();

  function customizations() {
    const d = M.devices.find((x) => x.id === cDev);
    const list = d.entities.filter(
      (e) =>
        (!cFilter || (cFilter === 'mod' ? cChanged(d, e) || e.m || e.cn || e.mi !== undefined || e.ma !== undefined : e.m & cFilter)) &&
        (!cQuery || `${e.name} ${e.cn || ''} ${e.id}`.toLowerCase().includes(cQuery))
    );
    const count = (bit) => d.entities.filter((e) => e.m & bit).length;
    const devPicker = M.devices
      .map((x) => {
        const n = x.entities.filter((e) => e.m || e.cn).length;
        return `<button class="${x.id === cDev ? 'on' : ''}" data-cdev="${x.id}"><span class="ms xs">${x.icon}</span>${x.name}${n ? ` <span class="badge">${n}</span>` : ''}</button>`;
      })
      .join('');
    const filters = [
      `<span class="chip clickable ${!cFilter ? 'selected' : ''}" data-cf="0">All ${d.entities.length}</span>`,
      ...MASKS.map((m) => `<span class="chip clickable ${cFilter === m.bit ? 'selected' : ''}" data-cf="${m.bit}" title="${m.help}"><span class="ms xs">${m.icon}</span>${m.label} ${count(m.bit)}</span>`),
      `<span class="chip clickable ${cFilter === 'mod' ? 'selected' : ''}" data-cf="mod"><span class="ms xs">tune</span>Customized</span>`
    ].join('');
    const nSel = [...cSel].filter((id) => d.entities.some((e) => e.id === id)).length;
    const bulk = nSel
      ? `<span class="lb">${nSel} selected</span>
         ${MASKS.map((m) => `<button class="icon-btn" title="${m.label}: toggle for selected" data-bulk="${m.bit}"><span class="ms sm">${m.icon}</span></button>`).join('')}
         <button class="btn sm secondary" data-bulk="reset"><span class="ms xs">restart_alt</span>Reset</button>
         <button class="btn sm secondary" data-bulk="none">Clear</button>`
      : '<span class="cap">Tick rows to change several entities at once</span>';
    const rows = list
      .map((e) => {
        const changed = cChanged(d, e);
        const removed = e.m & 128;
        const toggles = MASKS.map((m) => {
          const dis = m.writable && !(e.w && e.w.type !== 'cmd');
          return `<button class="mtog ${e.m & m.bit ? 'on' : ''} b${m.bit}" ${dis ? 'disabled' : ''} title="${m.label}${dis ? ' (not writable)' : ''}" data-mt="${e.id}" data-bit="${m.bit}"><span class="ms sm ${e.m & m.bit ? 'fill' : ''}">${m.icon}</span></button>`;
        }).join('');
        const lim = e.w && e.w.type === 'number' ? `${e.mi ?? e.w.min} – ${e.ma ?? e.w.max}` : '';
        const limCustom = e.mi !== undefined || e.ma !== undefined;
        return `<tr class="${changed ? 'st-mod' : ''} ${removed ? 'dim' : ''}">
          <td class="chk"><input type="checkbox" data-csel="${e.id}" ${cSel.has(e.id) ? 'checked' : ''}></td>
          <td class="toggles">${toggles}</td>
          <td class="ename" data-cedit="${e.id}">${e.cn ? `${h(e.cn)} <span class="cap">was ${h(e.name)}</span>` : h(e.name)}
            <span class="mono muted">${e.id}</span>${changed ? '<span class="tag mod">changed</span>' : ''}</td>
          <td class="r val">${fmt(e)}</td>
          <td class="r ${limCustom ? 'lb' : 'muted'}">${lim} ${lim && e.u ? h(e.u) : ''}</td></tr>`;
      })
      .join('');
    return `<div class="section">
      ${head('Customizations', 'Choose a device, then set how each of its entities is shown, published and written.', '<button class="btn danger" id="c-resetall"><span class="ms sm">delete_sweep</span>Reset all</button>')}
      <div class="toggle-group dev-picker" style="margin-top:12px">${devPicker}</div>
    </div>
    <div class="section">
      <div class="row wrap">${filters}</div>
      <div class="toolbar">${search('cq', cQuery, 'Search entities')}<span class="grow"></span>${bulk}</div>
      <div class="scroll"><table class="tbl mtbl ctbl"><thead><tr>
        <th class="chk"><input type="checkbox" id="c-all" ${list.length && list.every((e) => cSel.has(e.id)) ? 'checked' : ''}></th>
        <th>Options</th><th>Entity <span class="cap" style="text-transform:none">· click a name to rename or set limits</span></th><th class="r">Value</th><th class="r">Range</th></tr></thead>
        <tbody>${rows || '<tr><td colspan="5" class="muted">no entities match</td></tr>'}</tbody></table></div>
      <div class="legend">${MASKS.map((m) => `<span><span class="ms xs">${m.icon}</span> ${m.label}: ${m.help}</span>`).join('')}</div>
    </div>
    ${saveBar('customizations', cPending())}`;
  }

  function editCustomization(d, e, route) {
    const numeric = e.w && e.w.type === 'number';
    openForm({
      title: 'Customize Entity',
      values: { cn: e.cn || '', mi: e.mi ?? '', ma: e.ma ?? '' },
      intro: () => `<p class="cap" style="margin:12px 0 0">${h(d.name)} <span class="mono lb">${d.type}/${e.id}</span> · ${h(e.name)}</p>`,
      fields: [
        { k: 'cn', label: 'Custom name', full: true, ph: e.name, help: 'leave empty to use the built-in name' },
        ...(numeric
          ? [
              { k: 'mi', label: 'Minimum', type: 'number', step: e.w.step, unit: e.u, ph: String(e.w.min), help: `built-in ${e.w.min}` },
              { k: 'ma', label: 'Maximum', type: 'number', step: e.w.step, unit: e.u, ph: String(e.w.max), help: `built-in ${e.w.max}` }
            ]
          : [])
      ],
      extra: [{ icon: 'restart_alt', label: 'Reset', run: () => { e.m = 0; delete e.cn; delete e.mi; delete e.ma; route(); } }],
      validate: (v) => (numeric && v.mi !== '' && v.ma !== '' && v.mi > v.ma ? 'Minimum must be below maximum' : ''),
      onSave: (v) => {
        if (v.cn) e.cn = v.cn; else delete e.cn;
        if (v.mi !== '') e.mi = v.mi; else delete e.mi;
        if (v.ma !== '') e.ma = v.ma; else delete e.ma;
        route();
      }
    });
  }

  // ---------- Commands ----------
  let cmdQuery = '';
  const usedBy = (name) => M.schedules.filter((s) => s.cmd_name === name && s._st !== 'del');

  function commands() {
    const list = M.commands.filter((c) => !cmdQuery || `${c.name} ${c.cmd} ${c.value}`.toLowerCase().includes(cmdQuery));
    const rows = list
      .map((c) => {
        const st = stateOf('commands', 0, c);
        if (st === 'del')
          return `<tr class="st-del"><td>${h(c.name)}</td><td class="mono">${h(c.cmd)}</td><td colspan="3" class="muted">removed when you apply</td><td class="r">${undoBtn(`data-cundo="${c.id}"`)}</td></tr>`;
        const users = usedBy(c.name);
        return `<tr class="${stCls(st)}" data-cmd="${c.id}">
          <td>${h(c.name)}${stTag(st)}</td>
          <td class="mono lb">${h(c.cmd)}</td>
          <td class="mono">${c.value === '' ? '<span class="muted">none</span>' : h(c.value)}</td>
          <td>${c.guest ? '<span class="tag guest"><span class="ms xs">group</span>guests</span>' : '<span class="tag sys">admin</span>'}</td>
          <td>${users.length ? `<span class="tag type" title="${users.map((s) => s.name).join(', ')}"><span class="ms xs">more_time</span>${plural(users.length, 'schedule')}</span>` : '<span class="muted">-</span>'}</td>
          <td class="r"><button class="icon-btn" title="Run now" data-run="${c.id}"><span class="ms sm">play_arrow</span></button></td></tr>`;
      })
      .join('');
    return `<div class="section">
      ${head('Commands', 'Reusable named commands. Run them from here, the console, API or MQTT, or let the Scheduler run them.', `${search('cmdq', cmdQuery)}${addBtn('cmd-add')}`)}
      <table class="tbl mtbl" style="margin-top:12px"><thead><tr><th>Name</th><th>Command</th><th>Value</th><th>Who can run</th><th>Used by</th><th></th></tr></thead>
        <tbody>${rows || '<tr><td colspan="6" class="muted">no commands match</td></tr>'}</tbody></table>
    </div>
    ${saveBar('commands', pending('commands'))}`;
  }

  function editCommand(c, route) {
    const creating = !c;
    const item = c || { id: nextId(M.commands), name: '', cmd: '', value: '', guest: false };
    const users = creating ? [] : usedBy(item.name);
    openForm({
      title: creating ? 'Add Command' : 'Edit Command',
      values: item,
      intro: () =>
        users.length
          ? `<div class="msg info" style="margin:12px 0 0"><span class="ms">more_time</span><span>Used by ${users.map((s) => `<b>${h(s.name)}</b>`).join(', ')}. Renaming it updates those schedules.</span></div>`
          : '',
      fields: [
        { k: 'name', label: 'Name', full: true, mono: true, help: 'call it with <span class="mono">command/&lt;name&gt;</span> from the API or MQTT' },
        { k: 'cmd', label: 'Command', mono: true, ph: 'thermostat/hc1/seltemp', help: 'device/command, or device/hc/command' },
        { k: 'value', label: 'Value', mono: true, ph: 'optional' },
        { k: 'guest', label: 'Guests may run this command', type: 'switch', full: true }
      ],
      okLabel: creating ? 'Add' : 'Update',
      okIcon: creating ? 'add' : 'check',
      extra: creating ? [] : [{ icon: 'play_arrow', label: 'Run', close: false, run: (v) => toast(`Ran ${v.cmd}${v.value ? ` = ${v.value}` : ''}`) }],
      validate: (v) =>
        !v.name.trim() ? 'Name is required' : !/^[a-z0-9_]+$/.test(v.name) ? 'Use lowercase letters, digits and _ only' : !v.cmd.includes('/') ? 'Command must look like device/command' : '',
      onRemove: creating
        ? null
        : () => { item._st = 'del'; route(); if (users.length) toast(`${plural(users.length, 'schedule')} will have no command`); },
      onSave: (v) => {
        if (!creating && v.name !== item.name) users.forEach((s) => (s.cmd_name = v.name));
        if (creating) M.commands.push(v);
        else Object.assign(item, v);
        route();
      }
    });
  }

  // ---------- Scheduler ----------
  const DAYS = ['Su', 'Mo', 'Tu', 'We', 'Th', 'Fr', 'Sa'];
  const DAY_ORDER = [1, 2, 3, 4, 5, 6, 0];
  const NOW = { dow: 1, min: 21 * 60 + 7 }; // mock clock: Monday 21:07
  const TYPES = [
    ['day', 'Weekly', 'calendar_month'],
    ['timer', 'Timer', 'timer'],
    ['onchange', 'On Change', 'change_circle'],
    ['condition', 'Condition', 'rule']
  ];
  let schedFilter = 'all';

  function whenText(s, html = true) {
    const m = (x) => (html ? `<span class="mono lb">${h(x)}</span>` : x);
    if (s.type === 'day') {
      const days = s.days.length === 7 ? 'every day' : s.days.join() === '1,2,3,4,5' ? 'Mon-Fri' : s.days.join() === '0,6' ? 'weekends' : DAY_ORDER.filter((d) => s.days.includes(d)).map((d) => DAYS[d]).join(' ');
      return `${days} ${html ? `<b>${s.time}</b>` : s.time}`;
    }
    if (s.type === 'timer') return s.time ? `every ${s.time}` : 'at start-up';
    if (s.type === 'onchange') return `when ${m(s.time)} changes`;
    return `when ${m(s.time)} is true`;
  }

  // days ahead of the mock clock for a weekly schedule, -1 if it never runs
  function daysAhead(s) {
    const [hh, mm] = s.time.split(':').map(Number);
    for (let k = 0; k < 8; k++) {
      if (s.days.includes((NOW.dow + k) % 7) && !(k === 0 && hh * 60 + mm <= NOW.min)) return k;
    }
    return -1;
  }

  function nextRun(s) {
    if (!s.active) return '<span class="muted">paused</span>';
    if (s.type === 'day') {
      const k = daysAhead(s);
      if (k < 0) return '<span class="muted">never</span>';
      return k === 0 ? `today ${s.time}` : k === 1 ? `tomorrow ${s.time}` : `${DAYS[(NOW.dow + k) % 7]} ${s.time}`;
    }
    if (s.type === 'timer') return s.time ? `in ${s.time}` : 'next start-up';
    return '<span class="muted">on event</span>';
  }

  function scheduler() {
    const list = M.schedules.filter((s) => schedFilter === 'all' || s.type === schedFilter);
    const active = M.schedules.filter((s) => s.active).length;
    const typeIcon = (t) => TYPES.find((x) => x[0] === t)[2];
    const rows = list
      .map((s) => {
        const st = stateOf('scheduler', 0, s);
        if (st === 'del')
          return `<tr class="st-del"><td></td><td>${h(s.name)}</td><td colspan="3" class="muted">removed when you apply</td><td class="r">${undoBtn(`data-sundo="${s.id}"`)}</td></tr>`;
        const c = M.commands.find((x) => x.name === s.cmd_name && x._st !== 'del');
        const dayStrip =
          s.type === 'day'
            ? `<span class="days">${DAY_ORDER.map((d) => `<span class="${s.days.includes(d) ? 'on' : ''}">${DAYS[d][0]}</span>`).join('')}</span>`
            : '';
        return `<tr class="${stCls(st)} ${s.active ? '' : 'dim'}" data-sch="${s.id}">
          <td style="width:52px"><div class="switch ${s.active ? 'on' : ''}" data-sact="${s.id}" title="${s.active ? 'pause' : 'activate'}"></div></td>
          <td>${h(s.name)}${stTag(st)}</td>
          <td class="when"><span class="ms xs muted">${typeIcon(s.type)}</span> ${s.type === 'day' ? `${dayStrip}<b>${s.time}</b>` : whenText(s)}</td>
          <td>${c ? `<a data-nav="commands">${h(c.name)}</a><div class="mono muted">${h(c.cmd)}${c.value ? ' = ' + h(c.value) : ''}</div>` : `<span class="tag err"><span class="ms xs">link_off</span>${h(s.cmd_name)} missing</span>`}</td>
          <td class="r nowrap">${nextRun(s)}</td>
          <td class="r"><button class="icon-btn" title="Run command now" data-srun="${s.id}" ${c ? '' : 'disabled'}><span class="ms sm">play_arrow</span></button></td></tr>`;
      })
      .join('');
    const tabs = [['all', 'All', 'list'], ...TYPES]
      .map(([k, l, i]) => `<button class="${schedFilter === k ? 'on' : ''}" data-stype="${k}"><span class="ms xs">${i}</span>${l}</button>`)
      .join('');
    const due = (s) => daysAhead(s) * 1440 + s.time.split(':').reduce((a, b) => a * 60 + +b, 0);
    const next = M.schedules
      .filter((s) => s.active && s.type === 'day' && s._st !== 'del' && daysAhead(s) >= 0)
      .sort((a, b) => due(a) - due(b))[0];
    return `<div class="section">
      ${head('Scheduler', `${active} active, ${M.schedules.length - active} paused${next ? ` · next: <b>${h(next.name)}</b> ${nextRun(next)}` : ''} · device time Mon 21:07`, addBtn('sch-add'))}
      <div class="toolbar"><span class="toggle-group">${tabs}</span></div>
      <table class="tbl mtbl"><thead><tr><th>Active</th><th>Name</th><th>When</th><th>Runs</th><th class="r">Next</th><th></th></tr></thead>
        <tbody>${rows || '<tr><td colspan="6" class="muted">no schedules of this type</td></tr>'}</tbody></table>
    </div>
    ${saveBar('scheduler', pending('scheduler'))}`;
  }

  function editSchedule(s, route) {
    const creating = !s;
    const item = s || { id: nextId(M.schedules), name: '', type: 'day', days: [1, 2, 3, 4, 5], time: '07:00', cmd_name: M.commands[0].name, active: true };
    const cmds = M.commands.filter((c) => c._st !== 'del');
    openForm({
      title: creating ? 'Add Schedule' : 'Edit Schedule',
      width: 560,
      values: item,
      fields: [
        { k: 'type', label: 'Type', type: 'seg', full: true, opts: TYPES },
        { k: 'days', label: 'Days', type: 'days', full: true, show: (v) => v.type === 'day' },
        { k: 'time', label: 'Time', type: 'time', show: (v) => v.type === 'day' },
        { k: 'time', label: 'Interval', ph: 'hh:mm', show: (v) => v.type === 'timer', help: 'empty or 00:00 runs once at start-up' },
        { k: 'time', label: 'Entity', mono: true, full: true, ph: 'thermostat/hc1/mode', show: (v) => v.type === 'onchange', help: 'runs every time this value changes' },
        { k: 'time', label: 'Condition', mono: true, full: true, ph: 'boiler/outdoortemp < 0', show: (v) => v.type === 'condition', help: 'runs once each time this becomes true' },
        { k: 'cmd_name', label: 'Command', type: 'select', opts: cmds.map((c) => [c.name, `${c.name}  (${c.cmd}${c.value ? ' = ' + c.value : ''})`]), full: true, help: 'add or change commands on the <a data-nav="commands">Commands</a> page' },
        { k: 'name', label: 'Name', mono: true, help: 'optional, needed to switch it from the API' },
        { k: 'active', label: 'Active', type: 'switch' }
      ],
      okLabel: creating ? 'Add' : 'Update',
      okIcon: creating ? 'add' : 'check',
      validate: (v) => (v.type === 'day' && !v.days.length ? 'Pick at least one day' : (v.type === 'onchange' || v.type === 'condition') && !v.time.trim() ? 'This type needs an entity or condition' : ''),
      onRemove: creating ? null : () => { item._st = 'del'; route(); },
      onSave: (v) => {
        if (v.type === 'timer' && v.time === '00:00') v.time = '';
        if (creating) M.schedules.push(v);
        else Object.assign(item, v);
        route();
      }
    });
  }

  // ---------- Custom Entities ----------
  const STORAGE = [
    [0, 'EMS', 'swap_horiz', 'read from an EMS telegram'],
    [1, 'RAM', 'memory', 'kept until restart'],
    [2, 'NVS', 'save', 'kept across restarts']
  ];
  const VTYPES = ['int8', 'uint8', 'int16', 'uint16', 'uint24', 'uint32', 'bool', 'raw'];
  let ceFilter = -1, ceQuery = '';

  function customentities() {
    const list = M.custom.filter((c) => (ceFilter < 0 || c.ram === ceFilter) && (!ceQuery || c.name.toLowerCase().includes(ceQuery)));
    const rows = list
      .map((c) => {
        const st = stateOf('customentities', 0, c);
        if (st === 'del')
          return `<tr class="st-del"><td>${h(c.name)}</td><td colspan="3" class="muted">removed when you apply</td><td class="r">${undoBtn(`data-eundo="${c.id}"`)}</td></tr>`;
        const [, sl, si] = STORAGE[c.ram];
        const src = c.ram === 0 ? `device ${c.device_id} · telegram ${c.type_id} · offset ${c.offset} · ${c.value_type}${c.factor !== 1 ? ` ×${c.factor}` : ''}` : STORAGE[c.ram][3];
        return `<tr class="${stCls(st)}" data-ce="${c.id}">
          <td>${h(c.name)} ${c.writeable ? '<span class="ms xs lb" title="writable">edit</span>' : ''}${stTag(st)}</td>
          <td><span class="tag s${c.ram}"><span class="ms xs">${si}</span>${sl}</span></td>
          <td class="${c.ram === 0 ? 'mono' : 'cap'} muted">${src}</td>
          <td class="mono muted">custom/${h(c.name)}</td>
          <td class="r val">${fmt(c)}</td></tr>`;
      })
      .join('');
    const tabs = [[-1, 'All'], ...STORAGE]
      .map(([k, l, i]) => `<button class="${ceFilter === k ? 'on' : ''}" data-cef="${k}">${i ? `<span class="ms xs">${i}</span>` : ''}${l} ${k < 0 ? M.custom.length : M.custom.filter((c) => c.ram === k).length}</button>`)
      .join('');
    return `<div class="section">
      ${head('Custom Entities', 'Your own values: read a field from any EMS telegram, or keep a variable in RAM or NVS for the Scheduler and automations.', `${search('ceq', ceQuery)}${addBtn('ce-add')}`)}
      <div class="toolbar"><span class="toggle-group">${tabs}</span></div>
      <table class="tbl mtbl"><thead><tr><th>Name</th><th>Storage</th><th>Source</th><th>API / MQTT</th><th class="r">Value</th></tr></thead>
        <tbody>${rows || '<tr><td colspan="5" class="muted">no entities match</td></tr>'}</tbody></table>
    </div>
    ${saveBar('customentities', pending('customentities'))}`;
  }

  function editCustom(c, route, dup) {
    const creating = !c || dup;
    const base = c ? clone(c) : { name: '', ram: 1, device_id: '0x08', type_id: '0x18', offset: 0, value_type: 'uint8', factor: 1, u: '', writeable: false, v: 0 };
    if (creating) { base.id = nextId(M.custom); base.name = dup ? `${base.name}_copy` : ''; delete base._st; }
    const ems = (v) => +v.ram === 0;
    openForm({
      title: creating ? 'Add Custom Entity' : 'Edit Custom Entity',
      width: 560,
      values: { ...base, ram: String(base.ram) },
      fields: [
        { k: 'name', label: 'Name', mono: true, full: true, help: 'lowercase, becomes the MQTT topic and API path custom/&lt;name&gt;' },
        { k: 'ram', label: 'Storage', type: 'seg', full: true, opts: STORAGE.map(([k, l, i]) => [String(k), l, i]), help: '' },
        { k: 'device_id', label: 'Device ID', mono: true, show: ems, ph: '0x08' },
        { k: 'type_id', label: 'Telegram type ID', mono: true, show: ems, ph: '0x18' },
        { k: 'offset', label: 'Offset', type: 'number', show: ems },
        { k: 'value_type', label: 'Value type', type: 'select', opts: VTYPES, show: ems },
        { k: 'factor', label: 'Factor', type: 'number', step: 0.01, show: ems },
        { k: 'v', label: 'Start value', show: (v) => !ems(v) },
        { k: 'u', label: 'Unit', ph: 'e.g. °C' },
        { k: 'writeable', label: 'Writable (can be set from the API, MQTT and Scheduler)', type: 'switch', full: true }
      ],
      okLabel: creating ? 'Add' : 'Update',
      okIcon: creating ? 'add' : 'check',
      extra: creating ? [] : [{ icon: 'content_copy', label: 'Duplicate', run: () => editCustom(c, route, true) }],
      validate: (v) =>
        !/^[a-z0-9_]+$/.test(v.name) ? 'Name must be lowercase letters, digits and _' : M.custom.some((x) => x.name === v.name && x.id !== v.id) ? 'Name already used' : '',
      onRemove: creating ? null : () => { c._st = 'del'; route(); },
      onSave: (v) => {
        v.ram = +v.ram;
        if (creating) M.custom.push(v);
        else Object.assign(c, v);
        route();
      }
    });
  }

  // ---------- wiring ----------
  function liveSearch(el, id, set, route) {
    const q = el.querySelector('#' + id);
    if (!q) return;
    q.addEventListener('input', () => {
      set(q.value.toLowerCase());
      const pos = q.selectionStart;
      route();
      const n = document.getElementById(id);
      n.focus();
      n.setSelectionRange(pos, pos);
    });
  }
  const on = (el, sel, fn) => el.querySelectorAll(sel).forEach((n) => n.addEventListener('click', (ev) => fn(n, ev)));

  function afterRender(key, el, route) {
    on(el, '[data-apply]', (b) => {
      const k = b.dataset.apply;
      if (k === 'customizations') {
        const restart = M.devices.some((d) => d.entities.some((e) => (e.m & 128) !== (csnap[ckey(d, e)].m & 128)));
        M.devices.forEach((d) => d.entities.forEach((e) => (e.fav = !!(e.m & 8))));
        csnapAll();
        toast(restart ? 'Saved. Restart EMS-ESP to remove entities from memory' : 'Customizations saved');
      } else {
        apply(k);
        toast('Changes applied');
      }
      route();
    });
    on(el, '[data-cancel]', (b) => {
      const k = b.dataset.cancel;
      if (k === 'customizations')
        M.devices.forEach((d) => d.entities.forEach((e) => {
          const o = csnap[ckey(d, e)];
          e.m = o.m;
          if (o.cn) e.cn = o.cn; else delete e.cn;
          if (o.mi !== undefined) e.mi = o.mi; else delete e.mi;
          if (o.ma !== undefined) e.ma = o.ma; else delete e.ma;
        }));
      else cancel(k);
      route();
    });

    if (key === 'sensors') {
      liveSearch(el, 'sq', (v) => (sQuery = v), route);
      on(el, '[data-ts]', (r) => editTemp(M.sensors.temp.find((t) => t.id === r.dataset.ts), route));
      on(el, '[data-as]', (r, ev) => { if (!ev.target.closest('[data-aout]')) editAnalog(M.sensors.analog.find((a) => a.id == r.dataset.as), route); });
      on(el, '[data-aout]', (s) => {
        const a = M.sensors.analog.find((x) => x.id == s.dataset.aout);
        a.v = !a.v;
        const o = snaps.sensors[1].find((x) => x.id === a.id);
        if (o) o.v = a.v;
        toast(`${a.name} switched ${a.v ? 'on' : 'off'}`);
        route();
      });
      on(el, '[data-aundo]', (b) => { delete M.sensors.analog.find((a) => a.id == b.dataset.aundo)._st; route(); });
      const add = el.querySelector('#as-add');
      if (add) add.onclick = () => editAnalog(null, route);
    }

    if (key === 'customizations') {
      const d = M.devices.find((x) => x.id === cDev);
      const ent = (id) => d.entities.find((e) => e.id === id);
      liveSearch(el, 'cq', (v) => (cQuery = v), route);
      on(el, '[data-cdev]', (b) => { cDev = +b.dataset.cdev; cSel = new Set(); route(); });
      on(el, '[data-cf]', (c) => { cFilter = c.dataset.cf === 'mod' ? 'mod' : +c.dataset.cf; route(); });
      on(el, '[data-mt]', (b) => { ent(b.dataset.mt).m ^= +b.dataset.bit; route(); });
      on(el, '[data-cedit]', (c) => editCustomization(d, ent(c.dataset.cedit), route));
      el.querySelectorAll('[data-csel]').forEach((c) => c.addEventListener('change', () => { c.checked ? cSel.add(c.dataset.csel) : cSel.delete(c.dataset.csel); route(); }));
      const all = el.querySelector('#c-all');
      if (all) all.addEventListener('change', () => {
        el.querySelectorAll('[data-csel]').forEach((c) => (all.checked ? cSel.add(c.dataset.csel) : cSel.delete(c.dataset.csel)));
        route();
      });
      on(el, '[data-bulk]', (b) => {
        const sel = d.entities.filter((e) => cSel.has(e.id));
        const v = b.dataset.bulk;
        if (v === 'none') cSel = new Set();
        else if (v === 'reset') sel.forEach((e) => { e.m = 0; delete e.cn; delete e.mi; delete e.ma; });
        else {
          const bit = +v;
          const targets = bit === 4 ? sel.filter((e) => e.w && e.w.type !== 'cmd') : sel;
          const set = !targets.every((e) => e.m & bit);
          targets.forEach((e) => (e.m = set ? e.m | bit : e.m & ~bit));
        }
        route();
      });
      const ra = el.querySelector('#c-resetall');
      if (ra) ra.onclick = () => {
        const bd = document.getElementById('dialog');
        bd.innerHTML = `<div class="dialog" style="width:440px" onclick="event.stopPropagation()"><h2>Reset all customizations?</h2>
          <div class="content"><p style="margin:12px 0 0">This clears favorites, write protection, custom names and limits on every device, then restarts EMS-ESP.</p></div>
          <div class="actions"><button class="btn secondary" data-close><span class="ms sm">cancel</span>Cancel</button>
          <button class="btn error contained" data-ok><span class="ms sm">delete_sweep</span>Reset</button></div></div>`;
        bd.classList.add('open');
        const close = () => bd.classList.remove('open');
        bd.onclick = close;
        bd.querySelector('[data-close]').onclick = close;
        bd.querySelector('[data-ok]').onclick = () => { close(); toast('All customizations removed. Restarting...'); };
      };
    }

    if (key === 'commands') {
      liveSearch(el, 'cmdq', (v) => (cmdQuery = v), route);
      on(el, '[data-cmd]', (r, ev) => { if (!ev.target.closest('[data-run]')) editCommand(M.commands.find((c) => c.id == r.dataset.cmd), route); });
      on(el, '[data-run]', (b) => { const c = M.commands.find((x) => x.id == b.dataset.run); toast(`Ran ${c.cmd}${c.value ? ` = ${c.value}` : ''}`); });
      on(el, '[data-cundo]', (b) => { delete M.commands.find((c) => c.id == b.dataset.cundo)._st; route(); });
      el.querySelector('#cmd-add').onclick = () => editCommand(null, route);
    }

    if (key === 'scheduler') {
      on(el, '[data-stype]', (b) => { schedFilter = b.dataset.stype; route(); });
      on(el, '[data-sch]', (r, ev) => { if (!ev.target.closest('[data-sact],[data-srun],[data-nav]')) editSchedule(M.schedules.find((s) => s.id == r.dataset.sch), route); });
      on(el, '[data-sact]', (s) => { const x = M.schedules.find((y) => y.id == s.dataset.sact); x.active = !x.active; route(); });
      on(el, '[data-srun]', (b) => {
        const s = M.schedules.find((x) => x.id == b.dataset.srun);
        const c = M.commands.find((x) => x.name === s.cmd_name);
        toast(`Ran ${c.cmd}${c.value ? ` = ${c.value}` : ''}`);
      });
      on(el, '[data-sundo]', (b) => { delete M.schedules.find((s) => s.id == b.dataset.sundo)._st; route(); });
      el.querySelector('#sch-add').onclick = () => editSchedule(null, route);
    }

    if (key === 'customentities') {
      liveSearch(el, 'ceq', (v) => (ceQuery = v), route);
      on(el, '[data-cef]', (b) => { ceFilter = +b.dataset.cef; route(); });
      on(el, '[data-ce]', (r) => editCustom(M.custom.find((c) => c.id == r.dataset.ce), route));
      on(el, '[data-eundo]', (b) => { delete M.custom.find((c) => c.id == b.dataset.eundo)._st; route(); });
      el.querySelector('#ce-add').onclick = () => editCustom(null, route);
    }
  }

  window.Modules = {
    views: { sensors, customizations, commands, scheduler, customentities },
    afterRender,
    whenText,
    openForm,
    titles: { customentities: 'Custom Entities' }
  };
})();
