import { useState } from 'react';

import { readCustomEntities, writeCustomEntities } from 'api/app';

import {
  Button,
  Icon,
  Listbox,
  SearchField,
  Section,
  Segmented,
  SortHeader,
  Switch,
  Table,
  Tag,
  TextField,
  ToggleButton,
  ToggleGroup,
  byNumber,
  byText,
  byValue,
  cx,
  rowAction,
  useTableSort
} from 'components/ui';
import type { IconName } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import { usePendingList } from 'utils';

import './CustomEntities.css';
import EntityDialog from './EntityDialog';
import ListPage, { PendingTag, pendingClass } from './ListPage';
import { DeviceValueType, DeviceValueTypeNames, DeviceValueUOM_s } from './types';
import type { EntityItem } from './types';
import { entityItemValidation } from './validators';

const MIN_ID = -100;
const MAX_ID = 100;

const STORAGE: { ram: number; label: string; icon: IconName }[] = [
  { ram: 0, label: 'EMS', icon: 'swap_horiz' },
  { ram: 1, label: 'RAM', icon: 'memory' },
  { ram: 2, label: 'NVS', icon: 'save' }
];

const VALUE_TYPE_OPTIONS = [
  DeviceValueType.BOOL,
  DeviceValueType.INT8,
  DeviceValueType.UINT8,
  DeviceValueType.INT16,
  DeviceValueType.UINT16,
  DeviceValueType.UINT24,
  DeviceValueType.TIME,
  DeviceValueType.UINT32,
  DeviceValueType.STRING
].map((t) => ({ value: String(t), label: DeviceValueTypeNames[t] }));

const UOM_OPTIONS = DeviceValueUOM_s.map((u, i) => ({
  value: String(i),
  label: u || '-'
}));

// a random id in the range the firmware expects, not used by another row
const newId = (items: EntityItem[]) => {
  let id: number;
  do {
    id = Math.floor(Math.random() * (MAX_ID - MIN_ID) + MIN_ID);
  } while (items.some((ei) => ei.id === id));
  return id;
};

const showHex = (value: number | string, digits: number) =>
  `0x${Number(value).toString(16).toUpperCase().padStart(digits, '0')}`;

// parseInt with radix 16 also accepts a leading 0x
const parseHex = (value: number | string) =>
  typeof value === 'string' ? Number.parseInt(value, 16) : value;

const isBool = (ei: EntityItem) => ei.value_type === DeviceValueType.BOOL;

// the dialog edits device id, type id and the bool bit mask as hex text
const toForm = (ei: EntityItem): EntityItem => ({
  ...ei,
  device_id: showHex(ei.device_id, 2),
  type_id: showHex(ei.type_id, 2),
  factor: isBool(ei) ? showHex(ei.factor, 2) : ei.factor
});

const fromForm = (v: EntityItem): EntityItem => ({
  ...v,
  device_id: parseHex(v.device_id),
  type_id: parseHex(v.type_id),
  offset: Number(v.offset),
  factor: isBool(v) ? parseHex(v.factor) : Number(v.factor)
});

const formatValue = (value: unknown, uom: number) => {
  if (value === undefined) return '';
  const unit = uom === 0 ? '' : ` ${DeviceValueUOM_s[uom]}`;
  return typeof value === 'number'
    ? new Intl.NumberFormat().format(value) + unit
    : `${value as string}${unit}`;
};

interface Editing {
  // undefined while adding
  key?: string;
  originalName?: string;
  item: EntityItem;
}

