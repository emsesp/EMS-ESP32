import { Tab } from '@mui/material';
import { useCallback, useState } from 'react';
import { Navigate, Routes, Route, useNavigate } from 'react-router-dom';
import NetworkSettings from './NetworkSettings';
import NetworkStatus from './NetworkStatus';
import { WiFiConnectionContext } from './WiFiConnectionContext';
import WiFiNetworkScanner from './WiFiNetworkScanner';
import type { FC } from 'react';

import type { WiFiNetwork } from 'types';
import { RouterTabs, useLayoutTitle, useRouterTab } from 'components';
import { useI18nContext } from 'i18n/i18n-react';

const Network: FC = () => {
  const { LL } = useI18nContext();
  useLayoutTitle(LL.NETWORK(0));

  const { routerTab } = useRouterTab();

  const navigate = useNavigate();

  const [selectedNetwork, setSelectedNetwork] = useState<WiFiNetwork>();

  const selectNetwork = useCallback(
    (network: WiFiNetwork) => {
      setSelectedNetwork(network);
      navigate('settings');
    },
    [navigate]
  );

  const deselectNetwork = useCallback(() => {
    setSelectedNetwork(undefined);
  }, []);

  return (
    <WiFiConnectionContext.Provider
      value={{
        selectedNetwork,
        selectNetwork,
        deselectNetwork
      }}
    >
      <RouterTabs value={routerTab}>
        <Tab value="settings" label={LL.SETTINGS_OF(LL.NETWORK(1))} />
        <Tab value="status" label={LL.STATUS_OF(LL.NETWORK(1))} />
        <Tab value="scan" label={LL.NETWORK_SCAN()} />
      </RouterTabs>
      <Routes>
        <Route path="status" element={<NetworkStatus />} />
        <Route path="scan" element={<WiFiNetworkScanner />} />
        <Route path="settings" element={<NetworkSettings />} />
        <Route path="*" element={<Navigate replace to="settings" />} />
      </Routes>
    </WiFiConnectionContext.Provider>
  );
};

export default Network;
