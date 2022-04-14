import { FC, useState, useEffect } from 'react';
import Schema, { ValidateFieldsError } from 'async-validator';

import CancelIcon from '@mui/icons-material/Cancel';
import PersonAddIcon from '@mui/icons-material/PersonAdd';

import { Button, Checkbox, Dialog, DialogActions, DialogContent, DialogTitle } from '@mui/material';

import { BlockFormControlLabel, ValidatedPasswordField, ValidatedTextField } from '../../components';
import { User } from '../../types';
import { updateValue } from '../../utils';
import { validate } from '../../validators';

interface UserFormProps {
  creating: boolean;
  validator: Schema;

  user?: User;
  setUser: React.Dispatch<React.SetStateAction<User | undefined>>;

  onDoneEditing: () => void;
  onCancelEditing: () => void;
}

const UserForm: FC<UserFormProps> = ({ creating, validator, user, setUser, onDoneEditing, onCancelEditing }) => {
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
          <DialogTitle id="user-form-dialog-title">{creating ? 'Add' : 'Modify'} User</DialogTitle>
          <DialogContent dividers>
            <ValidatedTextField
              fieldErrors={fieldErrors}
              name="username"
              label="Username"
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
              label="Password"
              fullWidth
              variant="outlined"
              value={user.password}
              onChange={updateFormValue}
              margin="normal"
            />
            <BlockFormControlLabel
              control={<Checkbox name="admin" checked={user.admin} onChange={updateFormValue} />}
              label="is Admin?"
            />
          </DialogContent>
          <DialogActions>
            <Button startIcon={<CancelIcon />} variant="outlined" onClick={onCancelEditing} color="secondary">
              Cancel
            </Button>
            <Button
              startIcon={<PersonAddIcon />}
              variant="outlined"
              onClick={validateAndDone}
              color="primary"
              autoFocus
            >
              Add
            </Button>
          </DialogActions>
        </>
      )}
    </Dialog>
  );
};

export default UserForm;
