import { FC, useContext, useState, ChangeEventHandler } from 'react';
import { ValidateFieldsError } from 'async-validator';
import { useSnackbar } from 'notistack';

import { Box, Fab, Paper, Typography, MenuItem } from '@mui/material';
import ForwardIcon from '@mui/icons-material/Forward';

import * as AuthenticationApi from './api/authentication';
import { PROJECT_NAME } from './api/env';
import { AuthenticationContext } from './contexts/authentication';

import { AxiosError } from 'axios';

import { extractErrorMessage, onEnterCallback, updateValue } from './utils';
import { SignInRequest } from './types';
import { ValidatedTextField } from './components';
import { SIGN_IN_REQUEST_VALIDATOR, validate } from './validators';

import { I18nContext } from './i18n/i18n-react';
import type { Locales } from './i18n/i18n-types';
import { locales } from './i18n/i18n-util';
import { loadLocaleAsync } from './i18n/i18n-util.async';

const SignIn: FC = () => {
  const authenticationContext = useContext(AuthenticationContext);
  const { enqueueSnackbar } = useSnackbar();

  const [signInRequest, setSignInRequest] = useState<SignInRequest>({
    username: '',
    password: ''
  });
  const [processing, setProcessing] = useState<boolean>(false);
  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();

  const updateLoginRequestValue = updateValue(setSignInRequest);

  const validateAndSignIn = async () => {
    setProcessing(true);
    SIGN_IN_REQUEST_VALIDATOR.messages({
      required: '%s ' + LL.IS_REQUIRED()
    });
    try {
      await validate(SIGN_IN_REQUEST_VALIDATOR, signInRequest);
      signIn();
    } catch (errors: any) {
      setFieldErrors(errors);
      setProcessing(false);
    }
  };

  const signIn = async () => {
    try {
      const { data: loginResponse } = await AuthenticationApi.signIn(signInRequest);
      authenticationContext.signIn(loginResponse.access_token);
    } catch (error: unknown) {
      if (error instanceof AxiosError) {
        if (error.response?.status === 401) {
          enqueueSnackbar(LL.INVALID_LOGIN(), { variant: 'warning' });
        }
      } else {
        enqueueSnackbar(extractErrorMessage(error, 'Unexpected error, please try again'), { variant: 'error' });
      }
      setProcessing(false);
    }
  };

  const submitOnEnter = onEnterCallback(signIn);

  const { locale, LL, setLocale } = useContext(I18nContext);

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
          paddingTop: '200px',
          backgroundImage: 'url("/app/icon.png")',
          backgroundRepeat: 'no-repeat',
          backgroundPosition: '50% ' + theme.spacing(2),
          backgroundSize: 'auto 150px',
          width: '100%'
        })}
      >
        <Typography variant="h4">{PROJECT_NAME}</Typography>
        <Box
          sx={{
            '& .MuiTextField-root': { m: 2, width: '15ch' }
          }}
        >
          <ValidatedTextField
            name="locale"
            label="Language/Sprache"
            variant="outlined"
            value={locale || ''}
            onChange={onLocaleSelected}
            margin="normal"
            size="small"
            select
          >
            {locales.map((loc) => (
              <MenuItem key={loc} value={loc}>
                {loc}
              </MenuItem>
            ))}
          </ValidatedTextField>
        </Box>
        <ValidatedTextField
          fieldErrors={fieldErrors}
          disabled={processing}
          name="username"
          label={LL.USERNAME()}
          value={signInRequest.username}
          onChange={updateLoginRequestValue}
          margin="normal"
          variant="outlined"
          fullWidth
        />
        <ValidatedTextField
          fieldErrors={fieldErrors}
          disabled={processing}
          type="password"
          name="password"
          label={LL.PASSWORD()}
          value={signInRequest.password}
          onChange={updateLoginRequestValue}
          onKeyDown={submitOnEnter}
          margin="normal"
          variant="outlined"
          fullWidth
        />
        <Fab variant="extended" color="primary" sx={{ mt: 2 }} onClick={validateAndSignIn} disabled={processing}>
          <ForwardIcon sx={{ mr: 1 }} />
          {LL.SIGN_IN()}
        </Fab>
      </Paper>
    </Box>
  );
};

export default SignIn;
