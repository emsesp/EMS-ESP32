import React, { Component, Fragment } from "react";
import { withStyles, Theme, createStyles } from "@material-ui/core/styles";

import {
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableRow,
  TableContainer,
  withWidth,
  WithWidthProps,
  isWidthDown,
  Button,
  Tooltip,
  DialogTitle,
  DialogContent,
  DialogActions,
  Box,
  Dialog,
  Typography,
} from "@material-ui/core";

import RefreshIcon from "@material-ui/icons/Refresh";
import ListIcon from "@material-ui/icons/List";

import {
  redirectingAuthorizedFetch,
  withAuthenticatedContext,
  AuthenticatedContextProps,
} from "../authentication";

import { RestFormProps, FormButton } from "../components";

import { EMSESPDevices, EMSESPDeviceData, Device } from "./EMSESPtypes";

import { ENDPOINT_ROOT } from "../api";
export const SCANDEVICES_ENDPOINT = ENDPOINT_ROOT + "scanDevices";
export const DEVICE_DATA_ENDPOINT = ENDPOINT_ROOT + "deviceData";

const StyledTableCell = withStyles((theme: Theme) =>
  createStyles({
    head: {
      backgroundColor: theme.palette.common.black,
      color: theme.palette.common.white,
    },
    body: {
      fontSize: 14,
    },
  })
)(TableCell);

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
  deviceData?: EMSESPDeviceData;
}

type EMSESPDevicesFormProps = RestFormProps<EMSESPDevices> &
  AuthenticatedContextProps &
  WithWidthProps;

function formatTemp(t: string) {
  if (t == null) {
    return "(not available)";
  }
  return t + " °C";
}

class EMSESPDevicesForm extends Component<
  EMSESPDevicesFormProps,
  EMSESPDevicesFormState
