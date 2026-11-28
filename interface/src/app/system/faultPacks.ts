// Fault code packs (see faults/README.md): the explanations of a manufacturer's fault codes, one file per
// brand and language. They are stored on the gateway, so every browser sees them, also without internet.
// A pack is installed from a file; the Web UI validates it, compresses it and uploads it to the gateway.
import { useEffect, useSyncExternalStore } from 'react';

import {
  deleteFaultPack,
  readFaultPack,
  readFaultPacks,
  uploadFaultPack
} from '../../api/app';
import type { FaultPacksData } from '../../api/app';
import type { FaultPack, Severity } from './faultLookup';

// a page that lists the packs that can be downloaded
export const FAULT_PACK_LIST_URL = 'https://github.com/beabel/EMS-ESP-faultcodes';

const MAX_FILE_BYTES = 2_000_000; // uncompressed

// ----- validation of what comes from a source -----

const SEVERITIES: readonly Severity[] = [
  'info',
  'service',
  'warn',
  'error',
  'unknown'
];
const NAME = /^[a-z0-9-]{1,24}$/;
const VERSION = /^[A-Za-z0-9._-]{1,32}$/;

const isObject = (v: unknown): v is Record<string, unknown> =>
  typeof v === 'object' && v !== null && !Array.isArray(v);

// [code, additional code, class, title, cause, remedy], optionally with a flag and the id of a scope
const isCode = (r: unknown): boolean =>
  Array.isArray(r) &&
  r.length >= 6 &&
  r.length <= 8 &&
  typeof r[0] === 'string' &&
  (r[1] === null || typeof r[1] === 'number') &&
  [2, 3, 4, 5].every((i) => typeof r[i] === 'string') &&
  (r[6] === undefined || r[6] === '' || r[6] === 'f' || r[6] === 'm') &&
  (r[7] === undefined || typeof r[7] === 'string');

const isScope = (s: unknown): boolean =>
  isObject(s) &&
  (s.name === undefined || typeof s.name === 'string') &&
  (s.products === undefined ||
    (Array.isArray(s.products) && s.products.every((x) => typeof x === 'number'))) &&
  (s.names === undefined ||
    (Array.isArray(s.names) && s.names.every((x) => typeof x === 'string')));

export const isFaultPack = (p: unknown): p is FaultPack => {
  if (!isObject(p)) return false;
  if (
    p.format !== 1 ||
    typeof p.brand !== 'string' ||
    !NAME.test(p.brand) ||
    typeof p.lang !== 'string' ||
    !NAME.test(p.lang) ||
    typeof p.version !== 'string' ||
    !VERSION.test(p.version) ||
    typeof p.source !== 'string' ||
    typeof p.license !== 'string' ||
    !isObject(p.classes) ||
    !Array.isArray(p.codes)
  ) {
    return false;
  }
  const classesOk = Object.values(p.classes).every(
    (c) =>
      isObject(c) &&
      typeof c.title === 'string' &&
      typeof c.meaning === 'string' &&
      SEVERITIES.includes(c.severity as Severity)
  );
  const scopesOk =
    p.scopes === undefined ||
    (isObject(p.scopes) && Object.values(p.scopes).every(isScope));
  const appliesOk =
    p.applies === undefined ||
    (isObject(p.applies) &&
      (p.applies.brands === undefined ||
        (Array.isArray(p.applies.brands) &&
          p.applies.brands.every((b) => typeof b === 'string'))));
  return classesOk && scopesOk && appliesOk && p.codes.every(isCode);
};

// ----- installed on the gateway -----

interface Store {
  info: FaultPacksData | null;
  loading: boolean;
  packs: Map<string, FaultPack>; // "brand.lang.version" -> content
  pending: Set<string>;
  version: number;
}

