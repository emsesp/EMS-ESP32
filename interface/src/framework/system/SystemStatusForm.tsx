import { FC, useContext, useState, useEffect } from 'react';
import { useSnackbar } from 'notistack';
import {
  Avatar,
  Box,
  Button,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  Divider,
  List,
  ListItem,
  ListItemAvatar,
  ListItemText,
  Link,
  Typography
} from '@mui/material';

import DevicesIcon from '@mui/icons-material/Devices';
import ShowChartIcon from '@mui/icons-material/ShowChart';
import MemoryIcon from '@mui/icons-material/Memory';
import AppsIcon from '@mui/icons-material/Apps';
import SdStorageIcon from '@mui/icons-material/SdStorage';
import FolderIcon from '@mui/icons-material/Folder';
import RefreshIcon from '@mui/icons-material/Refresh';
import PowerSettingsNewIcon from '@mui/icons-material/PowerSettingsNew';
import SettingsBackupRestoreIcon from '@mui/icons-material/SettingsBackupRestore';
import BuildIcon from '@mui/icons-material/Build';
import TimerIcon from '@mui/icons-material/Timer';

import { ButtonRow, FormLoader, SectionContent } from '../../components';
import { EspPlatform, SystemStatus, Version } from '../../types';
import * as SystemApi from '../../api/system';
import { extractErrorMessage, useRest } from '../../utils';

import { AuthenticatedContext } from '../../contexts/authentication';

import axios from 'axios';

export const VERSIONCHECK_ENDPOINT = 'https://api.github.com/repos/emsesp/EMS-ESP32/releases/latest';
export const VERSIONCHECK_DEV_ENDPOINT = 'https://api.github.com/repos/emsesp/EMS-ESP32/releases/tags/latest';
export const uploadURL = window.location.origin + '/system/upload';

function formatNumber(num: number) {
  return new Intl.NumberFormat().format(num);
}

