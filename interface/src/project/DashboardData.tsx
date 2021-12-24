import { FC, Fragment, useState, useContext, useEffect } from 'react';

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
import EditIcon from '@mui/icons-material/Edit';
import SensorsIcon from '@mui/icons-material/Sensors';
import CategoryIcon from '@mui/icons-material/Category';

import { AuthenticatedContext } from '../contexts/authentication';

import { ButtonRow, FormLoader, ValidatedTextField, SectionContent, MessageBox } from '../components';

import * as EMSESP from './api';

import { numberValue, updateValue, extractErrorMessage, useRest } from '../utils';

import {
  SensorData,
  CoreData,
  DeviceData,
  Device,
  DeviceValue,
  DeviceValueUOM,
  DeviceValueUOM_s,
  Sensor,
  Analog
} from './types';

const StyledTableCell = styled(TableCell)(({ theme }) => ({
  [`&.${tableCellClasses.head}`]: {
    backgroundColor: theme.palette.common.black,
    color: theme.palette.common.white,
    fontSize: 14
  }
}));

const StyledTableRow = styled(TableRow)(({ theme }) => ({
  '&:nth-of-type(odd)': {
    backgroundColor: theme.palette.action.hover
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
  const { loadData, data, errorMessage } = useRest<CoreData>({ read: EMSESP.readCoreData });

  const { me } = useContext(AuthenticatedContext);

  const { enqueueSnackbar } = useSnackbar();

  const [deviceData, setDeviceData] = useState<DeviceData>();
  const [sensorData, setSensorData] = useState<SensorData>();
  const [deviceValue, setDeviceValue] = useState<DeviceValue>();
  const [sensor, setSensor] = useState<Sensor>();
  const [analog, setAnalog] = useState<Analog>();
  const [selectedDevice, setSelectedDevice] = useState<number>();

  const refreshData = () => {
    if (analog || sensor || deviceValue) {
      return;
    }
    loadData();

    if (selectedDevice === 0) {
      fetchSensorData();
    } else if (selectedDevice) {
      fetchDeviceData(selectedDevice);
    }
  };

  useEffect(() => {
    const timer = setInterval(() => refreshData(), 30000);
    return () => {
      clearInterval(timer);
    };
    // eslint-disable-next-line
  }, [analog, sensor, deviceValue, selectedDevice]);

  const fetchDeviceData = async (unique_id: number) => {
    try {
      setDeviceData((await EMSESP.readDeviceData({ id: unique_id })).data);
    } catch (error: any) {
      enqueueSnackbar(extractErrorMessage(error, 'Problem fetching device data'), { variant: 'error' });
    } finally {
      setSelectedDevice(unique_id);
      setSensorData(undefined);
    }
  };

  const fetchSensorData = async () => {
    try {
      setSensorData((await EMSESP.readSensorData()).data);
    } catch (error: any) {
      enqueueSnackbar(extractErrorMessage(error, 'Problem fetching sensor data'), { variant: 'error' });
    } finally {
      setSelectedDevice(0);
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
      return '';
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
      case DeviceValueUOM.DEGREES_R:
      case DeviceValueUOM.FAHRENHEIT:
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
    if (selectedDevice && deviceValue) {
      try {
        const response = await EMSESP.writeValue({
          id: selectedDevice,
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
        fetchDeviceData(selectedDevice);
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
                value={deviceValue.u ? Intl.NumberFormat().format(deviceValue.v) : deviceValue.v}
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
        fetchSensorData();
      }
    }
  };

  const renderSensorDialog = () => {
    if (sensor) {
      return (
        <Dialog open={sensor !== undefined} onClose={() => setSensor(undefined)}>
          <DialogTitle>Edit Temperature Sensor</DialogTitle>
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

  const renderCoreData = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage} />;
    }

    const noDevices = () => {
      return data.devices.length === 0;
    };

    const haveSensors = () => {
      return data.dallassensor_count + data.analogsensor_count !== 0;
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
        <Table size="small">
          <TableHead>
            <TableRow>
              <StyledTableCell align="left" padding="checkbox"></StyledTableCell>
              <StyledTableCell>Type</StyledTableCell>
              <StyledTableCell>Name</StyledTableCell>
              <StyledTableCell align="right"># Entities</StyledTableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            {data.devices.sort(compareDevices).map((device) => (
              <TableRow
                hover
                selected={device.i === selectedDevice}
                key={device.i}
                onClick={() => device.e && fetchDeviceData(device.i)}
              >
                <TableCell>
                  <StyledTooltip
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
                    <CategoryIcon color="primary" fontSize="large" />
                  </StyledTooltip>
                </TableCell>
                <TableCell>{device.t}</TableCell>
                <TableCell>{device.n}</TableCell>
                <TableCell align="right">{device.e}</TableCell>
              </TableRow>
            ))}
            {haveSensors() && (
              <TableRow hover selected={selectedDevice === 0} onClick={() => fetchSensorData()}>
                <TableCell>
                  <SensorsIcon color="primary" fontSize="large" />
                </TableCell>
                <TableCell>Sensors</TableCell>
                <TableCell>Temperature and Analog Sensors</TableCell>
                <TableCell align="right">{data.dallassensor_count + data.analogsensor_count}</TableCell>
              </TableRow>
            )}
          </TableBody>
        </Table>

        {noDevices() && (
          <MessageBox
            my={2}
            level="error"
            message="No EMS devices found. If this message persists then check the interface board and look for any errors in the System Log"
          />
        )}
      </>
    );
  };

  const renderDeviceData = () => {
    if (data?.devices.length === 0 || !deviceData || !selectedDevice) {
      return;
    }

    const sendCommand = (dv: DeviceValue) => {
      if (dv.c && me.admin) {
        setDeviceValue(dv);
      }
    };

    const renderNameCell = (dv: DeviceValue) => {
      if (dv.v === undefined && dv.c) {
        return (
          <StyledTableCell component="th" scope="row" sx={{ color: 'yellow' }}>
            call command:&nbsp;{dv.n}
          </StyledTableCell>
        );
      }
      return (
        <StyledTableCell component="th" scope="row">
          {dv.n}
        </StyledTableCell>
      );
    };

    return (
      <>
        <Typography sx={{ pt: 2, pb: 1 }} variant="h6" color="primary">
          {deviceData.label}
        </Typography>
        <Table size="small">
          <TableHead>
            <TableRow>
              <StyledTableCell padding="checkbox" style={{ width: 18 }}></StyledTableCell>
              <StyledTableCell align="left">Entity Name/Command</StyledTableCell>
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
                {renderNameCell(dv)}
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

  const updateAnalog = (analogdata: Analog) => {
    if (me.admin) {
      setAnalog(analogdata);
    }
  };

  const renderDallasData = () => (
    <>
      <Typography sx={{ pt: 2, pb: 1 }} variant="h6" color="primary">
        Temperature Sensors
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
          {sensorData?.sensors.map((sensor_data) => (
            <StyledTableRow key={sensor_data.n} onClick={() => updateSensor(sensor_data)}>
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
                {sensor_data.n}
              </StyledTableCell>
              <StyledTableCell align="right">{formatValue(sensor_data.t, sensor_data.u)}</StyledTableCell>
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
        <TableBody>
          {sensorData?.analogs.map((analog_data) => (
            <StyledTableRow key={analog_data.n} onClick={() => updateAnalog(analog_data)}>
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
                {analog_data.n}
              </StyledTableCell>
              <StyledTableCell align="right">
                {Intl.NumberFormat().format(analog_data.v)}&nbsp;{analog_data.u}{' '}
              </StyledTableCell>
            </StyledTableRow>
          ))}
        </TableBody>
      </Table>
    </>
  );

  const sendAnalog = async () => {
    if (analog) {
      try {
        const response = await EMSESP.writeAnalog({
          id: analog.i,
          name: analog.n,
          offset: analog.o,
          factor: analog.f,
          uom: analog.u
        });
        if (response.status === 204) {
          enqueueSnackbar('Analog calibration failed', { variant: 'error' });
        } else if (response.status === 403) {
          enqueueSnackbar('Access denied', { variant: 'error' });
        } else {
          enqueueSnackbar('Analog calibration updated', { variant: 'success' });
        }
        // setAnalog(undefined);
      } catch (error: any) {
        enqueueSnackbar(extractErrorMessage(error, 'Problem updating analog'), { variant: 'error' });
      } finally {
        setAnalog(undefined);
        fetchSensorData();
      }
    }
  };

  const renderAnalogDialog = () => {
    if (analog) {
      return (
        <Dialog open={analog !== undefined} onClose={() => setAnalog(undefined)}>
          <DialogTitle>Edit Analog Sensor</DialogTitle>
          <DialogContent dividers>
            <Grid container spacing={1}>
              <Grid item>
                <ValidatedTextField
                  name="n"
                  label="Name"
                  value={analog.n}
                  autoFocus
                  sx={{ width: '30ch' }}
                  variant="outlined"
                  onChange={updateValue(setAnalog)}
                />
              </Grid>
              <Grid item>
                <ValidatedTextField
                  name="u"
                  label="Unit"
                  value={analog.u}
                  sx={{ width: '10ch' }}
                  variant="outlined"
                  onChange={updateValue(setAnalog)}
                />
              </Grid>
            </Grid>
            <p></p>
            <Grid container spacing={1}>
              <Grid item>
                <ValidatedTextField
                  name="o"
                  label="Offset"
                  value={numberValue(analog.o)}
                  sx={{ width: '20ch' }}
                  type="number"
                  variant="outlined"
                  onChange={updateValue(setAnalog)}
                  inputProps={{ min: '0', max: '3300', step: '1' }}
                  InputProps={{
                    endAdornment: <InputAdornment position="end">mV</InputAdornment>
                  }}
                />
              </Grid>
              <Grid item>
                <ValidatedTextField
                  name="f"
                  label="Factor"
                  value={numberValue(analog.f)}
                  sx={{ width: '20ch' }}
                  type="number"
                  variant="outlined"
                  onChange={updateValue(setAnalog)}
                  inputProps={{ min: '-100', max: '100', step: '0.1' }}
                />
              </Grid>
            </Grid>
          </DialogContent>
          <DialogActions>
            <Button variant="outlined" onClick={() => setAnalog(undefined)} color="secondary">
              Cancel
            </Button>
            <Button variant="outlined" type="submit" onClick={() => sendAnalog()} color="warning">
              Save
            </Button>
          </DialogActions>
        </Dialog>
      );
    }
  };

  const content = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage} />;
    }

    return (
      <>
        {renderCoreData()}
        {renderDeviceData()}
        {sensorData && sensorData.sensors.length > 0 && renderDallasData()}
        {sensorData && sensorData.analogs.length > 0 && renderAnalogData()}
        {renderDeviceValueDialog()}
        {renderSensorDialog()}
        {renderAnalogDialog()}
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
