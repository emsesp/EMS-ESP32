import { useContext, useEffect, useRef, useState } from 'react';
import { useNavigate, useSearchParams } from 'react-router';

import { useRequest } from 'alova/client';
import {
  Button,
  Icon,
  IconButton,
  Loader,
  SearchField,
  Section,
  SortHeader,
  Table,
  ToggleButton,
  ToggleGroup,
  cx,
  rowAction,
  toast,
  useTableSort
} from 'components/ui';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import { saveBlob, useInterval } from 'utils';

import { readCoreData, readDeviceData, writeDeviceValue } from '../../api/app';
import DeviceIcon from './DeviceIcon';
import './Devices.css';
import DevicesDialog from './DevicesDialog';
import EmsBusWarning from './EmsBusWarning';
import { MASKS } from './EntityMaskToggle';
import { formatValue } from './deviceValue';
import { DeviceEntityMask, DeviceType, DeviceValueUOM_s } from './types';
import type { Device, DeviceValue } from './types';
import { deviceValueItemValidation } from './validators';

// the custom entities "device" always has this id
const CUSTOM_DEVICE_ID = 99;

const hasMask = (id: string, mask: number) =>
  (parseInt(id.slice(0, 2), 16) & mask) === mask;

const SHOWN_MASKS = MASKS.filter((o) =>
  [
    DeviceEntityMask.DV_FAVORITE,
    DeviceEntityMask.DV_READONLY,
    DeviceEntityMask.DV_API_MQTT_EXCLUDE
  ].includes(o.bit)
);

const hex = (n: number) => '0x' + n.toString(16).toUpperCase().padStart(2, '0');

const text = (v: unknown) =>
  typeof v === 'string' || typeof v === 'number' || typeof v === 'boolean'
    ? String(v)
    : '';

