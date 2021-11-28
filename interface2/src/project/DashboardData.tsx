import { FC, Fragment, useState, useContext } from 'react';

import {
  Button,
  Table,
  TableContainer,
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
  InputAdornment
} from '@mui/material';

import Tooltip, { TooltipProps, tooltipClasses } from '@mui/material/Tooltip';
import TableCell, { tableCellClasses } from '@mui/material/TableCell';

import { styled } from '@mui/material/styles';

import parseMilliseconds from 'parse-ms';

import { useSnackbar } from 'notistack';

import RefreshIcon from '@mui/icons-material/Refresh';
import ListIcon from '@mui/icons-material/List';
import IconButton from '@mui/material/IconButton';
import EditIcon from '@mui/icons-material/Edit';

import { AuthenticatedContext } from '../contexts/authentication';

import { ButtonRow, FormLoader, ValidatedTextField, SectionContent } from '../components';

import * as EMSESP from './api';

import { updateValue, extractErrorMessage, useRest } from '../utils';

import { Data, DeviceData, Device, DeviceValue, DeviceValueUOM, DeviceValueUOM_s, Sensor } from './types';

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
  },
  '&.Mui-selected, &.Mui-selected:hover': {
    backgroundColor: 'purple'
  }
}));

const StyledTooltip = styled(({ className, ...props }: TooltipProps) => (
  <Tooltip {...props} classes={{ popper: className }} />
))(({ theme }) => ({
  [`& .${tooltipClasses.tooltip}`]: {
    backgroundColor: theme.palette.secondary.dark,
    color: 'white',
    fontSize: 11,
    border: '1px solid #dadde9'
  }
}));

const HtmlTooltip = styled(({ className, ...props }: TooltipProps) => (
  <Tooltip {...props} classes={{ popper: className }} />
))(({ theme }) => ({
  [`& .${tooltipClasses.tooltip}`]: {
    backgroundColor: theme.palette.secondary.dark,
    color: 'white',
    maxWidth: 220,
    fontSize: theme.typography.pxToRem(12),
    border: '1px solid #dadde9'
  }
}));

