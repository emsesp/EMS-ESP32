import { FC, useContext } from 'react';

import { Typography, Button, Box, List, ListItem, ListItemText, Link, ListItemAvatar } from '@mui/material';

import { SectionContent, ButtonRow, MessageBox } from '../components';

import { AuthenticatedContext } from '../contexts/authentication';

import { useSnackbar } from 'notistack';

import CommentIcon from '@mui/icons-material/CommentTwoTone';
import MenuBookIcon from '@mui/icons-material/MenuBookTwoTone';
import GitHubIcon from '@mui/icons-material/GitHub';
import StarIcon from '@mui/icons-material/Star';
import DownloadIcon from '@mui/icons-material/GetApp';
import TuneIcon from '@mui/icons-material/Tune';

import { extractErrorMessage } from '../utils';

import * as EMSESP from './api';

const HelpInformation: FC = () => {
  const { enqueueSnackbar } = useSnackbar();

  const { me } = useContext(AuthenticatedContext);

  const saveFile = (json: any, endpoint: string) => {
    const a = document.createElement('a');
    const filename = 'emsesp_' + endpoint + '.json';
    a.href = URL.createObjectURL(
      new Blob([JSON.stringify(json, null, 2)], {
        type: 'text/plain'
      })
    );
    a.setAttribute('download', filename);
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    enqueueSnackbar('File downloaded', { variant: 'info' });
  };

  const callAPI = async (endpoint: string) => {
    try {
      const response = await EMSESP.API({
        device: 'system',
        entity: endpoint,
        id: 0
      });
      if (response.status !== 200) {
        enqueueSnackbar('API call failed', { variant: 'error' });
      } else {
        saveFile(response.data, endpoint);
      }
    } catch (error: unknown) {
      enqueueSnackbar(extractErrorMessage(error, 'Problem with downloading'), { variant: 'error' });
    }
  };

  const downloadSettings = async () => {
    try {
      const response = await EMSESP.getSettings();
      if (response.status !== 200) {
        enqueueSnackbar('Unable to get settings', { variant: 'error' });
      } else {
        saveFile(response.data, 'settings');
      }
    } catch (error: unknown) {
      enqueueSnackbar(extractErrorMessage(error, 'Problem with downloading'), { variant: 'error' });
    }
  };

  const downloadCustomizations = async () => {
    try {
      const response = await EMSESP.getCustomizations();
      if (response.status !== 200) {
        enqueueSnackbar('Unable to get customizations', { variant: 'error' });
      } else {
        saveFile(response.data, 'customizations');
      }
    } catch (error: unknown) {
      enqueueSnackbar(extractErrorMessage(error, 'Problem with downloading'), { variant: 'error' });
    }
  };

  return (
    <SectionContent title="Application Information &amp; Support" titleGutter>
      <List>
        <ListItem>
          <ListItemAvatar>
            <TuneIcon />
          </ListItemAvatar>
          <ListItemText>
            For a help on each of the Application Settings see&nbsp;
            <Link
              target="_blank"
              href="https://emsesp.github.io/docs/#/Configure-firmware?id=ems-esp-settings"
              color="primary"
            >
              {'Configuring EMS-ESP'}
            </Link>
          </ListItemText>
        </ListItem>

        <ListItem>
          <ListItemAvatar>
            <MenuBookIcon />
          </ListItemAvatar>
          <ListItemText>
            For general information about EMS-ESP visit the online&nbsp;
            <Link target="_blank" href="https://emsesp.github.io/docs" color="primary">
              {'Documentation'}
            </Link>
          </ListItemText>
        </ListItem>

        <ListItem>
          <ListItemAvatar>
            <CommentIcon />
          </ListItemAvatar>
          <ListItemText>
            For live community chat join our&nbsp;
            <Link target="_blank" href="https://discord.gg/3J3GgnzpyT" color="primary">
              {'Discord'}
            </Link>
            &nbsp;server
          </ListItemText>
        </ListItem>

        <ListItem>
          <ListItemAvatar>
            <GitHubIcon />
          </ListItemAvatar>
          <ListItemText>
            To report an issue or request a feature, please&nbsp;
            <Link component="button" variant="body1" onClick={() => callAPI('info')}>
              download
            </Link>
            &nbsp;the debug information and include in a new&nbsp;
            <Link target="_blank" href="https://github.com/emsesp/EMS-ESP32/issues/new/choose" color="primary">
              GitHub issue
            </Link>
          </ListItemText>
        </ListItem>
      </List>

      {me.admin && (
        <>
          <Typography sx={{ pt: 2, pb: 2 }} variant="h6" color="primary">
            Download Settings
          </Typography>
          <Box color="warning.main">
            <Typography variant="body2">
              Export the application settings and any customizations to a JSON file. These files can later be uploaded
              via System&rarr;Upload.
            </Typography>
          </Box>
          <Box sx={{ display: 'flex' }}>
            <ButtonRow>
              <Button
                startIcon={<DownloadIcon />}
                variant="outlined"
                color="primary"
                onClick={() => downloadSettings()}
              >
                settings
              </Button>
              <Button
                startIcon={<DownloadIcon />}
                variant="outlined"
                color="primary"
                onClick={() => downloadCustomizations()}
              >
                customizations
              </Button>
            </ButtonRow>
          </Box>
          <MessageBox
            my={2}
            level="warning"
            message="Be careful when sharing your Settings as the file contains passwords and other sensitive system
              information!"
          />
        </>
      )}

      <Box bgcolor="secondary.info" border={1} p={1} mt={4}>
        <Typography align="center" variant="h6">
          EMS-ESP is a free and open-source project.
          <br></br>Please consider supporting us by giving it a&nbsp;
          <StarIcon style={{ fontSize: 16, color: '#fdff3a', verticalAlign: 'middle' }} /> on&nbsp;
          <Link href="https://github.com/emsesp/EMS-ESP32" color="primary">
            {'GitHub'}
          </Link>
          &nbsp;!
        </Typography>
      </Box>
    </SectionContent>
  );
};

export default HelpInformation;
