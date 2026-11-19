import { useContext, useEffect, useState } from 'react';
import { useNavigate } from 'react-router';

import { readSetup, saveSetup } from 'api/security';

import { useRequest } from 'alova/client';
import {
  Button,
  Dialog,
  Message,
  TextField,
  focusInvalid,
  toast
} from 'components/ui';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import type { SetupType } from 'types';
import { ValidationError, validate } from 'validators';
import type { ValidateFieldsError } from 'validators/schema';
import { createSetupValidator } from 'validators/security';

const DISMISSED_KEY = 'apPasswordBannerDismissed';

const EMPTY_SETUP: SetupType = {
  password: '',
  ap_password: '',
  view_username: '',
  view_password: ''
};

// blocks admins until the factory passwords are replaced. A factory access point password alone only gets a warning,
// unless no WiFi network is set and the access point is the way in
const SetupDialog = () => {
  const { LL } = useI18nContext();
  const navigate = useNavigate();
  const { me, signIn, signOut } = useContext(AuthenticatedContext);
  const [setup, setSetup] = useState<SetupType>(EMPTY_SETUP);
  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();
  const [saving, setSaving] = useState(false);
  const [dismissed, setDismissed] = useState<boolean>(
    () => sessionStorage.getItem(DISMISSED_KEY) === 'true'
  );

  const { data: state, send: fetchState } = useRequest(readSetup, {
    immediate: false
  });

  useEffect(() => {
    if (me.admin) {
      void fetchState();
    }
  }, [me.admin, me.username]);

  if (!me.admin || !state) {
    return null;
  }

  const blocking = state.admin || state.ap_required;

  if (!blocking) {
    if (!state.ap || dismissed) {
      return null;
    }
    const dismiss = () => {
      sessionStorage.setItem(DISMISSED_KEY, 'true');
      setDismissed(true);
    };
    return (
      <Message
        level="warning"
        actions={
          <>
            <Button
              icon="settings"
              color="warning"
              onClick={() => void navigate('/settings/ap')}
            >
              {LL.SETTINGS_OF(LL.ACCESS_POINT(0))}
            </Button>
            <Button icon="close" color="secondary" onClick={dismiss}>
              {LL.CLOSE()}
            </Button>
          </>
        }
      >
        {LL.SETUP_AP_DEFAULT()}
      </Message>
    );
  }

  const field = (name: keyof SetupType) => ({
    name,
    fullWidth: true,
    value: setup[name],
    error: fieldErrors?.[name]?.[0]?.message,
    onInput: (e: { currentTarget: HTMLInputElement }) => {
      const { value } = e.currentTarget;
      setSetup((prev) => ({ ...prev, [name]: value }));
    }
  });

  const onSave = async () => {
    try {
      setFieldErrors(undefined);
      await validate(createSetupValidator(state, setup), setup);
    } catch (error) {
      setFieldErrors((error as ValidationError).fieldErrors);
      focusInvalid();
      return;
    }
    setSaving(true);
    try {
      const response = await saveSetup(setup);
      signIn(response.access_token);
      setSetup(EMPTY_SETUP);
      await fetchState();
    } catch (error) {
      toast.error((error as Error).message);
    } finally {
      setSaving(false);
    }
  };

  return (
    <Dialog
      open
      size="md"
      title={LL.SETUP_TITLE()}
      onSubmit={() => void onSave()}
      actions={
        <>
          <Button icon="logout" color="secondary" onClick={() => signOut(true)}>
            {LL.SIGN_OUT()}
          </Button>
          <Button type="submit" icon="check" disabled={saving}>
            {LL.UPDATE()}
          </Button>
        </>
      }
    >
      <p>{LL.SETUP_TEXT()}</p>
      <div className="form">
        {state.admin && (
          <TextField
            type="password"
            autoComplete="new-password"
            label={LL.SETUP_ADMIN_PASSWORD({ name: me.username })}
            {...field('password')}
          />
        )}
        {state.ap && (
          <TextField
            type="password"
            autoComplete="new-password"
            label={LL.SETUP_AP_PASSWORD()}
            helperText={
              state.ap_required ? LL.SETUP_AP_REQUIRED() : LL.SETUP_AP_OPTIONAL()
            }
            {...field('ap_password')}
          />
        )}
        <p className="full">{LL.SETUP_VIEW_USER()}</p>
        <TextField
          autoComplete="off"
          autoCapitalize="none"
          label={LL.USERNAME(1)}
          {...field('view_username')}
        />
        <TextField
          type="password"
          autoComplete="new-password"
          label={LL.PASSWORD()}
          {...field('view_password')}
        />
      </div>
    </Dialog>
  );
};

export default SetupDialog;
