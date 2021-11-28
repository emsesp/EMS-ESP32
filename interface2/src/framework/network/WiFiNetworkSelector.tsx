import { FC, useContext } from 'react';

import { Avatar, Badge, List, ListItem, ListItemAvatar, ListItemIcon, ListItemText } from '@mui/material';

import LockOpenIcon from '@mui/icons-material/LockOpen';
import LockIcon from '@mui/icons-material/Lock';
import WifiIcon from '@mui/icons-material/Wifi';

import { WiFiEncryptionType, WiFiNetwork, WiFiNetworkList } from '../../types';

import { WiFiConnectionContext } from './WiFiConnectionContext';

interface WiFiNetworkSelectorProps {
  networkList: WiFiNetworkList;
}

export const isNetworkOpen = ({ encryption_type }: WiFiNetwork) => encryption_type === WiFiEncryptionType.WIFI_AUTH_OPEN;

export const networkSecurityMode = ({ encryption_type }: WiFiNetwork) => {
  switch (encryption_type) {
    case WiFiEncryptionType.WIFI_AUTH_WEP:
    case WiFiEncryptionType.WIFI_AUTH_WEP_PSK:
      return "WEP";
    case WiFiEncryptionType.WIFI_AUTH_WEP2_PSK:
      return "WEP2";
    case WiFiEncryptionType.WIFI_AUTH_WPA_WPA2_PSK:
      return "WPA/WEP2";
    case WiFiEncryptionType.WIFI_AUTH_WPA2_ENTERPRISE:
      return "WEP2 Enterprise";
    case WiFiEncryptionType.WIFI_AUTH_OPEN:
      return "None";
    default:
      return "Unknown";
  }
};

const WiFiNetworkSelector: FC<WiFiNetworkSelectorProps> = ({ networkList }) => {
  const wifiConnectionContext = useContext(WiFiConnectionContext);

  const renderNetwork = (network: WiFiNetwork) => {
    return (
      <ListItem key={network.bssid} button onClick={() => wifiConnectionContext.selectNetwork(network)}>
        <ListItemAvatar>
          <Avatar>
            {isNetworkOpen(network) ? <LockOpenIcon /> : <LockIcon />}
          </Avatar>
        </ListItemAvatar>
        <ListItemText
          primary={network.ssid}
          secondary={"Security: " + networkSecurityMode(network) + ", Ch: " + network.channel}
        />
        <ListItemIcon>
          <Badge badgeContent={network.rssi + "db"}>
            <WifiIcon />
          </Badge>
        </ListItemIcon>
      </ListItem>
    );
  };

  return (
    <List>
      {networkList.networks.map(renderNetwork)}
    </List>
  );

};

export default WiFiNetworkSelector;
