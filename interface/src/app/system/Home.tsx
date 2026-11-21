import { memo } from 'react';
import type { ReactNode } from 'react';

import { ButtonLink, Icon, Loader, Message, StatusDot } from 'components/ui';
import type { IconName, StatusState } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';

import { ChangesBar } from '../main/ChangesBar';
import { ValueControl, useFormat } from './ValueRow';
import { findItem, str } from './model';
import type { SysItem } from './model';
import { useEdits, useSystemData } from './useSystemData';
import type { Edits } from './useSystemData';
import { SystemBar, useSystemView } from './view';

// the "on" rendering of a boolean in every language of the firmware (see BOOL_PAIRS in the Dashboard)
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

const Tile = ({
  icon,
  title,
  sub,
  children
}: {
  icon: IconName;
  title: string;
  sub?: ReactNode;
  children: ReactNode;
}) => (
  <section className="sys-tile">
    <div className="sys-tile-head">
      <Icon name={icon} />
      {title}
    </div>
    <div className="sys-tile-sub">{sub}</div>
    {children}
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
  if (!item) return null;
  return (
    <>
      <span>{label}</span>
      <span>{format(item.dv.v, item.dv.u) || '–'}</span>
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

// Home: the most important things as tiles, easy to operate
const Home = memo(() => {
  const { LL } = useI18nContext();
  const view = useSystemView();
  const data = useSystemData();
  const edits = useEdits(data);
  const it = data.items;
  const f = (key: string, tag?: string) => findItem(it, key, tag);

  const bar = <SystemBar page="home" view={view} />;

  if (data.loading && it.length === 0) {
    return (
      <>
        {bar}
        <Loader />
      </>
    );
  }
  if (it.length === 0) {
    return (
      <>
        {bar}
        <div className="sys-tiles">
          <Message level={data.error ? 'error' : 'info'}>
            {data.error ?? LL.SYS_NO_DEVICES()}
          </Message>
        </div>
      </>
    );
  }

  const hcTags = [
    ...new Set(
      it
        .filter((i) => /^hc\d$/.test(i.tag) && i.key.startsWith('thermostat:hc/'))
        .map((i) => i.tag)
    )
  ].sort();

  const burner = f('boiler:burngas');
  const dhwCur = f('boiler:dhw/curtemp');
  // the controller has no target temperature on some systems, then the boiler's counts
  const dhwSet = f('thermostat:dhw/settemp') ?? f('boiler:dhw/seltemp');
  const dhwMode = f('thermostat:dhw/mode');
  const dhwCharge = f('thermostat:dhw/charge');

  return (
    <>
      {bar}
      <h1 className="sys-title">{LL.SYS_HOME()}</h1>
      <p className="sys-sub">{LL.SYS_HOME_HELP()}</p>
      <div className="sys-tiles">
        {hcTags.map((tag) => {
          const modeType = f('thermostat:hc/modetype', tag);
          const eco = /eco/i.test(str(modeType?.dv.v));
          const sel = f('thermostat:hc/seltemp', tag);
          return (
            <Tile
              key={tag}
              icon="device_thermostat"
              title={LL.SYS_HC(tag.slice(2))}
              sub={
                modeType && (
                  <Status state={eco ? 'off' : 'warn'}>{str(modeType.dv.v)}</Status>
                )
              }
            >
              <Big item={f('thermostat:hc/currtemp', tag)} />
              <div className="sys-cap">{LL.SYS_ROOM_TEMP()}</div>
              {sel && (
                <Line label={LL.SYS_WISH()}>
                  <Control item={sel} edits={edits} />
                </Line>
              )}
              <Control item={f('thermostat:hc/mode', tag)} edits={edits} />
            </Tile>
          );
        })}

        {(dhwMode ?? dhwCur) && (
          <Tile
            icon="water_drop"
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
        )}

        {f('boiler:curflowtemp') && (
          <Tile
            icon="local_fire_department"
            title={LL.SYS_GROUP_HEATING()}
            sub={
              isOn(burner) ? (
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
        )}

        <Tile icon="build" title={LL.SYS_NOTES()} sub={LL.SYS_NOTES_SUB()}>
          <div className="sys-kv">
            <Kv label={LL.SYS_LAST_FAULT()} item={f('boiler:lastcode')} />
            <Kv label={LL.SYS_MAINT_MSG()} item={f('boiler:maintenancemessage')} />
            <Kv label={LL.SYS_NEXT_MAINT()} item={f('boiler:maintenancedate')} />
          </div>
          <ButtonLink to="/system/service" color="neutral" icon="build">
            {LL.SYS_GOTO_SERVICE()}
          </ButtonLink>
        </Tile>
      </div>
      {edits.pending.size > 0 && (
        <ChangesBar
          message={LL.UNSAVED_CHANGES({ num: edits.pending.size })}
          saving={edits.saving}
          onDiscard={edits.discard}
          onSave={() => void edits.apply(LL.WRITE_CMD_SENT())}
        />
      )}
    </>
  );
});

export default Home;
