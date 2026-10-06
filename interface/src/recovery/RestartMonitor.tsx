import { useEffect, useState } from 'react';

import RefreshIcon from '@mui/icons-material/Refresh';
import { Box, Button, LinearProgress, Typography } from '@mui/material';

import MessageBox from 'components/MessageBox';

import { STATUS_URL } from './api';

const POLL_INTERVAL_MS = 1000;
const POLL_TIMEOUT_MS = 3000;
const FIRST_POLL_DELAY_MS = 1500;
// reload anyway if the device never appeared to go down
const NO_DOWNTIME_RELOAD_MS = 20000;
const GIVE_UP_MS = 90000;

interface RestartMonitorProps {
  target: string;
  viaAccessPoint: boolean;
  hostname: string;
}

// Waits for the device to go down and come back, then reloads the page. Both the recovery app
// and EMS-ESP answer on STATUS_URL (EMS-ESP serves its WebUI for unknown GETs).
const RestartMonitor = ({
  target,
  viaAccessPoint,
  hostname
}: RestartMonitorProps) => {
  const [gaveUp, setGaveUp] = useState(false);

  useEffect(() => {
    const started = Date.now();
    let seenDown = false;
    let timer: ReturnType<typeof setTimeout>;
    let cancelled = false;

    const poll = async () => {
      let up: boolean;
      try {
        const response = await fetch(STATUS_URL, {
          cache: 'no-store',
          signal: AbortSignal.timeout(POLL_TIMEOUT_MS)
        });
        up = response.ok;
      } catch {
        up = false;
      }
      if (cancelled) return;

      const elapsed = Date.now() - started;
      if (!up) {
        seenDown = true;
      } else if (seenDown || elapsed > NO_DOWNTIME_RELOAD_MS) {
        window.location.reload();
        return;
      }
      if (elapsed > GIVE_UP_MS) {
        setGaveUp(true);
        return;
      }
      timer = setTimeout(() => void poll(), POLL_INTERVAL_MS);
    };

    timer = setTimeout(() => void poll(), FIRST_POLL_DELAY_MS);
    return () => {
      cancelled = true;
      clearTimeout(timer);
    };
  }, []);

  return (
    <Box
      sx={{
        position: 'fixed',
        inset: 0,
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'center'
      }}
    >
      <Box
        sx={{
          width: '30%',
          minWidth: '300px',
          maxWidth: '500px',
          backgroundColor: '#393939',
          border: 2,
          borderColor: '#565656',
          borderRadius: '8px',
          boxShadow: '0 8px 32px rgba(0, 0, 0, 0.3)',
          p: 3,
          display: 'flex',
          alignItems: 'center',
          flexDirection: 'column'
        }}
      >
        <img
          src="/app/icon.png"
          alt="EMS-ESP"
          style={{ width: '40px', height: '40px', marginBottom: '16px' }}
        />
        <Typography sx={{ textAlign: 'center' }} variant="h6">
          Restarting{target ? ` from ${target}` : ''}
        </Typography>

        {gaveUp ? (
          <>
            <MessageBox sx={{ mt: 2 }} level="warning">
              The device has not come back on this address.
              {viaAccessPoint &&
                ` If you were connected to the recovery access point, reconnect to your normal network and open http://${hostname}.local`}
            </MessageBox>
            <Button
              sx={{ mt: 2 }}
              startIcon={<RefreshIcon />}
              variant="outlined"
              onClick={() => window.location.reload()}
            >
              Reload
            </Button>
          </>
        ) : (
          <>
            <Typography sx={{ mt: 2, textAlign: 'center' }} variant="h6">
              Please wait&hellip;
            </Typography>
            <Box sx={{ width: '100%', mt: 2 }}>
              <LinearProgress />
            </Box>
            {viaAccessPoint && (
              <Typography
                sx={{ mt: 2, textAlign: 'center' }}
                variant="body2"
                color="text.secondary"
              >
                EMS-ESP does not use the recovery access point. If this page does not
                reload, reconnect to your normal network and open http://{hostname}
                .local
              </Typography>
            )}
          </>
        )}
      </Box>
    </Box>
  );
};

export default RestartMonitor;
