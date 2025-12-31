import { memo, useCallback, useContext, useMemo, useState } from 'react';
import type { ReactElement } from 'react';
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
import type { SxProps, Theme } from '@mui/material/styles';

import { useRequest } from 'alova/client';
import { SectionContent, useLayoutTitle } from 'components';
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

const DEFAULT_IMAGE_URL = 'https://emsesp.org/_media/images/installer.jpeg';

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

const HelpComponent = () => {
  const { LL } = useI18nContext();
  useLayoutTitle(LL.HELP());

  const { me } = useContext(AuthenticatedContext);

  const [customSupport, setCustomSupport] = useState<CustomSupport>({
    img_url: null,
    html: null
  });
  const [imgError, setImgError] = useState<boolean>(false);

  // Memoize the request method to prevent re-creation on every render
  const getCustomSupportMethod = useMemo(
    () => callAction({ action: 'getCustomSupport' }),
    []
  );

  useRequest(getCustomSupportMethod).onSuccess((event) => {
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

  // Optimize API call memoization
  const apiCall = useMemo(() => ({ device: 'system', cmd: 'info', id: 0 }), []);

  const handleDownloadSystemInfo = useCallback(() => {
    void sendAPI(apiCall);
  }, [sendAPI, apiCall]);

  const handleImageError = useCallback(() => {
    setImgError(true);
  }, []);

  // Memoize help links to prevent recreation on every render
  const helpLinks: HelpLink[] = useMemo(
    () => [
      {
        href: 'https://emsesp.org',
        icon: <MenuBookIcon />,
        label: () => LL.HELP_INFORMATION_1()
      },
      {
        href: 'https://discord.gg/3J3GgnzpyT',
        icon: <CommentIcon />,
        label: () => LL.HELP_INFORMATION_2()
      },
      {
        href: 'https://github.com/emsesp/EMS-ESP32/issues/new/choose',
        icon: <GitHubIcon />,
        label: () => LL.HELP_INFORMATION_3()
      }
    ],
    [LL]
  );

  const isAdmin = useMemo(() => me?.admin ?? false, [me?.admin]);

  // Memoize image source computation
  const imageSrc = useMemo(
    () =>
      imgError || !customSupport.img_url ? DEFAULT_IMAGE_URL : customSupport.img_url,
    [imgError, customSupport.img_url]
  );

  return (
    <SectionContent>
      {customSupport.html && (
        <Stack
          padding={1}
          mb={2}
          direction="row"
          divider={<Divider orientation="vertical" flexItem />}
          sx={SUPPORT_BOX_STYLES}
        >
          <Typography variant="subtitle1">
            <div dangerouslySetInnerHTML={{ __html: customSupport.html }} />
          </Typography>
          <Box
            component="img"
            referrerPolicy="no-referrer"
            sx={IMAGE_STYLES}
            onError={handleImageError}
            src={imageSrc}
          />
        </Stack>
      )}

      {isAdmin && (
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

      <Box p={2} color="warning.main">
        <Typography mb={1} variant="body1">
          {LL.HELP_INFORMATION_4()}.
        </Typography>
        <Button
          startIcon={<DownloadIcon />}
          variant="outlined"
          color="primary"
          onClick={handleDownloadSystemInfo}
        >
          {LL.SUPPORT_INFORMATION(0)}
        </Button>
      </Box>

      <Divider sx={{ mt: 4 }} />

      <Typography color="white" variant="subtitle1" align="center" mt={1}>
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

// Memoize the component to prevent unnecessary re-renders
const Help = memo(HelpComponent);

export default Help;
