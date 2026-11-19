import { useCallback, useEffect, useState } from 'react';

import { Button, Loader, Toaster } from 'components/ui';

import Dashboard from './Dashboard';
import './Recovery.css';
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
    content = <Loader page error={error} onRetry={() => void loadStatus()} />;
  } else if (!status.authenticated) {
    content = <SignIn onSignedIn={() => void loadStatus()} />;
  } else {
    content = (
      <>
        <header className="rc-bar">
          <img src="/app/icon.png" alt="" />
          <h1 className="grow">
            EMS-ESP<span className="rc-sep">|</span>Recovery
          </h1>
          <Button
            icon="logout"
            aria-label="Sign Out"
            className="rc-signout"
            onClick={signOut}
          >
            <span>Sign Out</span>
          </Button>
        </header>
        <main className="rc-main">
          <Dashboard
            status={status}
            onRestarting={setRestartTarget}
            onSignOut={signOut}
            onChanged={() => void loadStatus()}
          />
        </main>
      </>
    );
  }

  return (
    <>
      {content}
      <Toaster />
    </>
  );
};

export default RecoveryApp;
