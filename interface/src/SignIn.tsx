import { FC, useContext, useState } from 'react';
import { ValidateFieldsError } from 'async-validator';
import { toast } from 'react-toastify';

import { Box, Fab, Paper, Typography, Button } from '@mui/material';
import ForwardIcon from '@mui/icons-material/Forward';

import * as AuthenticationApi from 'api/authentication';
import { PROJECT_NAME } from 'api/env';
import { AuthenticationContext } from 'contexts/authentication';

import { extractErrorMessage, onEnterCallback, updateValue } from 'utils';
import { SignInRequest } from 'types';
import { ValidatedTextField } from 'components';
import { SIGN_IN_REQUEST_VALIDATOR, validate } from 'validators';

import { I18nContext } from 'i18n/i18n-react';
import type { Locales } from 'i18n/i18n-types';
import { loadLocaleAsync } from 'i18n/i18n-util.async';

import { ReactComponent as NLflag } from 'i18n/NL.svg';
import { ReactComponent as DEflag } from 'i18n/DE.svg';
import { ReactComponent as GBflag } from 'i18n/GB.svg';
import { ReactComponent as SVflag } from 'i18n/SV.svg';
import { ReactComponent as PLflag } from 'i18n/PL.svg';
import { ReactComponent as NOflag } from 'i18n/NO.svg';
import { ReactComponent as FRflag } from 'i18n/FR.svg';
import { ReactComponent as TRflag } from 'i18n/TR.svg';

const SignIn: FC = () => {
  const authenticationContext = useContext(AuthenticationContext);

  const { LL, setLocale, locale } = useContext(I18nContext);

  const [signInRequest, setSignInRequest] = useState<SignInRequest>({
    username: '',
    password: ''
  });
  const [processing, setProcessing] = useState<boolean>(false);
  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();

  const updateLoginRequestValue = updateValue(setSignInRequest);

  const signIn = async () => {
    try {
      const { data: loginResponse } = await AuthenticationApi.signIn(signInRequest);
      authenticationContext.signIn(loginResponse.access_token);
    } catch (error) {
      if (error.response) {
        if (error.response?.status === 401) {
          toast.warn(LL.INVALID_LOGIN());
        }
      } else {
        toast.error(extractErrorMessage(error, LL.ERROR()));
      }
      setProcessing(false);
    }
  };

  const validateAndSignIn = async () => {
    setProcessing(true);
    SIGN_IN_REQUEST_VALIDATOR.messages({
      required: LL.IS_REQUIRED('%s')
    });
    try {
      await validate(SIGN_IN_REQUEST_VALIDATOR, signInRequest);
      signIn();
    } catch (errors: any) {
      setFieldErrors(errors);
      setProcessing(false);
    }
  };

  const submitOnEnter = onEnterCallback(signIn);

  const selectLocale = async (loc: Locales) => {
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
        <Box
          mt={2}
          mb={2}
          sx={{
            '& button, & a, & .MuiCard-root': {
              mt: 1,
              mx: 1
            }
          }}
        >
          <Button size="small" variant={locale === 'en' ? 'contained' : 'outlined'} onClick={() => selectLocale('en')}>
            <GBflag style={{ width: 24 }} />
            &nbsp;EN
          </Button>
          <Button size="small" variant={locale === 'de' ? 'contained' : 'outlined'} onClick={() => selectLocale('de')}>
            <DEflag style={{ width: 24 }} />
            &nbsp;DE
          </Button>
          <Button size="small" variant={locale === 'fr' ? 'contained' : 'outlined'} onClick={() => selectLocale('fr')}>
            <FRflag style={{ width: 24 }} />
            &nbsp;FR
          </Button>
          <Button size="small" variant={locale === 'nl' ? 'contained' : 'outlined'} onClick={() => selectLocale('nl')}>
            <NLflag style={{ width: 24 }} />
            &nbsp;NL
          </Button>
          <Button size="small" variant={locale === 'no' ? 'contained' : 'outlined'} onClick={() => selectLocale('no')}>
            <NOflag style={{ width: 24 }} />
            &nbsp;NO
          </Button>
          <Button size="small" variant={locale === 'pl' ? 'contained' : 'outlined'} onClick={() => selectLocale('pl')}>
            <PLflag style={{ width: 24 }} />
            &nbsp;PL
          </Button>
          <Button size="small" variant={locale === 'sv' ? 'contained' : 'outlined'} onClick={() => selectLocale('sv')}>
            <SVflag style={{ width: 24 }} />
            &nbsp;SV
          </Button>
          <Button size="small" variant={locale === 'tr' ? 'contained' : 'outlined'} onClick={() => selectLocale('tr')}>
            <TRflag style={{ width: 24 }} />
            &nbsp;TR
          </Button>
        </Box>

        <ValidatedTextField
          fieldErrors={fieldErrors}
          disabled={processing}
          name="username"
          label={LL.USERNAME(0)}
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
