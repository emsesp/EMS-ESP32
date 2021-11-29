import React, { FC } from 'react';
import { Redirect, Route, Switch, useHistory, useRouteMatch } from 'react-router-dom';

import { Tab, Tabs } from '@mui/material';

import { useLayoutTitle } from '../../components';
import SecuritySettingsForm from './SecuritySettingsForm';
import ManageUsersForm from './ManageUsersForm';

const SecurityRouting: FC = () => {
  useLayoutTitle('Security');

  const history = useHistory();
  const { url } = useRouteMatch();

  const handleTabChange = (event: React.ChangeEvent<{}>, path: string) => {
    history.push(path);
  };

  return (
    <>
      <Tabs value={url} onChange={handleTabChange} variant="fullWidth">
        <Tab value="/security/users" label="Manage Users" />
        <Tab value="/security/settings" label="Security Settings" />
      </Tabs>
      <Switch>
        <Route exact path="/security/users">
          <ManageUsersForm />
        </Route>
        <Route exact path="/security/settings">
          <SecuritySettingsForm />
        </Route>
        <Redirect to="/security/users" />
      </Switch>
    </>
  );
};

const Security: FC = () => (
  <Switch>
    <Route exact path={`/security/*`}>
      <SecurityRouting />
    </Route>
    <Redirect to="/security/users" />
  </Switch>
);

export default Security;
