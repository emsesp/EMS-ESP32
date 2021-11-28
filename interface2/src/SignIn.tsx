import { FC, useContext, useState } from 'react';
import { ValidateFieldsError } from 'async-validator';
import { useSnackbar } from 'notistack';

import { Box, Fab, Paper, Typography } from '@mui/material';
import ForwardIcon from '@mui/icons-material/Forward';

import * as AuthenticationApi from './api/authentication';
import { PROJECT_NAME } from './api/env';
import { AuthenticationContext } from './contexts/authentication';

import { extractErrorMessage, onEnterCallback, updateValue } from './utils';
import { SignInRequest } from './types';
import { ValidatedTextField } from './components';
import { SIGN_IN_REQUEST_VALIDATOR, validate } from './validators';

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
    } catch (error: any) {
      if (error.response?.status === 401) {
        enqueueSnackbar('Invalid login details', { variant: 'warning' });
      } else {
        enqueueSnackbar(extractErrorMessage(error, 'Unexpected error, please try again'), { variant: 'error' });
      }
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
          paddingTop: '200px',
          backgroundImage: 'url("/app/icon.png")',
          backgroundRepeat: 'no-repeat',
          backgroundPosition: '50% ' + theme.spacing(2),
          backgroundSize: 'auto 150px',
          width: '100%'
        })}
      >
        <Typography variant="h4">{PROJECT_NAME}</Typography>
        <ValidatedTextField
          fieldErrors={fieldErrors}
          disabled={processing}
          name="username"
          label="Username"
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
          label="Password"
          value={signInRequest.password}
          onChange={updateLoginRequestValue}
          onKeyDown={submitOnEnter}
          margin="normal"
          variant="outlined"
          fullWidth
        />
        <Fab variant="extended" color="primary" sx={{ mt: 2 }} onClick={validateAndSignIn} disabled={processing}>
          <ForwardIcon sx={{ mr: 1 }} />
          Sign In
        </Fab>
      </Paper>
    </Box>
  );
};

export default SignIn;
