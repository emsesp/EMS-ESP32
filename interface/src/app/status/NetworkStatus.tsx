import DeviceHubIcon from '@mui/icons-material/DeviceHub';
import DnsIcon from '@mui/icons-material/Dns';
import GiteIcon from '@mui/icons-material/Gite';
import RouterIcon from '@mui/icons-material/Router';
import SettingsInputAntennaIcon from '@mui/icons-material/SettingsInputAntenna';
import SettingsInputComponentIcon from '@mui/icons-material/SettingsInputComponent';
import WifiIcon from '@mui/icons-material/Wifi';
import {
  Avatar,
  Box,
  Divider,
  List,
  ListItem,
  ListItemAvatar,
  ListItemText,
  Typography,
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

// Utility functions
const isConnected = ({ status }: NetworkStatusType) =>
  status === NetworkConnectionStatus.WIFI_STATUS_CONNECTED ||
  status === NetworkConnectionStatus.ETHERNET_STATUS_CONNECTED;

export const isWiFi = ({ status }: NetworkStatusType) =>
  status === NetworkConnectionStatus.WIFI_STATUS_CONNECTED;

export const isEthernet = ({ status }: NetworkStatusType) =>
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

// one address per line, with an optional dimmed note such as the IPv6 scope
const AddressList = ({
  addresses
}: {
  addresses: { value: string; note?: string }[];
}) => {
  if (addresses.length === 0) {
    return 'none';
  }
  return addresses.map(({ value, note }) => (
    <Box component="span" sx={{ display: 'block' }} key={value}>
      {value}
      {note && (
        <Typography
          component="span"
          variant="body2"
          sx={{ color: 'grey.600', ml: 1 }} // dimmer than the grey[500] secondary text
        >
          ({note})
        </Typography>
      )}
    </Box>
  ));
};

const isSet = (ip?: string): ip is string => !!ip && ip !== '0.0.0.0' && ip !== '::';

const ipAddresses = ({ local_ip, ipv6 }: NetworkStatusType) => [
  ...(isSet(local_ip) ? [{ value: local_ip }] : []),
  ...(ipv6 ?? []).map(({ address, scope }) => ({ value: address, note: scope }))
];

const gateways = ({ gateway_ip, gateway_ipv6 }: NetworkStatusType) => [
  ...(isSet(gateway_ip) ? [{ value: gateway_ip }] : []),
  ...(gateway_ipv6 ?? []).filter(isSet).map((value) => ({ value }))
];

const dnsServers = ({ dns }: NetworkStatusType) =>
  (dns ?? []).filter(isSet).map((value) => ({ value }));

const getNetworkStatusText = (
  status: NetworkConnectionStatus,
  reconnectCount: number,
  LL: ReturnType<typeof useI18nContext>['LL']
) => {
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
      return LL.CONNECTED(0) + ' (WiFi) (' + reconnectCount + ')';
    case NetworkConnectionStatus.WIFI_STATUS_CONNECT_FAILED:
      return LL.CONNECTED(1) + ' ' + LL.FAILED(0) + ' (' + reconnectCount + ')';
    case NetworkConnectionStatus.WIFI_STATUS_CONNECTION_LOST:
      return LL.CONNECTED(1) + ' ' + LL.LOST() + ' (' + reconnectCount + ')';
    case NetworkConnectionStatus.WIFI_STATUS_DISCONNECTED:
      return LL.DISCONNECTED();
    default:
      return LL.UNKNOWN();
  }
};

const NetworkStatus = () => {
  const { data, send: loadData, error } = useRequest(NetworkApi.readNetworkStatus);

  useInterval(() => {
    void loadData();
  });

  const { LL } = useI18nContext();
  useLayoutTitle(LL.NETWORK(1));

  const theme = useTheme();

  if (!data) {
    return (
      <SectionContent>
        <FormLoader onRetry={loadData} errorMessage={error?.message || ''} />
      </SectionContent>
    );
  }

  const statusText = getNetworkStatusText(data.status, data.reconnect_count, LL);
  const statusColor = networkStatusHighlight(data, theme);
  const qualityColor = networkQualityHighlight(data, theme);

  return (
    <SectionContent>
      <List>
        <ListItem>
          <ListItemAvatar>
            <Avatar sx={{ bgcolor: statusColor }}>
              {isWiFi(data) && <WifiIcon />}
              {isEthernet(data) && <RouterIcon />}
            </Avatar>
          </ListItemAvatar>
          <ListItemText primary="Status" secondary={statusText} />
        </ListItem>
        <Divider variant="inset" component="li" />
        <ListItem>
          <ListItemAvatar>
            <Avatar sx={{ bgcolor: statusColor }}>
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
                <Avatar sx={{ bgcolor: qualityColor }}>
                  <SettingsInputAntennaIcon />
                </Avatar>
              </ListItemAvatar>
              <ListItemText
                primary="SSID (RSSI)"
                secondary={`${data.ssid} (${data.rssi} dBm)`}
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
              <ListItemText
                primary={LL.ADDRESS_OF('IP')}
                secondary={<AddressList addresses={ipAddresses(data)} />}
              />
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
                secondary={<AddressList addresses={gateways(data)} />}
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
                secondary={<AddressList addresses={dnsServers(data)} />}
              />
            </ListItem>
          </>
        )}
      </List>
    </SectionContent>
  );
};

export default NetworkStatus;
