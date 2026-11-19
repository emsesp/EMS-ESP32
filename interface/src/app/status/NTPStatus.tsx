import * as NTPApi from 'api/ntp';

import { useRequest } from 'alova/client';
import { Loader } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import { NTPSyncStatus } from 'types';
import { formatDateTime, useInterval } from 'utils';

import StatusPage from './StatusPage';
import { ntpInfo } from './status';

const NTPStatus = () => {
  const { LL } = useI18nContext();
  const { data, send: loadData, error } = useRequest(NTPApi.readNTPStatus);

  useInterval(() => {
    void loadData();
  });

  if (!data) {
    return <Loader error={error?.message} onRetry={() => void loadData()} />;
  }

  const enabled = data.status !== NTPSyncStatus.NTP_DISABLED;
  const { state, text } = ntpInfo(data.status, LL);

  return (
    <StatusPage
      state={state}
      icon="schedule"
      title={`NTP ${text}`}
      text={enabled ? data.server : undefined}
      settings={{ to: '/settings/ntp', label: LL.SETTINGS_OF('NTP') }}
      rows={[
        enabled && { label: LL.NTP_SERVER(), value: data.server },
        { label: LL.LOCAL_TIME(0), value: formatDateTime(data.local_time) },
        { label: LL.UTC_TIME(), value: formatDateTime(data.utc_time) }
      ]}
    />
  );
};

export default NTPStatus;
