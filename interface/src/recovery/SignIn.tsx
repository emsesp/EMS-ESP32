import { useState } from 'react';

import { Button, TextField, toast } from 'components/ui';

import { HttpError, setToken, signIn } from './api';

interface SignInProps {
  onSignedIn: () => void;
}

const SignIn = ({ onSignedIn }: SignInProps) => {
  const [username, setUsername] = useState('');
  const [password, setPassword] = useState('');
  const [processing, setProcessing] = useState(false);

  const doSignIn = async () => {
    if (!username || !password) {
      toast.warning('Please enter the admin username and password');
      return;
    }
    setProcessing(true);
    try {
      const { access_token } = await signIn(username, password);
      setToken(access_token);
      onSignedIn();
    } catch (error) {
      if (error instanceof HttpError && error.status === 401) {
        toast.warning('Invalid login details');
      } else if (error instanceof HttpError && error.status === 429) {
        toast.warning('Too many failed sign-ins, try again in a few minutes');
      } else {
        toast.error('Error: ' + (error as Error).message);
      }
      setProcessing(false);
    }
  };

  return (
    <main className="rc-signin">
      <form
        className="rc-signin-card"
        noValidate
        onSubmit={(e) => {
          e.preventDefault();
          void doSignIn();
        }}
      >
        <img className="rc-signin-logo" src="/app/icon.png" alt="" />
        <h1>EMS-ESP</h1>
        <p className="rc-signin-sub">Recovery</p>
        <TextField
          label="Username"
          name="username"
          autoComplete="username"
          autoCapitalize="none"
          autoCorrect="off"
          autoFocus
          fullWidth
          disabled={processing}
          value={username}
          onInput={(e) => setUsername(e.currentTarget.value)}
        />
        <TextField
          type="password"
          label="Password"
          name="password"
          autoComplete="current-password"
          fullWidth
          disabled={processing}
          value={password}
          onInput={(e) => setPassword(e.currentTarget.value)}
        />
        <Button
          type="submit"
          variant="contained"
          icon="login"
          className="rc-signin-submit"
          disabled={processing}
        >
          Sign In
        </Button>
      </form>
    </main>
  );
};

export default SignIn;
