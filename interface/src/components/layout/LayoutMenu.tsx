import { memo, useCallback, useContext, useState } from 'react';

import AccountCircleIcon from '@mui/icons-material/AccountCircle';
import AssessmentIcon from '@mui/icons-material/Assessment';
import CategoryIcon from '@mui/icons-material/Category';
import ConstructionIcon from '@mui/icons-material/Construction';
import KeyboardArrowDown from '@mui/icons-material/KeyboardArrowDown';
import LiveHelpIcon from '@mui/icons-material/LiveHelp';
import MoreTimeIcon from '@mui/icons-material/MoreTime';
import PlaylistAddIcon from '@mui/icons-material/PlaylistAdd';
import SensorsIcon from '@mui/icons-material/Sensors';
import SettingsIcon from '@mui/icons-material/Settings';
import StarIcon from '@mui/icons-material/Star';
import { Box, Divider, List, ListItemButton, ListItemText } from '@mui/material';

import LayoutMenuItem from 'components/layout/LayoutMenuItem';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';

const LayoutMenuComponent = () => {
  const { me } = useContext(AuthenticatedContext);
  const { LL } = useI18nContext();
  const [menuOpen, setMenuOpen] = useState(true);

  const handleMenuToggle = useCallback(() => {
    setMenuOpen((prev) => !prev);
  }, []);

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
            onClick={handleMenuToggle}
            sx={{
              '&:hover, &:focus': { '& svg': { opacity: 1 } }
            }}
          >
            <ListItemText
              primary={LL.MODULES()}
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
        <Divider />
        <LayoutMenuItem icon={AccountCircleIcon} label={me.username} to={`/user`} />
      </List>
    </>
  );
};

const LayoutMenu = memo(LayoutMenuComponent);

export default LayoutMenu;
