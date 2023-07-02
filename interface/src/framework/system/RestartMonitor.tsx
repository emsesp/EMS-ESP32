import { useRetriableRequest } from '@alova/scene-react';
import { useState } from 'react';
import type { FC } from 'react';

import * as SystemApi from 'api/system';
import { FormLoader } from 'components';

import { useI18nContext } from 'i18n/i18n-react';

const RestartMonitor: FC = () => {
  const [failed, setFailed] = useState<boolean>(false);

  const { LL } = useI18nContext();

  // eslint-disable-next-line @typescript-eslint/unbound-method
  const { onFail, onSuccess } = useRetriableRequest(SystemApi.readSystemStatus(), {
    retry: 10,
    backoff: {
      delay: 1500
    }
  });

  onFail(() => {
    setFailed(true);
  });

  onSuccess(() => {
    document.location.href = '/fileUpdated';
  });

  return <FormLoader message={LL.APPLICATION_RESTARTING() + '...'} errorMessage={failed ? 'Timed out' : undefined} />;
};

export default RestartMonitor;
