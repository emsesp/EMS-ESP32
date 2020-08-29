import React, { Component } from 'react';

import {restController, RestControllerProps, RestFormLoader, SectionContent } from '../components';
import { SYSTEM_STATUS_ENDPOINT } from '../api';

import SystemStatusForm from './SystemStatusForm';
import { SystemStatus } from './types';

type SystemStatusControllerProps = RestControllerProps<SystemStatus>;

class SystemStatusController extends Component<SystemStatusControllerProps> {

  componentDidMount() {
    this.props.loadData();
  }

  render() {
    return (
      <SectionContent title="System Status">
        <RestFormLoader
          {...this.props}
          render={formProps => <SystemStatusForm {...formProps} />}
        />
      </SectionContent>
    );
  }

}

export default restController(SYSTEM_STATUS_ENDPOINT, SystemStatusController);
