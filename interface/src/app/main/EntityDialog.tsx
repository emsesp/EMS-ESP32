import { type ReactNode, useState } from 'react';

import { Button, Dialog, focusInvalid } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import { formFields } from 'utils';
import { ValidationError, validate } from 'validators';
import type Schema from 'validators/schema';
import type { ValidateFieldsError } from 'validators/schema';

export interface EntityForm<T extends object> extends ReturnType<
  typeof formFields<T>
> {
  values: T;
  set: (patch: Partial<T>) => void;
  errors: ValidateFieldsError | undefined;
}

interface EntityDialogProps<T extends object> {
  title: string;
  // the values the dialog starts with; mount the dialog only while it is open
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
  const unchanged = !creating && JSON.stringify(values) === JSON.stringify(item);

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
        focusInvalid();
      } else throw error;
    }
  };

  return (
    <Dialog
      open
      title={title}
      size={size}
      onClose={onClose}
      onSubmit={() => void save()}
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
          <Button
            type="submit"
            icon={creating ? 'add' : 'check'}
            disabled={unchanged}
          >
            {creating ? LL.ADD(0) : LL.UPDATE()}
          </Button>
        </>
      }
    >
      <div className="form">{children({ ...fields, values, set, errors })}</div>
    </Dialog>
  );
};

export default EntityDialog;
