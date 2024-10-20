import { useContext, useState } from 'react';
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
  Stack,
  Typography
} from '@mui/material';

import { useRequest } from 'alova/client';
import { SectionContent, useLayoutTitle } from 'components';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import { saveFile } from 'utils';

import { API, callAction } from '../../api/app';
import type { APIcall } from './types';

const Help = () => {
  const { LL } = useI18nContext();
  useLayoutTitle(LL.HELP());

  const { me } = useContext(AuthenticatedContext);

  const [customSupportIMG, setCustomSupportIMG] = useState<string | null>(null);
  const [customSupportHTML, setCustomSupportHTML] = useState<string | null>(null);
  const [notFound, setNotFound] = useState<boolean>(false);

  useRequest(() => callAction({ action: 'customSupport' })).onSuccess((event) => {
    if (event && event.data && Object.keys(event.data).length !== 0) {
      const data = event.data.Support;
      if (data.img_url) {
        setCustomSupportIMG(data.img_url);
      }
      if (data.html) {
        setCustomSupportHTML(data.html.join('<br/>'));
      }
    }
  });

  const { send: sendExportAllValues } = useRequest(
    () => callAction({ action: 'export', param: 'allvalues' }),
    {
      immediate: false
    }
  )
    .onSuccess((event) => {
      saveFile(event.data, 'allvalues', '.txt');
      toast.info(LL.DOWNLOAD_SUCCESSFUL());
    })
    .onError((error) => {
      toast.error(error.message);
    });

  const { send: sendAPI } = useRequest((data: APIcall) => API(data), {
    immediate: false
  })
    .onSuccess((event) => {
      saveFile(event.data, 'system_info', '.json');
      toast.info(LL.DOWNLOAD_SUCCESSFUL());
    })
    .onError((error) => {
      toast.error(error.message);
    });

  return (
    <SectionContent>
      <Stack
        padding={1}
        mb={2}
        direction="row"
        divider={<Divider orientation="vertical" flexItem />}
        sx={{
          borderRadius: 3,
          border: '2px solid grey',
          justifyContent: 'space-evenly',
          alignItems: 'center'
        }}
      >
        <Typography variant="subtitle1">
          {customSupportHTML ? (
            <div dangerouslySetInnerHTML={{ __html: customSupportHTML }} />
          ) : (
            LL.HELP_INFORMATION_5()
          )}
        </Typography>
        <Box
          component="img"
          referrerPolicy="no-referrer"
          sx={{
            maxHeight: { xs: 100, md: 250 }
          }}
          onError={(e) => setNotFound(true)}
          src={
            notFound
              ? ''
              : customSupportIMG || 'https://emsesp.org/_media/images/installer.jpeg'
          }
        />
      </Stack>

      {me.admin && (
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
      )}

      <Box p={2} color="warning.main">
        <Typography mb={1} variant="body1">
          {LL.HELP_INFORMATION_4()}.
        </Typography>
        <Button
          startIcon={<DownloadIcon />}
          variant="outlined"
          color="primary"
          onClick={() => sendAPI({ device: 'system', cmd: 'info', id: 0 })}
        >
          {LL.DOWNLOAD(1)}&nbsp;{LL.SUPPORT_INFORMATION(0)}
        </Button>
      </Box>

      <Button
        sx={{ ml: 2 }}
        startIcon={<DownloadIcon />}
        variant="outlined"
        color="primary"
        onClick={() => sendExportAllValues()}
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
  );
};

export default Help;
