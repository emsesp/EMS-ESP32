import { Component } from 'react';
import { Redirect, Switch, RouteComponentProps } from 'react-router-dom';

import { Tabs, Tab } from '@material-ui/core';

import { PROJECT_PATH } from '../api';
import { MenuAppBar } from '../components';
import { AuthenticatedRoute } from '../authentication';

import EMSESPStatusController from './EMSESPStatusController';
import EMSESPDataController from './EMSESPDataController';
import EMSESPHelp from './EMSESPHelp';

class EMSESP extends Component<RouteComponentProps> {
  handleTabChange = (path: string) => {
    this.props.history.push(path);
  };

  render() {
    return (
      <MenuAppBar sectionTitle="Dashboard">
        <Tabs
          value={this.props.match.url}
          onChange={(e, path) => this.handleTabChange(path)}
          variant="fullWidth"
        >
          <Tab value={`/${PROJECT_PATH}/data`} label="Devices &amp; Sensors" />
          <Tab value={`/${PROJECT_PATH}/status`} label="EMS Status" />
          <Tab value={`/${PROJECT_PATH}/help`} label="EMS-ESP Help" />
        </Tabs>
        <Switch>
          <AuthenticatedRoute
            exact
            path={`/${PROJECT_PATH}/data`}
            component={EMSESPDataController}
          />
          <AuthenticatedRoute
            exact
            path={`/${PROJECT_PATH}/status`}
            component={EMSESPStatusController}
          />
          <AuthenticatedRoute
            exact
            path={`/${PROJECT_PATH}/help`}
            component={EMSESPHelp}
          />
          <Redirect to={`/${PROJECT_PATH}/data`} />
        </Switch>
      </MenuAppBar>
    );
  }
}

export default EMSESP;
