import { Fragment } from 'react';

import { useI18nContext } from 'i18n/i18n-react';

import { useFormat } from './ValueRow';
import { findItem, str } from './model';
import type { SysItem } from './model';

// The faults and the maintenance of the system as cards, on the page Maintenance & faults. The values come from
// the devices as they are; the codes are not explained here.

// the controller writes a few characters as look-alikes
const normalize = (s: string): string => s.toUpperCase().replace(/[Ξ≡]/g, '=');

interface ParsedCode {
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

interface FaultCardData {
  kind: 'last' | 'ctrl' | 'service';
  code: string;
  add: number | null;
  span: string;
}

const FaultCard = ({ data }: { data: FaultCardData }) => {
  const { LL } = useI18nContext();
  const head =
    data.kind === 'last'
      ? LL.SYS_FAULT_LAST()
      : data.kind === 'ctrl'
        ? LL.SYS_FAULT_CTRL()
        : LL.SYS_FAULT_SERVICE();
  const span = data.span.replace(/\s*-\s*now$/i, ` ${LL.SYS_FAULT_UNTIL_NOW()}`);
  return (
    <div className="sys-fault">
      <div className="sys-fault-head">
        <span>{head}</span>
      </div>
      <div className="sys-fault-code">
        {data.code}
        <small>
          {data.add ? LL.SYS_FAULT_ADD(data.add) : ''}
          {span ? ` · ${span}` : ''}
        </small>
      </div>
      <p>{LL.SYS_FAULT_RAW_ONLY()}</p>
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

// the fault cards of the page Maintenance & faults
export const FaultPanel = ({ items }: { items: readonly SysItem[] }) => {
  const { LL } = useI18nContext();
  const cards: FaultCardData[] = [];
  for (const kind of ['last', 'ctrl'] as const) {
    const item = findItem(
      items,
      kind === 'last' ? 'boiler:lastcode' : 'thermostat:lastcode'
    );
    const parsed = parseLastCode(item?.dv.v);
    if (parsed) cards.push({ kind, ...parsed });
  }
  const service = findItem(items, 'boiler:servicecode');
  const code = str(service?.dv.v).trim();
  if (code) {
    const nr = Number(findItem(items, 'boiler:servicecodenumber')?.dv.v);
    cards.push({ kind: 'service', code, add: nr > 0 ? nr : null, span: '' });
  }
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
    </>
  );
};
