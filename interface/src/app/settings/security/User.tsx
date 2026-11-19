import { memo, useState } from 'react';
import type { FC } from 'react';

import { Button, Dialog, Switch, TextField, focusInvalid } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import type { UserType } from 'types';
import { ValidationError, validate } from 'validators';
import type Schema from 'validators/schema';
import type { ValidateFieldsError } from 'validators/schema';

interface UserProps {
  user: UserType;
  creating: boolean;
  validator: Schema;
  onDone: (user: UserType) => void;
  onRemove?: (() => void) | undefined;
  onClose: () => void;
}

// add or edit one user; the change is kept in the page list until Save
const User: FC<UserProps> = ({
  user,
  creating,
  validator,
  onDone,
  onRemove,
  onClose
}) => {
  const { LL } = useI18nContext();
  const [value, setValue] = useState<UserType>(user);
  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();

  const set = (patch: Partial<UserType>) => {
    setValue((v) => ({ ...v, ...patch }));
    setFieldErrors((errs) => {
      if (!errs) return errs;
      const left = { ...errs };
      for (const k of Object.keys(patch)) delete left[k];
      return left;
    });
  };

  const submit = async () => {
    try {
      await validate(validator, value);
      onDone(value);
    } catch (error) {
      setFieldErrors((error as ValidationError).fieldErrors);
      focusInvalid();
    }
  };

  const err = (name: keyof UserType) => fieldErrors?.[name]?.[0]?.message;

  return (
    <Dialog
      open
      title={creating ? LL.ADD_USER() : LL.EDIT_USER()}
      onClose={onClose}
      onSubmit={() => void submit()}
      actions={
        <>
          {onRemove && (
            <Button color="error" icon="delete" onClick={onRemove}>
              {LL.REMOVE()}
            </Button>
          )}
          <span className="grow" />
          <Button icon="cancel" color="secondary" onClick={onClose}>
            {LL.CANCEL()}
          </Button>
          <Button
            type="submit"
            icon={creating ? 'add' : 'check'}
            disabled={!creating && JSON.stringify(value) === JSON.stringify(user)}
          >
            {creating ? LL.ADD(0) : LL.UPDATE()}
          </Button>
        </>
      }
    >
      <div className="form">
        <TextField
          label={LL.USERNAME(1)}
          name="username"
          fullWidth
          autoComplete="off"
          value={value.username}
          disabled={!creating}
          error={err('username')}
          onChange={(e) => set({ username: e.currentTarget.value })}
        />
        <TextField
          label={creating ? LL.PASSWORD() : LL.NEW_PASSWORD()}
          name="password"
          type="password"
          fullWidth
          autoComplete="new-password"
          value={value.password}
          error={err('password')}
          helperText={creating ? undefined : LL.PASSWORD_KEEP()}
          onChange={(e) => set({ password: e.currentTarget.value })}
        />
        <Switch
          label={LL.ADMIN()}
          helperText={value.admin ? LL.ADMIN_ACCESS_TEXT() : LL.GUEST_ACCESS_TEXT()}
          checked={value.admin}
          onChange={(admin) => set({ admin })}
        />
      </div>
    </Dialog>
  );
};

export default memo(User);
