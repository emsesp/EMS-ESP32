import React, { Component } from 'react';
import { Typography, Box, Link } from '@material-ui/core';
import { SectionContent } from '../components';

class EMSESPHelp extends Component {

    render() {
        return (
            <SectionContent title='EMS-ESP Help' titleGutter>

                <Box bgcolor="info.main" border={1} p={3} mt={1} mb={0}>
                    <Typography variant="body1">
                        EMS-ESP is an open-source firmware to communicate with heating devices that support the EMS protocol, such as equipment from Bosch, Junkers, Nefit, Buderus and Worcester.
            <p></p>
            Please consider supporting this project via the GitHub page <Link href="https://github.com/proddy/EMS-ESP" color="primary">{'http://github.com/proddy/EMS-ESP'}</Link>.
                    </Typography>
                </Box>
                <br></br>
                <Typography variant="body1" paragraph>
                    Check for news and updates on the <Link href="https://emsesp.github.io/docs/#/" color="primary">{'Wiki'}</Link>.
                </Typography>
                <Typography variant="body1" paragraph>
                    For live community chat go to <Link href="https://gitter.im/EMS-ESP/community#" color="primary">{'Gitter'}</Link>.
                </Typography>
                <Typography variant="body1" paragraph>
                    To report an issue or feature request go to <Link href="https://github.com/proddy/EMS-ESP/issues/new/choose" color="primary">{'the github project page'}</Link>.
                </Typography>

            </SectionContent>
        )
    }

}

export default EMSESPHelp;
