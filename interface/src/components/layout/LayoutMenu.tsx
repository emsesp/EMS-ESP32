import AccountCircleIcon from '@mui/icons-material/AccountCircle';
import AssessmentIcon from '@mui/icons-material/Assessment';
import CategoryIcon from '@mui/icons-material/Category';
import ConstructionIcon from '@mui/icons-material/Construction';
import KeyboardArrowDown from '@mui/icons-material/KeyboardArrowDown';
import LiveHelpIcon from '@mui/icons-material/LiveHelp';
import MoreTimeIcon from '@mui/icons-material/MoreTime';
import PersonIcon from '@mui/icons-material/Person';
import PlaylistAddIcon from '@mui/icons-material/PlaylistAdd';
import SensorsIcon from '@mui/icons-material/Sensors';
import SettingsIcon from '@mui/icons-material/Settings';

import {
  Divider,
  List,
  Box,
  Button,
  Popover,
  Avatar,
  MenuItem,
  TextField,
  ListItem,
  ListItemButton,
  ListItemIcon,
  ListItemText
} from '@mui/material';

import { useContext, useState } from 'react';
import type { Locales } from 'i18n/i18n-types';
import type { FC, ChangeEventHandler } from 'react';
import LayoutMenuItem from 'components/layout/LayoutMenuItem';
import { AuthenticatedContext } from 'contexts/authentication';

import DEflag from 'i18n/DE.svg';
import FRflag from 'i18n/FR.svg';
import GBflag from 'i18n/GB.svg';
import ITflag from 'i18n/IT.svg';
import NLflag from 'i18n/NL.svg';
import NOflag from 'i18n/NO.svg';
import PLflag from 'i18n/PL.svg';
import SKflag from 'i18n/SK.svg';
import SVflag from 'i18n/SV.svg';
import TRflag from 'i18n/TR.svg';

import { I18nContext } from 'i18n/i18n-react';
import { loadLocaleAsync } from 'i18n/i18n-util.async';

const LayoutMenu: FC = () => {
  const { me, signOut } = useContext(AuthenticatedContext);
  const { locale, LL, setLocale } = useContext(I18nContext);

  const [anchorEl, setAnchorEl] = useState<HTMLButtonElement | null>(null);

  const open = Boolean(anchorEl);
  const id = anchorEl ? 'app-menu-popover' : undefined;

  const [menuOpen, setMenuOpen] = useState(true);

  const onLocaleSelected: ChangeEventHandler<HTMLInputElement> = async ({ target }) => {
    const loc = target.value as Locales;
    localStorage.setItem('lang', loc);
    await loadLocaleAsync(loc);
    setLocale(loc);
  };

  const handleClick = (event: any) => {
    setAnchorEl(event.currentTarget);
  };

  const handleClose = () => {
    setAnchorEl(null);
  };

  return (
    <>
      <List component="nav">
        <LayoutMenuItem icon={CategoryIcon} label={LL.DEVICES()} to={`/devices`} />
        <LayoutMenuItem icon={SensorsIcon} label={LL.SENSORS()} to={`/sensors`} />
        <Divider />

        <Box
          sx={{
            bgcolor: menuOpen ? 'rgba(71, 98, 130, 0.2)' : null,
            pb: menuOpen ? 2 : 0
          }}
        >
          <ListItemButton
            alignItems="flex-start"
            onClick={() => setMenuOpen(!menuOpen)}
            sx={{
              pt: 2.5,
              pb: menuOpen ? 0 : 2.5,
              '&:hover, &:focus': { '& svg': { opacity: 1 } }
            }}
          >
            <ListItemText
              // TODO: translate
              primary="Customize"
              primaryTypographyProps={{
                fontWeight: '600',
                mb: '2px',
                color: 'lightblue'
              }}
              // TODO: translate
              secondary="Customizations, Scheduler and Custom Entities"
              secondaryTypographyProps={{
                noWrap: true,
                fontSize: 12,
                color: menuOpen ? 'rgba(0,0,0,0)' : 'rgba(255,255,255,0.5)'
              }}
              sx={{ my: 0 }}
            />
            <KeyboardArrowDown
              sx={{
                mr: -1,
                opacity: 0,
                transform: menuOpen ? 'rotate(-180deg)' : 'rotate(0)',
                transition: '0.2s'
              }}
            />
          </ListItemButton>
          {menuOpen && (
            <>
              <LayoutMenuItem
                icon={ConstructionIcon}
                label={LL.CUSTOMIZATIONS()}
                disabled={!me.admin}
                to={`/customizations`}
              />
              <LayoutMenuItem icon={MoreTimeIcon} label={LL.SCHEDULER()} disabled={!me.admin} to={`/scheduler`} />
              <LayoutMenuItem
                icon={PlaylistAddIcon}
                label={LL.CUSTOM_ENTITIES(0)}
                disabled={!me.admin}
                to={`/customentities`}
              />
            </>
          )}
        </Box>
      </List>

      <List style={{ marginTop: `auto` }}>
        <LayoutMenuItem icon={AssessmentIcon} label={LL.SYSTEM(0)} to="/system" />
        <LayoutMenuItem icon={SettingsIcon} label={LL.SETTINGS(0)} disabled={!me.admin} to="/settings" />
        <LayoutMenuItem icon={LiveHelpIcon} label={LL.HELP_OF('')} to={`/help`} />
      </List>
      <Divider />
      <List>
        <ListItem disablePadding onClick={handleClick}>
          <ListItemButton>
            <ListItemIcon>
              <AccountCircleIcon />
            </ListItemIcon>
            <ListItemText>{me.username}</ListItemText>
          </ListItemButton>
        </ListItem>
      </List>

      <Popover
        id={id}
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
        <Box
          p={2}
          sx={{
            borderRadius: 2,
            border: '2px solid grey'
          }}
        >
          <List>
            <ListItem disablePadding>
              <Avatar sx={{ bgcolor: '#b1395f', color: 'white' }}>
                <PersonIcon />
              </Avatar>
              <ListItemText
                sx={{ pl: 2 }}
                primary={me.username}
                secondary={(me.admin ? LL.ADMIN() : LL.GUEST()) + ' Account'}
              />
            </ListItem>
          </List>
          <Box p={2}>
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
              <MenuItem key="sk" value="sk">
                <img src={SKflag} style={{ width: 16, verticalAlign: 'middle' }} />
                &nbsp;SK
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
          </Box>
          <Box>
            <Button variant="outlined" fullWidth color="primary" onClick={() => signOut(true)}>
              {LL.SIGN_OUT()}
            </Button>
          </Box>
        </Box>
      </Popover>
    </>
  );
};

export default LayoutMenu;
