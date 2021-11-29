import { FC } from 'react';

import { Typography, Button, Box, List, ListItem, ListItemText, Link, ListItemAvatar } from '@mui/material';

import { SectionContent, ButtonRow } from '../components';

import { useSnackbar } from 'notistack';

import CommentIcon from '@mui/icons-material/CommentTwoTone';
import MenuBookIcon from '@mui/icons-material/MenuBookTwoTone';
import GitHubIcon from '@mui/icons-material/GitHub';
import StarIcon from '@mui/icons-material/Star';
import DownloadIcon from '@mui/icons-material/GetApp';
import TuneIcon from '@mui/icons-material/Tune';

import { extractErrorMessage } from '../utils';

import * as EMSESP from './api';

const SettingsHelp: FC = () => {
  const { enqueueSnackbar } = useSnackbar();

  const onDownload = async (endpoint: string) => {
    try {
      const response = await EMSESP.API({
        device: 'system',
        entity: endpoint,
        id: 0
      });
      if (response.status !== 200) {
        enqueueSnackbar('API call failed', { variant: 'error' });
      } else {
        const json = response.data;
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
        enqueueSnackbar('Downloaded file', { variant: 'info' });
      }
    } catch (error: any) {
      enqueueSnackbar(extractErrorMessage(error, 'Problem with downloading'), { variant: 'error' });
    }
  };

  return (
    <SectionContent title="Help and Information" titleGutter>
      <List>
        <ListItem>
          <ListItemAvatar>
            <TuneIcon />
          </ListItemAvatar>
          <ListItemText>
            For a description of each of the Application Settings click&nbsp;
            <Link href="https://emsesp.github.io/docs/#/Configure-firmware?id=ems-esp-settings" color="primary">
              {'here'}
            </Link>
          </ListItemText>
        </ListItem>

        <ListItem>
          <ListItemAvatar>
            <MenuBookIcon />
          </ListItemAvatar>
          <ListItemText>
            For general information about EMS-ESP visit the online&nbsp;
            <Link href="https://emsesp.github.io/docs" color="primary">
              {'documentation'}
            </Link>
          </ListItemText>
        </ListItem>

        <ListItem>
          <ListItemAvatar>
            <CommentIcon />
          </ListItemAvatar>
          <ListItemText>
            For live community chat join our&nbsp;
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
            To report an issue or request a feature, please do so via&nbsp;
            <Link href="https://github.com/emsesp/EMS-ESP32/issues/new/choose" color="primary">
              {'GitHub'}
            </Link>
          </ListItemText>
        </ListItem>
      </List>

      <Box sx={{ display: 'flex', flexWrap: 'nowrap' }}>
        <ButtonRow>
          <Button startIcon={<DownloadIcon />} variant="outlined" color="primary" onClick={() => onDownload('info')}>
            download system info
          </Button>
          <Button
            startIcon={<DownloadIcon />}
            variant="outlined"
            color="primary"
            onClick={() => onDownload('settings')}
          >
            download settings
          </Button>
        </ButtonRow>
      </Box>

      <Box bgcolor="info.main" border={1} p={2} mt={4} mb={1}>
        <Typography variant="body1">
          EMS-ESP is a free and open-source project. Please consider supporting us by giving it a&nbsp;
          <StarIcon style={{ color: '#fdff3a' }} /> on&nbsp;
          <Link href="https://github.com/emsesp/EMS-ESP32" color="primary">
            {'GitHub'}
          </Link>
          &nbsp;!
        </Typography>
      </Box>
    </SectionContent>
  );
};

export default SettingsHelp;
