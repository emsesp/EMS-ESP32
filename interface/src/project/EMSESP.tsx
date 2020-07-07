import React, { Component } from 'react';
import { Redirect, Switch, RouteComponentProps } from 'react-router-dom'

import { Tabs, Tab } from '@material-ui/core';

import { PROJECT_PATH } from '../api';
import { MenuAppBar } from '../components';
import { AuthenticatedRoute } from '../authentication';

import EMSESPSettingsController from './EMSESPSettingsController';
import EMSESPStatusController from './EMSESPStatusController';
import EMSESPDevicesController from './EMSESPDevicesController';

class EMSESP extends Component<RouteComponentProps> {

  handleTabChange = (event: React.ChangeEvent<{}>, path: string) => {
    this.props.history.push(path);
  };

  render() {
    return (
      <MenuAppBar sectionTitle="EMS-ESP">
        <Tabs value={this.props.match.url} onChange={this.handleTabChange} variant="fullWidth">
          <Tab value={`/${PROJECT_PATH}/status`} label="EMS-ESP Status" />
          <Tab value={`/${PROJECT_PATH}/devices`} label="EMS Devices" />
          <Tab value={`/${PROJECT_PATH}/settings`} label="EMS-ESP Settings" />
        </Tabs>
        <Switch>
          <AuthenticatedRoute exact path={`/${PROJECT_PATH}/status`} component={EMSESPStatusController} />
          <AuthenticatedRoute exact path={`/${PROJECT_PATH}/devices`} component={EMSESPDevicesController} />
          <AuthenticatedRoute exact path={`/${PROJECT_PATH}/settings`} component={EMSESPSettingsController} />
          <Redirect to={`/${PROJECT_PATH}/status`} />
        </Switch>
      </MenuAppBar>

    )
  }

}

export default EMSESP;
