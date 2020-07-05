import React, { Component } from 'react';

import {restController, RestControllerProps, RestFormLoader, SectionContent } from '../components';
import { NTP_SETTINGS_ENDPOINT } from '../api';

import NTPSettingsForm from './NTPSettingsForm';
import { NTPSettings } from './types';

type NTPSettingsControllerProps = RestControllerProps<NTPSettings>;

class NTPSettingsController extends Component<NTPSettingsControllerProps> {

  componentDidMount() {
    this.props.loadData();
  }

  render() {
    return (
      <SectionContent title="NTP Settings" titleGutter>
        <RestFormLoader
          {...this.props}
          render={formProps => <NTPSettingsForm {...formProps} />}
        />
      </SectionContent>
    )
  }

}

export default restController(NTP_SETTINGS_ENDPOINT, NTPSettingsController);
