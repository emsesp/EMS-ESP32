import { useContext } from 'react';
import { toast } from 'react-toastify';

import CommentIcon from '@mui/icons-material/CommentTwoTone';
import DownloadIcon from '@mui/icons-material/GetApp';
import GitHubIcon from '@mui/icons-material/GitHub';
import MenuBookIcon from '@mui/icons-material/MenuBookTwoTone';
import {
  Avatar,
  Box,
  Button,
  Divider,
  Link,
  List,
  ListItem,
  ListItemAvatar,
  ListItemButton,
  ListItemText,
  Paper,
  Stack,
  Typography,
  styled
} from '@mui/material';

import { useRequest } from 'alova/client';
import { SectionContent, useLayoutTitle } from 'components';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';

import { API } from '../../api/app';
import type { APIcall } from './types';

const Help = () => {
  const { LL } = useI18nContext();
  useLayoutTitle(LL.HELP());

  const { me } = useContext(AuthenticatedContext);

  const { send: sendAPI } = useRequest((data: APIcall) => API(data), {
    immediate: false
  }).onSuccess((event) => {
    const anchor = document.createElement('a');
    anchor.href = URL.createObjectURL(
      new Blob([JSON.stringify(event.data, null, 2)], {
        type: 'text/plain'
      })
    );

    anchor.download =
      'emsesp_' + event.args[0].device + '_' + event.args[0].entity + '.txt';
    anchor.click();
    URL.revokeObjectURL(anchor.href);
    toast.info(LL.DOWNLOAD_SUCCESSFUL());
  });

  const callAPI = async (device: string, cmd: string) => {
    await sendAPI({ device, cmd, id: 0 }).catch((error: Error) => {
      toast.error(error.message);
    });
  };

  return (
    <>
      <SectionContent>
        {me.admin ? (
          <List sx={{ borderRadius: 3, border: '2px solid grey' }}>
            <ListItem>
              <ListItemButton component="a" href="https://emsesp.org">
                <ListItemAvatar>
                  <Avatar sx={{ bgcolor: '#72caf9' }}>
                    <MenuBookIcon />
                  </Avatar>
                </ListItemAvatar>
                <ListItemText primary={LL.HELP_INFORMATION_1()} />
              </ListItemButton>
            </ListItem>

            <ListItem>
              <ListItemButton component="a" href="https://discord.gg/3J3GgnzpyT">
                <ListItemAvatar>
                  <Avatar sx={{ bgcolor: '#72caf9' }}>
                    <CommentIcon />
                  </Avatar>
                </ListItemAvatar>
                <ListItemText primary={LL.HELP_INFORMATION_2()} />
              </ListItemButton>
            </ListItem>

            <ListItem>
              <ListItemButton
                component="a"
                href="https://github.com/emsesp/EMS-ESP32/issues/new/choose"
              >
                <ListItemAvatar>
                  <Avatar sx={{ bgcolor: '#72caf9' }}>
                    <GitHubIcon />
                  </Avatar>
                </ListItemAvatar>
                <ListItemText primary={LL.HELP_INFORMATION_3()} />
              </ListItemButton>
            </ListItem>
          </List>
        ) : (
          <Stack
            spacing={1}
            padding={1}
            direction="row"
            divider={<Divider orientation="vertical" flexItem />}
            sx={{
              borderRadius: 3,
              border: '2px solid grey',
              justifyContent: 'space-around',
              alignItems: 'center'
            }}
          >
            <Typography border="red" variant="subtitle1">
              {LL.HELP_INFORMATION_5()}
            </Typography>
            <Box
              padding={1}
              component="img"
              sx={{
                maxHeight: { xs: 100, md: 250 }
              }}
              src="https://emsesp.org/_media/images/installer.jpeg"
            />
          </Stack>
        )}

        <Box p={2} color="warning.main">
          <Typography mb={1} variant="body2">
            {LL.HELP_INFORMATION_4()}
          </Typography>
          <Button
            startIcon={<DownloadIcon />}
            variant="outlined"
            color="primary"
            onClick={() => callAPI('system', 'info')}
          >
            {LL.DOWNLOAD(1)}&nbsp;{LL.SUPPORT_INFORMATION(0)}
          </Button>
        </Box>

        <Button
          sx={{ ml: 2 }}
          startIcon={<DownloadIcon />}
          variant="outlined"
          color="primary"
          onClick={() => callAPI('system', 'allvalues')}
        >
          {LL.DOWNLOAD(1)}&nbsp;{LL.ALLVALUES()}
        </Button>

        <Divider sx={{ mt: 4 }} />

        <Typography color="white" variant="subtitle1" align="center" mt={1}>
          &copy;&nbsp;
          <Link target="_blank" href="https://emsesp.org" color="primary">
            {'emsesp.org'}
          </Link>
        </Typography>
      </SectionContent>
    </>
  );
};

export default Help;
