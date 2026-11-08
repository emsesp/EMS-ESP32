import { useCallback, useEffect, useState } from 'react';

import RefreshIcon from '@mui/icons-material/Refresh';
import {
  AppBar,
  Box,
  Button,
  CircularProgress,
  Toolbar,
  Typography
} from '@mui/material';

import CustomTheme from 'CustomTheme';
import MessageBox from 'components/MessageBox';
import SectionContent from 'components/SectionContent';
import { Toaster } from 'components/toast';

import Dashboard from './Dashboard';
import RestartMonitor from './RestartMonitor';
import SignIn from './SignIn';
import { getToken, readStatus, setToken } from './api';
import type { RecoveryStatus } from './types';

const STATUS_REFRESH_MS = 5000;

const RecoveryApp = () => {
  const [status, setStatus] = useState<RecoveryStatus>();
  const [error, setError] = useState<string>();
  const [restartTarget, setRestartTarget] = useState<string>();

  const loadStatus = useCallback(async () => {
    try {
      const data = await readStatus();
      if (!data.authenticated && getToken()) {
        setToken(null); // stale token, e.g. after the device restarted
      }
      setStatus(data);
      setError(undefined);
    } catch (e) {
      setError((e as Error).message);
    }
  }, []);

  useEffect(() => {
    void loadStatus();
  }, [loadStatus]);

  useEffect(() => {
    if (!status?.authenticated || restartTarget !== undefined) return;
    const timer = setInterval(() => void loadStatus(), STATUS_REFRESH_MS);
    return () => clearInterval(timer);
  }, [status?.authenticated, restartTarget, loadStatus]);

  const signOut = () => {
    setToken(null);
    void loadStatus();
  };

  let content;
  if (restartTarget !== undefined && status) {
    content = (
      <RestartMonitor
        target={restartTarget}
        viaAccessPoint={window.location.hostname === status.ap_ip}
        hostname={status.hostname}
      />
    );
  } else if (!status) {
    content = (
      <SectionContent>
        {error ? (
          <MessageBox level="error" message={error}>
            <Button
              sx={{ ml: 2 }}
              startIcon={<RefreshIcon />}
              variant="contained"
              color="error"
              onClick={() => void loadStatus()}
            >
              Retry
            </Button>
          </MessageBox>
        ) : (
          <Box sx={{ display: 'flex', justifyContent: 'center', p: 4 }}>
            <CircularProgress size={100} />
          </Box>
        )}
      </SectionContent>
    );
  } else if (!status.authenticated) {
    content = <SignIn onSignedIn={() => void loadStatus()} />;
  } else {
    content = (
      <>
        <AppBar
          position="fixed"
          sx={{ boxShadow: 'none', backgroundColor: '#2e586a' }}
        >
          <Toolbar>
            <img
              src="/app/icon.png"
              alt="EMS-ESP"
              style={{ height: 32, marginRight: 16 }}
            />
            <Typography variant="h6">EMS-ESP</Typography>
            <Typography variant="h6" sx={{ color: '#90caf9' }}>
              &nbsp;&nbsp;|&nbsp;&nbsp;
            </Typography>
            <Typography variant="h6">Recovery</Typography>
          </Toolbar>
        </AppBar>
        <Box component="main" sx={{ maxWidth: 900, mx: 'auto' }}>
          <Toolbar />
          <Dashboard
            status={status}
            onRestarting={setRestartTarget}
            onSignOut={signOut}
          />
        </Box>
      </>
    );
  }

  return (
    <CustomTheme>
      {content}
      <Toaster />
    </CustomTheme>
  );
};

export default RecoveryApp;
