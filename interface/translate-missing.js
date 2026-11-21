// Drafts missing web UI translations with DeepL, for a native speaker to review.
// Translates from English: keys a language doesn't have yet, keys whose English text changed
// since a git ref (--since), keys named with --keys, or every key (--all, replaces reviewed
// translations). Machine translations are marked `// DeepL`.
// Strings with plural syntax ({{s}}, {{a|b}}) in English or in the translation, and arrays, are
// listed for manual translation.
// usage: DEEPL_API_KEY=... node translate-missing.js [--lang de,nl] [--since <git ref>] [--keys A,B] [--all]
//        [--list] list what would be translated, no DeepL calls
//        (default) translate and print, files are not changed
//        [--write] translate and update src/i18n/<lang>/index.ts
import { execFileSync } from 'child_process';
import { readFileSync, writeFileSync } from 'fs';
import { parseArgs } from 'util';
import { transformWithOxc } from 'vite';

const { values: args } = parseArgs({
  options: {
    lang: { type: 'string' },
    since: { type: 'string' },
    keys: { type: 'string' },
    all: { type: 'boolean', default: false },
    list: { type: 'boolean', default: false },
    write: { type: 'boolean', default: false }
  }
});

const LANGS = [
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
const DEEPL_LANG = { no: 'NB' };
// match the tone of the existing translations; ignored by DeepL where unsupported
const FORMALITY = {
  de: 'prefer_more',
  fr: 'prefer_more',
  it: 'prefer_less',
  nl: 'prefer_less'
};
const CONTEXT =
  'Text in the web interface of EMS-ESP, an open-source gateway for monitoring and ' +
  'controlling home heating systems (boilers, heat pumps, thermostats, hot water). ' +
  'Most texts are short labels, buttons, menu items or messages: keep them short.';
// existing translations of these keys become a glossary, so DeepL reuses the same terms
const GLOSSARY_KEYS = [
  'DEVICE',
  'DEVICES',
  'SENSOR',
  'SENSORS',
  'ENTITIES',
  'ENTITY_NAME',
  'CUSTOMIZATIONS',
  'COMMANDS',
  'SCHEDULER',
  'DASHBOARD',
  'FAVORITES',
  'AUTOMATION',
  'CONNECTIVITY',
  'MAINTENANCE',
  'ANALOG_SENSORS',
  'TEMP_SENSORS',
  'MODULES'
];
const KEEP = ['EMS-ESP', 'EMS', 'MQTT', 'NTP', 'GPIO', 'API', 'Home Assistant'];
const MARK = ' // DeepL';

const fileOf = (lang) => `src/i18n/${lang}/index.ts`;
const PLACEHOLDER = /\{[^{}]+\}/g;

const evalLocale = async (code, id) => {
  const { code: js } = await transformWithOxc(code, id, { lang: 'ts' });
  const url = `data:text/javascript;base64,${Buffer.from(js).toString('base64')}`;
  return (await import(url)).default;
};

const en = await evalLocale(readFileSync(fileOf('en'), 'utf8'), fileOf('en'));
const enKeys = Object.keys(en);

let changedKeys = [];
if (args.since) {
  const old = execFileSync('git', ['show', `${args.since}:./${fileOf('en')}`], {
    encoding: 'utf8'
  });
  const before = await evalLocale(old, fileOf('en'));
  changedKeys = enKeys.filter(
    (k) => k in before && JSON.stringify(before[k]) !== JSON.stringify(en[k])
  );
}
const forcedKeys = args.keys ? args.keys.split(',') : [];
const unknown = forcedKeys.filter((k) => !(k in en));
if (unknown.length) {
  console.error(`Unknown keys: ${unknown.join(', ')}`);
  process.exit(2);
}

// what each language needs
const langs = args.lang ? args.lang.split(',') : LANGS;
const plan = [];
const manual = [];
for (const lang of langs) {
  const source = readFileSync(fileOf(lang), 'utf8');
  const current = await evalLocale(source, fileOf(lang));
  const keys = enKeys.filter(
    (k) =>
      args.all ||
      !(k in current) ||
      changedKeys.includes(k) ||
      forcedKeys.includes(k)
  );
  const todo = [];
  for (const key of keys) {
    const text = en[key];
    const old = current[key];
    if (
      typeof text !== 'string' ||
      text.includes('{{') ||
      (typeof old === 'string' && old.includes('{{'))
    )
      manual.push(`${lang} ${key}`);
    else todo.push(key);
  }
  plan.push({ lang, source, current, todo });
}

for (const { lang, todo } of plan) {
  console.log(`${lang}: ${todo.length ? todo.join(', ') : 'nothing to translate'}`);
}
if (manual.length) {
  console.log(
    `\nTranslate by hand (plural syntax or not a plain string):\n  ${manual.join(', ')}`
  );
}
const total = plan.reduce((n, p) => n + p.todo.length, 0);
if (args.list || !total) process.exit(0);

const apiKey = process.env.DEEPL_API_KEY;
if (!apiKey) {
  console.error('\nSet DEEPL_API_KEY to translate');
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

// placeholders become tags DeepL leaves alone; the text itself is XML-escaped
const xml = (s) =>
  s.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
const unxml = (s) =>
  s.replace(/&lt;/g, '<').replace(/&gt;/g, '>').replace(/&amp;/g, '&');
// the placeholder stays visible inside an ignored tag: an empty tag makes DeepL invent a noun
// for the gap ("tag {0}", "element {0}")
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
const sameArgs = (a, b) =>
  JSON.stringify((a.match(PLACEHOLDER) ?? []).sort()) ===
  JSON.stringify((b.match(PLACEHOLDER) ?? []).sort());

const glossaryFor = async (lang, current) => {
  const entries = new Map(KEEP.map((t) => [t, t]));
  for (const k of GLOSSARY_KEYS) {
    const from = en[k];
    const to = current[k];
    if (
      typeof from === 'string' &&
      typeof to === 'string' &&
      !/[{}\t]/.test(from + to)
    ) {
      entries.set(from, to);
    }
  }
  try {
    const { glossary_id } = await deepl('/v2/glossaries', {
      method: 'POST',
      body: JSON.stringify({
        name: `ems-esp-webui-${lang}`,
        source_lang: 'en',
        target_lang: (DEEPL_LANG[lang] ?? lang).toLowerCase(),
        entries: [...entries].map(([f, t]) => `${f}\t${t}`).join('\n'),
        entries_format: 'tsv'
      })
    });
    return glossary_id;
  } catch (e) {
    console.warn(`${lang}: no glossary (${e.message.split('\n')[0]})`);
    return undefined;
  }
};

// keeps the file's layout: replaces the key's line, or inserts before the next key in English order
const setLine = (source, key, value) => {
  const quoted = `'${value.replace(/\\/g, '\\\\').replace(/'/g, "\\'")}'`;
  const line = `  ${key}: ${quoted},${MARK}`;
  const lines = source.split('\n');
  const at = lines.findIndex((l) => new RegExp(`^\\s*${key}:`).test(l));
  if (at >= 0) {
    if (!/^\s*\w+: (['"`]).*\1,?(\s*\/\/.*)?$/.test(lines[at])) return null;
    lines[at] = line;
    return lines.join('\n');
  }
  const after = enKeys.slice(enKeys.indexOf(key) + 1);
  let insert = lines.findIndex((l) =>
    after.some((k) => new RegExp(`^\\s*${k}:`).test(l))
  );
  if (insert < 0) insert = lines.findIndex((l) => /^};/.test(l));
  lines.splice(insert, 0, line);
  return lines.join('\n');
};

const failed = [];
const written = [];
for (const p of plan.filter((x) => x.todo.length)) {
  const { lang, todo } = p;
  const glossary_id = await glossaryFor(lang, p.current);
  try {
    const results = [];
    for (let i = 0; i < todo.length; i += 50) {
      const batch = todo.slice(i, i + 50).map((k) => protect(en[k]));
      const { translations } = await deepl('/v2/translate', {
        method: 'POST',
        body: JSON.stringify({
          text: batch.map((b) => b.xml),
          source_lang: 'EN',
          target_lang: DEEPL_LANG[lang] ?? lang.toUpperCase(),
          context: CONTEXT,
          formality: FORMALITY[lang] ?? 'default',
          tag_handling: 'xml',
          ignore_tags: ['x'],
          ...(glossary_id && { glossary_id })
        })
      });
      translations.forEach((t, j) => results.push(restore(t.text, batch[j].found)));
    }
    console.log(`\n${lang}`);
    let source = p.source;
    todo.forEach((key, i) => {
      const text = results[i];
      if (!sameArgs(en[key], text)) {
        failed.push(`${lang} ${key}: placeholders changed ("${text}")`);
        return;
      }
      if (text === p.current[key]) {
        console.log(`  ${key}: (unchanged) ${text}`);
        return;
      }
      console.log(`  ${key}: ${text}`);
      if (args.write) {
        const next = setLine(source, key, text);
        if (next === null)
          failed.push(`${lang} ${key}: existing value spans lines, not replaced`);
        else source = next;
      }
    });
    if (args.write && source !== p.source) {
      writeFileSync(fileOf(lang), source);
      written.push(fileOf(lang));
    }
  } finally {
    if (glossary_id)
      await deepl(`/v2/glossaries/${glossary_id}`, { method: 'DELETE' });
  }
}

if (failed.length) console.log(`\nNot applied:\n  ${failed.join('\n  ')}`);
if (written.length) {
  execFileSync('npx', ['prettier', '-w', ...written], { stdio: 'ignore' });
  console.log(
    `\nUpdated ${written.length} file(s). Review the lines marked "${MARK.trim()}", ` +
      'then run `pnpm typesafe-i18n` and `pnpm check-layout`.'
  );
} else if (!args.write) {
  console.log('\nNothing written; add --write to update the translation files.');
}
