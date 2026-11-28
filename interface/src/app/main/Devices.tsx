import { useContext, useEffect, useRef, useState } from 'react';
import { useNavigate, useSearchParams } from 'react-router';

import { useRequest } from 'alova/client';
import {
  Button,
  Icon,
  IconButton,
  Loader,
  Menu,
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
import type { MenuItem } from 'components/ui';
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

// circuit tags the firmware puts in front of entity names, in every language (tag_* in locale_translations.h)
const TAGGED =
  /^((?:hc|hk|vk|og|id|ahs|ahq|avk|aźc|hs|hq|źc|src)\d{1,2}|(?:dhw|wwk|vvk|cwu|sks|tuv|túv|ecs|hmv)\d{0,2}) (.+)$/iu;

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

const highlight = (s: string, q: string) => {
  const i = q ? s.toLowerCase().indexOf(q) : -1;
  if (i < 0) return s;
  return (
    <>
      {s.slice(0, i)}
      <mark>{s.slice(i, i + q.length)}</mark>
      {s.slice(i + q.length)}
    </>
  );
};

interface Row {
  dv: DeviceValue;
  name: string;
  tag: string;
  short: string;
  value: string;
}

const Devices = () => {
  const { LL } = useI18nContext();
  const { me } = useContext(AuthenticatedContext);
  const navigate = useNavigate();
  const [params, setParams] = useSearchParams();
  const [onlyFav, setOnlyFav] = useState(false);
  const [search, setSearch] = useState('');
  const [collapsed, setCollapsed] = useState<ReadonlySet<string>>(new Set());
  const [changed, setChanged] = useState<ReadonlySet<string>>(new Set());
  const [selectedValue, setSelectedValue] = useState<DeviceValue>();
  const [loadedId, setLoadedId] = useState<number>();
  const detailRef = useRef<HTMLElement>(null);
  const lastValues = useRef<{ id?: number; values: Map<string, unknown> }>({
    values: new Map()
  });

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
  }).onSuccess(({ args: [id], data }) => {
    const values = new Map(data.nodes.map((dv) => [dv.id.slice(2), dv.v]));
    const last = lastValues.current;
    setChanged(
      new Set(
        last.id === id
          ? [...values.keys()].filter(
              (k) => last.values.has(k) && last.values.get(k) !== values.get(k)
            )
          : []
      )
    );
    lastValues.current = { id, values };
    setLoadedId(id);
  });

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
    setCollapsed(new Set());
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

  const q = search.trim().toLowerCase();
  const shown: Row[] = sort.rows.flatMap((dv) => {
    if (onlyFav && !hasMask(dv.id, DeviceEntityMask.DV_FAVORITE)) return [];
    const name = dv.id.slice(2);
    const value = formatValue(LL, dv.v, dv.u);
    if (q && !name.toLowerCase().includes(q) && !value.toLowerCase().includes(q)) {
      return [];
    }
    const m = TAGGED.exec(name);
    return [{ dv, name, tag: m?.[1] ?? '', short: m?.[2] ?? name, value }];
  });

  // untagged entities first, then each circuit in the order the device lists them
  const groups = new Map<string, Row[]>([['', []]]);
  for (const row of shown) {
    const key = row.tag.toLowerCase();
    const group = groups.get(key);
    if (group) group.push(row);
    else groups.set(key, [row]);
  }
  if (!groups.get('')?.length) groups.delete('');
  const grouped = groups.size > 1;

  const canWrite = (dv: DeviceValue) =>
    me.admin && dv.c !== undefined && !hasMask(dv.id, DeviceEntityMask.DV_READONLY);

  const customize = () => {
    if (selectedId === CUSTOM_DEVICE_ID) {
      void navigate('/customentities');
    } else {
      void navigate('/customizations', { state: selectedId });
    }
  };

  const toggleGroup = (key: string) => {
    const next = new Set(collapsed);
    if (!next.delete(key)) next.add(key);
    setCollapsed(next);
  };

  const filename = (d: Device, ext: string) => `${d.tn}_${d.n}.${ext}`;

  const exportCsv = () => {
    if (!device) return;
    const columns: [string, (dv: DeviceValue) => unknown][] = [
      [LL.ENTITY_NAME(0), (dv) => dv.id.slice(2)],
      [
        LL.VALUE(),
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
      shown.map(({ dv }) => line(columns.map(([, get]) => get(dv)))).join('');
    saveBlob(
      new Blob([csv], { type: 'text/csv;charset=utf-8' }),
      filename(device, 'csv')
    );
  };

  const exportJson = () => {
    if (!device) return;
    saveBlob(
      new Blob([JSON.stringify({ device, ...deviceData }, null, 2)], {
        type: 'application/json'
      }),
      filename(device, 'json')
    );
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
          `${LL.DEVICE_ID()} ${hex(d.d)}`,
          `${LL.PRODUCT_ID()} ${d.p}`,
          d.v && `${LL.VERSION()} ${d.v}`
        ]
          .filter(Boolean)
          .join(' · ');

  const renderDeviceList = () => (
    <Section className="dv-list">
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

  const renderRow = ({ dv, name, short, value }: Row) => (
    <tr key={dv.id} {...rowAction(() => setSelectedValue(dv), name)}>
      <td>
        <span className="dv-ename">
          <span>{highlight(grouped ? short : name, q)}</span>
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
      <td className="r dv-val">
        <span key={text(dv.v)} className={cx(changed.has(name) && 'dv-chg')}>
          {highlight(value, q)}
        </span>
      </td>
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
  );

  const renderGroup = (key: string, rows: Row[]) => {
    const open = !!q || !collapsed.has(key);
    return [
      <tr key={`g-${key}`} className="dv-grp">
        <td colSpan={me.admin ? 3 : 2}>
          <button
            type="button"
            aria-expanded={open}
            disabled={!!q}
            onClick={() => toggleGroup(key)}
          >
            <Icon name="chevron_right" size="sm" className="dv-chev" />
            {rows[0]?.tag || LL.GENERAL()}
            <span className="count">{rows.length}</span>
          </button>
        </td>
      </tr>,
      ...(open ? rows.map(renderRow) : [])
    ];
  };

  const renderDevice = (d: Device) => {
    const masksShown = SHOWN_MASKS.filter((o) =>
      shown.some(({ dv }) => hasMask(dv.id, o.bit))
    );
    const exportItems: MenuItem[] = [
      { label: 'CSV', icon: 'list_alt', onSelect: exportCsv },
      { label: 'JSON', icon: 'data_object', onSelect: exportJson }
    ];
    const moreItems: MenuItem[] = [
      ...exportItems.map((o) => ({ ...o, label: `${LL.EXPORT()} ${o.label}` })),
      ...(me.admin
        ? [
            {
              label: LL.CUSTOMIZATIONS(),
              icon: 'construction' as const,
              onSelect: customize
            }
          ]
        : [])
    ];
    return (
      <section className="section dv-detail" ref={detailRef} aria-label={d.n}>
        <div className="dv-head">
          <Button
            className="dv-back"
            color="neutral"
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
            <Menu
              className="dv-wide"
              icon="download"
              label={LL.EXPORT()}
              items={exportItems}
              disabled={!nodes.length}
            />
            {me.admin && (
              <Button
                className="dv-wide"
                color="secondary"
                icon="construction"
                onClick={customize}
              >
                {LL.CUSTOMIZATIONS()}
              </Button>
            )}
            <Menu
              className="dv-narrow"
              iconOnly
              icon="more_vert"
              label={LL.MORE()}
              items={moreItems}
            />
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
        </div>

        {loadedId !== selectedId ? (
          <Loader
            error={deviceError?.message}
            onRetry={() => void fetchDeviceData(d.id)}
          />
        ) : (
          <Table className="dv-values">
            <thead>
              <tr>
                <SortHeader {...sort.header('name')}>{LL.ENTITY_NAME(0)}</SortHeader>
                <SortHeader className="r" {...sort.header('value')}>
                  {LL.VALUE()}
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
              {grouped
                ? [...groups].flatMap(([key, rows]) => renderGroup(key, rows))
                : shown.map(renderRow)}
            </tbody>
          </Table>
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
