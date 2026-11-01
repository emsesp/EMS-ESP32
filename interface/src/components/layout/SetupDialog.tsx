import { useContext, useEffect, useState } from 'react';
import { useNavigate } from 'react-router';

import SaveIcon from '@mui/icons-material/Save';
import {
  Box,
  Button,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  Typography
} from '@mui/material';

import { readSetup, saveSetup } from 'api/security';

import { dialogStyle } from 'CustomTheme';
import { useRequest } from 'alova/client';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import type { SetupType } from 'types';
import { updateValue } from 'utils';
import { ValidationError, validate } from 'validators';
import type { ValidateFieldsError } from 'validators/schema';
import { createSetupValidator } from 'validators/security';

import MessageBox from '../MessageBox';
import { ValidatedPasswordField, ValidatedTextField } from '../inputs';
import { toast } from '../toast';

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

  const updateFormValue = updateValue((updater) =>
    setSetup(
      (prev) =>
        updater(prev as unknown as Record<string, unknown>) as unknown as SetupType
    )
  );

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
      <Box sx={{ px: 2, pt: 2 }}>
        <MessageBox level="warning" message={LL.SETUP_AP_DEFAULT()}>
          <Button
            sx={{ ml: 2 }}
            variant="contained"
            color="warning"
            onClick={() => void navigate('/settings/ap')}
          >
            {LL.SETTINGS_OF(LL.ACCESS_POINT(0))}
          </Button>
          <Button
            sx={{ ml: 1 }}
            variant="contained"
            color="warning"
            onClick={dismiss}
          >
            {LL.CLOSE()}
          </Button>
        </MessageBox>
      </Box>
    );
  }

  const onSave = async () => {
    try {
      setFieldErrors(undefined);
      await validate(createSetupValidator(state, setup), setup);
    } catch (error) {
      setFieldErrors((error as ValidationError).fieldErrors);
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
    <Dialog sx={dialogStyle} open fullWidth maxWidth="sm">
      <DialogTitle>{LL.SETUP_TITLE()}</DialogTitle>
      <DialogContent dividers>
        <Typography variant="body2">{LL.SETUP_TEXT()}</Typography>
        {state.admin && (
          <ValidatedPasswordField
            fieldErrors={fieldErrors || {}}
            name="password"
            label={LL.SETUP_ADMIN_PASSWORD({ name: me.username })}
            fullWidth
            variant="outlined"
            value={setup.password}
            onChange={updateFormValue}
            margin="normal"
          />
        )}
        {state.ap && (
          <ValidatedPasswordField
            fieldErrors={fieldErrors || {}}
            name="ap_password"
            label={LL.SETUP_AP_PASSWORD()}
            helperText={
              state.ap_required ? LL.SETUP_AP_REQUIRED() : LL.SETUP_AP_OPTIONAL()
            }
            fullWidth
            variant="outlined"
            value={setup.ap_password}
            onChange={updateFormValue}
            margin="normal"
          />
        )}
        <Typography variant="body2" sx={{ mt: 2 }}>
          {LL.SETUP_VIEW_USER()}
        </Typography>
        <ValidatedTextField
          fieldErrors={fieldErrors || {}}
          name="view_username"
          label={LL.USERNAME(1)}
          fullWidth
          variant="outlined"
          value={setup.view_username}
          onChange={updateFormValue}
          margin="normal"
        />
        <ValidatedPasswordField
          fieldErrors={fieldErrors || {}}
          name="view_password"
          label={LL.PASSWORD()}
          fullWidth
          variant="outlined"
          value={setup.view_password}
          onChange={updateFormValue}
          margin="normal"
        />
      </DialogContent>
      <DialogActions>
        <Button variant="outlined" color="secondary" onClick={() => signOut(true)}>
          {LL.SIGN_OUT()}
        </Button>
        <Button
          startIcon={<SaveIcon />}
          variant="outlined"
          color="primary"
          disabled={saving}
          onClick={onSave}
        >
          {LL.UPDATE()}
        </Button>
      </DialogActions>
    </Dialog>
  );
};

export default SetupDialog;
