import ForwardIcon from '@mui/icons-material/Forward';
import { Box, Paper, Typography, MenuItem, TextField, Button } from '@mui/material';
import { useRequest } from 'alova';
import { useContext, useState } from 'react';
import { toast } from 'react-toastify';
import { FeaturesContext } from './contexts/features';
import type { ValidateFieldsError } from 'async-validator';

import type { Locales } from 'i18n/i18n-types';
import type { ChangeEventHandler, FC } from 'react';
import type { SignInRequest } from 'types';
import * as AuthenticationApi from 'api/authentication';
import { PROJECT_NAME } from 'api/env';

import { ValidatedPasswordField, ValidatedTextField } from 'components';
import { AuthenticationContext } from 'contexts/authentication';

import DEflag from 'i18n/DE.svg';
import FRflag from 'i18n/FR.svg';
import GBflag from 'i18n/GB.svg';
import ITflag from 'i18n/IT.svg';
import NLflag from 'i18n/NL.svg';
import NOflag from 'i18n/NO.svg';
import PLflag from 'i18n/PL.svg';
import SKflag from 'i18n/SK.svg';
import SVflag from 'i18n/SV.svg';
import TRflag from 'i18n/TR.svg';
import { I18nContext } from 'i18n/i18n-react';
import { loadLocaleAsync } from 'i18n/i18n-util.async';
import { onEnterCallback, updateValue } from 'utils';
import { SIGN_IN_REQUEST_VALIDATOR, validate } from 'validators';

const SignIn: FC = () => {
  const authenticationContext = useContext(AuthenticationContext);

  const { LL, setLocale, locale } = useContext(I18nContext);

  const { features } = useContext(FeaturesContext);

  const [signInRequest, setSignInRequest] = useState<SignInRequest>({
    username: '',
    password: ''
  });
  const [processing, setProcessing] = useState<boolean>(false);
  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();

  const { send: callSignIn, onSuccess } = useRequest((request: SignInRequest) => AuthenticationApi.signIn(request), {
    immediate: false
  });

  onSuccess((response) => {
    if (response.data) {
      authenticationContext.signIn(response.data.access_token);
    }
  });

  const updateLoginRequestValue = updateValue(setSignInRequest);

  const signIn = async () => {
    await callSignIn(signInRequest).catch((event) => {
      if (event.message === 'Unauthorized') {
        toast.warning(LL.INVALID_LOGIN());
      } else {
        toast.error(LL.ERROR() + ' ' + event.message);
      }
      setProcessing(false);
    });
  };

  const validateAndSignIn = async () => {
    setProcessing(true);
    SIGN_IN_REQUEST_VALIDATOR.messages({
      required: LL.IS_REQUIRED('%s')
    });
    try {
      await validate(SIGN_IN_REQUEST_VALIDATOR, signInRequest);
      await signIn();
    } catch (errors: any) {
      setFieldErrors(errors);
      setProcessing(false);
    }
  };

  const submitOnEnter = onEnterCallback(signIn);

  const onLocaleSelected: ChangeEventHandler<HTMLInputElement> = async ({ target }) => {
    const loc = target.value as Locales;
    localStorage.setItem('lang', loc);
    await loadLocaleAsync(loc);
    setLocale(loc);
  };

  return (
    <Box
      display="flex"
      height="100vh"
      margin="auto"
      padding={2}
      justifyContent="center"
      flexDirection="column"
      maxWidth={(theme) => theme.breakpoints.values.sm}
    >
      <Paper
        sx={(theme) => ({
          textAlign: 'center',
          padding: theme.spacing(2),
          paddingTop: '172px',
          backgroundImage: 'url("/app/icon.png")',
          backgroundRepeat: 'no-repeat',
          backgroundPosition: '50% ' + theme.spacing(2),
          width: '100%'
        })}
      >
        <Typography variant="h4">{PROJECT_NAME}</Typography>
        <Typography variant="subtitle2">{features.version}</Typography>

        <TextField name="locale" variant="outlined" value={locale} onChange={onLocaleSelected} size="small" select>
          <MenuItem key="de" value="de">
            <img src={DEflag} style={{ width: 16, verticalAlign: 'middle' }} />
            &nbsp;DE
          </MenuItem>
          <MenuItem key="en" value="en">
            <img src={GBflag} style={{ width: 16, verticalAlign: 'middle' }} />
            &nbsp;EN
          </MenuItem>
          <MenuItem key="fr" value="fr">
            <img src={FRflag} style={{ width: 16, verticalAlign: 'middle' }} />
            &nbsp;FR
          </MenuItem>
          <MenuItem key="it" value="it">
            <img src={ITflag} style={{ width: 16, verticalAlign: 'middle' }} />
            &nbsp;IT
          </MenuItem>
          <MenuItem key="nl" value="nl">
            <img src={NLflag} style={{ width: 16, verticalAlign: 'middle' }} />
            &nbsp;NL
          </MenuItem>
          <MenuItem key="no" value="no">
            <img src={NOflag} style={{ width: 16, verticalAlign: 'middle' }} />
            &nbsp;NO
          </MenuItem>
          <MenuItem key="pl" value="pl">
            <img src={PLflag} style={{ width: 16, verticalAlign: 'middle' }} />
            &nbsp;PL
          </MenuItem>
          <MenuItem key="sk" value="sk">
            <img src={SKflag} style={{ width: 16, verticalAlign: 'middle' }} />
            &nbsp;SK
          </MenuItem>
          <MenuItem key="sv" value="sv">
            <img src={SVflag} style={{ width: 16, verticalAlign: 'middle' }} />
            &nbsp;SV
          </MenuItem>
          <MenuItem key="tr" value="tr">
            <img src={TRflag} style={{ width: 16, verticalAlign: 'middle' }} />
            &nbsp;TR
          </MenuItem>
        </TextField>

        <Box display="flex" flexDirection="column" alignItems="center">
          <ValidatedTextField
            fieldErrors={fieldErrors}
            disabled={processing}
            sx={{
              width: 240
            }}
            name="username"
            label={LL.USERNAME(0)}
            value={signInRequest.username}
            onChange={updateLoginRequestValue}
            margin="normal"
            variant="outlined"
          />
          <ValidatedPasswordField
            fieldErrors={fieldErrors}
            disabled={processing}
            sx={{
              width: 240
            }}
            name="password"
            label={LL.PASSWORD()}
            value={signInRequest.password}
            onChange={updateLoginRequestValue}
            onKeyDown={submitOnEnter}
            variant="outlined"
          />
        </Box>

        <Button variant="contained" color="primary" sx={{ mt: 2 }} onClick={validateAndSignIn} disabled={processing}>
          <ForwardIcon sx={{ mr: 1 }} />
          {LL.SIGN_IN()}
        </Button>
      </Paper>
    </Box>
  );
};

export default SignIn;
