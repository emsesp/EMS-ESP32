import { useContext, useState } from 'react';
import { useNavigate } from 'react-router-dom';
import { toast } from 'react-toastify';

import AccessTimeIcon from '@mui/icons-material/AccessTime';
import BuildIcon from '@mui/icons-material/Build';
import CancelIcon from '@mui/icons-material/Cancel';
import DeviceHubIcon from '@mui/icons-material/DeviceHub';
import DirectionsBusIcon from '@mui/icons-material/DirectionsBus';
import LogoDevIcon from '@mui/icons-material/LogoDev';
import MemoryIcon from '@mui/icons-material/Memory';
import PowerSettingsNewIcon from '@mui/icons-material/PowerSettingsNew';
import RouterIcon from '@mui/icons-material/Router';
import SettingsInputAntennaIcon from '@mui/icons-material/SettingsInputAntenna';
import TimerIcon from '@mui/icons-material/Timer';
import UpgradeIcon from '@mui/icons-material/Upgrade';
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
import { useAutoRequest, useRequest } from 'alova/client';
import { type APIcall, busConnectionStatus } from 'app/main/types';
import { FormLoader, SectionContent, useLayoutTitle } from 'components';
import ListMenuItem from 'components/layout/ListMenuItem';
import { AuthenticatedContext } from 'contexts/authentication';
import { useI18nContext } from 'i18n/i18n-react';
import { NTPSyncStatus, NetworkConnectionStatus } from 'types';

import RestartMonitor from './RestartMonitor';

