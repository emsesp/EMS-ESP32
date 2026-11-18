// Shared app shell (drawer, app bar, hash router, edit dialog, toast) used by every concept page.
(function () {
  const M = window.MOCK;

  const NAV = [
    { key: 'dashboard', icon: 'star', label: 'Dashboard' },
    { key: 'devices', icon: 'category', label: 'Devices' },
    { divider: true },
    {
      group: 'Modules',
      items: [
        { key: 'sensors', icon: 'sensors', label: 'Sensors' },
        { key: 'customizations', icon: 'construction', label: 'Customizations' },
        { key: 'commands', icon: 'playlist_play', label: 'Commands' },
        { key: 'scheduler', icon: 'more_time', label: 'Scheduler' },
        { key: 'customentities', icon: 'playlist_add', label: 'Custom Entities' }
      ]
    }
  ];
  const NAV_BOTTOM = [
    { key: 'status', icon: 'assessment', label: 'Status' },
    { key: 'settings', icon: 'settings', label: 'Settings', badge: true },
    { key: 'help', icon: 'live_help', label: 'Help' },
    { divider: true },
    { key: 'user', icon: 'account_circle', label: 'admin' }
  ];

  const h = (s) => String(s).replace(/[&<>"]/g, (c) => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;' })[c]);

  const fmt = (e) => {
    if (e.w && e.w.type === 'cmd') return '';
    if (typeof e.v === 'boolean') return e.v ? 'on' : 'off';
    if (typeof e.v === 'number') {
      const n = Number.isInteger(e.v) ? e.v.toLocaleString('en-US') : e.v.toLocaleString('en-US', { maximumFractionDigits: 2 });
      return e.u ? `${n} ${e.u}` : n;
    }
    return e.u ? `${e.v} ${e.u}` : String(e.v);
  };

  const navItem = (it, active) => {
    if (it.divider) return '<li class="nav-divider"></li>';
    return `<li class="nav-item ${it.key === active ? 'active' : ''}" data-nav="${it.key}">
      <span class="ms">${it.icon}</span><span>${it.label}</span>${it.badge ? '<span class="badge-dot" title="update available"></span>' : ''}
    </li>`;
  };

  const renderNav = (active) => {
    const top = NAV.map((it) =>
      it.group
        ? `<div class="nav-group"><div class="nav-group-title">${it.group}</div>${it.items.map((i) => navItem(i, active)).join('')}</div>`
        : navItem(it, active)
    ).join('');
    return `<ul class="nav">${top}</ul><ul class="nav bottom">${NAV_BOTTOM.map((i) => navItem(i, active)).join('')}</ul>`;
  };

  const allNav = [...NAV.flatMap((i) => i.items || [i]), ...NAV_BOTTOM].filter((i) => i.key);

  let cfg;

  function placeholder(key) {
    const item = allNav.find((i) => i.key === key);
    return `<div class="section"><div class="msg info"><span class="ms">info</span>
      <span><b>${item ? item.label : key}</b> is not part of this mockup. Try the Dashboard, Devices, Modules, Status or Settings pages.</span></div></div>`;
  }

  const titleOf = (key) => {
    const item = allNav.find((i) => i.key === key);
    return (cfg.titles && cfg.titles[key]) || (item ? item.label : key);
  };

  function route() {
    const key = location.hash.slice(1) || cfg.defaultView || 'dashboard';
    const chain = [key];
    while (cfg.parents && cfg.parents[chain[0]]) chain.unshift(cfg.parents[chain[0]]);
    document.querySelectorAll('.drawer').forEach((d) => (d.innerHTML = drawerHead() + renderNav(chain[0])));
    document.getElementById('title').innerHTML = chain
      .map((k, i) => (i < chain.length - 1 ? `<a class="crumb-link" data-nav="${k}">${h(titleOf(k))}</a><span class="crumb">›</span>` : h(titleOf(k))))
      .join('');
    const view = cfg.views[key];
    const el = document.getElementById('content');
    el.innerHTML = view ? view() : placeholder(key);
    if (cfg.afterRender) cfg.afterRender(key, el);
    document.body.classList.remove('drawer-open');
    window.scrollTo(0, 0);
  }

  const drawerHead = () => `<div class="drawer-head"><img src="assets/icon.png" alt="EMS-ESP"><div>
      <div class="title">EMS-ESP</div><div class="sub">${M.system.name}</div></div></div>`;

  // ---------- edit dialog (MUI style, as DevicesDialog.tsx) ----------
  function editDialog(e, dev, onDone) {
    const bd = document.getElementById('dialog');
    const w = e.w || {};
    let input = '';
    if (w.type === 'number') {
      input = `<div class="field" style="width:100%"><label>${h(e.name)}</label>
        <div class="row"><input id="dlg-v" class="input grow" type="number" value="${e.v}" min="${w.min}" max="${w.max}" step="${w.step}"><span class="muted">${h(e.u)}</span></div>
        <span class="cap" style="margin-top:6px">range ${w.min} to ${w.max} ${h(e.u)}</span></div>`;
    } else if (w.type === 'enum') {
      input = `<div class="field" style="width:100%"><label>${h(e.name)}</label>
        <select id="dlg-v" class="input">${w.options.map((o) => `<option ${o === e.v ? 'selected' : ''}>${o}</option>`).join('')}</select></div>`;
    } else if (w.type === 'bool') {
      input = `<div class="row" style="padding:8px 0"><div id="dlg-v" class="switch ${e.v ? 'on' : ''}" onclick="this.classList.toggle('on')"></div><span>${h(e.name)}</span></div>`;
    } else if (w.type === 'cmd') {
      input = `<p>Run command <span class="mono lb">${dev.type}/${e.id}</span>?</p>`;
    }
    bd.innerHTML = `<div class="dialog" onclick="event.stopPropagation()">
      <h2>${w.type === 'cmd' ? 'Run' : 'Change Value'}</h2>
      <div class="content">
        <p class="cap" style="margin:12px 0">${h(dev.name)} <span class="mono">${dev.type}/${e.id}</span></p>
        ${input}
      </div>
      <div class="actions">
        <button class="btn secondary" data-close><span class="ms sm">cancel</span>Cancel</button>
        <button class="btn contained" data-ok><span class="ms sm">${w.type === 'cmd' ? 'play_arrow' : 'check'}</span>${w.type === 'cmd' ? 'Run' : 'Update'}</button>
      </div></div>`;
    bd.classList.add('open');
    const close = () => bd.classList.remove('open');
    bd.onclick = close;
    bd.querySelector('[data-close]').onclick = close;
    bd.querySelector('[data-ok]').onclick = () => {
      const v = document.getElementById('dlg-v');
      if (w.type === 'number') e.v = parseFloat(v.value);
      else if (w.type === 'enum') e.v = v.value;
      else if (w.type === 'bool') e.v = v.classList.contains('on');
      close();
      toast('Write command sent');
      if (onDone) onDone();
    };
  }

  let toastTimer;
  function toast(msg) {
    const t = document.getElementById('toast');
    t.innerHTML = `<span class="ms sm">check_circle</span>${h(msg)}`;
    t.classList.add('show');
    clearTimeout(toastTimer);
    toastTimer = setTimeout(() => t.classList.remove('show'), 2200);
  }

  function init(c) {
    cfg = c;
    document.body.insertAdjacentHTML(
      'afterbegin',
      `<nav class="drawer"></nav><div class="scrim"></div>
      <header class="appbar">
        <button class="icon-btn plain menu-btn" id="menu-btn" style="color:#fff"><span class="ms">menu</span></button>
        <h1 id="title"></h1><div class="spacer"></div>${c.appbarExtra || ''}
      </header>
      <main class="main"><div id="content"></div></main>
      <div class="dialog-backdrop" id="dialog"></div>
      <div class="toast" id="toast"></div>`
    );
    document.getElementById('menu-btn').onclick = () => document.body.classList.toggle('drawer-open');
    document.querySelector('.scrim').onclick = () => document.body.classList.remove('drawer-open');
    document.addEventListener('click', (ev) => {
      const n = ev.target.closest('[data-nav]');
      if (n) location.hash = n.dataset.nav;
    });
    window.addEventListener('hashchange', route);
    route();
  }

  window.Shell = { init, route, fmt, h, editDialog, toast, M };
})();
