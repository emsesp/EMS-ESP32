import { FC, Fragment, useState, useContext } from 'react';

import {
  Button,
  Table,
  TableBody,
  TableHead,
  TableRow,
  Typography,
  Box,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  MenuItem,
  InputAdornment,
  IconButton,
  Grid
} from '@mui/material';

import Tooltip, { TooltipProps, tooltipClasses } from '@mui/material/Tooltip';
import TableCell, { tableCellClasses } from '@mui/material/TableCell';

import { styled } from '@mui/material/styles';

import parseMilliseconds from 'parse-ms';

import { useSnackbar } from 'notistack';

import RefreshIcon from '@mui/icons-material/Refresh';
import ListIcon from '@mui/icons-material/List';
import EditIcon from '@mui/icons-material/Edit';

import { AuthenticatedContext } from '../contexts/authentication';

import { ButtonRow, FormLoader, ValidatedTextField, SectionContent, MessageBox } from '../components';

import * as EMSESP from './api';

import { numberValue, updateValue, extractErrorMessage, useRest } from '../utils';

import { CoreData, DeviceData, Device, DeviceValue, DeviceValueUOM, DeviceValueUOM_s, Sensor } from './types';

const StyledTableCell = styled(TableCell)(({ theme }) => ({
  [`&.${tableCellClasses.head}`]: {
    backgroundColor: theme.palette.common.black,
    color: theme.palette.common.white,
    fontSize: 14
  }
  // [`&.${tableCellClasses.body}`]: {
  //   fontSize: 12
  // }
}));

const StyledTableRow = styled(TableRow)(({ theme }) => ({
  '&:nth-of-type(odd)': {
    backgroundColor: theme.palette.action.hover
  },
  // hide last border
  '&:last-child td, &:last-child th': {
    border: 0
  },
  '&:hover': {
    backgroundColor: theme.palette.info.light
  }
}));

const StyledTooltip = styled(({ className, ...props }: TooltipProps) => (
  <Tooltip {...props} classes={{ popper: className }} />
))(({ theme }) => ({
  [`& .${tooltipClasses.tooltip}`]: {
    backgroundColor: theme.palette.secondary.dark,
    color: 'white',
    border: '1px solid #dadde9'
  }
}));

