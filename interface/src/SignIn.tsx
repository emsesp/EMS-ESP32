import { useContext, useState } from 'react';
import { toast } from 'react-toastify';

import ForwardIcon from '@mui/icons-material/Forward';
import { Box, Button, Paper, Typography } from '@mui/material';

import * as AuthenticationApi from 'components/routing/authentication';
import { useRequest } from 'alova/client';
import type { ValidateFieldsError } from 'async-validator';
import {
  LanguageSelector,
  ValidatedPasswordField,
  ValidatedTextField
} from 'components';
import { AuthenticationContext } from 'contexts/authentication';
import { PROJECT_NAME } from 'env';
import { useI18nContext } from 'i18n/i18n-react';
import type { SignInRequest } from 'types';
import { onEnterCallback, updateValue } from 'utils';
import { SIGN_IN_REQUEST_VALIDATOR, validate } from 'validators';

const SignIn = () => {
  const authenticationContext = useContext(AuthenticationContext);

  const { LL } = useI18nContext();

  const [signInRequest, setSignInRequest] = useState<SignInRequest>({
    username: '',
    password: ''
  });
  const [processing, setProcessing] = useState<boolean>(false);
  const [fieldErrors, setFieldErrors] = useState<ValidateFieldsError>();

  const { send: callSignIn } = useRequest(
    (request: SignInRequest) => AuthenticationApi.signIn(request),
    {
      immediate: false
    }
  ).onSuccess((response) => {
    if (response.data) {
      authenticationContext.signIn(response.data.access_token);
    }
  });

  const updateLoginRequestValue = updateValue(setSignInRequest);

  const signIn = async () => {
    await callSignIn(signInRequest).catch((event: Error) => {
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
    } catch (error) {
      setFieldErrors(error as ValidateFieldsError);
      setProcessing(false);
    }
  };

  const submitOnEnter = onEnterCallback(signIn);

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

        <LanguageSelector />

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
            slotProps={{
              input: {
                autoCapitalize: 'none',
                autoCorrect: 'off'
              }
            }}
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

        <Button
          variant="contained"
          color="primary"
          sx={{ mt: 2 }}
          onClick={validateAndSignIn}
          disabled={processing}
        >
          <ForwardIcon sx={{ mr: 1 }} />
          {LL.SIGN_IN()}
        </Button>
      </Paper>
    </Box>
  );
};

export default SignIn;
