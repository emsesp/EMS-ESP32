import React, { Component } from 'react';

import {restController, RestControllerProps, RestFormLoader, SectionContent } from '../components';
import { AP_STATUS_ENDPOINT } from '../api';

import APStatusForm from './APStatusForm';
import { APStatus } from './types';

type APStatusControllerProps = RestControllerProps<APStatus>;

class APStatusController extends Component<APStatusControllerProps> {

  componentDidMount() {
    this.props.loadData();
  }

  render() {
    return (
      <SectionContent title="Access Point Status">
        <RestFormLoader
          {...this.props}
          render={formProps => <APStatusForm {...formProps} />}
        />
      </SectionContent>
    )
  }
}

export default restController(AP_STATUS_ENDPOINT, APStatusController);
