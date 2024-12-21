import DeviceHubIcon from '@mui/icons-material/DeviceHub';
import DnsIcon from '@mui/icons-material/Dns';
import GiteIcon from '@mui/icons-material/Gite';
import RouterIcon from '@mui/icons-material/Router';
import SettingsInputAntennaIcon from '@mui/icons-material/SettingsInputAntenna';
import SettingsInputComponentIcon from '@mui/icons-material/SettingsInputComponent';
import WifiIcon from '@mui/icons-material/Wifi';
import {
  Avatar,
  Divider,
  List,
  ListItem,
  ListItemAvatar,
  ListItemText,
  useTheme
} from '@mui/material';
import type { Theme } from '@mui/material';

import * as NetworkApi from 'api/network';

import { useRequest } from 'alova/client';
import { FormLoader, SectionContent, useLayoutTitle } from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import type { NetworkStatusType } from 'types';
import { NetworkConnectionStatus } from 'types';
import { useInterval } from 'utils';

const isConnected = ({ status }: NetworkStatusType) =>
  status === NetworkConnectionStatus.WIFI_STATUS_CONNECTED ||
  status === NetworkConnectionStatus.ETHERNET_STATUS_CONNECTED;

const networkStatusHighlight = ({ status }: NetworkStatusType, theme: Theme) => {
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

const networkQualityHighlight = ({ rssi }: NetworkStatusType, theme: Theme) => {
  if (rssi <= -85) {
    return theme.palette.error.main;
  } else if (rssi <= -75) {
    return theme.palette.warning.main;
  }
  return theme.palette.success.main;
};

export const isWiFi = ({ status }: NetworkStatusType) =>
  status === NetworkConnectionStatus.WIFI_STATUS_CONNECTED;
export const isEthernet = ({ status }: NetworkStatusType) =>
  status === NetworkConnectionStatus.ETHERNET_STATUS_CONNECTED;

const dnsServers = ({ dns_ip_1, dns_ip_2 }: NetworkStatusType) => {
  if (!dns_ip_1) {
    return 'none';
  }
  return dns_ip_1 + (!dns_ip_2 || dns_ip_2 === '0.0.0.0' ? '' : ', ' + dns_ip_2);
};

const IPs = (status: NetworkStatusType) => {
  if (
    !status.local_ipv6 ||
    status.local_ipv6 === '0000:0000:0000:0000:0000:0000:0000:0000' ||
    status.local_ipv6 === '::'
  ) {
    return status.local_ip;
  }
  if (!status.local_ip || status.local_ip === '0.0.0.0') {
    return status.local_ipv6;
  }
  return status.local_ip + ', ' + status.local_ipv6;
};

const NetworkStatus = () => {
  const { data, send: loadData, error } = useRequest(NetworkApi.readNetworkStatus);

  useInterval(() => {
    void loadData();
  });

  const { LL } = useI18nContext();
  useLayoutTitle(LL.NETWORK(1));

  const theme = useTheme();

  const networkStatus = ({ status }: NetworkStatusType) => {
    switch (status) {
      case NetworkConnectionStatus.ETHERNET_STATUS_CONNECTED:
        return LL.CONNECTED(0) + ' (Ethernet)';
      case NetworkConnectionStatus.WIFI_STATUS_NO_SHIELD:
        return LL.INACTIVE(1);
      case NetworkConnectionStatus.WIFI_STATUS_IDLE:
        return LL.IDLE();
      case NetworkConnectionStatus.WIFI_STATUS_NO_SSID_AVAIL:
        return 'No SSID Available';
      case NetworkConnectionStatus.WIFI_STATUS_CONNECTED:
        return LL.CONNECTED(0) + ' (WiFi) (' + data.reconnect_count + ')';
      case NetworkConnectionStatus.WIFI_STATUS_CONNECT_FAILED:
        return (
          LL.CONNECTED(1) + ' ' + LL.FAILED(0) + ' (' + data.reconnect_count + ')'
        );
      case NetworkConnectionStatus.WIFI_STATUS_CONNECTION_LOST:
        return LL.CONNECTED(1) + ' ' + LL.LOST() + ' (' + data.reconnect_count + ')';
      case NetworkConnectionStatus.WIFI_STATUS_DISCONNECTED:
        return LL.DISCONNECTED();
      default:
        return LL.UNKNOWN();
    }
  };

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={error?.message} />;
    }

    return (
      <List>
        <ListItem>
          <ListItemAvatar>
            <Avatar sx={{ bgcolor: networkStatusHighlight(data, theme) }}>
              {isWiFi(data) && <WifiIcon />}
              {isEthernet(data) && <RouterIcon />}
            </Avatar>
          </ListItemAvatar>
          <ListItemText primary="Status" secondary={networkStatus(data)} />
        </ListItem>
        <Divider variant="inset" component="li" />
        <ListItem>
          <ListItemAvatar>
            <Avatar sx={{ bgcolor: networkStatusHighlight(data, theme) }}>
              <GiteIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText primary={LL.HOSTNAME()} secondary={data.hostname} />
        </ListItem>
        <Divider variant="inset" component="li" />
        {isWiFi(data) && (
          <>
            <ListItem>
              <ListItemAvatar>
                <Avatar sx={{ bgcolor: networkQualityHighlight(data, theme) }}>
                  <SettingsInputAntennaIcon />
                </Avatar>
              </ListItemAvatar>
              <ListItemText
                primary="SSID (RSSI)"
                secondary={data.ssid + ' (' + data.rssi + ' dBm)'}
              />
            </ListItem>
            <Divider variant="inset" component="li" />
          </>
        )}
        {isConnected(data) && (
          <>
            <ListItem>
              <ListItemAvatar>
                <Avatar>IP</Avatar>
              </ListItemAvatar>
              <ListItemText primary={LL.ADDRESS_OF('IP')} secondary={IPs(data)} />
            </ListItem>
            <Divider variant="inset" component="li" />
            <ListItem>
              <ListItemAvatar>
                <Avatar>
                  <DeviceHubIcon />
                </Avatar>
              </ListItemAvatar>
              <ListItemText
                primary={LL.ADDRESS_OF('MAC')}
                secondary={data.mac_address}
              />
            </ListItem>
            <Divider variant="inset" component="li" />
            <ListItem>
              <ListItemAvatar>
                <Avatar>#</Avatar>
              </ListItemAvatar>
              <ListItemText
                primary={LL.NETWORK_SUBNET()}
                secondary={data.subnet_mask}
              />
            </ListItem>
            <Divider variant="inset" component="li" />
            <ListItem>
              <ListItemAvatar>
                <Avatar>
                  <SettingsInputComponentIcon />
                </Avatar>
              </ListItemAvatar>
              <ListItemText
                primary={LL.NETWORK_GATEWAY()}
                secondary={data.gateway_ip || 'none'}
              />
            </ListItem>
            <Divider variant="inset" component="li" />
            <ListItem>
              <ListItemAvatar>
                <Avatar>
                  <DnsIcon />
                </Avatar>
              </ListItemAvatar>
              <ListItemText
                primary={LL.NETWORK_DNS()}
                secondary={dnsServers(data)}
              />
            </ListItem>
          </>
        )}
      </List>
    );
  };

  return <SectionContent>{content()}</SectionContent>;
};

export default NetworkStatus;
