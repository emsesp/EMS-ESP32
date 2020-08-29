import React, { Component, Fragment } from 'react';
import moment from 'moment';

import { WithTheme, withTheme } from '@material-ui/core/styles';
import { Avatar, Divider, List, ListItem, ListItemAvatar, ListItemText, Button } from '@material-ui/core';
import { Dialog, DialogTitle, DialogContent, DialogActions, Box, TextField } from '@material-ui/core';

import SwapVerticalCircleIcon from '@material-ui/icons/SwapVerticalCircle';
import AccessTimeIcon from '@material-ui/icons/AccessTime';
import DNSIcon from '@material-ui/icons/Dns';
import UpdateIcon from '@material-ui/icons/Update';
import AvTimerIcon from '@material-ui/icons/AvTimer';
import RefreshIcon from '@material-ui/icons/Refresh';

import { RestFormProps, FormButton, HighlightAvatar } from '../components';
import { isNtpActive, ntpStatusHighlight, ntpStatus } from './NTPStatus';
import { formatIsoDateTime, formatLocalDateTime } from './TimeFormat';
import { NTPStatus, Time } from './types';
import { redirectingAuthorizedFetch, withAuthenticatedContext, AuthenticatedContextProps } from '../authentication';
import { TIME_ENDPOINT } from '../api';

type NTPStatusFormProps = RestFormProps<NTPStatus> & WithTheme & AuthenticatedContextProps;

interface NTPStatusFormState {
  settingTime: boolean;
  localTime: string;
  processing: boolean;
}

class NTPStatusForm extends Component<NTPStatusFormProps, NTPStatusFormState> {

  constructor(props: NTPStatusFormProps) {
    super(props);
    this.state = {
      settingTime: false,
      localTime: '',
      processing: false
    };
  }

  updateLocalTime = (event: React.ChangeEvent<HTMLInputElement>) => {
    this.setState({ localTime: event.target.value });
  }

  openSetTime = () => {
    this.setState({ localTime: formatLocalDateTime(moment()), settingTime: true, });
  }

  closeSetTime = () => {
    this.setState({ settingTime: false });
  }

  createAdjustedTime = (): Time => {
    const currentLocalTime = moment(this.props.data.time_local);
    const newLocalTime = moment(this.state.localTime);
    newLocalTime.subtract(currentLocalTime.utcOffset())
    newLocalTime.milliseconds(0);
    newLocalTime.utc();
    return {
      time_utc: newLocalTime.format()
    }
  }

  configureTime = () => {
    this.setState({ processing: true });
    redirectingAuthorizedFetch(TIME_ENDPOINT,
      {
        method: 'POST',
        body: JSON.stringify(this.createAdjustedTime()),
        headers: {
          'Content-Type': 'application/json'
        }
      })
      .then(response => {
        if (response.status === 200) {
          this.props.enqueueSnackbar("Time set successfully", { variant: 'success' });
          this.setState({ processing: false, settingTime: false }, this.props.loadData);
        } else {
          throw Error("Error setting time, status code: " + response.status);
        }
      })
      .catch(error => {
        this.props.enqueueSnackbar(error.message || "Problem setting the time", { variant: 'error' });
        this.setState({ processing: false, settingTime: false });
      });
  }

  renderSetTimeDialog() {
    return (
      <Dialog
        open={this.state.settingTime}
        onClose={this.closeSetTime}
      >
        <DialogTitle>Set Time</DialogTitle>
        <DialogContent dividers>
          <Box mb={2}>Enter local date and time below to set the device's time.</Box>
          <TextField
            label="Local Time"
            type="datetime-local"
            value={this.state.localTime}
            onChange={this.updateLocalTime}
            disabled={this.state.processing}
            variant="outlined"
            fullWidth
            InputLabelProps={{
              shrink: true,
            }}
          />
        </DialogContent>
        <DialogActions>
          <Button variant="contained" onClick={this.closeSetTime} color="secondary">
            Cancel
          </Button>
          <Button startIcon={<AccessTimeIcon />} variant="contained" onClick={this.configureTime} disabled={this.state.processing} color="primary" autoFocus>
            Set Time
          </Button>
        </DialogActions>
      </Dialog>
    )
  }

  render() {
    const { data, theme } = this.props
    const me = this.props.authenticatedContext.me;
    return (
      <Fragment>
        <List>
          <ListItem>
            <ListItemAvatar>
              <HighlightAvatar color={ntpStatusHighlight(data, theme)}>
                <UpdateIcon />
              </HighlightAvatar>
            </ListItemAvatar>
            <ListItemText primary="Status" secondary={ntpStatus(data)} />
          </ListItem>
          <Divider variant="inset" component="li" />
          {isNtpActive(data) && (
            <Fragment>
              <ListItem>
                <ListItemAvatar>
                  <Avatar>
                    <DNSIcon />
                  </Avatar>
                </ListItemAvatar>
                <ListItemText primary="NTP Server" secondary={data.server} />
              </ListItem>
              <Divider variant="inset" component="li" />
            </Fragment>
          )}
          <ListItem>
            <ListItemAvatar>
              <Avatar>
                <AccessTimeIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary="Local Time" secondary={formatIsoDateTime(data.time_local)} />
          </ListItem>
          <Divider variant="inset" component="li" />
          <ListItem>
            <ListItemAvatar>
              <Avatar>
                <SwapVerticalCircleIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary="UTC Time" secondary={formatIsoDateTime(data.time_utc)} />
          </ListItem>
          <Divider variant="inset" component="li" />
          <ListItem>
            <ListItemAvatar>
              <Avatar>
                <AvTimerIcon />
              </Avatar>
            </ListItemAvatar>
            <ListItemText primary="Uptime" secondary={moment.duration(data.uptime, 'seconds').humanize()} />
          </ListItem>
          <Divider variant="inset" component="li" />
        </List>
        <Box display="flex" flexWrap="wrap">
          <Box flexGrow={1} padding={1}>
            <FormButton startIcon={<RefreshIcon />} variant="contained" color="secondary" onClick={this.props.loadData}>
              Refresh
            </FormButton>
          </Box>
          {me.admin && !isNtpActive(data) && (
            <Box flexWrap="none" padding={1} whiteSpace="nowrap">
              <Button onClick={this.openSetTime} variant="contained" color="primary" startIcon={<AccessTimeIcon />}>
                Set Time
              </Button>
            </Box>
          )}
        </Box>
        {this.renderSetTimeDialog()}
      </Fragment>
    );
  }
}

export default withAuthenticatedContext(withTheme(NTPStatusForm));
