import { Fragment, useMemo } from 'react';
import { Link } from 'react-router';

import { useRequest } from 'alova/client';
import { Icon, Loader, Message, Tag } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import { useInterval } from 'utils';

import { readSensorData } from '../../api/app';
import { isOnOff } from '../main/Sensors';
import { formatValue } from '../main/deviceValue';
import type { SensorData } from '../main/types';
import { Tile } from './Tiles';

// The temperature and analog sensors of the gateway in the Dashboard: only the name and the value. The page for
// changing them is the Sensors page of the main menu, which the icon leads to. The sensors are read once by the
// menu and shared by the menu entry (the count), the tile of Home and the page.

interface SensorRow {
  key: string;
  name: string;
  value: string;
  system: boolean;
}

export interface Sensors {
  data: SensorData | undefined;
  error: Error | undefined;
  reload: () => void;
  temps: SensorRow[];
  analogs: SensorRow[];
}

export const useSensors = (): Sensors => {
  const { LL } = useI18nContext();
  const { data, error, send } = useRequest(readSensorData);
  useInterval(() => {
    if (!document.hidden) void send();
  }, 15000);

  const rows = useMemo(() => {
    const temps: SensorRow[] = (data?.ts ?? []).map((s) => ({
      key: `t${s.id}`,
      name: s.n,
      value: formatValue(LL, s.t, s.u),
      system: s.s
    }));
    const analogs: SensorRow[] = (data?.as ?? []).map((s) => ({
      key: `a${s.id}`,
      name: s.n,
      value: isOnOff(s) ? (s.v ? LL.ON() : LL.OFF()) : formatValue(LL, s.v, s.u),
      system: s.s
    }));
    return { temps, analogs };
  }, [data, LL]);

  return { data, error, reload: () => void send(), ...rows };
};

const NameCell = ({ row }: { row: SensorRow }) => {
  const { LL } = useI18nContext();
  return (
    <>
      {row.name}
      {row.system && <Tag kind="sys">{LL.SYSTEM(0)}</Tag>}
    </>
  );
};

const SensorsOverview = ({ sensors }: { sensors: Sensors }) => {
  const { LL } = useI18nContext();
  const { data, error, reload, temps, analogs } = sensors;

  if (!data) return <Loader error={error?.message} onRetry={reload} />;
  if (temps.length + analogs.length === 0) {
    return <Message level="info">{LL.NO_SENSORS()}</Message>;
  }

  const head = (title: string) => (
    <div className="sys-sec-head">
      <span>{title}</span>
      <Link
        to="/sensors"
        className="sys-sec-link"
        aria-label={LL.SENSORS()}
        title={LL.SENSORS()}
      >
        <Icon name="open_in_new" size="xs" />
      </Link>
    </div>
  );
  const list = (title: string, rows: SensorRow[]) =>
    rows.length > 0 && (
      <section className="section">
        {head(title)}
        {rows.map((r) => (
          <div key={r.key} className="sys-row">
            <div className="sys-lab">
              <span className="txt">
                <b>
                  <NameCell row={r} />
                </b>
              </span>
            </div>
            <div className="sys-ctl">{r.value}</div>
          </div>
        ))}
      </section>
    );

  return (
    <>
      {list(LL.TEMP_SENSORS(), temps)}
      {list(LL.ANALOG_SENSORS(), analogs)}
    </>
  );
};

// the tiles of Home, one for each kind of sensor: all of them with name and value, and the icon to the page
export const SensorsTile = ({
  sensors,
  kind,
  to
}: {
  sensors: Sensors;
  kind: 'temps' | 'analogs';
  to: string;
}) => {
  const { LL } = useI18nContext();
  const rows = sensors[kind];
  if (rows.length === 0) return null;
  const title = kind === 'temps' ? LL.TEMP_SENSORS() : LL.ANALOG_SENSORS();

  return (
    <Tile
      icon={kind === 'temps' ? 'thermostat' : 'sensors'}
      title={title}
      link={{ to, label: LL.SENSORS() }}
    >
      <div className="sys-kv">
        {rows.map((r) => (
          <Fragment key={r.key}>
            <span>
              <NameCell row={r} />
            </span>
            <span>{r.value}</span>
          </Fragment>
        ))}
      </div>
    </Tile>
  );
};

export default SensorsOverview;
