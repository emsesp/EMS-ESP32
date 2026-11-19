import { type ChangeEvent, useState } from 'react';

import { callAction } from '@/api/app';
import { useRequest } from 'alova/client';
import {
  Button,
  Dialog,
  Segmented,
  Select,
  TextField,
  focusInvalid,
  toast
} from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import { extractEventValue, numberValue } from 'utils';
import { ValidationError, validate } from 'validators';
import type Schema from 'validators/schema';
import type { ValidateFieldsError } from 'validators/schema';

import './Devices.css';
import { formatValue } from './deviceValue';
import { DeviceValueUOM, DeviceValueUOM_s, getDeviceValueKind } from './types';
import type { DeviceValue } from './types';

// mount only while it is open
interface DevicesDialogProps {
  onClose: () => void;
  onSave: (as: DeviceValue) => void;
  selectedItem: DeviceValue;
  writeable: boolean;
  validator: Schema;
  progress: boolean;
}

const isNumberField = (dv: DeviceValue) =>
  !dv.l && (!!dv.s || dv.u !== DeviceValueUOM.NONE);

// short lists (e.g. off/on) are buttons, longer ones a select
const isShortList = (l: string[]) => l.length <= 3 && l.every((o) => o.length <= 12);

// the firmware sends the step as a number although the type says string
const decimals = (step?: string | number) => {
  const d = String(step ?? '').split('.')[1];
  return d ? d.length : 0;
};

const initialValue = (dv: DeviceValue): DeviceValue => {
  if (typeof dv.v !== 'number' || !isNumberField(dv)) return dv;
  const f = 10 ** Math.max(1, decimals(dv.s));
  return { ...dv, v: Math.round(dv.v * f) / f };
};

// a value typed into a number field can still be text like "1." or NaN when empty
const toSend = (dv: DeviceValue): DeviceValue => {
  if (!isNumberField(dv)) return dv;
  if (typeof dv.v === 'number') return { ...dv, v: isNaN(dv.v) ? '' : dv.v };
  if (typeof dv.v === 'string' && dv.v !== '' && !isNaN(Number(dv.v))) {
    return { ...dv, v: Number(dv.v) };
  }
  return dv;
};

const DevicesDialog = ({
  onClose,
  onSave,
  selectedItem,
  writeable,
  validator,
  progress
}: DevicesDialogProps) => {
  const { LL } = useI18nContext();
  const [editItem, setEditItem] = useState<DeviceValue>(() =>
    initialValue(selectedItem)
  );
  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();

  const { send: executeCommand } = useRequest(
    (id: string) => callAction({ action: 'executeCommand', param: id }),
    { immediate: false }
  )
    .onSuccess(() => {
      toast.success(LL.COMMAND_EXECUTED());
    })
    .onError((error) => {
      toast.error(String(error.error?.message || LL.ERROR()));
    });

  const setValue = (v: unknown) => {
    setEditItem((dv) => ({ ...dv, v }));
    setFieldErrors(undefined);
  };

  const doAction = async () => {
    const item = toSend(editItem);
    try {
      setFieldErrors(undefined);
      if (item.v === undefined && item.c !== undefined) {
        await executeCommand(item.c);
      } else {
        await validate(validator, item);
      }
      onSave(item);
    } catch (error) {
      if (error instanceof ValidationError) {
        setFieldErrors(error.fieldErrors);
        focusInvalid();
      }
    }
  };

  const unitText = (uom?: number) => {
    switch (uom) {
      case undefined:
        return undefined;
      case DeviceValueUOM.HOURS:
        return LL.HOURS();
      case DeviceValueUOM.MINUTES:
        return LL.MINUTES();
      case DeviceValueUOM.SECONDS:
        return LL.SECONDS();
      default:
        return DeviceValueUOM_s[uom as DeviceValueUOM] || undefined;
    }
  };

  const kind = getDeviceValueKind(selectedItem);
  const isRun =
    kind === 'command' || (kind === 'deviceCommand' && selectedItem.v === '');

  const title = isRun
    ? LL.RUN_COMMAND()
    : kind === 'scheduler'
      ? LL.EDIT_SCHEDULE()
      : writeable
        ? LL.CHANGE_VALUE()
        : LL.VALUE(0);

  const dv = editItem;
  const error = fieldErrors?.v?.[0]?.message;
  const label = LL.VALUE(0);
  const range =
    dv.m !== undefined && dv.x !== undefined ? `${dv.m} → ${dv.x}` : undefined;
  const value = typeof dv.v === 'string' || typeof dv.v === 'number' ? dv.v : '';

  const renderField = () => {
    if (kind === 'command') return null;
    if (!writeable) {
      return (
        <p className="dvd-value full">
          {formatValue(LL, selectedItem.v, selectedItem.u)}
        </p>
      );
    }
    if (dv.l) {
      const options = dv.l.map((o) => ({ value: o, label: o }));
      if (!dv.l.includes(String(value))) {
        options.unshift({ value: String(value), label: String(value) });
      }
      return isShortList(dv.l) ? (
        <Segmented
          label={label}
          options={options}
          value={String(value)}
          onChange={setValue}
          helperText={error ?? dv.h}
          fullWidth
        />
      ) : (
        <Select
          label={label}
          name="v"
          options={options}
          value={String(value)}
          onChange={(e) => setValue(e.currentTarget.value)}
          error={error}
          helperText={dv.h}
          fullWidth
        />
      );
    }
    if (isNumberField(dv)) {
      return (
        <TextField
          label={label}
          name="v"
          type="number"
          autoFocus
          value={typeof value === 'number' ? numberValue(value) : value}
          onChange={(e: ChangeEvent<HTMLInputElement>) =>
            setValue(extractEventValue(e))
          }
          min={dv.s ? dv.m : undefined}
          max={dv.s ? dv.x : undefined}
          step={dv.s ?? 'any'}
          unit={unitText(dv.u)}
          error={error}
          helperText={dv.h ?? range}
          fullWidth
        />
      );
    }
    return (
      <TextField
        label={label}
        name="v"
        autoFocus
        value={String(value)}
        onChange={(e: ChangeEvent<HTMLInputElement>) =>
          setValue(e.currentTarget.value)
        }
        error={error}
        helperText={dv.h}
        fullWidth
      />
    );
  };

  return (
    <Dialog
      open
      title={title}
      onClose={onClose}
      onSubmit={
        writeable
          ? () => {
              if (!progress) void doAction();
            }
          : undefined
      }
      actions={
        writeable ? (
          <>
            <Button color="secondary" icon="cancel" onClick={onClose}>
              {LL.CANCEL()}
            </Button>
            <Button
              type="submit"
              icon={isRun ? 'play_arrow' : 'check'}
              disabled={progress}
            >
              {isRun ? LL.RUN() : LL.UPDATE()}
            </Button>
          </>
        ) : (
          <Button icon="close" color="secondary" onClick={onClose}>
            {LL.CLOSE()}
          </Button>
        )
      }
    >
      <div className="form">
        <p className="dvd-name full">
          <span>{dv.id.slice(2)}</span>
          {dv.c && <span className="mono muted">{dv.c}</span>}
        </p>
        {renderField()}
      </div>
    </Dialog>
  );
};

export default DevicesDialog;
