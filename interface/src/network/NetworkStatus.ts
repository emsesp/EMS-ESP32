import { Theme } from '@material-ui/core';
import { NetworkStatus, NetworkConnectionStatus } from './types';

export const isConnected = ({ status }: NetworkStatus) => {
  return (
    status === NetworkConnectionStatus.WIFI_STATUS_CONNECTED ||
    status === NetworkConnectionStatus.ETHERNET_STATUS_CONNECTED
  );
};

export const isWiFi = ({ status }: NetworkStatus) =>
  status === NetworkConnectionStatus.WIFI_STATUS_CONNECTED;
export const isEthernet = ({ status }: NetworkStatus) =>
  status === NetworkConnectionStatus.ETHERNET_STATUS_CONNECTED;

export const networkStatusHighlight = (
  { status }: NetworkStatus,
  theme: Theme
) => {
  switch (status) {
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

export const networkStatus = ({ status }: NetworkStatus) => {
  switch (status) {
    case NetworkConnectionStatus.WIFI_STATUS_NO_SHIELD:
      return 'Inactive';
    case NetworkConnectionStatus.WIFI_STATUS_IDLE:
      return 'Idle';
    case NetworkConnectionStatus.WIFI_STATUS_NO_SSID_AVAIL:
      return 'No SSID Available';
    case NetworkConnectionStatus.WIFI_STATUS_CONNECTED:
      return 'Connected (WiFi)';
    case NetworkConnectionStatus.ETHERNET_STATUS_CONNECTED:
      return 'Connected (Ethernet)';
    case NetworkConnectionStatus.WIFI_STATUS_CONNECT_FAILED:
      return 'Connection Failed';
    case NetworkConnectionStatus.WIFI_STATUS_CONNECTION_LOST:
      return 'Connection Lost';
    case NetworkConnectionStatus.WIFI_STATUS_DISCONNECTED:
      return 'Disconnected';
    default:
      return 'Unknown';
  }
};
