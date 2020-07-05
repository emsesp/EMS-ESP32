import React, { Component } from 'react';

import {restController, RestControllerProps, RestFormLoader, SectionContent } from '../components';
import { MQTT_STATUS_ENDPOINT } from '../api';

import MqttStatusForm from './MqttStatusForm';
import { MqttStatus } from './types';

type MqttStatusControllerProps = RestControllerProps<MqttStatus>;

class MqttStatusController extends Component<MqttStatusControllerProps> {

  componentDidMount() {
    this.props.loadData();
  }

  render() {
    return (
      <SectionContent title="MQTT Status">
        <RestFormLoader
          {...this.props}
          render={formProps => <MqttStatusForm {...formProps} />}
        />
      </SectionContent>
    )
  }
}

export default restController(MQTT_STATUS_ENDPOINT, MqttStatusController);
