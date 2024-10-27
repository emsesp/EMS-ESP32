import { useContext } from 'react';

import LockIcon from '@mui/icons-material/Lock';
import LockOpenIcon from '@mui/icons-material/LockOpen';
import WifiIcon from '@mui/icons-material/Wifi';
import {
  Avatar,
  Badge,
  List,
  ListItem,
  ListItemAvatar,
  ListItemIcon,
  ListItemText,
  useTheme
} from '@mui/material';
import type { Theme } from '@mui/material';

import { MessageBox } from 'components';
import { useI18nContext } from 'i18n/i18n-react';
import type { WiFiNetwork, WiFiNetworkList } from 'types';
import { WiFiEncryptionType } from 'types';

import { WiFiConnectionContext } from './WiFiConnectionContext';

export const isNetworkOpen = ({ encryption_type }: WiFiNetwork) =>
  encryption_type === WiFiEncryptionType.WIFI_AUTH_OPEN;

export const networkSecurityMode = ({ encryption_type }: WiFiNetwork) => {
  switch (encryption_type) {
    case WiFiEncryptionType.WIFI_AUTH_WEP:
      return 'WEP';
    case WiFiEncryptionType.WIFI_AUTH_WPA_PSK:
      return 'WPA';
    case WiFiEncryptionType.WIFI_AUTH_WPA2_PSK:
      return 'WPA2';
    case WiFiEncryptionType.WIFI_AUTH_WPA_WPA2_PSK:
      return 'WPA/WPA2';
    case WiFiEncryptionType.WIFI_AUTH_WPA2_ENTERPRISE:
      return 'WPA2 Enterprise';
    case WiFiEncryptionType.WIFI_AUTH_OPEN:
      return 'None';
    case WiFiEncryptionType.WIFI_AUTH_WPA3_PSK:
      return 'WPA3';
    case WiFiEncryptionType.WIFI_AUTH_WPA2_WPA3_PSK:
      return 'WPA2/WPA3';
    default:
      return 'Unknown: ' + String(encryption_type);
  }
};

const networkQualityHighlight = ({ rssi }: WiFiNetwork, theme: Theme) => {
  if (rssi <= -85) {
    return theme.palette.error.main;
  } else if (rssi <= -75) {
    return theme.palette.warning.main;
  }
  return theme.palette.success.main;
};

const WiFiNetworkSelector = ({ networkList }: { networkList: WiFiNetworkList }) => {
  const { LL } = useI18nContext();
  const theme = useTheme();

  const wifiConnectionContext = useContext(WiFiConnectionContext);

  const renderNetwork = (network: WiFiNetwork) => (
    <ListItem
      key={network.bssid}
      onClick={() => wifiConnectionContext.selectNetwork(network)}
    >
      <ListItemAvatar>
        <Avatar>{isNetworkOpen(network) ? <LockOpenIcon /> : <LockIcon />}</Avatar>
      </ListItemAvatar>
      <ListItemText
        primary={network.ssid}
        secondary={
          'Security: ' +
          networkSecurityMode(network) +
          ', Ch: ' +
          network.channel +
          ', bssid: ' +
          network.bssid
        }
      />
      <ListItemIcon>
        <Badge badgeContent={network.rssi + 'dBm'}>
          <WifiIcon sx={{ color: networkQualityHighlight(network, theme) }} />
        </Badge>
      </ListItemIcon>
    </ListItem>
  );

  if (networkList.networks.length === 0) {
    return <MessageBox mt={2} mb={1} message={LL.NETWORK_NO_WIFI()} level="info" />;
  }

  return <List>{networkList.networks.map(renderNetwork)}</List>;
};

export default WiFiNetworkSelector;
