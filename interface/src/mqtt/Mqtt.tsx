import React, { Component } from 'react';
import { Redirect, Switch, RouteComponentProps } from 'react-router-dom'

import { Tabs, Tab } from '@material-ui/core';

import { AuthenticatedContextProps, withAuthenticatedContext, AuthenticatedRoute } from '../authentication';
import { MenuAppBar } from '../components';
import MqttStatusController from './MqttStatusController';
import MqttSettingsController from './MqttSettingsController';

type MqttProps = AuthenticatedContextProps & RouteComponentProps;

class Mqtt extends Component<MqttProps> {

  handleTabChange = (event: React.ChangeEvent<{}>, path: string) => {
    this.props.history.push(path);
  };

  render() {
    const { authenticatedContext } = this.props;
    return (
      <MenuAppBar sectionTitle="MQTT">
        <Tabs value={this.props.match.url} onChange={this.handleTabChange} variant="fullWidth">
          <Tab value="/mqtt/status" label="MQTT Status" />
          <Tab value="/mqtt/settings" label="MQTT Settings" disabled={!authenticatedContext.me.admin} />
        </Tabs>
        <Switch>
          <AuthenticatedRoute exact path="/mqtt/status" component={MqttStatusController} />
          <AuthenticatedRoute exact path="/mqtt/settings" component={MqttSettingsController} />
          <Redirect to="/mqtt/status" />
        </Switch>
      </MenuAppBar>
    )
  }
}

export default withAuthenticatedContext(Mqtt);
