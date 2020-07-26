import React, { Component } from 'react';

import { restController, RestControllerProps, RestFormLoader, SectionContent } from '../components';
import { ENDPOINT_ROOT } from '../api';
import EMSESPDevicesForm from './EMSESPDevicesForm';
import { EMSESPDevices } from './EMSESPtypes';

export const EMSESP_DEVICES_ENDPOINT = ENDPOINT_ROOT + "allDevices";

type EMSESPDevicesControllerProps = RestControllerProps<EMSESPDevices>;

class EMSESPDevicesController extends Component<EMSESPDevicesControllerProps> {

    componentDidMount() {
        this.props.loadData();
    }

    render() {
        return (
            <SectionContent title="EMS Devices">
                <RestFormLoader
                    {...this.props}
                    render={formProps => <EMSESPDevicesForm {...formProps} />}
                />
            </SectionContent>
        )
    }
}

export default restController(EMSESP_DEVICES_ENDPOINT, EMSESPDevicesController);
