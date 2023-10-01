import AccountCircleIcon from '@mui/icons-material/AccountCircle';
import PersonIcon from '@mui/icons-material/Person';
import {
  Box,
  Button,
  Divider,
  IconButton,
  Popover,
  Typography,
  Avatar,
  styled,
  MenuItem,
  TextField
} from '@mui/material';
import { useState, useContext } from 'react';
import type { TypographyProps } from '@mui/material';

import type { Locales } from 'i18n/i18n-types';
import type { FC, ChangeEventHandler } from 'react';
import { AuthenticatedContext } from 'contexts/authentication';

import DEflag from 'i18n/DE.svg';
import FRflag from 'i18n/FR.svg';
import GBflag from 'i18n/GB.svg';
import ITflag from 'i18n/IT.svg';
import NLflag from 'i18n/NL.svg';
import NOflag from 'i18n/NO.svg';
import PLflag from 'i18n/PL.svg';
import SVflag from 'i18n/SV.svg';
import TRflag from 'i18n/TR.svg';
import { I18nContext } from 'i18n/i18n-react';
import { loadLocaleAsync } from 'i18n/i18n-util.async';

const ItemTypography = styled(Typography)<TypographyProps>({
  maxWidth: '250px',
  whiteSpace: 'nowrap',
  overflow: 'hidden',
  textOverflow: 'ellipsis'
});

const LayoutAuthMenu: FC = () => {
  const { me, signOut } = useContext(AuthenticatedContext);

  const [anchorEl, setAnchorEl] = useState<HTMLButtonElement | null>(null);

  const handleClick = (event: React.MouseEvent<HTMLButtonElement>) => {
    setAnchorEl(event.currentTarget);
  };

  const { locale, LL, setLocale } = useContext(I18nContext);

  const onLocaleSelected: ChangeEventHandler<HTMLInputElement> = async ({ target }) => {
    const loc = target.value as Locales;
    localStorage.setItem('lang', loc);
    await loadLocaleAsync(loc);
    setLocale(loc);
  };

  const handleClose = () => {
    setAnchorEl(null);
  };

  const open = Boolean(anchorEl);
  const id = anchorEl ? 'app-menu-popover' : undefined;

  return (
    <>
      <TextField
        name="locale"
        InputProps={{ style: { fontSize: 10 } }}
        variant="outlined"
        value={locale}
        onChange={onLocaleSelected}
        size="small"
        select
      >
        <MenuItem key="de" value="de">
          <img src={DEflag} style={{ width: 16, verticalAlign: 'middle' }} />
          &nbsp;DE
        </MenuItem>
        <MenuItem key="en" value="en">
          <img src={GBflag} style={{ width: 16, verticalAlign: 'middle' }} />
          &nbsp;EN
        </MenuItem>
        <MenuItem key="fr" value="fr">
          <img src={FRflag} style={{ width: 16, verticalAlign: 'middle' }} />
          &nbsp;FR
        </MenuItem>
        <MenuItem key="it" value="it">
          <img src={ITflag} style={{ width: 16, verticalAlign: 'middle' }} />
          &nbsp;IT
        </MenuItem>
        <MenuItem key="nl" value="nl">
          <img src={NLflag} style={{ width: 16, verticalAlign: 'middle' }} />
          &nbsp;NL
        </MenuItem>
        <MenuItem key="no" value="no">
          <img src={NOflag} style={{ width: 16, verticalAlign: 'middle' }} />
          &nbsp;NO
        </MenuItem>
        <MenuItem key="pl" value="pl">
          <img src={PLflag} style={{ width: 16, verticalAlign: 'middle' }} />
          &nbsp;PL
        </MenuItem>
        <MenuItem key="sv" value="sv">
          <img src={SVflag} style={{ width: 16, verticalAlign: 'middle' }} />
          &nbsp;SV
        </MenuItem>
        <MenuItem key="tr" value="tr">
          <img src={TRflag} style={{ width: 16, verticalAlign: 'middle' }} />
          &nbsp;TR
        </MenuItem>
      </TextField>

      <IconButton
        id="open-auth-menu"
        sx={{ ml: 1, padding: 0 }}
        aria-describedby={id}
        color="inherit"
        onClick={handleClick}
      >
        <AccountCircleIcon />
      </IconButton>
      <Popover
        id="app-menu-popover"
        sx={{ mt: 1 }}
        open={open}
        anchorEl={anchorEl}
        onClose={handleClose}
        anchorOrigin={{
          vertical: 'bottom',
          horizontal: 'center'
        }}
        transformOrigin={{
          vertical: 'top',
          horizontal: 'center'
        }}
      >
        <Box display="flex" flexDirection="row" alignItems="center" p={2}>
          <Avatar sx={{ width: 80, height: 80 }}>
            <PersonIcon fontSize="large" />
          </Avatar>
          <Box pl={2}>
            <ItemTypography variant="h6">{me.username}</ItemTypography>
            <ItemTypography variant="body1">
              {me.admin ? LL.ADMIN() : LL.GUEST()}&nbsp;{LL.USER(2)}
            </ItemTypography>
          </Box>
        </Box>
        <Divider />
        <Box p={1.5}>
          <Button variant="outlined" fullWidth color="primary" onClick={() => signOut(true)}>
            {LL.SIGN_OUT()}
          </Button>
        </Box>
      </Popover>
    </>
  );
};

export default LayoutAuthMenu;
