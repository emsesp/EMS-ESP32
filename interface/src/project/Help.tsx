import CommentIcon from '@mui/icons-material/CommentTwoTone';
import EastIcon from '@mui/icons-material/East';
import GitHubIcon from '@mui/icons-material/GitHub';
import MenuBookIcon from '@mui/icons-material/MenuBookTwoTone';
import { Box, List, ListItem, ListItemAvatar, ListItemText, Link, Typography } from '@mui/material';
import type { FC } from 'react';
import { SectionContent, useLayoutTitle } from 'components';
import { useI18nContext } from 'i18n/i18n-react';

const Help: FC = () => {
  const { LL } = useI18nContext();
  useLayoutTitle(LL.HELP_OF(''));

  const uploadURL = window.location.origin + '/system/upload';

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
            <i>({LL.HELP_INFORMATION_4()}</i>&nbsp;
            <EastIcon style={{ fontSize: 24, color: 'lightblue', verticalAlign: 'middle' }} />
            <Link href={uploadURL} color="primary">
              {LL.UPLOAD_DOWNLOAD()}
            </Link>
            )
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

export default Help;
