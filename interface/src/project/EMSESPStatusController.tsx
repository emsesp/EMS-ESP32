import React, { Component } from 'react';

import { restController, RestControllerProps, RestFormLoader, SectionContent } from '../components';
import { ENDPOINT_ROOT } from '../api';
import EMSESPStatusForm from './EMSESPStatusForm';
import { EMSESPStatus } from './EMSESPtypes';

export const EMSESP_STATUS_ENDPOINT = ENDPOINT_ROOT + "emsespStatus";

type EMSESPStatusControllerProps = RestControllerProps<EMSESPStatus>;

class EMSESPStatusController extends Component<EMSESPStatusControllerProps> {

    componentDidMount() {
        this.props.loadData();
    }

    render() {
        return (
            <SectionContent title="EMS Status">
                <RestFormLoader
                    {...this.props}
                    render={formProps => <EMSESPStatusForm {...formProps} />}
                />
            </SectionContent>
        )
    }
}

export default restController(EMSESP_STATUS_ENDPOINT, EMSESPStatusController);
