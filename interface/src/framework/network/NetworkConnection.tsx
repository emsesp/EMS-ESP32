import React, { FC, useCallback, useContext, useState } from 'react';
import { Navigate, Routes, Route, useNavigate } from 'react-router-dom';

import { Tab } from '@mui/material';

import { RequireAdmin, RouterTabs, useLayoutTitle, useRouterTab } from '../../components';
import { WiFiNetwork } from '../../types';
import { AuthenticatedContext } from '../../contexts/authentication';
import { WiFiConnectionContext } from './WiFiConnectionContext';
import NetworkStatusForm from './NetworkStatusForm';
import WiFiNetworkScanner from './WiFiNetworkScanner';
import NetworkSettingsForm from './NetworkSettingsForm';

import { useI18nContext } from '../../i18n/i18n-react';

const NetworkConnection: FC = () => {
  const { LL } = useI18nContext();
  useLayoutTitle(LL.NETWORK(0));

  const { routerTab } = useRouterTab();

  const authenticatedContext = useContext(AuthenticatedContext);
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
        <Tab value="status" label={LL.STATUS_OF(LL.NETWORK(1))} />
        <Tab value="scan" label={LL.NETWORK_SCAN()} disabled={!authenticatedContext.me.admin} />
        <Tab value="settings" label={LL.SETTINGS_OF(LL.NETWORK(1))} disabled={!authenticatedContext.me.admin} />
      </RouterTabs>
      <Routes>
        <Route path="status" element={<NetworkStatusForm />} />
        <Route
          path="scan"
          element={
            <RequireAdmin>
              <WiFiNetworkScanner />
            </RequireAdmin>
          }
        />
        <Route
          path="settings"
          element={
            <RequireAdmin>
              <NetworkSettingsForm />
            </RequireAdmin>
          }
        />
        <Route path="/*" element={<Navigate replace to="status" />} />
      </Routes>
    </WiFiConnectionContext.Provider>
  );
};

export default NetworkConnection;
