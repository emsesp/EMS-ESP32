import { FC } from 'react';
import { Avatar, Button, Divider, List, ListItem, ListItemAvatar, ListItemText, Theme, useTheme } from '@mui/material';

import SettingsInputComponentIcon from '@mui/icons-material/SettingsInputComponent';
import SettingsInputAntennaIcon from '@mui/icons-material/SettingsInputAntenna';
import DeviceHubIcon from '@mui/icons-material/DeviceHub';
import WifiIcon from '@mui/icons-material/Wifi';
import DnsIcon from '@mui/icons-material/Dns';
import RefreshIcon from '@mui/icons-material/Refresh';
import RouterIcon from '@mui/icons-material/Router';

import { ButtonRow, FormLoader, SectionContent } from '../../components';
import { NetworkConnectionStatus, NetworkStatus } from '../../types';
import * as NetworkApi from '../../api/network';
import { useRest } from '../../utils';

// TODO - Move some of the below to 'types' as required

const isConnected = ({ status }: NetworkStatus) => status === NetworkConnectionStatus.WIFI_STATUS_CONNECTED;

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

// TODO - Replace this with a metadata object
const networkStatus = ({ status }: NetworkStatus) => {
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

export const isWiFi = ({ status }: NetworkStatus) => status === NetworkConnectionStatus.WIFI_STATUS_CONNECTED;
export const isEthernet = ({ status }: NetworkStatus) => status === NetworkConnectionStatus.ETHERNET_STATUS_CONNECTED;

const dnsServers = ({ dns_ip_1, dns_ip_2 }: NetworkStatus) => {
  if (!dns_ip_1) {
    return 'none';
  }
  return dns_ip_1 + (dns_ip_2 ? ',' + dns_ip_2 : '');
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
  const { loadData, data, errorMessage } = useRest<NetworkStatus>({ read: NetworkApi.readNetworkStatus });

  const theme = useTheme();

  const content = () => {
    if (!data) {
      return <FormLoader loadData={loadData} errorMessage={errorMessage} />;
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
        {isWiFi(data) && (
          <>
            <ListItem>
              <ListItemAvatar>
                <Avatar>
                  <SettingsInputAntennaIcon />
                </Avatar>
              </ListItemAvatar>
              <ListItemText primary="SSID" secondary={data.ssid} />
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
              <ListItemText primary="IP Address" secondary={IPs(data)} />
            </ListItem>
            <Divider variant="inset" component="li" />
            <ListItem>
              <ListItemAvatar>
                <Avatar>
                  <DeviceHubIcon />
                </Avatar>
              </ListItemAvatar>
              <ListItemText primary="MAC Address" secondary={data.mac_address} />
            </ListItem>
            <Divider variant="inset" component="li" />
            <ListItem>
              <ListItemAvatar>
                <Avatar>#</Avatar>
              </ListItemAvatar>
              <ListItemText primary="Subnet Mask" secondary={data.subnet_mask} />
            </ListItem>
            <Divider variant="inset" component="li" />
            <ListItem>
              <ListItemAvatar>
                <Avatar>
                  <SettingsInputComponentIcon />
                </Avatar>
              </ListItemAvatar>
              <ListItemText primary="Gateway IP" secondary={data.gateway_ip || 'none'} />
            </ListItem>
            <Divider variant="inset" component="li" />
            <ListItem>
              <ListItemAvatar>
                <Avatar>
                  <DnsIcon />
                </Avatar>
              </ListItemAvatar>
              <ListItemText primary="DNS Server IP" secondary={dnsServers(data)} />
            </ListItem>
            <Divider variant="inset" component="li" />
          </>
        )}
      </List>
    );
  };

  return (
    <SectionContent title="Network Status" titleGutter>
      {content()}
      <ButtonRow>
        <Button startIcon={<RefreshIcon />} variant="outlined" color="secondary" onClick={loadData}>
          Refresh
        </Button>
      </ButtonRow>
    </SectionContent>
  );
};

export default NetworkStatusForm;
