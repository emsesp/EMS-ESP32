import React, { Component } from 'react';
import { Redirect, Switch, RouteComponentProps } from 'react-router-dom'

import { Tabs, Tab } from '@material-ui/core';

import { PROJECT_PATH } from '../api';
import { MenuAppBar } from '../components';
import { AuthenticatedRoute } from '../authentication';

import EMSESPStatusController from './EMSESPStatusController';
import EMSESPDevicesController from './EMSESPDevicesController';
import EMSESPHelp from './EMSESPHelp';

class EMSESP extends Component<RouteComponentProps> {

  handleTabChange = (event: React.ChangeEvent<{}>, path: string) => {
    this.props.history.push(path);
  };

  render() {
    return (
      <MenuAppBar sectionTitle="Dashboard">
        <Tabs value={this.props.match.url} onChange={this.handleTabChange} variant="fullWidth">
          <Tab value={`/${PROJECT_PATH}/status`} label="EMS Status" />
          <Tab value={`/${PROJECT_PATH}/devices`} label="EMS Devices" />
          <Tab value={`/${PROJECT_PATH}/help`} label="EMS-ESP Help" />
        </Tabs>
        <Switch>
          <AuthenticatedRoute exact path={`/${PROJECT_PATH}/status`} component={EMSESPStatusController} />
          <AuthenticatedRoute exact path={`/${PROJECT_PATH}/devices`} component={EMSESPDevicesController} />
          <AuthenticatedRoute exact path={`/${PROJECT_PATH}/help`} component={EMSESPHelp} />
          <Redirect to={`/${PROJECT_PATH}/status`} />
        </Switch>
      </MenuAppBar>

    )
  }

}

export default EMSESP;
