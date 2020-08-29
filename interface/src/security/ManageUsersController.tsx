import React, { Component } from 'react';

import {restController, RestControllerProps, RestFormLoader, SectionContent } from '../components';
import { SECURITY_SETTINGS_ENDPOINT } from '../api';

import ManageUsersForm from './ManageUsersForm';
import { SecuritySettings } from './types';

type ManageUsersControllerProps = RestControllerProps<SecuritySettings>;

class ManageUsersController extends Component<ManageUsersControllerProps> {

  componentDidMount() {
    this.props.loadData();
  }

  render() {
    return (
      <SectionContent title="Manage Users" titleGutter>
        <RestFormLoader
          {...this.props}
          render={formProps => <ManageUsersForm {...formProps} />}
        />
      </SectionContent>
    )
  }

}

export default restController(SECURITY_SETTINGS_ENDPOINT, ManageUsersController);
