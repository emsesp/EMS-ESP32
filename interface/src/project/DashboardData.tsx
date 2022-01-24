import { FC, useState, useContext, useEffect } from 'react';

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
  FormHelperText,
  IconButton,
  List,
  ListItem,
  ListItemText,
  Grid,
  useMediaQuery
} from '@mui/material';

import TableCell, { tableCellClasses } from '@mui/material/TableCell';

import { styled } from '@mui/material/styles';

import parseMilliseconds from 'parse-ms';

import { useSnackbar } from 'notistack';

import RefreshIcon from '@mui/icons-material/Refresh';
import EditIcon from '@mui/icons-material/Edit';
import InfoOutlinedIcon from '@mui/icons-material/InfoOutlined';
import AddCircleOutlineOutlinedIcon from '@mui/icons-material/AddCircleOutlineOutlined';
import CancelIcon from '@mui/icons-material/Cancel';
import SendIcon from '@mui/icons-material/TrendingFlat';
import SaveIcon from '@mui/icons-material/Save';
import RemoveIcon from '@mui/icons-material/RemoveCircleOutline';

import DeviceIcon from './DeviceIcon';

import { IconContext } from 'react-icons';

import { AuthenticatedContext } from '../contexts/authentication';

import { ButtonRow, FormLoader, ValidatedTextField, SectionContent, MessageBox } from '../components';

import * as EMSESP from './api';

import { numberValue, updateValue, extractErrorMessage, useRest } from '../utils';

