import { useRef, useState, useEffect } from 'react';
import type { FC } from 'react';

import * as SystemApi from 'api/system';
import { FormLoader } from 'components';

import { useI18nContext } from 'i18n/i18n-react';

const RESTART_TIMEOUT = 2 * 60 * 1000;
const POLL_TIMEOUT = 2000;
const POLL_INTERVAL = 5000;

const RestartMonitor: FC = () => {
  const [failed, setFailed] = useState<boolean>(false);
  const [timeoutId, setTimeoutId] = useState<NodeJS.Timeout>();

  const { LL } = useI18nContext();

  const timeoutAt = useRef(new Date().getTime() + RESTART_TIMEOUT);
  const poll = useRef(async () => {
    try {
      await SystemApi.readSystemStatus(POLL_TIMEOUT);
      document.location.href = '/fileUpdated';
    } catch (error) {
      if (new Date().getTime() < timeoutAt.current) {
        setTimeoutId(setTimeout(poll.current, POLL_INTERVAL));
      } else {
        setFailed(true);
      }
    }
  });

  useEffect(() => {
    poll.current();
  }, []);

  useEffect(() => () => timeoutId && clearTimeout(timeoutId), [timeoutId]);

  return <FormLoader message={LL.APPLICATION_RESTARTING() + '...'} errorMessage={failed ? 'Timed out' : undefined} />;
};

export default RestartMonitor;
