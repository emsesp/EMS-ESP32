import React, { FC, useContext } from 'react';
import { Redirect, Route, Switch, useHistory, useRouteMatch } from 'react-router-dom';

import { Tab, Tabs } from '@mui/material';

import { AuthenticatedContext } from '../../contexts/authentication';
import APStatusForm from './APStatusForm';
import APSettingsForm from './APSettingsForm';
import { AdminRoute, useLayoutTitle } from '../../components';

const AccessPointRouting: FC = () => {
  useLayoutTitle('Access Point');

  const history = useHistory();
  const { url } = useRouteMatch();
  const authenticatedContext = useContext(AuthenticatedContext);

  const handleTabChange = (event: React.ChangeEvent<{}>, path: string) => {
    history.push(path);
  };

  return (
    <>
      <Tabs value={url} onChange={handleTabChange} variant="fullWidth">
        <Tab value="/ap/status" label="Access Point Status" />
        <Tab value="/ap/settings" label="Access Point Settings" disabled={!authenticatedContext.me.admin} />
      </Tabs>
      <Switch>
        <Route exact path="/ap/status">
          <APStatusForm />
        </Route>
        <AdminRoute exact path="/ap/settings">
          <APSettingsForm />
        </AdminRoute>
        <Redirect to="/ap/status" />
      </Switch>
    </>
  );
};

const AccessPoint: FC = () => (
  <Switch>
    <Route exact path={`/ap/*`}>
      <AccessPointRouting />
    </Route>
    <Redirect to="/ap/status" />
  </Switch>
);

export default AccessPoint;
