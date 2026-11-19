import { useEffect, useState } from 'react';

import { Button, Message } from 'components/ui';

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
    <div className="rc-restart">
      <div className="rc-restart-box">
        <img src="/app/icon.png" alt="" />
        <h1>Restarting{target ? ` from ${target}` : ''}</h1>
        {gaveUp ? (
          <>
            <Message level="warning">
              The device has not come back on this address.
              {viaAccessPoint &&
                ` If you were connected to the recovery access point, reconnect to your normal network and open http://${hostname}.local`}
            </Message>
            <Button icon="refresh" onClick={() => window.location.reload()}>
              Reload
            </Button>
          </>
        ) : (
          <>
            <p className="rc-wait">Please wait&hellip;</p>
            <div
              className="rc-indeterminate"
              role="progressbar"
              aria-label="Restarting"
            />
            {viaAccessPoint && (
              <p className="cap">
                EMS-ESP does not use the recovery access point. If this page does not
                reload, reconnect to your normal network and open http://{hostname}
                .local
              </p>
            )}
          </>
        )}
      </div>
    </div>
  );
};

export default RestartMonitor;
