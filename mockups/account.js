// Help and User Profile pages for concept A.
(function () {
  const { h, toast, M } = Shell;
  const S = M.system;

  const LANGS = [
    ['en', 'English'],
    ['de', 'Deutsch'],
    ['nl', 'Nederlands'],
    ['fr', 'Français'],
    ['it', 'Italiano'],
    ['pl', 'Polski'],
    ['no', 'Norsk'],
    ['sv', 'Svenska'],
    ['cs', 'Čeština'],
    ['sk', 'Slovenčina'],
    ['hu', 'Magyar'],
    ['tr', 'Türkçe']
  ];
  let lang = 'en';
  let showSupportBox = true;

  const summary = () =>
    [
      `EMS-ESP ${S.version} on ${S.board}`,
      `uptime ${S.uptime}, free heap ${S.freeHeap} KB`,
      `EMS bus ${S.bus.status} (${S.bus.tx}), ${S.bus.quality}% quality, ${S.bus.txFail} Tx fails`,
      `devices: ${M.devices.map((d) => `${d.name} ${d.model} (${d.productId})`).join(', ')}`,
      `network ${S.network.type}, MQTT ${S.mqtt.connected ? 'connected' : 'disconnected'}`
    ].join('\n');

  // ---------- Help ----------
  function help() {
    const links = [
      ['menu_book', '#72caf9', 'Documentation', 'Instructions for setting up and configuring EMS-ESP', 'emsesp.org'],
      ['forum', '#5865f2', 'Community chat', 'Ask questions and get help from other users on Discord', 'discord.gg'],
      ['bug_report', '#c5572c', 'Report a bug or request a feature', 'Opens a new issue on GitHub', 'github.com/emsesp']
    ];
    const support = showSupportBox
      ? `<div class="section support-box">
          <div class="grow">
            <div class="cap" style="text-transform:uppercase;letter-spacing:.5px">Your installer</div>
            <div class="h6" style="margin:4px 0 6px">Heating Services Ltd.</div>
            <div>For questions about your boiler settings call <b>+44 20 7946 0000</b> or email <a>support@heating-services.example</a>.</div>
            <div class="cap" style="margin-top:8px">Shown because a customSupport file was uploaded. <a id="hide-support">Preview without it</a></div>
          </div>
          <div class="support-img"><span class="ms">engineering</span></div>
        </div>`
      : `<div class="section"><div class="cap">No installer support file uploaded. <a id="show-support">Preview with one</a></div></div>`;
    return `${support}
    <div class="section">
      <h2 class="sec-title">Get help</h2>
      <div class="help-links">${links
        .map(([i, c, k, v, url]) => `<a class="help-link" data-link="${k}"><div class="avatar" style="background:${c}"><span class="ms">${i}</span></div>
          <div class="grow"><div class="k">${k}</div><div class="v">${v}</div><div class="cap">${url}</div></div><span class="ms muted">open_in_new</span></a>`)
        .join('')}</div>
    </div>
    <div class="section">
      <h2 class="sec-title">Reporting a problem</h2>
      <p class="sec-text">Include your system information so others can see your setup straight away. Passwords and API keys are not included.</p>
      <div class="steps">
        <div class="step"><span class="num">1</span><div class="grow"><div>Download the system information</div>
          <div class="cap">version, board, EMS bus status, connected devices and settings</div></div>
          <button class="btn" id="dl-info"><span class="ms xs">download</span>System Information</button></div>
        <div class="step"><span class="num">2</span><div class="grow"><div>Save the recent log</div>
          <div class="cap">set the log level to DEBUG first if the problem is with a device</div></div>
          <button class="btn secondary" data-nav="status"><span class="ms xs">list_alt</span>System Log</button></div>
        <div class="step"><span class="num">3</span><div class="grow"><div>Open an issue or ask on Discord</div>
          <div class="cap">attach both files, or paste the short summary below into a chat</div></div>
          <button class="btn secondary" data-link="Report a bug or request a feature"><span class="ms xs">open_in_new</span>New issue</button></div>
      </div>
      <div class="sum-head"><span class="cap">Short summary</span><span class="grow"></span>
        <button class="btn sm" id="copy-sum"><span class="ms xs">content_copy</span>Copy</button></div>
      <pre class="summary mono">${h(summary())}</pre>
    </div>
    <div class="about">EMS-ESP ${S.version} · © <a data-link="Documentation">emsesp.org</a> · open source under the LGPL-3.0 license</div>`;
  }

  // ---------- User Profile ----------
  function user() {
    const cur = LANGS.find((l) => l[0] === lang);
    return `<div class="section">
      <div class="profile">
        <div class="avatar big" style="background:#2196f3"><span class="ms">person</span></div>
        <div class="grow">
          <div class="h6" style="font-size:20px">admin</div>
          <div class="row" style="margin-top:4px"><span class="tag mod" style="margin-left:0">Administrator</span>
            <span class="cap">full access to settings, devices and the API</span></div>
        </div>
        <button class="btn" id="signout"><span class="ms sm">logout</span>Sign Out</button>
      </div>
    </div>
    <div class="section">
      <div class="mod-head"><span class="ms lb">translate</span><h3 class="h6 grow">Language</h3><span class="cap">only changes this browser · now ${cur[1]}</span></div>
      <div class="lang-grid">${LANGS.map(([c, n]) => `<button class="lang ${c === lang ? 'on' : ''}" data-lang="${c}"><span class="code">${c.toUpperCase()}</span>${n}</button>`).join('')}</div>
      <p class="cap" style="margin:10px 0 0">The language for device entity names in MQTT and the API is set in <a data-nav="settings">Application Settings</a>.</p>
    </div>
    <div class="section">
      <div class="mod-head"><span class="ms lb">admin_panel_settings</span><h3 class="h6 grow">Administration</h3><span class="cap">admin only</span></div>
      <div class="help-links" style="margin-top:10px">
        <a class="help-link" data-nav="settings"><div class="avatar" style="background:#40828f"><span class="ms">group</span></div>
          <div class="grow"><div class="k">Manage users</div><div class="v">2 users: admin, guest</div></div><span class="ms muted">chevron_right</span></a>
        <a class="help-link" data-nav="settings"><div class="avatar" style="background:#40828f"><span class="ms">key</span></div>
          <div class="grow"><div class="k">API keys</div><div class="v">1 key for Home Assistant (read and write)</div></div><span class="ms muted">chevron_right</span></a>
        <a class="help-link" id="chpw"><div class="avatar" style="background:#40828f"><span class="ms">password</span></div>
          <div class="grow"><div class="k">Change my password</div><div class="v">for the admin account</div></div><span class="ms muted">chevron_right</span></a>
      </div>
    </div>`;
  }

  function passwordDialog() {
    const bd = document.getElementById('dialog');
    bd.innerHTML = `<div class="dialog" style="width:420px" onclick="event.stopPropagation()"><h2>Change Password</h2>
      <div class="content"><div class="form">
        <div class="field full"><label>Current password</label><input class="input" type="password"></div>
        <div class="field full"><label>New password</label><input class="input" type="password"></div>
        <div class="field full"><label>Repeat new password</label><input class="input" type="password"></div>
      </div></div>
      <div class="actions"><button class="btn secondary" data-close><span class="ms sm">cancel</span>Cancel</button>
      <button class="btn contained" data-ok><span class="ms sm">check</span>Update</button></div></div>`;
    bd.classList.add('open');
    const close = () => bd.classList.remove('open');
    bd.onclick = close;
    bd.querySelector('[data-close]').onclick = close;
    bd.querySelector('[data-ok]').onclick = () => { close(); toast('Password changed'); };
  }

  function afterRender(key, el, route) {
    el.querySelectorAll('[data-link]').forEach((a) => a.addEventListener('click', () => toast(`Opens ${a.dataset.link} in a new tab`)));
    if (key === 'help') {
      const dl = el.querySelector('#dl-info');
      if (dl) dl.onclick = () => toast('Download successful');
      el.querySelector('#copy-sum').onclick = () => {
        navigator.clipboard?.writeText(summary());
        toast('Summary copied');
      };
      const hs = el.querySelector('#hide-support');
      if (hs) hs.onclick = () => { showSupportBox = false; route(); };
      const ss = el.querySelector('#show-support');
      if (ss) ss.onclick = () => { showSupportBox = true; route(); };
    }
    if (key === 'user') {
      el.querySelectorAll('[data-lang]').forEach((b) => b.addEventListener('click', () => {
        lang = b.dataset.lang;
        route();
        toast(`Language set to ${LANGS.find((l) => l[0] === lang)[1]}`);
      }));
      el.querySelector('#signout').onclick = () => toast('Signed out');
      el.querySelector('#chpw').onclick = passwordDialog;
    }
  }

  window.Account = { views: { help, user }, titles: { user: 'User Profile' }, afterRender };
})();
