import { useEffect, useRef, useState } from 'react';

import { setPasswordPrompt } from 'api/passwordPrompt';
import { confirmPassword } from 'api/security';

import { Button, Dialog, TextField } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';

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
      open={open}
      title={LL.PASSWORD_CONFIRM_TITLE()}
      onClose={() => close(false)}
      onSubmit={() => {
        if (password && !checking) void onConfirm();
      }}
      actions={
        <>
          <Button icon="cancel" color="neutral" onClick={() => close(false)}>
            {LL.CANCEL()}
          </Button>
          <Button
            type="submit"
            icon="lock"
            disabled={checking || password.length === 0}
          >
            {LL.PASSWORD_CONFIRM_BUTTON()}
          </Button>
        </>
      }
    >
      <p>{LL.PASSWORD_CONFIRM_TEXT()}</p>
      <div className="form">
        <TextField
          type="password"
          name="password"
          label={LL.PASSWORD()}
          autoComplete="current-password"
          autoFocus
          fullWidth
          error={error}
          value={password}
          onInput={(e) => setPassword(e.currentTarget.value)}
        />
      </div>
    </Dialog>
  );
};

export default PasswordConfirmDialog;
