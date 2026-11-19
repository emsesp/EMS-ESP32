import type { IconName, StatusState } from 'components/ui';
import type { TranslationFunctions } from 'i18n/i18n-types';
import {
  APNetworkStatus,
  type APStatusType,
  type MqttStatusType,
  NTPSyncStatus,
  NetworkConnectionStatus,
  SystemStatusCodes
} from 'types';

import { busConnectionStatus } from '../main/types';

type LL = TranslationFunctions;

export interface StatusInfo {
  state: StatusState;
  text: string;
}

export const nf = new Intl.NumberFormat();

export const formatDuration = (seconds: number, LL: LL) => {
  const days = Math.trunc(seconds / 86400);
  const hours = Math.trunc(seconds / 3600) % 24;
  const minutes = Math.trunc(seconds / 60) % 60;
  const parts: string[] = [];
  if (days) parts.push(LL.NUM_DAYS({ num: days }));
  if (hours) parts.push(LL.NUM_HOURS({ num: hours }));
  if (minutes) parts.push(LL.NUM_MINUTES({ num: minutes }));
  // seconds only while it is short, they change on every poll
  if (!days && !hours) parts.push(LL.NUM_SECONDS({ num: Math.trunc(seconds % 60) }));
  return parts.join(' ');
};

// values above 90 can only be Fahrenheit
export const formatTemperature = (temp: number) =>
  `${temp} °${temp > 90 ? 'F' : 'C'}`;

export const systemInfo = (status: number, LL: LL): StatusInfo => {
  if (status >= SystemStatusCodes.SYSTEM_STATUS_UPLOADING) {
    return { state: 'warn', text: LL.WAIT_FIRMWARE() };
  }
  switch (status) {
    case SystemStatusCodes.SYSTEM_STATUS_PENDING_UPLOAD:
      return { state: 'warn', text: LL.WAIT_FIRMWARE() };
    case SystemStatusCodes.SYSTEM_STATUS_ERROR_UPLOAD:
    case SystemStatusCodes.SYSTEM_STATUS_ERROR_NOT_OFFICIAL:
      return { state: 'err', text: LL.ERROR() };
    case SystemStatusCodes.SYSTEM_STATUS_PENDING_RESTART:
    case SystemStatusCodes.SYSTEM_STATUS_RESTART_REQUESTED:
      return { state: 'warn', text: LL.RESTARTING_PRE() };
    case SystemStatusCodes.SYSTEM_STATUS_INVALID_GPIO:
      return { state: 'err', text: LL.GPIO_INVALID() };
    default:
      return { state: 'ok', text: LL.STATUS_RUNNING() };
  }
};

export const busInfo = (
  bus_status: busConnectionStatus,
  bus_uptime: number,
  LL: LL
): StatusInfo => {
  switch (bus_status) {
    case busConnectionStatus.BUS_STATUS_CONNECTED:
      return {
        state: 'ok',
        text: `EMS ${LL.CONNECTED(0)} (${formatDuration(bus_uptime, LL)})`
      };
    case busConnectionStatus.BUS_STATUS_TX_ERRORS:
      return { state: 'warn', text: `EMS ${LL.TX_ISSUES()}` };
    case busConnectionStatus.BUS_STATUS_OFFLINE:
      return { state: 'err', text: `EMS ${LL.DISCONNECTED()}` };
    default:
      return { state: 'warn', text: LL.UNKNOWN() };
  }
};

export const isWiFi = (status: NetworkConnectionStatus) =>
  status === NetworkConnectionStatus.WIFI_STATUS_CONNECTED;

export const isEthernet = (status: NetworkConnectionStatus) =>
  status === NetworkConnectionStatus.ETHERNET_STATUS_CONNECTED;

export const networkIcon = (status: NetworkConnectionStatus): IconName =>
  isEthernet(status) ? 'lan' : 'wifi';

export const networkInfo = (status: NetworkConnectionStatus, LL: LL): StatusInfo => {
  switch (status) {
    case NetworkConnectionStatus.ETHERNET_STATUS_CONNECTED:
      return { state: 'ok', text: LL.CONNECTED_VIA('Ethernet') };
    case NetworkConnectionStatus.WIFI_STATUS_CONNECTED:
      return { state: 'ok', text: LL.CONNECTED_VIA('Wi-Fi') };
    case NetworkConnectionStatus.WIFI_STATUS_NO_SHIELD:
      return { state: 'off', text: LL.INACTIVE(1) };
    case NetworkConnectionStatus.WIFI_STATUS_IDLE:
      return { state: 'off', text: LL.IDLE() };
    case NetworkConnectionStatus.WIFI_STATUS_NO_SSID_AVAIL:
      return { state: 'warn', text: LL.NETWORK_NOT_FOUND() };
    case NetworkConnectionStatus.WIFI_STATUS_CONNECT_FAILED:
      return { state: 'err', text: LL.CONNECTION_FAILED() };
    case NetworkConnectionStatus.WIFI_STATUS_CONNECTION_LOST:
      return { state: 'err', text: LL.CONNECTION_LOST() };
    case NetworkConnectionStatus.WIFI_STATUS_DISCONNECTED:
      return { state: 'off', text: LL.DISCONNECTED() };
    default:
      return { state: 'warn', text: LL.UNKNOWN() };
  }
};

export const ntpInfo = (status: NTPSyncStatus, LL: LL): StatusInfo => {
  switch (status) {
    case NTPSyncStatus.NTP_DISABLED:
      return { state: 'off', text: LL.NOT_ENABLED() };
    case NTPSyncStatus.NTP_INACTIVE:
      return { state: 'err', text: LL.INACTIVE(0) };
    case NTPSyncStatus.NTP_ACTIVE:
      return { state: 'ok', text: LL.ACTIVE() };
    default:
      return { state: 'warn', text: LL.UNKNOWN() };
  }
};

export const mqttInfo = (
  { enabled, connected }: Pick<MqttStatusType, 'enabled' | 'connected'>,
  LL: LL
): StatusInfo => {
  if (!enabled) return { state: 'off', text: LL.NOT_ENABLED() };
  return connected
    ? { state: 'ok', text: LL.CONNECTED(0) }
    : { state: 'err', text: LL.DISCONNECTED() };
};

export const apInfo = (status: APStatusType['status'], LL: LL): StatusInfo => {
  switch (status) {
    case APNetworkStatus.ACTIVE:
      return { state: 'ok', text: LL.ACTIVE() };
    case APNetworkStatus.INACTIVE:
      return { state: 'off', text: LL.INACTIVE(0) };
    case APNetworkStatus.LINGERING:
      return { state: 'warn', text: LL.AP_LINGERING() };
    default:
      return { state: 'warn', text: LL.UNKNOWN() };
  }
};

// avatar colours of the status tiles, by state
export const STATE_COLORS: Record<StatusState, string> = {
  ok: '#43a047',
  warn: '#ef6c00',
  err: '#c62828',
  off: '#616161'
};
