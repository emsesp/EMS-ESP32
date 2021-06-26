import { Component } from 'react';

import { ENDPOINT_ROOT } from '../api';
import EMSESPSettingsForm from './EMSESPSettingsForm';

import {
  restController,
  RestControllerProps,
  RestFormLoader,
  SectionContent
} from '../components';

import { EMSESPSettings } from './EMSESPtypes';

export const EMSESP_SETTINGS_ENDPOINT = ENDPOINT_ROOT + 'emsespSettings';

type EMSESPSettingsControllerProps = RestControllerProps<EMSESPSettings>;

class EMSESPSettingsController extends Component<EMSESPSettingsControllerProps> {
  componentDidMount() {
    this.props.loadData();
  }

  render() {
    return (
      <SectionContent title="" titleGutter>
        <RestFormLoader
          {...this.props}
          render={(formProps) => <EMSESPSettingsForm {...formProps} />}
        />
      </SectionContent>
    );
  }
}

export default restController(
  EMSESP_SETTINGS_ENDPOINT,
  EMSESPSettingsController
);
