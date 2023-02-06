import { FC } from 'react';

import { Typography, Button, Box, List, ListItem, ListItemText, Link, ListItemAvatar } from '@mui/material';

import { SectionContent } from '../components';

import { useSnackbar } from 'notistack';

import CommentIcon from '@mui/icons-material/CommentTwoTone';
import MenuBookIcon from '@mui/icons-material/MenuBookTwoTone';
import GitHubIcon from '@mui/icons-material/GitHub';
import DownloadIcon from '@mui/icons-material/GetApp';
import EastIcon from '@mui/icons-material/East';

import { extractErrorMessage } from '../utils';

import { useI18nContext } from '../i18n/i18n-react';

import * as EMSESP from './api';

const HelpInformation: FC = () => {
  const { LL } = useI18nContext();

  const { enqueueSnackbar } = useSnackbar();

  const saveFile = (json: any, endpoint: string) => {
    const a = document.createElement('a');
    const filename = 'emsesp_' + endpoint + '.txt';
    a.href = URL.createObjectURL(
      new Blob([JSON.stringify(json, null, 2)], {
        type: 'text/plain'
      })
    );
    a.setAttribute('download', filename);
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    enqueueSnackbar(LL.DOWNLOAD_SUCCESSFUL(), { variant: 'info' });
  };

  const callAPI = async (endpoint: string) => {
    try {
      const response = await EMSESP.API({
        device: 'system',
        entity: endpoint,
        id: 0
      });
      if (response.status !== 200) {
        enqueueSnackbar(LL.PROBLEM_LOADING(), { variant: 'error' });
      } else {
        saveFile(response.data, endpoint);
      }
    } catch (error) {
      enqueueSnackbar(extractErrorMessage(error, LL.PROBLEM_LOADING()), { variant: 'error' });
    }
  };

  return (
    <SectionContent title={LL.SUPPORT_INFORMATION()} titleGutter>
      <List>
        <ListItem>
          <ListItemAvatar>
            <MenuBookIcon style={{ fontSize: 24, color: 'lightblue', verticalAlign: 'middle' }} />
          </ListItemAvatar>
          <ListItemText>
            {LL.HELP_INFORMATION_1()}&nbsp;
            <EastIcon style={{ fontSize: 24, color: 'lightblue', verticalAlign: 'middle' }} />
            &nbsp;
            <Link target="_blank" href="https://emsesp.github.io/docs" color="primary">
              {LL.CLICK_HERE()}
            </Link>
          </ListItemText>
        </ListItem>

        <ListItem>
          <ListItemAvatar>
            <CommentIcon style={{ fontSize: 24, color: 'lightblue', verticalAlign: 'middle' }} />
          </ListItemAvatar>
          <ListItemText>
            {LL.HELP_INFORMATION_2()}&nbsp;
            <EastIcon style={{ fontSize: 24, color: 'lightblue', verticalAlign: 'middle' }} />
            &nbsp;
            <Link target="_blank" href="https://discord.gg/3J3GgnzpyT" color="primary">
              {LL.CLICK_HERE()}
            </Link>
          </ListItemText>
        </ListItem>

        <ListItem>
          <ListItemAvatar>
            <GitHubIcon style={{ fontSize: 24, color: 'lightblue', verticalAlign: 'middle' }} />
          </ListItemAvatar>
          <ListItemText>
            {LL.HELP_INFORMATION_3()}&nbsp;
            <EastIcon style={{ fontSize: 24, color: 'lightblue', verticalAlign: 'middle' }} />
            <Link target="_blank" href="https://github.com/emsesp/EMS-ESP32/issues/new/choose" color="primary">
              {LL.CLICK_HERE()}
            </Link>
            <br />
            <i>({LL.HELP_INFORMATION_4()}</i>&nbsp;&nbsp;
            <Button
              startIcon={<DownloadIcon />}
              size="small"
              variant="outlined"
              color="primary"
              onClick={() => callAPI('info')}
            >
              {LL.SUPPORT_INFO()}
            </Button>
            &nbsp;)
          </ListItemText>
        </ListItem>
      </List>

      <Box border={1} p={1} mt={4} color="orange">
        <Typography align="center" variant="subtitle1" color="orange">
          <b>{LL.HELP_INFORMATION_5()}</b>
        </Typography>
        <Typography align="center">
          <Link target="_blank" href="https://github.com/emsesp/EMS-ESP32" color="primary">
            {'github.com/emsesp/EMS-ESP32'}
          </Link>
        </Typography>
        <Typography color="white" align="center">
          @proddy @MichaelDvP
        </Typography>
      </Box>
    </SectionContent>
  );
};

export default HelpInformation;
