import type { ReactNode } from 'react';

import { ButtonLink, Icon, StatusDot } from 'components/ui';
import type { IconName, StatusState } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';

import { ValueControl, useFormat } from './ValueRow';
import { findItem, str } from './model';
import type { SysItem } from './model';
import type { Edits } from './useSystemData';

// The tiles: the most important things of a heating circuit, hot water, the boiler and the notes. They are
// shown on the Dashboard overview and at the top of the page of the same name in the menu.

// the "on" rendering of a boolean in every language of the firmware (see BOOL_PAIRS in the old Dashboard)
const ON = new Set([
  'on',
  'an',
  'aan',
  'på',
  'włączono',
  'wł.',
  'açık',
  'zap',
  'be',
  'true',
  '1'
]);

const isOn = (item: SysItem | undefined): boolean =>
  !!item && ON.has(str(item.dv.v).trim().toLowerCase());

// a tile; with `link` it ends with the button to the page of its topic (on the Dashboard overview), which
// has the icon and the name of the menu entry
interface TileLink {
  to: string;
  icon: IconName;
  label: string;
}

const Tile = ({
  icon,
  title,
  sub,
  link,
  children
}: {
  icon: IconName;
  title: string;
  sub?: ReactNode;
  link?: TileLink | undefined;
  children: ReactNode;
}) => (
  <section className="sys-tile">
    <div className="sys-tile-head">
      <Icon name={icon} />
      {title}
    </div>
    <div className="sys-tile-sub">{sub}</div>
    {children}
    {link && (
      <div className="sys-tile-link">
        <ButtonLink to={link.to} color="neutral" icon={link.icon}>
          {link.label}
          <Icon name="open_in_new" size="sm" />
        </ButtonLink>
      </div>
    )}
  </section>
);

const Status = ({
  state,
  children
}: {
  state: StatusState;
  children: ReactNode;
}) => (
  <>
    <StatusDot state={state} />
    {children}
  </>
);

// big number with a small unit, e.g. 19.4 °C
const Big = ({ item }: { item: SysItem | undefined }) => {
  const format = useFormat();
  const text = item ? format(item.dv.v, item.dv.u) : '';
  const m = /^(.*?)\s+(\S+)$/.exec(text);
  return (
    <div className="sys-big">
      {text === '' ? '–' : m ? m[1] : text}
      {m && <small>{m[2]}</small>}
    </div>
  );
};

const Kv = ({ label, item }: { label: string; item: SysItem | undefined }) => {
  const format = useFormat();
  const { LL } = useI18nContext();
  if (!item) return null;
  const text = format(item.dv.v, item.dv.u) || '–';
  return (
    <>
      <span>{label}</span>
      <span>{text.replace(/\s*-\s*now$/i, ` ${LL.SYS_FAULT_UNTIL_NOW()}`)}</span>
    </>
  );
};

const Control = ({ item, edits }: { item: SysItem | undefined; edits: Edits }) => {
  const { LL } = useI18nContext();
  return item ? (
    <ValueControl
      variant="tile"
      item={item}
      value={edits.value(item)}
      onChange={(v) => edits.set(item, v)}
      onRun={() => void edits.run(item, LL.WRITE_CMD_SENT())}
    />
  ) : null;
};

const Line = ({ label, children }: { label: string; children: ReactNode }) => (
  <div className="sys-line">
    <span>{label}</span>
    {children}
  </div>
);

interface TileProps {
  items: readonly SysItem[];
  edits: Edits;
  // the page of the tile's topic, for the link at its end
  to?: string | undefined;
}

// the heating circuits that have a thermostat, e.g. ['hc1', 'hc2']
export const heatingCircuits = (items: readonly SysItem[]): string[] =>
  [
    ...new Set(
      items
        .filter((i) => /^hc\d$/.test(i.tag) && i.key.startsWith('thermostat:hc/'))
        .map((i) => i.tag)
    )
  ].sort();

