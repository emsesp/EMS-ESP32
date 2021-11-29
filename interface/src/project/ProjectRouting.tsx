import { FC } from 'react';
import { Redirect, Switch } from 'react-router';
import { Route } from 'react-router-dom';

import { PROJECT_PATH } from '../api/env';

import Dashboard from './Dashboard';
import Settings from './Settings';

const ProjectRouting: FC = () => {
  return (
    <Switch>
      <Route exact path={`/${PROJECT_PATH}/dashboard/*`}>
        <Dashboard />
      </Route>
      <Route exact path={`/${PROJECT_PATH}/dashboard`}>
        <Dashboard />
      </Route>
      <Route exact path={`/${PROJECT_PATH}/settings/*`}>
        <Settings />
      </Route>
      <Route exact path={`/${PROJECT_PATH}/settings`}>
        <Settings />
      </Route>
      {/*
       * The redirect below caters for the default project route and redirecting invalid paths.
       * The "to" property must match one of the routes above for this to work correctly.
       */}
      <Redirect to={`/${PROJECT_PATH}/dashboard/`} />
    </Switch>
  );
};

export default ProjectRouting;
