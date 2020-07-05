import React, { Component, Fragment } from 'react';

import { Avatar, Button, Divider, Dialog, DialogTitle, DialogContent, DialogActions, Box } from '@material-ui/core';
import { List, ListItem, ListItemAvatar, ListItemText } from '@material-ui/core';

import DevicesIcon from '@material-ui/icons/Devices';
import MemoryIcon from '@material-ui/icons/Memory';
import ShowChartIcon from '@material-ui/icons/ShowChart';
import SdStorageIcon from '@material-ui/icons/SdStorage';
import FolderIcon from '@material-ui/icons/Folder';
import DataUsageIcon from '@material-ui/icons/DataUsage';
import AppsIcon from '@material-ui/icons/Apps';
import PowerSettingsNewIcon from '@material-ui/icons/PowerSettingsNew';
import RefreshIcon from '@material-ui/icons/Refresh';
import SettingsBackupRestoreIcon from '@material-ui/icons/SettingsBackupRestore';

import { redirectingAuthorizedFetch, AuthenticatedContextProps, withAuthenticatedContext } from '../authentication';
import { RestFormProps, FormButton, ErrorButton } from '../components';
import { FACTORY_RESET_ENDPOINT, RESTART_ENDPOINT } from '../api';

import { SystemStatus, EspPlatform } from './types';

interface SystemStatusFormState {
  confirmRestart: boolean;
  confirmFactoryReset: boolean;
  processing: boolean;
}

type SystemStatusFormProps = AuthenticatedContextProps & RestFormProps<SystemStatus>;

function formatNumber(num: number) {
  return new Intl.NumberFormat().format(num);
}

class SystemStatusForm extends Component<SystemStatusFormProps, SystemStatusFormState> {

  state: SystemStatusFormState = {
    confirmRestart: false,
    confirmFactoryReset: false,
    processing: false
  }

  createListItems() {
    const { data } = this.props
    return (
      <Fragment>
        <ListItem >
          <ListItemAvatar>
            <Avatar>
              <DevicesIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText primary="Device (Platform / SDK)" secondary={data.esp_platform + ' / ' + data.sdk_version} />
        </ListItem>
        <Divider variant="inset" component="li" />
        <ListItem >
          <ListItemAvatar>
            <Avatar>
              <ShowChartIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText primary="CPU Frequency" secondary={data.cpu_freq_mhz + ' MHz'} />
        </ListItem>
        <Divider variant="inset" component="li" />
        <ListItem >
          <ListItemAvatar>
            <Avatar>
              <MemoryIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText primary="Heap (Free / Max Alloc)" secondary={formatNumber(data.free_heap) + ' / ' + formatNumber(data.max_alloc_heap) + ' bytes ' + (data.esp_platform === EspPlatform.ESP8266 ? '(' + data.heap_fragmentation + '% fragmentation)' : '')} />
        </ListItem>
        {
          (data.esp_platform === EspPlatform.ESP32 && data.psram_size > 0) && (
            <Fragment>
              <Divider variant="inset" component="li" />
              <ListItem >
                <ListItemAvatar>
                  <Avatar>
                    <AppsIcon />
                  </Avatar>
                </ListItemAvatar>
                <ListItemText primary="PSRAM (Size / Free)" secondary={formatNumber(data.psram_size) + ' / ' + formatNumber(data.free_psram) + ' bytes'} />
              </ListItem>
            </Fragment>)
        }
        <Divider variant="inset" component="li" />
        <ListItem >
          <ListItemAvatar>
            <Avatar>
              <DataUsageIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText primary="Sketch (Size / Free)" secondary={formatNumber(data.sketch_size) + ' / ' + formatNumber(data.free_sketch_space) + ' bytes'} />
        </ListItem>
        <Divider variant="inset" component="li" />
        <ListItem >
          <ListItemAvatar>
            <Avatar>
              <SdStorageIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText primary="Flash Chip (Size / Speed)" secondary={formatNumber(data.flash_chip_size) + ' bytes / ' + (data.flash_chip_speed / 1000000).toFixed(0) + ' MHz'} />
        </ListItem>
        <Divider variant="inset" component="li" />
        <ListItem >
          <ListItemAvatar>
            <Avatar>
              <FolderIcon />
            </Avatar>
          </ListItemAvatar>
          <ListItemText primary="File System (Used / Total)" secondary={formatNumber(data.fs_used) + ' / ' + formatNumber(data.fs_total) + ' bytes (' + formatNumber(data.fs_total - data.fs_used) + '\xa0bytes free)'} />
        </ListItem>
        <Divider variant="inset" component="li" />
      </Fragment>
    );
  }

