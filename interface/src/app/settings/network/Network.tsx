import { useCallback, useState } from 'react';
import {
  Navigate,
  Route,
  Routes,
  matchRoutes,
  useLocation,
  useNavigate
} from 'react-router';

import { Tab } from '@mui/material';

import { RouterTabs, useLayoutTitle } from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import type { WiFiNetwork } from 'types';

import NetworkSettings from './NetworkSettings';
import { WiFiConnectionContext } from './WiFiConnectionContext';
import WiFiNetworkScanner from './WiFiNetworkScanner';

const Network = () => {
  const { LL } = useI18nContext();
  useLayoutTitle(LL.NETWORK(0));

  // this also works!
  // const routerTab = useMatch(`settings/network/:path/*`)?.pathname || false;
  const matchedRoutes = matchRoutes(
    [
      {
        path: '/settings/network/settings',
        element: <NetworkSettings />,
        dog: 'woof'
      },
      { path: '/settings/network/scan', element: <WiFiNetworkScanner /> }
    ],
    useLocation()
  );
  const routerTab = matchedRoutes?.[0].route.path || false;

  const navigate = useNavigate();

  const [selectedNetwork, setSelectedNetwork] = useState<WiFiNetwork>();

  const selectNetwork = useCallback(
    (network: WiFiNetwork) => {
      setSelectedNetwork(network);
      void navigate('/settings/network/settings');
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
        <Tab
          value="/settings/network/settings"
          label={LL.SETTINGS_OF(LL.NETWORK(1))}
        />
        <Tab value="/settings/network/scan" label={LL.NETWORK_SCAN()} />
      </RouterTabs>
      <Routes>
        <Route path="scan" element={<WiFiNetworkScanner />} />
        <Route path="settings" element={<NetworkSettings />} />
        <Route
          path="*"
          element={<Navigate replace to="/settings/network/settings" />}
        />
      </Routes>
    </WiFiConnectionContext.Provider>
  );
};

export default Network;
