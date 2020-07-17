import React, { Component } from 'react';

import {restController, RestControllerProps, RestFormLoader, SectionContent } from '../components';
import { MQTT_SETTINGS_ENDPOINT } from '../api';

import MqttSettingsForm from './MqttSettingsForm';
import { MqttSettings } from './types';

type MqttSettingsControllerProps = RestControllerProps<MqttSettings>;

class MqttSettingsController extends Component<MqttSettingsControllerProps> {

  componentDidMount() {
    this.props.loadData();
  }

  render() {
    return (
      <SectionContent title="MQTT Settings" titleGutter>
        <RestFormLoader
          {...this.props}
          render={formProps => <MqttSettingsForm {...formProps} />}
        />
      </SectionContent>
    )
  }

}

export default restController(MQTT_SETTINGS_ENDPOINT, MqttSettingsController);
