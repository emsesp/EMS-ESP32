import { useEffect, useRef, useState } from 'react';
import type { FC } from 'react';

import * as SystemApi from 'api/system';

import { useRequest } from 'alova';
import { FormLoader } from 'components';
import { useI18nContext } from 'i18n/i18n-react';

const RESTART_TIMEOUT = 2 * 60 * 1000;
const POLL_INTERVAL = 3000;

const RestartMonitor: FC = () => {
  const [failed, setFailed] = useState<boolean>(false);
  const [timeoutId, setTimeoutId] = useState<NodeJS.Timeout>();
  const { LL } = useI18nContext();
  const { send } = useRequest(SystemApi.readSystemStatus, { force: true });
  const timeoutAt = useRef(new Date().getTime() + RESTART_TIMEOUT);

  const poll = useRef(async () => {
    try {
      await send();
      document.location.href = '/';
    } catch (error) {
      if (new Date().getTime() < timeoutAt.current) {
        setTimeoutId(setTimeout(poll.current, POLL_INTERVAL));
      } else {
        setFailed(true);
      }
    }
  });

  useEffect(() => {
    void poll.current();
  }, []);

  useEffect(() => () => timeoutId && clearTimeout(timeoutId), [timeoutId]);

  return (
    <FormLoader
      message={LL.APPLICATION_RESTARTING() + '...'}
      errorMessage={failed ? 'Timed out' : undefined}
    />
  );
};

export default RestartMonitor;
