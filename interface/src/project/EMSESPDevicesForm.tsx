import React, { Component, Fragment } from 'react';
import { withStyles, Theme, createStyles } from '@material-ui/core/styles';

import parseMilliseconds from 'parse-ms';

import { Decoder } from '@msgpack/msgpack';
const decoder = new Decoder();

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
  Typography
} from '@material-ui/core';

import RefreshIcon from '@material-ui/icons/Refresh';
import ListIcon from '@material-ui/icons/List';
import IconButton from '@material-ui/core/IconButton';
import EditIcon from '@material-ui/icons/Edit';

import {
  redirectingAuthorizedFetch,
  withAuthenticatedContext,
  AuthenticatedContextProps
} from '../authentication';
import { RestFormProps, FormButton, extractEventValue } from '../components';

import {
  EMSESPDevices,
  EMSESPDeviceData,
  Device,
  DeviceValue,
  DeviceValueUOM,
  DeviceValueUOM_s
} from './EMSESPtypes';

import ValueForm from './ValueForm';

import { ENDPOINT_ROOT } from '../api';

export const SCANDEVICES_ENDPOINT = ENDPOINT_ROOT + 'scanDevices';
export const DEVICE_DATA_ENDPOINT = ENDPOINT_ROOT + 'deviceData';
export const WRITE_VALUE_ENDPOINT = ENDPOINT_ROOT + 'writeValue';

const StyledTableCell = withStyles((theme: Theme) =>
  createStyles({
    head: {
      backgroundColor: theme.palette.common.black,
      color: theme.palette.common.white
    },
    body: {
      fontSize: 14
    }
  })
)(TableCell);

const CustomTooltip = withStyles((theme: Theme) => ({
  tooltip: {
    backgroundColor: theme.palette.secondary.main,
    color: 'white',
    boxShadow: theme.shadows[1],
    fontSize: 11,
    border: '1px solid #dadde9'
  }
}))(Tooltip);

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
  selectedDevice?: number;
  edit_devicevalue?: DeviceValue;
}

type EMSESPDevicesFormProps = RestFormProps<EMSESPDevices> &
  AuthenticatedContextProps &
  WithWidthProps;

export const formatDuration = (duration_min: number) => {
  const { days, hours, minutes } = parseMilliseconds(duration_min * 60000);
  let formatted = '';
  if (days) {
    formatted += pluralize(days, 'day');
  }
  if (hours) {
    formatted += pluralize(hours, 'hour');
  }
  if (minutes) {
    formatted += pluralize(minutes, 'minute');
  }
  return formatted;
};

const pluralize = (count: number, noun: string, suffix = 's') =>
  ` ${count} ${noun}${count !== 1 ? suffix : ''} `;

function formatValue(value: any, uom: number) {
  switch (uom) {
    case DeviceValueUOM.HOURS:
      return value ? formatDuration(value * 60) : '0 hours';
    case DeviceValueUOM.MINUTES:
      return value ? formatDuration(value) : '0 minutes';
    case DeviceValueUOM.NONE:
      return value;
    case DeviceValueUOM.NUM:
      return new Intl.NumberFormat().format(value);
    case DeviceValueUOM.BOOLEAN:
      return value ? 'on' : 'off';
    default:
      return (
        new Intl.NumberFormat().format(value) + ' ' + DeviceValueUOM_s[uom]
      );
  }
}

class EMSESPDevicesForm extends Component<
  EMSESPDevicesFormProps,
  EMSESPDevicesFormState
