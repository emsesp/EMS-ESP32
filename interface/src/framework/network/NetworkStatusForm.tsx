import DeviceHubIcon from '@mui/icons-material/DeviceHub';
import DnsIcon from '@mui/icons-material/Dns';
import RefreshIcon from '@mui/icons-material/Refresh';
import RouterIcon from '@mui/icons-material/Router';
import SettingsInputAntennaIcon from '@mui/icons-material/SettingsInputAntenna';
import SettingsInputComponentIcon from '@mui/icons-material/SettingsInputComponent';
import WifiIcon from '@mui/icons-material/Wifi';
import { Avatar, Button, Divider, List, ListItem, ListItemAvatar, ListItemText, useTheme } from '@mui/material';
import { useRequest } from 'alova';
import type { Theme } from '@mui/material';
import type { FC } from 'react';

import type { NetworkStatus } from 'types';
import * as NetworkApi from 'api/network';
import { ButtonRow, FormLoader, SectionContent } from 'components';

import { useI18nContext } from 'i18n/i18n-react';
import { NetworkConnectionStatus } from 'types';

const isConnected = ({ status }: NetworkStatus) =>
  status === NetworkConnectionStatus.WIFI_STATUS_CONNECTED ||
  status === NetworkConnectionStatus.ETHERNET_STATUS_CONNECTED;

const networkStatusHighlight = ({ status }: NetworkStatus, theme: Theme) => {
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

const networkQualityHighlight = ({ rssi }: NetworkStatus, theme: Theme) => {
  if (rssi <= -85) {
    return theme.palette.error.main;
  } else if (rssi <= -75) {
    return theme.palette.warning.main;
  }
  return theme.palette.success.main;
};

export const isWiFi = ({ status }: NetworkStatus) => status === NetworkConnectionStatus.WIFI_STATUS_CONNECTED;
export const isEthernet = ({ status }: NetworkStatus) => status === NetworkConnectionStatus.ETHERNET_STATUS_CONNECTED;

const dnsServers = ({ dns_ip_1, dns_ip_2 }: NetworkStatus) => {
  if (!dns_ip_1) {
    return 'none';
  }
  return dns_ip_1 + (!dns_ip_2 || dns_ip_2 === '0.0.0.0' ? '' : ',' + dns_ip_2);
};

const IPs = (status: NetworkStatus) => {
  if (!status.local_ipv6 || status.local_ipv6 === '0000:0000:0000:0000:0000:0000:0000:0000') {
    return status.local_ip;
  }
  if (!status.local_ip || status.local_ip === '0.0.0.0') {
    return status.local_ipv6;
  }
  return status.local_ip + ', ' + status.local_ipv6;
};

const NetworkStatusForm: FC = () => {
  const { data: data, send: loadData, error } = useRequest(NetworkApi.readNetworkStatus);

  const { LL } = useI18nContext();

  const theme = useTheme();

  const networkStatus = ({ status }: NetworkStatus) => {
    switch (status) {
      case NetworkConnectionStatus.WIFI_STATUS_NO_SHIELD:
        return LL.INACTIVE(1);
      case NetworkConnectionStatus.WIFI_STATUS_IDLE:
        return LL.IDLE();
      case NetworkConnectionStatus.WIFI_STATUS_NO_SSID_AVAIL:
        return 'No SSID Available';
      case NetworkConnectionStatus.WIFI_STATUS_CONNECTED:
        return LL.CONNECTED(0) + ' (WiFi)';
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

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={error?.message} />;
    }

    return (
      <>
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
          {isWiFi(data) && (
            <>
              <ListItem>
                <ListItemAvatar>
                  <Avatar sx={{ bgcolor: networkQualityHighlight(data, theme) }}>
                    <SettingsInputAntennaIcon />
                  </Avatar>
                </ListItemAvatar>
                <ListItemText primary="SSID (RSSI)" secondary={data.ssid + ' (' + data.rssi + ' dBm)'} />
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
                <ListItemText primary={LL.ADDRESS_OF('MAC')} secondary={data.mac_address} />
              </ListItem>
              <Divider variant="inset" component="li" />
              <ListItem>
                <ListItemAvatar>
                  <Avatar>#</Avatar>
                </ListItemAvatar>
                <ListItemText primary={LL.NETWORK_SUBNET()} secondary={data.subnet_mask} />
              </ListItem>
              <Divider variant="inset" component="li" />
              <ListItem>
                <ListItemAvatar>
                  <Avatar>
                    <SettingsInputComponentIcon />
                  </Avatar>
                </ListItemAvatar>
                <ListItemText primary={LL.NETWORK_GATEWAY()} secondary={data.gateway_ip || 'none'} />
              </ListItem>
              <Divider variant="inset" component="li" />
              <ListItem>
                <ListItemAvatar>
                  <Avatar>
                    <DnsIcon />
                  </Avatar>
                </ListItemAvatar>
                <ListItemText primary={LL.NETWORK_DNS()} secondary={dnsServers(data)} />
              </ListItem>
              <Divider variant="inset" component="li" />
            </>
          )}
        </List>
        <ButtonRow>
          <Button startIcon={<RefreshIcon />} variant="outlined" color="secondary" onClick={loadData}>
            {LL.REFRESH()}
          </Button>
        </ButtonRow>
      </>
    );
  };

  return (
    <SectionContent title={LL.STATUS_OF(LL.NETWORK(1))} titleGutter>
      {content()}
    </SectionContent>
  );
};

export default NetworkStatusForm;
