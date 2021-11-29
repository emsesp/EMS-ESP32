import React, { FC } from 'react';
import { Redirect, Route, Switch, useHistory, useRouteMatch } from 'react-router-dom';

import { Tab, Tabs } from '@mui/material';

import { PROJECT_PATH } from '../api/env';
import { useLayoutTitle } from '../components';

import DashboardStatus from './DashboardStatus';
import DashboardData from './DashboardData';

const Dashboard: FC = () => {
  const history = useHistory();
  const { url } = useRouteMatch();

  useLayoutTitle('Dashboard');

  const handleTabChange = (event: React.ChangeEvent<{}>, path: string) => {
    history.push(path);
  };

  return (
    <>
      <Tabs value={url} onChange={handleTabChange} variant="fullWidth">
        <Tab value={`/${PROJECT_PATH}/dashboard/data`} label="Devices &amp; Sensors" />
        <Tab value={`/${PROJECT_PATH}/dashboard/status`} label="Bus Status" />
      </Tabs>
      <Switch>
        <Route exact path={`/${PROJECT_PATH}/dashboard/data`}>
          <DashboardData />
        </Route>
        <Route exact path={`/${PROJECT_PATH}/dashboard/status`}>
          <DashboardStatus />
        </Route>
        <Redirect to={`/${PROJECT_PATH}/dashboard/data`} />
      </Switch>
    </>
  );
};

export default Dashboard;
