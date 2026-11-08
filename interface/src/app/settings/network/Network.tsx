import { memo, useState } from 'react';
import { Outlet, useMatch, useNavigate } from 'react-router';

import { Tab } from '@mui/material';

import { RouterTabs, useLayoutTitle } from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import type { WiFiNetwork } from 'types';

import { WiFiConnectionContext } from './WiFiConnectionContext';

const Network = () => {
  const { LL } = useI18nContext();
  useLayoutTitle(LL.NETWORK(0));

  const routerTab = useMatch('/settings/network/:tab')?.pathname || false;

  const navigate = useNavigate();

  const [selectedNetwork, setSelectedNetwork] = useState<WiFiNetwork>();

  const selectNetwork = (network: WiFiNetwork) => {
    setSelectedNetwork(network);
    void navigate('/settings/network/settings');
  };

  const deselectNetwork = () => {
    setSelectedNetwork(undefined);
  };

  const contextValue = {
    ...(selectedNetwork && { selectedNetwork }),
    selectNetwork,
    deselectNetwork
  };

  return (
    <WiFiConnectionContext.Provider value={contextValue}>
      <RouterTabs value={routerTab}>
        <Tab
          value="/settings/network/settings"
          label={LL.SETTINGS_OF(LL.NETWORK(1))}
        />
        <Tab value="/settings/network/scan" label={LL.NETWORK_SCAN()} />
      </RouterTabs>
      <Outlet />
    </WiFiConnectionContext.Provider>
  );
};

export default memo(Network);
