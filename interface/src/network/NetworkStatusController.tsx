import { Component } from 'react';

import {
  restController,
  RestControllerProps,
  RestFormLoader,
  SectionContent
} from '../components';
import NetworkStatusForm from './NetworkStatusForm';
import { NETWORK_STATUS_ENDPOINT } from '../api';
import { NetworkStatus } from './types';

type NetworkStatusControllerProps = RestControllerProps<NetworkStatus>;

class NetworkStatusController extends Component<NetworkStatusControllerProps> {
  componentDidMount() {
    this.props.loadData();
  }

  render() {
    return (
      <SectionContent title="Network Status">
        <RestFormLoader
          {...this.props}
          render={(formProps) => <NetworkStatusForm {...formProps} />}
        />
      </SectionContent>
    );
  }
}

export default restController(NETWORK_STATUS_ENDPOINT, NetworkStatusController);
