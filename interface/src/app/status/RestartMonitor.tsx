import { type FC, useEffect, useRef, useState } from 'react';

import { readHardwareStatus } from 'api/system';

import { useRequest } from 'alova/client';
import { FormLoader } from 'components';
import { useI18nContext } from 'i18n/i18n-react';

const RESTART_TIMEOUT = 2 * 60 * 1000; // 2 minutes
const POLL_INTERVAL = 2000; // every 2 seconds

export interface RestartMonitorProps {
  message?: string;
}

const RestartMonitor: FC<RestartMonitorProps> = ({ message }) => {
  const [failed, setFailed] = useState<boolean>(false);
  const [timeoutId, setTimeoutId] = useState<NodeJS.Timeout>();

  const { LL } = useI18nContext();

  const timeoutAt = useRef(new Date().getTime() + RESTART_TIMEOUT);

  const { send } = useRequest(readHardwareStatus, { immediate: false });

  const poll = useRef(async () => {
    try {
      await send();
      document.location.href = '/';
    } catch {
      if (new Date().getTime() < timeoutAt.current) {
        setTimeoutId(setTimeout(poll.current, POLL_INTERVAL));
      } else {
        setFailed(true);
      }
    }
  });

  useEffect(() => {
    setTimeoutId(setTimeout(poll.current, POLL_INTERVAL));
  }, []);

  useEffect(() => () => timeoutId && clearTimeout(timeoutId), [timeoutId]);

  return (
    <FormLoader
      message={message ? message : LL.APPLICATION_RESTARTING() + '...'}
      errorMessage={failed ? 'Timed out' : undefined}
    />
  );
};

export default RestartMonitor;
