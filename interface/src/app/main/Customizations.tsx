import { useEffect, useState } from 'react';
import { useBlocker, useLocation } from 'react-router';

import { useRequest } from 'alova/client';
import SystemMonitor from 'app/status/SystemMonitor';
import BlockNavigation from 'components/routing/BlockNavigation';
import {
  Button,
  Checkbox,
  Dialog,
  Icon,
  Loader,
  SaveBar,
  SearchField,
  Section,
  Select,
  SortHeader,
  Table,
  TextField,
  byNumber,
  byText,
  byValue,
  cx,
  toast,
  useTableSort
} from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';

import {
  API,
  readCoreData,
  readDeviceEntities,
  resetCustomizations,
  writeCustomizationEntities,
  writeDeviceName
} from '../../api/app';
import './Customizations.css';
import CustomizationsDialog from './CustomizationsDialog';
import EntityMaskToggle, { MASKS, maskAllowed, withMask } from './EntityMaskToggle';
import { PendingTag } from './ListPage';
import { DeviceEntityMask } from './types';
import type { APIcall, DeviceEntity } from './types';

const APIURL = `${window.location.origin}/api/`;

// the firmware reads the list into a 2048 byte buffer
const MAX_BUFFER_SIZE = 2000;

const createMaskedEntityId = (de: DeviceEntity): string => {
  const maskHex = de.m.toString(16).padStart(2, '0');
  const hasCustomizations = !!(de.cn || de.mi || de.ma);
  const customizations = [
    de.cn || '',
    de.mi ? `>${de.mi}` : '',
    de.ma ? `<${de.ma}` : ''
  ]
    .filter(Boolean)
    .join('');

  return `${maskHex}${de.id}${hasCustomizations ? `|${customizations}` : ''}`;
};

const hasChanged = (de: DeviceEntity, o: DeviceEntity | undefined) =>
  !o ||
  (de.cn || '') !== (o.cn || '') ||
  de.m !== o.m ||
  de.mi !== o.mi ||
  de.ma !== o.ma;

const resetEntity = (de: DeviceEntity): DeviceEntity => {
  const next: DeviceEntity = { ...de, m: 0, cn: '', c: false };
  if (de.mi !== undefined) next.mi = null;
  if (de.ma !== undefined) next.ma = null;
  return next;
};

const isCommand = (de: DeviceEntity) => !!de.n && de.n[0] === '!';

const formatValue = (value: unknown) => {
  if (typeof value === 'number') return new Intl.NumberFormat().format(value);
  if (value === undefined || value === null) return '';
  if (typeof value === 'boolean') return value ? 'true' : 'false';
  return value as string;
};