import {
  SensorData,
  CoreData,
  DeviceData,
  DeviceValue,
  DeviceValueUOM,
  DeviceValueUOM_s,
  AnalogTypes,
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
  const [deviceDialog, setDeviceDialog] = useState<number>(-1);

  const desktopWindow = useMediaQuery('(min-width:600px)');

  const refreshData = () => {
    if (analog || sensor || deviceValue) {
      return;
    }
    loadData();

    if (sensorData) {
      fetchSensorData();
    } else if (selectedDevice) {
      fetchDeviceData(selectedDevice);
    }
  };

  useEffect(() => {
    const timer = setInterval(() => refreshData(), 60000);
    return () => {
      clearInterval(timer);
    };
    // eslint-disable-next-line
  }, [analog, sensor, deviceValue, sensorData, selectedDevice]);

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
      setSelectedDevice(undefined);
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
            {deviceValue.h && (
              <FormHelperText>{deviceValue.h}</FormHelperText>
            )}
          </DialogContent>
          <DialogActions>
            <Button
              startIcon={<CancelIcon />}
              variant="outlined"
              onClick={() => setDeviceValue(undefined)}
              color="secondary"
            >
              Cancel
            </Button>
            <Button
              startIcon={<SendIcon />}
              variant="outlined"
              type="submit"
              onClick={() => sendDeviceValue()}
              color="warning"
            >
              Send
            </Button>
          </DialogActions>
        </Dialog>
      );
    }
  };

  const addAnalogSensor = () => {
    setAnalog({ i: 0, n: '', u: 0, v: 0, o: 0, t: 0, f: 1 });
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
            <Button
              startIcon={<CancelIcon />}
              variant="outlined"
              onClick={() => setSensor(undefined)}
              color="secondary"
            >
              Cancel
            </Button>
            <Button
              startIcon={<SaveIcon />}
              variant="outlined"
              type="submit"
              onClick={() => sendSensor()}
              color="warning"
            >
              Save
            </Button>
          </DialogActions>
        </Dialog>
      );
    }
  };

  const renderDeviceDialog = () => {
    if (data && data.devices.length > 0 && deviceDialog !== -1) {
      return (
        <Dialog open={deviceDialog !== -1} onClose={() => setDeviceDialog(-1)}>
          <DialogTitle>Device Details</DialogTitle>
          <DialogContent dividers>
            <List dense={true}>
              <ListItem>
                <ListItemText primary="Type" secondary={data.devices[deviceDialog].t} />
              </ListItem>
              <ListItem>
                <ListItemText primary="Name" secondary={data.devices[deviceDialog].n} />
              </ListItem>
              <ListItem>
                <ListItemText primary="Brand" secondary={data.devices[deviceDialog].b} />
              </ListItem>
              <ListItem>
                <ListItemText
                  primary="Device ID"
                  secondary={'0x' + ('00' + data.devices[deviceDialog].d.toString(16).toUpperCase()).slice(-2)}
                />
              </ListItem>
              <ListItem>
                <ListItemText primary="Product ID" secondary={data.devices[deviceDialog].p} />
              </ListItem>
              <ListItem>
                <ListItemText primary="Version" secondary={data.devices[deviceDialog].v} />
              </ListItem>
            </List>
          </DialogContent>
          <DialogActions>
            <Button variant="outlined" onClick={() => setDeviceDialog(-1)} color="secondary">
              Close
            </Button>
          </DialogActions>
        </Dialog>
      );
    }
  };

  const toggleDeviceData = (index: number) => {
    if (selectedDevice === index) {
      setSelectedDevice(undefined);
    } else {
      fetchDeviceData(index);
    }
  };

  const toggleSensorData = () => {
    if (sensorData) {
      setSensorData(undefined);
    } else {
      fetchSensorData();
    }
  };

  const renderCoreData = () => {
    if (!data) {
      return <FormLoader onRetry={loadData} errorMessage={errorMessage} />;
    }

    return (
      <IconContext.Provider value={{ color: 'lightblue', size: '24', style: { verticalAlign: 'middle' } }}>
        {data.devices.length === 0 && <MessageBox my={2} level="warning" message="Scanning for EMS devices..." />}
        <Table size="small">
          <TableHead>
            <TableRow>
              <StyledTableCell padding="checkbox" align="left" colSpan={2}>
                TYPE
              </StyledTableCell>
              {desktopWindow && <StyledTableCell>DESCRIPTION</StyledTableCell>}
              <StyledTableCell align="center">ENTITIES</StyledTableCell>
              <StyledTableCell />
            </TableRow>
          </TableHead>
          <TableBody>
            {data.devices.map((device, index) => (
              <TableRow
                hover
                selected={device.i === selectedDevice}
                key={index}
                onClick={() => device.e && toggleDeviceData(device.i)}
              >
                <TableCell padding="checkbox">
                  <DeviceIcon type={device.t} />
                </TableCell>
                <TableCell>{device.t}</TableCell>
                {desktopWindow && <TableCell>{device.n}</TableCell>}
                <TableCell align="center">{device.e}</TableCell>
                <TableCell align="right">
                  <IconButton size="small" onClick={() => setDeviceDialog(index)}>
                    <InfoOutlinedIcon color="info" fontSize="small" sx={{ verticalAlign: 'middle' }} />
                  </IconButton>
                </TableCell>
              </TableRow>
            ))}
            {(data.active_sensors > 0 || data.analog_enabled) && (
              <TableRow hover selected={sensorData !== undefined} onClick={() => toggleSensorData()}>
                <TableCell>
                  <DeviceIcon type="Sensor" />
                </TableCell>
                <TableCell>Sensors</TableCell>
                {desktopWindow && <TableCell>Attached EMS-ESP Sensors</TableCell>}
                <TableCell align="center">{data.active_sensors}</TableCell>
                <TableCell align="right">
                  <IconButton size="small" onClick={() => addAnalogSensor()} disabled={!data.analog_enabled}>
                    <AddCircleOutlineOutlinedIcon fontSize="small" sx={{ verticalAlign: 'middle' }} />
                  </IconButton>
                </TableCell>
              </TableRow>
            )}
          </TableBody>
        </Table>
      </IconContext.Provider>
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
            command:&nbsp;{dv.n}
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
              <StyledTableCell align="left">ENTITY NAME/COMMAND</StyledTableCell>
              <StyledTableCell align="right">VALUE</StyledTableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            {deviceData.data.map((dv, i) => (
              <StyledTableRow key={i} onClick={() => sendCommand(dv)}>
                <StyledTableCell padding="checkbox">
                  {dv.c && me.admin && (
                    <IconButton size="small" aria-label="Edit">
                      <EditIcon color="primary" fontSize="small" />
                    </IconButton>
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
            <StyledTableCell align="left">NAME</StyledTableCell>
            <StyledTableCell align="right">TEMPERATURE</StyledTableCell>
          </TableRow>
        </TableHead>
        <TableBody>
          {sensorData?.sensors.map((sensor_data) => (
            <StyledTableRow key={sensor_data.n} onClick={() => updateSensor(sensor_data)}>
              <StyledTableCell padding="checkbox">
                {me.admin && (
                  <IconButton edge="start" size="small" aria-label="Edit">
                    <EditIcon color="primary" fontSize="small" />
                  </IconButton>
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
        <TableHead>
          <TableRow>
            <StyledTableCell padding="checkbox" style={{ width: 18 }}></StyledTableCell>
            <StyledTableCell>GPIO</StyledTableCell>
            <StyledTableCell>NAME</StyledTableCell>
            <StyledTableCell>TYPE</StyledTableCell>
            <StyledTableCell align="right">VALUE</StyledTableCell>
          </TableRow>
        </TableHead>
        <TableBody>
          {sensorData?.analogs.map((analog_data) => (
            <StyledTableRow key={analog_data.i} onClick={() => updateAnalog(analog_data)}>
              <StyledTableCell padding="checkbox">
                {me.admin && (
                  <IconButton edge="start" size="small" aria-label="Edit">
                    <EditIcon color="primary" fontSize="small" />
                  </IconButton>
                )}
              </StyledTableCell>
              <StyledTableCell component="th" scope="row">
                {analog_data.i}
              </StyledTableCell>
              <StyledTableCell>{analog_data.n}</StyledTableCell>
              <StyledTableCell>{AnalogTypes[analog_data.t]}</StyledTableCell>
              <StyledTableCell align="right">{formatValue(analog_data.v, analog_data.u)}</StyledTableCell>
            </StyledTableRow>
          ))}
        </TableBody>
      </Table>
    </>
  );

  const sendRemoveAnalog = async () => {
    if (analog) {
      try {
        const response = await EMSESP.writeAnalog({
          id: analog.i,
          name: analog.n,
          offset: analog.o,
          factor: analog.f,
          uom: analog.u,
          type: -1
        });

        if (response.status === 204) {
          enqueueSnackbar('Analog deletion failed', { variant: 'error' });
        } else if (response.status === 403) {
          enqueueSnackbar('Access denied', { variant: 'error' });
        } else {
          enqueueSnackbar('Analog sensor removed', { variant: 'success' });
        }
      } catch (error: any) {
        enqueueSnackbar(extractErrorMessage(error, 'Problem updating analog sensor'), { variant: 'error' });
      } finally {
        setAnalog(undefined);
        fetchSensorData();
      }
    }
  };

  const sendAnalog = async () => {
    if (analog) {
      try {
        const response = await EMSESP.writeAnalog({
          id: analog.i,
          name: analog.n,
          offset: analog.o,
          factor: analog.f,
          uom: analog.u,
          type: analog.t
        });

        if (response.status === 204) {
          enqueueSnackbar('Analog sensor update failed', { variant: 'error' });
        } else if (response.status === 403) {
          enqueueSnackbar('Access denied', { variant: 'error' });
        } else {
          enqueueSnackbar('Analog sensor updated', { variant: 'success' });
        }
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
            <Grid container spacing={2}>
              <Grid item>
                <ValidatedTextField
                  name="i"
                  label="GPIO"
                  value={numberValue(analog.i)}
                  type="number"
                  variant="outlined"
                  autoFocus
                  onChange={updateValue(setAnalog)}
                />
              </Grid>
              <Grid item>
                <ValidatedTextField
                  name="n"
                  label="Name"
                  value={analog.n}
                  sx={{ width: '20ch' }}
                  variant="outlined"
                  onChange={updateValue(setAnalog)}
                />
              </Grid>
              <Grid item>
                <ValidatedTextField name="t" label="Type" value={analog.t} select onChange={updateValue(setAnalog)}>
                  {AnalogTypes.map((val, i) => (
                    <MenuItem key={i} value={i}>
                      {val}
                    </MenuItem>
                  ))}
                </ValidatedTextField>
              </Grid>
              {analog.t === 3 && (
                <>
                  <Grid item>
                    <ValidatedTextField name="u" label="UoM" value={analog.u} select onChange={updateValue(setAnalog)}>
                      {DeviceValueUOM_s.map((val, i) => (
                        <MenuItem key={i} value={i}>
                          {val}
                        </MenuItem>
                      ))}
                    </ValidatedTextField>
                  </Grid>
                  <Grid item></Grid>
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
                </>
              )}
            </Grid>
            <Box color="warning.main" mt={2}>
              <Typography variant="body2">Warning: be careful when assigning a GPIO!</Typography>
            </Box>
          </DialogContent>
          <DialogActions>
            <Box flexGrow={1} sx={{ '& button': { mt: 0 } }}>
              <Button startIcon={<RemoveIcon />} variant="outlined" color="error" onClick={() => sendRemoveAnalog()}>
                Remove
              </Button>
            </Box>
            <Button
              startIcon={<CancelIcon />}
              variant="outlined"
              onClick={() => setAnalog(undefined)}
              color="secondary"
            >
              Cancel
            </Button>
            <Button
              startIcon={<SaveIcon />}
              variant="outlined"
              type="submit"
              onClick={() => sendAnalog()}
              color="warning"
            >
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
        {renderDeviceDialog()}
        {sensorData && sensorData.sensors.length > 0 && renderDallasData()}
        {sensorData && sensorData.analogs.length > 0 && renderAnalogData()}
        {renderDeviceValueDialog()}
        {renderSensorDialog()}
        {renderAnalogDialog()}
        <ButtonRow>
          <Button startIcon={<RefreshIcon />} variant="outlined" color="secondary" onClick={refreshData}>
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
