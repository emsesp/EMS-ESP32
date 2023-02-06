import { FC, useState, useContext, ChangeEventHandler } from 'react';

import {
  Box,
  Button,
  Divider,
  IconButton,
  Popover,
  Typography,
  Avatar,
  styled,
  TypographyProps,
  MenuItem,
  TextField
} from '@mui/material';

import PersonIcon from '@mui/icons-material/Person';
import AccountCircleIcon from '@mui/icons-material/AccountCircle';

import { AuthenticatedContext } from '../../contexts/authentication';

import { I18nContext } from '../../i18n/i18n-react';
import type { Locales } from '../../i18n/i18n-types';
import { loadLocaleAsync } from '../../i18n/i18n-util.async';

import { ReactComponent as NLflag } from '../../i18n/NL.svg';
import { ReactComponent as DEflag } from '../../i18n/DE.svg';
import { ReactComponent as GBflag } from '../../i18n/GB.svg';
import { ReactComponent as SVflag } from '../../i18n/SV.svg';
import { ReactComponent as PLflag } from '../../i18n/PL.svg';
import { ReactComponent as NOflag } from '../../i18n/NO.svg';
import { ReactComponent as FRflag } from '../../i18n/FR.svg';

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
        <MenuItem key="en" value="en">
          <GBflag style={{ width: 16, verticalAlign: 'middle' }} />
          &nbsp;EN
        </MenuItem>
        <Divider />
        <MenuItem key="de" value="de">
          <DEflag style={{ width: 16, verticalAlign: 'middle' }} />
          &nbsp;DE
        </MenuItem>
        <MenuItem key="fr" value="fr">
          <FRflag style={{ width: 16, verticalAlign: 'middle' }} />
          &nbsp;FR
        </MenuItem>
        <MenuItem key="nl" value="nl">
          <NLflag style={{ width: 16, verticalAlign: 'middle' }} />
          &nbsp;NL
        </MenuItem>
        <MenuItem key="no" value="no">
          <NOflag style={{ width: 16, verticalAlign: 'middle' }} />
          &nbsp;NO
        </MenuItem>
        <MenuItem key="pl" value="pl">
          <PLflag style={{ width: 16, verticalAlign: 'middle' }} />
          &nbsp;PL
        </MenuItem>
        <MenuItem key="sv" value="sv">
          <SVflag style={{ width: 16, verticalAlign: 'middle' }} />
          &nbsp;SV
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
