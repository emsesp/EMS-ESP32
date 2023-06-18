import AppsIcon from '@mui/icons-material/Apps';
import BuildIcon from '@mui/icons-material/Build';
import CancelIcon from '@mui/icons-material/Cancel';
import DevicesIcon from '@mui/icons-material/Devices';
import FolderIcon from '@mui/icons-material/Folder';
import MemoryIcon from '@mui/icons-material/Memory';
import PowerSettingsNewIcon from '@mui/icons-material/PowerSettingsNew';
import RefreshIcon from '@mui/icons-material/Refresh';
import SdCardAlertIcon from '@mui/icons-material/SdCardAlert';
import SdStorageIcon from '@mui/icons-material/SdStorage';
import SettingsBackupRestoreIcon from '@mui/icons-material/SettingsBackupRestore';
import ShowChartIcon from '@mui/icons-material/ShowChart';
import TimerIcon from '@mui/icons-material/Timer';
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

import { useRequest } from 'alova';
import axios from 'axios';
import { useContext, useState, useEffect } from 'react';
import { toast } from 'react-toastify';
import RestartMonitor from './RestartMonitor';
import type { FC } from 'react';

import type { Version } from 'types';
import * as SystemApi from 'api/system';
import { ButtonRow, FormLoader, SectionContent, MessageBox } from 'components';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';

export const VERSIONCHECK_ENDPOINT = 'https://api.github.com/repos/emsesp/EMS-ESP32/releases/latest';
export const VERSIONCHECK_DEV_ENDPOINT = 'https://api.github.com/repos/emsesp/EMS-ESP32/releases/tags/latest';
export const uploadURL = window.location.origin + '/system/upload';

function formatNumber(num: number) {
  return new Intl.NumberFormat().format(num);
}