const DashboardData: FC = () => {
  const { loadData, data, errorMessage } = useRest<CoreData>({ read: EMSESP.readData });

  const { me } = useContext(AuthenticatedContext);

  const { enqueueSnackbar } = useSnackbar();

  const [deviceData, setDeviceData] = useState<DeviceData>();
  const [deviceValue, setDeviceValue] = useState<DeviceValue>();
  const [sensor, setSensor] = useState<Sensor>();
  const [selectedDevice, setSelectedDevice] = useState<number>(0);

  const fetchDeviceData = async (unique_id: number) => {
    setSelectedDevice(unique_id);
    try {
      setDeviceData((await EMSESP.readDeviceData({ id: unique_id })).data);
    } catch (error: any) {
      enqueueSnackbar(extractErrorMessage(error, 'Problem fetching device data'), { variant: 'error' });
    }
  };

  const pluralize = (count: number, noun: string, suffix = 's') =>
    ` ${Intl.NumberFormat().format(count)} ${noun}${count !== 1 ? suffix : ''} `;

  const formatDuration = (duration_min: number) => {
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

  function formatValue(value: any, uom: number) {
    if (value === undefined) {
      return 'offline';
    }
    switch (uom) {
      case DeviceValueUOM.HOURS:
        return value ? formatDuration(value * 60) : '0 hours';
      case DeviceValueUOM.MINUTES:
        return value ? formatDuration(value) : '0 minutes';
      case DeviceValueUOM.NONE:
        if (typeof value === 'number') {
          return new Intl.NumberFormat().format(value);
        }
        return value;
      case DeviceValueUOM.DEGREES:
        return (
          new Intl.NumberFormat(undefined, {
            minimumFractionDigits: 1
          }).format(value) +
          ' ' +
          DeviceValueUOM_s[uom]
        );
      case DeviceValueUOM.TIMES:
      case DeviceValueUOM.SECONDS:
        return pluralize(value, DeviceValueUOM_s[uom]);
      default:
        return new Intl.NumberFormat().format(value) + ' ' + DeviceValueUOM_s[uom];
    }
  }

  const sendDeviceValue = async () => {
    if (deviceValue && deviceData) {
      try {
        const response = await EMSESP.writeValue({
          id: deviceData.id,
          devicevalue: deviceValue
        });
        if (response.status === 204) {
          enqueueSnackbar('Write command failed', { variant: 'error' });
        } else if (response.status === 403) {
          enqueueSnackbar('Write access denied', { variant: 'error' });
        } else {
          enqueueSnackbar('Write command sent', { variant: 'success' });
        }
        setDeviceValue(undefined);
      } catch (error: any) {
        enqueueSnackbar(extractErrorMessage(error, 'Problem writing value'), { variant: 'error' });
      } finally {
        setDeviceValue(undefined);
        fetchDeviceData(deviceData.id);
        loadData();
      }
    }
  };

  const renderDeviceValueDialog = () => {
    if (deviceValue) {
      return (
        <Dialog open={deviceValue !== undefined} onClose={() => setDeviceValue(undefined)}>
          <DialogTitle>Change Value</DialogTitle>
          <DialogContent dividers>
            {deviceValue.l && (
              <ValidatedTextField
                name="v"
                label={deviceValue.n}
                value={deviceValue.v}
                autoFocus
                sx={{ width: '30ch' }}
                select
                onChange={updateValue(setDeviceValue)}
              >
                {deviceValue.l.map((val) => (
                  <MenuItem value={val}>{val}</MenuItem>
                ))}
              </ValidatedTextField>
            )}
            {!deviceValue.l && (
              <ValidatedTextField
                name="v"
                label={deviceValue.n}
                value={deviceValue.v}
                autoFocus
                sx={{ width: '30ch' }}
                type={deviceValue.u ? 'number' : 'text'}
                onChange={updateValue(setDeviceValue)}
                InputProps={{
                  startAdornment: <InputAdornment position="start">{DeviceValueUOM_s[deviceValue.u]}</InputAdornment>
                }}
              />
            )}
          </DialogContent>
          <DialogActions>
            <Button variant="outlined" onClick={() => setDeviceValue(undefined)} color="secondary">
              Cancel
            </Button>
            <Button variant="outlined" type="submit" onClick={() => sendDeviceValue()} color="warning">
              Send
            </Button>
          </DialogActions>
        </Dialog>
      );
    }
  };

  const sendSensor = async () => {
    if (sensor) {
      try {
        const response = await EMSESP.writeSensor({
          id_str: sensor.is,
          name: sensor.n,
          offset: sensor.o
        });
        if (response.status === 204) {
          enqueueSnackbar('Sensor change failed', { variant: 'error' });
        } else if (response.status === 403) {
          enqueueSnackbar('Access denied', { variant: 'error' });
        } else {
          enqueueSnackbar('Sensor updated', { variant: 'success' });
        }
        setSensor(undefined);
      } catch (error: any) {
        enqueueSnackbar(extractErrorMessage(error, 'Problem updating sensor'), { variant: 'error' });
      } finally {
        setSensor(undefined);
        loadData();
      }
    }
  };

  const renderSensorDialog = () => {
    if (sensor) {
      return (
        <Dialog open={sensor !== undefined} onClose={() => setSensor(undefined)}>
          <DialogTitle>Edit Sensor</DialogTitle>
          <DialogContent dividers>
            <Box color="warning.main" p={0} pl={0} pr={0} mt={0} mb={2}>
              <Typography variant="body2">Sensor ID {sensor.is}</Typography>
            </Box>
            <Grid container spacing={1}>
              <Grid item>
                <ValidatedTextField
                  name="n"
                  label="Name"
                  value={sensor.n}
                  autoFocus
                  sx={{ width: '30ch' }}
                  onChange={updateValue(setSensor)}
                />
              </Grid>
              <Grid item>
                <ValidatedTextField
                  name="o"
                  label="Offset"
                  value={numberValue(sensor.o)}
                  sx={{ width: '12ch' }}
                  type="number"
                  variant="outlined"
                  onChange={updateValue(setSensor)}
                  inputProps={{ min: '-5', max: '5', step: '0.1' }}
                  InputProps={{
                    startAdornment: <InputAdornment position="start">°C</InputAdornment>
                  }}
                />
              </Grid>
            </Grid>
          </DialogContent>
          <DialogActions>
            <Button variant="outlined" onClick={() => setSensor(undefined)} color="secondary">
              Cancel
            </Button>
            <Button variant="outlined" type="submit" onClick={() => sendSensor()} color="warning">
              Save
            </Button>
          </DialogActions>
        </Dialog>
      );
    }
  };

  const renderData = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage} />;
    }

    const noDevices = () => {
      return data.devices.length === 0;
    };

    function compareDevices(a: Device, b: Device) {
      if (a.t < b.t) {
        return -1;
      }
      if (a.t > b.t) {
        return 1;
      }
      return 0;
    }

    return (
      <>
        <Typography sx={{ pt: 2 }} variant="h6" color="primary">
          EMS Devices
        </Typography>
        {!noDevices() && (
          <ButtonRow>
            {data.devices.sort(compareDevices).map((device) => (
              <StyledTooltip
                key={device.i}
                title={
                  <Fragment>
                    <Typography variant="h6" color="primary">
                      Device Details
                    </Typography>
                    {'Type: ' + device.t}
                    <br />
                    {'Name: ' + device.n}
                    <br />
                    {'Brand: ' + device.b}
                    <br />
                    {'DeviceID: 0x' + ('00' + device.d.toString(16).toUpperCase()).slice(-2)}
                    <br />
                    {'ProductID: ' + device.p}
                    <br />
                    {'Version: ' + device.v}
                  </Fragment>
                }
                placement="top"
              >
                <Button
                  startIcon={<ListIcon />}
                  size="small"
                  variant={selectedDevice === device.i ? 'contained' : 'outlined'}
                  color={selectedDevice === device.i ? 'secondary' : 'inherit'}
                  onClick={() => device.e && fetchDeviceData(device.i)}
                >
                  {device.s}&nbsp;({device.e})
                </Button>
              </StyledTooltip>
            ))}
          </ButtonRow>
        )}
        {noDevices() && (
          <MessageBox
            my={2}
            level="error"
            message="No EMS devices found. Check the interface's hardware connections and for possible Rx and Tx errors in the System Log"
          />
        )}
      </>
    );
  };

  const renderDeviceData = () => {
    if (data?.devices.length === 0 || !deviceData) {
      return;
    }

    const sendCommand = (dv: DeviceValue) => {
      if (dv.c && me.admin) {
        setDeviceValue(dv);
      }
    };

    return (
      <>
        <Typography sx={{ pt: 2, pb: 1 }} variant="h6" color="primary">
          {deviceData.type}&nbsp;Data
        </Typography>
        <Table size="small">
          <TableHead>
            <TableRow>
              <StyledTableCell padding="checkbox" style={{ width: 18 }}></StyledTableCell>
              <StyledTableCell align="left">Entity Name</StyledTableCell>
              <StyledTableCell align="right">Value</StyledTableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            {deviceData.data.map((dv, i) => (
              <StyledTableRow key={i} onClick={() => sendCommand(dv)}>
                <StyledTableCell padding="checkbox">
                  {dv.c && me.admin && (
                    <StyledTooltip title="change value" placement="left-end">
                      <IconButton edge="start" size="small" aria-label="Edit">
                        <EditIcon color="primary" fontSize="small" />
                      </IconButton>
                    </StyledTooltip>
                  )}
                </StyledTableCell>
                <StyledTableCell component="th" scope="row">
                  {dv.n}
                </StyledTableCell>
                <StyledTableCell align="right">{formatValue(dv.v, dv.u)}</StyledTableCell>
              </StyledTableRow>
            ))}
          </TableBody>
        </Table>
      </>
    );
  };

  const updateSensor = (sensordata: Sensor) => {
    if (sensordata && me.admin) {
      setSensor(sensordata);
    }
  };

  const renderSensorData = () => (
    <>
      <Typography sx={{ pt: 2, pb: 1 }} variant="h6" color="primary">
        Dallas Sensors
      </Typography>
      <Table size="small">
        <TableHead>
          <TableRow>
            <StyledTableCell padding="checkbox" style={{ width: 18 }}></StyledTableCell>
            <StyledTableCell align="left">Name</StyledTableCell>
            <StyledTableCell align="right">Temperature</StyledTableCell>
          </TableRow>
        </TableHead>
        <TableBody>
          {data?.sensors.map((sensorData) => (
            <StyledTableRow key={sensorData.n} onClick={() => updateSensor(sensorData)}>
              <StyledTableCell padding="checkbox">
                {me.admin && (
                  <StyledTooltip title="edit" placement="left-end">
                    <IconButton edge="start" size="small" aria-label="Edit">
                      <EditIcon color="primary" fontSize="small" />
                    </IconButton>
                  </StyledTooltip>
                )}
              </StyledTableCell>
              <StyledTableCell component="th" scope="row">
                {sensorData.n}
              </StyledTableCell>
              <StyledTableCell align="right">{formatValue(sensorData.t, DeviceValueUOM.DEGREES)}</StyledTableCell>
            </StyledTableRow>
          ))}
        </TableBody>
      </Table>
    </>
  );

  const renderAnalogData = () => (
    <>
      <Typography sx={{ pt: 2, pb: 1 }} variant="h6" color="primary">
        Analog Sensors
      </Typography>
      <Table size="small">
        <TableHead>
          <TableRow>
            <StyledTableCell style={{ width: 18 }}></StyledTableCell>
            <StyledTableCell>Type</StyledTableCell>
            <StyledTableCell align="right">Value</StyledTableCell>
          </TableRow>
        </TableHead>
        <TableBody>
          <StyledTableRow>
            <StyledTableCell>&nbsp;&nbsp;</StyledTableCell>
            <StyledTableCell component="th" scope="row">
              Analog Input (ADC0)
            </StyledTableCell>
            <StyledTableCell align="right">{formatValue(data?.analog, DeviceValueUOM.MV)}</StyledTableCell>
          </StyledTableRow>
        </TableBody>
      </Table>
    </>
  );

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage} />;
    }

    return (
      <>
        {renderData()}
        {renderDeviceData()}
        {data.sensors.length !== 0 && renderSensorData()}
        {data.analog && renderAnalogData()}
        {renderDeviceValueDialog()}
        {renderSensorDialog()}
        <ButtonRow>
          <Button startIcon={<RefreshIcon />} variant="outlined" color="secondary" onClick={loadData}>
            Refresh
          </Button>
        </ButtonRow>
      </>
    );
  };

  return (
    <SectionContent title="Device and Sensor Data" titleGutter>
      {content()}
    </SectionContent>
  );
};

export default DashboardData;
