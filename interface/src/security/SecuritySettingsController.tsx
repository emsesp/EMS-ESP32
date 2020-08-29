import React, { Component } from 'react';

import {restController, RestControllerProps, RestFormLoader, SectionContent } from '../components';
import { SECURITY_SETTINGS_ENDPOINT } from '../api';

import SecuritySettingsForm from './SecuritySettingsForm';
import { SecuritySettings } from './types';

type SecuritySettingsControllerProps = RestControllerProps<SecuritySettings>;

class SecuritySettingsController extends Component<SecuritySettingsControllerProps> {

  componentDidMount() {
    this.props.loadData();
  }

  render() {
    return (
      <SectionContent title="Security Settings" titleGutter>
        <RestFormLoader
          {...this.props}
          render={formProps => <SecuritySettingsForm {...formProps} />}
        />
      </SectionContent>
    );
  }

}

export default restController(SECURITY_SETTINGS_ENDPOINT, SecuritySettingsController);
