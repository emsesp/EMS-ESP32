import React, { Component } from 'react';

import { restController, RestControllerProps, RestFormLoader, SectionContent } from '../components';
import NetworkSettingsForm from './NetworkSettingsForm';
import { NETWORK_SETTINGS_ENDPOINT } from '../api';
import { NetworkSettings } from './types';

type NetworkSettingsControllerProps = RestControllerProps<NetworkSettings>;

class NetworkSettingsController extends Component<NetworkSettingsControllerProps> {

  componentDidMount() {
    this.props.loadData();
  }

  render() {
    return (
      <SectionContent title="Network Settings">
        <RestFormLoader
          {...this.props}
          render={formProps => <NetworkSettingsForm {...formProps} />}
        />
      </SectionContent>
    );
  }

}

export default restController(NETWORK_SETTINGS_ENDPOINT, NetworkSettingsController);