import { Component } from 'react';
import { Redirect, Switch, RouteComponentProps } from 'react-router-dom';

import { Tabs, Tab } from '@material-ui/core';

import {
  AuthenticatedContextProps,
  AuthenticatedRoute
} from '../authentication';
import { MenuAppBar } from '../components';

import ManageUsersController from './ManageUsersController';
import SecuritySettingsController from './SecuritySettingsController';

type SecurityProps = AuthenticatedContextProps & RouteComponentProps;

class Security extends Component<SecurityProps> {
  handleTabChange = (path: string) => {
    this.props.history.push(path);
  };

  render() {
    return (
      <MenuAppBar sectionTitle="Security">
        <Tabs
          value={this.props.match.url}
          onChange={(e, path) => this.handleTabChange(path)}
          variant="fullWidth"
        >
          <Tab value="/security/users" label="Manage Users" />
          <Tab value="/security/settings" label="Security Settings" />
        </Tabs>
        <Switch>
          <AuthenticatedRoute
            exact
            path="/security/users"
            component={ManageUsersController}
          />
          <AuthenticatedRoute
            exact
            path="/security/settings"
            component={SecuritySettingsController}
          />
          <Redirect to="/security/users" />
        </Switch>
      </MenuAppBar>
    );
  }
}

export default Security;
