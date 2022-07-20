import { FC } from 'react';

import { Typography, Button, Box, List, ListItem, ListItemText, Link, ListItemAvatar } from '@mui/material';

import { SectionContent } from '../components';

import { useSnackbar } from 'notistack';

import CommentIcon from '@mui/icons-material/CommentTwoTone';
import MenuBookIcon from '@mui/icons-material/MenuBookTwoTone';
import GitHubIcon from '@mui/icons-material/GitHub';
import StarIcon from '@mui/icons-material/Star';
import DownloadIcon from '@mui/icons-material/GetApp';

import { extractErrorMessage } from '../utils';

import * as EMSESP from './api';

const HelpInformation: FC = () => {
  const { enqueueSnackbar } = useSnackbar();

  const saveFile = (json: any, endpoint: string) => {
    const a = document.createElement('a');
    const filename = 'emsesp_' + endpoint + '_json.txt';
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

  return (
    <SectionContent title="Support Information" titleGutter>
      <List>
        <ListItem>
          <ListItemAvatar>
            <MenuBookIcon />
          </ListItemAvatar>
          <ListItemText>
            Visit the online&nbsp;
            <Link target="_blank" href="https://emsesp.github.io/docs" color="primary">
              {'Wiki'}
            </Link>
            &nbsp;to get instructions on how to&nbsp;
            <Link
              target="_blank"
              href="https://emsesp.github.io/docs/#/Configure-firmware?id=ems-esp-settings"
              color="primary"
            >
              {'configure'}
            </Link>
            &nbsp;EMS-ESP and access other information.
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
            &nbsp;server.
          </ListItemText>
        </ListItem>

        <ListItem>
          <ListItemAvatar>
            <GitHubIcon />
          </ListItemAvatar>

          <ListItemText>
            Submit a&nbsp;
            <Link target="_blank" href="https://github.com/emsesp/EMS-ESP32/issues/new/choose" color="primary">
              support issue
            </Link>
            &nbsp;for requesting a new feature or reporting a bug.
            <br />
            Make sure you also&nbsp;
            <Button startIcon={<DownloadIcon />} variant="outlined" color="primary" onClick={() => callAPI('info')}>
              download
            </Button>
            &nbsp; and attach your system details for a faster response.
          </ListItemText>
        </ListItem>
      </List>

      <Box border={1} p={1} mt={4}>
        <Typography align="center" variant="h6" color="orange">
          EMS-ESP will always be a free and open-source project
          <br></br>Please consider supporting it with a&nbsp;
          <StarIcon style={{ fontSize: 16, color: 'yellow', verticalAlign: 'middle' }} /> on&nbsp;
          <Link href="https://github.com/emsesp/EMS-ESP32" color="primary">
            {'GitHub'}
          </Link>
        </Typography>
        <Typography align="center">@proddy @MichaelDvP</Typography>
      </Box>
    </SectionContent>
  );
};

export default HelpInformation;
