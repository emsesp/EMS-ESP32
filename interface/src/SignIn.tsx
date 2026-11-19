import { memo, useContext, useState } from 'react';

import * as AuthenticationApi from 'components/routing/authentication';
import { useRequest } from 'alova/client';
import { Button, LanguageSelect, TextField, toast } from 'components/ui';
import { AuthenticationContext } from 'contexts/authentication';
import { PROJECT_NAME } from 'env';
import { useI18nContext } from 'i18n/i18n-react';
import type { SignInRequest } from 'types';
import { SIGN_IN_REQUEST_VALIDATOR, ValidationError, validate } from 'validators';
import type { ValidateFieldsError } from 'validators/schema';

import './SignIn.css';

const SignIn = memo(() => {
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
  ).onSuccess((response: { data: { access_token: string } }) => {
    if (response.data) {
      authenticationContext.signIn(response.data.access_token);
    }
  });

  const signIn = async () => {
    await callSignIn(signInRequest).catch((event: Error) => {
      if (event.message === 'Unauthorized') {
        toast.warning(LL.INVALID_LOGIN());
      } else if (event.message === 'Too Many Requests') {
        toast.warning(LL.TOO_MANY_LOGINS());
      } else {
        toast.error(event.message || LL.ERROR());
      }
      setProcessing(false);
    });
  };

  const validateAndSignIn = async () => {
    setProcessing(true);
    setFieldErrors(undefined);
    SIGN_IN_REQUEST_VALIDATOR.messages({
      required: LL.IS_REQUIRED('%s')
    });
    try {
      await validate(SIGN_IN_REQUEST_VALIDATOR, signInRequest);
      await signIn();
    } catch (error) {
      setFieldErrors((error as ValidationError).fieldErrors);
      setProcessing(false);
    }
  };

  const field = (name: keyof SignInRequest) => ({
    name,
    fullWidth: true,
    disabled: processing,
    value: signInRequest[name],
    error: fieldErrors?.[name]?.[0]?.message,
    onInput: (e: { currentTarget: HTMLInputElement }) => {
      const { value } = e.currentTarget;
      setSignInRequest((prev) => ({ ...prev, [name]: value }));
    }
  });

  return (
    <main className="signin">
      <LanguageSelect className="signin-lang" />
      <form
        className="signin-card"
        noValidate
        onSubmit={(e) => {
          e.preventDefault();
          void validateAndSignIn();
        }}
      >
        <div className="signin-brand">
          <img className="signin-logo" src="/app/icon.png" alt="" />
          <h1>{PROJECT_NAME}</h1>
          <p className="signin-sub">{LL.SIGN_IN_SUBTITLE()}</p>
        </div>
        <div className="signin-fields">
          <TextField
            label={LL.USERNAME(0)}
            autoComplete="username"
            autoCapitalize="none"
            autoCorrect="off"
            autoFocus
            {...field('username')}
          />
          <TextField
            type="password"
            label={LL.PASSWORD()}
            autoComplete="current-password"
            {...field('password')}
          />
          <Button
            type="submit"
            icon="login"
            className="signin-submit"
            disabled={processing}
          >
            {LL.SIGN_IN()}
          </Button>
        </div>
      </form>
      <footer className="signin-foot">
        <a href="https://emsesp.org" target="_blank" rel="noreferrer">
          {LL.DOCUMENTATION()}
        </a>
        <span>·</span>
        <a
          href="https://github.com/emsesp/EMS-ESP32"
          target="_blank"
          rel="noreferrer"
        >
          GitHub
        </a>
      </footer>
    </main>
  );
});

export default SignIn;
