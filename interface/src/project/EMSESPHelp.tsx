import React, { Component } from 'react';
import { Typography, Box, List, ListItem, ListItemText, Link, ListItemAvatar } from '@material-ui/core';
import { SectionContent } from '../components';

import CommentIcon from "@material-ui/icons/CommentTwoTone";
import MenuBookIcon from "@material-ui/icons/MenuBookTwoTone";
import GitHubIcon from "@material-ui/icons/GitHub";
import StarIcon from "@material-ui/icons/Star";
import ImportExportIcon from "@material-ui/icons/ImportExport";
import BugReportIcon from "@material-ui/icons/BugReportTwoTone";

export const WebAPISystemSettings = window.location.origin + "/api?device=system&cmd=settings";
export const WebAPISystemInfo = window.location.origin + "/api?device=system&cmd=info";

class EMSESPHelp extends Component {

    render() {
        return (
            <SectionContent title='EMS-ESP Help' titleGutter>

                <List>

                    <ListItem>
                        <ListItemAvatar>
                            <MenuBookIcon />
                        </ListItemAvatar>
                        <ListItemText>
                            For the latest news and updates go to the <Link href="https://emsesp.github.io/docs" color="primary">{'official documentation'}&nbsp;website</Link>
                        </ListItemText>
                    </ListItem>

                    <ListItem>
                        <ListItemAvatar>
                            <CommentIcon />
                        </ListItemAvatar>
                        <ListItemText>
                            For live community chat join our <Link href="https://discord.gg/3J3GgnzpyT" color="primary">{'Discord'}&nbsp;server</Link>
                        </ListItemText>
                    </ListItem>

                    <ListItem>
                        <ListItemAvatar>
                            <GitHubIcon />
                        </ListItemAvatar>
                        <ListItemText>
                            To report an issue or feature request go to <Link href="https://github.com/proddy/EMS-ESP/issues/new/choose" color="primary">{'click here'}</Link>
                        </ListItemText>
                    </ListItem>

                    <ListItem>
                        <ListItemAvatar>
                            <ImportExportIcon />
                        </ListItemAvatar>
                        <ListItemText>
                            To list your system settings <Link target="_blank" href={WebAPISystemSettings} color="primary">{'click here'}</Link>
                        </ListItemText>
                    </ListItem>


                    <ListItem>
                        <ListItemAvatar>
                            <BugReportIcon />
                        </ListItemAvatar>
                        <ListItemText>
                            To create a report of the current EMS-ESP status <Link target="_blank" href={WebAPISystemInfo} color="primary">{'click here'}</Link>
                        </ListItemText>
                    </ListItem>

                </List>

                <Box bgcolor="info.main" border={1} p={3} mt={1} mb={0}>
                    <Typography variant="h6">
                        EMS-ESP is free and open-source.
                        <br></br>Please consider supporting this project by giving it a <StarIcon style={{ color: '#fdff3a' }} /> on our <Link href="https://github.com/proddy/EMS-ESP" color="primary">{'GitHub page'}</Link>.
                    </Typography>
                </Box>
                <br></br>

            </SectionContent>
        )
    }

}

export default EMSESPHelp;
