import { DeviceType } from '../main/types';
import type { Device, DeviceEntity, DeviceValue } from '../main/types';
import { CATALOG } from './catalog';
import type { CatalogEntry, SectionId } from './catalog';

// Data model of the System menu: assigns the values of the firmware (/rest/deviceData)
// via /rest/deviceEntities (short names) to the catalog and to a menu entry.

export const TYPE_NAME: Readonly<Record<number, string>> = {
  [DeviceType.BOILER]: 'boiler',
  [DeviceType.THERMOSTAT]: 'thermostat',
  [DeviceType.MIXER]: 'mixer',
  [DeviceType.SOLAR]: 'solar',
  [DeviceType.HEATPUMP]: 'heatpump',
  [DeviceType.GATEWAY]: 'gateway',
  [DeviceType.SWITCH]: 'switch',
  [DeviceType.CONTROLLER]: 'controller',
  [DeviceType.CONNECT]: 'connect',
  [DeviceType.ALERT]: 'alert',
  [DeviceType.EXTENSION]: 'extension',
  [DeviceType.HEATSOURCE]: 'heatsource',
  [DeviceType.VENTILATION]: 'ventilation',
  [DeviceType.WATER]: 'water',
  [DeviceType.POOL]: 'pool'
};

export interface SysItem {
  uid: string; // unique: device id + value id
  devId: number; // unique id of the device (for writeDeviceValue)
  devType: string;
  devName: string;
  typeName: string; // translated device type, e.g. Boiler
  brand: string;
  key: string; // catalog key, e.g. thermostat:hc/seltemp
  tag: string; // z. B. hc1, dhw, '' (kein Tag)
  label: string; // plain text from the firmware (without the tag prefix)
  dv: DeviceValue;
  section: SectionId;
  level: 1 | 2;
  order: number;
  range?: { min: number; max: number; step: number };
  known: boolean;
  menu: string;
}

// a value as text, safely (the firmware sends a number, a text or nothing)
export const str = (v: unknown): string =>
  typeof v === 'string'
    ? v
    : typeof v === 'number' || typeof v === 'boolean'
      ? String(v)
      : '';

export const TAG_PATTERN = /^(hc\d|wwc\d|dhw\d*|ahs\d|hs\d+|src\d+)$/;

const tagNorm = (tag: string): string =>
  tag.replace(/^(hc|wwc|dhw|ahs|hs|src)\d*$/, '$1');

const tagOfLabel = (name: string): string => {
  const first = name.split(' ')[0] ?? '';
  return TAG_PATTERN.test(first) ? first : '';
};

export const menuOf = (devType: string, tag: string, section: SectionId): string => {
  if (section === 'service') return 'service';
  if (section === 'energy') return 'energy';
  const t = tagNorm(tag);
  if (/^hc\d$/.test(tag)) return tag;
  if (t === 'dhw') return 'dhw';
  if (t === 'ahs') return 'ahs';
  if (t === 'hs') return 'hs';
  if (t === 'src') return 'src';
  switch (devType) {
    case 'boiler':
    case 'thermostat':
    case 'mixer':
    case 'solar':
    case 'heatpump':
    case 'ventilation':
    case 'pool':
      return devType;
    case 'controller':
      return 'thermostat';
    default:
      return 'other';
  }
};

const entityKey = (e: DeviceEntity): string =>
  (e.t ? e.t + ' ' : '') + (e.cn ?? e.n ?? '');

