import { useCallback, useContext, useMemo, useState } from 'react';
import { toast } from 'react-toastify';

import AccessTimeIcon from '@mui/icons-material/AccessTime';
import BuildIcon from '@mui/icons-material/Build';
import CancelIcon from '@mui/icons-material/Cancel';
import DeviceHubIcon from '@mui/icons-material/DeviceHub';
import DirectionsBusIcon from '@mui/icons-material/DirectionsBus';
import LogoDevIcon from '@mui/icons-material/LogoDev';
import MemoryIcon from '@mui/icons-material/Memory';
import MonitorHeartIcon from '@mui/icons-material/MonitorHeart';
import PowerSettingsNewIcon from '@mui/icons-material/PowerSettingsNew';
import RouterIcon from '@mui/icons-material/Router';
import SettingsInputAntennaIcon from '@mui/icons-material/SettingsInputAntenna';
import WifiIcon from '@mui/icons-material/Wifi';
import {
  Avatar,
  Button,
  Dialog,
  DialogActions,
  DialogContent,
  DialogTitle,
  List,
  ListItem,
  ListItemAvatar,
  ListItemText,
  useTheme
} from '@mui/material';

import { API } from 'api/app';
import { readSystemStatus } from 'api/system';

import { dialogStyle } from 'CustomTheme';
import { useRequest } from 'alova/client';
import { type APIcall, busConnectionStatus } from 'app/main/types';
import { FormLoader, SectionContent, useLayoutTitle } from 'components';
import ListMenuItem from 'components/layout/ListMenuItem';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import { NTPSyncStatus, NetworkConnectionStatus, SystemStatusCodes } from 'types';
import { useInterval } from 'utils';
import { formatDateTime } from 'utils/time';

import SystemMonitor from './SystemMonitor';

// Pure functions moved outside component to avoid recreation on each render
const formatNumber = (num: number) => new Intl.NumberFormat().format(num);

const formatDurationSec = (
  duration_sec: number,
  LL: ReturnType<typeof useI18nContext>['LL']
) => {
  const ms = duration_sec * 1000;
  const days = Math.trunc(ms / 86400000);
  const hours = Math.trunc(ms / 3600000) % 24;
  const minutes = Math.trunc(ms / 60000) % 60;
  const seconds = Math.trunc(ms / 1000) % 60;

  const parts: string[] = [];
  if (days) parts.push(LL.NUM_DAYS({ num: days }));
  if (hours) parts.push(LL.NUM_HOURS({ num: hours }));
  if (minutes) parts.push(LL.NUM_MINUTES({ num: minutes }));
  parts.push(LL.NUM_SECONDS({ num: seconds }));

  return parts.join(' ');
};