> {
  state: EMSESPDevicesFormState = {
    confirmScanDevices: false,
    processing: false,
  };

  noDevices = () => {
    return this.props.data.devices.length === 0;
  };

  noSensors = () => {
    return this.props.data.sensors.length === 0;
  };

  noDeviceData = () => {
    return (this.state.deviceData?.deviceData || []).length === 0;
  };

  createDeviceItems() {
    const { width, data } = this.props;
    return (
      <TableContainer>
        <Typography variant="h6" color="primary">
          EMS Devices
        </Typography>
        <p></p>
        {!this.noDevices() && (
          <Table
            size="small"
            padding={isWidthDown("xs", width!) ? "none" : "default"}
          >
            <TableHead>
              <TableRow>
                <StyledTableCell>Type</StyledTableCell>
                <StyledTableCell align="center">Brand</StyledTableCell>
                <StyledTableCell align="center">Model</StyledTableCell>
                <StyledTableCell align="center">Device ID</StyledTableCell>
                <StyledTableCell align="center">Product ID</StyledTableCell>
                <StyledTableCell align="center">Version</StyledTableCell>
                <StyledTableCell></StyledTableCell>
              </TableRow>
            </TableHead>
            <TableBody>
              {data.devices.sort(compareDevices).map((device) => (
                <TableRow
                  hover
                  key={device.id}
                  onClick={() => this.handleRowClick(device.id)}
                >
                  <TableCell component="th" scope="row">
                    <Tooltip
                      title="click for details..."
                      arrow
                      placement="right-end"
                    >
                      <Button
                        startIcon={<ListIcon />}
                        size="small"
                        variant="outlined"
                      >
                        {device.type}
                      </Button>
                    </Tooltip>
                  </TableCell>
                  <TableCell align="center">{device.brand}</TableCell>
                  <TableCell align="center">{device.name}</TableCell>
                  <TableCell align="center">
                    0x
                    {("00" + device.deviceid.toString(16).toUpperCase()).slice(
                      -2
                    )}
                  </TableCell>
                  <TableCell align="center">{device.productid}</TableCell>
                  <TableCell align="center">{device.version}</TableCell>
                  <TableCell></TableCell>
                </TableRow>
              ))}
            </TableBody>
          </Table>
        )}
        {this.noDevices() && (
          <Box
            bgcolor="error.main"
            color="error.contrastText"
            p={2}
            mt={2}
            mb={2}
          >
            <Typography variant="body1">
              No EMS devices found. Check the connections and for possible Tx
              errors.
            </Typography>
          </Box>
        )}
      </TableContainer>
    );
  }

  createSensorItems() {
    const { data } = this.props;
    return (
      <TableContainer>
        <p></p>
        <Typography variant="h6" color="primary" paragraph>
          Dallas Sensors
        </Typography>
        {!this.noSensors() && (
          <Table size="small" padding="default">
            <TableHead>
              <TableRow>
                <StyledTableCell>Sensor #</StyledTableCell>
                <StyledTableCell align="center">ID</StyledTableCell>
                <StyledTableCell align="right">Temperature</StyledTableCell>
              </TableRow>
            </TableHead>
            <TableBody>
              {data.sensors.map((sensorData) => (
                <TableRow key={sensorData.no}>
                  <TableCell component="th" scope="row">
                    {sensorData.no}
                  </TableCell>
                  <TableCell align="center">{sensorData.id}</TableCell>
                  <TableCell align="right">
                    {formatTemp(sensorData.temp)}
                  </TableCell>
                </TableRow>
              ))}
            </TableBody>
          </Table>
        )}
        {this.noSensors() && (
          <Box color="warning.main" p={0} mt={0} mb={0}>
            <Typography variant="body1">
              <i>There are no external temperature sensors connected</i>
            </Typography>
          </Box>
        )}
      </TableContainer>
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
          Are you sure you want to initiate a scan on the EMS bus for all new
          devices?
        </DialogContent>
        <DialogActions>
          <Button
            variant="contained"
            onClick={this.onScanDevicesRejected}
            color="secondary"
          >
            Cancel
          </Button>
          <Button
            startIcon={<RefreshIcon />}
            variant="contained"
            onClick={this.onScanDevicesConfirmed}
            disabled={this.state.processing}
            color="primary"
            autoFocus
          >
            Start Scan
          </Button>
        </DialogActions>
      </Dialog>
    );
  }

  onScanDevices = () => {
    this.setState({ confirmScanDevices: true });
  };

  onScanDevicesRejected = () => {
    this.setState({ confirmScanDevices: false });
  };

  onScanDevicesConfirmed = () => {
    this.setState({ processing: true });
    redirectingAuthorizedFetch(SCANDEVICES_ENDPOINT)
      .then((response) => {
        if (response.status === 200) {
          this.props.enqueueSnackbar("Device scan is starting...", {
            variant: "info",
          });
          this.setState({ processing: false, confirmScanDevices: false });
        } else {
          throw Error("Invalid status code: " + response.status);
        }
      })
      .catch((error) => {
        this.props.enqueueSnackbar(error.message || "Problem with scan", {
          variant: "error",
        });
        this.setState({ processing: false, confirmScanDevices: false });
      });
  };

  handleRowClick = (id: any) => {
    this.setState({ deviceData: undefined });
    redirectingAuthorizedFetch(DEVICE_DATA_ENDPOINT, {
      method: "POST",
      body: JSON.stringify({ id: id }),
      headers: {
        "Content-Type": "application/json",
      },
    })
      .then((response) => {
        if (response.status === 200) {
          return response.json();
          // this.setState({ errorMessage: undefined }, this.props.loadData);
        }
        throw Error("Unexpected response code: " + response.status);
      })
      .then((json) => {
        this.setState({ deviceData: json });
      })
      .catch((error) => {
        this.props.enqueueSnackbar(
          error.message || "Problem getting device data",
          { variant: "error" }
        );
        this.setState({ deviceData: undefined });
      });
  };

  renderDeviceData() {
    const { deviceData } = this.state;
    const { width } = this.props;

    if (this.noDevices()) {
      return;
    }

    if (!deviceData) {
      return;
    }

    return (
      <Fragment>
        <p></p>
        <Box bgcolor="info.main" p={1} mt={1} mb={1}>
          <Typography variant="body1" color="initial">
            {deviceData.deviceName}
          </Typography>
        </Box>
        {!this.noDeviceData() && (
          <TableContainer>
            <Table
              size="small"
              padding={isWidthDown("xs", width!) ? "none" : "default"}
            >
              <TableHead></TableHead>
              <TableBody>
                {deviceData.deviceData.map((deviceData) => (
                  <TableRow key={deviceData.n}>
                    <TableCell component="th" scope="row">
                      {deviceData.n}
                    </TableCell>
                    <TableCell align="right">{deviceData.v}</TableCell>
                  </TableRow>
                ))}
              </TableBody>
            </Table>
          </TableContainer>
        )}
        {this.noDeviceData() && (
          <Box color="warning.main" p={0} mt={0} mb={0}>
            <Typography variant="body1">
              <i>No data available for this device</i>
            </Typography>
          </Box>
        )}
      </Fragment>
    );
  }

  render() {
    return (
      <Fragment>
        <br></br>
        {this.createDeviceItems()}
        {this.renderDeviceData()}
        {this.createSensorItems()}
        <br></br>
        <Box display="flex" flexWrap="wrap">
          <Box flexGrow={1} padding={1}>
            <FormButton
              startIcon={<RefreshIcon />}
              variant="contained"
              color="secondary"
              onClick={this.props.loadData}
            >
              Refresh
            </FormButton>
          </Box>
          <Box flexWrap="none" padding={1} whiteSpace="nowrap">
            <FormButton
              startIcon={<RefreshIcon />}
              variant="contained"
              color="primary"
              onClick={this.onScanDevices}
            >
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
