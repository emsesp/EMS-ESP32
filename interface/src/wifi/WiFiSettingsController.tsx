import React, { Component } from 'react';

import { restController, RestControllerProps, RestFormLoader, SectionContent } from '../components';
import WiFiSettingsForm from './WiFiSettingsForm';
import { WIFI_SETTINGS_ENDPOINT } from '../api';
import { WiFiSettings } from './types';

type WiFiSettingsControllerProps = RestControllerProps<WiFiSettings>;

class WiFiSettingsController extends Component<WiFiSettingsControllerProps> {

  componentDidMount() {
    this.props.loadData();
  }

  render() {
    return (
      <SectionContent title="WiFi Settings">
        <RestFormLoader
          {...this.props}
          render={formProps => <WiFiSettingsForm {...formProps} />}
        />
      </SectionContent>
    );
  }

}

export default restController(WIFI_SETTINGS_ENDPOINT, WiFiSettingsController);