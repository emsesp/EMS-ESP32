import React, { Component } from 'react';

import {restController, RestControllerProps, RestFormLoader, SectionContent } from '../components';
import WiFiSettingsForm from './WiFiSettingsForm';
import { WiFiConnectionContext } from './WiFiConnectionContext';
import { WIFI_SETTINGS_ENDPOINT } from '../api';
import { WiFiSettings } from './types';

type WiFiSettingsControllerProps = RestControllerProps<WiFiSettings>;

class WiFiSettingsController extends Component<WiFiSettingsControllerProps> {

  static contextType = WiFiConnectionContext;
  context!: React.ContextType<typeof WiFiConnectionContext>;

  componentDidMount() {
    const { selectedNetwork } = this.context;
    if (selectedNetwork) {
      const wifiSettings: WiFiSettings = {
        ssid: selectedNetwork.ssid,
        password: "",
        hostname: "esp8266-react",
        static_ip_config: false,
      }
      this.props.setData(wifiSettings);
    } else {
      this.props.loadData();
    }
  }

  deselectNetworkAndLoadData = () => {
    this.context.deselectNetwork();
    this.props.loadData();
  }

  componentWillUnmount() {
    this.context.deselectNetwork();
  }

  render() {
    return (
      <SectionContent title="WiFi Settings">
        <RestFormLoader
          {...this.props}
          loadData={this.deselectNetworkAndLoadData}
          render={formProps => <WiFiSettingsForm {...formProps} />}
        />
      </SectionContent>
    );
  }

}

export default restController(WIFI_SETTINGS_ENDPOINT, WiFiSettingsController);
