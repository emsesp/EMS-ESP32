import { Fragment, useMemo, useState } from 'react';

import { SearchField, Tag } from 'components/ui';
import type { TagKind } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';

import { FaultPackManager, brandName } from './FaultPackManager';
import { useFormat } from './ValueRow';
import { findFaults, packApplies, parseLastCode, searchFaults } from './faultLookup';
import type { FaultEntry, Severity } from './faultLookup';
import { useFaultPacks } from './faultPacks';
import { findItem, str } from './model';
import type { SysItem } from './model';

const SEV_TAG: Record<Severity, TagKind> = {
  info: 'ok',
  service: 'warn',
  warn: 'warn',
  error: 'err',
  unknown: 'muted'
};

// what a card can say about its code
//   entry   - explained from the pack of the brand
//   unknown - the pack has no entry for this code
//   nopack  - the brand is known, but no pack is installed for it
//   raw     - no brand, or the pack doesn't apply: only the raw code
//   loading - the installed packs aren't loaded yet
export type FaultState = 'entry' | 'unknown' | 'nopack' | 'raw' | 'loading';

export interface FaultCardData {
  kind: 'last' | 'ctrl' | 'service' | 'hit';
  code: string;
  add: number | null;
  span: string;
  state: FaultState;
  entry: FaultEntry | null; // the first entry, sets the severity
  entries: FaultEntry[]; // all entries of the code
}

// the fault codes the devices report, explained from the pack of the device's brand. The same code can mean
// something else for another manufacturer, so a pack is only used for the devices of its brand.
export const useFaultCards = (items: readonly SysItem[]) => {
  const { locale } = useI18nContext();

  const sources = useMemo(
    () => ({
      last: findItem(items, 'boiler:lastcode'),
      ctrl: findItem(items, 'thermostat:lastcode'),
      service: findItem(items, 'boiler:servicecode'),
      serviceNr: findItem(items, 'boiler:servicecodenumber')
    }),
    [items]
  );

  const boilerBrand =
    items
      .find((i) => i.devType === 'boiler')
      ?.brand.trim()
      .toLowerCase() ?? '';
  const brandOf = (i: SysItem) => i.brand.trim().toLowerCase() || boilerBrand;

  const brands = [
    ...new Set(
      [sources.last, sources.ctrl, sources.service]
        .map((i) => (i ? brandOf(i) : ''))
        .filter(Boolean)
    )
  ].sort();
  const { packs, info } = useFaultPacks(brands, locale);

  const cards: FaultCardData[] = [];
  const add = (
    kind: FaultCardData['kind'],
    item: SysItem,
    code: string,
    nr: number | null,
    span: string
  ) => {
    if (!code) return;
    const b = brandOf(item);
    const pack = b ? packs[b] : null;
    let state: FaultState;
    let entries: FaultEntry[] = [];
    if (!b) {
      state = 'raw';
    } else if (!info) {
      state = 'loading';
    } else if (!pack) {
      // installed but not loaded yet is not "missing"
      state = info.packs.some((p) => p.brand === b) ? 'loading' : 'nopack';
    } else if (!packApplies(pack, item.brand, boilerBrand)) {
      state = 'raw';
    } else {
      entries = findFaults(pack, code, nr, {
        product: item.prod,
        name: item.devName
      });
      state = entries.length > 0 ? 'entry' : 'unknown';
    }
    cards.push({
      kind,
      code,
      add: nr,
      span,
      state,
      entry: entries[0] ?? null,
      entries
    });
  };

  for (const kind of ['last', 'ctrl'] as const) {
    const item = sources[kind];
    const parsed = parseLastCode(item?.dv.v);
    if (item && parsed) add(kind, item, parsed.code, parsed.add, parsed.span);
  }
  if (sources.service) {
    const code = str(sources.service.dv.v).trim();
    const nr = Number(sources.serviceNr?.dv.v);
    add('service', sources.service, code, nr > 0 ? nr : null, '');
  }

  const missing = brands.filter(
    (b) => info && !info.packs.some((p) => p.brand === b)
  );
  return { cards, packs, brands, missing };
};

const FaultBody = ({ entry }: { entry: FaultEntry }) => {
  const { LL } = useI18nContext();
  return (
    <>
      {entry.scope && (
        <div className="sys-fault-lbl">{LL.SYS_FAULT_SCOPE(entry.scope)}</div>
      )}
      <div className="sys-fault-title">{entry.title}</div>
      {entry.cause && (
        <>
          <div className="sys-fault-lbl">{LL.SYS_FAULT_CAUSE()}</div>
          <p>{entry.cause}</p>
        </>
      )}
      {entry.fix && (
        <>
          <div className="sys-fault-lbl">{LL.SYS_FAULT_FIX()}</div>
          <p>{entry.fix}</p>
        </>
      )}
      {entry.flag && (
        <p className="sys-sub">
          {entry.flag === 'm' ? LL.SYS_FAULT_MACHINE() : LL.SYS_FAULT_FALLBACK()}
        </p>
      )}
      {entry.classText && (
        <details>
          <summary>{LL.SYS_FAULT_CLASS()}</summary>
          <p>
            <b>{entry.classText}</b>
            {'\n'}
            {entry.classMeaning}
          </p>
        </details>
      )}
    </>
  );
};