const CustomEntities = () => {
  const { LL } = useI18nContext();
  const [query, setQuery] = useState('');
  const [storage, setStorage] = useState(-1);
  const [editing, setEditing] = useState<Editing>();

  const list = usePendingList<EntityItem>({
    read: readCustomEntities,
    write: (items) =>
      writeCustomEntities({
        entities: items.map((ei) => ({
          id: ei.id,
          ram: ei.ram,
          name: ei.name,
          device_id: ei.device_id,
          type_id: ei.type_id,
          offset: ei.offset,
          factor: ei.factor,
          uom: ei.uom,
          writeable: ei.writeable,
          hide: ei.hide,
          value_type: ei.value_type,
          value: ei.value
        }))
      }),
    // live values; no reload while a dialog is open
    ...(!editing && { pollMs: 3000 }),
    savedText: LL.ENTITIES_UPDATED()
  });

  const add = () =>
    setEditing({
      item: toForm({
        id: newId(list.items),
        name: '',
        ram: 0,
        device_id: 0,
        type_id: 0,
        offset: 0,
        factor: 1,
        uom: 0,
        value_type: 0,
        writeable: false,
        hide: false,
        value: ''
      })
    });

  const duplicate = (v: EntityItem) =>
    setEditing({
      item: { ...v, id: newId(list.items), name: `${v.name}_` }
    });

  const save = (v: EntityItem) => {
    const item = fromForm(v);
    if (editing?.key) list.update(editing.key, item);
    else list.add(item);
    setEditing(undefined);
  };

  const source = (ei: EntityItem) => {
    if (ei.ram === 1) return LL.CE_RAM_TEXT();
    if (ei.ram === 2) return LL.CE_NVS_TEXT();
    const factor =
      ei.factor !== 1 && !isBool(ei) && ei.value_type !== DeviceValueType.STRING
        ? ` ×${ei.factor}`
        : '';
    return `${LL.CE_EMS_SOURCE({
      device: showHex(ei.device_id, 2),
      type: showHex(ei.type_id, 3),
      offset: ei.offset
    })} · ${DeviceValueTypeNames[ei.value_type]}${factor}`;
  };

  const live = list.rows.filter((r) => r.state !== 'del');
  const q = query.trim().toLowerCase();
  const rows = list.rows
    .filter(
      ({ item }) =>
        (storage < 0 || item.ram === storage) &&
        (!q || item.name.toLowerCase().includes(q))
    )
    .sort((a, b) => a.item.name.localeCompare(b.item.name));
  const sort = useTableSort(rows, {
    name: byText((r) => r.item.name),
    storage: byNumber((r) => r.item.ram),
    source: byText((r) => source(r.item)),
    api: byNumber((r) => r.item.hide),
    value: byValue((r) => r.item.value)
  });

  return (
    <ListPage list={list}>
      <Section
        desc={`${LL.ENTITIES_HELP_1()}.`}
        actions={
          <>
            <SearchField
              label={LL.SEARCH()}
              value={query}
              onInput={(e) => setQuery(e.currentTarget.value)}
            />
            <Button icon="add" onClick={add}>
              {LL.ADD(0)}
            </Button>
          </>
        }
      >
        <div className="ce-filter">
          <ToggleGroup label={LL.CE_STORAGE()}>
            <ToggleButton on={storage < 0} onClick={() => setStorage(-1)}>
              {LL.CE_ALL()} {live.length}
            </ToggleButton>
            {STORAGE.map((s) => (
              <ToggleButton
                key={s.ram}
                on={storage === s.ram}
                icon={s.icon}
                className={`ce-s${s.ram}`}
                onClick={() => setStorage(s.ram)}
              >
                {s.label} {live.filter((r) => r.item.ram === s.ram).length}
              </ToggleButton>
            ))}
          </ToggleGroup>
        </div>
        <div className="tbl-wrap">
          <Table stack className="ce-tbl">
            <thead>
              <tr>
                <SortHeader {...sort.header('name')}>{LL.NAME(0)}</SortHeader>
                <SortHeader {...sort.header('storage')}>
                  {LL.CE_STORAGE()}
                </SortHeader>
                <SortHeader {...sort.header('source')}>{LL.CE_SOURCE()}</SortHeader>
                <SortHeader {...sort.header('api')}>API / MQTT</SortHeader>
                <SortHeader className="r" {...sort.header('value')}>
                  {LL.VALUE()}
                </SortHeader>
                <th />
              </tr>
            </thead>
            <tbody>
              {sort.rows.map(({ key, item, state }) => {
                const st = STORAGE[item.ram] ?? STORAGE[0]!;
                return (
                  <tr
                    key={key}
                    {...(state !== 'del' &&
                      rowAction(
                        () =>
                          setEditing({
                            key,
                            originalName: item.name,
                            item: toForm(item)
                          }),
                        `${LL.EDIT()} ${item.name}`
                      ))}
                    className={cx(state !== 'del' && 'click', pendingClass(state))}
                  >
                    <td>
                      {item.name}
                      {item.writeable && (
                        <Icon
                          name="edit"
                          size="xs"
                          className="lb"
                          label={LL.WRITEABLE()}
                        />
                      )}
                      <PendingTag state={state} />
                    </td>
                    <td data-label={LL.CE_STORAGE()}>
                      <Tag kind="plain" icon={st.icon} className={`ce-s${item.ram}`}>
                        {st.label}
                      </Tag>
                    </td>
                    <td
                      data-label={LL.CE_SOURCE()}
                      className={cx('muted', item.ram === 0 && 'mono')}
                    >
                      {source(item)}
                    </td>
                    <td data-label="API / MQTT">
                      {item.hide ? (
                        <Tag kind="muted" icon="visibility_off">
                          {LL.CE_EXCLUDED()}
                        </Tag>
                      ) : (
                        <span className="mono muted">custom/{item.name}</span>
                      )}
                    </td>
                    <td data-label={LL.VALUE()} className="r num ce-val">
                      {formatValue(item.value, item.uom)}
                    </td>
                    <td className="act">
                      {state === 'del' && (
                        <Button
                          size="sm"
                          color="neutral"
                          icon="undo"
                          onClick={() => list.undo(key)}
                        >
                          {LL.UNDO()}
                        </Button>
                      )}
                    </td>
                  </tr>
                );
              })}
              {rows.length === 0 && (
                <tr>
                  <td colSpan={6} className="tbl-empty">
                    {live.length ? LL.NO_ENTITIES_MATCH() : LL.CE_NONE()}
                  </td>
                </tr>
              )}
            </tbody>
          </Table>
        </div>
      </Section>

      {editing && (
        <EntityDialog
          title={editing.key ? LL.EDIT_ENTITY() : LL.ADD_ENTITY()}
          item={editing.item}
          creating={!editing.key}
          validator={() => entityItemValidation(list.items, editing.originalName)}
          onClose={() => setEditing(undefined)}
          onSave={save}
          onRemove={
            editing.key
              ? () => {
                  list.remove(editing.key!);
                  setEditing(undefined);
                }
              : undefined
          }
          extraActions={(v) =>
            editing.key && (
              <Button icon="content_copy" onClick={() => duplicate(v)}>
                {LL.DUPLICATE()}
              </Button>
            )
          }
          size="md"
        >
          {(f) => {
            const v = f.values;
            const ems = v.ram === 0;
            const numeric =
              v.value_type !== DeviceValueType.BOOL &&
              v.value_type !== DeviceValueType.STRING;
            return (
              <>
                <TextField label={LL.NAME(0)} mono fullWidth {...f.text('name')} />
                <Segmented
                  label={LL.CE_STORAGE()}
                  fullWidth
                  options={STORAGE.map((s) => ({
                    value: String(s.ram),
                    label: s.label
                  }))}
                  {...f.segmented('ram')}
                />
                {ems ? (
                  <>
                    <TextField
                      label={LL.DEVICE_ID()}
                      mono
                      placeholder="0x08"
                      {...f.text('device_id')}
                    />
                    <TextField
                      label={LL.TYPE_ID()}
                      mono
                      placeholder="0x18"
                      {...f.text('type_id')}
                    />
                    <TextField label={LL.OFFSET()} {...f.number('offset')} />
                    <Listbox
                      label={LL.VALUE_TYPE()}
                      options={VALUE_TYPE_OPTIONS}
                      {...f.select('value_type')}
                    />
                    {numeric && (
                      <>
                        <TextField
                          label={LL.FACTOR()}
                          step="0.001"
                          {...f.number('factor')}
                        />
                        <Listbox
                          label={LL.UNIT()}
                          options={UOM_OPTIONS}
                          {...f.select('uom')}
                        />
                      </>
                    )}
                    {v.value_type === DeviceValueType.STRING &&
                      parseHex(v.device_id) !== 0 && (
                        <TextField
                          label={LL.BYTES()}
                          min="1"
                          max="255"
                          step="1"
                          {...f.number('factor')}
                        />
                      )}
                    {v.value_type === DeviceValueType.BOOL && (
                      <TextField label={LL.BITMASK()} mono {...f.text('factor')} />
                    )}
                    <Switch label={LL.WRITEABLE()} {...f.check('writeable')} />
                  </>
                ) : (
                  <>
                    <TextField label={LL.DEFAULT_VALUE()} {...f.text('value')} />
                    <Listbox
                      label={LL.UNIT()}
                      options={UOM_OPTIONS}
                      {...f.select('uom')}
                    />
                  </>
                )}
                <Switch label={LL.CUSTOMIZATIONS_HELP_4()} {...f.check('hide')} />
              </>
            );
          }}
        </EntityDialog>
      )}
    </ListPage>
  );
};

export default CustomEntities;
