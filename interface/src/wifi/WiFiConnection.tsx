import React, { Component } from 'react';
import { Redirect, Switch, RouteComponentProps } from 'react-router-dom'

import { Tabs, Tab } from '@material-ui/core';

import { withAuthenticatedContext, AuthenticatedContextProps, AuthenticatedRoute } from '../authentication';
import { MenuAppBar } from '../components';

import WiFiStatusController from './WiFiStatusController';
import WiFiSettingsController from './WiFiSettingsController';
import WiFiNetworkScanner from './WiFiNetworkScanner';
import { WiFiConnectionContext } from './WiFiConnectionContext';
import { WiFiNetwork } from './types';

type WiFiConnectionProps = AuthenticatedContextProps & RouteComponentProps;

class WiFiConnection extends Component<WiFiConnectionProps, WiFiConnectionContext> {

  constructor(props: WiFiConnectionProps) {
    super(props);
    this.state = {
      selectNetwork: this.selectNetwork,
      deselectNetwork: this.deselectNetwork
    };
  }

  selectNetwork = (network: WiFiNetwork) => {
    this.setState({ selectedNetwork: network });
    this.props.history.push('/wifi/settings');
  }

  deselectNetwork = () => {
    this.setState({ selectedNetwork: undefined });
  }

  handleTabChange = (event: React.ChangeEvent<{}>, path: string) => {
    this.props.history.push(path);
  };

  render() {
    const { authenticatedContext } = this.props;
    return (
      <WiFiConnectionContext.Provider value={this.state}>
        <MenuAppBar sectionTitle="WiFi Connection">
          <Tabs value={this.props.match.url} onChange={this.handleTabChange} variant="fullWidth">
            <Tab value="/wifi/status" label="WiFi Status" />
            <Tab value="/wifi/scan" label="Scan Networks" disabled={!authenticatedContext.me.admin} />
            <Tab value="/wifi/settings" label="WiFi Settings" disabled={!authenticatedContext.me.admin} />
          </Tabs>
          <Switch>
            <AuthenticatedRoute exact path="/wifi/status" component={WiFiStatusController} />
            <AuthenticatedRoute exact path="/wifi/scan" component={WiFiNetworkScanner} />
            <AuthenticatedRoute exact path="/wifi/settings" component={WiFiSettingsController} />
            <Redirect to="/wifi/status" />
          </Switch>
        </MenuAppBar>
      </WiFiConnectionContext.Provider>
    )
  }
}

export default withAuthenticatedContext(WiFiConnection);
