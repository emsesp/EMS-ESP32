import React, { FC } from 'react';
import { Redirect, Route, Switch, useHistory, useRouteMatch } from 'react-router-dom';

import { Tab, Tabs } from '@mui/material';

import { PROJECT_PATH } from '../api/env';

import { AdminRoute, useLayoutTitle } from '../components';

import SettingsApplication from './SettingsApplication';
import SettingsEntities from './SettingsEntities';
import SettingsHelp from './SettingsHelp';

const Settings: FC = () => {
  const history = useHistory();
  const { url } = useRouteMatch();

  useLayoutTitle('Settings');

  const handleTabChange = (event: React.ChangeEvent<{}>, path: string) => {
    history.push(path);
  };

  return (
    <>
      <Tabs value={url} onChange={handleTabChange} variant="fullWidth">
        <Tab value={`/${PROJECT_PATH}/settings/application`} label="Application Settings" />
        <Tab value={`/${PROJECT_PATH}/settings/entities`} label="Device Entities" />
        <Tab value={`/${PROJECT_PATH}/settings/help`} label="Information" />
      </Tabs>
      <Switch>
        <AdminRoute exact path={`/${PROJECT_PATH}/settings/application`}>
          <SettingsApplication />
        </AdminRoute>
        <AdminRoute exact path={`/${PROJECT_PATH}/settings/entities`}>
          <SettingsEntities />
        </AdminRoute>
        <Route exact path={`/${PROJECT_PATH}/settings/help`}>
          <SettingsHelp />
        </Route>
        <Redirect to={`/${PROJECT_PATH}/settings/application`} />
      </Switch>
    </>
  );
};

export default Settings;