const SystemStatusForm: FC = () => {
  const { loadData, data, errorMessage } = useRest<SystemStatus>({ read: SystemApi.readSystemStatus });

  const { me } = useContext(AuthenticatedContext);
  const [confirmRestart, setConfirmRestart] = useState<boolean>(false);
  const [confirmFactoryReset, setConfirmFactoryReset] = useState<boolean>(false);
  const [processing, setProcessing] = useState<boolean>(false);
  const { enqueueSnackbar } = useSnackbar();
  const [showingVersion, setShowingVersion] = useState<boolean>(false);
  const [latestVersion, setLatestVersion] = useState<Version>();
  const [latestDevVersion, setLatestDevVersion] = useState<Version>();

  useEffect(() => {
    axios.get(VERSIONCHECK_ENDPOINT).then((response) => {
      setLatestVersion({
        version: response.data.name,
        url: response.data.assets[1].browser_download_url,
        changelog: response.data.html_url
      });
    });
    axios.get(VERSIONCHECK_DEV_ENDPOINT).then((response) => {
      setLatestDevVersion({
        version: response.data.name.split(/\s+/).splice(-1),
        url: response.data.assets[1].browser_download_url,
        changelog: response.data.assets[0].browser_download_url
      });
    });
  }, []);

  const restart = async () => {
    setProcessing(true);
    try {
      await SystemApi.restart();
      enqueueSnackbar('EMS-ESP is restarting', { variant: 'info' });
    } catch (error: any) {
      enqueueSnackbar(extractErrorMessage(error, 'Problem restarting device'), { variant: 'error' });
    } finally {
      setConfirmRestart(false);
      setProcessing(false);
    }
  };

  const renderRestartDialog = () => (
    <Dialog open={confirmRestart} onClose={() => setConfirmRestart(false)}>
      <DialogTitle>Restart</DialogTitle>
      <DialogContent dividers>Are you sure you want to restart EMS-ESP?</DialogContent>
      <DialogActions>
        <Button variant="outlined" onClick={() => setConfirmRestart(false)} color="secondary">
          Cancel
        </Button>
        <Button
          startIcon={<PowerSettingsNewIcon />}
          variant="outlined"
          onClick={restart}
          disabled={processing}
          color="primary"
          autoFocus
        >
          Restart
        </Button>
      </DialogActions>
    </Dialog>
  );

  const renderVersionDialog = () => {
    return (
      <Dialog open={showingVersion} onClose={() => setShowingVersion(false)}>
        <DialogTitle>Version Information</DialogTitle>
        <DialogContent dividers>
          <Box bgcolor="primary.main" color="primary.contrastText" p={2} mt={2} mb={2}>
            <Typography variant="body1">
              You are currently running EMS-ESP version <b>v{data?.emsesp_version}</b>
            </Typography>
          </Box>
          {latestVersion && (
            <Box mt={2} mb={2}>
              The latest <u>stable</u> version is <b>{latestVersion.version}</b>&nbsp;(
              <Link target="_blank" href={latestVersion.changelog} color="primary">
                {'release notes'}
              </Link>
              )&nbsp;(
              <Link target="_blank" href={latestVersion.url} color="primary">
                {'download'}
              </Link>
              )
            </Box>
          )}

          {latestDevVersion && (
            <Box mt={2} mb={2}>
              The latest <u>development</u> version is&nbsp;<b>{latestDevVersion.version}</b>
              &nbsp;(
              <Link target="_blank" href={latestDevVersion.changelog} color="primary">
                {'release notes'}
              </Link>
              )&nbsp;(
              <Link target="_blank" href={latestDevVersion.url} color="primary">
                {'download'}
              </Link>
              )
            </Box>
          )}

          <Box color="warning.main" p={0} pl={0} pr={0} mt={4} mb={0}>
            <Typography variant="body2">
              Upgrade to a new version using&nbsp;
              <Link target="_blank" href={uploadURL} color="primary">
                {'UPLOAD FIRMWARE'}
              </Link>
            </Typography>
          </Box>
        </DialogContent>
        <DialogActions>
          <Button variant="outlined" onClick={() => setShowingVersion(false)} color="secondary">
            Close
          </Button>
        </DialogActions>
      </Dialog>
    );
  };

  const factoryReset = async () => {
    setProcessing(true);
    try {
      await SystemApi.factoryReset();
      enqueueSnackbar('Device has been factory reset and will now restart', { variant: 'info' });
    } catch (error: any) {
      enqueueSnackbar(extractErrorMessage(error, 'Problem factory resetting the device'), { variant: 'error' });
    } finally {
      setConfirmFactoryReset(false);
      setProcessing(false);
    }
  };

  const renderFactoryResetDialog = () => (
    <Dialog open={confirmFactoryReset} onClose={() => setConfirmFactoryReset(false)}>
      <DialogTitle>Factory Reset</DialogTitle>
      <DialogContent dividers>Are you sure you want to reset the device to its factory defaults?</DialogContent>
      <DialogActions>
        <Button variant="outlined" onClick={() => setConfirmFactoryReset(false)} color="secondary">
          Cancel
        </Button>
        <Button
          startIcon={<SettingsBackupRestoreIcon />}
          variant="outlined"
          onClick={factoryReset}
          disabled={processing}
          autoFocus
          color="error"
        >
          Factory Reset
        </Button>
      </DialogActions>
    </Dialog>
  );

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage} />;
    }

    return (
      <>
        <List>
          <ListItem>
            <ListItemAvatar>
              <Avatar>
                <BuildIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary="EMS-ESP Version" secondary={'v' + data.emsesp_version} />
            {latestVersion && (
              <Button color="primary" onClick={() => setShowingVersion(true)}>
                Version Information&hellip;
              </Button>
            )}
          </ListItem>
          <Divider variant="inset" component="li" />
          <ListItem>
            <ListItemAvatar>
              <Avatar>
                <DevicesIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary="Device (Platform / SDK)" secondary={data.esp_platform + ' / ' + data.sdk_version} />
          </ListItem>
          <Divider variant="inset" component="li" />
          <ListItem>
            <ListItemAvatar>
              <Avatar>
                <TimerIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary="System Uptime" secondary={data.uptime} />
          </ListItem>
          <Divider variant="inset" component="li" />
          <ListItem>
            <ListItemAvatar>
              <Avatar>
                <ShowChartIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary="CPU Frequency" secondary={data.cpu_freq_mhz + ' MHz'} />
          </ListItem>
          <Divider variant="inset" component="li" />
          <ListItem>
            <ListItemAvatar>
              <Avatar>
                <MemoryIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText
              primary="Heap (Free / Max Alloc)"
              secondary={
                formatNumber(data.free_heap) +
                ' / ' +
                formatNumber(data.max_alloc_heap) +
                ' bytes ' +
                (data.esp_platform === EspPlatform.ESP8266 ? '(' + data.heap_fragmentation + '% fragmentation)' : '')
              }
            />
          </ListItem>
          {data.esp_platform === EspPlatform.ESP32 && data.psram_size > 0 && (
            <>
              <Divider variant="inset" component="li" />
              <ListItem>
                <ListItemAvatar>
                  <Avatar>
                    <AppsIcon />
                  </Avatar>
                </ListItemAvatar>
                <ListItemText
                  primary="PSRAM (Size / Free)"
                  secondary={formatNumber(data.psram_size) + ' / ' + formatNumber(data.free_psram) + ' bytes'}
                />
              </ListItem>
            </>
          )}
          <Divider variant="inset" component="li" />
          <ListItem>
            <ListItemAvatar>
              <Avatar>
                <SdStorageIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText
              primary="Flash Chip (Size / Speed)"
              secondary={
                formatNumber(data.flash_chip_size) + ' bytes / ' + (data.flash_chip_speed / 1000000).toFixed(0) + ' MHz'
              }
            />
          </ListItem>
          <Divider variant="inset" component="li" />
          <ListItem>
            <ListItemAvatar>
              <Avatar>
                <FolderIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText
              primary="File System (Used / Total)"
              secondary={
                formatNumber(data.fs_used) +
                ' / ' +
                formatNumber(data.fs_total) +
                ' bytes (' +
                formatNumber(data.fs_total - data.fs_used) +
                '\xa0bytes free)'
              }
            />
          </ListItem>
          <Divider variant="inset" component="li" />
        </List>
        <Box display="flex" flexWrap="wrap">
          <Box flexGrow={1} sx={{ '& button': { mt: 2 } }}>
            <ButtonRow>
              <Button startIcon={<RefreshIcon />} variant="outlined" color="secondary" onClick={loadData}>
                Refresh
              </Button>
            </ButtonRow>
          </Box>
          {me.admin && (
            <Box flexWrap="nowrap" whiteSpace="nowrap">
              <ButtonRow>
                <Button
                  startIcon={<PowerSettingsNewIcon />}
                  variant="outlined"
                  color="primary"
                  onClick={() => setConfirmRestart(true)}
                >
                  Restart
                </Button>
                <Button
                  startIcon={<SettingsBackupRestoreIcon />}
                  variant="outlined"
                  onClick={() => setConfirmFactoryReset(true)}
                  color="error"
                >
                  Factory reset
                </Button>
              </ButtonRow>
            </Box>
          )}
        </Box>
        {renderVersionDialog()}
        {renderRestartDialog()}
        {renderFactoryResetDialog()}
      </>
    );
  };

  return (
    <SectionContent title="System Status" titleGutter>
      {content()}
    </SectionContent>
  );
};

export default SystemStatusForm;