export const HcTile = ({ tag, items, edits, to }: TileProps & { tag: string }) => {
  const { LL } = useI18nContext();
  const f = (key: string) => findItem(items, key, tag);
  const modeType = f('thermostat:hc/modetype');
  const eco = /eco/i.test(str(modeType?.dv.v));
  const sel = f('thermostat:hc/seltemp');
  // a mixer on this circuit
  const mixer = ['flowtemphc', 'valvestatus', 'pumpstatus']
    .map((k) => f(`mixer:hc/${k}`))
    .filter((i): i is SysItem => !!i);
  return (
    <Tile
      icon="device_thermostat"
      link={
        to
          ? { to, icon: 'device_thermostat', label: LL.SYS_HC(tag.slice(2)) }
          : undefined
      }
      title={LL.SYS_HC(tag.slice(2))}
      sub={
        modeType && (
          <Status state={eco ? 'off' : 'warn'}>{str(modeType.dv.v)}</Status>
        )
      }
    >
      <Big item={f('thermostat:hc/currtemp')} />
      <div className="sys-cap">{LL.SYS_ROOM_TEMP()}</div>
      {sel && (
        <Line label={LL.SYS_WISH()}>
          <Control item={sel} edits={edits} />
        </Line>
      )}
      <Control item={f('thermostat:hc/mode')} edits={edits} />
      {mixer.length > 0 && (
        <div className="sys-kv" style={{ marginTop: 10 }}>
          {mixer.map((i) => (
            <Kv key={i.uid} label={i.label} item={i} />
          ))}
        </div>
      )}
    </Tile>
  );
};

export const DhwTile = ({ items, edits, to }: TileProps) => {
  const { LL } = useI18nContext();
  const f = (key: string) => findItem(items, key);
  const dhwCur = f('boiler:dhw/curtemp');
  // the controller has no target temperature on some systems, then the boiler's counts
  const dhwSet = f('thermostat:dhw/settemp') ?? f('boiler:dhw/seltemp');
  const dhwMode = f('thermostat:dhw/mode');
  const dhwCharge = f('thermostat:dhw/charge');
  if (!(dhwMode ?? dhwCur)) return null;
  return (
    <Tile
      icon="water_drop"
      link={to ? { to, icon: 'water_drop', label: LL.SYS_DHW() } : undefined}
      title={LL.SYS_DHW()}
      sub={
        isOn(f('boiler:tapwateractive')) ? (
          <Status state="warn">{LL.SYS_DHW_HEATING()}</Status>
        ) : (
          <Status state="ok">{LL.SYS_READY()}</Status>
        )
      }
    >
      <Big item={dhwCur ?? dhwSet} />
      <div className="sys-cap">{LL.SYS_CURRENT()}</div>
      {dhwSet && (
        <Line label={LL.SYS_WISH()}>
          <Control item={dhwSet} edits={edits} />
        </Line>
      )}
      <Control item={dhwMode} edits={edits} />
      {dhwCharge && (
        <div style={{ marginTop: 10 }}>
          <Line label={LL.SYS_ONE_TIME()}>
            <Control item={dhwCharge} edits={edits} />
          </Line>
        </div>
      )}
    </Tile>
  );
};

export const BoilerTile = ({
  items,
  to
}: {
  items: readonly SysItem[];
  to?: string | undefined;
}) => {
  const { LL } = useI18nContext();
  const f = (key: string) => findItem(items, key);
  if (!f('boiler:curflowtemp')) return null;
  return (
    <Tile
      icon="local_fire_department"
      link={
        to
          ? {
              to,
              icon: 'local_fire_department',
              label:
                items.find((i) => i.menu === 'boiler')?.typeName ??
                LL.SYS_GROUP_HEATING()
            }
          : undefined
      }
      title={LL.SYS_GROUP_HEATING()}
      sub={
        isOn(f('boiler:burngas')) ? (
          <Status state="warn">{LL.SYS_BURNER_ON()}</Status>
        ) : (
          <Status state="ok">{LL.SYS_BURNER_OFF()}</Status>
        )
      }
    >
      <div className="sys-kv">
        <Kv label={LL.SYS_OUTDOOR()} item={f('boiler:outdoortemp')} />
        <Kv label={LL.SYS_FLOW()} item={f('boiler:curflowtemp')} />
        <Kv label={LL.SYS_RETURN()} item={f('boiler:rettemp')} />
        <Kv label={LL.SYS_PRESSURE()} item={f('boiler:syspress')} />
        <Kv label={LL.SYS_POWER()} item={f('boiler:curburnpow')} />
      </div>
    </Tile>
  );
};

