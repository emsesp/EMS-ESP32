// Back-translates firmware + web UI strings to English via DeepL so you can
// scan for bad / unintended wording.
// usage: DEEPL_API_KEY=... node backtranslate-check.js --lang de
//        [--source core|webui|all]  (default all)
//        [--list]                  collect only, no DeepL calls
//        [--skip-same]             skip entries identical to the English source
//        [--limit N]               only the first N strings (for a quick test)
//        [--out file.tsv]          also write a TSV report
import { readFileSync, writeFileSync, existsSync } from 'fs';
import { resolve, dirname } from 'path';
import { fileURLToPath } from 'url';
import { parseArgs } from 'util';
import { transformWithOxc } from 'vite';

const { values: args } = parseArgs({
  options: {
    lang: { type: 'string' },
    source: { type: 'string', default: 'all' },
    list: { type: 'boolean', default: false },
    'skip-same': { type: 'boolean', default: false },
    limit: { type: 'string' },
    out: { type: 'string' }
  }
});

const ROOT = resolve(dirname(fileURLToPath(import.meta.url)), '..');
const CORE_FILE = resolve(ROOT, 'src/core/locale_translations.h');
const i18nFile = (lang) => resolve(ROOT, `interface/src/i18n/${lang}/index.ts`);

// order in MAKE_*_TRANSLATION after the English slot
const CORE_LANGS = [
  'en',
  'de',
  'nl',
  'sv',
  'pl',
  'no',
  'fr',
  'tr',
  'it',
  'sk',
  'cs',
  'hu',
  'zh'
];
const WEBUI_LANGS = [
  'cs',
  'de',
  'fr',
  'hu',
  'it',
  'nl',
  'no',
  'pl',
  'sk',
  'sv',
  'tr',
  'zh'
];
const DEEPL_LANG = { no: 'NB', zh: 'ZH' };
const PLACEHOLDER = /\{[^{}]+\}|%[-\+ #0]*\d*(?:\.\d+)?[a-zA-Z]|EMS-ESP/g;
const BATCH = 50;

const lang = (args.lang ?? '').toLowerCase();
if (!lang) {
  console.error('Usage: DEEPL_API_KEY=... node backtranslate-check.js --lang <code>');
  process.exit(2);
}
if (args.source !== 'all' && args.source !== 'core' && args.source !== 'webui') {
  console.error('--source must be core, webui, or all');
  process.exit(2);
}
if (args.source !== 'webui' && !CORE_LANGS.includes(lang)) {
  console.error(`Unknown core language "${lang}". Known: ${CORE_LANGS.filter((l) => l !== 'en').join(', ')}`);
  process.exit(2);
}
if (args.source !== 'core' && !WEBUI_LANGS.includes(lang)) {
  console.error(`Unknown web UI language "${lang}". Known: ${WEBUI_LANGS.join(', ')}`);
  process.exit(2);
}
if (lang === 'en') {
  console.error('Pick a non-English language to back-translate');
  process.exit(2);
}

const limit = args.limit === undefined ? Infinity : Number(args.limit);
if (args.limit !== undefined && (!Number.isFinite(limit) || limit < 1)) {
  console.error('--limit must be a positive number');
  process.exit(2);
}

// --- parse locale_translations.h ------------------------------------------------
const UNESCAPE = { n: '\n', t: '\t', '"': '"', '\\': '\\' };
const ENTRY_START = /^(\s*)(\/\/\s*)?(MAKE_WORD_TRANSLATION|MAKE_TRANSLATION)\(/;

const readString = (line, i) => {
  let out = '';
  let j = i + 1;
  while (j < line.length) {
    const c = line[j];
    if (c === '\\') {
      const mapped = UNESCAPE[line[j + 1]];
      if (mapped === undefined) return null;
      out += mapped;
      j += 2;
    } else if (c === '"') {
      return { value: out, end: j + 1 };
    } else {
      out += c;
      j++;
    }
  }
  return null;
};

const parseCoreEntry = (line) => {
  const m = ENTRY_START.exec(line);
  if (!m || m[2]) return null;
  let i = m[0].length;
  const skipWs = () => {
    while (i < line.length && /\s/.test(line[i])) i++;
  };
  skipWs();
  const km = /^\w+/.exec(line.slice(i));
  if (!km) return null;
  const key = km[0];
  i += key.length;
  skipWs();
  if (line[i] !== ',') return null;
  i++;
  const strings = [];
  for (;;) {
    skipWs();
    if (line[i] !== '"') return null;
    const s = readString(line, i);
    if (!s) return null;
    strings.push(s.value);
    i = s.end;
    skipWs();
    if (line[i] === ',') {
      i++;
      continue;
    }
    if (line[i] === ')') break;
    return null;
  }
  const word = m[3] === 'MAKE_WORD_TRANSLATION';
  // MAKE_WORD: en, de, …   MAKE_TRANSLATION: mqtt-key, en, de, …
  const values = word ? strings : strings.slice(1);
  return { key, values };
};

const loadCore = (code) => {
  const idx = CORE_LANGS.indexOf(code);
  const text = readFileSync(CORE_FILE, 'utf8');
  const out = [];
  for (const line of text.split('\n')) {
    const e = parseCoreEntry(line);
    if (!e) continue;
    const en = e.values[0] ?? '';
    // machine drafts sometimes end with *
    let foreign = (e.values[idx] ?? '').replace(/\*+$/, '');
    if (!foreign.trim()) foreign = en; // missing slot falls back to English in firmware
    out.push({ source: 'core', key: e.key, text: foreign, en });
  }
  return out;
};

// --- web UI --------------------------------------------------------------------
const evalLocale = async (code, id) => {
  const { code: js } = await transformWithOxc(code, id, { lang: 'ts' });
  const url = `data:text/javascript;base64,${Buffer.from(js).toString('base64')}`;
  return (await import(url)).default;
};

const flatten = (obj, prefix = '') => {
  const rows = [];
  for (const [k, v] of Object.entries(obj)) {
    const key = prefix ? `${prefix}.${k}` : k;
    if (typeof v === 'string') rows.push({ key, text: v });
    else if (Array.isArray(v))
      v.forEach((item, i) => {
        if (typeof item === 'string') rows.push({ key: `${key}[${i}]`, text: item });
      });
  }
  return rows;
};

const loadWebui = async (code) => {
  const path = i18nFile(code);
  if (!existsSync(path)) {
    console.error(`Missing web UI locale file: ${path}`);
    process.exit(2);
  }
  const en = flatten(await evalLocale(readFileSync(i18nFile('en'), 'utf8'), i18nFile('en')));
  const enMap = Object.fromEntries(en.map((r) => [r.key, r.text]));
  const cur = flatten(await evalLocale(readFileSync(path, 'utf8'), path));
  return cur.map((r) => ({
    source: 'webui',
    key: r.key,
    text: r.text,
    en: enMap[r.key] ?? ''
  }));
};

// --- collect -------------------------------------------------------------------
const rows = [];
if (args.source === 'all' || args.source === 'core') rows.push(...loadCore(lang));
if (args.source === 'all' || args.source === 'webui') rows.push(...(await loadWebui(lang)));

let todo = rows.filter((r) => r.text.trim());
if (args['skip-same']) todo = todo.filter((r) => r.text !== r.en);
todo = todo.slice(0, limit);

console.log(
  `${lang}: ${todo.length} string(s)` +
    (args['skip-same'] ? ' (skipped identical-to-EN)' : '') +
    (Number.isFinite(limit) ? ` [limit ${limit}]` : '')
);
if (args.list) {
  for (const r of todo) console.log(`  [${r.source}] ${r.key}: ${r.text}`);
  process.exit(0);
}

// --- DeepL ---------------------------------------------------------------------
const apiKey = process.env.DEEPL_API_KEY;
if (!apiKey) {
  console.error('\nSet DEEPL_API_KEY to back-translate');
  process.exit(2);
}
const api =
  process.env.DEEPL_API_URL ??
  (apiKey.endsWith(':fx') ? 'https://api-free.deepl.com' : 'https://api.deepl.com');

const deepl = async (path, init = {}) => {
  const res = await fetch(api + path, {
    ...init,
    headers: {
      Authorization: `DeepL-Auth-Key ${apiKey}`,
      'Content-Type': 'application/json',
      ...init.headers
    }
  });
  if (!res.ok) throw new Error(`DeepL ${path}: ${res.status} ${await res.text()}`);
  return res.status === 204 ? null : res.json();
};

const xml = (s) =>
  s.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
const unxml = (s) =>
  s.replace(/&lt;/g, '<').replace(/&gt;/g, '>').replace(/&amp;/g, '&');
const protect = (text) => {
  const found = text.match(PLACEHOLDER) ?? [];
  let i = 0;
  return {
    found,
    xml: xml(text).replace(PLACEHOLDER, (p) => `<x id="${i++}">${p}</x>`)
  };
};
const restore = (out, found) =>
  unxml(
    out.replace(/<x id="(\d+)">.*?<\/x>/g, (_, i) => found[Number(i)] ?? `{?${i}}`)
  );

const sourceLang = DEEPL_LANG[lang] ?? lang.toUpperCase();
const back = [];
for (let i = 0; i < todo.length; i += BATCH) {
  const batch = todo.slice(i, i + BATCH).map((r) => protect(r.text));
  const { translations } = await deepl('/v2/translate', {
    method: 'POST',
    body: JSON.stringify({
      text: batch.map((b) => b.xml),
      source_lang: sourceLang,
      target_lang: 'EN-GB',
      tag_handling: 'xml',
      ignore_tags: ['x'],
      context:
        'UI labels and entity names from EMS-ESP, a home heating system gateway. ' +
        'Back-translate literally so a reviewer can spot inappropriate wording.'
    })
  });
  translations.forEach((t, j) => back.push(restore(t.text, batch[j].found)));
  process.stderr.write(`\rDeepL ${Math.min(i + BATCH, todo.length)}/${todo.length}`);
}
process.stderr.write('\n');

const tsvEscape = (s) => `"${String(s).replace(/"/g, '""')}"`;
const header = ['source', 'key', 'original', 'back_en', 'ref_en'];
const lines = [header.join('\t')];
console.log(`\n${'source'.padEnd(6)}  ${'key'.padEnd(28)}  back→EN  (original)  [ref EN]`);
console.log('-'.repeat(100));
todo.forEach((r, i) => {
  const enBack = back[i];
  console.log(
    `[${r.source}] ${r.key}\n` +
      `  original: ${r.text}\n` +
      `  back→EN:  ${enBack}\n` +
      `  ref EN:   ${r.en || '(none)'}`
  );
  lines.push(
    [r.source, r.key, r.text, enBack, r.en].map(tsvEscape).join('\t')
  );
});

if (args.out) {
  writeFileSync(args.out, lines.join('\n') + '\n');
  console.log(`\nWrote ${args.out}`);
}
