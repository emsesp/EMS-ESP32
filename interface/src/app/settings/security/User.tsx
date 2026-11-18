import { memo, useState } from 'react';
import type { FC, FormEvent } from 'react';

import { Button, Dialog, Switch, TextField } from 'components/ui';
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
  onClose: () => void;
}

// add or edit one user; the change is kept in the page list until Save
const User: FC<UserProps> = ({ user, creating, validator, onDone, onClose }) => {
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

  const submit = async (e?: FormEvent) => {
    e?.preventDefault();
    try {
      await validate(validator, value);
      onDone(value);
    } catch (error) {
      setFieldErrors((error as ValidationError).fieldErrors);
    }
  };

  const err = (name: keyof UserType) => fieldErrors?.[name]?.[0]?.message;

  return (
    <Dialog
      open
      title={creating ? LL.ADD_USER() : LL.EDIT_USER()}
      onClose={onClose}
      actions={
        <>
          <Button color="secondary" onClick={onClose}>
            {LL.CANCEL()}
          </Button>
          <Button
            variant="contained"
            icon={creating ? 'person_add' : 'check'}
            onClick={() => void submit()}
          >
            {creating ? LL.ADD(0) : LL.UPDATE()}
          </Button>
        </>
      }
    >
      <form className="form" onSubmit={(e) => void submit(e)} noValidate>
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
        <button type="submit" hidden />
      </form>
    </Dialog>
  );
};

export default memo(User);
