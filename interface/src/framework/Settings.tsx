import AccessTimeIcon from '@mui/icons-material/AccessTime';
import CancelIcon from '@mui/icons-material/Cancel';
import CastIcon from '@mui/icons-material/Cast';
import DeviceHubIcon from '@mui/icons-material/DeviceHub';
import ImportExportIcon from '@mui/icons-material/ImportExport';
import LockIcon from '@mui/icons-material/Lock';
import NavigateNextIcon from '@mui/icons-material/NavigateNext';
import PowerSettingsNewIcon from '@mui/icons-material/PowerSettingsNew';
import SettingsBackupRestoreIcon from '@mui/icons-material/SettingsBackupRestore';
import SettingsEthernetIcon from '@mui/icons-material/SettingsEthernet';
import SettingsInputAntennaIcon from '@mui/icons-material/SettingsInputAntenna';
import TuneIcon from '@mui/icons-material/Tune';

import {
  List,
  ListItem,
  ListItemAvatar,
  ListItemText,
  Avatar,
  ListItemButton,
  ListItemIcon,
  Button,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  Box
} from '@mui/material';
import { useRequest } from 'alova';
import { useState, type FC } from 'react';
import { Link } from 'react-router-dom';
import { toast } from 'react-toastify';
import RestartMonitor from './system/RestartMonitor';
import { dialogStyle } from 'CustomTheme';
import * as SystemApi from 'api/system';
import { ButtonRow, SectionContent, useLayoutTitle } from 'components';
import { useI18nContext } from 'i18n/i18n-react';

const Settings: FC = () => {
  const { LL } = useI18nContext();
  useLayoutTitle(LL.SETTINGS(0));

  const [confirmRestart, setConfirmRestart] = useState<boolean>(false);
  const [confirmFactoryReset, setConfirmFactoryReset] = useState<boolean>(false);
  const [processing, setProcessing] = useState<boolean>(false);
  const [restarting, setRestarting] = useState<boolean>();

  const { send: restartCommand } = useRequest(SystemApi.restart(), {
    immediate: false
  });

  const { send: factoryResetCommand } = useRequest(SystemApi.factoryReset(), {
    immediate: false
  });

  const { send: partitionCommand } = useRequest(SystemApi.partition(), {
    immediate: false
  });

  const restart = async () => {
    setProcessing(true);
    await restartCommand()
      .then(() => {
        setRestarting(true);
      })
      .catch((err) => {
        toast.error(err.message);
      })
      .finally(() => {
        setConfirmRestart(false);
        setProcessing(false);
      });
  };

  const factoryReset = async () => {
    setProcessing(true);
    await factoryResetCommand()
      .then(() => {
        setRestarting(true);
      })
      .catch((err) => {
        toast.error(err.message);
      })
      .finally(() => {
        setConfirmFactoryReset(false);
        setProcessing(false);
      });
  };

  const partition = async () => {
    setProcessing(true);
    await partitionCommand()
      .then(() => {
        setRestarting(true);
      })
      .catch((err) => {
        toast.error(err.message);
      })
      .finally(() => {
        setConfirmRestart(false);
        setProcessing(false);
      });
  };

  const renderRestartDialog = () => (
    <Dialog sx={dialogStyle} open={confirmRestart} onClose={() => setConfirmRestart(false)}>
      <DialogTitle>{LL.RESTART()}</DialogTitle>
      <DialogContent dividers>{LL.RESTART_CONFIRM()}</DialogContent>
      <DialogActions>
        <Button
          startIcon={<CancelIcon />}
          variant="outlined"
          onClick={() => setConfirmRestart(false)}
          disabled={processing}
          color="secondary"
        >
          {LL.CANCEL()}
        </Button>
        <Button
          startIcon={<PowerSettingsNewIcon />}
          variant="outlined"
          onClick={restart}
          disabled={processing}
          color="primary"
        >
          {LL.RESTART()}
        </Button>
        <Button
          startIcon={<PowerSettingsNewIcon />}
          variant="outlined"
          onClick={partition}
          disabled={processing}
          color="primary"
        >
          EMS-ESP-Loader
        </Button>
      </DialogActions>
    </Dialog>
  );

  const renderFactoryResetDialog = () => (
    <Dialog sx={dialogStyle} open={confirmFactoryReset} onClose={() => setConfirmFactoryReset(false)}>
      <DialogTitle>{LL.FACTORY_RESET()}</DialogTitle>
      <DialogContent dividers>{LL.SYSTEM_FACTORY_TEXT_DIALOG()}</DialogContent>
      <DialogActions>
        <Button
          startIcon={<CancelIcon />}
          variant="outlined"
          onClick={() => setConfirmFactoryReset(false)}
          disabled={processing}
          color="secondary"
        >
          {LL.CANCEL()}
        </Button>
        <Button
          startIcon={<SettingsBackupRestoreIcon />}
          variant="outlined"
          onClick={factoryReset}
          disabled={processing}
          color="error"
        >
          {LL.FACTORY_RESET()}
        </Button>
      </DialogActions>
    </Dialog>
  );

  const content = () => (
    <>
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
            <ListItemText primary={LL.APPLICATION_SETTINGS()} secondary="Modify EMS-ESP Application Settings" />
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
            <ListItemText primary={LL.SECURITY(0)} secondary="Add/Remove Users" />
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
      {renderRestartDialog()}
      {renderFactoryResetDialog()}
      <Box display="flex" flexWrap="wrap">
        <Box flexGrow={1} sx={{ '& button': { mt: 2 } }}>
          <ButtonRow>
            <Button
              startIcon={<PowerSettingsNewIcon />}
              variant="outlined"
              color="primary"
              onClick={() => setConfirmRestart(true)}
            >
              {LL.RESTART()}
            </Button>
          </ButtonRow>
        </Box>
        <Box flexWrap="nowrap" whiteSpace="nowrap">
          <ButtonRow>
            <Button
              startIcon={<SettingsBackupRestoreIcon />}
              variant="outlined"
              onClick={() => setConfirmFactoryReset(true)}
              color="error"
            >
              {LL.FACTORY_RESET()}
            </Button>
          </ButtonRow>
        </Box>
      </Box>
    </>
  );

  return <SectionContent>{restarting ? <RestartMonitor /> : content()}</SectionContent>;
};

export default Settings;
