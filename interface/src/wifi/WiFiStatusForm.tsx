import React, { Component, Fragment } from 'react';

import { WithTheme, withTheme } from '@material-ui/core/styles';
import { Avatar, Divider, List, ListItem, ListItemAvatar, ListItemText } from '@material-ui/core';

import DNSIcon from '@material-ui/icons/Dns';
import WifiIcon from '@material-ui/icons/Wifi';
import SettingsInputComponentIcon from '@material-ui/icons/SettingsInputComponent';
import SettingsInputAntennaIcon from '@material-ui/icons/SettingsInputAntenna';
import DeviceHubIcon from '@material-ui/icons/DeviceHub';
import RefreshIcon from '@material-ui/icons/Refresh';

import { RestFormProps, FormActions, FormButton, HighlightAvatar } from '../components';
import { wifiStatus, wifiStatusHighlight, isConnected } from './WiFiStatus';
import { WiFiStatus } from './types';

type WiFiStatusFormProps = RestFormProps<WiFiStatus> & WithTheme;

class WiFiStatusForm extends Component<WiFiStatusFormProps> {

  dnsServers(status: WiFiStatus) {
    if (!status.dns_ip_1) {
      return "none";
    }
    return status.dns_ip_1 + (status.dns_ip_2 ? ',' + status.dns_ip_2 : '');
  }

  createListItems() {
    const { data, theme } = this.props
    return (
      <Fragment>
        <ListItem>
          <ListItemAvatar>
            <HighlightAvatar color={wifiStatusHighlight(data, theme)}>
              <WifiIcon />
            </HighlightAvatar>
          </ListItemAvatar>
          <ListItemText primary="Status" secondary={wifiStatus(data)} />
        </ListItem>
        <Divider variant="inset" component="li" />
        {
          isConnected(data) &&
          <Fragment>
            <ListItem>
              <ListItemAvatar>
                <Avatar>
                  <SettingsInputAntennaIcon />
                </Avatar>
              </ListItemAvatar>
              <ListItemText primary="SSID" secondary={data.ssid} />
            </ListItem>
            <Divider variant="inset" component="li" />
            <ListItem>
              <ListItemAvatar>
                <Avatar>IP</Avatar>
              </ListItemAvatar>
              <ListItemText primary="IP Address" secondary={data.local_ip} />
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
              <ListItemText primary="Gateway IP" secondary={data.gateway_ip || "none"} />
            </ListItem>
            <Divider variant="inset" component="li" />
            <ListItem>
              <ListItemAvatar>
                <Avatar>
                  <DNSIcon />
                </Avatar>
              </ListItemAvatar>
              <ListItemText primary="DNS Server IP" secondary={this.dnsServers(data)} />
            </ListItem>
            <Divider variant="inset" component="li" />
          </Fragment>
        }
      </Fragment>
    );
  }

  render() {
    return (
      <Fragment>
        <List>
          {this.createListItems()}
        </List>
        <FormActions>
          <FormButton startIcon={<RefreshIcon />} variant="contained" color="secondary" onClick={this.props.loadData}>
            Refresh
          </FormButton>
        </FormActions>
      </Fragment>
    );
  }

}

export default withTheme(WiFiStatusForm);
