import { useMemo } from 'react';

import { Button, Icon, KeyValueList, TextField } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import Schema from 'validators/schema';
import type { InternalRuleItem } from 'validators/schema';

import EntityDialog from './EntityDialog';
import EntityMaskToggle from './EntityMaskToggle';
import { DeviceEntityMask } from './types';
import type { DeviceEntity } from './types';

interface CustomizationForm {
  m: number;
  cn: string;
  // a number, or text while a number is typed or the field is empty
  mi: number | string | undefined;
  ma: number | string | undefined;
}

interface CustomizationsDialogProps {
  entity: DeviceEntity;
  // the entity as loaded, to tell whether there is anything to reset
  original: DeviceEntity | undefined;
  onClose: () => void;
  onSave: (de: DeviceEntity) => void;
  onReset: () => void;
}

const toNumber = (v: number | string | undefined) => {
  const n = typeof v === 'string' ? (v.trim() === '' ? NaN : Number(v)) : (v ?? NaN);
  return Number.isNaN(n) ? undefined : n;
};

// an empty limit is sent as null when the entity had one, so the built-in limit returns
const limit = (v: number | string | undefined, before: number | null | undefined) =>
  toNumber(v) ?? (before === undefined ? undefined : null);

const CustomizationsDialog = ({
  entity,
  original,
  onClose,
  onSave,
  onReset
}: CustomizationsDialogProps) => {
  const { LL } = useI18nContext();

  const item = useMemo<CustomizationForm>(
    () => ({
      m: entity.m,
      cn: entity.cn ?? '',
      mi: entity.mi ?? undefined,
      ma: entity.ma ?? undefined
    }),
    [entity]
  );

  const isWriteableNumber = (m: number) =>
    typeof entity.v === 'number' && entity.w && !(m & DeviceEntityMask.DV_READONLY);

  const showReset = (v: CustomizationForm) =>
    !!entity.c ||
    v.m !== 0 ||
    v.cn !== '' ||
    entity.mi !== original?.mi ||
    entity.ma !== original?.ma;

  const validator = (v: CustomizationForm) =>
    new Schema({
      ma: {
        validator(
          _rule: InternalRuleItem,
          _value: unknown,
          callback: (error?: string) => void
        ) {
          const mi = toNumber(v.mi);
          const ma = toNumber(v.ma);
          if (
            isWriteableNumber(v.m) &&
            mi !== undefined &&
            ma !== undefined &&
            mi > ma
          ) {
            callback(LL.MIN_ABOVE_MAX());
            return;
          }
          callback();
        }
      }
    });

  const save = (v: CustomizationForm) => {
    const next: DeviceEntity = { ...entity, m: v.m, cn: v.cn };
    delete next.mi;
    delete next.ma;
    const mi = isWriteableNumber(v.m) ? limit(v.mi, entity.mi) : entity.mi;
    const ma = isWriteableNumber(v.m) ? limit(v.ma, entity.ma) : entity.ma;
    if (mi !== undefined) next.mi = mi;
    if (ma !== undefined) next.ma = ma;
    onSave(next);
  };

  return (
    <EntityDialog
      title={LL.EDIT_ENTITY()}
      item={item}
      validator={validator}
      onClose={onClose}
      onSave={save}
      extraActions={(v) =>
        showReset(v) && (
          <Button color="error" icon="settings_backup_restore" onClick={onReset}>
            {LL.RESET()}
          </Button>
        )
      }
    >
      {(f) => (
        <>
          <div className="full">
            <KeyValueList
              rows={[
                {
                  label: LL.ID_OF(LL.ENTITY()),
                  value: <span className="mono">{entity.id}</span>
                },
                { label: LL.DEFAULT_NAME(), value: entity.n },
                {
                  label: LL.WRITEABLE(),
                  value: (
                    <Icon
                      name={entity.w ? 'check' : 'close'}
                      size="sm"
                      label={entity.w ? 'true' : 'false'}
                      className={entity.w ? 'cz-yes' : 'cz-no'}
                    />
                  )
                }
              ]}
            />
          </div>
          <div className="full">
            <EntityMaskToggle
              de={entity}
              mask={f.values.m}
              onChange={(m) => f.set({ m })}
            />
          </div>
          <TextField
            label={LL.NEW_NAME_OF(LL.ENTITY())}
            placeholder={entity.n}
            fullWidth
            {...f.text('cn')}
          />
          {isWriteableNumber(f.values.m) && (
            <>
              <TextField
                label={LL.MIN()}
                helperText={LL.LIMIT_HELP()}
                {...f.number('mi')}
              />
              <TextField label={LL.MAX()} {...f.number('ma')} />
            </>
          )}
        </>
      )}
    </EntityDialog>
  );
};

export default CustomizationsDialog;
