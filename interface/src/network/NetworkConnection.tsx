import { Component } from 'react';
import { Redirect, Switch, RouteComponentProps } from 'react-router-dom';

import { Tabs, Tab } from '@material-ui/core';

import {
  withAuthenticatedContext,
  AuthenticatedContextProps,
  AuthenticatedRoute
} from '../authentication';
import { MenuAppBar } from '../components';

import NetworkStatusController from './NetworkStatusController';
import NetworkSettingsController from './NetworkSettingsController';
import WiFiNetworkScanner from './WiFiNetworkScanner';
import {
  NetworkConnectionContext,
  NetworkConnectionContextValue
} from './NetworkConnectionContext';

import { WiFiNetwork } from './types';

type NetworkConnectionProps = AuthenticatedContextProps & RouteComponentProps;

class NetworkConnection extends Component<
  NetworkConnectionProps,
  NetworkConnectionContextValue
> {
  constructor(props: NetworkConnectionProps) {
    super(props);
    this.state = {
      selectNetwork: this.selectNetwork,
      deselectNetwork: this.deselectNetwork
    };
  }

  selectNetwork = (network: WiFiNetwork) => {
    this.setState({ selectedNetwork: network });
    this.props.history.push('/network/settings');
  };

  deselectNetwork = () => {
    this.setState({ selectedNetwork: undefined });
  };

  handleTabChange = (path: string) => {
    this.props.history.push(path);
  };

  render() {
    const { authenticatedContext } = this.props;
    return (
      <NetworkConnectionContext.Provider value={this.state}>
        <MenuAppBar sectionTitle="Network Connection">
          <Tabs
            value={this.props.match.url}
            onChange={(e, path) => this.handleTabChange(path)}
            variant="fullWidth"
          >
            <Tab value="/network/status" label="Network Status" />
            <Tab
              value="/network/scan"
              label="Scan WiFi Networks"
              disabled={!authenticatedContext.me.admin}
            />
            <Tab
              value="/network/settings"
              label="Network Settings"
              disabled={!authenticatedContext.me.admin}
            />
          </Tabs>
          <Switch>
            <AuthenticatedRoute
              exact
              path="/network/status"
              component={NetworkStatusController}
            />
            <AuthenticatedRoute
              exact
              path="/network/scan"
              component={WiFiNetworkScanner}
            />
            <AuthenticatedRoute
              exact
              path="/network/settings"
              component={NetworkSettingsController}
            />
            <Redirect to="/network/status" />
          </Switch>
        </MenuAppBar>
      </NetworkConnectionContext.Provider>
    );
  }
}

export default withAuthenticatedContext(NetworkConnection);
