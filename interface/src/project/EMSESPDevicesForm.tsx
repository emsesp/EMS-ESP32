import React, { Component, Fragment } from "react";

import {
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableRow,
  List,
  withWidth,
  WithWidthProps,
  isWidthDown,
  Button,
  DialogTitle, DialogContent, DialogActions, Box, Dialog, Typography
} from "@material-ui/core";

import RefreshIcon from "@material-ui/icons/Refresh";

import { redirectingAuthorizedFetch, withAuthenticatedContext, AuthenticatedContextProps } from '../authentication';

import {
  RestFormProps,
  FormButton,
} from "../components";

import { EMSESPDevices, Device } from "./types";

import { ENDPOINT_ROOT } from '../api';
export const SCANDEVICES_ENDPOINT = ENDPOINT_ROOT + "scanDevices";

function compareDevices(a: Device, b: Device) {
  if (a.type < b.type) {
    return -1;
  }
  if (a.type > b.type) {
    return 1;
  }
  return 0;
}

interface EMSESPDevicesFormState {
  confirmScanDevices: boolean;
  processing: boolean;
}

type EMSESPDevicesFormProps = RestFormProps<EMSESPDevices> & AuthenticatedContextProps & WithWidthProps;

class EMSESPDevicesForm extends Component<EMSESPDevicesFormProps, EMSESPDevicesFormState> {

  state: EMSESPDevicesFormState = {
    confirmScanDevices: false,
    processing: false
  }

  noData = () => {
    return (this.props.data.devices.length === 0);
  };

  createListItems() {
    const { width, data } = this.props;
    return (
      <Fragment>
        <Table size="small" padding={isWidthDown('xs', width!) ? "none" : "default"}>
          <TableHead>
            <TableRow>
              <TableCell>Type</TableCell>
              <TableCell align="center">Brand</TableCell>
              <TableCell align="center">Name</TableCell>
              <TableCell align="center">Device ID</TableCell>
              <TableCell align="center">Product ID</TableCell>
              <TableCell align="center">Version</TableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            {data.devices.sort(compareDevices).map(device => (
              <TableRow key={device.type}>
                <TableCell component="th" scope="row">
                  {device.type}
                </TableCell>
                <TableCell align="center">
                  {device.brand}
                </TableCell>
                <TableCell align="center">
                  {device.name}
                </TableCell>
                <TableCell align="center">
                  0x{('00' + device.deviceid.toString(16).toUpperCase()).slice(-2)}
                </TableCell>
                <TableCell align="center">
                  {device.productid}
                </TableCell>
                <TableCell align="center">
                  {device.version}
                </TableCell>
              </TableRow>
            ))}
          </TableBody>
        </Table>
        {this.noData() &&
          (
            <Box bgcolor="error.main" color="error.contrastText" p={2} mt={2} mb={2}>
              <Typography variant="body1">
                No EMS devices found.
              </Typography>
            </Box>
          )
        }
      </Fragment >
    );
  }

  renderScanDevicesDialog() {
    return (
      <Dialog
        open={this.state.confirmScanDevices}
        onClose={this.onScanDevicesRejected}
      >
        <DialogTitle>Confirm Scan Devices</DialogTitle>
        <DialogContent dividers>
          Are you sure you want to scan the EMS bus for all new devices?
        </DialogContent>
        <DialogActions>
          <Button variant="contained" onClick={this.onScanDevicesRejected} color="secondary">
            Cancel
          </Button>
          <Button startIcon={<RefreshIcon />} variant="contained" onClick={this.onScanDevicesConfirmed} disabled={this.state.processing} color="primary" autoFocus>
            Start Scan
          </Button>
        </DialogActions>
      </Dialog>
    )
  }

  onScanDevices = () => {
    this.setState({ confirmScanDevices: true });
  }

  onScanDevicesRejected = () => {
    this.setState({ confirmScanDevices: false });
  }

  onScanDevicesConfirmed = () => {
    this.setState({ processing: true });
    redirectingAuthorizedFetch(SCANDEVICES_ENDPOINT, { method: 'POST' })
      .then(response => {
        if (response.status === 200) {
          this.props.enqueueSnackbar("Device scan is starting...", { variant: 'info' });
          this.setState({ processing: false, confirmScanDevices: false });
        } else {
          throw Error("Invalid status code: " + response.status);
        }
      })
      .catch(error => {
        this.props.enqueueSnackbar(error.message || "Problem with scan", { variant: 'error' });
        this.setState({ processing: false, confirmScanDevices: false });
      });
  }

  render() {
    return (
      <Fragment>
        <List>{this.createListItems()}</List>

        <Box display="flex" flexWrap="wrap">
          <Box flexGrow={1} padding={1}>
            <FormButton startIcon={<RefreshIcon />} variant="contained" color="secondary" onClick={this.props.loadData}>
              Refresh
            </FormButton>
          </Box>

          <Box flexWrap="none" padding={1} whiteSpace="nowrap">
            <FormButton startIcon={<RefreshIcon />} variant="contained" color="primary" onClick={this.onScanDevices}>
              Scan Devices
              </FormButton>
          </Box>
        </Box>
        {this.renderScanDevicesDialog()}
      </Fragment>
    );
  }

}

export default withAuthenticatedContext(withWidth()(EMSESPDevicesForm));
