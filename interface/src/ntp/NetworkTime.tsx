import React, { Component } from 'react';
import { Redirect, Switch, RouteComponentProps } from 'react-router-dom'

import { Tabs, Tab } from '@material-ui/core';

import { withAuthenticatedContext, AuthenticatedContextProps, AuthenticatedRoute } from '../authentication';
import { MenuAppBar } from '../components';

import NTPStatusController from './NTPStatusController';
import NTPSettingsController from './NTPSettingsController';

type NetworkTimeProps = AuthenticatedContextProps & RouteComponentProps;

class NetworkTime extends Component<NetworkTimeProps> {

  handleTabChange = (event: React.ChangeEvent<{}>, path: string) => {
    this.props.history.push(path);
  };

  render() {
    const { authenticatedContext } = this.props;
    return (
      <MenuAppBar sectionTitle="Network Time">
        <Tabs value={this.props.match.url} onChange={this.handleTabChange} variant="fullWidth">
          <Tab value="/ntp/status" label="NTP Status" />
          <Tab value="/ntp/settings" label="NTP Settings" disabled={!authenticatedContext.me.admin} />
        </Tabs>
        <Switch>
          <AuthenticatedRoute exact path="/ntp/status" component={NTPStatusController} />
          <AuthenticatedRoute exact path="/ntp/settings" component={NTPSettingsController} />
          <Redirect to="/ntp/status" />
        </Switch>
      </MenuAppBar>
    )
  }

}

export default withAuthenticatedContext(NetworkTime)
