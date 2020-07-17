import React, { Component } from 'react';

import { AP_SETTINGS_ENDPOINT } from '../api';
import {restController, RestControllerProps, RestFormLoader, SectionContent } from '../components';

import APSettingsForm from './APSettingsForm';
import { APSettings } from './types';

type APSettingsControllerProps = RestControllerProps<APSettings>;

class APSettingsController extends Component<APSettingsControllerProps> {

  componentDidMount() {
    this.props.loadData();
  }

  render() {
    return (
      <SectionContent title="Access Point Settings" titleGutter>
        <RestFormLoader
          {...this.props}
          render={formProps => <APSettingsForm {...formProps} />}
        />
      </SectionContent>
    )
  }

}

export default restController(AP_SETTINGS_ENDPOINT, APSettingsController);