const SystemStatusForm: FC = () => {
  const { LL } = useI18nContext();

  const { me } = useContext(AuthenticatedContext);
  const [confirmRestart, setConfirmRestart] = useState<boolean>(false);
  const [confirmFactoryReset, setConfirmFactoryReset] = useState<boolean>(false);
  const [processing, setProcessing] = useState<boolean>(false);
  const [showingVersion, setShowingVersion] = useState<boolean>(false);
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

  const { data: latestVersion } = useRequest(SystemApi.getStableVersion);
  const { data: latestDevVersion } = useRequest(SystemApi.getDevVersion);

  const { data: data, send: loadData, error } = useRequest(SystemApi.readSystemStatus, { force: true });

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
    <Dialog open={confirmRestart} onClose={() => setConfirmRestart(false)}>
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
        {data?.has_loader && (
          <Button
            startIcon={<PowerSettingsNewIcon />}
            variant="outlined"
            onClick={partition}
            disabled={processing}
            color="primary"
          >
            EMS-ESP-Loader
          </Button>
        )}
      </DialogActions>
    </Dialog>
  );

  const renderVersionDialog = () => (
    <Dialog open={showingVersion} onClose={() => setShowingVersion(false)}>
      <DialogTitle>{LL.VERSION_CHECK(1)}</DialogTitle>
      <DialogContent dividers>
        <MessageBox my={0} level="info" message={LL.VERSION_ON() + ' v' + data?.emsesp_version} />
        {latestVersion && (
          <Box mt={2} mb={2}>
            {LL.THE_LATEST()}&nbsp;<u>{LL.OFFICIAL()}</u>&nbsp;{LL.RELEASE_IS()}&nbsp;<b>{latestVersion.version}</b>
            &nbsp;(
            <Link target="_blank" href={latestVersion.changelog} color="primary">
              {LL.RELEASE_NOTES()}
            </Link>
            )&nbsp;(
            <Link target="_blank" href={latestVersion.url} color="primary">
              {LL.DOWNLOAD(1)}
            </Link>
            )
          </Box>
        )}

        {latestDevVersion && (
          <Box mt={2} mb={2}>
            {LL.THE_LATEST()}&nbsp;<u>{LL.DEVELOPMENT()}</u>&nbsp;{LL.RELEASE_IS()}&nbsp;
            <b>{latestDevVersion.version}</b>
            &nbsp;(
            <Link target="_blank" href={latestDevVersion.changelog} color="primary">
              {LL.RELEASE_NOTES()}
            </Link>
            )&nbsp;(
            <Link target="_blank" href={latestDevVersion.url} color="primary">
              {LL.DOWNLOAD(1)}
            </Link>
            )
          </Box>
        )}

        <Box color="warning.main" p={0} pl={0} pr={0} mt={4} mb={0}>
          <Typography variant="body2">
            {LL.USE()}&nbsp;
            <Link href={uploadURL} color="primary">
              {LL.UPLOAD()}
            </Link>
            &nbsp;{LL.SYSTEM_APPLY_FIRMWARE()}
          </Typography>
        </Box>
      </DialogContent>
      <DialogActions>
        <Button variant="outlined" onClick={() => setShowingVersion(false)} color="secondary">
          {LL.CLOSE()}
        </Button>
      </DialogActions>
    </Dialog>
  );

  const renderFactoryResetDialog = () => (
    <Dialog open={confirmFactoryReset} onClose={() => setConfirmFactoryReset(false)}>
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

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={error?.message} />;
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
            <ListItemText primary={LL.EMS_ESP_VER()} secondary={'v' + data.emsesp_version} />
            {latestVersion && (
              <Button color="primary" onClick={() => setShowingVersion(true)}>
                {LL.VERSION_CHECK(0)}
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
            <ListItemText primary={LL.PLATFORM()} secondary={data.esp_platform + ' / ' + data.sdk_version} />
          </ListItem>
          <Divider variant="inset" component="li" />
          <ListItem>
            <ListItemAvatar>
              <Avatar>
                <TimerIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary={LL.UPTIME()} secondary={data.uptime} />
          </ListItem>
          <Divider variant="inset" component="li" />
          <ListItem>
            <ListItemAvatar>
              <Avatar>
                <ShowChartIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary={LL.CPU_FREQ()} secondary={data.cpu_freq_mhz + ' MHz'} />
          </ListItem>
          <Divider variant="inset" component="li" />
          <ListItem>
            <ListItemAvatar>
              <Avatar>
                <MemoryIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText
              primary={LL.HEAP()}
              secondary={formatNumber(data.free_heap) + ' KB / ' + formatNumber(data.max_alloc_heap) + ' KB '}
            />
          </ListItem>
          {data.psram_size !== undefined && data.free_psram !== undefined && (
            <>
              <Divider variant="inset" component="li" />
              <ListItem>
                <ListItemAvatar>
                  <Avatar>
                    <AppsIcon />
                  </Avatar>
                </ListItemAvatar>
                <ListItemText
                  primary={LL.PSRAM()}
                  secondary={formatNumber(data.psram_size) + ' KB / ' + formatNumber(data.free_psram) + ' KB'}
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
              primary={LL.FLASH()}
              secondary={
                formatNumber(data.flash_chip_size) + ' KB / ' + (data.flash_chip_speed / 1000000).toFixed(0) + ' MHz'
              }
            />
          </ListItem>
          <Divider variant="inset" component="li" />
          <ListItem>
            <ListItemAvatar>
              <Avatar>
                <SdCardAlertIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText
              primary={LL.APPSIZE()}
              secondary={formatNumber(data.app_used) + ' KB / ' + formatNumber(data.app_free) + ' KB'}
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
              primary={LL.FILESYSTEM()}
              secondary={formatNumber(data.fs_used) + ' KB / ' + formatNumber(data.fs_free) + ' KB'}
            />
          </ListItem>
          <Divider variant="inset" component="li" />
        </List>
        <Box display="flex" flexWrap="wrap">
          <Box flexGrow={1} sx={{ '& button': { mt: 2 } }}>
            <ButtonRow>
              <Button startIcon={<RefreshIcon />} variant="outlined" color="secondary" onClick={loadData}>
                {LL.REFRESH()}
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
                  {LL.RESTART()}
                </Button>
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
          )}
        </Box>
        {renderVersionDialog()}
        {renderRestartDialog()}
        {renderFactoryResetDialog()}
      </>
    );
  };

  return (
    <SectionContent title={LL.STATUS_OF(LL.SYSTEM(1))} titleGutter>
      {restarting ? <RestartMonitor /> : content()}
    </SectionContent>
  );
};

export default SystemStatusForm;