const FaultCard = ({ data }: { data: FaultCardData }) => {
  const { LL } = useI18nContext();
  const e = data.entry;
  const head =
    data.kind === 'last'
      ? LL.SYS_FAULT_LAST()
      : data.kind === 'ctrl'
        ? LL.SYS_FAULT_CTRL()
        : data.kind === 'hit'
          ? LL.SYS_FAULT_SEARCH()
          : LL.SYS_FAULT_SERVICE();
  const span = data.span.replace(/\s*-\s*now$/i, ` ${LL.SYS_FAULT_UNTIL_NOW()}`);
  const sev = {
    info: LL.SYS_SEV_INFO(),
    service: LL.SYS_SEV_SERVICE(),
    warn: LL.SYS_SEV_WARN(),
    error: LL.SYS_SEV_ERROR(),
    unknown: LL.SYS_SEV_UNKNOWN()
  };
  return (
    <div className={`sys-fault ${e?.severity ?? ''}`}>
      <div className="sys-fault-head">
        <span>{head}</span>
        {e && <Tag kind={SEV_TAG[e.severity]}>{sev[e.severity]}</Tag>}
      </div>
      <div className="sys-fault-code">
        {data.code}
        <small>
          {data.add ? LL.SYS_FAULT_ADD(data.add) : ''}
          {span ? ` · ${span}` : ''}
        </small>
      </div>
      {data.state === 'raw' && <p>{LL.SYS_FAULT_RAW_ONLY()}</p>}
      {data.state === 'nopack' && <p>{LL.SYS_FAULT_PACK_SHORT()}</p>}
      {data.state === 'unknown' && (
        <div className="sys-fault-title">{LL.SYS_FAULT_UNKNOWN()}</div>
      )}
      {data.entries.map((x, i) => (
        <div key={i}>
          {i > 0 && <hr />}
          <FaultBody entry={x} />
        </div>
      ))}
    </div>
  );
};

// the maintenance values of the boiler on one card
const MaintenanceCard = ({ items }: { items: readonly SysItem[] }) => {
  const { LL } = useI18nContext();
  const format = useFormat();
  const rows = [
    'boiler:maintenancemessage',
    'boiler:maintenance',
    'boiler:maintenancetime',
    'boiler:maintenancedate'
  ]
    .map((k) => findItem(items, k))
    .filter((i): i is SysItem => !!i);
  if (rows.length === 0) return null;
  return (
    <div className="sys-fault">
      <div className="sys-fault-head">
        <span>{LL.SYS_MAINTENANCE()}</span>
      </div>
      <div className="sys-kv" style={{ marginTop: 8 }}>
        {rows.map((i) => (
          <Fragment key={i.uid}>
            <span>{i.label}</span>
            <span>{format(i.dv.v, i.dv.u) || '–'}</span>
          </Fragment>
        ))}
      </div>
    </div>
  );
};

// the fault cards of the page Maintenance & faults, with a code lookup and the fault code packs
export const FaultPanel = ({ items }: { items: readonly SysItem[] }) => {
  const { LL } = useI18nContext();
  const { cards, packs, missing } = useFaultCards(items);
  const [q, setQ] = useState('');
  const loaded = Object.values(packs).filter((p) => p !== null);
  const results = q.trim()
    ? loaded.flatMap((p) => searchFaults(p, q).map((e) => ({ brand: p.brand, e })))
    : [];
  return (
    <>
      {cards.length === 0 && (
        <div className="sys-fault">
          <p>{LL.SYS_FAULT_NONE()}</p>
        </div>
      )}
      {cards.map((c) => (
        <FaultCard key={c.kind} data={c} />
      ))}
      <MaintenanceCard items={items} />
      {loaded.length > 0 && (
        <div className="sys-fault">
          <div className="sys-fault-head">{LL.SYS_FAULT_SEARCH()}</div>
          <p>
            <SearchField
              label={LL.SYS_FAULT_SEARCH()}
              placeholder="H08, 6A 227, H08(1018), 1018"
              value={q}
              onChange={(e) => setQ(e.target.value)}
            />
          </p>
          <p className="sys-sub" style={{ margin: '6px 0 0' }}>
            {LL.SYS_FAULT_SEARCH_HINT()}
          </p>
          <div className="sys-search-res">
            {results.map(({ brand, e }) => (
              <div key={`${brand}-${e.code}-${e.add}`}>
                {loaded.length > 1 && <p className="sys-sub">{brandName(brand)}</p>}
                <FaultCard
                  data={{
                    kind: 'hit',
                    code: e.code,
                    add: e.add,
                    span: '',
                    state: 'entry',
                    entry: e,
                    entries: [e]
                  }}
                />
              </div>
            ))}
          </div>
        </div>
      )}
      <FaultPackManager missing={missing} />
    </>
  );
};

export const faultSummary = (
  cards: readonly FaultCardData[]
): FaultCardData | undefined => cards.find((c) => c.kind === 'last');
