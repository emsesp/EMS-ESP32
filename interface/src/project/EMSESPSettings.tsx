import { Component } from 'react';
import { Redirect, Switch, RouteComponentProps } from 'react-router-dom';

import { Tabs, Tab } from '@material-ui/core';

import { PROJECT_PATH } from '../api';
import { MenuAppBar } from '../components';
import { AuthenticatedRoute } from '../authentication';

import EMSESPSettingsController from './EMSESPSettingsController';

class EMSESP extends Component<RouteComponentProps> {
  handleTabChange = (path: string) => {
    this.props.history.push(path);
  };

  render() {
    return (
      <MenuAppBar sectionTitle="Settings">
        <Tabs
          value={this.props.match.url}
          onChange={(e, path) => this.handleTabChange(path)}
          variant="fullWidth"
        >
          <Tab value={`/${PROJECT_PATH}/settings`} label="EMS-ESP Settings" />
        </Tabs>
        <Switch>
          <AuthenticatedRoute
            exact
            path={`/${PROJECT_PATH}/settings`}
            component={EMSESPSettingsController}
          />
          <Redirect to={`/${PROJECT_PATH}/settings`} />
        </Switch>
      </MenuAppBar>
    );
  }
}

export default EMSESP;