const escapeCsvCell = (cell: unknown) => {
  const sc = text(cell).trim();
  if (sc === '' || sc === '""') return sc;
  if (/["\n\r;]/.test(sc)) return '"' + sc.replace(/"/g, '""') + '"';
  return sc;
};

const compareValues = (a: DeviceValue, b: DeviceValue) =>
  typeof a.v === 'number' && typeof b.v === 'number'
    ? a.v - b.v
    : text(a.v).localeCompare(text(b.v));

const Devices = () => {
  const { LL } = useI18nContext();
  const { me } = useContext(AuthenticatedContext);
  const navigate = useNavigate();
  const [params, setParams] = useSearchParams();
  const [onlyFav, setOnlyFav] = useState(false);
  const [search, setSearch] = useState('');
  const [selectedValue, setSelectedValue] = useState<DeviceValue>();
  const [loadedId, setLoadedId] = useState<number>();
  const detailRef = useRef<HTMLElement>(null);

  const selectedId = Number(params.get('device')) || undefined;

  const {
    data: coreData,
    error: coreError,
    send: fetchCoreData
  } = useRequest(readCoreData);

  const {
    data: deviceData,
    error: deviceError,
    send: fetchDeviceData
  } = useRequest((id: number) => readDeviceData(id), {
    immediate: false
  }).onSuccess((event) => setLoadedId(event.args[0]));

  const { loading: submitting, send: sendDeviceValue } = useRequest(
    (data: { id: number; c: string; v: unknown }) => writeDeviceValue(data),
    { immediate: false }
  );

  const device = coreData?.devices.find((d) => d.id === selectedId);
  const coreLoaded = !!coreData;
  const selectable = !!device && device.e !== 0;

  useEffect(() => {
    if (selectedId !== undefined && coreLoaded && !selectable) {
      setParams({}, { replace: true });
    }
  }, [selectedId, coreLoaded, selectable]);

  useEffect(() => {
    if (selectedId === undefined || !selectable) return;
    setSearch('');
    void fetchDeviceData(selectedId);
  }, [selectedId, selectable]);

  useEffect(() => {
    const el = detailRef.current;
    if (!el || selectedId === undefined) return;
    const { top } = el.getBoundingClientRect();
    if (top < 0 || top > window.innerHeight * 0.6) {
      el.scrollIntoView({ block: 'start', behavior: 'smooth' });
    }
  }, [selectedId, loadedId]);

  const selectDevice = (id?: number) => {
    setParams(id === undefined ? {} : { device: String(id) });
  };

  useEffect(() => {
    const onKey = (e: KeyboardEvent) => {
      if (
        e.key === 'Escape' &&
        !e.defaultPrevented &&
        !document.querySelector('dialog[open]') &&
        !(e.target instanceof HTMLInputElement)
      ) {
        setParams({});
      }
    };
    document.addEventListener('keydown', onKey);
    return () => document.removeEventListener('keydown', onKey);
  }, [setParams]);

  useInterval(() => {
    if (selectedValue) return;
    void fetchCoreData();
    if (selectedId !== undefined) void fetchDeviceData(selectedId);
  });

  const nodes = loadedId === selectedId ? (deviceData?.nodes ?? []) : [];
  const sort = useTableSort(nodes, {
    name: (a, b) => a.id.slice(2).localeCompare(b.id.slice(2)),
    value: compareValues
  });
  if (!coreData) {
    return (
      <Loader error={coreError?.message} onRetry={() => void fetchCoreData()} />
    );
  }

  const favNodes = onlyFav
    ? nodes.filter((dv) => hasMask(dv.id, DeviceEntityMask.DV_FAVORITE))
    : nodes;
  const q = search.trim().toLowerCase();
  const shown = sort.rows.filter(
    (dv) =>
      (!onlyFav || hasMask(dv.id, DeviceEntityMask.DV_FAVORITE)) &&
      dv.id.slice(2).toLowerCase().includes(q)
  );

  const canWrite = (dv: DeviceValue) =>
    me.admin && dv.c !== undefined && !hasMask(dv.id, DeviceEntityMask.DV_READONLY);

  const customize = () => {
    if (selectedId === CUSTOM_DEVICE_ID) {
      void navigate('/customentities');
    } else {
      void navigate('/customizations', { state: selectedId });
    }
  };

  const download = () => {
    if (!device) return;
    const filename = device.tn + '_' + device.n;
    const columns: [string, (dv: DeviceValue) => unknown][] = [
      [LL.ENTITY_NAME(0), (dv) => dv.id.slice(2)],
      [
        LL.VALUE(0),
        (dv) =>
          typeof dv.v === 'number' ? new Intl.NumberFormat().format(dv.v) : dv.v
      ],
      [
        'UoM',
        (dv) =>
          dv.u !== undefined && DeviceValueUOM_s[dv.u]
            ? DeviceValueUOM_s[dv.u]?.replace(/[^a-zA-Z0-9]/g, '')
            : ''
      ],
      [
        LL.WRITEABLE(),
        (dv) =>
          dv.c && !hasMask(dv.id, DeviceEntityMask.DV_READONLY) ? 'yes' : 'no'
      ],
      [
        'Range',
        (dv) =>
          dv.h
            ? dv.h
            : dv.l
              ? dv.l.join(' | ')
              : dv.m !== undefined && dv.x !== undefined
                ? dv.m + ', ' + dv.x
                : ''
      ]
    ];
    const line = (cells: unknown[]) => cells.map(escapeCsvCell).join(';') + '\r\n';
    const csv =
      line(columns.map(([name]) => name)) +
      favNodes.map((dv) => line(columns.map(([, get]) => get(dv)))).join('');

    saveBlob(
      new Blob([JSON.stringify({ device, ...deviceData }, null, 2)], {
        type: 'application/json'
      }),
      filename + '.json'
    );
    saveBlob(new Blob([csv], { type: 'text/csv;charset=utf-8' }), filename + '.csv');
  };

  const saveValue = async (dv: DeviceValue) => {
    if (selectedId === undefined) return;
    await sendDeviceValue({ id: selectedId, c: dv.c ?? '', v: dv.v })
      .then(() => toast.success(LL.WRITE_CMD_SENT()))
      .catch((error: Error) => toast.error(error.message))
      .finally(() => {
        setSelectedValue(undefined);
        void fetchDeviceData(selectedId);
      });
  };

  const closeValue = () => {
    setSelectedValue(undefined);
    if (selectedId !== undefined) void fetchDeviceData(selectedId);
  };

  const deviceCaption = (d: Device) =>
    d.t === DeviceType.CUSTOM
      ? d.tn
      : [
          d.tn,
          d.b,
          `${LL.ID_OF(LL.DEVICE())} ${hex(d.d)}`,
          `${LL.ID_OF(LL.PRODUCT())} ${d.p}`,
          d.v && `${LL.VERSION()} ${d.v}`
        ]
          .filter(Boolean)
          .join(' · ');

  const renderDeviceList = () => (
    <Section
      title={`${LL.DEVICES()} (${coreData.devices.length})`}
      icon="category"
      className="dv-list"
    >
      {coreData.devices.length === 0 ? (
        <p className="tbl-empty dv-empty">{LL.NO_DEVICES()}</p>
      ) : (
        <div className="dv-cards">
          {coreData.devices.map((d) => (
            <button
              key={d.id}
              type="button"
              className={cx('dv-card', d.id === selectedId && 'sel')}
              disabled={d.e === 0}
              aria-current={d.id === selectedId || undefined}
              onClick={() => selectDevice(d.id)}
            >
              <span className="dv-av">
                <DeviceIcon type_id={d.t} size="sm" />
              </span>
              <span className="dv-text">
                <span className="dv-dname" title={d.n}>
                  {d.n}
                </span>
                <span className="cap">
                  {d.tn}
                  {d.b && ` · ${d.b}`}
                </span>
              </span>
              <span className="count" title={LL.ENTITIES(d.e)}>
                {d.e}
              </span>
            </button>
          ))}
        </div>
      )}
    </Section>
  );

  const renderDevice = (d: Device) => {
    const masksShown = SHOWN_MASKS.filter((o) =>
      shown.some((dv) => hasMask(dv.id, o.bit))
    );
    return (
      <section className="section dv-detail" ref={detailRef} aria-label={d.n}>
        <div className="dv-head">
          <Button
            className="dv-back"
            color="secondary"
            size="sm"
            icon="arrow_back"
            onClick={() => selectDevice()}
          >
            {LL.DEVICES()}
          </Button>
          <div className="row dv-title">
            <DeviceIcon type_id={d.t} className="lb" />
            <h2 className="h6">{d.n}</h2>
            <span className="grow" />
            <IconButton
              className="dv-close"
              icon="close"
              label={LL.CLOSE()}
              plain
              onClick={() => selectDevice()}
            />
          </div>
          <p className="cap dv-caption">{deviceCaption(d)}</p>
        </div>

        <div className="dv-toolbar">
          <SearchField
            label={LL.SEARCH()}
            value={search}
            onInput={(e) => setSearch(e.currentTarget.value)}
          />
          <ToggleGroup label={LL.FAVORITES()}>
            <ToggleButton on={!onlyFav} onClick={() => setOnlyFav(false)}>
              {LL.ALL()}
            </ToggleButton>
            <ToggleButton on={onlyFav} icon="star" onClick={() => setOnlyFav(true)}>
              {LL.FAVORITES()}
            </ToggleButton>
          </ToggleGroup>
          <span className="cap dv-count">
            {LL.SHOWING()} {shown.length}/{d.e} {LL.ENTITIES(shown.length)}
          </span>
          <span className="grow" />
          <Button icon="download" onClick={download} disabled={!nodes.length}>
            {LL.EXPORT()}
          </Button>
          {me.admin && (
            <Button color="secondary" icon="construction" onClick={customize}>
              {LL.CUSTOMIZATIONS()}
            </Button>
          )}
        </div>

        {loadedId !== selectedId ? (
          <Loader
            error={deviceError?.message}
            onRetry={() => void fetchDeviceData(d.id)}
          />
        ) : (
          <div className="dv-scroll">
            <Table className="dv-values">
              <thead>
                <tr>
                  <SortHeader {...sort.header('name')}>
                    {LL.ENTITY_NAME(0)}
                  </SortHeader>
                  <SortHeader className="r" {...sort.header('value')}>
                    {LL.VALUE(0)}
                  </SortHeader>
                  {me.admin && <th className="dv-act" />}
                </tr>
              </thead>
              <tbody>
                {shown.length === 0 && (
                  <tr>
                    <td colSpan={3} className="tbl-empty">
                      {LL.NO_ENTITIES_MATCH()}
                    </td>
                  </tr>
                )}
                {shown.map((dv) => (
                  <tr
                    key={dv.id}
                    {...rowAction(() => setSelectedValue(dv), dv.id.slice(2))}
                  >
                    <td>
                      <span className="dv-ename">
                        {dv.id.slice(2)}
                        {SHOWN_MASKS.map(
                          (o) =>
                            hasMask(dv.id, o.bit) && (
                              <Icon
                                key={o.bit}
                                name={o.icon}
                                size="xs"
                                label={o.label(LL)}
                                className={cx('dv-mask', `b${o.bit}`)}
                              />
                            )
                        )}
                      </span>
                    </td>
                    <td className="r dv-val">{formatValue(LL, dv.v, dv.u)}</td>
                    {me.admin && (
                      <td className="dv-act">
                        {canWrite(dv) && (
                          <Icon
                            name={dv.v === '' ? 'play_arrow' : 'edit'}
                            size="xs"
                            className="lb"
                          />
                        )}
                      </td>
                    )}
                  </tr>
                ))}
              </tbody>
            </Table>
          </div>
        )}
        {masksShown.length > 0 && (
          <p className="cap dv-legend">
            {masksShown.map((o) => (
              <span key={o.bit}>
                <Icon
                  name={o.icon}
                  size="xs"
                  className={cx('dv-mask', `b${o.bit}`)}
                />
                {o.label(LL)}
              </span>
            ))}
          </p>
        )}
      </section>
    );
  };

  return (
    <div className={cx('dv-page', device && 'has-sel')}>
      {!coreData.connected && <EmsBusWarning className="dv-msg" />}
      {coreData.connected && renderDeviceList()}
      {coreData.connected && device && renderDevice(device)}
      {selectedValue && (
        <DevicesDialog
          onClose={closeValue}
          onSave={(dv) => void saveValue(dv)}
          selectedItem={selectedValue}
          writeable={canWrite(selectedValue)}
          validator={deviceValueItemValidation(selectedValue)}
          progress={submitting}
        />
      )}
    </div>
  );
};

export default Devices;
