import { useEffect, useState } from 'react';

import { useRequest } from 'alova/client';
import {
  Button,
  Checkbox,
  Dialog,
  Loader,
  Message,
  TextField,
  toast
} from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import { saveBlob } from 'utils';

import {
  type HeatingBackup,
  hex,
  readHeatingBackup,
  timestamp
} from './heatingBackup';

const HeatingBackupDialog = ({
  open,
  onClose
}: {
  open: boolean;
  onClose: () => void;
}) => {
  const { LL } = useI18nContext();
  const [selected, setSelected] = useState<number[]>([]);
  const [fileName, setFileName] = useState('');
  const [note, setNote] = useState('');

  const { data, loading, error, send } = useRequest(readHeatingBackup, {
    immediate: false
  });

  useEffect(() => {
    if (!open) return;
    setFileName('heating_' + timestamp().slice(0, 10));
    setNote('');
    void send().then((backup) => setSelected(backup.devices.map((_, i) => i)));
  }, [open]);

  const download = () => {
    const backup: HeatingBackup = {
      type: 'heatingbackup',
      version: data.version,
      created: timestamp(),
      ...(note.trim() && { note: note.trim() }),
      fahrenheit: data.fahrenheit,
      devices: data.devices.filter((_, i) => selected.includes(i))
    };
    saveBlob(
      new Blob([JSON.stringify(backup, null, 2)], { type: 'application/json' }),
      (fileName.trim() || 'heating') + '.json'
    );
    toast.success(LL.DOWNLOAD_SUCCESSFUL());
    onClose();
  };

  const toggle = (i: number, on: boolean) =>
    setSelected((s) => (on ? [...s, i] : s.filter((x) => x !== i)));

  return (
    <Dialog
      open={open}
      title={LL.HB_BACKUP()}
      onClose={onClose}
      onSubmit={download}
      size="md"
      actions={
        <>
          <Button color="secondary" icon="cancel" onClick={onClose}>
            {LL.CANCEL()}
          </Button>
          <Button
            type="submit"
            icon="download"
            disabled={loading || !data || selected.length === 0}
          >
            {LL.DOWNLOAD()}
          </Button>
        </>
      }
    >
      {loading || !data ? (
        <Loader error={error?.message} onRetry={() => void send()} />
      ) : (
        <>
          <p className="cap du-label">{LL.DEVICES()}</p>
          {data.devices.length === 0 && <p className="muted">{LL.HB_NOTHING()}</p>}
          {data.devices.map((d, i) => (
            <Checkbox
              key={i}
              className="du-dev"
              checked={selected.includes(i)}
              onChange={(on) => toggle(i, on)}
              label={
                <>
                  <span className="grow du-dev-name">
                    <span>
                      {d.name} <span className="muted">{d.type}</span>
                    </span>
                    <span className="cap">
                      {LL.DEVICE_ID()} {hex(d.deviceId)} ·{' '}
                      {LL.PRODUCT_ID()} {d.productId}
                    </span>
                  </span>
                  <span className="cap">
                    {LL.NUM_VALUES({ num: Object.keys(d.values).length })}
                  </span>
                </>
              }
            />
          ))}
          <div className="form">
            <TextField
              label={LL.FILE_NAME()}
              name="filename"
              unit=".json"
              value={fileName}
              onChange={(e) => setFileName(e.currentTarget.value)}
            />
            <TextField
              label={LL.HB_NOTE()}
              name="note"
              value={note}
              maxLength={200}
              onChange={(e) => setNote(e.currentTarget.value)}
            />
          </div>
          <Message level="info" className="du-msg">
            {LL.HB_INCOMPLETE()}
          </Message>
        </>
      )}
    </Dialog>
  );
};

export default HeatingBackupDialog;
