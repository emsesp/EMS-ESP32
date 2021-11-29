import { FC, useCallback, useContext, useState } from 'react';
import { Redirect, Route, Switch, useHistory, useRouteMatch } from 'react-router-dom';

import { Tab, Tabs } from '@mui/material';

import { AuthenticatedContext } from '../../contexts/authentication';
import { WiFiNetwork } from '../../types';

import { WiFiConnectionContext } from './WiFiConnectionContext';
import NetworkStatusForm from './NetworkStatusForm';
import WiFiNetworkScanner from './WiFiNetworkScanner';
import NetworkSettingsForm from './NetworkSettingsForm';
import { AdminRoute, useLayoutTitle } from '../../components';

const NetworkConnectionRouting: FC = () => {
  useLayoutTitle('Network Connection');

  const history = useHistory();
  const { url } = useRouteMatch();
  const authenticatedContext = useContext(AuthenticatedContext);
  const [selectedNetwork, setSelectedNetwork] = useState<WiFiNetwork>();

  const selectNetwork = useCallback(
    (network: WiFiNetwork) => {
      setSelectedNetwork(network);
      history.push('/network/settings');
    },
    [history]
  );

  const deselectNetwork = useCallback(() => {
    setSelectedNetwork(undefined);
  }, []);

  const handleTabChange = (event: React.ChangeEvent<{}>, path: string) => {
    history.push(path);
  };

  return (
    <WiFiConnectionContext.Provider
      value={{
        selectedNetwork,
        selectNetwork,
        deselectNetwork
      }}
    >
      <Tabs value={url} onChange={handleTabChange} variant="fullWidth">
        <Tab value="/network/status" label="Network Status" />
        <Tab value="/network/scan" label="Scan WiFi Networks" disabled={!authenticatedContext.me.admin} />
        <Tab value="/network/settings" label="Network Settings" disabled={!authenticatedContext.me.admin} />
      </Tabs>
      <Switch>
        <Route exact path="/network/status">
          <NetworkStatusForm />
        </Route>
        <AdminRoute exact path="/network/scan">
          <WiFiNetworkScanner />
        </AdminRoute>
        <AdminRoute exact path="/network/settings">
          <NetworkSettingsForm />
        </AdminRoute>
        <Redirect to="/network/status" />
      </Switch>
    </WiFiConnectionContext.Provider>
  );
};

const WiFiConnection: FC = () => (
  <Switch>
    <Route exact path={`/network/*`}>
      <NetworkConnectionRouting />
    </Route>
    <Redirect to="/network/status" />
  </Switch>
);

export default WiFiConnection;
