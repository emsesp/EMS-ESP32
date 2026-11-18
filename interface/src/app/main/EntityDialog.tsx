import { type ReactNode, useEffect, useState } from 'react';

import { Button, Dialog } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import { formFields } from 'utils';
import { ValidationError, validate } from 'validators';
import type Schema from 'validators/schema';
import type { ValidateFieldsError } from 'validators/schema';

export interface EntityForm<T extends object> extends ReturnType<typeof formFields<T>> {
  values: T;
  set: (patch: Partial<T>) => void;
  errors: ValidateFieldsError | undefined;
}

interface EntityDialogProps<T extends object> {
  open: boolean;
  title: string;
  // the values the dialog starts with, copied when it opens
  item: T;
  creating?: boolean;
  validator?: ((item: T) => Schema) | undefined;
  onClose: () => void;
  onSave: (item: T) => void;
  // shows a Remove button on the left
  onRemove?: (() => void) | undefined;
  // more buttons on the left, e.g. Run
  extraActions?: (values: T) => ReactNode;
  size?: 'sm' | 'md';
  children: (form: EntityForm<T>) => ReactNode;
}

// add/edit dialog for one row of a list page; the change is kept until the page is saved
const EntityDialog = <T extends object>({
  open,
  title,
  item,
  creating,
  validator,
  onClose,
  onSave,
  onRemove,
  extraActions,
  size = 'sm',
  children
}: EntityDialogProps<T>) => {
  const { LL } = useI18nContext();
  const [values, setValues] = useState<T>(item);
  const [errors, setErrors] = useState<ValidateFieldsError>();

  useEffect(() => {
    if (open) {
      setValues(item);
      setErrors(undefined);
    }
  }, [open, item]);

  const set = (patch: Partial<T>) => {
    setValues((v) => ({ ...v, ...patch }));
    setErrors((errs) => {
      if (!errs) return errs;
      const left = { ...errs };
      for (const k of Object.keys(patch)) delete left[k];
      return left;
    });
  };

  const fields = formFields(values, set, (name) => ({
    error: errors?.[name]?.[0]?.message
  }));

  const save = async () => {
    try {
      if (validator) await validate(validator(values), values);
      onSave(values);
    } catch (error) {
      if (error instanceof ValidationError) {
        setErrors(error.fieldErrors);
        setTimeout(() =>
          document
            .querySelector<HTMLElement>('.dialog [aria-invalid="true"]')
            ?.focus()
        );
      } else throw error;
    }
  };

  return (
    <Dialog
      open={open}
      title={title}
      size={size}
      onClose={onClose}
      actions={
        <>
          {onRemove && (
            <Button color="error" icon="delete" onClick={onRemove}>
              {LL.REMOVE()}
            </Button>
          )}
          {extraActions?.(values)}
          <span className="grow" />
          <Button color="secondary" icon="cancel" onClick={onClose}>
            {LL.CANCEL()}
          </Button>
          <Button variant="contained" icon={creating ? 'add' : 'check'} onClick={() => void save()}>
            {creating ? LL.ADD(0) : LL.UPDATE()}
          </Button>
        </>
      }
    >
      <form
        className="form"
        onSubmit={(e) => {
          e.preventDefault();
          void save();
        }}
      >
        {children({ ...fields, values, set, errors })}
      </form>
    </Dialog>
  );
};

export default EntityDialog;
