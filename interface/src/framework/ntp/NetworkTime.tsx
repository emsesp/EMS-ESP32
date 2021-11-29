import React, { FC, useContext } from 'react';
import { Redirect, Route, Switch, useHistory, useRouteMatch } from 'react-router-dom';

import { Tab, Tabs } from '@mui/material';

import { AuthenticatedContext } from '../../contexts/authentication';
import NTPStatusForm from './NTPStatusForm';
import NTPSettingsForm from './NTPSettingsForm';
import { AdminRoute, useLayoutTitle } from '../../components';

const NetworkTimeRouting: FC = () => {
  useLayoutTitle('Network Time');

  const history = useHistory();
  const { url } = useRouteMatch();
  const authenticatedContext = useContext(AuthenticatedContext);

  const handleTabChange = (event: React.ChangeEvent<{}>, path: string) => {
    history.push(path);
  };

  return (
    <>
      <Tabs value={url} onChange={handleTabChange} variant="fullWidth">
        <Tab value="/ntp/status" label="NTP Status" />
        <Tab value="/ntp/settings" label="NTP Settings" disabled={!authenticatedContext.me.admin} />
      </Tabs>
      <Switch>
        <Route exact path="/ntp/status">
          <NTPStatusForm />
        </Route>
        <AdminRoute exact path="/ntp/settings">
          <NTPSettingsForm />
        </AdminRoute>
        <Redirect to="/ntp/status" />
      </Switch>
    </>
  );
};

const NetworkTime: FC = () => (
  <Switch>
    <Route exact path={`/ntp/*`}>
      <NetworkTimeRouting />
    </Route>
    <Redirect to="/ntp/status" />
  </Switch>
);

export default NetworkTime;
