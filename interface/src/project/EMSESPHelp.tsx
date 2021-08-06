import { Component } from 'react';
import {
  Typography,
  Box,
  List,
  ListItem,
  ListItemText,
  Link,
  ListItemAvatar
} from '@material-ui/core';
import { SectionContent } from '../components';

import CommentIcon from '@material-ui/icons/CommentTwoTone';
import MenuBookIcon from '@material-ui/icons/MenuBookTwoTone';
import GitHubIcon from '@material-ui/icons/GitHub';
import StarIcon from '@material-ui/icons/Star';
import DownloadIcon from '@material-ui/icons/GetApp';

import { FormButton } from '../components';

import { API_ENDPOINT_ROOT } from '../api';

import { redirectingAuthorizedFetch } from '../authentication';

class EMSESPHelp extends Component {
  onDownload = (endpoint: string) => {
    redirectingAuthorizedFetch(API_ENDPOINT_ROOT + 'system/' + endpoint)
      .then((response) => {
        if (response.status === 200) {
          return response.json();
        }
        throw Error(
          'Device returned unexpected response code: ' + response.status
        );
      })
      .then((json) => {
        const a = document.createElement('a');
        const filename = 'emsesp_system_' + endpoint + '.txt';
        a.href = URL.createObjectURL(
          new Blob([JSON.stringify(json, null, 2)], {
            type: 'text/plain'
          })
        );
        a.setAttribute('download', filename);
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
      });
  };

  render() {
    return (
      <SectionContent title="EMS-ESP Help" titleGutter>
        <List>
          <ListItem>
            <ListItemAvatar>
              <MenuBookIcon />
            </ListItemAvatar>
            <ListItemText>
              For help and information on the latest updates visit the{' '}
              <Link href="https://emsesp.github.io/docs" color="primary">
                {'online documentation'}
              </Link>
            </ListItemText>
          </ListItem>

          <ListItem>
            <ListItemAvatar>
              <CommentIcon />
            </ListItemAvatar>
            <ListItemText>
              For live community chat join our{' '}
              <Link href="https://discord.gg/3J3GgnzpyT" color="primary">
                {'Discord'}&nbsp;server
              </Link>
            </ListItemText>
          </ListItem>

          <ListItem>
            <ListItemAvatar>
              <GitHubIcon />
            </ListItemAvatar>
            <ListItemText>
              To report an issue or request a feature go to{' '}
              <Link
                href="https://github.com/emsesp/EMS-ESP32/issues/new/choose"
                color="primary"
              >
                {'GitHub'}
              </Link>
            </ListItemText>
          </ListItem>
        </List>

        <Box flexWrap="none" padding={1} whiteSpace="nowrap">
          <FormButton
            startIcon={<DownloadIcon />}
            variant="contained"
            color="primary"
            onClick={() => this.onDownload('info')}
          >
            download system info
          </FormButton>
          <FormButton
            startIcon={<DownloadIcon />}
            variant="contained"
            color="primary"
            onClick={() => this.onDownload('settings')}
          >
            download all settings
          </FormButton>
        </Box>

        <Box bgcolor="info.main" border={1} p={3} mt={1} mb={0}>
          <Typography variant="h6">
            EMS-ESP is free and open-source.
            <br></br>Please consider supporting this project by giving it a{' '}
            <StarIcon style={{ color: '#fdff3a' }} /> on our{' '}
            <Link href="https://github.com/emsesp/EMS-ESP32" color="primary">
              {'GitHub page'}
            </Link>
            .
          </Typography>
        </Box>
        <br></br>
      </SectionContent>
    );
  }
}

export default EMSESPHelp;