const SystemStatus = () => {
  const { LL } = useI18nContext();

  const navigate = useNavigate();

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
  } = useAutoRequest(readSystemStatus, {
    initialData: [],
    pollingTime: 3000,
    async middleware(_, next) {
      if (!restarting) {
        await next();
      }
    }
  });

  const theme = useTheme();

  const formatDurationSec = (duration_sec: number) => {
    const days = Math.trunc((duration_sec * 1000) / 86400000);
    const hours = Math.trunc((duration_sec * 1000) / 3600000) % 24;
    const minutes = Math.trunc((duration_sec * 1000) / 60000) % 60;
    const seconds = Math.trunc((duration_sec * 1000) / 1000) % 60;

    let formatted = '';
    if (days) {
      formatted += LL.NUM_DAYS({ num: days }) + ' ';
    }
    if (hours) {
      formatted += LL.NUM_HOURS({ num: hours }) + ' ';
    }
    if (minutes) {
      formatted += LL.NUM_MINUTES({ num: minutes }) + ' ';
    }
    formatted += LL.NUM_SECONDS({ num: seconds });
    return formatted;
  };

  function formatNumber(num: number) {
    return new Intl.NumberFormat().format(num);
  }

  const busStatus = () => {
    if (data) {
      switch (data.bus_status) {
        case busConnectionStatus.BUS_STATUS_CONNECTED:
          return (
            'EMS ' +
            LL.CONNECTED(0) +
            ' (' +
            formatDurationSec(data.bus_uptime) +
            ')'
          );
        case busConnectionStatus.BUS_STATUS_TX_ERRORS:
          return 'EMS ' + LL.TX_ISSUES();
        case busConnectionStatus.BUS_STATUS_OFFLINE:
          return 'EMS ' + LL.DISCONNECTED();
      }
    }
    return 'EMS state unknown';
  };

  const busStatusHighlight = () => {
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
  };

  const ntpStatus = () => {
    switch (data.ntp_status) {
      case NTPSyncStatus.NTP_DISABLED:
        return LL.NOT_ENABLED();
      case NTPSyncStatus.NTP_INACTIVE:
        return LL.INACTIVE(0);
      case NTPSyncStatus.NTP_ACTIVE:
        return LL.ACTIVE();
      default:
        return LL.UNKNOWN();
    }
  };

  const ntpStatusHighlight = () => {
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
  };

  const networkStatusHighlight = () => {
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
  };

  const networkStatus = () => {
    switch (data.network_status) {
      case NetworkConnectionStatus.WIFI_STATUS_NO_SHIELD:
        return LL.INACTIVE(1);
      case NetworkConnectionStatus.WIFI_STATUS_IDLE:
        return LL.IDLE();
      case NetworkConnectionStatus.WIFI_STATUS_NO_SSID_AVAIL:
        return 'No SSID Available';
      case NetworkConnectionStatus.WIFI_STATUS_CONNECTED:
        return LL.CONNECTED(0) + ' (WiFi, ' + data.wifi_rssi + ' dBm)';
      case NetworkConnectionStatus.ETHERNET_STATUS_CONNECTED:
        return LL.CONNECTED(0) + ' (Ethernet)';
      case NetworkConnectionStatus.WIFI_STATUS_CONNECT_FAILED:
        return LL.CONNECTED(1) + ' ' + LL.FAILED(0);
      case NetworkConnectionStatus.WIFI_STATUS_CONNECTION_LOST:
        return LL.CONNECTED(1) + ' ' + LL.LOST();
      case NetworkConnectionStatus.WIFI_STATUS_DISCONNECTED:
        return LL.DISCONNECTED();
      default:
        return LL.UNKNOWN();
    }
  };

  const activeHighlight = (value: boolean) =>
    value ? theme.palette.success.main : theme.palette.info.main;

  const doRestart = async () => {
    setConfirmRestart(false);
    setRestarting(true);
    await sendAPI({ device: 'system', cmd: 'restart', id: 0 }).catch(
      (error: Error) => {
        toast.error(error.message);
      }
    );
  };

  const renderRestartDialog = () => (
    <Dialog
      sx={dialogStyle}
      open={confirmRestart}
      onClose={() => setConfirmRestart(false)}
    >
      <DialogTitle>{LL.RESTART()}</DialogTitle>
      <DialogContent dividers>{LL.RESTART_CONFIRM()}</DialogContent>
      <DialogActions>
        <Button
          startIcon={<CancelIcon />}
          variant="outlined"
          onClick={() => setConfirmRestart(false)}
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
  );

  const content = () => {
    if (!data || !LL) {
      return <FormLoader onRetry={loadData} errorMessage={error?.message} />;
    }

    return (
      <>
        <List sx={{ borderRadius: 3, border: '2px solid grey' }}>
          <ListItem>
            <ListItemAvatar>
              <Avatar sx={{ bgcolor: '#134ba2', color: 'white' }}>
                <BuildIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText
              primary={LL.EMS_ESP_VER()}
              secondary={'' + data.emsesp_version}
            />
            {me.admin && (
              <Button
                startIcon={<UpgradeIcon />}
                variant="outlined"
                color="primary"
                onClick={() => navigate('/settings/upload')}
              >
                {LL.UPDATE()}
              </Button>
            )}
          </ListItem>

          <ListItem>
            <ListItemAvatar>
              <Avatar sx={{ bgcolor: '#c5572c', color: 'white' }}>
                <TimerIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText
              primary={LL.UPTIME()}
              secondary={formatDurationSec(data.uptime)}
            />
            {me.admin && (
              <Button
                startIcon={<PowerSettingsNewIcon />}
                variant="outlined"
                color="error"
                onClick={() => setConfirmRestart(true)}
              >
                {LL.RESTART()}
              </Button>
            )}
          </ListItem>

          <ListMenuItem
            disabled={!me.admin}
            icon={DirectionsBusIcon}
            bgcolor={busStatusHighlight()}
            label={LL.DATA_TRAFFIC()}
            text={busStatus()}
            to="/status/activity"
          />

          <ListMenuItem
            disabled={!me.admin}
            icon={MemoryIcon}
            bgcolor="#68374d"
            label={LL.STATUS_OF(LL.HARDWARE())}
            text={formatNumber(data.free_heap) + ' KB' + ' ' + LL.FREE_MEMORY()}
            to="/status/hardwarestatus"
          />

          <ListMenuItem
            disabled={!me.admin}
            icon={
              data.network_status === NetworkConnectionStatus.WIFI_STATUS_CONNECTED
                ? WifiIcon
                : RouterIcon
            }
            bgcolor={networkStatusHighlight()}
            label={LL.STATUS_OF(LL.NETWORK(1))}
            text={networkStatus()}
            to="/status/network"
          />

          <ListMenuItem
            disabled={!me.admin}
            icon={DeviceHubIcon}
            bgcolor={activeHighlight(data.mqtt_status)}
            label={LL.STATUS_OF('MQTT')}
            text={data.mqtt_status ? LL.ACTIVE() : LL.INACTIVE(0)}
            to="/status/mqtt"
          />

          <ListMenuItem
            disabled={!me.admin}
            icon={AccessTimeIcon}
            bgcolor={ntpStatusHighlight()}
            label={LL.STATUS_OF('NTP')}
            text={ntpStatus()}
            to="/status/ntp"
          />

          <ListMenuItem
            disabled={!me.admin}
            icon={SettingsInputAntennaIcon}
            bgcolor={activeHighlight(data.ap_status)}
            label={LL.STATUS_OF(LL.ACCESS_POINT(0))}
            text={data.ap_status ? LL.ACTIVE() : LL.INACTIVE(0)}
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

        {renderRestartDialog()}
      </>
    );
  };

  return (
    <SectionContent>{restarting ? <RestartMonitor /> : content()}</SectionContent>
  );
};

export default SystemStatus;
