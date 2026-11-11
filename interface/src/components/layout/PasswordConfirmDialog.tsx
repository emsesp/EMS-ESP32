import { useEffect, useRef, useState } from 'react';

import LockIcon from '@mui/icons-material/Lock';
import {
  Button,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  Typography
} from '@mui/material';

import { setPasswordPrompt } from 'api/passwordPrompt';
import { confirmPassword } from 'api/security';

import { dialogStyle } from 'CustomTheme';
import { useI18nContext } from 'i18n/i18n-react';

import { ValidatedPasswordField } from '../inputs';

// asks for the password again before changes to the firmware, the users or the security settings
const PasswordConfirmDialog = () => {
  const { LL } = useI18nContext();
  const [open, setOpen] = useState(false);
  const [password, setPassword] = useState('');
  const [error, setError] = useState<string>();
  const [checking, setChecking] = useState(false);
  const resolver = useRef<(confirmed: boolean) => void>(undefined);

  useEffect(() => {
    setPasswordPrompt(
      () =>
        new Promise<boolean>((resolve) => {
          resolver.current = resolve;
          setPassword('');
          setError(undefined);
          setOpen(true);
        }),
      LL.PASSWORD_NOT_CONFIRMED()
    );
    return () => setPasswordPrompt(undefined);
  }, [LL]);

  const close = (confirmed: boolean) => {
    setOpen(false);
    setPassword('');
    resolver.current?.(confirmed);
    resolver.current = undefined;
  };

  const onConfirm = async () => {
    setChecking(true);
    setError(undefined);
    try {
      await confirmPassword(password);
      close(true);
    } catch (e) {
      setError(
        (e as Error).message === 'Too Many Requests'
          ? LL.TOO_MANY_LOGINS()
          : LL.PASSWORD_WRONG()
      );
    } finally {
      setChecking(false);
    }
  };

  return (
    <Dialog
      sx={dialogStyle}
      open={open}
      onClose={() => close(false)}
      fullWidth
      maxWidth="xs"
    >
      <DialogTitle>{LL.PASSWORD_CONFIRM_TITLE()}</DialogTitle>
      <DialogContent dividers>
        <Typography variant="body2">{LL.PASSWORD_CONFIRM_TEXT()}</Typography>
        <form
          onSubmit={(e) => {
            e.preventDefault();
            void onConfirm();
          }}
        >
          <ValidatedPasswordField
            fieldErrors={error ? { password: [{ message: error }] } : {}}
            name="password"
            label={LL.PASSWORD()}
            autoComplete="current-password"
            autoFocus
            fullWidth
            variant="outlined"
            value={password}
            onChange={(e) => setPassword(e.target.value)}
            margin="normal"
          />
        </form>
      </DialogContent>
      <DialogActions>
        <Button variant="outlined" color="secondary" onClick={() => close(false)}>
          {LL.CANCEL()}
        </Button>
        <Button
          startIcon={<LockIcon />}
          variant="outlined"
          color="primary"
          disabled={checking || password.length === 0}
          onClick={() => void onConfirm()}
        >
          {LL.PASSWORD_CONFIRM_BUTTON()}
        </Button>
      </DialogActions>
    </Dialog>
  );
};

export default PasswordConfirmDialog;
