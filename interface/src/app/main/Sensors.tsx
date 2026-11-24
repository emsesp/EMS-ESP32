import { useContext, useState } from 'react';

import { useRequest } from 'alova/client';
import {
  Button,
  ConfirmDialog,
  Loader,
  SearchField,
  Section,
  SortHeader,
  Table,
  Tag,
  rowAction,
  toast,
  useTableSort
} from 'components/ui';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import { useInterval } from 'utils';

import {
  readSensorData,
  writeAnalogSensor,
  writeTemperatureSensor
} from '../../api/app';
import './Sensors.css';
import SensorsAnalogDialog from './SensorsAnalogDialog';
import SensorsTemperatureDialog from './SensorsTemperatureDialog';
import { formatValue } from './deviceValue';
import {
  AnalogType,
  AnalogTypeNames,
  DeviceValueUOM,
  DeviceValueUOM_s
} from './types';
import type { AnalogSensor, TemperatureSensor } from './types';

const MIN_ID = -100;
const MAX_ID = 100;

const num = (v: unknown) => {
  const n = Number(v);
  return isNaN(n) ? 0 : n;
};

// digital types show on/off; a Digital Out on a DAC pin has a 0-255 value
const isOnOff = (as: AnalogSensor) =>
  as.t === AnalogType.DIGITAL_IN ||
  as.t === AnalogType.PULSE ||
  (as.t === AnalogType.DIGITAL_OUT && as.g !== 25 && as.g !== 26);

