import * as APApi from 'api/ap';

import { useRequest } from 'alova/client';
import { Loader } from 'components/ui';
import { useI18nContext } from 'i18n/i18n-react';
import { APNetworkStatus } from 'types';
import { useInterval } from 'utils';

import StatusPage from './StatusPage';
import { apInfo } from './status';

const APStatus = () => {
  const { LL } = useI18nContext();
  const { data, send: loadData, error } = useRequest(APApi.readAPStatus);

  useInterval(() => {
    void loadData();
  });

  if (!data) {
    return <Loader error={error?.message} onRetry={() => void loadData()} />;
  }

  const { state, text } = apInfo(data.status, LL);

  return (
    <StatusPage
      state={state}
      icon={
        data.status === APNetworkStatus.INACTIVE
          ? 'wifi_tethering_off'
          : 'wifi_tethering'
      }
      title={`${LL.ACCESS_POINT(0)}: ${text}`}
      text={data.ip_address || undefined}
      settings={{ to: '/settings/ap', label: LL.SETTINGS_OF(LL.ACCESS_POINT(0)) }}
      rows={[
        { label: LL.ADDRESS_OF('IP'), value: data.ip_address || LL.NONE() },
        { label: LL.ADDRESS_OF('MAC'), value: data.mac_address || LL.NONE() },
        { label: LL.AP_CLIENTS(), value: data.station_num }
      ]}
    />
  );
};

export default APStatus;
