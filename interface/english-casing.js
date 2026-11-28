// Title case for short English web UI labels: "Custom Entities", not "Custom entities".
// Shared by check-layout.js (report) and translate-missing.js (--casing).
// Firmware names in locale_translations.h are not handled here.

const CASING_ACRONYMS = new Set([
  'EMS',
  'ESP',
  'MQTT',
  'NTP',
  'API',
  'GPIO',
  'LED',
  'PHY',
  'ID',
  'Tx',
  'Rx',
  'TX',
  'RX',
  'ON',
  'OFF',
  'DHW',
  'KB',
  'MB',
  'OTA',
  'WiFi',
  'Wi-Fi',
  'AP',
  'HTTP',
  'HTTPS',
  'IP',
  'DNS',
  'DHCP',
  'SSL',
  'TLS',
  'RGB',
  'PWM',
  'NTC',
  'HT3',
  'CRC',
  'UART',
  'USB',
  'SDK',
  'CPU',
  'PSRAM',
  'RSSI',
  'HA',
  'JSON',
  'CSV',
  'URL',
  'UI',
  'N/A',
  'UoM'
]);
const CASING_PROPER = new Set([
  'Fahrenheit',
  'Celsius',
  'Telnet',
  'Syslog',
  'Ethernet',
  'Modbus',
  'Dallas',
  'Discord',
  'Arduino',
  'Espressif',
  'LittleFS',
  'Home',
  'Assistant',
  'BBQKees',
  'EMS-ESP',
  'Bearer'
]);
// not capitalized in title case, unless they are the first word
const CASING_SMALL = new Set([
  'a',
  'an',
  'the',
  'and',
  'but',
  'or',
  'nor',
  'for',
  'of',
  'on',
  'in',
  'to',
  'with',
  'at',
  'by',
  'from',
  'as',
  'per',
  'via',
  'vs'
]);

const isShortLabel = (value) => {
  if (/[.!?]/.test(value)) return false;
  const words = value.trim().split(/\s+/);
  return words.length >= 2 && words.length <= 4;
};

const titleCaseWord = (bare) =>
  bare
    .split('-')
    .map((part) => part.charAt(0).toUpperCase() + part.slice(1))
    .join('-');

const titleCaseSuggestion = (value, badWords) => {
  const bad = new Set(badWords);
  return value
    .split(/(\s+)/)
    .map((part, i) => {
      if (/^\s+$/.test(part) || i === 0) return part;
      const bare = part.replace(/^[^A-Za-z0-9]+|[^A-Za-z0-9]+$/g, '');
      if (!bad.has(bare)) return part;
      return part.replace(bare, titleCaseWord(bare));
    })
    .join('');
};

// A value that is only a typesafe-i18n plural is checked once per alternative.
// Splitting on spaces alone would hide "Entities" inside "entities|Custom".
const labelTexts = (value) => {
  const whole = value.match(/^\{\{([^{}]*)\}\}$/);
  return whole ? [...new Set(whole[1].split('|'))] : [value];
};

const casingIssue = (key, value) => {
  if (!isShortLabel(value)) return null;
  const words = value.trim().split(/\s+/);
  // "mark as favorite" is a sentence fragment, not a title
  const first = words[0].replace(/^[^A-Za-z0-9]+|[^A-Za-z0-9]+$/g, '');
  if (/^[a-z]/.test(first)) return null;
  const bad = [];
  for (let i = 1; i < words.length; i++) {
    const word = words[i];
    if (/[{}]/.test(word)) continue;
    const bare = word.replace(/^[^A-Za-z0-9]+|[^A-Za-z0-9]+$/g, '');
    if (!bare || CASING_SMALL.has(bare)) continue;
    if (CASING_ACRONYMS.has(bare) || CASING_PROPER.has(bare)) continue;
    // a main word written in lowercase: "entities" in "Custom entities"
    if (/^[a-z]+(?:-[a-z]+)*$/.test(bare)) bad.push(bare);
  }
  if (!bad.length) return null;
  return { key, value, bad, suggest: titleCaseSuggestion(value, bad) };
};

// null when this value is already title case. A {{a|b|c}} plural is rewritten as a whole.
const fixEnglishLabel = (value) => {
  const whole = value.match(/^\{\{([^{}]*)\}\}$/);
  if (whole) {
    let changed = false;
    const alts = whole[1].split('|').map((alt) => {
      const issue = casingIssue('', alt);
      if (!issue) return alt;
      changed = true;
      return issue.suggest;
    });
    return changed ? `{{${alts.join('|')}}}` : null;
  }
  const issue = casingIssue('', value);
  return issue ? issue.suggest : null;
};

const quote = (value) => `'${value.replace(/\\/g, '\\\\').replace(/'/g, "\\'")}'`;

export const checkEnglishCasing = (src) => {
  const entries = [...src.matchAll(/^\s*([A-Z0-9_]+):\s*'((?:\\'|[^'])*)'/gm)].map(
    ([, key, value]) => [key, value.replace(/\\'/g, "'")]
  );
  return entries.flatMap(([key, value]) =>
    labelTexts(value)
      .map((text) => casingIssue(key, text))
      .filter(Boolean)
  );
};

// Rewrites short labels in the English locale source. Comments and other lines stay put.
export const applyEnglishCasing = (src) => {
  const changes = [];
  const next = src.replace(
    /^(\s*)([A-Z0-9_]+)(:\s*)'((?:\\'|[^'])*)'(.*)$/gm,
    (full, indent, key, sep, raw, suffix) => {
      const value = raw.replace(/\\'/g, "'");
      const fixed = fixEnglishLabel(value);
      if (fixed == null) return full;
      changes.push({ key, from: value, to: fixed });
      return `${indent}${key}${sep}${quote(fixed)}${suffix}`;
    }
  );
  return { src: next, changes };
};
