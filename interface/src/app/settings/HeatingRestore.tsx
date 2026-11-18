import { type ChangeEvent, type DragEvent, useRef, useState } from 'react';
import { useBlocker } from 'react-router';

import { API } from 'api/app';
import { passwordCancelledMessage } from 'api/passwordPrompt';

import { useRequest } from 'alova/client';
import type { APIcall } from 'app/main/types';
import BlockNavigation from 'components/routing/BlockNavigation';
import {
  Button,
  ButtonLink,
  Checkbox,
  Dialog,
  Icon,
  Loader,
  Message,
  ProgressBar,
  SearchField,
  Section,
  SortHeader,
  Table,
  Tag,
  ToggleButton,
  ToggleGroup,
  byText,
  byValue,
  cx,
  toast,
  useTableSort
} from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';

import './DownloadUpload.css';
import {
  type BackupValue,
  type HeatingBackup,
  type HeatingBackupDevice,
  isHeatingBackup,
  readHeatingBackup
} from './heatingBackup';

type RowStatus = 'ok' | 'disconnected' | 'mismatch' | 'unit' | 'missing';

interface Row {
  key: string;
  device: HeatingBackupDevice;
  cmd: string;
  backup: BackupValue;
  now: BackupValue | undefined;
  status: RowStatus;
}

interface Result {
  state: 'pending' | 'ok' | 'err';
  message?: string;
}

const connectedDevice = (current: HeatingBackup, d: HeatingBackupDevice) =>
  current.devices.find((c) => c.type === d.type && c.deviceId === d.deviceId);

const buildRows = (backup: HeatingBackup, current: HeatingBackup): Row[] =>
  backup.devices.flatMap((d, i) => {
    const connected = connectedDevice(current, d);
    const deviceStatus: RowStatus = !connected
      ? 'disconnected'
      : connected.productId !== d.productId
        ? 'mismatch'
        : !!backup.fahrenheit !== !!current.fahrenheit
          ? 'unit'
          : 'ok';
    return Object.entries(d.values).map(([cmd, backupValue]) => {
      const now = connected?.values[cmd];
      return {
        key: `${i}/${cmd}`,
        device: d,
        cmd,
        backup: backupValue,
        now,
        status: deviceStatus === 'ok' && now === undefined ? 'missing' : deviceStatus
      };
    });
  });

const text = (v: BackupValue | undefined) => (v === undefined ? '-' : String(v));