const Customizations = () => {
  const { LL } = useI18nContext();
  const location = useLocation();

  const [selectedDevice, setSelectedDevice] = useState<number>(
    Number(location.state) || -1
  );
  const [entities, setEntities] = useState<DeviceEntity[]>([]);
  const [original, setOriginal] = useState<Map<string, DeviceEntity>>(new Map());
  const [maskFilter, setMaskFilter] = useState(0);
  const [modifiedOnly, setModifiedOnly] = useState(false);
  const [search, setSearch] = useState('');
  const [selectedIds, setSelectedIds] = useState<Set<string>>(new Set());
  const [editing, setEditing] = useState<DeviceEntity>();
  const [renaming, setRenaming] = useState<{ name: string; brand: string }>();
  const [confirmReset, setConfirmReset] = useState(false);
  const [restartNeeded, setRestartNeeded] = useState(false);
  const [restarting, setRestarting] = useState(false);

  const {
    data: coreData,
    error: coreError,
    send: fetchCoreData
  } = useRequest(readCoreData);
  const { send: sendAPI, loading: restartSending } = useRequest(
    (call: APIcall) => API(call),
    { immediate: false }
  );
  const { send: sendResetCustomizations } = useRequest(resetCustomizations(), {
    immediate: false
  });
  const { send: sendDeviceName, loading: renameSending } = useRequest(
    (data: { id: number; name: string; brand: string }) => writeDeviceName(data),
    { immediate: false }
  );
  const { send: sendCustomizationEntities, loading: saving } = useRequest(
    (data: { id: number; entity_ids: string[] }) => writeCustomizationEntities(data),
    { immediate: false }
  );

  const setLoaded = (data: DeviceEntity[]) => {
    setEntities(data);
    setOriginal(new Map(data.map((de) => [de.id, de])));
  };

  const {
    loading: entitiesLoading,
    error: entitiesError,
    send: loadEntities
  } = useRequest((id: number) => readDeviceEntities(id), {
    initialData: [],
    immediate: false
  }).onSuccess((event) => setLoaded(event.data));

  useEffect(() => {
    setLoaded([]);
    setSelectedIds(new Set());
    setRestartNeeded(false);
    if (selectedDevice !== -1) void loadEntities(selectedDevice);
  }, [selectedDevice]);

  useEffect(() => {
    if (
      coreData &&
      selectedDevice !== -1 &&
      !coreData.devices.some((d) => d.id === selectedDevice)
    ) {
      setSelectedDevice(-1);
    }
  }, [coreData]);

  const changed = (de: DeviceEntity) => hasChanged(de, original.get(de.id));
  const numChanges = entities.filter(changed).length;
  const blocker = useBlocker(numChanges !== 0);
  const sort = useTableSort(entities, {
    options: byNumber((de) => de.m),
    name: byText((de) => `${de.t ?? ''} ${de.cn || de.n || ''}`.trim()),
    value: byValue((de) => de.v),
    range: byNumber((de) => de.mi ?? de.ma ?? undefined)
  });

  if (restarting) return <SystemMonitor />;
  if (!coreData) {
    return (
      <Loader error={coreError?.message} onRetry={() => void fetchCoreData()} />
    );
  }

  const devices = coreData.devices.filter((d) => d.id < 90 && d.e !== 0);
  const device = coreData.devices.find((d) => d.id === selectedDevice);

  const formatName = (de: DeviceEntity, withShortname: boolean) => {
    let name: string;
    if (isCommand(de)) {
      name = de.t
        ? `${LL.COMMAND(1)}: ${de.t} ${de.n?.slice(1)}`
        : `${LL.COMMAND(1)}: ${de.n?.slice(1)}`;
    } else if (de.cn && de.cn !== '') {
      name = de.t ? `${de.t} ${de.cn}` : de.cn;
    } else {
      name = de.t ? `${de.t} ${de.n}` : de.n || '';
    }
    return withShortname ? `${name} ${de.id}` : name;
  };

  const filterEntity = (de: DeviceEntity) =>
    (!maskFilter || de.m & maskFilter) &&
    (!modifiedOnly || !!de.c || changed(de)) &&
    formatName(de, true).toLowerCase().includes(search.toLowerCase());

  const filtered = sort.rows.filter(filterEntity);
  const allFilteredSelected =
    filtered.length > 0 && filtered.every((de) => selectedIds.has(de.id));
  const someFilteredSelected = filtered.some((de) => selectedIds.has(de.id));

  // the ticked rows, or every row the filter shows when none are ticked
  const targets =
    selectedIds.size > 0
      ? entities.filter((de) => selectedIds.has(de.id))
      : filtered;

  const update = (id: string, change: (de: DeviceEntity) => DeviceEntity) =>
    setEntities((prev) => prev.map((de) => (de.id === id ? change(de) : de)));

  const bulkTargets = (bit: DeviceEntityMask) =>
    targets.filter((de) => maskAllowed(de, de.m, bit));

  const bulkOn = (bit: DeviceEntityMask) => {
    const list = bulkTargets(bit);
    return list.length > 0 && list.every((de) => (de.m & bit) === bit);
  };

  const bulkMask = (bit: DeviceEntityMask) => {
    const ids = new Set(bulkTargets(bit).map((de) => de.id));
    const on = !bulkOn(bit);
    setEntities((prev) =>
      prev.map((de) =>
        ids.has(de.id) ? { ...de, m: withMask(de, de.m, bit, on) } : de
      )
    );
  };

  const resetSelected = () => {
    setEntities((prev) =>
      prev.map((de) => (selectedIds.has(de.id) ? resetEntity(de) : de))
    );
    setSelectedIds(new Set());
  };

  const toggleSelected = (id: string) =>
    setSelectedIds((prev) => {
      const next = new Set(prev);
      if (next.has(id)) next.delete(id);
      else next.add(id);
      return next;
    });

  const toggleSelectFiltered = () =>
    setSelectedIds((prev) => {
      const next = new Set(prev);
      filtered.forEach((de) =>
        allFilteredSelected ? next.delete(de.id) : next.add(de.id)
      );
      return next;
    });

  const cancel = () =>
    setEntities((prev) => prev.map((de) => original.get(de.id) ?? de));

  const save = async () => {
    const masked = entities.filter(changed).map(createMaskedEntityId);
    if (new TextEncoder().encode(JSON.stringify(masked)).length > MAX_BUFFER_SIZE) {
      toast.warning(LL.CUSTOMIZATIONS_FULL());
      return;
    }
    await sendCustomizationEntities({ id: selectedDevice, entity_ids: masked })
      .then(() => {
        toast.success(LL.CUSTOMIZATIONS_SAVED());
        // reload so cleared limits show the built-in ones and the saved flag matches the file
        void loadEntities(selectedDevice);
      })
      .catch((error: Error) => {
        if (error.message === 'Reboot required') {
          setOriginal(new Map(entities.map((de) => [de.id, de])));
          setRestartNeeded(true);
        } else {
          toast.error(error.message);
        }
      });
  };

  const renameDevice = async () => {
    if (!renaming) return;
    await sendDeviceName({ id: selectedDevice, ...renaming })
      .then(() => toast.success(LL.UPDATED_OF(LL.NAME(1))))
      .catch(() => toast.error(`${LL.UPDATE_OF(LL.NAME(1))} ${LL.FAILED(1)}`))
      .finally(() => {
        setRenaming(undefined);
        void fetchCoreData();
      });
  };

  const resetAll = async () => {
    try {
      await sendResetCustomizations();
      toast.info(LL.CUSTOMIZATIONS_RESTART());
    } catch (error) {
      toast.error((error as Error).message);
    } finally {
      setConfirmReset(false);
      setRestarting(true);
    }
  };

  const count = (bit: number) => entities.filter((de) => de.m & bit).length;

  const renderFilters = () => (
    <div className="cz-filters" role="group" aria-label={LL.SEARCH()}>
      <button
        type="button"
        className={cx('cz-chip', !maskFilter && !modifiedOnly && 'on')}
        aria-pressed={!maskFilter && !modifiedOnly}
        onClick={() => {
          setMaskFilter(0);
          setModifiedOnly(false);
        }}
      >
        {LL.FILTER_ALL()} {entities.length}
      </button>
      {MASKS.map((o) => (
        <button
          key={o.bit}
          type="button"
          className={cx('cz-chip', !!(maskFilter & o.bit) && 'on')}
          aria-pressed={!!(maskFilter & o.bit)}
          onClick={() => setMaskFilter((f) => f ^ o.bit)}
        >
          <Icon name={o.icon} size="xs" />
          {o.label(LL)} {count(o.bit)}
        </button>
      ))}
      <button
        type="button"
        className={cx('cz-chip', modifiedOnly && 'on')}
        aria-pressed={modifiedOnly}
        onClick={() => setModifiedOnly((v) => !v)}
      >
        <Icon name="tune" size="xs" />
        {LL.MODIFIED()}
      </button>
    </div>
  );

  const renderBulk = () => (
    <div className="cz-bulk">
      <span className="lb">
        {selectedIds.size > 0
          ? LL.NUM_SELECTED({ num: selectedIds.size })
          : LL.ALL_SHOWN({ num: filtered.length })}
      </span>
      {MASKS.map((o) => {
        const on = bulkOn(o.bit);
        return (
          <Button
            key={o.bit}
            size="sm"
            color="secondary"
            icon={o.icon}
            className={cx(`b${o.bit}`, on && 'on')}
            aria-pressed={on}
            disabled={bulkTargets(o.bit).length === 0}
            onClick={() => bulkMask(o.bit)}
          >
            {o.label(LL)}
          </Button>
        );
      })}
      {selectedIds.size > 0 && (
        <>
          <Button size="sm" color="error" icon="restart_alt" onClick={resetSelected}>
            {LL.RESET_SELECTED()}
          </Button>
          <Button
            size="sm"
            variant="text"
            icon="close"
            onClick={() => setSelectedIds(new Set())}
          >
            {LL.CLEAR_SELECTION()}
          </Button>
        </>
      )}
    </div>
  );

  const renderRow = (de: DeviceEntity) => {
    const isChanged = changed(de);
    const readonly =
      (de.m & DeviceEntityMask.DV_READONLY) === DeviceEntityMask.DV_READONLY;
    const hasLimit =
      !readonly && (typeof de.mi === 'number' || typeof de.ma === 'number');
    const editable = de.n !== undefined && !isCommand(de);
    const name = formatName(de, false);
    return (
      <tr
        key={de.id}
        className={cx(
          isChanged && 'st-mod',
          ((de.v === undefined && !isCommand(de)) ||
            !!(de.m & DeviceEntityMask.DV_DELETED)) &&
            'dim'
        )}
      >
        <td className="chk">
          <Checkbox
            aria-label={name}
            checked={selectedIds.has(de.id)}
            onChange={() => toggleSelected(de.id)}
          />
        </td>
        <td className="cz-opts" data-label={LL.OPTIONS()}>
          <EntityMaskToggle
            de={de}
            onChange={(m) => update(de.id, (prev) => ({ ...prev, m }))}
          />
        </td>
        <td className="cz-name">
          {editable ? (
            <button type="button" className="cz-edit" onClick={() => setEditing(de)}>
              {name}
            </button>
          ) : (
            <span>{name}</span>
          )}{' '}
          <a
            className="mono cz-id"
            target="_blank"
            rel="noreferrer"
            href={`${APIURL}${device?.url || ''}/${de.id}`}
          >
            {de.id}
          </a>
          {isChanged && <PendingTag state="mod" />}
        </td>
        <td className="r num" data-label={LL.VALUE(0)}>
          {formatValue(de.v)}
        </td>
        <td className="r num" data-label={`${LL.MIN()} – ${LL.MAX()}`}>
          {hasLimit && `${formatValue(de.mi)} – ${formatValue(de.ma)}`}
        </td>
      </tr>
    );
  };

  const renderEntities = () => {
    if (!entities.length && (entitiesLoading || entitiesError)) {
      return (
        <Loader
          error={entitiesError?.message}
          onRetry={() => void loadEntities(selectedDevice)}
        />
      );
    }
    return (
      <Section className="cz-entities">
        {renderFilters()}
        <div className="cz-toolbar">
          <SearchField
            label={LL.SEARCH()}
            value={search}
            onInput={(e) => setSearch(e.currentTarget.value)}
          />
          <span className="cap">
            {LL.SHOWING()} {filtered.length}/{entities.length}{' '}
            {LL.ENTITIES(entities.length)}
          </span>
        </div>
        {renderBulk()}
        <p className="cap cz-hint">{LL.BULK_HINT()}</p>
        <div className="tbl-wrap">
          <Table stack className="cz-tbl">
            <thead>
              <tr>
                <th className="chk">
                  <label className="checkbox">
                    <input
                      type="checkbox"
                      aria-label={LL.FILTER_ALL()}
                      checked={allFilteredSelected}
                      ref={(el) => {
                        if (el)
                          el.indeterminate =
                            !allFilteredSelected && someFilteredSelected;
                      }}
                      disabled={filtered.length === 0}
                      onChange={toggleSelectFiltered}
                    />
                  </label>
                </th>
                <SortHeader {...sort.header('options')}>{LL.OPTIONS()}</SortHeader>
                <SortHeader {...sort.header('name')}>{LL.NAME(1)}</SortHeader>
                <SortHeader className="r" {...sort.header('value')}>
                  {LL.VALUE(0)}
                </SortHeader>
                <SortHeader className="r" {...sort.header('range')}>
                  {LL.MIN()} – {LL.MAX()}
                </SortHeader>
              </tr>
            </thead>
            <tbody>
              {filtered.length ? (
                filtered.map(renderRow)
              ) : (
                <tr>
                  <td colSpan={5} className="tbl-empty">
                    {LL.NO_ENTITIES_MATCH()}
                  </td>
                </tr>
              )}
            </tbody>
          </Table>
        </div>
        <div className="cz-legend">
          {MASKS.map((o) => (
            <span key={o.bit}>
              <Icon name={o.icon} size="xs" className={`b${o.bit}`} />
              {o.label(LL)}: {o.help(LL)}
            </span>
          ))}
        </div>
      </Section>
    );
  };

  return (
    <>
      {blocker && <BlockNavigation blocker={blocker} />}
      <Section
        title={LL.CUSTOMIZATIONS()}
        icon="construction"
        actions={
          <Button
            color="error"
            icon="delete_sweep"
            onClick={() => setConfirmReset(true)}
          >
            {LL.REMOVE_ALL()}
          </Button>
        }
      >
        <p className="cap">{LL.CUSTOMIZATIONS_HELP_1()}.</p>
        <div className="cz-device">
          <Select
            label={LL.EMS_DEVICE()}
            value={String(selectedDevice)}
            disabled={numChanges !== 0}
            onChange={(e) => setSelectedDevice(Number(e.currentTarget.value))}
            options={[
              { value: '-1', label: `${LL.SELECT_DEVICE()}...`, disabled: true },
              ...devices.map((d) => ({
                value: String(d.id),
                label: `${d.n} (${d.tn})`
              }))
            ]}
          />
          {device && (
            <Button
              icon="edit"
              onClick={() => setRenaming({ name: device.n, brand: device.b })}
            >
              {LL.RENAME()}
            </Button>
          )}
        </div>
      </Section>

      {selectedDevice !== -1 && renderEntities()}

      {numChanges > 0 ? (
        <SaveBar message={LL.UNSAVED_CHANGES({ num: numChanges })}>
          <Button color="secondary" icon="undo" disabled={saving} onClick={cancel}>
            {LL.CANCEL()}
          </Button>
          <Button
            variant="contained"
            icon="check"
            disabled={saving}
            onClick={() => void save()}
          >
            {LL.SAVE()}
          </Button>
        </SaveBar>
      ) : (
        restartNeeded && (
          <SaveBar warning icon="restart_alt" message={LL.SAVED_RESTART()}>
            <Button
              variant="contained"
              color="warning"
              icon="restart_alt"
              disabled={restartSending}
              onClick={() =>
                void sendAPI({ device: 'system', cmd: 'restart', id: 0 })
                  .then(() => setRestarting(true))
                  .catch((error: Error) => toast.error(error.message))
              }
            >
              {LL.RESTART_NOW()}
            </Button>
          </SaveBar>
        )
      )}

      {editing && (
        <CustomizationsDialog
          open
          entity={editing}
          original={original.get(editing.id)}
          onClose={() => setEditing(undefined)}
          onSave={(de) => {
            update(de.id, () => de);
            setEditing(undefined);
          }}
          onReset={() => {
            update(editing.id, resetEntity);
            setEditing(undefined);
          }}
        />
      )}

      <Dialog
        open={!!renaming}
        title={LL.RENAME_DEVICE()}
        onClose={() => setRenaming(undefined)}
        actions={
          <>
            <Button
              color="secondary"
              icon="cancel"
              onClick={() => setRenaming(undefined)}
            >
              {LL.CANCEL()}
            </Button>
            <Button
              variant="contained"
              icon="check"
              disabled={renameSending}
              onClick={() => void renameDevice()}
            >
              {LL.RENAME()}
            </Button>
          </>
        }
      >
        {renaming && (
          <form
            className="form"
            onSubmit={(e) => {
              e.preventDefault();
              void renameDevice();
            }}
          >
            <TextField
              label={LL.EMS_DEVICE()}
              name="device"
              value={renaming.name}
              onInput={(e) =>
                setRenaming({ ...renaming, name: e.currentTarget.value })
              }
            />
            <TextField
              label={LL.BRAND()}
              name="brand"
              value={renaming.brand}
              onInput={(e) =>
                setRenaming({ ...renaming, brand: e.currentTarget.value })
              }
            />
          </form>
        )}
      </Dialog>

      <Dialog
        open={confirmReset}
        title={LL.REMOVE_ALL()}
        onClose={() => setConfirmReset(false)}
        actions={
          <>
            <Button
              color="secondary"
              icon="cancel"
              onClick={() => setConfirmReset(false)}
            >
              {LL.CANCEL()}
            </Button>
            <Button
              variant="contained"
              color="error"
              icon="delete_sweep"
              onClick={() => void resetAll()}
            >
              {LL.REMOVE_ALL()}
            </Button>
          </>
        }
      >
        <p>{LL.CUSTOMIZATIONS_RESET()}</p>
      </Dialog>
    </>
  );
};

export default Customizations;
