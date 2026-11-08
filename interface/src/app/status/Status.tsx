import AccessTimeIcon from '@mui/icons-material/AccessTime';
import DeviceHubIcon from '@mui/icons-material/DeviceHub';
import DirectionsBusIcon from '@mui/icons-material/DirectionsBus';
import LogoDevIcon from '@mui/icons-material/LogoDev';
import MemoryIcon from '@mui/icons-material/Memory';
import MonitorHeartIcon from '@mui/icons-material/MonitorHeart';
import RouterIcon from '@mui/icons-material/Router';
import SettingsInputAntennaIcon from '@mui/icons-material/SettingsInputAntenna';
import WifiIcon from '@mui/icons-material/Wifi';
import {
  Avatar,
  List,
  ListItem,
  ListItemAvatar,
  ListItemText,
  useTheme
} from '@mui/material';

import { readSystemStatus } from 'api/system';

import { useRequest } from 'alova/client';
import { busConnectionStatus } from 'app/main/types';
import { FormLoader, SectionContent, useLayoutTitle } from 'components';
import ListMenuItem from 'components/layout/ListMenuItem';
import { useI18nContext } from 'i18n/i18n-react';
import { NTPSyncStatus, NetworkConnectionStatus, SystemStatusCodes } from 'types';
import { useInterval } from 'utils';
import { formatDateTime } from 'utils/time';

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

  const { data, send: loadData, error } = useRequest(readSystemStatus);

  useInterval(() => {
    void loadData();
  });

  const theme = useTheme();

  const busStatus = (() => {
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
  })();

  const systemStatus = (() => {
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
        return 'OK';
    }
  })();

  const busStatusHighlight = (() => {
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
  })();

  const ntpStatus = (() => {
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
  })();

  const ntpStatusHighlight = (() => {
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
  })();

  const networkStatusHighlight = (() => {
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
  })();

  const networkStatus = (() => {
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
  })();

  const activeHighlight = (value: boolean) =>
    value ? theme.palette.success.main : theme.palette.info.main;

  if (!data || !LL) {
    return (
      <SectionContent>
        <FormLoader onRetry={loadData} errorMessage={error?.message || ''} />
      </SectionContent>
    );
  }

  return (
    <SectionContent>
      <List>
        <ListItem>
          <ListItemAvatar>
            <Avatar sx={{ bgcolor: '#c5572c', color: 'white' }}>
              <MonitorHeartIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText
            primary={LL.STATUS_OF(LL.SYSTEM(0))}
            secondary={`${systemStatus} (${LL.UPTIME()}: ${formatDurationSec(data.uptime, LL)})`}
          />
        </ListItem>

        <ListMenuItem
          icon={MemoryIcon}
          bgcolor="#68374d"
          label={LL.HARDWARE()}
          text={`${formatNumber(data.free_heap)} KB ${LL.FREE_MEMORY()}`}
          to="/status/hardwarestatus"
        />

        <ListMenuItem
          icon={DirectionsBusIcon}
          bgcolor={busStatusHighlight}
          label={LL.DATA_TRAFFIC()}
          text={busStatus}
          to="/status/activity"
        />

        <ListMenuItem
          icon={
            data.network_status === NetworkConnectionStatus.WIFI_STATUS_CONNECTED
              ? WifiIcon
              : RouterIcon
          }
          bgcolor={networkStatusHighlight}
          label={LL.NETWORK(1)}
          text={networkStatus}
          to="/status/network"
        />

        <ListMenuItem
          icon={DeviceHubIcon}
          bgcolor={activeHighlight(data.mqtt_status)}
          label="MQTT"
          text={data.mqtt_status ? LL.CONNECTED(0) : LL.INACTIVE(0)}
          to="/status/mqtt"
        />

        <ListMenuItem
          icon={AccessTimeIcon}
          bgcolor={ntpStatusHighlight}
          label="NTP"
          text={ntpStatus}
          to="/status/ntp"
        />

        <ListMenuItem
          icon={SettingsInputAntennaIcon}
          bgcolor={activeHighlight(data.ap_status)}
          label={LL.ACCESS_POINT(0)}
          text={data.ap_status ? LL.ACTIVE() : LL.INACTIVE(0)}
          to="/status/ap"
        />

        <ListMenuItem
          icon={LogoDevIcon}
          bgcolor="#40828f"
          label={LL.LOG_OF(LL.SYSTEM(0))}
          text={LL.VIEW_LOG()}
          to="/status/log"
        />
      </List>
    </SectionContent>
  );
};

export default SystemStatus;