export const NotesTile = ({ items }: { items: readonly SysItem[] }) => {
  const { LL } = useI18nContext();
  const f = (key: string) => findItem(items, key);
  const maintenance = ['boiler:maintenance', 'boiler:maintenancetime'].map((k) =>
    f(k)
  );
  return (
    <Tile
      icon="build"
      title={LL.SYS_NOTES()}
      sub={LL.SYS_NOTES_SUB()}
      link={{ to: '/dashboard/service', icon: 'build', label: LL.SYS_SERVICE() }}
    >
      <div className="sys-kv">
        <Kv label={LL.SYS_LAST_FAULT()} item={f('boiler:lastcode')} />
        <Kv label={LL.SYS_FAULT_CTRL()} item={f('thermostat:lastcode')} />
        <Kv label={LL.SYS_FAULT_SERVICE()} item={f('boiler:servicecode')} />
        <Kv label={LL.SYS_MAINT_MSG()} item={f('boiler:maintenancemessage')} />
        {maintenance.map((i) => (
          <Kv key={i?.uid} label={i?.label ?? ''} item={i} />
        ))}
        <Kv label={LL.SYS_NEXT_MAINT()} item={f('boiler:maintenancedate')} />
      </div>
    </Tile>
  );
};

// Summary tiles of the other pages: the key values of a device type, whatever the device reports
interface SummaryDef {
  icon: IconName;
  big?: string; // the main value
  status?: string; // a state shown under the title
  rows: string[];
}

const SUMMARIES: Record<string, SummaryDef> = {
  solar: {
    icon: 'solar_power',
    big: 'solar:collectortemp',
    status: 'solar:solarpump',
    rows: [
      'solar:cylbottomtemp',
      'solar:solarpumpmod',
      'solar:solarpower',
      'solar:energytoday',
      'solar:energytotal'
    ]
  },
  heatpump: {
    icon: 'heat_pump',
    status: 'heatpump:hpactivity',
    rows: [
      'heatpump:outdoortemp',
      'heatpump:rettemp',
      'heatpump:hpcompspd',
      'heatpump:hpcurrpower',
      'heatpump:heatingpumpmod',
      'heatpump:nrgtotal'
    ]
  },
  ventilation: {
    icon: 'mode_fan',
    big: 'ventilation:infresh',
    rows: [
      'ventilation:outexhaust',
      'ventilation:ventinspeed',
      'ventilation:ventoutspeed',
      'ventilation:airquality',
      'ventilation:airhumidity',
      'ventilation:bypass'
    ]
  }
};

export const hasSummary = (menu: string): boolean => menu in SUMMARIES;

// the name of the device type of a menu entry (Solar module, Ventilation, ...)
export const typeNameOf = (items: readonly SysItem[], menu: string): string =>
  items.find((i) => i.menu === menu && i.devType === menu)?.typeName ??
  items.find((i) => i.menu === menu)?.typeName ??
  '';

// the menu entries that have a summary tile
export const summaryMenus = (): string[] => Object.keys(SUMMARIES);

export const SummaryTile = ({
  menu,
  title,
  items,
  to
}: {
  menu: string;
  title: string;
  items: readonly SysItem[];
  to?: string | undefined;
}) => {
  const format = useFormat();
  const def = SUMMARIES[menu];
  if (!def) return null;
  const f = (key?: string) => (key ? findItem(items, key) : undefined);
  const big = f(def.big);
  const status = f(def.status);
  const rows = def.rows.map((k) => f(k)).filter((i): i is SysItem => !!i);
  if (!big && !status && rows.length === 0) return null;
  return (
    <Tile
      icon={def.icon}
      title={title}
      link={to ? { to, icon: def.icon, label: title } : undefined}
      sub={
        status && (
          <Status state={isOn(status) ? 'warn' : 'off'}>
            {status.label}: {format(status.dv.v, status.dv.u)}
          </Status>
        )
      }
    >
      {big && (
        <>
          <Big item={big} />
          <div className="sys-cap">{big.label}</div>
        </>
      )}
      <div className="sys-kv">
        {rows.map((i) => (
          <Kv key={i.uid} label={i.label} item={i} />
        ))}
      </div>
    </Tile>
  );
};
