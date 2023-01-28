import { FC, useState, useEffect } from 'react';
import Schema, { ValidateFieldsError } from 'async-validator';

import CancelIcon from '@mui/icons-material/Cancel';
import PersonAddIcon from '@mui/icons-material/PersonAdd';
import SaveIcon from '@mui/icons-material/Save';

import { Button, Checkbox, Dialog, DialogActions, DialogContent, DialogTitle } from '@mui/material';

import { BlockFormControlLabel, ValidatedPasswordField, ValidatedTextField } from '../../components';
import { User } from '../../types';
import { updateValue } from '../../utils';
import { validate } from '../../validators';

import { useI18nContext } from '../../i18n/i18n-react';

interface UserFormProps {
  creating: boolean;
  validator: Schema;

  user?: User;
  setUser: React.Dispatch<React.SetStateAction<User | undefined>>;

  onDoneEditing: () => void;
  onCancelEditing: () => void;
}

const UserForm: FC<UserFormProps> = ({ creating, validator, user, setUser, onDoneEditing, onCancelEditing }) => {
  const { LL } = useI18nContext();

  const updateFormValue = updateValue(setUser);
  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();
  const open = !!user;

  useEffect(() => {
    if (open) {
      setFieldErrors(undefined);
    }
  }, [open]);

  const validateAndDone = async () => {
    if (user) {
      try {
        setFieldErrors(undefined);
        await validate(validator, user);
        onDoneEditing();
      } catch (errors: any) {
        setFieldErrors(errors);
      }
    }
  };

  return (
    <Dialog onClose={onCancelEditing} open={!!user} fullWidth maxWidth="sm">
      {user && (
        <>
          <DialogTitle id="user-form-dialog-title">
            {creating ? LL.ADD(1) : LL.MODIFY()}&nbsp;{LL.USER(1)}
          </DialogTitle>
          <DialogContent dividers>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="username"
              label={LL.USERNAME(1)}
              fullWidth
              variant="outlined"
              value={user.username}
              disabled={!creating}
              onChange={updateFormValue}
              margin="normal"
            />
            <ValidatedPasswordField
              fieldErrors={fieldErrors}
              name="password"
              label={LL.PASSWORD()}
              fullWidth
              variant="outlined"
              value={user.password}
              onChange={updateFormValue}
              margin="normal"
            />
            <BlockFormControlLabel
              control={<Checkbox name="admin" checked={user.admin} onChange={updateFormValue} />}
              label={LL.IS_ADMIN(1)}
            />
          </DialogContent>
          <DialogActions>
            <Button startIcon={<CancelIcon />} variant="outlined" onClick={onCancelEditing} color="secondary">
              {LL.CANCEL()}
            </Button>
            <Button
              startIcon={creating ? <PersonAddIcon /> : <SaveIcon />}
              variant="outlined"
              onClick={validateAndDone}
              color="primary"
              autoFocus
            >
              {creating ? LL.ADD(0) : LL.UPDATE()}
            </Button>
          </DialogActions>
        </>
      )}
    </Dialog>
  );
};

export default UserForm;