> {
  state: EMSESPDevicesFormState = {
    confirmScanDevices: false,
    processing: false
  };

  handleValueChange = (name: keyof DeviceValue) => (
    event: React.ChangeEvent<HTMLInputElement>
  ) => {
    this.setState({
      edit_devicevalue: {
        ...this.state.edit_devicevalue!,
        [name]: extractEventValue(event)
      }
    });
  };

  cancelEditingValue = () => {
    this.setState({ edit_devicevalue: undefined });
  };

  doneEditingValue = () => {
    const { edit_devicevalue, selectedDevice } = this.state;

    redirectingAuthorizedFetch(WRITE_VALUE_ENDPOINT, {
      method: 'POST',
      body: JSON.stringify({
        id: selectedDevice,
        devicevalue: edit_devicevalue
      }),
      headers: {
        'Content-Type': 'application/json'
      }
    })
      .then((response) => {
        if (response.status === 200) {
          this.props.enqueueSnackbar('Write command sent to device', {
            variant: 'success'
          });
        } else if (response.status === 204) {
          this.props.enqueueSnackbar('Write command failed', {
            variant: 'error'
          });
        } else if (response.status === 403) {
          this.props.enqueueSnackbar('Write access denied', {
            variant: 'error'
          });
        } else {
          throw Error('Unexpected response code: ' + response.status);
        }
      })
      .catch((error) => {
        this.props.enqueueSnackbar(error.message || 'Problem writing value', {
          variant: 'error'
        });
      });

    if (edit_devicevalue) {
      this.setState({ edit_devicevalue: undefined });
    }
  };

  sendCommand = (dv: DeviceValue) => {
    this.setState({ edit_devicevalue: dv });
  };

  noDevices = () => {
    return this.props.data.devices.length === 0;
  };

  noSensors = () => {
    return this.props.data.sensors.length === 0;
  };

  noDeviceData = () => {
    return (this.state.deviceData?.data || []).length === 0;
  };

  renderDeviceItems() {
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
            padding={isWidthDown('xs', width!) ? 'none' : 'default'}
          >
            <TableBody>
              {data.devices.sort(compareDevices).map((device) => (
                <TableRow
                  hover
                  key={device.id}
                  onClick={() => this.handleRowClick(device)}
                >
                  <TableCell>
                    <CustomTooltip
                      title={
                        'DeviceID:0x' +
                        (
                          '00' + device.deviceid.toString(16).toUpperCase()
                        ).slice(-2) +
                        ' ProductID:' +
                        device.productid +
                        ' Version:' +
                        device.version
                      }
                      placement="right-end"
                    >
                      <Button
                        startIcon={<ListIcon />}
                        size="small"
                        variant="outlined"
                      >
                        {device.type}
                      </Button>
                    </CustomTooltip>
                  </TableCell>
                  <TableCell align="right">
                    {device.brand + ' ' + device.name}{' '}
                  </TableCell>
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

  renderSensorItems() {
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
                    {formatValue(sensorData.temp, DeviceValueUOM.DEGREES)}
                  </TableCell>
                </TableRow>
              ))}
            </TableBody>
          </Table>
        )}
        {this.noSensors() && (
          <Box color="warning.main" p={0} mt={0} mb={0}>
            <Typography variant="body1">
              <i>no external temperature sensors were detected</i>
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
        fullWidth
        maxWidth="sm"
      >
        <DialogTitle>Confirm Scan Devices</DialogTitle>
        <DialogContent dividers>
          Are you sure you want to start a scan on the EMS bus for all new
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
          this.props.enqueueSnackbar('Device scan is starting...', {
            variant: 'info'
          });
          this.setState({ processing: false, confirmScanDevices: false });
        } else {
          throw Error('Invalid status code: ' + response.status);
        }
      })
      .catch((error) => {
        this.props.enqueueSnackbar(error.message || 'Problem with scan', {
          variant: 'error'
        });
        this.setState({ processing: false, confirmScanDevices: false });
      });
  };

  handleRowClick = (device: any) => {
    this.setState({ selectedDevice: device.id, deviceData: undefined });
    redirectingAuthorizedFetch(DEVICE_DATA_ENDPOINT, {
      method: 'POST',
      body: JSON.stringify({ id: device.id }),
      headers: {
        'Content-Type': 'application/json'
      }
    })
      .then((response) => {
        if (response.status === 200) {
          return response.arrayBuffer();
        }
        throw Error('Unexpected response code: ' + response.status);
      })
      .then((arrayBuffer) => {
        const json: any = decoder.decode(arrayBuffer);
        this.setState({ deviceData: json });
      })
      .catch((error) => {
        this.props.enqueueSnackbar(
          error.message || 'Problem getting device data',
          { variant: 'error' }
        );
        this.setState({ deviceData: undefined });
      });
  };

  renderDeviceData() {
    const { deviceData } = this.state;
    const { width } = this.props;
    const me = this.props.authenticatedContext.me;

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
            {deviceData.name}
          </Typography>
        </Box>
        {!this.noDeviceData() && (
          <TableContainer>
            <Table
              size="small"
              padding={isWidthDown('xs', width!) ? 'none' : 'default'}
            >
              <TableHead></TableHead>
              <TableBody>
                {deviceData.data.map((item, i) => (
                  <TableRow hover key={i}>
                    <TableCell padding="checkbox" style={{ width: 18 }}>
                      {item.c && me.admin && (
                        <CustomTooltip
                          title="change value"
                          placement="left-end"
                        >
                          <IconButton
                            edge="start"
                            size="small"
                            aria-label="Edit"
                            onClick={() => this.sendCommand(item)}
                          >
                            <EditIcon color="primary" fontSize="small" />
                          </IconButton>
                        </CustomTooltip>
                      )}
                    </TableCell>
                    <TableCell padding="none" component="th" scope="row">
                      {item.n}
                    </TableCell>
                    <TableCell padding="none" align="right">
                      {formatValue(item.v, item.u)}
                    </TableCell>
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
    const { edit_devicevalue } = this.state;
    return (
      <Fragment>
        <br></br>
        {this.renderDeviceItems()}
        {this.renderDeviceData()}
        {this.renderSensorItems()}
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
              onClick={this.onScanDevices}
            >
              Scan Devices
            </FormButton>
          </Box>
        </Box>
        {this.renderScanDevicesDialog()}
        {edit_devicevalue && (
          <ValueForm
            devicevalue={edit_devicevalue}
            onDoneEditing={this.doneEditingValue}
            onCancelEditing={this.cancelEditingValue}
            handleValueChange={this.handleValueChange}
          />
        )}
      </Fragment>
    );
  }
}

export default withAuthenticatedContext(withWidth()(EMSESPDevicesForm));
