import { useContext, useState } from 'react';

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
import StarIcon from '@mui/icons-material/Star';
import {
  Avatar,
  Box,
  Button,
  Divider,
  List,
  ListItem,
  ListItemButton,
  ListItemIcon,
  ListItemText,
  Popover
} from '@mui/material';

import { LanguageSelector } from 'components/inputs';
import LayoutMenuItem from 'components/layout/LayoutMenuItem';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';

const LayoutMenu = () => {
  const { me, signOut } = useContext(AuthenticatedContext);
  const { LL } = useI18nContext();

  const [anchorEl, setAnchorEl] = useState<HTMLButtonElement | null>(null);

  const open = Boolean(anchorEl);
  const id = anchorEl ? 'app-menu-popover' : undefined;

  const [menuOpen, setMenuOpen] = useState(true);

  const handleClick = (event: React.MouseEvent<HTMLButtonElement>) => {
    setAnchorEl(event.currentTarget);
  };

  const handleClose = () => {
    setAnchorEl(null);
  };

  return (
    <>
      <List component="nav">
        <LayoutMenuItem icon={StarIcon} label="Dashboard" to={`/dashboard`} />
        <LayoutMenuItem icon={CategoryIcon} label={LL.DEVICES()} to={`/devices`} />
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
              primary={LL.MODULES()}
              // secondary={
              //   LL.CUSTOMIZATIONS() +
              //   ', ' +
              //   LL.SCHEDULER() +
              //   ', ' +
              //   LL.CUSTOM_ENTITIES(0) +
              //   '...'
              // }
              // secondaryTypographyProps={{
              //   noWrap: true,
              //   fontSize: 12,
              //   color: menuOpen ? 'rgba(0,0,0,0)' : 'rgba(255,255,255,0.5)'
              // }}
              sx={{ my: 0 }}
              slotProps={{
                primary: {
                  fontWeight: '600',
                  mb: '2px',
                  color: 'lightblue'
                }
              }}
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
                icon={SensorsIcon}
                label={LL.SENSORS()}
                to={`/sensors`}
              />

              <LayoutMenuItem
                icon={ConstructionIcon}
                label={LL.CUSTOMIZATIONS()}
                disabled={!me.admin}
                to={`/customizations`}
              />
              <LayoutMenuItem
                icon={MoreTimeIcon}
                label={LL.SCHEDULER()}
                disabled={!me.admin}
                to={`/scheduler`}
              />
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
        <LayoutMenuItem
          icon={AssessmentIcon}
          label={LL.STATUS_OF('')}
          to="/status"
        />
        <LayoutMenuItem
          icon={SettingsIcon}
          label={LL.SETTINGS(0)}
          disabled={!me.admin}
          to="/settings"
        />
        <LayoutMenuItem icon={LiveHelpIcon} label={LL.HELP()} to={`/help`} />
      </List>
      <Divider />
      <List>
        <ListItem disablePadding>
          <ListItemButton component="button" onClick={handleClick}>
            <ListItemIcon sx={{ color: '#9e9e9e' }}>
              <AccountCircleIcon />
            </ListItemIcon>
            <ListItemText sx={{ color: '#2196f3' }}>{me.username}</ListItemText>
          </ListItemButton>
        </ListItem>
      </List>
      <Popover
        id={id}
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
          padding={2}
          justifyContent="center"
          flexDirection="column"
          sx={{
            borderRadius: 3,
            border: '3px solid grey'
          }}
        >
          <Button
            variant="outlined"
            fullWidth
            color="primary"
            onClick={() => signOut(true)}
          >
            {LL.SIGN_OUT()}
          </Button>
          <List>
            <ListItem disablePadding>
              <Avatar sx={{ bgcolor: '#9e9e9e', color: 'white' }}>
                <PersonIcon />
              </Avatar>
              <ListItemText
                sx={{ pl: 2, color: '#2196f3' }}
                primary={me.username}
                secondary={'(' + (me.admin ? LL.ADMINISTRATOR() : LL.GUEST()) + ')'}
              />
            </ListItem>
          </List>
          <LanguageSelector />
        </Box>
      </Popover>
    </>
  );
};

export default LayoutMenu;