const Sensors = () => {
  const { LL } = useI18nContext();
  const { me } = useContext(AuthenticatedContext);
  const [query, setQuery] = useState('');
  const [temp, setTemp] = useState<TemperatureSensor>();
  const [analog, setAnalog] = useState<AnalogSensor>();
  const [creating, setCreating] = useState(false);
  const [removing, setRemoving] = useState(false);

  const { data, error, send: fetchSensorData } = useRequest(readSensorData);

  useInterval(() => {
    if (!temp && !analog) void fetchSensorData();
  });

  const tempSort = useTableSort(data?.ts ?? [], {
    name: (a, b) => a.n.localeCompare(b.n),
    value: (a, b) => (a.t ?? 0) - (b.t ?? 0)
  });

  const analogSort = useTableSort(data?.as ?? [], {
    gpio: (a, b) => a.g - b.g,
    name: (a, b) => a.n.localeCompare(b.n),
    type: (a, b) => a.t - b.t,
    value: (a, b) => a.v - b.v
  });

  if (!data) {
    return <Loader error={error?.message} onRetry={() => void fetchSensorData()} />;
  }

  const q = query.trim().toLowerCase();
  const match = (...fields: (string | number)[]) =>
    !q || fields.join(' ').toLowerCase().includes(q);
  const temps = tempSort.rows.filter((ts) => match(ts.n, ts.id));
  const analogs = analogSort.rows.filter((as) => match(as.n, as.g));

  const saveTemperatureSensor = async (ts: TemperatureSensor) => {
    setTemp(undefined);
    try {
      await writeTemperatureSensor({
        id: ts.id,
        name: ts.n,
        offset: num(ts.o),
        is_system: ts.s
      });
      toast.success(LL.SENSOR_UPDATED());
    } catch (error) {
      toast.error(error instanceof Error ? error.message : LL.ERROR());
    }
    await fetchSensorData();
  };

  const saveAnalogSensor = async (as: AnalogSensor) => {
    setAnalog(undefined);
    try {
      await writeAnalogSensor({
        id: as.id,
        gpio: num(as.g),
        name: as.n,
        offset: num(as.o),
        factor: num(as.f),
        uom: num(as.u),
        type: num(as.t),
        deleted: as.d,
        is_system: as.s
      });
      toast.success(LL.ANALOG_SENSOR_UPDATED());
    } catch (error) {
      toast.error(error instanceof Error ? error.message : LL.ERROR());
    }
    await fetchSensorData();
  };

  const addAnalogSensor = () => {
    const gpio = data.available_gpios[0];
    if (gpio === undefined) {
      toast.error(LL.NO_GPIO());
      return;
    }
    setCreating(true);
    setAnalog({
      id: Math.floor(Math.random() * (MAX_ID - MIN_ID) + MIN_ID),
      n: '',
      g: gpio,
      u: DeviceValueUOM.NONE,
      v: 0,
      o: 0,
      f: 1,
      t: AnalogType.DIGITAL_IN,
      d: false,
      s: false
    });
  };

  const editAnalogSensor = (as: AnalogSensor) => {
    setCreating(false);
    setAnalog(as);
  };

  const offsetText = (ts: TemperatureSensor) =>
    `${ts.o > 0 ? '+' : ''}${ts.o} ${DeviceValueUOM_s[ts.u]}`;

  const empty = (cols: number) => (
    <tr>
      <td colSpan={cols} className="tbl-empty">
        {q ? LL.NO_SENSORS_MATCH() : LL.NO_SENSORS()}
      </td>
    </tr>
  );

  return (
    <>
      <Section
        className="sns"
        title={LL.SENSORS()}
        icon="sensors"
        actions={
          <SearchField
            label={LL.SEARCH_SENSORS()}
            value={query}
            onInput={(e) => setQuery(e.currentTarget.value)}
          />
        }
      >
        <Section
          title={LL.TEMP_SENSORS()}
          count={data.ts.length}
          icon="thermostat"
          desc={me.admin && LL.TEMP_SENSORS_HELP()}
        >
          <div className="tbl-wrap">
            <Table stack>
              <thead>
                <tr>
                  <SortHeader {...tempSort.header('name')}>{LL.NAME(0)}</SortHeader>
                  <th>ID</th>
                  <th className="r">{LL.OFFSET()}</th>
                  <SortHeader className="r" {...tempSort.header('value')}>
                    {LL.VALUE()}
                  </SortHeader>
                </tr>
              </thead>
              <tbody>
                {temps.length === 0 && empty(4)}
                {temps.map((ts) => (
                  <tr
                    key={ts.id}
                    {...(me.admin ? rowAction(() => setTemp(ts), ts.n) : {})}
                  >
                    <td data-label={LL.NAME(0)}>
                      <span>
                        {ts.n}
                        {ts.s && <Tag kind="sys">{LL.SYSTEM(0)}</Tag>}
                      </span>
                    </td>
                    <td data-label="ID" className="mono muted">
                      {ts.id}
                    </td>
                    <td
                      data-label={LL.OFFSET()}
                      className={ts.o ? 'r num lb' : 'r num muted'}
                    >
                      {offsetText(ts)}
                    </td>
                    <td data-label={LL.VALUE()} className="r num">
                      {formatValue(LL, ts.t, ts.u)}
                    </td>
                  </tr>
                ))}
              </tbody>
            </Table>
          </div>
        </Section>

        <Section
          title={LL.ANALOG_SENSORS()}
          count={data.as.length}
          icon="sensors"
          desc={
            me.admin &&
            data.analog_enabled && (
              <>
                {LL.FREE_GPIOS()}:{' '}
                <span className="mono">
                  {data.available_gpios.join(', ') || LL.NONE()}
                </span>
              </>
            )
          }
          actions={
            me.admin &&
            data.analog_enabled && (
              <Button icon="add" onClick={addAnalogSensor}>
                {LL.ADD(0)}
              </Button>
            )
          }
        >
          <div className="tbl-wrap">
            <Table stack>
              <thead>
                <tr>
                  <SortHeader {...analogSort.header('name')}>
                    {LL.NAME(0)}
                  </SortHeader>
                  <SortHeader {...analogSort.header('gpio')}>GPIO</SortHeader>
                  <SortHeader {...analogSort.header('type')}>
                    {LL.TYPE(0)}
                  </SortHeader>
                  <SortHeader className="r" {...analogSort.header('value')}>
                    {LL.VALUE()}
                  </SortHeader>
                </tr>
              </thead>
              <tbody>
                {analogs.length === 0 && empty(4)}
                {analogs.map((as) => (
                  <tr
                    key={as.id}
                    {...(me.admin
                      ? rowAction(() => editAnalogSensor(as), as.n)
                      : {})}
                  >
                    <td data-label={LL.NAME(0)}>
                      <span>
                        {as.n}
                        {as.s && <Tag kind="sys">{LL.SYSTEM(0)}</Tag>}
                      </span>
                    </td>
                    <td data-label="GPIO">
                      <span className="sns-gpio">{as.g}</span>
                    </td>
                    <td data-label={LL.TYPE(0)}>
                      <Tag kind="type">{AnalogTypeNames[as.t - 1]}</Tag>
                    </td>
                    <td data-label={LL.VALUE()} className="r num">
                      {isOnOff(as)
                        ? as.v
                          ? LL.ON()
                          : LL.OFF()
                        : formatValue(LL, as.v, as.u)}
                    </td>
                  </tr>
                ))}
              </tbody>
            </Table>
          </div>
        </Section>
      </Section>

      {temp && (
        <SensorsTemperatureDialog
          item={temp}
          sensors={data.ts}
          onClose={() => setTemp(undefined)}
          onSave={(ts) => void saveTemperatureSensor(ts)}
        />
      )}
      {analog && (
        <SensorsAnalogDialog
          item={analog}
          creating={creating}
          sensors={data.as}
          gpios={data.available_gpios}
          excludeTypes={data.exclude_types ?? []}
          onClose={() => setAnalog(undefined)}
          onSave={(as) => void saveAnalogSensor(as)}
          onRemove={() => setRemoving(true)}
        />
      )}
      {/* opens on top of the analog sensor dialog; Cancel goes back to it */}
      <ConfirmDialog
        open={removing && !!analog}
        title={LL.REMOVE_ANALOG_SENSOR()}
        label={LL.REMOVE()}
        icon="delete"
        color="error"
        onClose={() => setRemoving(false)}
        onConfirm={() => {
          setRemoving(false);
          return analog && saveAnalogSensor({ ...analog, d: true });
        }}
      >
        {LL.REMOVE_SENSOR_TEXT({ name: analog?.n ?? '' })}
      </ConfirmDialog>
    </>
  );
};

export default Sensors;
