import React, { Component } from 'react';

import { Avatar, Badge } from '@material-ui/core';
import { List, ListItem, ListItemIcon, ListItemText, ListItemAvatar } from '@material-ui/core';

import WifiIcon from '@material-ui/icons/Wifi';
import LockIcon from '@material-ui/icons/Lock';
import LockOpenIcon from '@material-ui/icons/LockOpen';

import { isNetworkOpen, networkSecurityMode } from './WiFiSecurityModes';
import { WiFiConnectionContext } from './WiFiConnectionContext';
import { WiFiNetwork, WiFiNetworkList } from './types';

interface WiFiNetworkSelectorProps {
  networkList: WiFiNetworkList;
}

class WiFiNetworkSelector extends Component<WiFiNetworkSelectorProps> {

  static contextType = WiFiConnectionContext;
  context!: React.ContextType<typeof WiFiConnectionContext>;

  renderNetwork = (network: WiFiNetwork) => {
    return (
      <ListItem key={network.bssid} button onClick={() => this.context.selectNetwork(network)}>
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
  }

  render() {
    return (
      <List>
        {this.props.networkList.networks.map(this.renderNetwork)}
      </List>
    );
  }

}

export default WiFiNetworkSelector;
