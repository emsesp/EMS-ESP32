import React, { Component, Fragment } from 'react';

import { WithTheme, withTheme } from '@material-ui/core/styles';
import { Avatar, Divider, List, ListItem, ListItemAvatar, ListItemText } from '@material-ui/core';

import SettingsInputAntennaIcon from '@material-ui/icons/SettingsInputAntenna';
import DeviceHubIcon from '@material-ui/icons/DeviceHub';
import ComputerIcon from '@material-ui/icons/Computer';
import RefreshIcon from '@material-ui/icons/Refresh';

import { RestFormProps, FormActions, FormButton, HighlightAvatar } from '../components';
import { apStatusHighlight, apStatus } from './APStatus';
import { APStatus } from './types';

type APStatusFormProps = RestFormProps<APStatus> & WithTheme;

class APStatusForm extends Component<APStatusFormProps> {

  createListItems() {
    const { data, theme } = this.props
    return (
      <Fragment>
        <ListItem>
          <ListItemAvatar>
            <HighlightAvatar color={apStatusHighlight(data, theme)}>
              <SettingsInputAntennaIcon />
            </HighlightAvatar>
          </ListItemAvatar>
          <ListItemText primary="Status" secondary={apStatus(data)} />
        </ListItem>
        <Divider variant="inset" component="li" />
        <ListItem>
          <ListItemAvatar>
            <Avatar>IP</Avatar>
          </ListItemAvatar>
          <ListItemText primary="IP Address" secondary={data.ip_address} />
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
            <Avatar>
              <ComputerIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText primary="AP Clients" secondary={data.station_num} />
        </ListItem>
        <Divider variant="inset" component="li" />
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

export default withTheme(APStatusForm);
