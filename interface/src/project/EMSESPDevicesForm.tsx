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
  isWidthDown
} from "@material-ui/core";

import { Box, Typography } from '@material-ui/core';

import RefreshIcon from "@material-ui/icons/Refresh";

import { withAuthenticatedContext, AuthenticatedContextProps } from '../authentication';

import {
  RestFormProps,
  FormActions,
  FormButton,
} from "../components";

import { EMSESPDevices, Device } from "./types";

function compareDevices(a: Device, b: Device) {
  if (a.type < b.type) {
    return -1;
  }
  if (a.type > b.type) {
    return 1;
  }
  return 0;
}

type EMSESPDevicesFormProps = RestFormProps<EMSESPDevices> & AuthenticatedContextProps & WithWidthProps;

class EMSESPDevicesForm extends Component<EMSESPDevicesFormProps> {

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

export default withAuthenticatedContext(withWidth()(EMSESPDevicesForm));

// export default withTheme(EMSESPDevicesForm);
