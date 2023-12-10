import CommentIcon from '@mui/icons-material/CommentTwoTone';
import EastIcon from '@mui/icons-material/East';
import DownloadIcon from '@mui/icons-material/GetApp';
import GitHubIcon from '@mui/icons-material/GitHub';
import MenuBookIcon from '@mui/icons-material/MenuBookTwoTone';
import { Box, List, ListItem, ListItemAvatar, ListItemText, Link, Typography, Button } from '@mui/material';
import { useRequest } from 'alova';
import { toast } from 'react-toastify';
import type { FC } from 'react';
import { SectionContent, useLayoutTitle } from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import * as EMSESP from 'project/api';

const Help: FC = () => {
  const { LL } = useI18nContext();
  useLayoutTitle(LL.HELP_OF(''));

  const { send: getAPI, onSuccess: onGetAPI } = useRequest((data) => EMSESP.API(data), {
    immediate: false
  });
  onGetAPI((event) => {
    const anchor = document.createElement('a');
    anchor.href = URL.createObjectURL(
      new Blob([JSON.stringify(event.data, null, 2)], {
        type: 'text/plain'
      })
    );
    anchor.download = 'emsesp_' + event.sendArgs[0].device + '_' + event.sendArgs[0].entity + '.json';
    anchor.click();
    URL.revokeObjectURL(anchor.href);
    toast.info(LL.DOWNLOAD_SUCCESSFUL());
  });
  const callAPI = async (device: string, entity: string) => {
    await getAPI({ device, entity, id: 0 }).catch((error) => {
      toast.error(error.message);
    });
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
          </ListItemText>
        </ListItem>
      </List>

      <Box color="warning.main">
        <Typography mb={1} variant="body2">
          {LL.HELP_INFORMATION_4()}
        </Typography>
      </Box>
      <Button startIcon={<DownloadIcon />} variant="outlined" color="primary" onClick={() => callAPI('system', 'info')}>
        {LL.SUPPORT_INFORMATION()}
      </Button>
      <Button
        sx={{ ml: 2 }}
        startIcon={<DownloadIcon />}
        variant="outlined"
        color="primary"
        onClick={() => callAPI('system', 'allvalues')}
      >
        All Values
      </Button>

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

export default Help;
