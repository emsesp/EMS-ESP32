// Looking up fault codes in a fault code pack (see faults/README.md).
// IMPORTANT: The same code can mean something completely different for another manufacturer.
// A pack is therefore only used for the devices of its brand (packApplies).

export type Severity = 'info' | 'service' | 'warn' | 'error' | 'unknown';

export interface FaultClass {
  title: string;
  meaning: string;
  severity: Severity;
}

// [code, additional code or null, class id, title, cause, remedy, flag, scope]
// The 7th value is a flag: 'f' = the text is the one of the original language (no translation yet), 'm' = machine
// translated and not reviewed, '' = none. The 8th value is the id of a scope (the product series the entry is for).
export type FaultCode =
  | [string, number | null, string, string, string, string]
  | [string, number | null, string, string, string, string, string]
  | [string, number | null, string, string, string, string, string, string];

// the product series or devices some entries are for: a code can mean something else on another series
export interface FaultScope {
  name?: string;
  products?: number[]; // product ids of the devices
  names?: string[]; // words in the device name, case does not matter
  source?: string;
}

// what a fault card knows about the device its code comes from
export interface FaultDevice {
  product: number;
  name: string;
}

export interface FaultPack {
  format: 1;
  brand: string; // lower case, as the devices report it
  lang: string;
  version: string;
  source: string;
  license: string;
  notice?: string;
  applies?: { unbranded_with_boiler?: boolean; brands?: string[] };
  scopes?: Record<string, FaultScope>;
  classes: Record<string, FaultClass>;
  codes: FaultCode[];
}

export interface FaultEntry {
  code: string;
  add: number | null;
  title: string;
  cause: string;
  fix: string;
  classText: string;
  classMeaning: string;
  severity: Severity;
  flag?: 'f' | 'm'; // not translated yet / machine translated
  scope?: string; // name of the series the entry is for
}

// A pack is used for a device of its brand, or for a device without a brand (e.g. a controller)
// when the pack says so and the boiler of the system has the brand of the pack.
export const packApplies = (
  pack: FaultPack,
  deviceBrand: string,
  boilerBrand: string
): boolean => {
  const b = deviceBrand.trim().toLowerCase();
  return (
    b === pack.brand ||
    (b !== '' &&
      !!pack.applies?.brands?.some((x) => x.trim().toLowerCase() === b)) ||
    (b === '' && !!pack.applies?.unbranded_with_boiler && boilerBrand === pack.brand)
  );
};

const normalize = (s: string): string => s.toUpperCase().replace(/[Ξ≡]/g, '=');

const toEntry = (pack: FaultPack, row: FaultCode): FaultEntry => {
  const [code, add, cls, title, cause, fix, flag, scopeId] = row;
  const c = pack.classes[cls];
  const scope = scopeId ? pack.scopes?.[scopeId] : undefined;
  return {
    code,
    add,
    title,
    cause,
    fix,
    classText: c?.title ?? '',
    classMeaning: c?.meaning ?? '',
    severity: c?.severity ?? 'unknown',
    ...(flag === 'f' || flag === 'm' ? { flag } : {}),
    ...(scopeId ? { scope: scope?.name || scopeId } : {})
  };
};

// does a device belong to the scope: by product id, or by a word in the device name
const inScope = (scope: FaultScope | undefined, device: FaultDevice): boolean => {
  if (!scope) return false;
  const name = device.name.toLowerCase();
  return (
    !!scope.products?.includes(device.product) ||
    !!scope.names?.some(
      (w) => w.trim() !== '' && name.includes(w.trim().toLowerCase())
    )
  );
};

// All entries of a code: a code can have several, e.g. for different variants of a product. An entry without a
// scope is for every device of the brand. An entry with a scope is only for a device that matches it. When entries of
// both kinds exist, the ones that match a scope are shown.
export const findFaults = (
  pack: FaultPack,
  code: string,
  add: number | null,
  device: FaultDevice
): FaultEntry[] => {
  const c = normalize(code);
  const rows = pack.codes.filter((r) => r[0].toUpperCase() === c && r[1] === add);
  const scoped = rows.filter((r) => r[7] && inScope(pack.scopes?.[r[7]], device));
  const use = scoped.length > 0 ? scoped : rows.filter((r) => !r[7]);
  return use.map((r) => toEntry(pack, r));
};

export interface ParsedCode {
  code: string;
  add: number;
  span: string;
}

// "H08(1018) 20.11.2026 20:58 - now"
export const parseLastCode = (value: unknown): ParsedCode | null => {
  const m = /^\s*([A-Z0-9=-]+)\((\d+)\)\s*(.*)$/.exec(
    normalize(
      typeof value === 'string' || typeof value === 'number' ? String(value) : ''
    )
  );
  if (!m) return null;
  return { code: m[1] ?? '', add: Number(m[2]), span: m[3] ?? '' };
};

export const searchFaults = (pack: FaultPack, raw: string): FaultEntry[] => {
  const q = normalize(raw.trim());
  if (!q) return [];
  let rows: FaultCode[];
  const m = /^([A-Z0-9=-]{1,3})\s*[\s(\-/,:]+\s*(\d{1,4})\)?$/.exec(q);
  if (m) {
    rows = pack.codes.filter(
      (r) => r[0].toUpperCase() === m[1] && r[1] === Number(m[2])
    );
  } else if (/^\d+$/.test(q)) {
    rows = pack.codes.filter((r) => r[0] === q || r[1] === Number(q));
  } else {
    const code = q.replace(/[^A-Z0-9]/g, '');
    rows = pack.codes.filter((r) => r[0].toUpperCase() === code);
  }
  return rows.slice(0, 25).map((r) => toEntry(pack, r));
};
