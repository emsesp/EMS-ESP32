import { type KeyboardEvent, useState } from 'react';

import ForwardIcon from '@mui/icons-material/Forward';
import VisibilityIcon from '@mui/icons-material/Visibility';
import VisibilityOffIcon from '@mui/icons-material/VisibilityOff';
import {
  Box,
  Button,
  IconButton,
  InputAdornment,
  Paper,
  TextField,
  Typography
} from '@mui/material';
import type { Theme } from '@mui/material/styles';

import { toast } from 'components/toast';

import { HttpError, setToken, signIn } from './api';

interface SignInProps {
  onSignedIn: () => void;
}

const SignIn = ({ onSignedIn }: SignInProps) => {
  const [username, setUsername] = useState('');
  const [password, setPassword] = useState('');
  const [showPassword, setShowPassword] = useState(false);
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
      } else {
        toast.error('Error: ' + (error as Error).message);
      }
      setProcessing(false);
    }
  };

  const onKeyDown = (event: KeyboardEvent) => {
    if (event.key === 'Enter') {
      void doSignIn();
    }
  };

  return (
    <Box
      sx={(theme: Theme) => ({
        display: 'flex',
        height: '100vh',
        margin: 'auto',
        padding: 2,
        justifyContent: 'center',
        flexDirection: 'column',
        maxWidth: theme.breakpoints.values.sm
      })}
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
        <Typography variant="h4">EMS-ESP</Typography>
        <Typography sx={{ mb: 2 }} variant="h6" color="secondary">
          Recovery
        </Typography>
        <Box
          sx={{
            display: 'flex',
            flexDirection: 'column',
            gap: 1,
            alignItems: 'center'
          }}
        >
          <TextField
            disabled={processing}
            sx={{ width: '32ch' }}
            name="username"
            label="Username"
            value={username}
            onChange={(e) => setUsername(e.target.value)}
            slotProps={{ input: { autoCapitalize: 'none', autoCorrect: 'off' } }}
          />
          <TextField
            disabled={processing}
            sx={{ width: '32ch' }}
            name="password"
            label="Password"
            type={showPassword ? 'text' : 'password'}
            value={password}
            onChange={(e) => setPassword(e.target.value)}
            onKeyDown={onKeyDown}
            slotProps={{
              input: {
                endAdornment: (
                  <InputAdornment position="end">
                    <IconButton
                      onClick={() => setShowPassword(!showPassword)}
                      edge="end"
                    >
                      {showPassword ? <VisibilityIcon /> : <VisibilityOffIcon />}
                    </IconButton>
                  </InputAdornment>
                )
              }
            }}
          />
        </Box>

        <Button
          variant="contained"
          color="primary"
          sx={{ mt: 2 }}
          onClick={doSignIn}
          disabled={processing}
        >
          <ForwardIcon sx={{ mr: 1 }} />
          Sign In
        </Button>
      </Paper>
    </Box>
  );
};

export default SignIn;