const SystemStatus = () => {
  const { LL } = useI18nContext();

  useLayoutTitle(LL.STATUS_OF(''));

  const { me } = useContext(AuthenticatedContext);

  const [confirmRestart, setConfirmRestart] = useState<boolean>(false);
  const [restarting, setRestarting] = useState<boolean>();

  const { send: sendAPI } = useRequest((data: APIcall) => API(data), {
    immediate: false
  });

  const {
    data,
    send: loadData,
    error
  } = useRequest(readSystemStatus, {
    async middleware(_, next) {
      if (!restarting) {
        await next();
      }
    }
  });

  useInterval(() => {
    void loadData();
  });

  const theme = useTheme();

  // Memoize derived status values to avoid recalculation on every render
  const busStatus = useMemo(() => {
    if (!data) return 'EMS state unknown';

    switch (data.bus_status) {
      case busConnectionStatus.BUS_STATUS_CONNECTED:
        return `EMS ${LL.CONNECTED(0)} (${formatDurationSec(data.bus_uptime, LL)})`;
      case busConnectionStatus.BUS_STATUS_TX_ERRORS:
        return 'EMS ' + LL.TX_ISSUES();
      case busConnectionStatus.BUS_STATUS_OFFLINE:
        return 'EMS ' + LL.DISCONNECTED();
      default:
        return 'EMS state unknown';
    }
  }, [data?.bus_status, data?.bus_uptime, LL]);

  // Memoize derived status values to avoid recalculation on every render
  const systemStatus = useMemo(() => {
    if (!data) return '??';

    switch (data.status) {
      case SystemStatusCodes.SYSTEM_STATUS_PENDING_UPLOAD:
      case SystemStatusCodes.SYSTEM_STATUS_UPLOADING:
        return LL.WAIT_FIRMWARE();
      case SystemStatusCodes.SYSTEM_STATUS_ERROR_UPLOAD:
        return LL.ERROR();
      case SystemStatusCodes.SYSTEM_STATUS_PENDING_RESTART:
      case SystemStatusCodes.SYSTEM_STATUS_RESTART_REQUESTED:
        return LL.RESTARTING_PRE();
      case SystemStatusCodes.SYSTEM_STATUS_INVALID_GPIO:
        return LL.GPIO_OF(LL.FAILED(0));
      default:
        // SystemStatusCodes.SYSTEM_STATUS_NORMAL
        return 'OK';
    }
  }, [data?.status, LL]);

  const busStatusHighlight = useMemo(() => {
    if (!data) return theme.palette.warning.main;

    switch (data.bus_status) {
      case busConnectionStatus.BUS_STATUS_TX_ERRORS:
        return theme.palette.warning.main;
      case busConnectionStatus.BUS_STATUS_CONNECTED:
        return theme.palette.success.main;
      case busConnectionStatus.BUS_STATUS_OFFLINE:
        return theme.palette.error.main;
      default:
        return theme.palette.warning.main;
    }
  }, [data?.bus_status, theme.palette]);

  const ntpStatus = useMemo(() => {
    if (!data) return LL.UNKNOWN();

    switch (data.ntp_status) {
      case NTPSyncStatus.NTP_DISABLED:
        return LL.NOT_ENABLED();
      case NTPSyncStatus.NTP_INACTIVE:
        return LL.INACTIVE(0);
      case NTPSyncStatus.NTP_ACTIVE:
        return data.ntp_time
          ? `${LL.ACTIVE()} (${formatDateTime(data.ntp_time)})`
          : LL.ACTIVE();
      default:
        return LL.UNKNOWN();
    }
  }, [data?.ntp_status, data?.ntp_time, LL]);

  const ntpStatusHighlight = useMemo(() => {
    if (!data) return theme.palette.error.main;

    switch (data.ntp_status) {
      case NTPSyncStatus.NTP_DISABLED:
        return theme.palette.info.main;
      case NTPSyncStatus.NTP_INACTIVE:
        return theme.palette.error.main;
      case NTPSyncStatus.NTP_ACTIVE:
        return theme.palette.success.main;
      default:
        return theme.palette.error.main;
    }
  }, [data?.ntp_status, theme.palette]);

  const networkStatusHighlight = useMemo(() => {
    if (!data) return theme.palette.warning.main;

    switch (data.network_status) {
      case NetworkConnectionStatus.WIFI_STATUS_IDLE:
      case NetworkConnectionStatus.WIFI_STATUS_DISCONNECTED:
      case NetworkConnectionStatus.WIFI_STATUS_NO_SHIELD:
        return theme.palette.info.main;
      case NetworkConnectionStatus.WIFI_STATUS_CONNECTED:
      case NetworkConnectionStatus.ETHERNET_STATUS_CONNECTED:
        return theme.palette.success.main;
      case NetworkConnectionStatus.WIFI_STATUS_CONNECT_FAILED:
      case NetworkConnectionStatus.WIFI_STATUS_CONNECTION_LOST:
        return theme.palette.error.main;
      default:
        return theme.palette.warning.main;
    }
  }, [data?.network_status, theme.palette]);

  const networkStatus = useMemo(() => {
    if (!data) return LL.UNKNOWN();

    switch (data.network_status) {
      case NetworkConnectionStatus.WIFI_STATUS_NO_SHIELD:
        return LL.INACTIVE(1);
      case NetworkConnectionStatus.WIFI_STATUS_IDLE:
        return LL.IDLE();
      case NetworkConnectionStatus.WIFI_STATUS_NO_SSID_AVAIL:
        return 'No SSID Available';
      case NetworkConnectionStatus.WIFI_STATUS_CONNECTED:
        return `${LL.CONNECTED(0)} (WiFi, ${data.wifi_rssi} dBm)`;
      case NetworkConnectionStatus.ETHERNET_STATUS_CONNECTED:
        return `${LL.CONNECTED(0)} (Ethernet)`;
      case NetworkConnectionStatus.WIFI_STATUS_CONNECT_FAILED:
        return `${LL.CONNECTED(1)} ${LL.FAILED(0)}`;
      case NetworkConnectionStatus.WIFI_STATUS_CONNECTION_LOST:
        return `${LL.CONNECTED(1)} ${LL.LOST()}`;
      case NetworkConnectionStatus.WIFI_STATUS_DISCONNECTED:
        return LL.DISCONNECTED();
      default:
        return LL.UNKNOWN();
    }
  }, [data?.network_status, data?.wifi_rssi, LL]);

  const activeHighlight = useCallback(
    (value: boolean) =>
      value ? theme.palette.success.main : theme.palette.info.main,
    [theme.palette]
  );

  const doRestart = useCallback(async () => {
    setConfirmRestart(false);
    setRestarting(true);
    await sendAPI({ device: 'system', cmd: 'restart', id: 0 }).catch(
      (error: Error) => {
        toast.error(error.message);
      }
    );
  }, [sendAPI]);

  const handleCloseRestartDialog = useCallback(() => {
    setConfirmRestart(false);
  }, []);

  const renderRestartDialog = useMemo(
    () => (
      <Dialog
        sx={dialogStyle}
        open={confirmRestart}
        onClose={handleCloseRestartDialog}
      >
        <DialogTitle>{LL.RESTART()}</DialogTitle>
        <DialogContent dividers>{LL.RESTART_CONFIRM()}</DialogContent>
        <DialogActions>
          <Button
            startIcon={<CancelIcon />}
            variant="outlined"
            onClick={handleCloseRestartDialog}
            color="secondary"
          >
            {LL.CANCEL()}
          </Button>
          <Button
            startIcon={<PowerSettingsNewIcon />}
            variant="outlined"
            onClick={doRestart}
            color="error"
          >
            {LL.RESTART()}
          </Button>
        </DialogActions>
      </Dialog>
    ),
    [confirmRestart, handleCloseRestartDialog, doRestart, LL]
  );

  // Memoize formatted values
  const firmwareVersion = useMemo(
    () => `v${data?.emsesp_version || ''}`,
    [data?.emsesp_version]
  );

  const uptimeText = useMemo(
    () => (data ? formatDurationSec(data.uptime, LL) : ''),
    [data?.uptime, LL]
  );

  const freeMemoryText = useMemo(
    () => (data ? `${formatNumber(data.free_heap)} KB ${LL.FREE_MEMORY()}` : ''),
    [data?.free_heap, LL]
  );

  const networkIcon = useMemo(
    () =>
      data?.network_status === NetworkConnectionStatus.WIFI_STATUS_CONNECTED
        ? WifiIcon
        : RouterIcon,
    [data?.network_status]
  );

  const mqttStatusText = useMemo(
    () => (data?.mqtt_status ? LL.CONNECTED(0) : LL.INACTIVE(0)),
    [data?.mqtt_status, LL]
  );

  const apStatusText = useMemo(
    () => (data?.ap_status ? LL.ACTIVE() : LL.INACTIVE(0)),
    [data?.ap_status, LL]
  );

  const handleRestartClick = useCallback(() => {
    setConfirmRestart(true);
  }, []);

  const content = useMemo(() => {
    if (!data || !LL) {
      return <FormLoader onRetry={loadData} errorMessage={error?.message || ''} />;
    }

    return (
      <>
        <List>
          <ListMenuItem
            icon={BuildIcon}
            bgcolor="#72caf9"
            label="EMS-ESP Firmware"
            text={firmwareVersion}
            to="version"
          />

          <ListItem>
            <ListItemAvatar>
              <Avatar sx={{ bgcolor: '#c5572c', color: 'white' }}>
                <MonitorHeartIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText
              primary={LL.STATUS_OF(LL.SYSTEM(0))}
              secondary={`${systemStatus} (${LL.UPTIME()}: ${uptimeText})`}
            />
            {me.admin && (
              <Button
                startIcon={<PowerSettingsNewIcon />}
                variant="outlined"
                color="error"
                onClick={handleRestartClick}
              >
                {LL.RESTART()}
              </Button>
            )}
          </ListItem>

          <ListMenuItem
            disabled={!me.admin}
            icon={MemoryIcon}
            bgcolor="#68374d"
            label={LL.HARDWARE()}
            text={freeMemoryText}
            to="/status/hardwarestatus"
          />

          <ListMenuItem
            disabled={!me.admin}
            icon={DirectionsBusIcon}
            bgcolor={busStatusHighlight}
            label={LL.DATA_TRAFFIC()}
            text={busStatus}
            to="/status/activity"
          />

          <ListMenuItem
            disabled={!me.admin}
            icon={networkIcon}
            bgcolor={networkStatusHighlight}
            label={LL.NETWORK(1)}
            text={networkStatus}
            to="/status/network"
          />

          <ListMenuItem
            disabled={!me.admin}
            icon={DeviceHubIcon}
            bgcolor={activeHighlight(data.mqtt_status)}
            label="MQTT"
            text={mqttStatusText}
            to="/status/mqtt"
          />

          <ListMenuItem
            disabled={!me.admin}
            icon={AccessTimeIcon}
            bgcolor={ntpStatusHighlight}
            label="NTP"
            text={ntpStatus}
            to="/status/ntp"
          />

          <ListMenuItem
            disabled={!me.admin}
            icon={SettingsInputAntennaIcon}
            bgcolor={activeHighlight(data.ap_status)}
            label={LL.ACCESS_POINT(0)}
            text={apStatusText}
            to="/status/ap"
          />

          <ListMenuItem
            disabled={!me.admin}
            icon={LogoDevIcon}
            bgcolor="#40828f"
            label={LL.LOG_OF(LL.SYSTEM(0))}
            text={LL.VIEW_LOG()}
            to="/status/log"
          />
        </List>

        {renderRestartDialog}
      </>
    );
  }, [
    data,
    LL,
    firmwareVersion,
    uptimeText,
    freeMemoryText,
    networkIcon,
    mqttStatusText,
    apStatusText,
    busStatus,
    busStatusHighlight,
    networkStatusHighlight,
    networkStatus,
    ntpStatusHighlight,
    ntpStatus,
    activeHighlight,
    me.admin,
    handleRestartClick,
    error,
    loadData,
    renderRestartDialog
  ]);

  return <SectionContent>{restarting ? <SystemMonitor /> : content}</SectionContent>;
};

export default SystemStatus;