const HeatingRestore = () => {
  const { LL } = useI18nContext();
  const input = useRef<HTMLInputElement>(null);
  const [over, setOver] = useState(false);
  const [file, setFile] = useState<{ name: string; backup: HeatingBackup }>();
  const [rows, setRows] = useState<Row[]>([]);
  const [checked, setChecked] = useState<Set<string>>(new Set());
  const [results, setResults] = useState<Record<string, Result>>({});
  const [onlyChanged, setOnlyChanged] = useState(true);
  const [query, setQuery] = useState('');
  const [confirm, setConfirm] = useState(false);
  const [running, setRunning] = useState(false);
  const [progress, setProgress] = useState(0);
  const [done, setDone] = useState<{ ok: number; failed: number }>();

  const blocker = useBlocker(running);

  const {
    data: current,
    loading,
    error,
    send: loadCurrent
  } = useRequest(readHeatingBackup, { immediate: false });
  const { send: sendAPI } = useRequest((call: APIcall) => API(call), {
    immediate: false
  });

  // the value on the device: what was just written, else what the export reported
  const nowOf = (r: Row) => (results[r.key]?.state === 'ok' ? r.backup : r.now);
  const isChanged = (r: Row) =>
    r.now === undefined || text(nowOf(r)) !== text(r.backup);
  const canSelect = (r: Row) => r.status === 'ok' && isChanged(r) && !running;

  const open = async (f: File | undefined) => {
    if (!f) return;
    let backup: unknown;
    try {
      backup = JSON.parse(await f.text());
    } catch {
      backup = undefined;
    }
    if (!isHeatingBackup(backup)) {
      toast.error(LL.HB_INVALID());
      return;
    }
    const now = await loadCurrent().catch((e: Error) => {
      toast.error(e.message);
      return undefined;
    });
    if (!now) return;
    const all = buildRows(backup, now);
    setFile({ name: f.name, backup });
    setRows(all);
    setChecked(
      new Set(
        all
          .filter((r) => r.status === 'ok' && text(r.now) !== text(r.backup))
          .map((r) => r.key)
      )
    );
    setResults({});
    setDone(undefined);
    setOnlyChanged(true);
    setQuery('');
  };

  const onChange = (e: ChangeEvent<HTMLInputElement>) => {
    void open(e.currentTarget.files?.[0]);
    e.currentTarget.value = '';
  };

  const onDrop = (e: DragEvent) => {
    e.preventDefault();
    setOver(false);
    void open(e.dataTransfer?.files[0]);
  };

  const run = async () => {
    setConfirm(false);
    setRunning(true);
    setDone(undefined);
    setProgress(0);
    const selected = rows.filter((r) => checked.has(r.key));
    let ok = 0;
    let failed = 0;
    for (const [i, r] of selected.entries()) {
      setResults((s) => ({ ...s, [r.key]: { state: 'pending' } }));
      let result: Result;
      try {
        await sendAPI({
          device: r.device.type,
          cmd: r.cmd,
          id: -1,
          data: String(r.backup)
        });
        result = { state: 'ok' };
        ok++;
      } catch (e) {
        result = { state: 'err', message: (e as Error).message };
        failed++;
      }
      setResults((s) => ({ ...s, [r.key]: result }));
      setChecked((s) => {
        const next = new Set(s);
        next.delete(r.key);
        return next;
      });
      setProgress(((i + 1) * 100) / selected.length);
      if (result.message === passwordCancelledMessage()) break;
    }
    setRunning(false);
    setDone({ ok, failed });
    setOnlyChanged(false);
  };

  const toggle = (keys: string[], on: boolean) =>
    setChecked((s) => {
      const next = new Set(s);
      keys.forEach((k) => (on ? next.add(k) : next.delete(k)));
      return next;
    });

  const sort = useTableSort(rows, {
    device: byText((r) => r.device.name),
    name: byText((r) => r.cmd),
    now: byValue((r) => r.now),
    backup: byValue((r) => r.backup)
  });

  if (!file) {
    return (
      <Section title={LL.HB_RESTORE()} icon="settings_backup_restore">
        <p className="du-text">{LL.HB_RESTORE_TEXT()}</p>
        {loading ? (
          <Loader />
        ) : (
          <>
            <button
              type="button"
              className={cx('du-drop', over && 'on')}
              onClick={() => input.current?.click()}
              onDragOver={(e) => {
                e.preventDefault();
                setOver(true);
              }}
              onDragLeave={() => setOver(false)}
              onDrop={onDrop}
            >
              <Icon name="upload_file" />
              {LL.HB_DROP()}
            </button>
            <input
              ref={input}
              type="file"
              hidden
              accept=".json"
              onChange={onChange}
            />
          </>
        )}
        {error && (
          <Message level="error" className="du-msg">
            {error.message}
          </Message>
        )}
      </Section>
    );
  }

  const { backup } = file;
  const unit = backup.fahrenheit ? '°F' : '°C';
  const changedCount = rows.filter(isChanged).length;
  const q = query.trim().toLowerCase();
  const list = sort.rows.filter(
    (r) =>
      (!onlyChanged || isChanged(r) || results[r.key]) &&
      (!q || `${r.device.name} ${r.device.type} ${r.cmd}`.toLowerCase().includes(q))
  );
  const selectable = list.filter(canSelect);
  const allSelected =
    selectable.length > 0 && selectable.every((r) => checked.has(r.key));
  const selectAll = (on: boolean) =>
    toggle(
      selectable.map((r) => r.key),
      on
    );
  const devices = [
    ...new Set(rows.filter((r) => checked.has(r.key)).map((r) => r.device.name))
  ];

  const status = (r: Row) => {
    const res = results[r.key];
    if (res?.state === 'pending')
      return <Tag icon="hourglass_top">{LL.HB_SENDING()}</Tag>;
    if (res?.state === 'ok')
      return (
        <Tag kind="ok" icon="check">
          {LL.HB_WRITTEN()}
        </Tag>
      );
    if (res?.state === 'err')
      return (
        <span className="du-res">
          <Tag kind="err" icon="block">
            {LL.FAILED(0)}
          </Tag>
          {res.message && <span className="cap">{res.message}</span>}
        </span>
      );
    if (r.status === 'disconnected')
      return (
        <Tag kind="err" icon="link_off">
          {LL.HB_NOT_CONNECTED()}
        </Tag>
      );
    if (r.status === 'mismatch')
      return <Tag kind="warn">{LL.HB_DIFFERENT_DEVICE()}</Tag>;
    if (r.status === 'unit') return <Tag kind="warn">{unit}</Tag>;
    if (r.status === 'missing') return <Tag>{LL.HB_NOT_AVAILABLE()}</Tag>;
    if (!isChanged(r)) return <span className="cap">{LL.UNCHANGED()}</span>;
    return null;
  };

  return (
    <>
      <Section>
        <div className="du-file">
          <Icon name="description" className="du-file-icon" />
          <div className="du-file-main">
            <div className="h6 du-name">{file.name}</div>
            <div className="cap">
              {[
                backup.created && `${LL.CREATED()} ${backup.created}`,
                backup.version && `EMS-ESP v${backup.version}`,
                unit
              ]
                .filter(Boolean)
                .join(' · ')}
            </div>
            {backup.note && <div className="du-note">{backup.note}</div>}
          </div>
          <Button
            size="sm"
            color="secondary"
            icon="close"
            disabled={running}
            onClick={() => setFile(undefined)}
          >
            {LL.HB_OTHER_FILE()}
          </Button>
        </div>
        <div className="row wrap du-chips">
          {backup.devices.map((d, i) => {
            const connected = current && connectedDevice(current, d);
            if (!connected)
              return (
                <Tag key={i} kind="err" icon="link_off">
                  {d.name}: {LL.HB_NOT_CONNECTED()}
                </Tag>
              );
            if (connected.productId !== d.productId)
              return (
                <Tag key={i} kind="warn" icon="warning">
                  {d.name}:{' '}
                  {LL.HB_OTHER_DEVICE({
                    backup: d.productId,
                    now: connected.productId
                  })}
                </Tag>
              );
            return (
              <Tag key={i} kind="ok" icon="check_circle">
                {d.name}: {LL.HB_SAME_DEVICE()}
              </Tag>
            );
          })}
        </div>
        {current && !!backup.fahrenheit !== !!current.fahrenheit && (
          <Message level="warning" className="du-msg">
            {LL.HB_UNIT_MISMATCH({ unit })}
          </Message>
        )}
      </Section>

      <Section>
        {done && (
          <Message level={done.failed ? 'warning' : 'success'} className="du-done">
            <div>{LL.HB_DONE({ num: done.ok })}</div>
            {done.failed > 0 && <div>{LL.HB_DONE_FAILED({ num: done.failed })}</div>}
          </Message>
        )}
        <div className="du-toolbar">
          <SearchField
            label={LL.SEARCH()}
            value={query}
            onChange={(e) => setQuery(e.currentTarget.value)}
          />
          <ToggleGroup>
            <ToggleButton on={onlyChanged} onClick={() => setOnlyChanged(true)}>
              {LL.HB_CHANGED()} ({changedCount})
            </ToggleButton>
            <ToggleButton on={!onlyChanged} onClick={() => setOnlyChanged(false)}>
              {LL.ALL()} ({rows.length})
            </ToggleButton>
          </ToggleGroup>
          <Checkbox
            className="du-selall"
            label={LL.SELECT_ALL()}
            checked={allSelected}
            disabled={selectable.length === 0}
            onChange={selectAll}
          />
        </div>
        <div className="tbl-wrap">
          <Table stack className="du-tbl">
            <thead>
              <tr>
                <th className="chk">
                  <Checkbox
                    aria-label={LL.SELECT_ALL()}
                    checked={allSelected}
                    disabled={selectable.length === 0}
                    onChange={selectAll}
                  />
                </th>
                <SortHeader {...sort.header('device')}>{LL.DEVICE()}</SortHeader>
                <SortHeader {...sort.header('name')}>{LL.ENTITY_NAME(0)}</SortHeader>
                <SortHeader className="r" {...sort.header('now')}>
                  {LL.HB_NOW()}
                </SortHeader>
                <SortHeader className="r" {...sort.header('backup')}>
                  {LL.HB_BACKUP_VALUE()}
                </SortHeader>
                <th className="r" />
              </tr>
            </thead>
            <tbody>
              {list.map((r) => (
                <tr key={r.key} className={cx(r.status !== 'ok' && 'dim')}>
                  <td className="chk">
                    <Checkbox
                      aria-label={`${r.device.name} ${r.cmd}`}
                      checked={checked.has(r.key)}
                      disabled={!canSelect(r)}
                      onChange={(on) => toggle([r.key], on)}
                    />
                  </td>
                  <td data-label={LL.DEVICE()}>{r.device.name}</td>
                  <td data-label={LL.ENTITY_NAME(0)}>
                    <span className="mono">{r.cmd}</span>
                  </td>
                  <td data-label={LL.HB_NOW()} className="r num">
                    {text(nowOf(r))}
                  </td>
                  <td
                    data-label={LL.HB_BACKUP_VALUE()}
                    className={cx(
                      'r num',
                      r.status === 'ok' && isChanged(r) && 'du-changed'
                    )}
                  >
                    {text(r.backup)}
                  </td>
                  <td className="r du-status">{status(r)}</td>
                </tr>
              ))}
            </tbody>
          </Table>
          {list.length === 0 && (
            <div className="tbl-empty">{LL.HB_NO_CHANGES()}</div>
          )}
        </div>
        {running && (
          <ProgressBar
            value={progress}
            label={LL.RESTORE()}
            className="du-progress"
          />
        )}
        <div className="du-footer">
          <ButtonLink to="/settings/downloadUpload" color="secondary" icon="cancel">
            {LL.CANCEL()}
          </ButtonLink>
          <Button
            variant="contained"
            icon="settings_backup_restore"
            disabled={checked.size === 0 || running}
            onClick={() => setConfirm(true)}
          >
            {LL.HB_RESTORE_NUM({ num: checked.size })}
          </Button>
        </div>
      </Section>

      <Dialog
        open={confirm}
        title={LL.HB_RESTORE()}
        onClose={() => setConfirm(false)}
        actions={
          <>
            <Button
              color="secondary"
              icon="cancel"
              onClick={() => setConfirm(false)}
            >
              {LL.CANCEL()}
            </Button>
            <Button
              variant="contained"
              icon="settings_backup_restore"
              onClick={() => void run()}
            >
              {LL.RESTORE()}
            </Button>
          </>
        }
      >
        <p>{LL.HB_CONFIRM({ num: checked.size, file: file.name })}</p>
        <p className="cap">{devices.join(', ')}</p>
        {backup.note && <div className="du-note">{backup.note}</div>}
        <p className="cap du-confirm-text">{LL.HB_CONFIRM_TEXT()}</p>
      </Dialog>

      <BlockNavigation blocker={blocker} />
    </>
  );
};

export default HeatingRestore;