export const buildItems = (
  device: Device,
  nodes: readonly DeviceValue[],
  entities: readonly DeviceEntity[] | null
): SysItem[] => {
  const devType = TYPE_NAME[device.t];
  if (!devType) return [];
  const byName = new Map<string, DeviceEntity>();
  entities?.forEach((e) => {
    if (e.n && !e.n.startsWith('!')) {
      byName.set(entityKey(e), e);
    }
  });

  return nodes.map((dv, idx) => {
    const name = dv.id.slice(2);
    const ent = byName.get(name);
    let entityId = ent?.id ?? dv.c ?? '';
    let tag = '';
    const slash = entityId.indexOf('/');
    if (slash > 0) {
      tag = entityId.slice(0, slash);
    } else if (!entityId) {
      tag = tagOfLabel(name);
    } else if (ent?.t) {
      tag = ent.t;
    }
    if (!entityId) entityId = name;
    const short = slash > 0 ? entityId.slice(slash + 1) : entityId;
    const key = `${devType}:${tag ? tagNorm(tag) + '/' : ''}${short}`;
    const cat: CatalogEntry | undefined = CATALOG.get(key);
    const writable = !!dv.c;
    const section: SectionId = cat?.section ?? (writable ? 'other' : 'measure');
    // The firmware prefixes the (translated) tag, e.g. "HK1 selected room temperature"
    const prefix = [ent?.t, tag].find((p) => p && name.startsWith(p + ' '));
    const label = prefix ? name.slice(prefix.length + 1) : name;
    const item: SysItem = {
      uid: `${device.id}:${dv.id}`,
      devId: device.id,
      devType,
      devName: device.n,
      typeName: device.tn,
      brand: device.b,
      key,
      tag,
      label,
      dv,
      section,
      level: cat ? cat.level : writable ? 2 : 1,
      order: cat ? cat.order : 100000 + idx,
      known: !!cat,
      menu: menuOf(devType, tag, section)
    };
    if (cat?.range) item.range = cat.range;
    return item;
  });
};

export interface MenuDef {
  id: string;
  group: 'heating' | 'plant' | 'overview';
}

const GROUP_ORDER: Record<string, number> = { heating: 0, plant: 1, overview: 2 };
const PLANT_ORDER = [
  'dhw',
  'boiler',
  'heatpump',
  'ahs',
  'hs',
  'src',
  'solar',
  'mixer',
  'ventilation',
  'pool',
  'other'
];

const OVERVIEW_ORDER = ['thermostat', 'service', 'energy'];

export const buildMenus = (items: readonly SysItem[]): MenuDef[] => {
  const ids = [...new Set(items.map((i) => i.menu))];
  const defs: MenuDef[] = ids.map((id) => ({
    id,
    group: /^hc\d$/.test(id)
      ? 'heating'
      : id === 'service' || id === 'energy' || id === 'thermostat'
        ? 'overview'
        : 'plant'
  }));
  const rank = (d: MenuDef): number =>
    d.group === 'plant'
      ? PLANT_ORDER.indexOf(d.id)
      : d.group === 'overview'
        ? OVERVIEW_ORDER.indexOf(d.id)
        : Number(d.id.slice(2));
  return defs.sort(
    (a, b) =>
      (GROUP_ORDER[a.group] ?? 9) - (GROUP_ORDER[b.group] ?? 9) ||
      rank(a) - rank(b) ||
      a.id.localeCompare(b.id)
  );
};

export interface ItemGroup {
  title: string; // device name, empty when there is only one device
  section: SectionId;
  items: SysItem[];
}

export const groupItems = (
  list: readonly SysItem[],
  level: 1 | 2
): { groups: ItemGroup[]; hidden: number } => {
  const visible = list.filter((i) => level === 2 || i.level === 1);
  const hidden = list.length - visible.length;
  const devices = new Set(visible.map((i) => i.devId));
  const map = new Map<string, ItemGroup>();
  [...visible]
    .sort((a, b) => a.order - b.order)
    .forEach((i) => {
      const k = `${i.devId}|${i.section}`;
      let g = map.get(k);
      if (!g) {
        g = {
          title: devices.size > 1 ? i.devName : '',
          section: i.section,
          items: []
        };
        map.set(k, g);
      }
      g.items.push(i);
    });
  return { groups: [...map.values()], hidden };
};

export const findItem = (
  items: readonly SysItem[],
  key: string,
  tag?: string
): SysItem | undefined =>
  items.find((i) => i.key === key && (tag === undefined || i.tag === tag));

export const favoriteId = (i: SysItem): string => `${i.devType}:${i.dv.id.slice(2)}`;

// numeric range and step; the firmware (m, x, s) wins over the catalog
export const rangeOf = (
  i: SysItem
): { min: number; max: number; step: number } | null => {
  const { m, x, s } = i.dv;
  if (typeof m === 'number' && typeof x === 'number' && x > m) {
    const step = Number(s);
    return { min: m, max: x, step: step > 0 ? step : 1 };
  }
  return i.range ?? null;
};