  renderRestartDialog() {
    return (
      <Dialog
        open={this.state.confirmRestart}
        onClose={this.onRestartRejected}
      >
        <DialogTitle>Confirm Restart</DialogTitle>
        <DialogContent dividers>
          Are you sure you want to restart the device?
        </DialogContent>
        <DialogActions>
          <Button variant="contained" onClick={this.onRestartRejected} color="secondary">
            Cancel
          </Button>
          <Button startIcon={<PowerSettingsNewIcon />} variant="contained" onClick={this.onRestartConfirmed} disabled={this.state.processing} color="primary" autoFocus>
            Restart
          </Button>
        </DialogActions>
      </Dialog>
    )
  }

  onRestart = () => {
    this.setState({ confirmRestart: true });
  }

  onRestartRejected = () => {
    this.setState({ confirmRestart: false });
  }

  onRestartConfirmed = () => {
    this.setState({ processing: true });
    redirectingAuthorizedFetch(RESTART_ENDPOINT, { method: 'POST' })
      .then(response => {
        if (response.status === 200) {
          this.props.enqueueSnackbar("Device is restarting", { variant: 'info' });
          this.setState({ processing: false, confirmRestart: false });
        } else {
          throw Error("Invalid status code: " + response.status);
        }
      })
      .catch(error => {
        this.props.enqueueSnackbar(error.message || "Problem restarting device", { variant: 'error' });
        this.setState({ processing: false, confirmRestart: false });
      });
  }

  renderFactoryResetDialog() {
    return (
      <Dialog
        open={this.state.confirmFactoryReset}
        onClose={this.onFactoryResetRejected}
      >
        <DialogTitle>Confirm Factory Reset</DialogTitle>
        <DialogContent dividers>
          Are you sure you want to reset the device to its factory defaults?
        </DialogContent>
        <DialogActions>
          <Button variant="contained" onClick={this.onFactoryResetRejected} color="secondary">
            Cancel
          </Button>
          <ErrorButton startIcon={<SettingsBackupRestoreIcon />} variant="contained" onClick={this.onFactoryResetConfirmed} disabled={this.state.processing} autoFocus>
            Factory Reset
          </ErrorButton>
        </DialogActions>
      </Dialog>
    )
  }

  onFactoryReset = () => {
    this.setState({ confirmFactoryReset: true });
  }

  onFactoryResetRejected = () => {
    this.setState({ confirmFactoryReset: false });
  }

  onFactoryResetConfirmed = () => {
    this.setState({ processing: true });
    redirectingAuthorizedFetch(FACTORY_RESET_ENDPOINT, { method: 'POST' })
      .then(response => {
        if (response.status === 200) {
          this.props.enqueueSnackbar("Factory reset in progress.", { variant: 'error' });
          this.setState({ processing: false, confirmFactoryReset: false });
        } else {
          throw Error("Invalid status code: " + response.status);
        }
      })
      .catch(error => {
        this.props.enqueueSnackbar(error.message || "Problem factory resetting device", { variant: 'error' });
        this.setState({ processing: false, confirmRestart: false });
      });
  }

  render() {
    const me = this.props.authenticatedContext.me;
    return (
      <Fragment>
        <List>
          {this.createListItems()}
        </List>
        <Box display="flex" flexWrap="wrap">
          <Box flexGrow={1} padding={1}>
            <FormButton startIcon={<RefreshIcon />} variant="contained" color="secondary" onClick={this.props.loadData}>
              Refresh
            </FormButton>
          </Box>
          {me.admin &&
            <Box flexWrap="none" padding={1} whiteSpace="nowrap">
              <FormButton startIcon={<PowerSettingsNewIcon />} variant="contained" color="primary" onClick={this.onRestart}>
                Restart
              </FormButton>
              <ErrorButton startIcon={<SettingsBackupRestoreIcon />} variant="contained" onClick={this.onFactoryReset}>
                Factory reset
              </ErrorButton>
            </Box>
          }
        </Box>
        {this.renderRestartDialog()}
        {this.renderFactoryResetDialog()}
      </Fragment>
    );
  }

}

export default withAuthenticatedContext(SystemStatusForm);
