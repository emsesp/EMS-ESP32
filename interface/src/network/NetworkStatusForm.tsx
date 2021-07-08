import { Component, Fragment } from 'react';

import { WithTheme, withTheme } from '@material-ui/core/styles';
import {
  Avatar,
  Divider,
  List,
  ListItem,
  ListItemAvatar,
  ListItemText
} from '@material-ui/core';

import DNSIcon from '@material-ui/icons/Dns';
import WifiIcon from '@material-ui/icons/Wifi';
import RouterIcon from '@material-ui/icons/Router';
import SettingsInputComponentIcon from '@material-ui/icons/SettingsInputComponent';
import SettingsInputAntennaIcon from '@material-ui/icons/SettingsInputAntenna';
import DeviceHubIcon from '@material-ui/icons/DeviceHub';
import RefreshIcon from '@material-ui/icons/Refresh';

import {
  RestFormProps,
  FormActions,
  FormButton,
  HighlightAvatar
} from '../components';
import {
  networkStatus,
  networkStatusHighlight,
  isConnected,
  isWiFi,
  isEthernet
} from './NetworkStatus';
import { NetworkStatus } from './types';

type NetworkStatusFormProps = RestFormProps<NetworkStatus> & WithTheme;

class NetworkStatusForm extends Component<NetworkStatusFormProps> {
  dnsServers(status: NetworkStatus) {
    if (!status.dns_ip_1) {
      return 'none';
    }
    if (!status.dns_ip_2 || status.dns_ip_2 === '0.0.0.0') {
      return status.dns_ip_1;
    }
    return status.dns_ip_1 + ', ' + status.dns_ip_2;
  }
  IPs(status: NetworkStatus) {
    if (
      !status.local_ipv6 ||
      status.local_ipv6 === '0000:0000:0000:0000:0000:0000:0000:0000'
    ) {
      return status.local_ip;
    }
    if (!status.local_ip || status.local_ip === '0.0.0.0') {
      return status.local_ipv6;
    }
    return status.local_ip + ', ' + status.local_ipv6;
  }

  createListItems() {
    const { data, theme } = this.props;

    return (
      <Fragment>
        <ListItem>
          <ListItemAvatar>
            <HighlightAvatar color={networkStatusHighlight(data, theme)}>
              {isWiFi(data) && <WifiIcon />}
              {isEthernet(data) && <RouterIcon />}
            </HighlightAvatar>
          </ListItemAvatar>
          <ListItemText primary="Status" secondary={networkStatus(data)} />
        </ListItem>
        <Divider variant="inset" component="li" />
        {isWiFi(data) && (
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
          </Fragment>
        )}
        {isConnected(data) && (
          <Fragment>
            <ListItem>
              <ListItemAvatar>
                <Avatar>IP</Avatar>
              </ListItemAvatar>
              <ListItemText primary="IP Address" secondary={this.IPs(data)} />
            </ListItem>
            <Divider variant="inset" component="li" />
            <ListItem>
              <ListItemAvatar>
                <Avatar>
                  <DeviceHubIcon />
                </Avatar>
              </ListItemAvatar>
              <ListItemText
                primary="MAC Address"
                secondary={data.mac_address}
              />
            </ListItem>
            <Divider variant="inset" component="li" />
            <ListItem>
              <ListItemAvatar>
                <Avatar>#</Avatar>
              </ListItemAvatar>
              <ListItemText
                primary="Subnet Mask"
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
                primary="Gateway IP"
                secondary={data.gateway_ip || 'none'}
              />
            </ListItem>
            <Divider variant="inset" component="li" />
            <ListItem>
              <ListItemAvatar>
                <Avatar>
                  <DNSIcon />
                </Avatar>
              </ListItemAvatar>
              <ListItemText
                primary="DNS Server IP"
                secondary={this.dnsServers(data)}
              />
            </ListItem>
            <Divider variant="inset" component="li" />
          </Fragment>
        )}
      </Fragment>
    );
  }

  render() {
    return (
      <Fragment>
        <List>{this.createListItems()}</List>
        <FormActions>
          <FormButton
            startIcon={<RefreshIcon />}
            variant="contained"
            color="secondary"
            onClick={this.props.loadData}
          >
            Refresh
          </FormButton>
        </FormActions>
      </Fragment>
    );
  }
}

export default withTheme(NetworkStatusForm);