const DashboardData: FC = () => {
  const { loadData, data, errorMessage } = useRest<Data>({ read: EMSESP.readData });

  const { me } = useContext(AuthenticatedContext);

  const { enqueueSnackbar } = useSnackbar();

  const [deviceData, setDeviceData] = useState<DeviceData>();
  const [deviceValue, setDeviceValue] = useState<DeviceValue>();
  const [sensor, setSensor] = useState<Sensor>();

  const fetchDeviceData = async (device: number) => {
    try {
      setDeviceData((await EMSESP.readDeviceData({ id: device })).data);
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
        // always show with one decimal place
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
          id: deviceData?.id,
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
              Change
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
          no: sensor.n,
          id: sensor.i,
          temp: sensor.t,
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
              <Typography variant="body2">Changing Sensor #{sensor.n}</Typography>
            </Box>
            <ValidatedTextField
              name="i"
              label="ID / Name"
              value={sensor.i}
              autoFocus
              sx={{ width: '30ch' }}
              onChange={updateValue(setSensor)}
            />
            <ValidatedTextField
              name="o"
              label="Offset"
              value={sensor.o}
              sx={{ width: '12ch' }}
              type="number"
              variant="outlined"
              onChange={updateValue(setSensor)}
              inputProps={{ min: '-5', max: '5', step: '0.1' }}
              InputProps={{
                startAdornment: <InputAdornment position="start">°C</InputAdornment>
              }}
            />
          </DialogContent>
          <DialogActions>
            <Button variant="outlined" onClick={() => setSensor(undefined)} color="secondary">
              Cancel
            </Button>
            <Button variant="outlined" type="submit" onClick={() => sendSensor()} color="warning">
              Change
            </Button>
          </DialogActions>
        </Dialog>
      );
    }
  };

  const renderData = () => {
    if (!data) {
      return <FormLoader loadData={loadData} errorMessage={errorMessage} />;
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
      <TableContainer>
        <Typography sx={{ pt: 2 }} variant="h6" color="primary">
          EMS Devices
        </Typography>
        {!noDevices() && (
          <Table size="small">
            <TableBody>
              {data.devices.sort(compareDevices).map((device) => (
                <TableRow hover key={device.i} onClick={() => fetchDeviceData(device.i)}>
                  <TableCell>
                    <HtmlTooltip
                      title={
                        <Fragment>
                          <Typography color="inherit">
                            <b>Device Details</b>
                          </Typography>
                          {'DeviceID: 0x' + ('00' + device.d.toString(16).toUpperCase()).slice(-2)}
                          <br />
                          {'ProductID: ' + device.p}
                          <br />
                          {'Version: ' + device.v}
                        </Fragment>
                      }
                      placement="left-end"
                    >
                      <Button startIcon={<ListIcon />} size="small" variant="outlined" color="inherit">
                        {device.t}
                      </Button>
                    </HtmlTooltip>
                  </TableCell>
                  <TableCell align="right">{device.b + ' ' + device.n} </TableCell>
                </TableRow>
              ))}
            </TableBody>
          </Table>
        )}
        {noDevices() && (
          <Box bgcolor="error.main" color="error.contrastText" p={2} mt={2} mb={2}>
            <Typography variant="body1">
              No EMS devices found. Check the connections and for possible Tx errors.
            </Typography>
          </Box>
        )}
      </TableContainer>
    );
  };

  const renderDeviceData = () => {
    if (data?.devices.length === 0 || !deviceData) {
      return;
    }

    const noDeviceData = () => {
      return (deviceData?.data || []).length === 0;
    };

    const sendCommand = (dv: DeviceValue) => {
      if (dv.c && me.admin) {
        setDeviceValue(dv);
      }
    };

    if (noDeviceData()) {
      return (
        <Fragment>
          <Typography sx={{ pt: 2 }} variant="h6" color="primary">
            {deviceData.type}&nbsp;Data{' '}
          </Typography>
          <Box color="warning.main" p={0} mt={0} mb={0}>
            <Typography variant="body1">
              <i>no data available for this device</i>
            </Typography>
          </Box>
        </Fragment>
      );
    }

    return (
      <TableContainer>
        <Typography sx={{ pt: 2 }} variant="h6" color="primary">
          {deviceData.type}&nbsp;Data{' '}
        </Typography>

        <Table size="small" padding="normal">
          <TableHead>
            <TableRow>
              <StyledTableCell padding="checkbox" style={{ width: 18 }}></StyledTableCell>
              <StyledTableCell align="left"> Name</StyledTableCell>
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
      </TableContainer>
    );
  };

  const updateSensor = (sensordata: Sensor) => {
    if (sensordata && me.admin) {
      setSensor(sensordata);
    }
  };

  const renderSensorData = () => (
    <>
      <Typography sx={{ pt: 2 }} variant="h6" color="primary">
        Dallas Sensors
      </Typography>
      <TableContainer>
        <Table size="small" padding="normal">
          <TableHead>
            <TableRow>
              <StyledTableCell padding="checkbox" style={{ width: 18 }}></StyledTableCell>
              <StyledTableCell>Dallas Sensor #</StyledTableCell>
              <StyledTableCell align="left">ID / Name</StyledTableCell>
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
                <StyledTableCell align="left">{sensorData.i}</StyledTableCell>
                <StyledTableCell align="right">{formatValue(sensorData.t, DeviceValueUOM.DEGREES)}</StyledTableCell>
              </StyledTableRow>
            ))}
          </TableBody>
        </Table>
      </TableContainer>
    </>
  );

  const renderAnalogData = () => (
    <>
      <Typography sx={{ pt: 2 }} variant="h6" color="primary">
        Analog Sensors
      </Typography>
      <TableContainer>
        <Table size="small" padding="normal">
          <TableHead>
            <TableRow>
              <StyledTableCell padding="normal" style={{ width: 18 }}></StyledTableCell>
              <StyledTableCell>Sensor Type</StyledTableCell>
              <StyledTableCell align="right">Value</StyledTableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            <StyledTableRow>
              <StyledTableCell padding="normal">&nbsp;&nbsp;</StyledTableCell>
              <StyledTableCell component="th" scope="row">
                Analog Input
              </StyledTableCell>
              <StyledTableCell align="right">{formatValue(data?.analog, DeviceValueUOM.MV)}</StyledTableCell>
            </StyledTableRow>
          </TableBody>
        </Table>
      </TableContainer>
    </>
  );

  return (
    <SectionContent title="Device and Sensor Data" titleGutter>
      {renderData()}
      {renderDeviceData()}
      {data?.sensors.length !== 0 && renderSensorData()}
      {data?.analog && renderAnalogData()}
      {renderDeviceValueDialog()}
      {renderSensorDialog()}
      <ButtonRow>
        <Button startIcon={<RefreshIcon />} variant="outlined" color="secondary" onClick={loadData}>
          Refresh
        </Button>
      </ButtonRow>
    </SectionContent>
  );
};

export default DashboardData;