const store: Store = {
  info: null,
  loading: false,
  packs: new Map(),
  pending: new Set(),
  version: 0
};
const listeners = new Set<() => void>();
const emit = () => {
  store.version++;
  listeners.forEach((l) => l());
};
const subscribe = (l: () => void) => {
  listeners.add(l);
  return () => listeners.delete(l);
};

export const refreshInstalled = async (): Promise<void> => {
  if (store.loading) return;
  store.loading = true;
  try {
    store.info = await readFaultPacks();
  } catch {
    store.info ??= { packs: [], fs_free: 0, max_size: 0 };
  } finally {
    store.loading = false;
    emit();
  }
};

// the language of the pack to use: the Web UI language, else English, else any
export const pickLang = (
  info: FaultPacksData | null,
  brand: string,
  locale: string
): string | undefined => {
  const langs = (info?.packs ?? [])
    .filter((p) => p.brand === brand)
    .map((p) => p.lang);
  return langs.includes(locale) ? locale : langs.includes('en') ? 'en' : langs[0];
};

const loadContent = async (brand: string, lang: string, version: string) => {
  const key = `${brand}.${lang}.${version}`;
  if (store.packs.has(key) || store.pending.has(key)) return;
  store.pending.add(key);
  try {
    const data = await readFaultPack(brand, lang);
    if (isFaultPack(data)) store.packs.set(key, data);
  } catch {
    // not readable: the raw codes stay
  } finally {
    store.pending.delete(key);
    emit();
  }
};

// The installed packs for the given brands, in the language of the Web UI. A brand maps to null while its
// pack is not installed or not loaded yet.
export const useFaultPacks = (brands: readonly string[], locale: string) => {
  const stamp = useSyncExternalStore(subscribe, () => store.version);
  const wanted = brands.join('|');

  useEffect(() => {
    if (!store.info && !store.loading) void refreshInstalled();
  }, []);

  useEffect(() => {
    if (!store.info) return;
    for (const brand of wanted.split('|').filter(Boolean)) {
      const lang = pickLang(store.info, brand, locale);
      const info = store.info.packs.find(
        (p) => p.brand === brand && p.lang === lang
      );
      if (info) void loadContent(brand, info.lang, info.version ?? '');
    }
  }, [wanted, locale, stamp]);

  const packs: Record<string, FaultPack | null> = {};
  for (const brand of brands) {
    const lang = pickLang(store.info, brand, locale);
    const info = store.info?.packs.find((p) => p.brand === brand && p.lang === lang);
    packs[brand] =
      (info && store.packs.get(`${brand}.${info.lang}.${info.version ?? ''}`)) ||
      null;
  }
  return { packs, info: store.info, loading: store.loading };
};

// ----- install and remove -----

const gzip = async (text: string): Promise<Blob> => {
  if (typeof CompressionStream === 'undefined') {
    throw new Error("this browser can't compress the file");
  }
  const stream = new Blob([text])
    .stream()
    .pipeThrough(new CompressionStream('gzip'));
  return new Response(stream).blob();
};

// validates a pack, compresses it and stores it on the gateway
const uploadPack = async (data: unknown): Promise<void> => {
  if (!isFaultPack(data)) throw new Error('not a valid fault code pack');
  const file = await gzip(JSON.stringify(data));
  const max = store.info?.max_size ?? 0;
  if (max > 0 && file.size > max) throw new Error('pack too large for the gateway');
  await uploadFaultPack(
    file,
    data.brand,
    data.lang,
    data.version,
    data.codes.length
  );
  store.packs.clear();
  await refreshInstalled();
};

// installs a pack from a file on the user's computer
export const installPackFile = async (file: File): Promise<void> => {
  if (file.size > MAX_FILE_BYTES) throw new Error('file too large');
  await uploadPack(JSON.parse(await file.text()) as unknown);
};

export const removePack = async (brand: string, lang: string): Promise<void> => {
  await deleteFaultPack({ brand, lang });
  store.packs.clear();
  await refreshInstalled();
};
