import React, { Component } from 'react';

import {
  restController,
  RestControllerProps,
  RestFormLoader,
  SectionContent
} from '../components';

import { ENDPOINT_ROOT } from '../api';
import EMSESPDataForm from './EMSESPDataForm';
import { EMSESPData } from './EMSESPtypes';

export const EMSESP_DATA_ENDPOINT = ENDPOINT_ROOT + 'data';

type EMSESPDataControllerProps = RestControllerProps<EMSESPData>;

class EMSESPDataController extends Component<EMSESPDataControllerProps> {
  componentDidMount() {
    this.props.loadData();
  }

  render() {
    return (
      <SectionContent title="Devices &amp; Sensors">
        <RestFormLoader
          {...this.props}
          render={(formProps) => <EMSESPDataForm {...formProps} />}
        />
      </SectionContent>
    );
  }
}

export default restController(EMSESP_DATA_ENDPOINT, EMSESPDataController);
