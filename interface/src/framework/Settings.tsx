import AccessTimeIcon from '@mui/icons-material/AccessTime';
import CastIcon from '@mui/icons-material/Cast';
import DeviceHubIcon from '@mui/icons-material/DeviceHub';
import ImportExportIcon from '@mui/icons-material/ImportExport';
import LockIcon from '@mui/icons-material/Lock';
import NavigateNextIcon from '@mui/icons-material/NavigateNext';
import SettingsEthernetIcon from '@mui/icons-material/SettingsEthernet';
import SettingsInputAntennaIcon from '@mui/icons-material/SettingsInputAntenna';
import TuneIcon from '@mui/icons-material/Tune';
import { List, ListItem, ListItemAvatar, ListItemText, Avatar, ListItemButton, ListItemIcon } from '@mui/material';
import { Link } from 'react-router-dom';
import type { FC } from 'react';
import { SectionContent, useLayoutTitle } from 'components';
import { useI18nContext } from 'i18n/i18n-react';

const Settings: FC = () => {
  const { LL } = useI18nContext();
  useLayoutTitle(LL.SETTINGS(0));

  return (
    <SectionContent>
      <List sx={{ borderRadius: 3, border: '2px solid grey' }}>
        <ListItem
          disablePadding
          secondaryAction={
            <ListItemIcon style={{ justifyContent: 'right', verticalAlign: 'middle' }}>
              <NavigateNextIcon />
            </ListItemIcon>
          }
        >
          <ListItemButton component={Link} to="ems-esp">
            <ListItemAvatar>
              <Avatar sx={{ bgcolor: '#134ba2', color: 'white' }}>
                <TuneIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary={LL.APPLICATION_SETTINGS()} secondary="Modify EMS-ESP system settings" />
          </ListItemButton>
        </ListItem>

        <ListItem
          disablePadding
          secondaryAction={
            <ListItemIcon style={{ justifyContent: 'right', color: 'lightblue', verticalAlign: 'middle' }}>
              <NavigateNextIcon />
            </ListItemIcon>
          }
        >
          <ListItemButton component={Link} to="network">
            <ListItemAvatar>
              <Avatar sx={{ bgcolor: '#40828f', color: 'white' }}>
                <SettingsEthernetIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary={LL.NETWORK(0)} secondary="Configure Network settings" />
          </ListItemButton>
        </ListItem>

        <ListItem
          disablePadding
          secondaryAction={
            <ListItemIcon style={{ justifyContent: 'right', color: 'lightblue', verticalAlign: 'middle' }}>
              <NavigateNextIcon />
            </ListItemIcon>
          }
        >
          <ListItemButton component={Link} to="ap">
            <ListItemAvatar>
              <Avatar sx={{ bgcolor: '#5f9a5f', color: 'white' }}>
                <SettingsInputAntennaIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary={LL.ACCESS_POINT(0)} secondary="Configure Access Point" />
          </ListItemButton>
        </ListItem>

        <ListItem
          disablePadding
          secondaryAction={
            <ListItemIcon style={{ justifyContent: 'right', color: 'lightblue', verticalAlign: 'middle' }}>
              <NavigateNextIcon />
            </ListItemIcon>
          }
        >
          <ListItemButton component={Link} to="ntp">
            <ListItemAvatar>
              <Avatar sx={{ bgcolor: '#c5572c', color: 'white' }}>
                <AccessTimeIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary="NTP" secondary="Configure Network Time" />
          </ListItemButton>
        </ListItem>

        <ListItem
          disablePadding
          secondaryAction={
            <ListItemIcon style={{ justifyContent: 'right', color: 'lightblue', verticalAlign: 'middle' }}>
              <NavigateNextIcon />
            </ListItemIcon>
          }
        >
          <ListItemButton component={Link} to="mqtt">
            <ListItemAvatar>
              <Avatar sx={{ bgcolor: '#68374d', color: 'white' }}>
                <DeviceHubIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary="MQTT" secondary="Configure MQTT" />
          </ListItemButton>
        </ListItem>

        <ListItem
          disablePadding
          secondaryAction={
            <ListItemIcon style={{ justifyContent: 'right', color: 'lightblue', verticalAlign: 'middle' }}>
              <NavigateNextIcon />
            </ListItemIcon>
          }
        >
          <ListItemButton component={Link} to="ota">
            <ListItemAvatar>
              <Avatar sx={{ bgcolor: '#efc34b', color: 'white' }}>
                <CastIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary="OTA" secondary="Configure OTA" />
          </ListItemButton>
        </ListItem>

        <ListItem
          disablePadding
          secondaryAction={
            <ListItemIcon style={{ justifyContent: 'right', color: 'lightblue', verticalAlign: 'middle' }}>
              <NavigateNextIcon />
            </ListItemIcon>
          }
        >
          <ListItemButton component={Link} to="security">
            <ListItemAvatar>
              <Avatar sx={{ bgcolor: '#b1395f', color: 'white' }}>
                <LockIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary={LL.SECURITY(0)} secondary="Configure user administration" />
          </ListItemButton>
        </ListItem>

        <ListItem
          disablePadding
          secondaryAction={
            <ListItemIcon style={{ justifyContent: 'right', color: 'lightblue', verticalAlign: 'middle' }}>
              <NavigateNextIcon />
            </ListItemIcon>
          }
        >
          <ListItemButton component={Link} to="upload">
            <ListItemAvatar>
              <Avatar sx={{ bgcolor: '#5d89f7', color: 'white' }}>
                <ImportExportIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary={LL.UPLOAD_DOWNLOAD()} secondary="Upload/Download Settings and Firmware" />
          </ListItemButton>
        </ListItem>
      </List>
    </SectionContent>
  );
};

export default Settings;
