import { memo, useContext, useState } from 'react';
import type { ReactElement } from 'react';

import CommentIcon from '@mui/icons-material/CommentTwoTone';
import DownloadIcon from '@mui/icons-material/GetApp';
import GitHubIcon from '@mui/icons-material/GitHub';
import MenuBookIcon from '@mui/icons-material/MenuBookTwoTone';
import {
  Avatar,
  Box,
  Button,
  Divider,
  Grid,
  Link,
  List,
  ListItem,
  ListItemAvatar,
  ListItemButton,
  ListItemText,
  Stack,
  Typography
} from '@mui/material';
import type { SxProps, Theme } from '@mui/material/styles';

import { useRequest } from 'alova/client';
import { SectionContent, useLayoutTitle } from 'components';
import { toast } from 'components/toast';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import { saveFile } from 'utils';

import { API, callAction } from '../../api/app';
import type { APIcall } from './types';

interface HelpLink {
  href: string;
  icon: ReactElement;
  label: () => string;
}

interface CustomSupport {
  img_url: string | null;
  html: string | null;
}

const DEFAULT_IMAGE_URL = 'https://emsesp.org/media/images/installer.jpeg';

const SUPPORT_BOX_STYLES: SxProps<Theme> = {
  borderRadius: 3,
  border: '1px solid lightblue',
  justifyContent: 'space-evenly',
  alignItems: 'center'
};

const IMAGE_STYLES: SxProps<Theme> = {
  maxHeight: { xs: 100, md: 250 }
};

const AVATAR_STYLES: SxProps<Theme> = {
  bgcolor: '#72caf9'
};

const SYSTEM_INFO_API: APIcall = { device: 'system', cmd: 'info', id: 0 };

const HelpComponent = () => {
  const { LL } = useI18nContext();
  useLayoutTitle(LL.HELP());

  const { me } = useContext(AuthenticatedContext);

  const [customSupport, setCustomSupport] = useState<CustomSupport>({
    img_url: null,
    html: null
  });
  const [imgError, setImgError] = useState<boolean>(false);

  useRequest(callAction({ action: 'getCustomSupport' })).onSuccess((event) => {
    if (event?.data && Object.keys(event.data).length !== 0) {
      const { Support } = event.data as {
        Support: { img_url?: string; html?: string[] };
      };
      setCustomSupport({
        img_url: Support.img_url || null,
        html: Support.html?.join('<br/>') || null
      });
    }
  });

  const { send: sendAPI } = useRequest((data: APIcall) => API(data), {
    immediate: false
  })
    .onSuccess((event) => {
      saveFile(event.data, 'system_info', '.json');
      toast.info(LL.DOWNLOAD_SUCCESSFUL());
    })
    .onError((error) => {
      toast.error(String(error.error?.message || 'An error occurred'));
    });

  const helpLinks: HelpLink[] = [
    {
      href: 'https://emsesp.org',
      icon: <MenuBookIcon />,
      label: () => LL.HELP_INFORMATION_1()
    },
    {
      href: 'https://discord.gg/GP9DPSgeJq',
      icon: <CommentIcon />,
      label: () => LL.HELP_INFORMATION_2()
    },
    {
      href: 'https://github.com/emsesp/EMS-ESP32/issues/new/choose',
      icon: <GitHubIcon />,
      label: () => LL.HELP_INFORMATION_3()
    }
  ];

  const imageSrc =
    imgError || !customSupport.img_url ? DEFAULT_IMAGE_URL : customSupport.img_url;

  return (
    <SectionContent>
      {customSupport.html && (
        <Stack
          direction="row"
          divider={<Divider orientation="vertical" flexItem />}
          sx={{ padding: 1, mb: 2, ...SUPPORT_BOX_STYLES }}
        >
          <Typography variant="subtitle1">
            <div dangerouslySetInnerHTML={{ __html: customSupport.html }} />
          </Typography>
          <Box
            component="img"
            referrerPolicy="no-referrer"
            sx={IMAGE_STYLES}
            onError={() => setImgError(true)}
            src={imageSrc}
          />
        </Stack>
      )}

      {me?.admin && (
        <List>
          {helpLinks.map(({ href, icon, label }) => (
            <ListItem key={href}>
              <ListItemButton
                component="a"
                target="_blank"
                rel="noreferrer"
                href={href}
              >
                <ListItemAvatar>
                  <Avatar sx={AVATAR_STYLES}>{icon}</Avatar>
                </ListItemAvatar>
                <ListItemText primary={label()} />
              </ListItemButton>
            </ListItem>
          ))}
        </List>
      )}

      <Grid container spacing={2} sx={{ mt: 2, alignItems: 'center' }}>
        <Typography sx={{ mb: 1 }} color="warning" variant="body1">
          {LL.HELP_INFORMATION_4()}:
        </Typography>
        <Button
          startIcon={<DownloadIcon />}
          variant="outlined"
          color="primary"
          onClick={() => void sendAPI(SYSTEM_INFO_API)}
        >
          {LL.SUPPORT_INFORMATION(0)}
        </Button>
      </Grid>

      <Divider sx={{ mt: 4 }} />

      <Typography color="white" variant="subtitle1" align="center" sx={{ mt: 1 }}>
        &copy;&nbsp;
        <Link
          target="_blank"
          rel="noreferrer"
          href="https://emsesp.org"
          color="primary"
        >
          emsesp.org
        </Link>
      </Typography>
    </SectionContent>
  );
};

const Help = memo(HelpComponent);

export default Help;
